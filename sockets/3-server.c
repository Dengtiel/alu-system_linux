#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define BUFSIZE 1024

/**
 * create_server - creates, binds and listens on a TCP socket
 * @port: port number to listen on
 *
 * Return: server file descriptor or -1 on failure
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
 * main - accepts connection, receives message, prints it
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len;
	char buf[BUFSIZE];
	ssize_t n;

	server_fd = create_server(PORT);
	if (server_fd == -1)
		return (EXIT_FAILURE);
	printf("Server listening on port %d\n", PORT);
	fflush(stdout);
	client_len = sizeof(client_addr);
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
		&client_len);
	if (client_fd == -1)
	{
		close(server_fd);
		return (EXIT_FAILURE);
	}
	printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
	memset(buf, 0, BUFSIZE);
	n = recv(client_fd, buf, BUFSIZE - 1, 0);
	if (n > 0)
		printf("Message received: \"%s\"\n", buf);
	close(client_fd);
	close(server_fd);
	return (EXIT_SUCCESS);
}
