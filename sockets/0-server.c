#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 12345

/**
 * create_socket - creates and configures a TCP socket
 *
 * Return: socket file descriptor or -1 on failure
 */
static int create_socket(void)
{
	int server_fd;
	int opt;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket");
		return (-1);
	}
	opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	return (server_fd);
}

/**
 * bind_and_listen - binds socket to port and starts listening
 * @server_fd: socket file descriptor
 *
 * Return: 0 on success, -1 on failure
 */
static int bind_and_listen(int server_fd)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		return (-1);
	}
	if (listen(server_fd, 10) == -1)
	{
		perror("listen");
		return (-1);
	}
	return (0);
}

/**
 * main - opens an IPv4/TCP socket and listens on port 12345
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	int server_fd;

	server_fd = create_socket();
	if (server_fd == -1)
		return (EXIT_FAILURE);

	if (bind_and_listen(server_fd) == -1)
	{
		close(server_fd);
		return (EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);
	fflush(stdout);

	while (1)
		pause();

	close(server_fd);
	return (EXIT_SUCCESS);
}
