#include <iostream>
#include "chttpprotocol.h"
using namespace std;

int main()
{
    WSADATA wsaData;
    WSAStartup(wVersionRequest,&wsaData);
    CHttpProtocol *ser = new CHttpProtocol();
    ser->StartHttpSrv();
    while (1) {

    }
}
