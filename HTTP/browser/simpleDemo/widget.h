#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "miniblink.h"

class QLineEdit;
class QToolButton;
class widget : public QWidget
{
    Q_OBJECT
public:
    explicit widget(QWidget *parent = 0);
    ~ widget();

protected:
    virtual void keyPressEvent(QKeyEvent*);

private:
    virtual void CreateUI();
    virtual QToolButton* CreateButton(const QString&,const QString&);

private:
    miniblink*  m_pWkeView;
    QLineEdit*  m_leUrl;
};

#endif // WIDGET_H
