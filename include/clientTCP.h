#ifndef CLIENTTCP_H
#define CLIENTTCP_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void createSocket(char *serverAddress, int serverPort);

#endif  // CLIENTTCP_H
