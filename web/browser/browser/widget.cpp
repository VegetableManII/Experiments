#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setMinimumSize(1000,800);
    // 获取miniblink路径
    QString dllpath = QDir::currentPath().replace(QDir::currentPath().split("/").last(),"../web/browser/browser/node.dll");

    // 判断miniblink dll是否存在
    if(!QFile::exists(dllpath))
        qDebug("we need node.dll\n");

    // 设置miniblink dll 路径
    wkeSetWkeDllPath(reinterpret_cast<const wchar_t *>(dllpath.utf16()));
    // 初始化miniblink
    wkeInitialize();

    // 创建miniblink窗口
    m_wkeView =  wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, (HWND)this->winId(), 0, 0, 1000, 800);
    // 加载本地默认html文件

    QString htmlpath = QDir::currentPath().replace(QDir::currentPath().split("/").last(),"../web/browser/browser/static/index.html");
    m_curUrl = QUrl(htmlpath);
    if(!QFile::exists(htmlpath))
        qDebug()<<htmlpath<<" Not found!!!";
    wkeLoadFile(m_wkeView, htmlpath.toUtf8().data());
    wkeShowWindow(m_wkeView, TRUE);
    wkeSetTransparent(m_wkeView, false);
    // 设置显示新窗口属性
    wkeSetNavigationToNewWindowEnable(m_wkeView, true);

    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

