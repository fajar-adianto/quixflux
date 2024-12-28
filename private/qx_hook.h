#ifndef QX_HOOK_H
#define QX_HOOK_H

#include <QObject>
#include <QJSValue>

class QxHook : public QObject
{
    Q_OBJECT
public:
    explicit QxHook(QObject *parent = nullptr);

    virtual void dispatch(QString type, QJSValue message) = 0;

signals:
    void dispatched(QString type, QJSValue message);
};

#endif // QX_HOOK_H
