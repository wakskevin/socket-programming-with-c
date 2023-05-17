#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

typedef enum
{
    INVALID_OPERATOR = 30,
    LEFT_OPERAND_NOT_A_NUMBER = 31,
    RIGHT_OPERAND_NOT_A_NUMBER = 32,
    CALCULATION_SUCCESSFUL = 33
} Response;

Response do_calculation(char expression[4][20], char result[BUFSIZ]);
int is_operator(char *operator);
int is_number(char *number);

int main()
{
    int r, sockfd, newsockfd, j, k;
    char recv_buffer[BUFSIZ], send_buffer[BUFSIZ], result[BUFSIZ], client[BUFSIZ];
    char expression[4][20];
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

        /* ******************************** RECEIVE CALCULATION INPUT **************************** */

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

            /* ************************** EXTRACT OPERANDS AND OPERATOR ***************************** */
            
            j = 0;
            k = 0;
            for (int i = 0; i < (strlen(recv_buffer) - 1); i++)
            {
                // remove the @@@ separators
                if (recv_buffer[i] == '@' && recv_buffer[i + 1] == '@' && recv_buffer[i + 2] == '@')
                {
                    i = i + 2;
                    expression[j][k] = '\0';
                    j++;
                    k = 0;
                }
                // stopping before the $$$ terminator
                else if (recv_buffer[i] == '$' && recv_buffer[i + 1] == '$' && recv_buffer[i + 2] == '$')
                {
                    expression[j][k] = '\0';
                    break;
                }
                else
                {
                    expression[j][k] = recv_buffer[i];
                    k++;
                }
            }

            /* **************************** DO CALCULATION ****************************** */

            r = do_calculation(expression, result);
        }

        /* ****************************** FORMULATE RESPONSE ******************************** */

        switch (r)
        {
        case INVALID_OPERATOR:
            // message displayed in color yellow
            strcpy(send_buffer, "\033[33mUnrecognized operator. Neither +, -, * nor / was picked\033[0m");
            break;
        case LEFT_OPERAND_NOT_A_NUMBER:
            // message displayed in color yellow
            strcpy(send_buffer, "\033[33mInvalid operand. First operand was not an integer\033[0m");
            break;
        case RIGHT_OPERAND_NOT_A_NUMBER:
            // message displayed in color yellow
            strcpy(send_buffer, "\033[33mInvalid operand. Second operand was not an integer\033[0m");
            break;
        case CALCULATION_SUCCESSFUL:
            // message displayed in color green
            strcpy(send_buffer, "\033[32mThe answer is \033[0m");
            strcat(send_buffer, result);
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

Response do_calculation(char expression[4][20], char result[BUFSIZ])
{
    enum expression_parts
    {
        FDIGIT,
        OPERATOR,
        SDIGIT
    };

    int r;
    float answer;

    r = is_operator(expression[OPERATOR]); // checks if operator provided is valid (either +, -, * or /)
    if (r == -1)
    {
        return INVALID_OPERATOR;
    }

    r = is_number(expression[FDIGIT]); // checks if the first operand is a valid integer
    if (r == -1)
    {
        return LEFT_OPERAND_NOT_A_NUMBER;
    }

    r = is_number(expression[SDIGIT]); // checks if the second operand is a valid integer
    if (r == -1)
    {
        return RIGHT_OPERAND_NOT_A_NUMBER;
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

    snprintf(result, BUFSIZ, "\033[32m%g\033[0m", answer); // result displayed in color green

    printf("\n%s %s %s = %s\n\n", expression[FDIGIT], expression[OPERATOR], expression[SDIGIT], result);

    return CALCULATION_SUCCESSFUL;
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
