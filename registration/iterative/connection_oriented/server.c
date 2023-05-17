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

Response add_student_record(char student_details[4][20]);

int main()
{
    int r, sockfd, newsockfd, j, k;
    char recv_buffer[BUFSIZ], send_buffer[BUFSIZ], client[BUFSIZ];
    char student_details[4][20];
    struct addrinfo hints, *host;
    struct sockaddr client_address;
    socklen_t clientaddr_len = sizeof client_address;
    time_t connect_time, disconnect_time;

    putchar('\n');

    /* ************************** CONFIGURE SERVER HOST ADDRESS ***************************** */

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming
    hints.ai_flags = AI_PASSIVE;     // listen to any interface

    r = getaddrinfo("127.0.0.1", "8080", &hints, &host);

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
        exit(EXIT_FAILURE);
    }

    puts("✅ Host server socket created successfully!");

    /* ******************************* BIND TO SOCKET ************************************* */

    r = bind(sockfd, host->ai_addr, host->ai_addrlen);

    if (r == -1)
    {
        puts("⛔ Server Malfunction! Failed to bind host to socket. Exiting server program...");
        exit(EXIT_FAILURE);
    }

    puts("✅ Socket bound to host successfully!");

    /* *************************** LISTEN TO INCOMING CONNECTIONS ************************* */

    r = listen(sockfd, 3); // number of pending connections that can wait in the queue is 3.

    if (r == -1)
    {
        puts("⛔ Server Malfunction! Failed to listen to incoming connections Exiting server program...");
        exit(EXIT_FAILURE);
    }

    puts("✅ TCP server is listening...");
    puts("\n--------------------------------------------------------");

    while (1)
    {
        /* ************************ ACCEPT CONNECTION REQUESTS *********************** */

        newsockfd = accept(sockfd, &client_address, &clientaddr_len);

        if (newsockfd == -1)
        {
            puts("⛔ Server Malfunction! Cannot accept incoming connections! Exiting server program...");
            exit(EXIT_FAILURE);
        }

        getnameinfo(&client_address, clientaddr_len, client, BUFSIZ, 0, 0, NI_NUMERICHOST); // get client ip address
        connect_time = time(NULL);                                                          // get the time that the client has connected
        printf("✅ Client %s connected to the server at \033[34m%s\033[0m", client, ctime(&connect_time));

        /* ******************************** RECEIVE STUDENT DETAILS **************************** */

        r = recv(newsockfd, recv_buffer, BUFSIZ, 0);

        if (r < 1)
        {
            printf("⛔ Received 0 bytes of data from client %s\n", client);
        }
        else
        {
            recv_buffer[r] = '\0'; // terminate the incoming string
            printf("📨 Received %d bytes of data from client %s\n", r, client);
            puts("⌛ Processing data...");

            /* ************************** EXTRACT STUDENT DETAILS ***************************** */
            j = 0;
            k = 0;
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

            /* **************************** ADD STUDENT RECORD ****************************** */

            r = add_student_record(student_details);
        }

        /* ****************************** FORMULATE RESPONSE ******************************** */

        switch (r)
        {
        case CANNOT_OPEN_FILE:
            // message displayed in color red
            strcpy(send_buffer, "\033[31mCould not open file student_details.txt\033[0m");
            break;
        case DUPLICATE_SERIAL:
            // message displayed in color yellow
            strcpy(send_buffer, "\033[33mDuplicate Serial no. Failed to add record\033[0m");
            break;
        case DUPLICATE_REGNO:
            // message displayed in color yellow
            strcpy(send_buffer, "\033[33mDuplicate Registration no. Failed to add record\033[0m");
            break;
        case DETAILS_SAVED_SUCCESSFULLY:
            // message displayed in color green
            strcpy(send_buffer, "\033[32mRecord added successfully\033[0m");
            break;
        default:
            // message displayed in color red
            strcpy(send_buffer, "\033[31mError saving record.\033[0m");
            break;
        }
        printf("...%s\n", send_buffer);

        /* ************************* SEND RESPONSE BACK TO CLIENT ************************* */

        r = send(newsockfd, send_buffer, strlen(send_buffer), 0);
        if (r < 1)
        {
            printf("⛔ Server Malfunction! Failed to send response back to the client %s", client);
            exit(EXIT_FAILURE);
        }

        printf("📤 Response sent to client %s\n", client);

        // close client socket
        close(newsockfd);

        disconnect_time = time(NULL);
        printf("✅ Client %s disconnected from the server at \033[34m%s\033[0m", client, ctime(&disconnect_time));

        puts("\n--------------------------------------------------------");
    }

    /* *********************************** CLOSE UP **************************************** */

    freeaddrinfo(host);
    close(sockfd);

    return EXIT_SUCCESS;
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
    char *token;
    char delimiter[] = "            ";

    fh = fopen("student_details.txt", "a+");
    if (fh == NULL)
        return CANNOT_OPEN_FILE;

    fseek(fh, 0, SEEK_END); // Move the file pointer to the end of the file

    switch (ftell(fh))
    {
        // Check if the file is empty
    case 0:
        fprintf(fh, "SERIAL         REGISTRATION           FULL NAME\n");
        fprintf(fh, "-----------------------------------------------\n");
        break;

    default:
        fseek(fh, -1, SEEK_END); // Move the file pointer to the last character
        if (fgetc(fh) != '\n')
            fprintf(fh, "\n"); // Add a newline if the last character is not a newline

        rewind(fh);
        while (fgets(line, sizeof(line), fh) != NULL)
        {
            if (line[strlen(line) - 1] == '\n')
                line[strlen(line) - 1] = '\0';

            token = strtok(line, delimiter);
            if (token != NULL)
            {
                if (strcmp(student_details[SERIAL], token) == 0)
                {
                    fclose(fh);
                    return DUPLICATE_SERIAL;
                }

                token = strtok(NULL, delimiter); // Move to the next token (REGNO)
                if (token != NULL && strcmp(student_details[REGNO], token) == 0)
                {
                    fclose(fh);
                    return DUPLICATE_REGNO;
                }
            }
        }
        break;
    }

    // Write the record to the file
    fprintf(fh, "%s            %s            %s %s\n", student_details[SERIAL], student_details[REGNO], student_details[FNAME], student_details[LNAME]);

    fclose(fh);

    return DETAILS_SAVED_SUCCESSFULLY;
}
