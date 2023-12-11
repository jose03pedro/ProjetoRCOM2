/**
 * Example code for getting the IP address from hostname.
 * tidy up includes
 */
#include "../include/main.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    struct hostent *h;
    int socketA, socketB, port;
    char ip[20];
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <address to get IP address>\n", argv[0]);
        exit(-1);
    }

    urlArgs url_args;
    if (url_parse(argv[1], &url_args) != 0) {
        perror("url_parse()");
        exit(-1);
    }
    char answer[500];
    socketA = createSocket(url_args.ip, 21);
    if (socketA < 0 || read_answer(socketA, answer) != 220) {
        printf("Socket to '%s' and port %d failed\n", url_args.ip, 21);
        exit(-1);
    }

    if (connection_server(socketA, url_args.user, url_args.password) < 0) {
        printf("Error connecting to server\n");
        exit(-1);
    }
    printf("Connected to server\n");
    if (passive_mode(socketA, ip, &port) != 0) {
        printf("Error entering passive mode\n");
        exit(-1);
    }

    socketB = createSocket(ip, port);
    if (requestResource(socketA, url_args.path) != 150) {
        printf("Error requesting resource\n");
        exit(-1);
    }

    if (getResource(socketA, socketB, url_args.fileName) != 226) {
        printf("Error getting resource\n");
        exit(-1);
    }
    printf("Resource downloaded\n");
    if (closeConnection(socketA, socketB) != 0) {
        printf("Error closing connection\n");
        exit(-1);
    }

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
        if (password != NULL) {
            strcpy(url_parse->password, password);
        } else {
            strcpy(url_parse->password, "pass");
        }
        strcpy(url_parse->host, host);
    } else {
        strcpy(url_parse->user, "anonymous");
        strcpy(url_parse->password, "pass");
        strcpy(url_parse->host, middle);
    }

    char *fileName = final, *p;
    for (p = final; *p; p++) {
        if (*p == '/' || *p == '\\' || *p == ':') {
            fileName = p + 1;
        }
    }

    strcpy(url_parse->fileName, fileName);
    strcpy(url_parse->path, final);

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

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

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
    printf("> %s", message);
    return 0;
}

int connection_server(int socket, char *user, char *password) {
    char response[500];
    char user_message[5 + strlen(user) + 1];
    sprintf(user_message, "user %s\n", user);
    write_to_server(socket, user_message);
    if (read_answer(socket, response) != 331) return -1;
    char pass_message[5 + strlen(password) + 1];
    sprintf(pass_message, "pass %s\n", password);
    write_to_server(socket, pass_message);

    if (read_answer(socket, response) != 230) return -1;
    printf("Logged in\n");
    return 0;
}

int read_answer(int socket, char *response) {
    FILE *fp = fdopen(socket, "r");
    if (fp == NULL) {
        perror("fdopen()");
        exit(-1);
    }
    char *line;
    size_t len = 0;
    int code;
    while ((getline(&line, &len, fp)) > 0) {
        strncat(response, line, len - 1);
        if (line[3] == ' ') {
            sscanf(line, "%d", &code);
            break;
        }
    }

    free(line);
    printf("< %s\n", response);
    return code;
}

int passive_mode(int socket, char *ip, int *port) {
    char *message = "pasv\n";
    char response[500];
    int ip1, ip2, ip3, ip4, port1, port2;
    write_to_server(socket, message);

    if (read_answer(socket, response) != 227) {
        printf("Error entering passive mode\n");
        return -1;
    }

    strtok(response, "(");
    char *response2 = strtok(NULL, ")");

    sscanf(response2, "%d,%d,%d,%d,%d,%d", &ip1, &ip2, &ip3, &ip4, &port1,
           &port2);
    *port = port1 * 256 + port2;

    // printf("IP: %d.%d.%d.%d\n", ip1, ip2, ip3, ip4);
    sprintf(ip, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
    return 0;
}

int createSocket(char *serverAddress, int serverPort) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";
    size_t bytes;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(serverAddress);
    server_addr.sin_port = htons(serverPort);

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        perror("connect()");
        exit(-1);
    }
    printf("Connected to %s:%d\n", serverAddress, serverPort);
    printf("Socket fd: %d\n", sockfd);
    return sockfd;
}

int requestResource(int socket, char *resource) {
    char fileCommand[5 + strlen(resource) + 1], answer[500];
    sprintf(fileCommand, "retr %s\n", resource);
    write_to_server(socket, fileCommand);
    return read_answer(socket, answer);
}

int getResource(int socketA, int socketB, const char *filename) {
    FILE *fd = fopen(filename, "wb");
    if (fd == NULL) {
        printf("Error opening or creating file '%s'\n", filename);
        exit(-1);
    }

    char buffer[1024];
    ssize_t bytes;
    printf("Starting to download the file with name %s\n", filename);
    while ((bytes = read(socketB, buffer, sizeof(buffer) - 1)) >
           0) {                // ensure buffer is not overflowed
        buffer[bytes] = '\0';  // null terminate the buffer
        if (fwrite(buffer, 1, bytes, fd) != bytes) {
            printf("Error writing data to file\n");
            return -1;
        }
    }

    if (bytes < 0) {
        printf("Error reading from data socket\n");
        return -1;
    }

    printf("Finished downloading the file\n");

    if (fclose(fd) < 0) {
        printf("Error closing file\n");
        return -1;
    }
    char answer[500];
    return read_answer(socketA, answer);
}
int closeConnection(const int socketA, const int socketB) {
    // char answer[500];
    printf("Closing connection\n");
    write_to_server(socketA, "quit\n");
    // if (read_answer(socketA, answer) != 221) return -1;
    return close(socketA) || close(socketB);
}