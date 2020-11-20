#include "chttpprotocol.h"

CHttpProtocol::CHttpProtocol()
{   
    char hname[256] = {0};
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(7777);
    gethostname(hname,sizeof(hname));
    hostent *p = gethostbyname(hname);
    sockAddr.sin_addr = *(IN_ADDR *)p->h_addr_list[0];
}
CHttpProtocol::~CHttpProtocol()
{
    //清理HANDLE

    //清理socket
    closesocket(m_listenSocket);
    WSACleanup();
}
bool CHttpProtocol::StartHttpSrv()
{
    //创建套接字
    m_listenSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
    //套接字绑定
    bind(m_listenSocket,(LPSOCKADDR)&sockAddr,sizeof (struct sockaddr));
    //监听套接字
    listen(m_listenSocket,SOMAXCONN);
    printf("[INFO]Server Listen on %s:7777\n",inet_ntoa(sockAddr.sin_addr));
    //创建ListenThread 线程接受客户机连接请求
    DWORD dwThreadId;
    m_pListThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ListenThread,this,0,&dwThreadId);
}
UINT CHttpProtocol::ListenThread(LPVOID param)
{
    CHttpProtocol *pHttpProtocal = (CHttpProtocol *)param;
    PREQUEST pReq;
    SOCKET socketClient;
    SOCKADDR_IN SockAddr;
    int nLen;
    while (1) {
        nLen = sizeof (SOCKADDR_IN);
        socketClient = accept(pHttpProtocal->m_listenSocket,(LPSOCKADDR)&SockAddr,&nLen);
        PREQUEST pReq = (PREQUEST)malloc(sizeof (REQUEST));
        //初始化REQUEST结构
        pReq->hExit = SockAddr;
        pReq->Socket = socketClient;
        pReq->hFile = INVALID_HANDLE_VALUE;
        pReq->dwRecv = 0;
        pReq->dwSend = 0;
        pReq->pHttpProtocal = pHttpProtocal;
        //创建客户处理线程，处理request
        printf("[INFO]Clinet connecting...\n");
        DWORD dwThreadId;
        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ClientThread,pReq,0,&dwThreadId);
    }
}
UINT CHttpProtocol::ClientThread(LPVOID param)
{
    BYTE buf[1024];
    PREQUEST pReq = (PREQUEST)param;
    CHttpProtocol *pHttpProtocal = (CHttpProtocol *)pReq->pHttpProtocal;
    //接收数据，放入缓冲区
    pHttpProtocal->RecvRequest(pReq,buf,sizeof (buf));
    //分析request信息，判断请求类型，获取Request——URL
    pHttpProtocal->Analyze(pReq,buf);
    pHttpProtocal->SendHeader(pReq);    //发送 200 ok
    //GET
    if(pReq->nMethod == METHOD_GET)
    {
        printf("[INFO]Client %s method GET URL=%s\n",inet_ntoa(pReq->hExit.sin_addr),pReq->szFileName);
        pHttpProtocal->SendFile(pReq);
    }
    pHttpProtocal->Disconnect(pReq);
    delete pReq;
    return 0;
}
//接收数据
int CHttpProtocol::RecvRequest(PREQUEST pReq, PBYTE buf, int bufLen)
{
    int rcvLen;
    rcvLen = recv(pReq->Socket,(char *)buf,bufLen,0);
    if(rcvLen <= 0)
    {
        return -1;
    }
    return 0;
}
//断开连接
int CHttpProtocol::Disconnect(PREQUEST pReq)
{
    return  closesocket(pReq->Socket);

}
//发送问价
void CHttpProtocol::SendBuffer(PREQUEST pReq, PBYTE buf, int len)
{
    send(pReq->Socket,(const char *)buf,len,0);
}
int CHttpProtocol::Analyze(PREQUEST pReq, LPBYTE pBuf)
{
    //分析接受的信息
    char szSeps[] = " \n";
    char *cpToken;
    if(strstr((const char *)pBuf,"..")!=NULL) //防止非法请求
    {
        strcpy(pReq->StatuCodeReason,HTTP_STATUS_BADREQUEST);
        return 1;
    }
    cpToken = strtok((char *)pBuf,szSeps);

    if(!_stricmp(cpToken,"GET"))
    {
        pReq->nMethod = METHOD_GET;
    }
    else {
        strcpy(pReq->StatuCodeReason,HTTP_STATUS_NOTIMPLEMENTED);
        return 1;
    }

    //获取Request——URL
    cpToken = strtok(NULL,szSeps);
    if(cpToken == NULL)
    {
        //返回错误状态吗404
        strcpy(pReq->StatuCodeReason,HTTP_STATUS_BADREQUEST);
        return 1;
    }
    strcpy(pReq->szFileName,m_strRootDir);
    if(strlen(cpToken)>1)
    {
        //把该文件名添加到路径结尾形成完整路径
        strcat(pReq->szFileName,cpToken);
    }else {
        strcat(pReq->szFileName,"index.html");
    }
    return 0;
}

size_t GetFileSize(const std::string& file_name){
    std::ifstream in(file_name.c_str());
    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.close();
    return size; //单位是：byte
}

void CHttpProtocol::SendHeader(PREQUEST pReq)
{
    if(_access(pReq->szFileName,0) < 0)
    {
        return;
    }

    pReq->hFile = CreateFileA(pReq->szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    SYSTEMTIME st;
    GetLocalTime(&st);
    //时间格式转换？？
    size_t length;
    length = GetFileSize(std::string(pReq->szFileName));
//    char curTime[50] = " ";
//    GetCurentTime((char *)curTime);
//    char last_modified[60] = " ";
//    GetLastModified(pReq->hFile,(char *)last_modified);
//    char ContentType[50] = " ";
//   GetContentType(pReq->hFile,(char *)ContentType);
    char Header[2048] = " ";
    //构造响应消息
    sprintf((char *)Header,"HTTP/1.0 %s\r\nServer: %s\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n",
            HTTP_STATUS_OK,"My Http Server",length);
    send(pReq->Socket,Header,strlen(Header),0);
}
void CHttpProtocol::SendFile(PREQUEST pReq)
{
    int n = _access(pReq->szFileName,0);
    if(n<0)
    {
        return;
    }

    static BYTE buf[2048];
    DWORD dwRead;
    BOOL fRet;
    int flag = 1;
    int count = 0;
    //读取数据
    while(1)
    {
        count ++;
        //从file中读取到buff中
        fRet = ReadFile(pReq->hFile,buf,sizeof (buf),&dwRead,NULL);
        int res = WSAGetLastError();
        if(!fRet)
        {
            static char szMsg[512];
            wsprintfA(szMsg,"%s",HTTP_STATUS_SERVERERROR);
            //向客户发出错误信息
            send(pReq->Socket,szMsg,strlen(szMsg),0);
            break;
        }
        if(dwRead == 0)
        {
            break;
        }
        //SendBuffer(pReq,buf,dwRead);
        //printf("[INFO]%d===>%d\n",count,dwRead);
        send(pReq->Socket,(const char *)buf,dwRead,0);
    }
    CloseHandle(pReq->hFile);
}
