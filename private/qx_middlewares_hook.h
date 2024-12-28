#ifndef QX_MIDDLEWARES_HOOK_H
#define QX_MIDDLEWARES_HOOK_H

#include <QPointer>

#include "qx_hook.h"

class QQmlEngine;

class QxMiddlewaresHook : public QxHook
{
    Q_OBJECT
public:
    explicit QxMiddlewaresHook(QObject *parent = nullptr);

    void dispatch(QString type, QJSValue message);
    void setup(QQmlEngine *engine, QObject *middlewares);

public slots:
    void next(int sender_index, QString type, QJSValue message);
    void resolve(QString type, QJSValue message);

private:
    QJSValue invoke_;
    QPointer<QObject> middlewares_;
};

#endif // QX_MIDDLEWARES_HOOK_H
