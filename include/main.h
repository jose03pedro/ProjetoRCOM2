#ifndef MAIN_H
#define MAIN_H

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
    char host[256];
    char user[64];
    char password[64];
    char path[256];
    char filename[128];
    char host_name[256];
    char ip[16];
} urlArgs;

// URL parsing
int url_parse(char *url, urlArgs *parsed_url);

// Socket creation and termination
int create_socket(char *ip, int port);
int terminate_connection(const int control_socket, const int data_socket);

// FTP commands
int write_to_server(int socket, char *message);
int read_answer(int socket, char *response);
int connection_server(int socket, char *user, char *password);
int passive_mode(int socket, char *ip, int *port);
int request_file(int connection_socket, char *filename);
int download_file(int control_socket, int data_socket,
                  const char *target_filename);

#endif  // MAIN_H
