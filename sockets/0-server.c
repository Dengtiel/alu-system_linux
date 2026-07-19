#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 12345

/**
 * main - opens an IPv4/TCP socket and listens on port 12345
 *
 * Return: EXIT_SUCCESS or EXIT_FAILURE
 */
int main(void)
{
	int server_fd;
	struct sockaddr_in addr;
	int opt;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket");
		return (EXIT_FAILURE);
	}

	opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		close(server_fd);
		return (EXIT_FAILURE);
	}

	if (listen(server_fd, 10) == -1)
	{
		perror("listen");
		close(server_fd);
		return (EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);
	fflush(stdout);

	/* Hang indefinitely */
	while (1)
		pause();

	close(server_fd);
	return (EXIT_SUCCESS);
}
