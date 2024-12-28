#include <QtDebug>

#include "qx_app_script_dispatcher_wrapper.h"

QxAppScriptDispatcherWrapper::QxAppScriptDispatcherWrapper(QObject *parent)
    : QObject{parent}
{
    // Intentionally left empty.
}

QString QxAppScriptDispatcherWrapper::type() const
{
    return type_;
}

void QxAppScriptDispatcherWrapper::setType(const QString &type)
{
    type_ = type;
}

QxDispatcher *QxAppScriptDispatcherWrapper::dispatcher() const
{
    return dispatcher_;
}

void QxAppScriptDispatcherWrapper::setDispatcher(QxDispatcher *dispatcher)
{
    dispatcher_ = dispatcher;
}

void QxAppScriptDispatcherWrapper::dispatch(QJSValue arguments)
{
    if (dispatcher_.isNull()) {
        qWarning() << "AppScript::Unexcepted condition: AppDispatcher is not present.";
        return;
    }

    dispatcher_->dispatch(type_,arguments);
}
