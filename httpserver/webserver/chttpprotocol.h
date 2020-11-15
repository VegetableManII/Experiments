#ifndef CHTTPPROTOCOL_H
#define CHTTPPROTOCOL_H
#define METHOD_GET 1
#define wVersionRequest MAKEWORD(2,2)
#include <winsock2.h>
#include <iphlpapi.h>
#include <windows.h>
#include <fileapi.h>
#include <processthreadsapi.h>
#include <time.h>
//#include <msopc.h>
//#include <mfmediaengine.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fstream>

const char HTTP_STATUS_SERVERERROR[] = "Server Error";
const char HTTP_STATUS_OK[] = "200 ok";
const char HTTP_STATUS_BADREQUEST[] = "404 Bad Request";
const char HTTP_STATUS_NOTIMPLEMENTED[] = "501 Not Implemented";
const char m_strRootDir[] = "C:\\webserver\\static";


typedef struct REQUEST {
    SOCKET Socket;
    int nMethod;
    DWORD dwRecv;
    DWORD dwSend;
    HANDLE hFile;
    char hExit;
    char szFileName[_MAX_PATH];
    char postfix[10];
    char StatuCodeReason[100];
    void * pHttpProtocal;
}REQUEST,*PREQUEST;

class CHttpProtocol
{
    static UINT ListenThread(LPVOID param);
    static UINT ClientThread(LPVOID param);
public:
    CHttpProtocol();
    ~CHttpProtocol();
    bool StartHttpSrv();
private:
    int Analyze(PREQUEST pReq,LPBYTE pBuf);
    void SendHeader(PREQUEST pReq);
    void SendFile(PREQUEST pReq);
    int RecvRequest(PREQUEST pReq,PBYTE buf,int bufLen);
    int Disconnect(PREQUEST pReq);
    void SendBuffer(PREQUEST pReq,PBYTE buf,int len);


    sockaddr_in sockAddr;
    SOCKET m_listenSocket;
    HANDLE m_pListThread;
    const char m_hExit = 'Q';
};

#endif // CHTTPPROTOCOL_H
