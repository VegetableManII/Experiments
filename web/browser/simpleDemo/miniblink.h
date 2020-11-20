#ifndef MINIBLINK_H
#define MINIBLINK_H

#include <QUrl>
#include <QWidget>
#include "miniblink/wke.h"

class miniblink : public QWidget
{
    Q_OBJECT

public:
    miniblink(QWidget *parent = 0);
    ~miniblink();

    virtual bool goBack();
    virtual bool goForward();
    virtual void refreash();
    virtual void setWebFocus(const bool&);
    virtual void load(const QUrl &url);
    virtual void loadHtml(const QString& path);
    virtual void setUrl(const QUrl &url);
    virtual QUrl getUrl() { return m_curUrl; }
    virtual wkeWebView getWebView() { return m_wkeView; }

    virtual QString runJS(const QString& strJS);

signals:
    void loadStartedSignal();
    void downloadUrlSignal(const QString &);
    void urlChangedSignal(const QString &);
    void titleChangedSignal(const QString &);
    void loadingFinishSignal(const bool &,const QString &);

private:
    virtual void InitMiniblink();
    virtual void resizeEvent(QResizeEvent*);

private:
    QUrl       m_curUrl;
    wkeWebView m_wkeView;
};

#endif // MINIBLINK_H
