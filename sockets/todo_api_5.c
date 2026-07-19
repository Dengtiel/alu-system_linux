#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT      8080
#define BUFSIZE   8192
#define MAX_TODOS 1024

/**
 * struct todo_s - todo item
 * @id: unique id
 * @title: title
 * @description: description
 */
typedef struct todo_s
{
	int  id;
	char title[256];
	char description[256];
} todo_t;

static todo_t todos[MAX_TODOS];
static int    todo_count;

/**
 * create_server - creates binds listens on TCP socket
 * @port: port number
 *
 * Return: server fd or -1
 */
static int create_server(int port)
{
	int fd, opt;
	struct sockaddr_in addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		return (-1);
	opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(fd);
		return (-1);
	}
	if (listen(fd, 10) == -1)
	{
		close(fd);
		return (-1);
	}
	return (fd);
}

/**
 * send_response - sends HTTP response
 * @fd: client fd
 * @status: HTTP status
 * @body: response body
 */
static void send_response(int fd, char const *status, char const *body)
{
	char response[BUFSIZE];
	int  len;

	if (body && *body)
		len = snprintf(response, BUFSIZE,
			"HTTP/1.1 %s\r\nContent-Length: %lu\r\n"
			"Content-Type: application/json\r\n\r\n%s",
			status, strlen(body), body);
	else
		len = snprintf(response, BUFSIZE,
			"HTTP/1.1 %s\r\n\r\n", status);
	send(fd, response, len, 0);
}

/**
 * handle_post_todos - handles POST /todos
 * @client_fd: client socket
 * @client_ip: client IP
 * @buf: request buffer
 */
static void handle_post_todos(int client_fd, char *client_ip, char *buf)
{
	char *body, *cl, *key, *val, *save, *pair;
	char title[256], description[256], json[BUFSIZE];

	cl = strstr(buf, "Content-Length:");
	if (!cl)
	{
		printf("%s POST /todos -> 411 Length Required\n", client_ip);
		send_response(client_fd, "411 Length Required", NULL);
		return;
	}
	body = strstr(buf, "\r\n\r\n");
	if (!body)
	{
		send_response(client_fd, "422 Unprocessable Entity", NULL);
		return;
	}
	body += 4;
	title[0] = '\0';
	description[0] = '\0';
	pair = strtok_r(body, "&", &save);
	while (pair)
	{
		key = strtok(pair, "=");
		val = strtok(NULL, "=");
		if (key && val)
		{
			if (strcmp(key, "title") == 0)
				strncpy(title, val, 255);
			else if (strcmp(key, "description") == 0)
				strncpy(description, val, 255);
		}
		pair = strtok_r(NULL, "&", &save);
	}
	if (!title[0] || !description[0])
	{
		printf("%s POST /todos -> 422 Unprocessable Entity\n",
			client_ip);
		send_response(client_fd, "422 Unprocessable Entity", NULL);
		return;
	}
	todos[todo_count].id = todo_count;
	strncpy(todos[todo_count].title, title, 255);
	strncpy(todos[todo_count].description, description, 255);
	snprintf(json, BUFSIZE,
		"{\"id\":%d,\"title\":\"%s\",\"description\":\"%s\"}",
		todo_count, title, description);
	todo_count++;
	printf("%s POST /todos -> 201 Created\n", client_ip);
	send_response(client_fd, "201 Created", json);
}

/**
 * handle_get_todos - handles GET /todos
 * @client_fd: client socket
 * @client_ip: client IP
 */
static void handle_get_todos(int client_fd, char *client_ip)
{
	char json[BUFSIZE];
	char item[512];
	int  i;

	strcpy(json, "[");
	for (i = 0; i < todo_count; i++)
	{
		snprintf(item, 512,
			"%s{\"id\":%d,\"title\":\"%s\","
			"\"description\":\"%s\"}",
			i > 0 ? "," : "",
			todos[i].id, todos[i].title, todos[i].description);
		strncat(json, item, BUFSIZE - strlen(json) - 1);
	}
	strncat(json, "]", BUFSIZE - strlen(json) - 1);
	printf("%s GET /todos -> 200 OK\n", client_ip);
	send_response(client_fd, "200 OK", json);
}

/**
 * main - listens on port 8080 handles connections
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
	char buf[BUFSIZE], tmp[BUFSIZE];
	char *method, *path, *client_ip;

	server_fd = create_server(PORT);
	if (server_fd == -1)
		return (EXIT_FAILURE);
	printf("Server listening on port %d\n", PORT);
	fflush(stdout);
	todo_count = 0;
	while (1)
	{
		client_len = sizeof(client_addr);
		client_fd = accept(server_fd,
			(struct sockaddr *)&client_addr, &client_len);
		if (client_fd == -1)
			continue;
		client_ip = inet_ntoa(client_addr.sin_addr);
		memset(buf, 0, BUFSIZE);
		recv(client_fd, buf, BUFSIZE - 1, 0);
		strncpy(tmp, buf, BUFSIZE - 1);
		method = strtok(tmp, " ");
		path   = strtok(NULL, " ");
		if (method && path && strcmp(path, "/todos") == 0)
		{
			if (strcmp(method, "POST") == 0)
				handle_post_todos(client_fd, client_ip, buf);
			else if (strcmp(method, "GET") == 0)
				handle_get_todos(client_fd, client_ip);
			else
				send_response(client_fd, "404 Not Found",
					NULL);
		}
		else
		{
			printf("%s %s %s -> 404 Not Found\n",
				client_ip,
				method ? method : "?",
				path ? path : "?");
			send_response(client_fd, "404 Not Found", NULL);
		}
		close(client_fd);
	}
	close(server_fd);
	return (EXIT_SUCCESS);
}
