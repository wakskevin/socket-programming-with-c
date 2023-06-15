#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// function prototypes
int getuserinput(char send_buffer[BUFSIZ], char serial[20], char regno[20], char fname[15], char lname[15]);

int main()
{
    int r, sockfd;
    char send_buffer[BUFSIZ], recv_buffer[BUFSIZ], serial[20], regno[20], fname[15], lname[15];
    struct addrinfo hints, *server;

    putchar('\n');

    // configure address

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4 connection
    hints.ai_socktype = SOCK_DGRAM; // UDP connection

    r = getaddrinfo("127.0.0.1", "8080", &hints, &server);

    if (r != 0)
    {
        puts("❌ Failed to set address of remote server. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("Address of remote server set successfully!");

    // create socket

    sockfd = socket(server->ai_family,
                    server->ai_socktype,
                    server->ai_protocol);
    if (sockfd == -1)
    {
        puts("❌ Failed to create client socket. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("Client socket created successfully!");

    // get user input

    r = getuserinput(send_buffer, serial, regno, fname, lname);

    if (r == -1)
    {
        puts("❌ Failed to get user input. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("User input got successfully!");

    // format input into string

    strcpy(send_buffer, serial);
    strcat(send_buffer, "@@@"); // separator indicator
    strcat(send_buffer, regno);
    strcat(send_buffer, "@@@"); // separator indicator
    strcat(send_buffer, fname);
    strcat(send_buffer, "@@@"); // separator indicator
    strcat(send_buffer, lname);
    strcat(send_buffer, "$$$"); // finish with the terminator indicator

    putchar('\n');
    // printf("%s\n", send_buffer);

    // send string

    r = sendto(sockfd, send_buffer, strlen(send_buffer), 0, server->ai_addr, server->ai_addrlen);

    if (r == -1)
    {
        puts("❌ Failed to send mesage. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    puts("📤 Student details sent to remote server successfully!");

    // receive server response

    r = recvfrom(sockfd, recv_buffer, BUFSIZ, 0, server->ai_addr, &server->ai_addrlen);

    if (r == -1)
    {
        puts("❌ Failed! Received 0 bytes of data. Exiting client program...");
        exit(EXIT_FAILURE);
    }

    recv_buffer[r] = '\0'; // terminate string
    printf("📨 Received %d bytes of data from the Server\n", r);
    printf("Server responded with '%s'.\n", recv_buffer);
    puts("\n--------------------------------------------------------");

    // close resources

    freeaddrinfo(server); // free allocated address memory
    close(sockfd);        // close socket
    putchar('\n');

    return EXIT_SUCCESS;
}

int getuserinput(char send_buffer[BUFSIZ], char serial[20], char regno[20], char fname[15], char lname[15])
{
    char confirm;
    int trial = 1;

    while (trial <= 3)
    {
        putchar('\n');

        puts("Your serial number?");
        scanf("%s", serial);

        // Check if the serial number has exactly 3 digits
        if (strlen(serial) != 3)
        {
            printf("Invalid serial number! Serial number should have exactly 3 digits.\n");
            printf("Trials done: %d of 3\n", trial);
            putchar('\n');
            trial++;
            continue;
        }

        // Check if the serial number contains only digits
        int i;
        for (i = 0; i < 3; i++)
        {
            if (!isdigit(serial[i]))
            {
                printf("Invalid serial number! Serial number should contain only digits.\n");
                printf("Trials done: %d of 3\n", trial);
                putchar('\n');
                trial++;
                break;
            }
        }

        // If any non-digit character was found in the serial number, restart the loop
        if (i != 3)
            continue;

        // If the serial number is valid, proceed to other input fields
        puts("Your registration number?");
        scanf("%s", regno);
        puts("Your first name?");
        scanf("%s", fname);
        puts("Your last name?");
        scanf("%s", lname);

        putchar('\n');
        printf("You are %s %s of registration %s. Your serial number is %s\n", fname, lname, regno, serial);
        puts("Is this correct? (type 'y' or 'n'):");
        scanf(" %c", &confirm);

        if (confirm == 'y')
        {
            return 0;
        }
        else if (confirm == 'n')
        {
            printf("Trials done: %d of 3\n", trial);
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
            printf("Trials done: %d of 3\n", trial);
            if (trial == 3)
            {
                puts("Trial limit reached!");
            }
            putchar('\n');
            trial++;
        }
    }

    return -1;
}
