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
    //����HANDLE

    //����socket
    closesocket(m_listenSocket);
    WSACleanup();
}
bool CHttpProtocol::StartHttpSrv()
{
    //�����׽���
    m_listenSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
    //�׽��ְ�
    bind(m_listenSocket,(LPSOCKADDR)&sockAddr,sizeof (struct sockaddr));
    //�����׽���
    listen(m_listenSocket,SOMAXCONN);
    printf("[INFO]Server Listen on %s:7777\n",inet_ntoa(sockAddr.sin_addr));
    //����ListenThread �߳̽��ܿͻ�����������
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
        //��ʼ��REQUEST�ṹ
        pReq->hExit = SockAddr;
        pReq->Socket = socketClient;
        pReq->hFile = INVALID_HANDLE_VALUE;
        pReq->dwRecv = 0;
        pReq->dwSend = 0;
        pReq->pHttpProtocal = pHttpProtocal;
        //�����ͻ������̣߳�����request
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
    //�������ݣ����뻺����
    pHttpProtocal->RecvRequest(pReq,buf,sizeof (buf));
    //����request��Ϣ���ж��������ͣ���ȡRequest����URL
    pHttpProtocal->Analyze(pReq,buf);
    pHttpProtocal->SendHeader(pReq);    //���� 200 ok
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
//��������
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
//�Ͽ�����
int CHttpProtocol::Disconnect(PREQUEST pReq)
{
    return  closesocket(pReq->Socket);

}
//�����ʼ�
void CHttpProtocol::SendBuffer(PREQUEST pReq, PBYTE buf, int len)
{
    send(pReq->Socket,(const char *)buf,len,0);
}
int CHttpProtocol::Analyze(PREQUEST pReq, LPBYTE pBuf)
{
    //�������ܵ���Ϣ
    char szSeps[] = " \n";
    char *cpToken;
    if(strstr((const char *)pBuf,"..")!=NULL) //��ֹ�Ƿ�����
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

    //��ȡRequest����URL
    cpToken = strtok(NULL,szSeps);
    if(cpToken == NULL)
    {
        //���ش���״̬��404
        strcpy(pReq->StatuCodeReason,HTTP_STATUS_BADREQUEST);
        return 1;
    }
    strcpy(pReq->szFileName,m_strRootDir);
    if(strlen(cpToken)>1)
    {
        //�Ѹ��ļ�����ӵ�·����β�γ�����·��
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
    return size; //��λ�ǣ�byte
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
    //ʱ���ʽת������
    size_t length;
    length = GetFileSize(std::string(pReq->szFileName));
//    char curTime[50] = " ";
//    GetCurentTime((char *)curTime);
//    char last_modified[60] = " ";
//    GetLastModified(pReq->hFile,(char *)last_modified);
//    char ContentType[50] = " ";
//   GetContentType(pReq->hFile,(char *)ContentType);
    char Header[2048] = " ";
    //������Ӧ��Ϣ
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
    //��ȡ����
    while(1)
    {
        count ++;
        //��file�ж�ȡ��buff��
        fRet = ReadFile(pReq->hFile,buf,sizeof (buf),&dwRead,NULL);
        int res = WSAGetLastError();
        if(!fRet)
        {
            static char szMsg[512];
            wsprintfA(szMsg,"%s",HTTP_STATUS_SERVERERROR);
            //��ͻ�����������Ϣ
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
