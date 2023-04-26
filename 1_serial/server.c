#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{
	int r, sockfd, clientfd;

	struct addrinfo hints, *server;
	struct sockaddr client_address;
	socklen_t client_len;

	memset(&hints, 0, sizeof(struct addrinfo)); /* use memset_s() */
	hints.ai_family = AF_INET;					/* IPv4 connection */
	hints.ai_socktype = SOCK_STREAM;			/* TCP, streaming */

	const char *acknowledgement = "Hello, client!\nYou sent me: ";
	const int buffer_size = 1024;
	char buffer[buffer_size];


	// configure address of this server
	r = getaddrinfo("127.0.0.1", "8888", &hints, &server);
	if (r != 0)
	{
		perror("Failed to configure server address\n\n");
		exit(1);
	}

	// create socket
	sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
	if (sockfd == -1)
	{
		perror("Failed to create socket\n\n");
		exit(1);
	}

	// bind server to the socket
	r = bind(sockfd, server->ai_addr, server->ai_addrlen);
	if (r == -1)
	{
		perror("Failed to bind server to the socket\n\n");
		exit(1);
	}

	// listen for incoming connections
	r = listen(sockfd, 1);
	if (r == -1)
	{
		perror("Failed to call listen() function");
		exit(1);
	}

	// accept a new connection
	client_len = sizeof(client_address);
	clientfd = accept(sockfd, &client_address, &client_len);
	if (clientfd == -1)
	{
		perror("Failed to accept connecion request");
		exit(1);
	}

	r = recv(clientfd, buffer, buffer_size, 0);
	if (r > 0)
	{
		buffer[r] = '\0'; /* terminate the incoming string */
		printf("%s", buffer);
		send(clientfd, acknowledgement, strlen(acknowledgement), 0);
		send(clientfd, buffer, strlen(buffer), 0);
	}

	// close
	freeaddrinfo(server);
	close(clientfd);
	close(sockfd);
	return (0);
}
