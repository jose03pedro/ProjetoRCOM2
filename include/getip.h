#ifndef GETIP_H
#define GETIP_H

#include <arpa/inet.h>
#include <netdb.h>

#define h_addr h_addr_list[0]

struct hostent {
    char *h_name;  // Official name of the host.
    char **
        h_aliases;  // A NULL-terminated array of alternate names  for the host.
    int h_addrtype;  // The type of address being returned; usually AF_INET.
    int h_length;    // The length of the
    //  address in bytes.
    char **h_addr_list;  // A zero-terminated array of network
                         //  addresses for the host.
                         // Host addresses are in Network Byte Order.
};

void getIP(char *hostName);

#endif  // GETIP_H
