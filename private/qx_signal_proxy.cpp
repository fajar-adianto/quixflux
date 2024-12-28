#include <QtDebug>
#include <QMetaObject>

#include "qx_signal_proxy.h"

QxSignalProxy::QxSignalProxy(QObject *parent)
    : QObject{parent}
{
    // Intentionally left empty.
}

void QxSignalProxy::bind(QObject *source, int signal_index, QQmlEngine *engine, QxDispatcher *dispatcher)
{
    const int member_offset = QObject::staticMetaObject.methodCount();

    QMetaMethod method = source->metaObject()->method(signal_index);

    parameter_types_ = QVector<int>(method.parameterCount());
    parameter_names_ = QVector<QString>(method.parameterCount());
    type = method.name();
    engine_ = engine;
    dispatcher_ = dispatcher;

    for (int i = 0 ; i < method.parameterCount() ; i++) {
        parameter_types_[i] = method.parameterType(i);
        parameter_names_[i] = QString(method.parameterNames().at(i));
    }

    if (!QMetaObject::connect(source, signal_index, this, member_offset, Qt::AutoConnection, 0)) {
        qWarning() << "Failed to bind signal";
    }
}

int QxSignalProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    int method_id = QObject::qt_metacall(_c, _id, _a);

    if (method_id < 0) {
        return method_id;
    }

    if (_c == QMetaObject::InvokeMetaMethod) {

        if (method_id == 0) {
            QVariantMap message;

            for (int i = 0 ; i < parameter_types_.count() ; i++) {
                const int type_id = parameter_types_.at(i);
                QVariant v;

                // Extract parameter value based on its type
                if (type_id == QMetaType::QVariant) {
                    v = *reinterpret_cast<QVariant *>(_a[i + 1]);
                } else if (QMetaType::isRegistered(type_id)) {
                    v= QVariant(QMetaType(type_id), _a[i + 1]);
                } else {
                    qWarning() << "QxSignalProxy: Unhandled parameter type:" << type_id;
                    continue;
                }

                message[parameter_names_.at(i)] = v;
            }

            dispatch(message);
        }
        method_id--;
    }

    return method_id;
}

QxDispatcher *QxSignalProxy::dispatcher() const
{
    return dispatcher_;
}

void QxSignalProxy::setDispatcher(QxDispatcher *dispatcher)
{
    dispatcher_ = dispatcher;
}

void QxSignalProxy::dispatch(const QVariantMap &message)
{
    if (engine_.isNull() || dispatcher_.isNull()) {
        return;
    }

    QJSValue value = engine_->newObject();

    QMapIterator<QString, QVariant> iter(message);
    while (iter.hasNext()) {
        iter.next();
        QJSValue v = engine_->toScriptValue<QVariant>(iter.value());
        value.setProperty(iter.key(), v);
    }

    dispatcher_->dispatch(type, value);
}
