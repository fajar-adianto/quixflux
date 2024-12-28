#include <QtDebug>

#include "qx_listener.h"
#include "../qx_dispatcher.h"

QxListener::QxListener(QObject *parent)
    : QObject{parent}
    , listener_id_(0)
{
    // Intentionally left empty.
}

QJSValue QxListener::callback() const
{
    return callback_;
}

void QxListener::setCallback(const QJSValue &callback)
{
    callback_ = callback;
}

void QxListener::dispatch(QxDispatcher *dispatcher,QString type, QJSValue message)
{

    if (wait_for_.size() > 0) {
        dispatcher->waitFor(wait_for_);
    }

    if (callback_.isCallable()) {
        QJSValueList args;
        args << type << message;
        QJSValue ret = callback_.call(args);

        if (ret.isError()) {
            QString message = QString("%1:%2: %3: %4")
            .arg(ret.property("fileName").toString())
                .arg(ret.property("lineNumber").toString())
                .arg(ret.property("name").toString())
                .arg(ret.property("message").toString());
            qWarning() << message;
        }
    }

    emit dispatched(type,message);
}

int QxListener::listenerId() const
{
    return listener_id_;
}

void QxListener::setListenerId(int listener_id)
{
    listener_id_ = listener_id;
}

QList<int> QxListener::waitFor() const
{
    return wait_for_;
}

void QxListener::setWaitFor(const QList<int> &wait_for)
{
    wait_for_ = wait_for;
}
