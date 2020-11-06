#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <processthreadsapi.h>
using namespace std;

struct UdpThreadParam {
    SOCKET socket;
    SOCKADDR_IN addr;
};
void getMsg(LPVOID lpPara) {
    char msg[256];
    int iSockAddrLen = sizeof (sockaddr);
    SOCKET UdpSocket = ((UdpThreadParam *)lpPara)->socket;
    SOCKADDR_IN addr = ((UdpThreadParam *)lpPara)->addr;

    /* HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
    *  COORD loc;
    *  loc.X = 0;
    *  loc.Y = 0;
    *  SetConsoleCursorPosition(hd,loc);
    */

    while(true)
    {
        memset(msg,0,sizeof (msg));
        int n = recvfrom(UdpSocket,msg,sizeof (msg),0,(sockaddr *)&addr,&iSockAddrLen);
        if(n > 0)
        {
            //SetConsoleCursorPosition(hd,loc);
            //CONSOLE_SCREEN_BUFFER_INFO csbi;
            //GetConsoleScreenBufferInfo(hd,&csbi);
            printf("\n\t%s\n",msg);
            //loc.Y += csbi.dwCursorPosition.Y;


        }
    }
}
int main(int argc,char *argv[])
{
    int BytesRecevied;
    char ClientBuf[256];
    char portnum[6];

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
    SOCKET TCPSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);

    SOCKADDR_IN TCPServer;
    TCPServer.sin_family = AF_INET;
    TCPServer.sin_port = htons((USHORT)atoi(argv[2]));
    TCPServer.sin_addr.S_un.S_addr = inet_addr(argv[1]);
    connect(TCPSocket,(sockaddr *)&TCPServer,sizeof (TCPServer));

    memset(portnum,0,6);
    memset(ClientBuf,0,sizeof (ClientBuf));

    //获取UDP端口号和启动命令
    BytesRecevied = recv(TCPSocket,ClientBuf,sizeof (ClientBuf),0);

    SOCKET UDPSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_IP);
    char myID[4] = {ClientBuf[0],ClientBuf[1],ClientBuf[2],ClientBuf[3]};
    memcpy(portnum,ClientBuf+sizeof (int),5);

    int ServerUDPPort = (USHORT)atoi(portnum);
    SOCKADDR_IN UDPServer;
    memset(&UDPServer,0,sizeof (SOCKADDR_IN));
    UDPServer.sin_family = AF_INET;
    UDPServer.sin_port = htons(ServerUDPPort);
    UDPServer.sin_addr.S_un.S_addr = inet_addr(argv[1]);

    int iSockAddrLen = sizeof (sockaddr);

    DWORD dwThreadId;
    UdpThreadParam para;
    para.socket = UDPSocket;
    para.addr = UDPServer;
    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)getMsg,&para,0,&dwThreadId);
    if(strcmp("START",ClientBuf+sizeof (int)+5)!=0)
        return -1;
    while (TRUE) {
//        HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
//        COORD tLoc;
//        COORD loc;
//        loc.X = 0;
//        loc.Y = 40;
//        SetConsoleCursorPosition(hd,loc);

        printf("\nOptions[...]");
        printf("\tGet Time[1]");
        printf("\tGet Reco[2]");
        printf("\tExit[3]\n");
        char UserChoice=0;
        scanf("%c",&UserChoice);
        getchar();
        switch(UserChoice){
        case '1':
            memset(ClientBuf,0,sizeof (ClientBuf));
            sprintf(ClientBuf,"GET CUR TIME");
            send(TCPSocket,ClientBuf,strlen(ClientBuf),0);
            memset(ClientBuf,0,sizeof (ClientBuf));
            recv(TCPSocket,ClientBuf,sizeof (ClientBuf),0);
//            tLoc.X = 0;
//            tLoc.Y = 39;
//            SetConsoleCursorPosition(hd,tLoc);
            printf("\tNow Time:%s\n",ClientBuf);
            break;
        case '2':
            memset(ClientBuf,0,sizeof (ClientBuf));
            cout<<"请输入文本信息，回车发送"<<endl;
            gets(ClientBuf+sizeof (int));
            memcpy(ClientBuf,myID,4);
            sendto(UDPSocket,ClientBuf,strlen(ClientBuf),0,(sockaddr *)&UDPServer,iSockAddrLen);
            //检查合法性
            break;
        case '3':
            closesocket(TCPSocket);
            closesocket(UDPSocket);
            WSACleanup();
            return 0;
        }
    }

}
