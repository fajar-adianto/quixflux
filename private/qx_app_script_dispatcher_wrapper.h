#ifndef QX_APP_SCRIPT_DISPATCHER_WRAPPER_H
#define QX_APP_SCRIPT_DISPATCHER_WRAPPER_H

#include "../qx_dispatcher.h"

class QxAppScriptDispatcherWrapper : public QObject
{
    Q_OBJECT
public:
    explicit QxAppScriptDispatcherWrapper(QObject *parent = nullptr);

    QString type() const;
    void setType(const QString &type);

    QxDispatcher *dispatcher() const;
    void setDispatcher(QxDispatcher *dispatcher);

public slots:
    void dispatch(QJSValue arguments);

private:
    QString type_;
    QPointer<QxDispatcher> dispatcher_;
};

#endif // QX_APP_SCRIPT_DISPATCHER_WRAPPER_H
