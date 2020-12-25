#include "widget.h"

#include <QApplication>
#include <winsock2.h>
#include <stdio.h>

const char url[] = "GET %s HTTP/1.0\r\n\r\n";
const char path[] = "C:\\qt-pro\\web\\browser\\browser\\static\\";

int cutUrl(char *Dest,char *Src)
{
    char tmp[2048] = {0};

    memcpy(tmp,Src,sizeof (tmp));
    char *ptr = strstr(tmp,"href=\"");
    if(ptr == NULL)
        return 0;
    ptr += 6;
    int len = ptr - tmp;

    memcpy(tmp,ptr,sizeof (tmp));
    ptr = strstr(tmp,"\"");
    if(ptr == NULL)
        return 0;
    int l = ptr-tmp;
    if(l > 1)
    {
        memcpy(Dest,tmp,l);
        return len+l;
    } else {
        return len+1;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int urlCount = 0;
    char linkURL[256][64]= {{0}};

    WSAData wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);

    SOCKET b = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
    SOCKADDR_IN Server;
    Server.sin_port = htons(7777);
    Server.sin_family = AF_INET;
    char hname[256] = {0};
    gethostname(hname,sizeof(hname));
    hostent *p = gethostbyname(hname);
    Server.sin_addr = *(IN_ADDR *)p->h_addr_list[0];

    connect(b,(sockaddr *)&Server,sizeof (Server));

    char sndbuf[2048] = {0};
    sprintf(sndbuf,url,"/");
    send(b,sndbuf,strlen(sndbuf),0);

    FILE *f;
    char filename[256] = {0};
    sprintf(filename,"%s%s",path,"index.html");
    f = fopen(filename,"a+");

    char recbuf[2048] = {0};
    int recCount = 0;

    while ((recCount = recv(b,recbuf,sizeof (recbuf),0)) > 0) {
        int u = 0;  //get url len
        while (u < recCount) {
            int res = cutUrl(linkURL[urlCount],recbuf+u);
            if(res == 0)
            {
                break;
            }
            u += res;
            urlCount ++;
            qDebug("len u=%d, url %d, %s\n",u,urlCount,linkURL[urlCount-1]);
        }
        char *p = strstr(recbuf,"<!DOCTYPE html>");
        if(u != 0)
        {
            if(p != NULL) {
                fprintf(f,p);
            }
            else
                fprintf(f,recbuf);
        }
        memset(recbuf,0,sizeof (recbuf));
    }
    fclose(f);

    memset(filename,0,sizeof (filename));
    sprintf(filename,"%s%s",path,linkURL[0]);
    f = fopen(filename,"a+");
    memset(sndbuf,0,sizeof (sndbuf));
    sprintf(sndbuf,url,linkURL[0]);
    b = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);

    connect(b,(sockaddr *)&Server,sizeof (Server));
    send(b,sndbuf,strlen(sndbuf),0);

    memset(recbuf,0,sizeof (recbuf));
    recCount = 0;
    while ((recCount = recv(b,recbuf,sizeof (recbuf),0)) > 0) {
        fprintf(f,recbuf);
        memset(recbuf,0,sizeof (recbuf));
    }
    fclose(f);

    Widget w;
    w.show();
    return a.exec();
}
