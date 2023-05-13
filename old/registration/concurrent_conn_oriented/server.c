#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>


typedef enum
{
    CANNOT_OPEN_FILE = 30,
    DUPLICATE_SERIAL = 31,
    DUPLICATE_REGNO = 32,
    DETAILS_SAVED_SUCCESSFULLY = 33
} Response;

void close_up(struct addrinfo *host, int sockfd);
Response add_student_record(char student_details[4][20]);

void main()
{
    int r, sockfd, newsockfd, j = 0, k = 0, client_id;
    char recv_buffer[BUFSIZ], send_buffer[BUFSIZ];
    char student_details[4][20];
    struct addrinfo hints, *host;
    struct sockaddr client_address;
    socklen_t clientaddr_len = sizeof client_address;
    pid_t pid;
    time_t connect_time, disconnect_time;

    putchar('\n');

    /* ***************************** CONFIGURE SERVER HOST ADDRESS DETAILS ********************************* */

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming

    r = getaddrinfo("127.0.0.1", "8082", &hints, &host);

    if (r != 0)
    {
        puts("⛔ Server Malfunction! Failed to configure host address details. Exiting program...");
        exit(EXIT_FAILURE);
    }

    puts("✅ Configured host server address details successfully!");

    /* ************************ CREATE SOCKET TO BIND TO HOST SERVER *********************** */

    sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);

    if (sockfd == -1)
    {
        puts("⛔ Server Malfunction! Failed to create host server socket. Exiting program...");
        freeaddrinfo(host); // free allocated address memory
        exit(EXIT_FAILURE);
    }

    puts("✅ Host server socket created successfully!");

    /* ************************ BIND TO SOCKET *********************** */

    r = bind(sockfd, host->ai_addr, host->ai_addrlen);

    if (r == -1)
    {
        puts("⛔ Server Malfunction! Failed to bind host to socket. Exiting server program...");
        close_up(host, sockfd); // free allocated address memory, close socket and exit
    }

    puts("✅ Socket bound to host successfully!");

    /* ************************ LISTEN TO INCOMING CONNECTIONS *********************** */

    r = listen(sockfd, 3); // number of pending connections that can wait in the queue is 3.

    if (r == -1)
    {
        puts("⛔ Server Malfunction! Failed to listen to incoming connections Exiting server program...");
        close_up(host, sockfd); // free allocated address memory, close socket and exit
    }

    puts("✅ TCP server is listening...");
    puts("\n***************************************************************");

    while (1)
    {
        /* ************************ ACCEPT CONNECTION REQUESTS *********************** */

        newsockfd = accept(sockfd, &client_address, &clientaddr_len);

        if (newsockfd == -1)
        {
            puts("⛔ Server Malfunction! Cannot accept incoming connections! Exiting server program...");
            close_up(host, sockfd); // free allocated address memory, close socket and exit
        }

        if (client_id == 3)
            client_id = 0; // reset client id number

        connect_time = time(NULL);
        printf("✅ Client %d is connected to the server at \033[34m%s\033[0m", ++client_id, ctime(&connect_time));

        /* ******************************** FORK NEW PROCESS TO HANDLE CONNECTION **************************** */

        pid = fork();

        if (pid == -1)
        {
            puts("⛔ Server Malfunction! Cannot fork new child processes! Exiting server program");
            close(newsockfd);
            close_up(host, sockfd);
        }
        else if (pid == 0) // This is the child process
        {

            // close resources which the child process won't require at all.
            freeaddrinfo(host);
            close(sockfd);

            /* ******************************** RECEIVE STUDENT DETAILS **************************** */

            r = recv(newsockfd, recv_buffer, BUFSIZ, 0);

            if (r < 1)
                printf("⛔ Received 0 bytes of data from Client %d\n", client_id);
            else
            {
                recv_buffer[r] = '\0'; // terminate the incoming string
                printf("📨 Received %d bytes of data from the Client %d\n", r, client_id);
                puts("⌛ Processing data...");

                /* ***************************** EXTRACT STUDENT DETAILS ******************************* */

                for (int i = 0; i < (strlen(recv_buffer) - 1); i++)
                {
                    // remove the @@@ separators
                    if (recv_buffer[i] == '@' && recv_buffer[i + 1] == '@' && recv_buffer[i + 2] == '@')
                    {
                        i = i + 2;
                        student_details[j][k] = '\0';
                        j++;
                        k = 0;
                    }
                    // stopping before the $$$ terminator
                    else if (recv_buffer[i] == '$' && recv_buffer[i + 1] == '$' && recv_buffer[i + 2] == '$')
                    {
                        student_details[j][k] = '\0';
                        break;
                    }
                    else
                    {
                        student_details[j][k] = recv_buffer[i];
                        k++;
                    }
                }

                /* ***************************** ADD STUDENT RECORD ******************************* */

                r = add_student_record(student_details);
            }

            /* **************************** FORMULATE RESPONSE ******************************** */

            switch (r)
            {
            case CANNOT_OPEN_FILE:
                strcpy(send_buffer, "\033[31mCould not open file student_details.txt\033[0m"); // message displayed in color red
                break;
            case DUPLICATE_SERIAL:
                strcpy(send_buffer, "\033[33mDuplicate Serial no. Failed to add record\033[0m"); // message displayed in color yellow
                break;
            case DUPLICATE_REGNO:
                strcpy(send_buffer, "\033[33mDuplicate Registration no. Failed to add record\033[0m"); // message displayed in color yellow
                break;
            case DETAILS_SAVED_SUCCESSFULLY:
                strcpy(send_buffer, "\033[32mRecord added successfully\033[0m"); // message displayed in color green
                break;
            default:
                strcpy(send_buffer, "\033[31mUnable to fork child processes or received 0 bytes of data.\033[0m"); // message displayed in color red
                break;
            }
            printf("...%s\n", send_buffer);

            /* ************************ SEND RESPONSE BACK TO CLIENT *********************** */

            r = send(newsockfd, send_buffer, strlen(send_buffer), 0);
            if (r < 1)
                puts("⛔ Server Malfunction! Failed to send response back to the client");
            else
                printf("📤 Response sent to Client %d\n", client_id);

            // close client socket
            close(newsockfd);
            disconnect_time = time(NULL);
            printf("✅ Client %d disconnected from the server at \033[34m%s\033[0m", client_id, ctime(&disconnect_time));

            puts("\n***************************************************************\n");
            exit(EXIT_SUCCESS);
        }
        else
        {
            // This is the parent process
            // Close the client socket and wait for the next connection
            close(newsockfd);
        }
    }

    /* ************************ CLOSE UP *********************** */

    freeaddrinfo(host);
    close(sockfd);
    exit(EXIT_SUCCESS);
}

void close_up(struct addrinfo *host, int sockfd)
{
    putchar('\n');
    freeaddrinfo(host); // free allocated address memory
    close(sockfd);      // close socket
    exit(EXIT_FAILURE);
}


Response add_student_record(char student_details[4][20])
{
    enum user_details
    {
        SERIAL,
        REGNO,
        FNAME,
        LNAME
    };

    FILE *fh;
    char line[100];
    int isEmptyLine;
    char *token;
    char delimiter[] = "    ";

    fh = fopen("student_details.txt", "a+");
    if (fh == NULL)
        return CANNOT_OPEN_FILE;

    isEmptyLine = 1;

    rewind(fh);

    // Check for duplicates

    while (fgets(line, sizeof(line), fh) != NULL)
    {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        token = strtok(line, delimiter);
        while (token != NULL)
        {
            if (strcmp(student_details[SERIAL], token) == 0)
            {
                fclose(fh);
                return DUPLICATE_SERIAL;
            }
            else if (strcmp(student_details[REGNO], token) == 0)
            {
                fclose(fh);
                return DUPLICATE_REGNO;
            }
            token = strtok(NULL, delimiter);
        }

        isEmptyLine = 0;
    }

    // Write the record to the file
    if (isEmptyLine == 0)
        fprintf(fh, "\n"); // Go to the next line if the line is not empty

    fprintf(fh, "%s    %s    %s    %s\n", student_details[SERIAL], student_details[REGNO], student_details[FNAME], student_details[LNAME]);

    fclose(fh);

    return DETAILS_SAVED_SUCCESSFULLY;
}
