#include <tchar.h>
#include <winsock2.h>
#include <stdio.h>


#include "cnodelist.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define IO_RCVALL  _WSAIOW(IOC_VENDOR , 1)
#define BUFFER_SIZE 65535
DWORD dwBufferLen[10];
DWORD dwBufferInLen = 1;
DWORD dwBytesReturned = 0;

struct IPHEADER
{
    unsigned char Version_HeaderLength;
    unsigned char TypeOfService;
    unsigned short TotalLength;
    unsigned short Indentification;
    unsigned short Flags_FragmentOffset;
    unsigned char  TimeToLive;
    unsigned char  Protocal;
    unsigned short HeaderCherksum;
    unsigned long  SourceAddress;
    unsigned long  DestAdderss;
};

int main(int argc, char* argv[]) {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);

    SOCKET sock;
    sock = WSASocket(AF_INET, SOCK_RAW, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

    char localName[256];
    gethostname(localName, 256);
    HOSTENT * pHost;
    pHost = gethostbyname(localName);

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(8000);
    addr_in.sin_addr = *((IN_ADDR *)pHost->h_addr_list[0]);
    bind(sock , (struct sockaddr*)&addr_in, sizeof(addr_in));

    WSAIoctl(sock, IO_RCVALL , &dwBufferInLen, sizeof(dwBufferInLen), dwBufferLen, sizeof(dwBufferLen), &dwBytesReturned, NULL, NULL);

    char url[256] = "jwzx.cqupt.edu.cn";
    HOSTENT *pdestHost = gethostbyname(url);
    struct sockaddr_in des_add_in;

    des_add_in.sin_addr = *((IN_ADDR*)pdestHost->h_addr_list[0]);
    printf("gethostbyname=>目的IP: %s\n", inet_ntoa(des_add_in.sin_addr));

    char buffer[BUFFER_SIZE];
    CNodeList IpList;
    while (true){
        int nPacketSize = recv(sock, buffer, BUFFER_SIZE, 0);
        if (nPacketSize > 0) {
            IPHEADER* pIpHdr;
            pIpHdr = (IPHEADER*)buffer;

            unsigned long addr = pIpHdr->SourceAddress;
            unsigned long desAddr = pIpHdr->DestAdderss;
            unsigned char proto = pIpHdr->Protocal;
            unsigned char *pAddr = (unsigned char*)&addr;
            unsigned char *pDesAddr = (unsigned char*)&desAddr;

            unsigned char headLen = (pIpHdr->Version_HeaderLength % 16) * 4;
            unsigned char tcpTag = 6;
            unsigned char* ptr = (unsigned char*)pIpHdr;


            unsigned int checkaddr = inet_addr(inet_ntoa(des_add_in.sin_addr));


            if (pIpHdr->Protocal == tcpTag && pIpHdr->DestAdderss == checkaddr) {

                ptr += headLen;
                unsigned short *pTcpHdr_Port = (unsigned short*)ptr;
                pTcpHdr_Port++;
                unsigned short desPort = ntohs(*pTcpHdr_Port);

                ptr += 12;
                unsigned char  len= (*(unsigned char*)ptr) >> 4;

                ptr += (len * 4);
                //unsigned char* http = (unsigned char*)ptr;
                printf("源IP： %d.%d.%d.%d\n",pAddr[0],pAddr[1],pAddr[2],pAddr[3]);
                printf("目的IP: %d.%d.%d.%d\n协议 %d\n",pDesAddr[0],pDesAddr[1],pDesAddr[2],pDesAddr[3],proto);
                printf("Length %d\nPort %d\n", headLen , desPort);
                printf("HTTP Content:\n%s",ptr);
                getchar();

            }
        }
        else
        {
            printf("\n\terror:%d", WSAGetLastError());
        }
    }
    WSACleanup();
    printf("over");
}
