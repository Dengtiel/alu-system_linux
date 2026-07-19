#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFSIZE 4096

/**
 * create_server - creates, binds and listens on a TCP socket
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
 * parse_request_line - parses and prints method, path, version
 * @line: first line of HTTP request
 */
static void parse_request_line(char *line)
{
	char *method, *path, *version;

	method = strtok(line, " ");
	path = strtok(NULL, " ");
	version = strtok(NULL, "\r\n");
	if (method && path && version)
	{
		printf("Method: %s\n", method);
		printf("Path: %s\n", path);
		printf("Version: %s\n", version);
	}
}

/**
 * handle_client - handles one client connection
 * @client_fd: client socket fd
 * @client_addr: client address
 */
static void handle_client(int client_fd, struct sockaddr_in *client_addr)
{
	char buf[BUFSIZE];
	char tmp[BUFSIZE];
	char *line;
	ssize_t n;

	printf("Client connected: %s\n", inet_ntoa(client_addr->sin_addr));
	memset(buf, 0, BUFSIZE);
	n = recv(client_fd, buf, BUFSIZE - 1, 0);
	if (n <= 0)
		return;
	printf("Raw request: \"%s\"\n", buf);
	strncpy(tmp, buf, BUFSIZE - 1);
	line = strtok(tmp, "\n");
	if (line)
		parse_request_line(line);
	send(client_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);
}

/**
 * main - listens on port 8080, handles connections
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;

	server_fd = create_server(PORT);
	if (server_fd == -1)
		return (EXIT_FAILURE);
	printf("Server listening on port %d\n", PORT);
	fflush(stdout);
	while (1)
	{
		client_len = sizeof(client_addr);
		client_fd = accept(server_fd,
			(struct sockaddr *)&client_addr, &client_len);
		if (client_fd == -1)
			continue;
		handle_client(client_fd, &client_addr);
		close(client_fd);
	}
	close(server_fd);
	return (EXIT_SUCCESS);
}
