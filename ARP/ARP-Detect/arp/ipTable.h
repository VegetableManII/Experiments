#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

//×óÒÆµØÖ·±ä¸ß ÓÒÒÆµØÖ·±äµÍ
unsigned int* my_ntohs(DWORD addr)
{
    unsigned int* str = (unsigned int *)malloc(4*4);
    str[0] = (addr&0x000000ff);
    str[1] = (addr&0x0000ff00)>>8;
    str[2] = (addr&0x00ff0000)>>16;
    str[3] = (addr & 0xff000000 )>>24;
    return str;
}

int getTable(MIB_IPADDRTABLE *pIPAddrTable)
{

    int i;
    
    //PMIB_IPADDRTABLE pIPAddrTable;
    /*
        typedef struct _MIB_IPADDRTABLE {
            DWORD dwNumEntries;
            MIB_IPADDRROW table[ANY_SIZE];
        } MIB_IPADDRTABLE, *PMIB_IPADDRTABLE;
        typedef MIB_IPADDRROW_XP MIB_IPADDRROW;
        typedef struct _MIB_IPADDRROW_XP {
            DWORD dwAddr;               IPµØÖ·
            IF_INDEX dwIndex;
            DWORD dwMask;               ×ÓÍøÑÚÂë
            DWORD dwBCastAddr;          ¹ã²¥µØÖ·
            DWORD dwReasmSize;
            unsigned short unused1;
            unsigned short wType;       ÍøÂç×´Ì¬
        } MIB_IPADDRROW_XP, *PMIB_IPADDRROW_XP;
    */

    DWORD iPAddr;
    unsigned int *str;
    printf("\tNum Entries: %ld\n", pIPAddrTable->dwNumEntries);
    for (i=0; i < (int) pIPAddrTable->dwNumEntries; i++) {
        iPAddr = pIPAddrTable->table[i].dwAddr;
        str = my_ntohs(iPAddr);
        printf("\n\tIP Address[%d]:     \t%d.%d.%d.%d\n", i, str[0],str[1],str[2],str[3]);
        free(str);
        iPAddr = pIPAddrTable->table[i].dwMask;
        str = my_ntohs(iPAddr);
        printf("\tSubnet Mask[%d]:    \t%d.%d.%d.%d\n", i, str[0],str[1],str[2],str[3]);
        free(str);
        iPAddr = pIPAddrTable->table[i].dwBCastAddr;
        str = my_ntohs(iPAddr);
        printf("\tBroadCast[%d]:      \t%d.%d.%d.%d\n", i, str[0],str[1],str[2],str[3]);
        free(str);

        printf("\tType and State[%d]:", i);
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_PRIMARY)
            printf("\tPrimary IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DYNAMIC)
            printf("\tDynamic IP Address");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DISCONNECTED)
            printf("\tAddress is on disconnected interface");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_DELETED)
            printf("\tAddress is being deleted");
        if (pIPAddrTable->table[i].wType & MIB_IPADDR_TRANSIENT)
            printf("\tTransient address");
        printf("\n");
    }
    return 0;
}
