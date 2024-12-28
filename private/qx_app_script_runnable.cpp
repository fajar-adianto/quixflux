#include <QtDebug>
#include <QUuid>

#include "qx_app_script_runnable.h"
#include "qx_app_script_dispatcher_wrapper.h"
#include "../qx_app_dispatcher.h"

QxAppScriptRunnable::QxAppScriptRunnable(QObject *parent)
    : QObject{parent}
    , engine_(0)
    , next_(0)
    , is_signal_condition_(false)
    , is_once_only_(true)
{
    // Intentionally left empty.
}

QxAppScriptRunnable::~QxAppScriptRunnable()
{
    release();
}

QJSValue QxAppScriptRunnable::script() const
{
    return script_;
}

void QxAppScriptRunnable::setScript(const QJSValue &script)
{
    script_ = script;
}

QString QxAppScriptRunnable::type() const
{
    return type_;
}

void QxAppScriptRunnable::run(QJSValue message)
{
    QJSValueList args;
    if (is_signal_condition_ &&
        message.hasProperty("length")) {
        int count = message.property("length").toInt();
        for (int i = 0 ; i < count;i++) {
            args << message.property(i);
        }
    } else {
        args << message;
    }
    QJSValue ret = script_.call(args);

    if (ret.isError()) {
        QString message = QString("%1:%2: %3: %4")
        .arg(ret.property("fileName").toString())
            .arg(ret.property("lineNumber").toString())
            .arg(ret.property("name").toString())
            .arg(ret.property("message").toString());
        qWarning() << message;
    }
}

QxAppScriptRunnable *QxAppScriptRunnable::next() const
{
    return next_;
}

void QxAppScriptRunnable::setNext(QxAppScriptRunnable *next)
{
    next_ = next;
}

void QxAppScriptRunnable::setCondition(QJSValue condition)
{
    condition_ = condition;

    if (condition.isString()) {
        setType(condition.toString());
        is_signal_condition_ = false;
    } else if (condition.isObject() && condition.hasProperty("connect")) {
        Q_ASSERT(!engine_.isNull());

        QString type = QString("QuickFlux.AppScript.%1").arg(QUuid::createUuid().toString());
        setType(type);

        QString generator = "(function(dispatcher) { return function() {dispatcher.dispatch(arguments)}})";
        QxAppDispatcher *dispatcher = QxAppDispatcher::instance(engine_);
        QxAppScriptDispatcherWrapper *wrapper = new QxAppScriptDispatcherWrapper();
        wrapper->setType(type);
        wrapper->setDispatcher(dispatcher);

        QJSValue generator_func = engine_->evaluate(generator);

        QJSValueList args;
        args << engine_->newQObject(wrapper);
        QJSValue callback = generator_func.call(args);

        args.clear();
        args << callback;

        QJSValue connect = condition.property("connect");
        connect.callWithInstance(condition,args);

        callback_ = callback;
        is_signal_condition_ = true;
    } else {
        qWarning() << "AppScript: Invalid condition type";
    }
}

void QxAppScriptRunnable::setEngine(QQmlEngine *engine)
{
    engine_ = engine;
}

void QxAppScriptRunnable::release()
{
    if (!condition_.isNull() &&
        condition_.isObject() &&
        condition_.hasProperty("disconnect")) {

        QJSValue disconnect = condition_.property("disconnect");
        QJSValueList args;
        args << callback_;

        disconnect.callWithInstance(condition_,args);
    }

    condition_ = QJSValue();
    callback_ = QJSValue();
}

bool QxAppScriptRunnable::isOnceOnly() const
{
    return is_once_only_;
}

void QxAppScriptRunnable::setIsOnceOnly(bool is_once_only)
{
    is_once_only_ = is_once_only;
}

QxAppScriptRunnable *QxAppScriptRunnable::then(QJSValue condition, QJSValue value)
{
    QxAppScriptRunnable *runnable = new QxAppScriptRunnable(this);
    runnable->setEngine(engine_.data());
    runnable->setCondition(condition);
    runnable->setScript(value);
    setNext(runnable);
    return runnable;
}

void QxAppScriptRunnable::setType(const QString &type)
{
    type_ = type;
}
