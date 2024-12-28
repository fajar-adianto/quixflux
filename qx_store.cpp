#include <QtQml>

#include "qx_store.h"
#include "qx_app_dispatcher.h"
#include "private/quix_functions.h"

/*!
   \qmltype QxStore
   \inqmlmodule QuixFlux
   \brief Store Component

    QxStore is a helper item for implement the data “Store” component in a QuixFlux application.
    It could listen from QxActionCreator / QxDispatcher component and redispatch the received action to another store components (e.g children store).

    It is a replacement of QxAppListener component

    Example:

    \code
        import QuixFlux

        QxStore {
            bindSource: QxAppDispatcher

            Filter {
                type: ActionTypes.addItem
                onDispatched: {
                    /// Process
                }
            }
        }
    \endcode

    \b{The order of action delivery:}

    \code
        QxStore {
            id: rootStore
            bindSource: QxAppDispatcher
            property alias page1 : page1

            QxStore {
                id: page1
            }

            QxStore {
                id: page2
            }

            QxFilter {
                id: filter1
            }

        }
    \endcode

    In the example above, the rootStore is bind to AppDispatcher, whatever the dispatcher dispatch an action,
    it will first re-dispatch the action to its children sequentially. Then emit the dispatched signal on itself.
    Therefore, the order of receivers is: page1, page2 then filter1.

    If the redispatchTargets property is set, Store component will also dispatch the received action to the listed objects.

 */

/*!
    \qmlsignal QxStore::dispatched(string type, object message)

    This signal is emitted when a message is received by this store.

    There has two suggested methods to listen this signal:

    Method 1 - Use Filter component

    \code
        Store {
            Filter {
                type: ActionTypes.addItem
                onDispatched: {
                    // process here
                }
            }
        }
    \endcode

    Method 2 - Use filter function

    \code
        Store {
            filterFunctionEnabled: true

            function addItem(message) { }
        }
    \endcode
*/

/*! \qmlproperty bool QxStore::filterFunctionEnabled
    If this property is true, whatever the store component received a new action. Beside to emit a dispatched signal, it will search for a function with a name as the action. If it exists, it will call also call the function.

    \code
        Store {
            filterFunctionEnabled: true

            function addItem(message) { }
        }
    \endcode

    The default value is false.
 */

QxStore::QxStore(QObject *parent)
    : QObject{parent}
    , filter_function_enabled_(false)
{
    // Intentionally left empty.
}

QQmlListProperty<QObject> QxStore::children()
{
    return QQmlListProperty<QObject>(qobject_cast<QObject *>(this), &children_);
}

/*! \qmlproperty object QxStore::bindSource
  This property holds the source of action. It can be an QxActionCreator / QxDispatcher component.
  The default value is null, and it listens to QxAppDispatcher.
 */

QObject *QxStore::bindSource() const
{
    return bind_source_.data();
}

void QxStore::setBindSource(QObject *source)
{
    bind_source_ = source;
    setup();
    emit bindSourceChanged();
}

/*! \qmlproperty array QxStore::redispatchTargets
    By default, the QxStore component redispatch the received action to its children sequentially.
    If this property is set, the action will be re-dispatch to the target objects too.

    \code
        Store {
            id: bridgeStore

            redispatchTargets: [
                SingletonStore1,
                SingletonStore2
            ]
        }
    \endcode
 */

QQmlListProperty<QObject> QxStore::redispatchTargets()
{
    return QQmlListProperty<QObject>(qobject_cast<QObject *>(this), &redispatch_targets_);
}

void QxStore::dispatch(QString type, QJSValue message)
{
    QQmlEngine *engine = qmlEngine(this);
    QX_PRECHECK_DISPATCH(engine, type, message);

    foreach(QObject *child , children_) {
        QxStore *store = qobject_cast<QxStore *>(child);
        if (!store) {
            continue;
        }
        store->dispatch(type, message);
    }

    foreach(QObject *child , redispatch_targets_) {
        QxStore *store = qobject_cast<QxStore *>(child);

        if (!store) {
            continue;
        }
        store->dispatch(type, message);
    }

    if (filter_function_enabled_) {
        const QMetaObject *meta = metaObject();
        QByteArray signature;
        int index;

        signature = QMetaObject::normalizedSignature(QString("%1(QVariant)").arg(type).toUtf8().constData());
        if ( (index = meta->indexOfMethod(signature.constData())) >= 0 ) {
            QMetaMethod method = meta->method(index);
            QVariant value = QVariant::fromValue<QJSValue>(message);

            method.invoke(this,Qt::DirectConnection, Q_ARG(QVariant, value));
        }

        signature = QMetaObject::normalizedSignature(QString("%1()").arg(type).toUtf8().constData());
        if ( (index = meta->indexOfMethod(signature.constData())) >= 0 ) {
            QMetaMethod method = meta->method(index);

            method.invoke(this);
        }
    }

    emit dispatched(type, message);
}

void QxStore::bind(QObject *source)
{
    setBindSource(source);
}

void QxStore::setup()
{
    QxActionCreator *creator = 0;
    QxDispatcher *dispatcher = 0;

    creator = qobject_cast<QxActionCreator *>(bind_source_.data());

    if (creator) {
        dispatcher = creator->dispatcher();
    } else {
        dispatcher = qobject_cast<QxDispatcher *>(bind_source_.data());
    }

    if (action_creator_.data() == creator &&
        dispatcher_.data() == dispatcher) {
        // Nothing changed.
        return;
    }

    if (!action_creator_.isNull() &&
        action_creator_.data() != creator) {
        action_creator_->disconnect(this);
    }

    if (!dispatcher_.isNull() &&
        dispatcher_.data() != dispatcher) {
        dispatcher_->disconnect(this);
    }

    action_creator_ = creator;
    dispatcher_ = dispatcher;

    if (!action_creator_.isNull()) {
        connect(action_creator_.data(),SIGNAL(dispatcherChanged()),
                this,SLOT(setup()));
    }

    if (!dispatcher_.isNull()) {
        connect(dispatcher,SIGNAL(dispatched(QString,QJSValue)),
                this,SLOT(dispatch(QString,QJSValue)));
    }
}
