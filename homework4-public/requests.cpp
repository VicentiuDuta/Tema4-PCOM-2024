#include <iostream>
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "json.hpp"


using namespace std;
using json = nlohmann::json;

#define PORT 8080
#define IP "34.246.184.49"


char* compute_delete_request(const char *host, const char *url, char* token) {
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    memset(line, 0, LINELEN);
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if(token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 3: add new line at end of header
    free(line);
    compute_message(message, "");

    return message;
}

char *compute_get_request(const char *host, const char *url, char* cookie, char* token) 
{
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    memset(line, 0, LINELEN);
    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3: add necessary headers (Cookie is mandatory)
    if(cookie != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    if(token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 4: add new line at end of header
    free(line);
    compute_message(message, "");

    return message;
}

char *compute_post_request(const char *host, const char *url, const char* content_type, char* token, json data)
{
    char *message = (char*) calloc(BUFLEN, sizeof(char));
    char *line = (char*) calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3: add necessary headers (Content-Type and Cookie are mandatory)
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // Step 4: add the content length
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", data.dump().size());
    compute_message(message, line);

    if(token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    memset(line, 0, LINELEN);
    compute_message(message, line);

    // Step 6: add the actual data from the POST request
    memset(line, 0, LINELEN);
    sprintf(line, "%s", data.dump().c_str());
    compute_message(message, line);


    free(line);
    return message;
}
