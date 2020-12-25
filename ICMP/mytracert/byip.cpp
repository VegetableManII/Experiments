#include <winsock2.h>
#include <ws2tcpip.h>
#include "Def.h"

int main(int argc,char *argv[])
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

    //创建原始套接字
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
    //cout <<"\nerror:\t" << WSAGetLastError() << endl;

    char localName[256];
    gethostname(localName, 256);
    HOSTENT * pHost;
    pHost = gethostbyname(localName);

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr = *((IN_ADDR *)pHost->h_addr_list[0]);

    sockaddr_in destSockAddr;
    memset(&destSockAddr, 0, sizeof(destSockAddr));
    destSockAddr.sin_family = AF_INET;
    destSockAddr.sin_addr.S_un.S_addr = ulDestIP;

    int iTimeout = 1000;
    BOOL blnFlag = TRUE;
    setsockopt(sockRaw,IPPROTO_IP,IP_HDRINCL,(char *)&blnFlag, sizeof (blnFlag));
    setsockopt(sockRaw, SOL_SOCKET,SO_RCVTIMEO,(char *)(&iTimeout), sizeof(iTimeout));
    setsockopt(sockRaw, SOL_SOCKET,SO_SNDTIMEO,(char *)(&iTimeout), sizeof(iTimeout));

    char IpSendBuf[sizeof (IP_HEADER) + sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE]; //发送缓冲区
    IP_HEADER * pIpHeader = (IP_HEADER *)IpSendBuf;
    ICMP_HEADER* pIcmpHeader = (ICMP_HEADER *)(IpSendBuf+sizeof (IP_HEADER));

    pIpHeader->hdrlen_ver = 0x45;
    pIpHeader->tos = 0x00;
    pIpHeader->total_len = 0x3C00;  //长度先设置为0
    pIpHeader->identifier = 0x1000;
    pIpHeader->frag_and_flags = 0x0000;
    pIpHeader->ttl = 0x00;
    pIpHeader->service = 0x01;
    pIpHeader->checksum = 0x0000;
    pIpHeader->sourceIP = addr_in.sin_addr.S_un.S_addr;
    pIpHeader->destIP = destSockAddr.sin_addr.S_un.S_addr;

    pIcmpHeader->type = ICMP_ECHO_REQUEST;
    pIcmpHeader->code = 0;
    pIcmpHeader->id = (USHORT)GetCurrentProcessId();
    memset(IpSendBuf + sizeof (IP_HEADER) + sizeof(ICMP_HEADER), 0, DEF_ICMP_DATA_SIZE);  //数据字段

    USHORT usSeqNo = 0;
    int iTTL = 1;
    BOOL bReachDestHost = FALSE;
    int iMaxHop = DEF_MAX_HOP;
    DECODE_RESULT DecodeResult;

    while (!bReachDestHost && iMaxHop--)
    {
        //设置IP报头的TTL字段
        //int res = setsockopt(sockRaw, IPPROTO_IP, IP_TTL , (const char *)&iTTL, sizeof(iTTL));
        pIpHeader->ttl = iTTL;
        cout << setw(3) << iTTL << flush;
        //填充ICMP报文中每次发送变化的字段
        pIcmpHeader->cksum = 0;
        pIcmpHeader->seq = htons(usSeqNo++);
        pIpHeader->checksum = checksum((USHORT *)IpSendBuf,sizeof (IP_HEADER));
        pIcmpHeader->cksum = checksum((USHORT*)(IpSendBuf + sizeof (IP_HEADER)), sizeof(ICMP_HEADER));
        //记录序列号和当前时间
        DecodeResult.usSeqNo = ((ICMP_HEADER *)(IpSendBuf + sizeof (IP_HEADER)))->seq;
        DecodeResult.dwRoundTripTime = GetTickCount();

        //发送ICMP回显请求消息
        sendto(sockRaw, IpSendBuf, sizeof(IpSendBuf), 0, (sockaddr *)&destSockAddr, sizeof(destSockAddr));
        //cout <<"\nerror:\t" << WSAGetLastError() << endl;
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
