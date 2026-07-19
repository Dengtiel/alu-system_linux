#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

/**
 * resolve_host - resolves hostname to IP address
 * @host: hostname string
 * @addr: pointer to sockaddr_in to fill
 *
 * Return: 0 on success, -1 on failure
 */
static int resolve_host(char const *host, struct sockaddr_in *addr)
{
	struct hostent *he;

	he = gethostbyname(host);
	if (!he)
	{
		herror("gethostbyname");
		return (-1);
	}
	memcpy(&addr->sin_addr, he->h_addr_list[0], he->h_length);
	return (0);
}

/**
 * main - connects to a server and prints confirmation
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char **argv)
{
	int sock_fd;
	struct sockaddr_in addr;
	unsigned short port;

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		return (EXIT_FAILURE);
	}

	port = (unsigned short)atoi(argv[2]);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
	{
		perror("socket");
		return (EXIT_FAILURE);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (resolve_host(argv[1], &addr) == -1)
	{
		close(sock_fd);
		return (EXIT_FAILURE);
	}

	if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("connect");
		close(sock_fd);
		return (EXIT_FAILURE);
	}

	printf("Connected to %s:%u\n", argv[1], port);

	close(sock_fd);
	return (EXIT_SUCCESS);
}
