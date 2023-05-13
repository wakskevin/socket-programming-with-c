#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 50

// -------------- function prototypes

int unformat_and_save(char buffer[BUFSIZ]);

/**
 * configure address details for the server
 * socket()
 * bind()
 * listen()
 * accept()
 * recvfrom()
 * (do stuff) - unformat_and_save() Serial@@@Regno@@@fname@@@lname$$$
 * sendto()
 * close()
*/


int main()
{
    int r;
    int sockfd;
    int newsockfd;
    char recv_buffer[BUFSIZ];
    char send_buffer[BUFSIZ];

    /* *********************** configure host *********************** */

    struct addrinfo hints, *host;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming

    r = getaddrinfo("127.0.0.1", "8724", &hints, &host);
    if (r != 0)
    {
        perror("Failed to configure host server address details\n\n");
        exit(1);
    }

    /* *********************** create socket to communicate with host *********************** */

    sockfd = socket(host->ai_family,
                    host->ai_socktype,
                    host->ai_protocol);
    if (sockfd == -1)
    {
        perror("Failed to create client socket.\n\n");
        exit(1);
    }

    /* *********************** bind host to socket *********************** */

    r = bind(sockfd,
             host->ai_addr,
             host->ai_addrlen);
    if (r == -1)
    {
        perror("Failed to bind host to socket\n\n");
    }

    /* *********************** listen to incoming connections *********************** */

    r = listen(sockfd, 3); // number of pending connections that can wait in the queue is 3.
    if (r == -1)
    {
        perror("Failed to listen to incoming connections\n\n");
    }
    else
    {
        puts("TCP server is listening...");
    }

    /* *********************** accept connection requests *********************** */

    struct sockaddr client_address;
    socklen_t clientaddr_len = sizeof(client_address);

    newsockfd = accept(sockfd, &client_address, &clientaddr_len);
    if (newsockfd == -1)
    {
        perror("Failed to create client socket.\n\n");
        exit(1);
    }
    else
    {
        printf("Accepting new connection on file descriptor %d\n", newsockfd);
    }

    /* *********************** receive student details *********************** */

    r = recv(newsockfd, recv_buffer, BUFSIZ, 0);
    if (r < 1)
    {
        perror("Failed! Received 0 bytes of data\n\n");
        exit(1);
    }
    else
    {
        recv_buffer[r] = '\0'; // terminate the incoming string

        printf("Received %s bytes of data from the Client\n", recv_buffer);
    }
    /* *********************** process student details *********************** */

    r = unformat_and_save(recv_buffer);

    /* *********************** formulate response *********************** */
    if (r == 1)
    {
        strcpy(send_buffer, "⛔ Could not create file student_details.csv. Exiting program...");
    }
    else if (r == -1)
    {
        strcpy(send_buffer, "⛔ Duplicate Serial no. Exiting Program...");
    }
    else if (r == -2)
    {
        strcpy(send_buffer, "⛔ Duplicate Registration no. Exiting Program...");
    }
    else
    {
        strcpy(send_buffer, "✅ Record added successfully\n\n");
    }

    /* *********************** send response *********************** */

    r = send(newsockfd, send_buffer, strlen(send_buffer), 0);
    if (r < 1)
    {
        perror("Failed to send message to client");
        exit(1);
    }

    close(newsockfd);   // close socket created by accept() function
    freeaddrinfo(host); // free up allocated memory
    close(sockfd);      // close socket created by socket() function

    putchar('\n');
    return 0;

    return 0;
}

int unformat_and_save(char buffer[BUFSIZ])
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

    /**
     * format the received string removing the '@@@' separators
     * and stopping before the '$$$' terminator symbol
     */

    for (int i = 0; i < (strlen(buffer) - 1); i++)
    {
        if (buffer[i] == '@' && buffer[i + 1] == '@' && buffer[i + 2] == '@')
        {
            i = i + 2;
            details[j][k] = '\0'; // strings in C are terminated with the null character.
            j++;
            k = 0;
        }
        else if (buffer[i] == '$' && buffer[i + 1] == '$' && buffer[i + 2] == '$')
        {
            details[j][k] = '\0'; // strings in C are terminated with the null character.
            break;
        }
        else
        {
            details[j][k] = buffer[i];
            k++;
        }
    }

    fh = fopen("student_details.csv", "a");
    if (fh == NULL)
    {
        return (1);
    }

    /* *********************** writing to the file *********************** */
    switch (ftell(fh))
    {
    case 0:
        fprintf(fh, "%s,%s,%s %s", details[SERIAL], details[REGNO], details[FNAME], details[LNAME]);
        break;

        // if the csv file already has records go to the next line and start writing from there
    default:
        FILE *file;
        char word[MAX_WORD_LENGTH];
        char delimiter[] = ",";
        char *token;

        // Open file for reading
        file = fopen("student_details.csv", "r");
        if (file == NULL)
        {
            return (1);
        }

        while (fgets(word, MAX_WORD_LENGTH, file) != NULL) // Read file word by word
        {
            if (word[strlen(word) - 1] == '\n') // Replace newline character with null character
            {
                word[strlen(word) - 1] = '\0';
            }

            token = strtok(word, delimiter); // Split word by comma
            while (token != NULL)
            {
                if (strcmp(details[SERIAL], token) == 0)
                {
                    return (-1);
                }
                else if (strcmp(details[REGNO], token) == 0)
                {
                    return (-2);
                }
                token = strtok(NULL, delimiter);
            }
        }
        fprintf(fh, "\n%s,%s,%s %s", details[SERIAL], details[REGNO], details[FNAME], details[LNAME]);
        fclose(file);
        break;
    }

    /* *********************** Close the file *********************** */

    fclose(fh);
    putchar('\n');
    return (0);
}
