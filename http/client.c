#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/* global variables */

int r;
int sockfd;
int count = 0;
char domain[BUFSIZ];
char path[BUFSIZ];
char response[BUFSIZ];
char request[BUFSIZ];
struct addrinfo hints;
struct addrinfo *webserver;

/* function prototypes */
void generate_request();                               // generate request header and body from user inpput
void get_webpage();                                    // get webpage that user requested fo
void close_resources(struct addrinfo *addr, int sock); // free allocated address memory, close socket and exit

void main()
{
    printf("Enter the domain name (without http:// or https://) to connect to: ");
    scanf("%s", domain);
    printf("Enter path (e.g. /index.html) or simply type (/): ");
    scanf("%s", path);

    generate_request();
    get_webpage();
    close_resources(webserver, sockfd);
}

void generate_request()
{
    count++;
    // printf("\n%d\n", count);
    memset(request, 0, BUFSIZ); // clear contents
    strcat(request, "GET ");
    if (count == 2)
    {
        strcat(request, "https://");
    }
    else
    {
        strcat(request, "http://");    
    }
    strcat(request, domain);
    strcat(request, path);
    strcat(request, " HTTP/1.1\r\n");
    strcat(request, "Host: ");
    strcat(request, domain);
    strcat(request, "\r\n");
    strcat(request, "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:89.0) Gecko/20100101 Firefox/89.0\r\n");
    strcat(request, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n");
    strcat(request, "Accept-Language: en-US,en;q=0.5\r\n");
    strcat(request, "Accept-Encoding: gzip, deflate, br\r\n");
    strcat(request, "Connection: keep-alive\r\n");
    strcat(request, "\r\n");

    // printf("\n%s\n", request);
}

void get_webpage()
{
    /* ***************************** configure remote address ********************************* */

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4 connection
    hints.ai_socktype = SOCK_STREAM; // TCP, streaming

    r = getaddrinfo(domain, "80", &hints, &webserver);
    if (r != 0)
    {
        puts("⛔ ERROR! Failed to configure server address details. Exiting program...\n");
        exit(0);
    }

    /* ************************ create socket to connect with server *********************** */

    sockfd = socket(webserver->ai_family,
                    webserver->ai_socktype,
                    webserver->ai_protocol);
    if (sockfd == -1)
    {
        puts("⛔ ERROR! Failed to create socket. Exiting program...\n");
        freeaddrinfo(webserver); // free allocated address memory
        exit(0);
    }

    /* ******************************* connect to the server ********************************* */

    r = connect(sockfd,
                webserver->ai_addr,
                webserver->ai_addrlen);
    if (r == -1)
    {
        puts("⛔ ERROR! Failed to connect to web server. Exiting program...\n");
        close_resources(webserver, sockfd);
    }

    /* ************************* send HTTP GET request ******************************* */

    r = send(sockfd, request, strlen(request), 0);
    if (r == -1)
    {
        puts("⛔ ERROR! Failed to send GET request. Exiting program...\n");
        close_resources(webserver, sockfd);
    }

    /* *************************** receive response from server ********************************* */

    r = recv(sockfd, response, BUFSIZ, 0);

    if (r == -1)
    {
        puts("⛔ ERROR! Cannot receive data. Exiting program...\n");
        close_resources(webserver, sockfd);
    }
    else if (r == 0)
    {
        printf("🛑 %s CLOSED THE CONNECTION! Exiting program...\n\n", domain);
    }
    else
    {
        response[r] = '\0'; // terminate string

        if (response[9] == '3' && response[10] == '0' && response[11] == '1') // HTTP 301 Moved Permanently
        {
            if (count > 2)
            {
                printf("📨 Received %d bytes of data from the Server\n\n\033[32m%s\033[0m", r, response); // response is displayed in color green in the terminal
            }
            else
            {
                generate_request();
                get_webpage();
            }
        }
        else // HTTP 200 OK
        {
            printf("📨 Received %d bytes of data from the Server\n\n\033[32m%s\033[0m", r, response); // response is displayed in color green in the terminal
        }
    }
}

void close_resources(struct addrinfo *addr, int sock) // free allocated address memory, close socket and exit
{
    freeaddrinfo(webserver);
    close(sockfd);
    putchar('\n');
    exit(0);
}
