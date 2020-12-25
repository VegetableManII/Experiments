#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include "ipTable.h"

int main(int argc, char *argv[]) {
    MIB_IPADDRTABLE* pIPAddrtable = (MIB_IPADDRTABLE *) malloc (sizeof(MIB_IPADDRTABLE));
    ULONG dwSize = 0, dwRetval = 0;
    if (GetIpAddrTable(pIPAddrtable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
        free(pIPAddrtable);
        pIPAddrtable = (MIB_IPADDRTABLE *) malloc(dwSize);
    }
    if ((dwRetval = GetIpAddrTable(pIPAddrtable, &dwSize, 0)) == NO_ERROR) {
        getTable(pIPAddrtable);
        printf("\n\twhich one?\n");
        int chooose;
        scanf("%d",&chooose);
        ULONG ulHostIp = ntohl(pIPAddrtable->table[chooose].dwAddr);
        //获得主机IP地址和子网掩码
        ULONG ulHostMask = ntohl(pIPAddrtable->table[chooose].dwMask);
        for (ULONG I = 1; I < ( ~ulHostMask); I++) {
            static ULONG uNo = 0;
            HRESULT hr;
            IPAddr ipAddr;
            ULONG pulMac[2];
            ULONG ulLen;
            ipAddr = htonl(I + (ulHostIp & ulHostMask));
            memset(pulMac, 0xff, sizeof(pulMac));
            ulLen = 6;
            hr = SendARP(ipAddr, 0, pulMac, &ulLen);  //探测主机MAC地址
            if (ulLen == 6) {
                uNo++;
                PBYTE pbHexMac = (PBYTE) pulMac;
                unsigned char * strIpAddr = (unsigned char *)(&ipAddr);
                printf("% d:MAC address %02X:%02X:%02X:%02X:%02X:%02X"
                       "IP Address %d.%d.%d.%d\n",
                       uNo, pbHexMac[0], pbHexMac[1], pbHexMac[2],
                       pbHexMac[3], pbHexMac[4], pbHexMac[5], strIpAddr[0],
                       strIpAddr[1], strIpAddr[2], strIpAddr[3]);
            }
        }

    }
    else {
        printf("Call to GetIpAddrTable failed.\n");
    }
    printf("OVER!\n");
    free(pIPAddrtable);
    return 0;
}
