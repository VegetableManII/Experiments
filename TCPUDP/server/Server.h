#ifndef SERVER_H
#define SERVER_H
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "def.h"

void TcpServeThread(LPVOID lpPara)
{
    SOCKET TcpSocket = ((TcpThreadParam *)lpPara)->socket;
    SOCKADDR_IN TcpClientAddr = ((TcpThreadParam *)lpPara)->addr;
    int cliNo = ((TcpThreadParam *)lpPara)->cnum;

    sprintf(ServerTCPBuf,"%4d%5d%s",cliNo,UDPSRV_PORT,"START");
    send(TcpSocket,ServerTCPBuf,strlen(ServerTCPBuf),0);

    int TCPBytesRecevied;
    time_t CurSysTime;
    while (TRUE) {
        //读取时间请求命令
        memset(ServerTCPBuf,0,sizeof (ServerTCPBuf));
        TCPBytesRecevied = recv(TcpSocket,ServerTCPBuf,sizeof (ServerTCPBuf),0);

        /**
         * TCPBytesRecevied 0 客户端已经正常关闭连接
         *                  SOCKET_ERROR 客户端状态错误
         */
        if(TCPBytesRecevied == 0 || TCPBytesRecevied == SOCKET_ERROR)
        {
            printf("[TCP INFO] Client%d Leaved...\n",cliNo);
            break;
        }

        //检查命令
        if(strcmp(ServerTCPBuf,"GET CUR TIME")!=0)
        {
            continue;
        }

        time(&CurSysTime);
        memset(ServerTCPBuf,0,sizeof(ServerTCPBuf));
        strftime(ServerTCPBuf,sizeof (ServerTCPBuf),"%Y-%m-%d %H:%M:%S", localtime(&CurSysTime));
        send(TcpSocket,ServerTCPBuf,strlen(ServerTCPBuf),0);
    }
    closesocket(TcpSocket);
}

void UdpServer()
{
    CLIENTS cls[5];
    for(int i = 0;i < 5; i++)
    {
        cls[i].isExist = false;
    }
    SOCKET UDPSrvSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
    char hostname[256];
    gethostname(hostname,sizeof (hostname));

    hostent *pHostent = gethostbyname(hostname);
    SOCKADDR_IN UDPSrvAddr;
    memset(&UDPSrvAddr,0,sizeof (SOCKADDR_IN));
    UDPSrvAddr.sin_family = AF_INET;
    UDPSrvAddr.sin_port = htons(UDPSRV_PORT);
    UDPSrvAddr.sin_addr = *(IN_ADDR *)pHostent->h_addr_list[0];

    bind(UDPSrvSocket,(sockaddr *)&UDPSrvAddr,sizeof (UDPSrvAddr));

    SOCKADDR_IN UDPClientAddr;
    char tmp[256];
    while (TRUE) {
        memset(ServerUDPBuf,0,sizeof (ServerUDPBuf));
        memset(tmp,0,sizeof (tmp));
        //接收回显数据
        int iSockAddrLen = sizeof (sockaddr);
        int res = recvfrom(UDPSrvSocket,ServerUDPBuf,sizeof (ServerUDPBuf),0,(sockaddr *)&UDPClientAddr,&iSockAddrLen);
        //解析
        char pNo[4] = {ServerUDPBuf[0],ServerUDPBuf[1],ServerUDPBuf[2],ServerUDPBuf[3]};
        int No = atoi(pNo);
        if(!cls[No].isExist)
        {
            cls[No].client = UDPClientAddr;
            cls[No].isExist = true;
        }
        //memcpy(cls[No].msg,ServerTCPBuf+sizeof (int),strlen(ServerUDPBuf)-sizeof (int));
        printf("Package From Client%d: %s\n",No,ServerUDPBuf+sizeof (int));
        iSockAddrLen = sizeof (sockaddr);  
        memcpy(tmp,ServerUDPBuf+sizeof (int),strlen(ServerUDPBuf)-sizeof (int));
        sprintf(ServerUDPBuf,"Client%3d sya:%s",No,tmp);
        int i = 0;
        for(;i<5;i++)
        {
            if(cls[i].isExist)
            {
                sendto(UDPSrvSocket,ServerUDPBuf,strlen(ServerUDPBuf),0,(sockaddr *)&cls[i].client,iSockAddrLen);
            }
        }

    }
}

#endif // SERVER_H
