#include <QMetaType>
#include <QMetaMethod>
#include <QtQml>

#include "qx_action_creator.h"
#include "qx_dispatcher.h"
#include "private/qx_signal_proxy.h"

/*!
    \qmltype QxActionCreator
    \brief Create message from signal then dispatch via QxAppDispatcher

    QxActionCreator is a component that listens on its own signals, convert to message then dispatch via QxAppDispatcher.
    The message type will be same as the signal name. There has no limitation on the number of arguments and their data type.

    For example, you may declare an QxActionCreator based component as:

    \code
        import QtQuick
        import QuixFlux
        pragma singleton

        QxActionCreator {
           signal open(string url);
        }
    \endcode

    It is equivalent to:

    \code
        import QtQuick
        import QuixFlux
        pragma singleton

        Item {
           function open(url) {
             QxAppDispatcher.dispatch(“open”, {url: url});
           }
        }
    \endcode

 */

QxActionCreator::QxActionCreator(QObject *parent)
    : QObject{parent}
{
    // Intentionally left empty.
}

/*! \qmlproperty object QxActionCreator::dispatcher

This property holds the target Dispatcher instance. It will dispatch all the actions to that object.

\code
    QxActionCreator {
        dispatcher: QxDispatcher {
        }
    }
\endcode

The default value is QxAppDispatcher
 */

QxDispatcher *QxActionCreator::dispatcher() const
{
    return dispatcher_;
}

void QxActionCreator::setDispatcher(QxDispatcher *value)
{
    dispatcher_ = value;
    for (int i = 0 ; i < proxy_list_.size();i++) {
        proxy_list_[i]->setDispatcher(dispatcher_);
    }

    emit dispatcherChanged();
}

QString QxActionCreator::genKeyTable()
{
    QStringList imports, header, footer, properties;

    imports << "pragma Singleton"
            << "import QtQuick"
            << "import QuixFlux\n";

    header << "KeyTable {\n";

    footer <<  "}";

    const int member_offset = QObject::staticMetaObject.methodCount();

    const QMetaObject *meta = metaObject();

    int count = meta->methodCount();

    for (int i = member_offset ; i < count ;i++) {
        QMetaMethod method = meta->method(i);
        if (method.name() == "dispatcherChanged") {
            continue;
        }
        if (method.methodType() == QMetaMethod::Signal) {
            properties << QString("    property string %1;\n").arg(QString(method.name()));
        }
    }

    QStringList content;
    content << imports << header << properties << footer;

    return content.join("\n");
}

void QxActionCreator::dispatch(QString type, QJSValue message)
{
    if (!dispatcher_.isNull()) {
        dispatcher_->dispatch(type, message);
    }
}

void QxActionCreator::classBegin()
{
    // Intentionally left empty.
}

void QxActionCreator::componentComplete()
{
    QQmlEngine *engine = qmlEngine(this);

    if (dispatcher_.isNull()) {
        setDispatcher(qobject_cast<QxDispatcher *>(QxAppDispatcher::instance(engine)));
    }

    QxDispatcher *dispatcher = dispatcher_.data();

    const int member_offset = QObject::staticMetaObject.methodCount();

    const QMetaObject *meta = metaObject();

    int count = meta->methodCount();

    for (int i = member_offset ; i < count ;i++) {
        QMetaMethod method = meta->method(i);
        if (method.name() == "dispatcherChanged") {
            continue;
        }

        if (method.methodType() == QMetaMethod::Signal) {
            QxSignalProxy *proxy = new QxSignalProxy(this);
            proxy->bind(this, i, engine, dispatcher);
            proxy_list_ << proxy;
        }
    }
}


