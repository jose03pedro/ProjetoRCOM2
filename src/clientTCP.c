#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int socketA, socketB;

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
    return sockfd;
}

int requestResource(const int socket, char *resource) {
    char fileCommand[5 + strlen(resource) + 1], answer[500];
    sprintf(fileCommand, "retr %s\n", resource);
    write(socket, fileCommand, sizeof(fileCommand));
    return readResponse(socket, answer);
}

int getResource(const int socketA, const int socketB, char *filename) {
    FILE *fd = fopen(filename, "wb");
    if (fd == NULL) {
        printf("Error opening or creating file '%s'\n", filename);
        exit(-1);
    }

    char buffer[1024];
    int bytes;
    printf("Starting to download the file with name %s\n", filename);
    while ((bytes = read(socketB, buffer, sizeof(buffer)))) {
        if (bytes < 0) {
            printf("Error reading from data socket\n");
            return -1;
        }
        if ((bytes = fwrite(buffer, bytes, 1, fd)) < 0) {
            printf("Error writing data to file\n");
            return -1;
        }
    }

    printf("Finished dowloading the file\n");

    if (fclose(fd) < 0) {
        printf("Error closing file\n");
        return -1;
    }

    return readResponse(socketA, buffer);
}

int closeConnection(const int socketA, const int socketB) {
    char answer[500];
    write(socketA, "quit\n", 5);
    if (readResponse(socketA, answer) != 221) return -1;
    return close(socketA) || close(socketB);
}

// void sendToSocket(int sockfd, char buf[]) { /*send a string to the
// server*/
//                                             // default, esqueleto inicial
//     size_t bytes = write(sockfd, buf, strlen(buf));
//     if (bytes > 0)
//         printf("Bytes escritos %ld\n", bytes);
//     else {
//         perror("write()");
//         exit(-1);
//     }

//     if (close(sockfd) < 0) {
//         perror("close()");
//         exit(-1);
//     }
// }
