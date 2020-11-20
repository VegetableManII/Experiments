#include "widget.h"
#include <QDebug>
#include <QLayout>
#include <QLineEdit>
#include <QKeyEvent>
#include <QMessageBox>
#include <QToolButton>


// 全局的 js 调用 c++ 的函数
jsValue JS_CALL jsMsgBoxCallback(jsExecState es)
{
    const wchar_t *title = jsToStringW(es, jsArg(es, 0));
    const wchar_t *text  = jsToStringW(es, jsArg(es, 1));

    QMessageBox::warning(NULL,QString::fromWCharArray(title), QString::fromWCharArray(text));

    return jsUndefined();
}


widget::widget(QWidget *parent) :
    QWidget(parent)
  , m_pWkeView(Q_NULLPTR)
{
    setWindowTitle(tr("Miniblink Simple Demo by Dione"));
    CreateUI();
}

widget::~widget()
{
    if(m_pWkeView){
        delete m_pWkeView;
        m_pWkeView = Q_NULLPTR;
    }
}

void widget::keyPressEvent(QKeyEvent *event)
{
    int key  = event->key();
    if(key == Qt::Key_Enter || key == Qt::Key_Return)
    {
        if(!m_leUrl->text().isEmpty())
            m_pWkeView->load(QUrl(m_leUrl->text()));
    }

    QWidget::keyPressEvent(event);
}

void widget::CreateUI()
{
    m_leUrl = new QLineEdit;
    m_leUrl->setFixedHeight(40);
    m_leUrl->setPlaceholderText(tr("搜索或输入网址"));
    QToolButton* pBackBtn = CreateButton("后退","");
    QToolButton* pForwardBtn = CreateButton("前进","");
    QToolButton* pRefreashBtn = CreateButton("刷新","");
    QToolButton* pGOBtn = CreateButton("Load","");
    QToolButton* pRunJSBtn = CreateButton("runJS","");
    QToolButton* pAboutBtn = CreateButton("关于","");
    pRunJSBtn->setStyleSheet("background-color:#7ED321;color:#FFFFFF;height:30px;");

    QHBoxLayout* pTitlelayout = new QHBoxLayout;
    pTitlelayout->addWidget(pBackBtn);
    pTitlelayout->addWidget(pForwardBtn);
    pTitlelayout->addWidget(pRefreashBtn);
    pTitlelayout->addWidget(m_leUrl);
    pTitlelayout->addWidget(pGOBtn);
    pTitlelayout->addWidget(pRunJSBtn);
    pTitlelayout->addWidget(pAboutBtn);

    m_pWkeView = new miniblink;
    // 绑定js调用c++接口 msgBox为js调用接口 jsMsgBoxCallback为C++回调接口 2表示可传入两个参数
    jsBindFunction("msgBox", jsMsgBoxCallback, 2);

    QVBoxLayout* pMainlayout = new QVBoxLayout(this);
    pMainlayout->setContentsMargins(0,6,0,0);
    pMainlayout->addLayout(pTitlelayout);
    pMainlayout->addWidget(m_pWkeView);

    connect(m_pWkeView,&miniblink::urlChangedSignal,[=](QUrl url){
        m_leUrl->setText(url.toString());
    });
    connect(m_pWkeView,&miniblink::loadingFinishSignal,[=](bool ok,const QString &errorString){
        qDebug()<<"loadFinished: "<<ok<<" errostr "<<errorString;
    });
    connect(pBackBtn,&QToolButton::clicked,[=](){
        m_pWkeView->goBack();
    });
    connect(pForwardBtn,&QToolButton::clicked,[=](){
        m_pWkeView->goForward();
    });
    connect(pGOBtn,&QToolButton::clicked,[=](){
        if(!m_leUrl->text().isEmpty())
            m_pWkeView->load(QUrl(m_leUrl->text()));
    });
    connect(pRefreashBtn,&QToolButton::clicked,[=](){
        m_pWkeView->refreash();
    });
    connect(pAboutBtn,&QToolButton::clicked,[=](){
        QMessageBox::about(this,tr("关于"),tr("mini browser link = miniblink\n基于chromium内核轻量级浏览器框架\n    移植作者：龙泉寺扫地僧"));
    });
    connect(pRunJSBtn,&QToolButton::clicked,[=](){
        QString jsStr = QString("var bname = document.getElementById(\"miniblink\").innerHTML;"
                                "alert('Modify Button Name '+bname+' to Miniblink JS Call C++');"
                                "document.getElementById(\"miniblink\").innerHTML=\"Miniblink JS Call C++\";"
                                "document.getElementById(\"p1\").innerHTML=\"伦敦是英国的首都。它是英国人口最多的城市，拥有超过1300万人口的大都市。\";");
        m_pWkeView->runJS(jsStr);
    });
}

QToolButton *widget::CreateButton(const QString &txt, const QString &icon)
{
    QToolButton* pButton = new QToolButton;
    pButton->setText(txt);
    pButton->setToolTip(txt);
    pButton->setStyleSheet("border:none;");

    return pButton;
}
