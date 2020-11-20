#include "miniblink.h"
#include <QResizeEvent>
#include <QDebug>
#include <QDir>

// miniblink 回调函数
void OnURLChangedCallback(wkeWebView webView, void* param, const wkeString url)
{
    Q_UNUSED(webView)
    miniblink *view = static_cast<miniblink*>(param);
    if(view){
        view->urlChangedSignal(QString::fromWCharArray(wkeToStringW(url)));
    }
    //qDebug()<<Q_FUNC_INFO<< QString::fromWCharArray(wkeToStringW(url));
}
void OnTitleChangedCallback(wkeWebView webView, void* param, const wkeString title)
{
    Q_UNUSED(webView)
    miniblink *view = static_cast<miniblink*>(param);
    if(view){
        view->titleChangedSignal(QString::fromWCharArray(wkeToStringW(title)));
    }
    //qDebug()<<Q_FUNC_INFO<< QString::fromWCharArray(wkeToStringW(title));
}
bool onDownloadCallback(wkeWebView webView, void* param, const char* url)
{
    Q_UNUSED(webView)
    miniblink *view = static_cast<miniblink*>(param);
    if(view){
        view->downloadUrlSignal(QString(url));
    }
    //qDebug()<<Q_FUNC_INFO<< QString(url);
    return true;
}
void OnLoadingFinishCallback(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
    Q_UNUSED(webView)
    miniblink *view = static_cast<miniblink*>(param);
    if(view){
        bool isOk = (result == WKE_LOADING_SUCCEEDED);
        view->loadingFinishSignal(isOk,QString::fromWCharArray(wkeToStringW(failedReason)));
    }
    //qDebug()<<Q_FUNC_INFO<<QString::fromWCharArray(wkeToStringW(failedReason));
}


miniblink::miniblink(QWidget *parent)
    : QWidget(parent)
    , m_wkeView(0)
{
    InitMiniblink();
}

miniblink::~miniblink()
{
    if(m_wkeView) wkeDestroyWebWindow(m_wkeView);
}

bool miniblink::goBack()
{
    if(m_wkeView){
        if(wkeCanGoBack(m_wkeView)){
            return   wkeGoBack(m_wkeView);
        }
    }
    return false;
}

bool miniblink::goForward()
{
    if(m_wkeView){
        if(wkeCanGoForward(m_wkeView))
            return   wkeGoForward(m_wkeView);
    }
    return false;
}

void miniblink::refreash()
{
    load(m_curUrl);
}

void miniblink::setWebFocus(const bool& focus)
{
    if(m_wkeView){
        if(focus)
            wkeSetFocus(m_wkeView);
        else
            wkeKillFocus(m_wkeView);
    }
}

void miniblink::load(const QUrl &url)
{
    if(!m_wkeView) return;
    m_curUrl = url;
    emit loadStartedSignal();
    wkeLoadURL(m_wkeView, url.toString().toUtf8().data());
}

void miniblink::loadHtml(const QString &path)
{
    if(!m_wkeView) return;
    m_curUrl = QUrl(path);
    emit loadStartedSignal();
    wkeLoadHTML(m_wkeView, path.toUtf8().data());
}

void miniblink::setUrl(const QUrl &url)
{
    if(!m_wkeView) return;
    m_curUrl = url;
    if( url.isEmpty() || !url.isValid() ){
        qWarning("Url is Empty Or is inVaild");
        return;
    }
    emit loadStartedSignal();
    wkeLoadURL(m_wkeView, url.toString().toUtf8().data());
}

QString miniblink::runJS(const QString &strJS)
{
    if (m_wkeView == NULL)
        return tr("wke 对象为空");

    jsValue jsRet = wkeRunJS(m_wkeView,strJS.toUtf8().data());
    jsExecState jsState = wkeGlobalExec(m_wkeView);
    const utf8* result = jsToString(jsState, jsRet);
    return QString::fromUtf8(result);
}

void miniblink::InitMiniblink()
{
    // 获取miniblink路径
    QString dllpath = QDir::currentPath().replace(QDir::currentPath().split("/").last(),"simpleDemo/miniblink/node.dll");

    // 判断miniblink dll是否存在
    if(!QFile::exists(dllpath))
        qWarning()<<dllpath<<" Not found!!!";

    // 设置miniblink dll 路径
    wkeSetWkeDllPath(reinterpret_cast<const wchar_t *>(dllpath.utf16()));
    // 初始化miniblink
    wkeInitialize();

    // 创建miniblink窗口
    m_wkeView =  wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, (HWND)this->winId(), 0, 0, this->width(), this->height());
    // 加载本地默认html文件
    QString htmlpath = QDir::currentPath().replace(QDir::currentPath().split("/").last(),"simpleDemo/miniblink/index.html");
    m_curUrl = QUrl(htmlpath);
    if(!QFile::exists(htmlpath))
        qWarning()<<htmlpath<<" Not found!!!";
    wkeLoadFile(m_wkeView, htmlpath.toUtf8().data());
    wkeShowWindow(m_wkeView, TRUE);
    wkeSetTransparent(m_wkeView, false);
    // 设置显示新窗口属性
    wkeSetNavigationToNewWindowEnable(m_wkeView, true);

    // 设置一些操作的回调函数 转换成qt signal 发送
    wkeOnURLChanged(m_wkeView,OnURLChangedCallback, this);
    wkeOnDownload(m_wkeView, onDownloadCallback, this);
    wkeOnTitleChanged(m_wkeView,OnTitleChangedCallback, this);
    wkeOnLoadingFinish(m_wkeView,OnLoadingFinishCallback, this);

    setMinimumSize(1000,600);
}

void miniblink::resizeEvent(QResizeEvent *event)
{
    if(event->size().isValid() && m_wkeView){
        wkeResizeWindow(m_wkeView, event->size().width(),event->size().height());
    }
}
