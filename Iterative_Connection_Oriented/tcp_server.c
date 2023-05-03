#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int savetofile(char buffer[BUFSIZ]); // savetofile() function prototype

int main()
{
    int r, sockfd, newsockfd;
    char recv_buffer[BUFSIZ];
    char *send_buffer;

    /* *********************** configure host *********************** */

    struct addrinfo hints, *host;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming

    r = getaddrinfo("127.0.0.1", "8009", &hints, &host);
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

    r = listen(sockfd, 1); // number of pending connections that can wait in the queue is 1.
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

        printf("Received %d bytes of data from the Client\n", r);
    }

    // TODO: /* *********************** process user input *********************** */

    r = savetofile(recv_buffer);
    if (r == -1)
    {
        perror("Failed to save received user data to file\n\n");
        exit(1);
    }
    else
    {
        puts("Record has been added");
    }

    // TODO: /* *********************** formulate response *********************** */
    /*
    if there's some error eg duplicate record
        send_buffer = "The appropriate message";
    else is everything is okay
        send_buffer = "The details have been received, processed and saved";
    */

    /* *********************** send response back to the client *********************** */
 
    r = send(newsockfd, send_buffer, strlen(send_buffer), 0);
    if (r < 1)
    {
        perror("Failed to send receipt acknowledgement message back to the client.");
        exit(1);
    }
    else
    {
        printf("Acknowledgement message sent to the Client\n");
    }

    /* *********************** close up *********************** */

    close(newsockfd);   // close socket created by accept() function
    freeaddrinfo(host); // free up allocated memory
    close(sockfd);      // close socket created by socket() function

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

    /* *********************** Open the file *********************** */

    fh = fopen("student_details.csv", "a");
    if (fh == NULL)
    {
        puts("Could not create file details.csv");
        return (-1);
    }

    /**
     * format the received string removing the '@@@' separators
     * and stopping before the '$$$' terminator symbol
     */

    for (int i = 0; i < (strlen(buffer) - 1); i++)
    {
        if (buffer[i] == '@' && buffer[i + 1] == '@' && buffer[i + 2] == '@')
        {
            i = i + 2;
            details[j][k] = '\0'; // strings in C end are terminated with the null character.
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

    /* *********************** writing to the file *********************** */

    switch (ftell(fh))
    {

        // start writing at the begining of the csv file if empty

    case 0:
        fprintf(fh, "%s,%s,%s %s", details[SERIAL], details[REGNO], details[FNAME], details[LNAME]);
        break;

        // if the csv file already has records go to the next line and start writing from there

    default:
        fprintf(fh, "\n%s,%s,%s %s", details[SERIAL], details[REGNO], details[FNAME], details[LNAME]);
        break;
    }

    /* *********************** Close the file *********************** */

    fclose(fh);

    putchar('\n');
    return (0);
}
