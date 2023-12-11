#include <stdio.h>

#include "../include/clientTCP.h"
#include "../include/getip.h"

int main(int argc, char *argv[]) {
    struct hostent *h;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <address to get IP address>\n", argv[0]);
        exit(-1);
    }
}