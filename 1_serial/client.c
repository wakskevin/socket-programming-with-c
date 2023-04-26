// a program that requests a user to enter his/her serial no. and saves it to a file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int get_student_details();

FILE *fh;

int main()
{
    int r, sockfd, c;
    struct addrinfo hints, *server;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* IPv4 connection */
    hints.ai_socktype = SOCK_STREAM; /* TCP, streaming */

    const int buffer_size = 1024;
    char send_buffer[buffer_size];
    char recv_buffer[buffer_size];
    int buffer_len = 0;

    //  asks for user to input details and saves to file
    r = get_student_details();
    if (r == 1)
    {
        puts("Failed to add record. Exiting program...\n");
        return (1);
    }

    // configure address of the server to which the information will be sent
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

    // connect to the server
    r = connect(sockfd, server->ai_addr, server->ai_addrlen);
    if (r == -1)
    {
        perror("Failed to connect to the server\n\n");
        exit(1);
    }

    // send data to the server

    while ((c = fgetc(fh)) != EOF)
    {
        send_buffer[buffer_len++] = c;
        printf("%s", send_buffer);
    }

    r = send(sockfd, send_buffer, buffer_size, 0);
    if (r < 1)
    {
        perror("Send failed\n\n");
        exit(1);
    }

    // recv data from the server

    r = recv(sockfd, recv_buffer, buffer_size, 0);
    if (r > 0)
    {
        printf("Received %d bytes of data\n\n", r);

        for (int i = 0; i < r; i++)
        {
            putchar(recv_buffer[i]);
        }
    }

    // close
    fclose(fh);
    freeaddrinfo(server);
    close(sockfd);

    putchar('\n');
    return 0;
}

int get_student_details()
{
    int serial;
    char confirm[5];
    int trial = 1;

    while (trial <= 3)
    {
        puts("*************************************");
        printf("Your serial number? ");
        scanf("%d", &serial);

        putchar('\n');
        printf("Your serial number is %d\n", serial);
        printf("Is this correct? (type 'yes' or 'no'): ");
        scanf("%s", confirm);

        if (strcmp(confirm, "yes") == 0)
        {
            // Open the file
            fh = fopen("student_details.txt", "a");
            if (fh == NULL)
            {
                puts("Failed to create file student_details.txt");
                return (1);
            }
            else
            {
                if (ftell(fh) == 0) // start writing at the begining of the file
                {
                    fprintf(fh, "%d", serial);
                }
                else // go to the next line and start writing from there
                {
                    fprintf(fh, "\n%d", serial);
                }
                puts("Record added to file ✅");
                break;
            }
        }
        else if (strcmp(confirm, "no") == 0)
        {
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("\nTrial limit reached!");
                return 1;
            }
            putchar('\n');
            trial++;
        }
        else
        {
            puts("\nInvalid input (Allowed values are 'yes' or 'no')");
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("\nTrial limit reached!");
                return 1;
            }
            putchar('\n');
            trial++;
        }
    }

    return EXIT_SUCCESS;
}
