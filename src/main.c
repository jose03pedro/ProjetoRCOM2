#include "../include/main.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <address to get IP address>\n", argv[0]);
        return -1;
    }

    char ip[20], answer[500];
    int port, control_socket, dataSocket;

    urlArgs url_args;
    if (url_parse(argv[1], &url_args) != 0) {
        fprintf(stderr, "Error parsing URL\n");
        return -1;
    }

    control_socket = create_socket(url_args.ip, 21);
    if (control_socket < 0 || read_answer(control_socket, answer) != 220) {
        fprintf(stderr, "Failed to create socket to '%s' on port %d\n",
                url_args.ip, 21);
        return -1;
    }

    if (connection_server(control_socket, url_args.user, url_args.password) <
        0) {
        fprintf(stderr, "Error connecting to server\n");
        return -1;
    }

    if (passive_mode(control_socket, ip, &port) != 0) {
        fprintf(stderr, "Error entering passive mode\n");
        return -1;
    }

    dataSocket = create_socket(ip, port);
    if (request_file(control_socket, url_args.path) != 150) {
        fprintf(stderr, "Error requesting file\n");
        return -1;
    }

    if (download_file(control_socket, dataSocket, url_args.filename) != 226) {
        fprintf(stderr, "Error downloading file\n");
        return -1;
    }

    if (terminate_connection(control_socket, dataSocket) != 0) {
        fprintf(stderr, "Error closing connection\n");
        return -1;
    }

    printf("File downloaded successfully\n");
    return 0;
}

int url_parse(char *url, urlArgs *url_parse) {
    struct hostent *h;
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

    char *filename = final, *p;
    for (p = final; *p; p++) {
        if (*p == '/' || *p == '\\' || *p == ':') {
            filename = p + 1;
        }
    }

    strcpy(url_parse->filename, filename);
    strcpy(url_parse->path, final);

    if (!strcmp(url_parse->host, "") || !strcmp(url_parse->path, "")) {
        printf("Invalid URL\n");
        return -1;
    }

    if ((h = gethostbyname(url_parse->host)) == NULL) {
        herror("gethostbyname()");
        return -1;
    }

    strcpy(url_parse->host_name, h->h_name);
    strcpy(url_parse->ip, inet_ntoa(*((struct in_addr *)h->h_addr)));

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

    return 0;
}

int create_socket(char *serverAddress, int serverPort) {
    int sockfd;
    struct sockaddr_in server_addr;
    // char buf[] = "Mensagem de teste na travessia da pilha TCP/IP\n";

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(serverAddress);
    server_addr.sin_port = htons(serverPort);

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket()");
        return -1;
    }
    /*connect to the server*/
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        fprintf(stderr, "connect()");
        return -1;
    }
    printf("Connected to %s:%d\n", serverAddress, serverPort);
    return sockfd;
}

int terminate_connection(const int control_socket, const int dataSocket) {
    printf("Closing connection\n");
    if (write_to_server(control_socket, "quit\n") < 0) {
        return -1;
    }
    // char answer[500];
    // if (read_answer(socketA, answer) != 221) return -1;
    return close(control_socket) || close(dataSocket);
}

int write_to_server(int socket, char *message) {
    size_t bytes;
    bytes = write(socket, message, strlen(message));
    if (bytes > 0)
        printf("Bytes written: %ld\n", bytes);
    else {
        fprintf(stderr, "write()");
        return -1;
    }
    printf("> %s", message);
    return 0;
}

int read_answer(int socket, char *response) {
    FILE *fp = fdopen(socket, "r");
    if (fp == NULL) {
        fprintf(stderr, "fdopen()");
        return -1;
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

int connection_server(int socket, char *user, char *password) {
    char response[500];
    char user_message[5 + strlen(user) + 1];  //  'user '= 5; + string pass + \0
    sprintf(user_message, "user %s\n", user);
    if (write_to_server(socket, user_message) < 0) {
        return -1;
    }
    if (read_answer(socket, response) != 331) return -1;
    char pass_message[5 + strlen(password) +
                      1];  // 'pass '= 5; + string pass + \0
    sprintf(pass_message, "pass %s\n", password);
    if (write_to_server(socket, pass_message) < 0) {
        return -1;
    }

    if (read_answer(socket, response) != 230) return -1;
    printf("Logged in\n");
    return 0;
}

int passive_mode(int socket, char *ip, int *port) {
    char *message = "pasv\n";
    char response[500];
    int ip1, ip2, ip3, ip4, port1, port2;
    if (write_to_server(socket, message) < 0) {
        return -1;
    }

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

int request_file(int connectionSocket, char *filename) {
    char fileCommand[5 + strlen(filename) + 1], answer[500];
    sprintf(fileCommand, "retr %s\n", filename);
    if (write_to_server(connectionSocket, fileCommand) < 0) {
        return -1;
    }
    return read_answer(connectionSocket, answer);
}

int download_file(int control_socket, int dataSocket,
                  const char *targetfilename) {
    FILE *fileDescriptor = fopen(targetfilename, "wb");
    if (fileDescriptor == NULL) {
        fprintf(stderr, "Error opening or creating file '%s'\n",
                targetfilename);
        return -1;
    }

    char buffer[1024];
    ssize_t bytesRead;
    printf("Downloading the file called %s\n", targetfilename);
    while ((bytesRead = read(dataSocket, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        if (fwrite(buffer, 1, bytesRead, fileDescriptor) != bytesRead) {
            fprintf(stderr, "Error writing data to file\n");
            fclose(fileDescriptor);
            return -1;
        }
    }

    if (bytesRead < 0) {
        fprintf(stderr, "Error reading from data socket\n");
        fclose(fileDescriptor);
        return -1;
    }

    printf("The file has successfully downloaded\n");

    if (fclose(fileDescriptor) < 0) {
        fprintf(stderr, "Error closing file\n");
        return -1;
    }
    char answer[500];
    return read_answer(control_socket, answer);
}
