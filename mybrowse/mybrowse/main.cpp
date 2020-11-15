#include <iostream>
#include <winsock2.h>
using namespace std;

int main(int argc,char *argv[])
{
    WSAData wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);

    SOCKET b = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
    SOCKADDR_IN Server;
    Server.sin_port = htons(7777);
    Server.sin_family = AF_INET;
    char hname[256] = {0};
    gethostname(hname,sizeof(hname));
    hostent *p = gethostbyname(hname);
    Server.sin_addr = *(IN_ADDR *)p->h_addr_list[0];

    connect(b,(sockaddr *)&Server,sizeof (Server));

    char sndbuf[] = "GET   HTTP/1.0\r\n\r\n";
    send(b,sndbuf,strlen(sndbuf),0);

    char recbuf[2048] = {0};
    int recCount = 0;
    while ((recCount = recv(b,recbuf,sizeof (recbuf),0))>= 0) {
        printf("%s",recbuf);
        memset(recbuf,0,sizeof (recbuf));
    }
    return 0;
}
