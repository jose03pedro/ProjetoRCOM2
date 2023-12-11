#include "../include/getip.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

char *argv1;

void getIp(char *hostName) {
    struct hostent *h;
    if ((h = gethostbyname(argv1)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

    urlArgs url_args;

    if (url_parse(argv1, &url_args) < 0) {
        perror("url_parse()");
        exit(-1);
    }
    if ((h = gethostbyname(argv1)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

    return 0;
}

int url_parse(char *url, urlArgs *url_parse) {
    char *start = strtok(url, "/");
    char *middle = strtok(NULL, "/");
    char *final = strtok(NULL, "");

    if (start == NULL || middle == NULL || final == NULL) {
        printf("Invalid URL\n");
        return -1;
    }

    if (strchr(middle, '@') != NULL) {
        char *login = strtok(middle, "@");
        char *host = strtok(NULL, "@");
        char *user = strtok(login, ":");
        char *password = strtok(NULL, ":");
        strcpy(url_parse->user, user);
        if (password != NULL)
            ? strcpy(url_parse->password, password)
            : strcpy(url_parse->password, "pass");
        strcpy(url_parse->host, host);
    } else {
        strcpy(url_parse->user, "anonymous");
        strcpy(url_parse->password, "pass");
        strcpy(url_parse->host, middle);
    }

    char *file_name = strrchr(final, '/');
    if (file_name == NULL) {
        strcpy(url_parse->fileName, final);
        strcpy(url_parse->path, "/");
    } else {
        strcpy(url_parse->fileName, file_name + 1);
        strcpy(url_parse->path, final);
    }

    if (!strcmp(url_parse->host, "") || !strcmp(url_parse->path, "")) {
        printf("Invalid URL\n");
        return -1;
    }

    struct hostent *h;

    if ((h = gethostbyname(url_parse->host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    strcpy(url_parse->host_name, h->h_name);
    strcpy(url_parse->ip, inet_ntoa(*((struct in_addr *)h->h_addr)));

    return 0;
}

int write_to_server(int socket, char *message) {
    size_t bytes;
    bytes = write(socket, message, strlen(message));
    if (bytes > 0)
        printf("Bytes escritos %ld\n", bytes);
    else {
        perror("write()");
        exit(-1);
    }
    return 0;
}

int connection_server(int socket, char *user, char *password) {
    char *message = "USER ";
    strcat(message, user);
    strcat(message, "\n");
    write_to_server(socket, message);

    message = "PASS ";
    strcat(message, password);
    strcat(message, "\n");
    write_to_server(socket, message);

    return 0;
}
