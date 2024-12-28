#include <QtDebug>
#include <QMetaObject>
#include <QtQml>

#include "qx_filter.h"
#include "private/quix_functions.h"

/*!
    \qmltype QxFilter
    \brief Add filter rule to QxAppListener

    QxFilter component listens for the parent's dispatched signal, if a dispatched signal match with its type,
    it will emit its own "dispatched" signal. Otherwise, it will simply ignore the signal.

    This component provides an alternative way to filter incoming message which is suitable for making QxStore component.

    Example:

    \code
        pragma Singleton
        import QtQuick
        import QuixFlux
        import "../actions"

        QxAppListener {
            id: store

            property ListModel model: ListModel { }

            QxFilter {
                type: ActionTypes.addTask
                onDispatched: {
                    model.append({task: message.task});
                }
            }
        }
    \endcode

    It is not suggested to use nested QxAppListener in a QxStore component.
    Because nested QxAppListener do not share the same QxAppListener::listenerId,
    it will be difficult to control the order of message reception between store component.

    In contrast, QxFilter share the same listenerId with its parent,
    and therefore it is a solution for above problem.
*/

/*! \qmlsignal Filter::dispatched(string type, object message)
    It is a proxy of parent's dispatched signal.
    If the parent emits a signal matched with the QxFilter::type / QxFilter::types property,
    it will emit this signal.
 */

QxFilter::QxFilter(QObject *parent)
    : QObject{parent}
{
    // Intentionally left empty.
}

/*! \qmlproperty string QxFilter::type
    These types determine the filtering rule for incoming message. Only type matched will emit the "dispatched" signal.

    \code
    AppListener {
        Filter {
            type: "action1"
            onDispatched: {
                // handle the action
            }
        }
    }
    \endcode

    \sa Filter::types
 */

QString QxFilter::type() const
{
    if (types_.size() == 0) {
        return "";
    } else {
        return types_[0];
    }
}

void QxFilter::setType(const QString &type)
{
    types_ = QStringList() << type;
    emit typeChanged();
    emit typesChanged();
}

/*! \qmlproperty array Filter::types
    These types determine the filtering rule for incoming message. Only type matched will emit the "dispatched" signal.

    \code
        AppListener {
            Filter {
                types: ["action1", "action2"]
                onDispatched: {
                    // handle the action
                }
            }
        }
    \endcode

    \sa Filter::type
 */

QStringList QxFilter::types() const
{
    return types_;
}

void QxFilter::setTypes(const QStringList &types)
{
    types_ = types;
}

QQmlListProperty<QObject> QxFilter::children()
{
    return QQmlListProperty<QObject>(qobject_cast<QObject *>(this), &children_);
}

void QxFilter::classBegin()
{
    // Intentionally left empty.
}

void QxFilter::componentComplete()
{
    QObject *object = parent();
    engine_ = qmlEngine(this);

    if (!object) {
        qDebug() << "QxFilter - Disabled due to missing parent.";
        return;
    }

    const QMetaObject *meta = object->metaObject();

    if (meta->indexOfSignal("dispatched(QString,QJSValue)") >= 0) {
        connect(object, SIGNAL(dispatched(QString,QJSValue)), this, SLOT(filter(QString,QJSValue)));
    } else if (meta->indexOfSignal("dispatched(QString,QVariant)") >= 0) {
        connect(object, SIGNAL(dispatched(QString,QVariant)), this, SLOT(filter(QString,QVariant)));
    } else {
        qDebug() << "QxFilter - Disabled due to missing dispatched signal in parent object.";
        return;
    }
}

void QxFilter::filter(QString type, QJSValue message)
{
    if (types_.indexOf(type) >= 0) {
        QX_PRECHECK_DISPATCH(engine_.data(), type, message);
        emit dispatched(type, message);
    }
}

void QxFilter::filter(QString type, QVariant message)
{
    if (types_.indexOf(type) >= 0) {
        QJSValue value = message.value<QJSValue>();
        QX_PRECHECK_DISPATCH(engine_.data(), type, value);

        emit dispatched(type, value);
    }
}
