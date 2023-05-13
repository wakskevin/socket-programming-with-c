#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// -------------- function prototypes

int getuserinput(char buffer[BUFSIZ]); // function to get user input and format based on application protocol

// -------------- main() function

int main()
{
    char send_buffer[BUFSIZ];
    char recv_buffer[BUFSIZ];
    int r, sockfd;

    /* *********************** configure address of the server to send to *********************** */

    struct addrinfo hints, *server;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming

    r = getaddrinfo("192.168.8.102", "8714", &hints, &server);

    if (r != 0)
    {
        printf("⛔ Failed to configure server address details. Exiting program...\n\n");
        exit(1);
    }

    /* *********************** create the socket *********************** */

    sockfd = socket(server->ai_family,
                    server->ai_socktype,
                    server->ai_protocol);

    if (sockfd == -1)
    {
        printf("⛔ Failed to create client socket. Exiting program...\n\n");
        exit(1);
    }

    /* *********************** connect via the socket to the server *********************** */

    r = connect(sockfd,
                server->ai_addr,
                server->ai_addrlen);
    if (r == -1)
    {
        printf("⛔ Client socket failed to connect to the server. Exiting program...\n\n");
        exit(1);
    }

    /* *********************** get user input and format it *********************** */

    r = getuserinput(send_buffer);
    if (r == -1)
    {
        printf("⛔ Failed to get and format user input for calculation. Exiting program...\n\n");
        exit(1);
    }

    /* *********************** send to the server *********************** */

    r = send(sockfd, send_buffer, strlen(send_buffer), 0);
    if (r < 1)
    {
        printf("⛔ Failed to send mesage. Exiting program...\n\n");
        exit(1);
    }
    else
    {
        puts("*************************************");
        printf("📤 User Input sent to the Server\n");
    }

    /* *********************** receive response from server *********************** */

    r = recv(sockfd, recv_buffer, BUFSIZ, 0);
    if (r < 1)
    {
        printf("⛔ Failed! Received 0 bytes of data\n\n");
        exit(1);
    }
    else
    {
        recv_buffer[r] = '\0';
        printf("📨 Server responded with %d bytes of data:\n", r);
        printf("%s\n", recv_buffer);
    }

    /* *********************** close up *********************** */

    freeaddrinfo(server); // free allocated memory
    close(sockfd);        // close the socket

    putchar('\n');

    return 0;
}

// -------------- subfunctions

int getuserinput(char buffer[BUFSIZ])
{
    char operator[2];
    char fdigit[9];
    char sdigit[9];
    int trial = 1;
    char confirm;

    while (trial <= 3) // user can try three times to ensure that he/she has entered correct details
    {
        printf("Which operation do you want to perform? (Choose either +, -, * or /): ");
        scanf("%s", operator);

        printf("Enter the first digit: ");
        scanf("%s", fdigit);

        printf("Enter the second digit: ");
        scanf("%s", sdigit);

        printf("You want do '%s %s %s'. Is this correct? (Answer 'y' or 'n'): ", fdigit, operator, sdigit);
        scanf(" %c", &confirm);

        if (confirm == 'y')
        {
            strcpy(buffer, fdigit);
            strcat(buffer, "@@@"); // separator indicator
            strcat(buffer, operator);
            strcat(buffer, "@@@"); // separator indicator
            strcat(buffer, sdigit);
            strcat(buffer, "$$$"); // finish with the terminator indicator/

            // printf("\n\n%s\n\n", buffer);
            putchar('\n');

            return 0;
        }
        else if (confirm == 'n')
        {
            printf("trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached!");
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
                puts("Trial limit reached!");
            }
            putchar('\n');
            trial++;
        }
    }

    putchar('\n');
    return -1;
}
