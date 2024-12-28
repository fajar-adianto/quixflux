#ifndef QX_APP_DISPATCHER_H
#define QX_APP_DISPATCHER_H

#include <QQmlEngine>

#include "qx_dispatcher.h"

class QxAppDispatcher : public QxDispatcher
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    static QxAppDispatcher *create(QQmlEngine *qml_engine, QJSEngine *js_engine)
    {
        Q_UNUSED(js_engine);

        QxAppDispatcher *result = new QxAppDispatcher();
        result->setEngine(qml_engine);

        return result;
    }

    explicit QxAppDispatcher(QObject *parent = nullptr);

    /// Obtain the singleton instance of AppDispatcher for specific QQmlEngine.
    static QxAppDispatcher *instance(QQmlEngine *engine);

    /// Obtain a singleton object from package for specific QQmlEngine.
    static QObject *singletonObject(QQmlEngine *engine,QString package, int versionMajor, int versionMinor, QString typeName);
};

#endif // QX_APP_DISPATCHER_H
