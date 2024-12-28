#include <QtDebug>
#include <QQmlComponent>

#include "qx_app_dispatcher.h"

/*!
   \qmltype QxAppDispatcher
   \inqmlmodule QuixFlux
   \brief Message Dispatcher
   \inherits QxDispatcher

    AppDispatcher is a singleton object for delivering action message.
 */
QxAppDispatcher::QxAppDispatcher(QObject *parent)
    : QxDispatcher{parent}
{
    // Intentionally left empty.
}

QxAppDispatcher *QxAppDispatcher::instance(QQmlEngine *engine)
{
    QxAppDispatcher *dispatcher = qobject_cast<QxAppDispatcher*>(singletonObject(engine,"QuixFlux",1,0,"QxAppDispatcher"));

    return dispatcher;
}

QObject *QxAppDispatcher::singletonObject(QQmlEngine *engine, QString package, int versionMajor, int versionMinor, QString typeName)
{
    QString pattern  = "import QtQuick\nimport %1 %2.%3;QtObject { property var object : %4 }";

    QString qml = pattern.arg(package).arg(versionMajor).arg(versionMinor).arg(typeName);

    QObject *holder = 0;

    QQmlComponent comp (engine);
    comp.setData(qml.toUtf8(),QUrl());
    holder = comp.create();

    if (!holder) {
        qWarning() << QString("QuixFlux: Failed to gain singleton object: %1").arg(typeName);
        qWarning() << QString("Error: ") << comp.errorString();
        return 0;
    }

    QObject *object = holder->property("object").value<QObject *>();
    holder->deleteLater();

    if (!object) {
        qWarning() << QString("QuixFlux: Failed to gain singleton object: %1").arg(typeName);
        qWarning() << QString("Error: Unknown");
    }

    return object;
}
