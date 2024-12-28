#include <QQmlEngine>

#include "qx_middlewares_hook.h"
#include "quix_functions.h"

QxMiddlewaresHook::QxMiddlewaresHook(QObject *parent)
    : QxHook{parent}
{
    // Intentionally left empty.
}

void QxMiddlewaresHook::dispatch(QString type, QJSValue message)
{
    if (middlewares_.isNull()) {
        emit dispatched(type , message);
    } else {
        next(-1, type , message);
    }
}

void QxMiddlewaresHook::setup(QQmlEngine *engine, QObject *middlewares)
{
    middlewares_ = middlewares;
    QJSValue m_obj = engine->newQObject(middlewares);
    QJSValue h_obj = engine->newQObject(this);

    QString source = "(function (middlewares, hook) {"
                     "  function create(senderIndex) {"
                     "    return function (type, message) {"
                     "      hook.next(senderIndex, type , message);"
                     "    }"
                     "  }"
                     "  var data = middlewares.data;"
                     "  for (var i = 0 ; i < data.length; i++) {"
                     "    var m = data[i];"
                     "    m._nextCallback = create(i);"
                     "  }"
                     "})";

    QJSValue function = engine->evaluate(source);

    QJSValueList args;
    args << m_obj;
    args << h_obj;

    QJSValue ret = function.call(args);

    if (ret.isError()) {
        QuixFlux::printException(ret);
    }

    source = "(function (middlewares, hook) {"
             "  return function invoke(receiverIndex, type , message) {"
             "     if (receiverIndex >= middlewares.data.length) {"
             "       hook.resolve(type, message);"
             "       return;"
             "     }"
             "     var m = middlewares.data[receiverIndex];"
             "     if (m.filterFunctionEnabled && m.hasOwnProperty(type) && typeof m[type] === \"function\") { "
             "       m[type](message);"
             "     } else if (m.hasOwnProperty(\"dispatch\") && typeof m.dispatch === \"function\") {"
             "       m.dispatch(type, message);"
             "     } else {"
             "       invoke(receiverIndex + 1,type, message);"
             "     }"
             "  }"
             "})";

    function = engine->evaluate(source);
    invoke_ = function.call(args);
    if (invoke_.isError()) {
        QuixFlux::printException(invoke_);
    }
}

void QxMiddlewaresHook::next(int sender_index, QString type, QJSValue message)
{
    QJSValueList args;

    args << QJSValue(sender_index + 1);
    args << QJSValue(type);
    args << message;
    QJSValue result = invoke_.call(args);
    if (result.isError()) {
        QuixFlux::printException(result);
    }
}

void QxMiddlewaresHook::resolve(QString type, QJSValue message)
{
    emit dispatched(type, message);
}
