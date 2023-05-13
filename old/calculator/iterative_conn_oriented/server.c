#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// -------------- function prototypes

void unformat_string(char buffer[BUFSIZ]);                     // function for removing '@@@' separators and stopping before '$$$'
int do_calculation(char *num1, char *operator, char * num2); // function for doing the calculation
int is_operator(char *operator);                             // function for checking whether operator provided is either +, -, * or /
int is_number(char *number);                                 // function for checking whether operands provided are valid integers

// ------------- global variables

char expression[4][20];
float answer;

// -------------- enumerations

enum expression_parts
{
    FDIGIT,
    OPERATOR,
    SDIGIT
};

// -------------- main() function

int main()
{
    int r;
    int sockfd;
    int newsockfd;
    char recv_buffer[BUFSIZ];
    char send_buffer[BUFSIZ];
    struct addrinfo hints;
    struct addrinfo *host;

    /* *********************** configure host *********************** */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming

    r = getaddrinfo("127.0.0.1", "8192", &hints, &host);
    if (r != 0)
    {
        printf("⛔ Failed to configure host server address details. Exiting server program...\n\n");
        exit(1);
    }

    /* *********************** create socket to communicate with host *********************** */

    sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol);
    if (sockfd == -1)
    {
        printf("⛔ Failed to create socket. Exiting server program...\n\n");
        exit(1);
    }

    /* *********************** bind host to socket *********************** */

    r = bind(sockfd, host->ai_addr, host->ai_addrlen);
    if (r == -1)
    {
        printf("⛔ Failed to bind server host to socket. Exiting server program...\n\n");
        exit(1);
    }

    /* *********************** listen to incoming connections *********************** */

    r = listen(sockfd, 3); // number of pending connections that can wait in the queue is 3.
    if (r == -1)
    {
        printf("⛔ Failed to listen to incoming connections. Exiting server program...\n\n");
        exit(1);
    }
    else
    {
        puts("✅ TCP server is listening...");
    }

    /* *********************** accept connection requests *********************** */

    struct sockaddr client_address;
    socklen_t clientaddr_len = sizeof(client_address);

    newsockfd = accept(sockfd, &client_address, &clientaddr_len);
    if (newsockfd == -1)
    {
        printf("⛔ Failed to create new socket by accept(). Exiting server program...\n\n");
        exit(1);
    }
    else
    {
        printf("✅ Accepting new connection on file descriptor %d\n", newsockfd);
    }

    /* *********************** receive calculation input *********************** */

    r = recv(newsockfd, recv_buffer, BUFSIZ, 0);
    if (r < 1)
    {
        printf("⛔ Received 0 bytes of data from client\n\n");
        exit(1);
    }
    else
    {
        recv_buffer[r] = '\0'; // terminate the incoming string

        printf("📨 Received %d bytes of data from the Client\n", r);
    }

    /* *********************** process user input *********************** */

    unformat_string(recv_buffer); // remove @@@ separators and $$$ terminating symbol

    r = do_calculation(expression[FDIGIT], expression[OPERATOR], expression[SDIGIT]); // do calculation

    /* *********************** formulate response *********************** */

    if (r == -1)
    {
        strcpy(send_buffer, "⛔ Unrecognized operator. Neither +, -, * nor / was picked. Exiting program...");
    }
    else if (r == -2)
    {
        strcpy(send_buffer, "⛔ Invalid operand. First operand was not an integer. Exiting program...");
    }
    else if (r == -3)
    {
        strcpy(send_buffer, "⛔ Invalid operand. Second operand was not an integer. Exiting program...");
    }
    else
    {
        char result[BUFSIZ];
        snprintf(result, BUFSIZ, "%g", answer);
        strcpy(send_buffer, "✅ The answer is ");
        strcat(send_buffer, result);
    }

    /* *********************** send response back to the client *********************** */

    r = send(newsockfd, send_buffer, strlen(send_buffer), 0);
    if (r < 1)
    {
        printf("⛔ Failed to send receipt acknowledgement message back to the client.");
        exit(1);
    }
    else
    {
        printf("📤 Response sent to the Client\n");
    }

    /* *********************** close up *********************** */

    close(newsockfd);   // close socket created by accept() function
    freeaddrinfo(host); // free up allocated memory
    close(sockfd);      // close socket created by socket() function

    putchar('\n');
    return 0;
}

// -------------- subfunctions

void unformat_string(char buffer[BUFSIZ])
{
    int j = 0, k = 0;

    for (int i = 0; i < (strlen(buffer) - 1); i++)
    {
        if (buffer[i] == '@' && buffer[i + 1] == '@' && buffer[i + 2] == '@')
        {
            i = i + 2;
            expression[j][k] = '\0'; // strings in C are terminated with the null character.
            j++;
            k = 0;
        }
        else if (buffer[i] == '$' && buffer[i + 1] == '$' && buffer[i + 2] == '$')
        {
            expression[j][k] = '\0'; // strings in C are terminated with the null character.
            break;
        }
        else
        {
            expression[j][k] = buffer[i];
            k++;
        }
    }
}

int do_calculation(char *num1, char *operator, char * num2)
{
    int r;

    r = is_operator(expression[OPERATOR]); // checks if operator provided is valid (either +, -, * or /)
    if (r == -1)
    {
        return (-1);
    }

    r = is_number(expression[FDIGIT]); // checks if the first operand is a valid integer
    if (r == -1)
    {
        return (-2);
    }

    r = is_number(expression[SDIGIT]); // checks if the second operand is a valid integer
    if (r == -1)
    {
        return (-3);
    }

    /* *********************** perform arithmetic operation *********************** */

    switch (expression[OPERATOR][0])
    {
    case '+':
        answer = (atoi(expression[FDIGIT])) + (atoi(expression[SDIGIT]));
        break;
    case '-':
        answer = (atoi(expression[FDIGIT])) - (atoi(expression[SDIGIT]));
        break;
    case '*':
        answer = (atoi(expression[FDIGIT])) * (atoi(expression[SDIGIT]));
        break;
    case '/':
        answer = (atoi(expression[FDIGIT])) / (atoi(expression[SDIGIT]));
        break;
    default:
        break;
    }

    printf("\n%s %s %s = %f\n\n", expression[FDIGIT], expression[OPERATOR], expression[SDIGIT], answer);

    return (0);
}

int is_operator(char *operator)
{
    if (strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0 || strcmp(operator, "*") == 0 || strcmp(operator, "/") == 0)
    {
        return (0);
    }
    else
    {
        return (-1);
    }
}

int is_number(char *number)
{
    for (int i = 0; i < strlen(number); i++)
    {
        if (!isdigit(number[i]))
        {
            return (-1);
        }
    }
    return (0);
}
