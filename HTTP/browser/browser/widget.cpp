#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setMinimumSize(1000,800);
    // ��ȡminiblink·��
    QString dllpath = QDir::currentPath().replace(QDir::currentPath().split("/").last(),"../web/browser/browser/node.dll");

    // �ж�miniblink dll�Ƿ����
    if(!QFile::exists(dllpath))
        qDebug("we need node.dll\n");

    // ����miniblink dll ·��
    wkeSetWkeDllPath(reinterpret_cast<const wchar_t *>(dllpath.utf16()));
    // ��ʼ��miniblink
    wkeInitialize();

    // ����miniblink����
    m_wkeView =  wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, (HWND)this->winId(), 0, 0, 1000, 800);
    // ���ر���Ĭ��html�ļ�

    QString htmlpath = QDir::currentPath().replace(QDir::currentPath().split("/").last(),"../web/browser/browser/static/index.html");
    m_curUrl = QUrl(htmlpath);
    if(!QFile::exists(htmlpath))
        qDebug()<<htmlpath<<" Not found!!!";
    wkeLoadFile(m_wkeView, htmlpath.toUtf8().data());
    wkeShowWindow(m_wkeView, TRUE);
    wkeSetTransparent(m_wkeView, false);
    // ������ʾ�´�������
    wkeSetNavigationToNewWindowEnable(m_wkeView, true);

    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

