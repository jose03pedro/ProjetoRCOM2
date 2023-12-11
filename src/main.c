#include <stdio.h>

#include "../include/clientTCP.h"
#include "../include/getip.h"

char *argv1;

int main(int argc, char *argv[]) {
    struct hostent *h;
    urlArgs url_args;
    char ip[500];
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <address to get IP address>\n", argv[0]);
        exit(-1);
    }
    argv1 = argv[1];
    int socketA = createSocket(url_args.ip, 21);
    int socketB = createSocket(ip, 21);
}