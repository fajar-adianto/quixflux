#ifndef QX_ACTION_CREATOR_H
#define QX_ACTION_CREATOR_H

#include <QQmlParserStatus>

#include "qx_app_dispatcher.h"

class QxSignalProxy;

class QxActionCreator : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QxDispatcher *dispatcher READ dispatcher WRITE setDispatcher NOTIFY dispatcherChanged)
    QML_ELEMENT
public:
    explicit QxActionCreator(QObject *parent = nullptr);

    QxDispatcher *dispatcher() const;
    void setDispatcher(QxDispatcher *value);

public slots:
    QString genKeyTable();
    void dispatch(QString type, QJSValue message = QJSValue());

protected:
    void classBegin();
    void componentComplete();

private:
    QPointer<QxDispatcher> dispatcher_;
    QList<QxSignalProxy *> proxy_list_;

signals:
    void dispatcherChanged();
};


#endif // QX_ACTION_CREATOR_H
