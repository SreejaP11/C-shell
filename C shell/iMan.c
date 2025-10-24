#include "headers.h"

// Function to strip HTML tags
void strip_html_tags(char *src, char *dest) {
    int in_tag = 0; // Flag to track whether we're inside an HTML tag
    while (*src) {
        if (*src == '<') {
            in_tag = 1;
        } else if (*src == '>') {
            in_tag = 0;
        } else if (!in_tag) {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0'; // Null-terminate the destination string
}

void fetch_man_page(const char *command) 
{
    int sockfd;
    struct sockaddr_in servaddr;
    struct hostent *server;
    char request[MAX_LENGTH];
    char response[RESPONSE_SIZE];
    char stripped_response[RESPONSE_SIZE]; // Buffer for stripped content
    char *body;
    ssize_t n;
    int total_bytes = 0;

    // Resolve the server address
    server = gethostbyname(SERVER);
    if (server == NULL) {
        fprintf(stderr, "\033[0;31mERROR: No such host\033[0m\n");
        exit(1);
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("\033[0;31mERROR: socket\033[0m");
        exit(1);
    }

    // Set up the server address struct
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    memcpy(&servaddr.sin_addr.s_addr, server->h_addr, server->h_length);
    servaddr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("\033[0;31mERROR: connect\033[0m");
        exit(1);
    }

    // Construct the URL with the command name
    char url[BUFFER_SIZE];
    snprintf(url, sizeof(url), "/man1/%s", command);

    // Send HTTP GET request
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.0\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             url, SERVER);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("\033[0;31mERROR: send\033[0m");
        exit(1);
    }

    // Receive the response in chunks
    total_bytes = 0;
    while ((n = recv(sockfd, response + total_bytes, sizeof(response) - total_bytes - 1, 0)) > 0) {
        total_bytes += n;
        // If response buffer is full, break to avoid overflow
        if (total_bytes >= RESPONSE_SIZE - 1) {
            fprintf(stderr, "\033[0;31mERROR: Response buffer overflow\033[0m\n");
            break;
        }
    }

    if (n < 0) {
        perror("\033[0;31mERROR: recv\033[0m");
        exit(1);
    }

    // Null-terminate the response string
    response[total_bytes] = '\0';

    // Close the socket
    close(sockfd);

    // Find the end of headers
    body = strstr(response, "\r\n\r\n");
    // printf("%s\n", body);
    if (body == NULL) {
        // Try different newline variations
        body = strstr(response, "\n\n");
        if (body == NULL) {
            body = strstr(response, "\r\r");
        }
    }
    if (body != NULL) {
        body += 7; // Skip past the \r\n\r\n (or \n\n, etc.)
    } else {
        // If no headers found, fallback to using the full response
        body = response;
    }

    // Strip HTML tags from the body content
    strip_html_tags(body, stripped_response);

    // Print the stripped response
    printf("%s\n", stripped_response);
}
