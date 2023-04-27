#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int getuserinput(char buffer[BUFSIZ]); // function prototype

int main()
{
    int r, sockfd;
    char send_buffer[BUFSIZ];
    char recv_buffer[BUFSIZ];

    // configure remote address
    struct addrinfo hints, *server;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_DGRAM; // UDP, datagram

    r = getaddrinfo("127.0.0.1", "8071", &hints, &server);

    if (r != 0)
    {
        perror("Failed to configure server address details\n\n");
        exit(1);
    }

    // create socket to communicate with server
    sockfd = socket(server->ai_family,
                    server->ai_socktype,
                    server->ai_protocol);
    if (sockfd == -1)
    {
        perror("Failed to create client socket.\n\n");
        exit(1);
    }

    // get user input
    r = getuserinput(send_buffer);
    if (r == -1)
    {
        perror("Failed to get user input\n\n");
        exit(1);
    }

    // send student details
    r = sendto(sockfd,
               send_buffer,
               strlen(send_buffer),
               0,
               server->ai_addr,
               server->ai_addrlen);
    if (r == 1)
    {
        perror("Failed to send mesage\n\n");
        exit(1);
    }
    else
    {
        puts("*************************************");
        printf("User details sent to the Server\n");
    }

    // receive acknowledgement of receipt by the server
    r = recvfrom(sockfd, recv_buffer, BUFSIZ, 0, server->ai_addr, &server->ai_addrlen);
    if (r == -1)
    {
        perror("Failed! Received 0 bytes of data\n\n");
        exit(1);
    }
    else
    {
        recv_buffer[r] = '\0';
        printf("Received %d bytes of data from the Server: %s\n", r, recv_buffer);
    }

    // free allocated memory
    freeaddrinfo(server);

    // close the socket
    close(sockfd);

    putchar('\n');
    return 0;
}

int getuserinput(char buffer[BUFSIZ])
{
    char serial[20];
    char regno[20];
    char fname[15];
    char lname[15];
    char confirm;
    int trial = 1;

    while (trial <= 3)
    {
        puts("*************************************");
        puts("Your serial number?");
        scanf("%s", serial);
        puts("Your registration number?");
        scanf("%s", regno);
        puts("Your first name? ");
        scanf("%s", fname);
        puts("Your last name?");
        scanf("%s", lname);

        putchar('\n');
        printf("You are %s %s of registration %s. Your serial number is %s\n", fname, lname, regno, serial);
        puts("Is this correct? (type 'y' or 'n'):");
        scanf(" %c", &confirm);

        if (confirm == 'y')
        {
            strcpy(buffer, serial);
            strcat(buffer, "@@@"); // separator indicator
            strcat(buffer, regno);
            strcat(buffer, "@@@"); // separator indicator
            strcat(buffer, fname);
            strcat(buffer, "@@@"); // separator indicator
            strcat(buffer, lname);
            strcat(buffer, "$$$"); // finish with the terminator indicator

            putchar('\n');
            return 0;
        }
        else if (confirm == 'n')
        {
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached! Exiting program...");
            }
            putchar('\n');
            trial++;
        }
        else
        {
            puts("Invalid input (Allowed values are 'y' or 'n')");
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached! Exiting program...");
            }
            putchar('\n');
            trial++;
        }
    }

    putchar('\n');
    return -1;
}
