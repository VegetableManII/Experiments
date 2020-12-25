#include <iostream>
#include <processthreadsapi.h>
#include "Server.h"

using namespace std;

int main(int argc,char *argv[])
{
    u_int TcpClientCount = 0;

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
    SOCKET ListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
    char hostname[256] = {0};
    gethostname(hostname,sizeof (hostname));
    hostent *pHostent = gethostbyname(hostname);

    SOCKADDR_IN ListenAddr;
    ListenAddr.sin_family = AF_INET;
    ListenAddr.sin_port = htons((USHORT)atoi(argv[1]));
    ListenAddr.sin_addr = * (IN_ADDR *)pHostent->h_addr_list[0];

    bind(ListenSocket,(sockaddr *)&ListenAddr,sizeof (ListenAddr));

    listen(ListenSocket,SOMAXCONN);
    printf("[Server Info] Server run on %s:%d\n",inet_ntoa(ListenAddr.sin_addr),atoi(argv[1]));

    SOCKET TcpSocket;
    SOCKADDR_IN TcpClientAddr;

    //启动UDP服务器
    DWORD dwThreadId;
    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)UdpServer,NULL,0,&dwThreadId);

    while (TRUE) {
        //接受请求
        int iSockAddrLen = sizeof (sockaddr);
        TcpSocket = accept(ListenSocket,(sockaddr *)&TcpClientAddr,&iSockAddrLen);

        printf("Client%d IP:%s\n",TcpClientCount+1,inet_ntoa(TcpClientAddr.sin_addr));
        //线程爆满
        if(TcpClientCount >= MAX_CLIENT) {
            closesocket(TcpSocket);
            continue;
        }
        TcpClientCount++;
        //传递给线程的结构体变量
        TcpThreadParam Param;
        Param.socket = TcpSocket;
        Param.addr = TcpClientAddr;
        Param.cnum = TcpClientCount;
        DWORD dwThreadId;
        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TcpServeThread,&Param,0,&dwThreadId);
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
