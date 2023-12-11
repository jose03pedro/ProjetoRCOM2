#ifndef GETIP_H
#define GETIP_H

#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define h_addr h_addr_list[0]
#define SERVER_PORT 21
#define SERVER_ADDR "ftp.up.pt"
typedef struct {
    char *h_name;  // Official name of the host.
    char **
        h_aliases;  // A NULL-terminated array of alternate names  for the host.
    int h_addrtype;      // The type of address being returned; usually AF_INET.
    int h_length;        // The length of the address in bytes.
    char **h_addr_list;  // A zero-terminated array of network
                         //  addresses for the host.
                         // Host addresses are in Network Byte Order.
} hostent;

typedef struct {
    char host[512];
    char user[256];
    char password[256];
    char path[512];
    char fileName[512];
    char host_name[512];
    char ip[20];
} urlArgs;

void getIP(char *hostName);
int url_parse(char *url, urlArgs *parsedUrl);
int write_to_server(int socket, char *message);
int closeConnection(int socketA, int socketB);
int passive_mode(int socket, char *ip, int *port);
int getResource(int socketA, int socketB, const char *fileName);

int createSocket(char *ip, int port);
int requestResource(int socket, char *resource);
int connection_server(int socket, char *user, char *password);
int read_answer(int socket, char *response);
#endif  // GETIP_H
