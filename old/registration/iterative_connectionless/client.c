#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// -------------- function prototypes

int getuserinput(char buffer[BUFSIZ]); // function to get user input and format based on application protocol

int main()
{
    int r;
    int sockfd;
    char send_buffer[BUFSIZ];
    char recv_buffer[BUFSIZ];
    struct addrinfo hints;
    struct addrinfo *server;

    /* *********************** configure address of the server to send to *********************** */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_DGRAM; // UDP, datagram

     r = getaddrinfo("192.168.8.102", "8714", &hints, &server);

    if (r != 0)
    {
        printf("⛔ Failed to configure server address details. Exiting program...\n\n");
        exit(1);
    }

    /* *********************** create the socket *********************** */

    sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

    if (sockfd == -1)
    {
        printf("⛔ Failed to create client socket. Exiting program...\n\n");
        exit(1);
    }

    /* *********************** get user input and format it *********************** */

    r = getuserinput(send_buffer);
    if (r == -1)
    {
        printf("⛔ Failed to get and format student user input. Exiting program...\n\n");
        exit(1);
    }

    /* *********************** send to the server *********************** */

    r = sendto(sockfd, send_buffer, strlen(send_buffer), 0, server->ai_addr, server->ai_addrlen);
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

    r = recvfrom(sockfd, recv_buffer, BUFSIZ, 0, server->ai_addr, &server->ai_addrlen);
    if (r < 1)
    {
        printf("⛔ Failed! Received 0 bytes of data. Exiting Program...\n\n");
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
