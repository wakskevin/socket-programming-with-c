#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int savetofile(char buffer[BUFSIZ]); // function prototype

int main()
{
    int r, sockfd;
    char recv_buffer[BUFSIZ];
    char duplicate_buffer[BUFSIZ];
    pid_t pid;

    // configure host address
    struct addrinfo hints, *host;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;      // IPv4 connection
    hints.ai_socktype = SOCK_DGRAM; // UDP, datagram
    hints.ai_flags = AI_PASSIVE;    // accept any connection

    r = getaddrinfo("127.0.0.1", "8701", &hints, &host);
    if (r != 0)
    {
        perror("Failed to configure host address details\n\n");
        exit(1);
    }

    // create socket to communicate with host
    sockfd = socket(host->ai_family,
                    host->ai_socktype,
                    host->ai_protocol);
    if (sockfd == -1)
    {
        perror("Failed to create client socket.\n\n");
        exit(1);
    }

    // bind host to socket
    r = bind(sockfd,
             host->ai_addr,
             host->ai_addrlen);
    if (r == -1)
    {
        perror("Failed to bind host to socket\n\n");
    }
    else
    {
        puts("UDP server is waiting...");
    }

    // no need to call listen() or accept() for a UDP server

    struct sockaddr client_address;
    socklen_t clientaddr_len = sizeof(client_address);

    // receive student details
    r = recvfrom(sockfd,
                 recv_buffer,
                 BUFSIZ,
                 0,
                 &client_address,
                 &clientaddr_len);
    if (r == -1)
    {
        perror("Failed! Received 0 bytes of data\n\n");
        exit(1);
    }
    else
    {
        recv_buffer[r] = '\0'; // terminate the incoming string
        strcpy(duplicate_buffer, recv_buffer);
        printf("Received %d bytes of data from the Client\n", r);
    }

    while (1)
    { // fork a new process to handle the client request
        pid = fork();
        if (pid == -1)
        {
            perror("Failed to fork new process\n\n");
            exit(1);
        }
        else if (pid == 0) // child process
        {
            memset(&recv_buffer, 0, BUFSIZ);
            // save received user input to file
            r = savetofile(duplicate_buffer);
            if (r == -1)
            {
                perror("Failed to save received user data to file\n\n");
                exit(1);
            }
            else
            {
                puts("Record has been added");
            }

            // free allocated memory
            freeaddrinfo(host);
            close(sockfd);
            putchar('\n');
            return 0;
        }
        else
        {
            // This is the parent process
            memset(&duplicate_buffer, 0, BUFSIZ);
        }
    }

    // free allocated memory
    freeaddrinfo(host);

    // close the socket
    close(sockfd);

    putchar('\n');
    return 0;
}

int savetofile(char buffer[BUFSIZ])
{
    char details[4][20];
    int j = 0, k = 0;

    enum user_details
    {
        SERIAL,
        REGNO,
        FNAME,
        LNAME
    };

    FILE *fh;

    // Open the file
    fh = fopen("student_details.csv", "a");
    if (fh == NULL)
    {
        puts("Could not create file details.csv");
        return (-1);
    }

    // format the received string removing the '@@@' separators
    // and stopping before the '$$$' terminator symbol
    for (int i = 0; i < (strlen(buffer) - 1); i++)
    {
        if (buffer[i] == '@' && buffer[i + 1] == '@' && buffer[i + 2] == '@')
        {
            i = i + 2;
            details[j][k] = '\0';
            j++;
            k = 0;
        }
        else if (buffer[i] == '$' && buffer[i + 1] == '$' && buffer[i + 2] == '$')
        {
            break;
        }
        else
        {
            details[j][k] = buffer[i];
            k++;
        }
    }
    printf("%s,%s,%s %s\n\n", details[SERIAL], details[REGNO], details[FNAME], details[LNAME]);

    // writing to the file
    switch (ftell(fh))
    {
        // start writing at the begining of the file if empty
    case 0:
        fprintf(fh, "%s,%s,%s %s", details[SERIAL], details[REGNO], details[FNAME], details[LNAME]);
        break;
        // if not empty go to the next line and start writing from there
    default:
        fprintf(fh, "\n%s,%s,%s %s", details[SERIAL], details[REGNO], details[FNAME], details[LNAME]);
        break;
    }

    // Close the file
    fclose(fh);

    putchar('\n');
    return (0);
}
