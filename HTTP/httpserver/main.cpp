#include <iostream>
#include "chttpprotocol.h"
using namespace std;

int main()
{
    WSADATA wsaData;
    WSAStartup(wVersionRequest,&wsaData);
    CHttpProtocol *ser = new CHttpProtocol();
    printf("[INFO]Starting...\n");
    ser->StartHttpSrv();
    while (1) {

    }
}
