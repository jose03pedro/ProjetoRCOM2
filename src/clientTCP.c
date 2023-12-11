#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 21
#define SERVER_ADDR "ftp.up.pt"

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

void sendToSocket(int sockfd, char buf[]) { /*send a string to the server*/
                                            // default, esqueleto inicial
    size_t bytes = write(sockfd, buf, strlen(buf));
    if (bytes > 0)
        printf("Bytes escritos %ld\n", bytes);
    else {
        perror("write()");
        exit(-1);
    }

    if (close(sockfd) < 0) {
        perror("close()");
        exit(-1);
    }
}
