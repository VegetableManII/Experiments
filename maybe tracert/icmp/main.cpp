#include <winsock2.h>
#include <ws2tcpip.h>
#include "Def.h"

int testmain(int argc,char *argv[])
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    u_long ulDestIP = inet_addr(argv[1]);

    if (ulDestIP == INADDR_NONE)
    {
         //转化不成功
        hostent * pHostent = gethostbyname(argv[1]);
        if (pHostent)
        {
            ulDestIP = (*(in_addr *)pHostent->h_addr_list[0]).S_un.S_addr;
        }
        else
        {
            WSACleanup();
            return -1;
        }
    }

    sockaddr_in destSockAddr;
    memset(&destSockAddr, 0, sizeof(destSockAddr));
    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_addr.S_un.S_addr = ulDestIP;

    //创建原始套接字
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);

    int iTimeout = 1000;
    setsockopt(sockRaw, SOL_SOCKET,SO_RCVTIMEO,(char *)(&iTimeout), sizeof(iTimeout));
    setsockopt(sockRaw, SOL_SOCKET,SO_SNDTIMEO,(char *)(&iTimeout), sizeof(iTimeout));

    char IcmpSendBuf[sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE]; //发送缓冲区
    ICMP_HEADER* pIcmpHeader = (ICMP_HEADER *)IcmpSendBuf;

    pIcmpHeader->type = ICMP_ECHO_REQUEST;
    pIcmpHeader->code = 0;
    pIcmpHeader->id = (USHORT)GetCurrentProcessId();
    memset(IcmpSendBuf + sizeof(ICMP_HEADER), 0, DEF_ICMP_DATA_SIZE);  //数据字段

    USHORT usSeqNo = 0;
    int iTTL = 1;
    BOOL bReachDestHost = FALSE;
    int iMaxHop = DEF_MAX_HOP;
    DECODE_RESULT DecodeResult;

    while (!bReachDestHost && iMaxHop--)
    {
        //设置IP报头的TTL字段
        int res = setsockopt(sockRaw, IPPROTO_IP, IP_TTL , (const char *)&iTTL, sizeof(iTTL));
        cout << setw(3) << iTTL << flush;
        //填充ICMP报文中每次发送变化的字段
        ((ICMP_HEADER*)IcmpSendBuf)->cksum = 0;
        ((ICMP_HEADER*)IcmpSendBuf)->seq = htons(usSeqNo++);
        ((ICMP_HEADER*)IcmpSendBuf)->cksum = checksum((USHORT*)IcmpSendBuf, sizeof(ICMP_HEADER));
        //记录序列号和当前时间
        DecodeResult.usSeqNo = ((ICMP_HEADER *)IcmpSendBuf)->seq;
        DecodeResult.dwRoundTripTime = GetTickCount();

        //发送ICMP回显请求消息
        sendto(sockRaw, IcmpSendBuf, sizeof(IcmpSendBuf), 0, (sockaddr *)&destSockAddr, sizeof(destSockAddr));

        /*
            接受处理

        */
        sockaddr_in from;
        int iFromLen = sizeof(from);
        int iReadDataLen;

        char IcmpRecvBuf[MAX_ICMP_PACKET_SIZE]; //接收缓冲区
        memset(IcmpRecvBuf,0,MAX_ICMP_PACKET_SIZE);

        while (1)
        {
            iReadDataLen = recvfrom(sockRaw, IcmpRecvBuf, MAX_ICMP_PACKET_SIZE, 0, (sockaddr *)&from, &iFromLen);
            if (iReadDataLen != SOCKET_ERROR)
            {
                if (DecodeIcmpResponse(IcmpRecvBuf, iReadDataLen, DecodeResult))
                {
                    //到达目的地，退出循环
                    if(DecodeResult.dwIPaddr.S_un.S_addr == destSockAddr.sin_addr.S_un.S_addr)
                        bReachDestHost = TRUE;
                    //打印IP地址
                    cout << '\t' << inet_ntoa(DecodeResult.dwIPaddr) << endl;
                    break;
                }
            }
            else if (WSAGetLastError() == WSAETIMEDOUT)
            {
                cout << setw(9) << '*' << '\t' << "Response time out" << endl;
                break;
            }
            else
            {
                //recvfrom错误，错误处理
                cout <<"\nerror:\t" << WSAGetLastError() << endl;
            }
        }
        iTTL++;
    }

}
