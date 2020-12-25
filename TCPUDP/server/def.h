#ifndef DEF_H
#define DEF_H

#include <winsock2.h>
#include <windows.h>

#define MAX_CLIENT 10
#define MAX_BUF_SIZE 65535
#define UDPSRV_PORT 2345

char ServerTCPBuf[256];
char ServerUDPBuf[256];

struct TcpThreadParam {
    SOCKET socket;
    sockaddr_in addr;
    int cnum;
};
typedef struct Clients {
    bool isExist;
    SOCKADDR_IN client;
}CLIENTS;


#endif // DEF_H
