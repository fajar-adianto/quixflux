#include <QtCore>
#include <QtQml>
#include <QVariant>
#include <QJSValue>
#include <QPointer>

#include "qx_dispatcher.h"
#include "private/quix_functions.h"

/*!
   \qmltype QxDispatcher
   \inqmlmodule QuixFlux
   \brief Message Dispatcher

    Dispatcher is a component for delivering action message.

    Usually you don't need to declare a Dispatcher component by yourself beside writing test case.
    It is suggested to use QxAppDispatcher directly.
 */

/*!
    \qmlsignal QxDispatcher::dispatched(string type, QJSValue message)

    This signal is emitted when an action message is ready to dispatch by Dispatcher.

    There has several methods to listen this signal:

    \b{Method 1 - Using Store component}

    It is the suggested method. (Since QuixFlux)

        \code
        import QuixFlux

        QxDispatcher {
          id: dispatcher
        }

        QxStore {
          bindSource: dispatcher

          QxFilter {
            type: ActionTypes.openItem
            onDispatched: {
              // ..
            }
          }
        }
        \endcode


    \b{Method 2 - Using Connections component}

        \code
        import QuixFlux

        QxDispatcher {
          id: dispatcher
        }

        Connections {
            target: dispatcher
            onDispatched: {
                switch (type) {
                    case "OpenItem";
                        // ...
                        break;
                    case "DeleteItem";
                        // ...
                        break;
                }
            }
        }
        \endcode
 */

QxDispatcher::QxDispatcher(QObject *parent)
    : QObject(parent)
    , is_dispatching_(false)
    , next_listener_id_(1)
{
    // Intentionally left empty.
}

/*!
  \qmlmethod QxDispatcher::dispatch(string type, object message)

    Dispatch an action by the Dispatcher. An action consists two parts: The type and message.
    The action may not be dispatched immediately. It will first pass to registered QxMiddleware. They may modify / remove the action.

    If there has more then one pending action,
    it will be placed on a queue to guarantees the order of messages are arrived in sequence to store (First come first served)

    \code
    Store {

      Filter {
        type: ActionTypes.askToRemoveItem
        onDispatched: {
            if (options.skipRemoveConfirmation) {
                AppDispatcher.dispatch(ActionTypes.removeItem, message);
                // Because AppDispatcher is still dispatching ActionTypes.askToRemoveItem,
                // ActionTypes.removeItem will be placed in a queue and will dispatch when
                // all the listeners received current message.
            }
        }
      }
    }

    \endcode
 */
void QxDispatcher::dispatch(QString type, QJSValue message)
{
    QX_PRECHECK_DISPATCH(engine_.data(), type, message);

    auto process = [=](QString type, QJSValue message) {
        if (hook_.isNull()) {
            send(type, message);
        } else {
            hook_->dispatch(type, message);
        }
    };

    if (is_dispatching_) {
        queue_.enqueue(QPair<QString,QJSValue> (type,message) );
        return;
    }

    is_dispatching_ = true;
    process(type,message);

    while (queue_.size() > 0) {
        QPair<QString,QJSValue> pair = queue_.dequeue();
        process(pair.first,pair.second);
    }
    is_dispatching_ = false;
}

/*!
  \qmlmethod QxDispatcher::waitFor(int listenerId)
  \b{This method is deprecated}

  Waits for a callback specifed via the listenerId to be executed before continue execution of current callback.
  You should call this method only by a callback registered via addListener.

 */
void QxDispatcher::waitFor(QList<int> ids)
{
    if (!is_dispatching_ || ids.size() == 0)
        return;

    int id = dispatching_listener_id_;

    waiting_listeners_[id] = true;
    invokeListeners(ids);
    waiting_listeners_.remove(id);
}

/*!
    \qmlmethod int QxDispatcher::addListener(func callback)
    \b{This method is deprecated}

    Registers a callback to be invoked with every dispatched message. Returns a listener ID that can be used with waitFor().
 */
int QxDispatcher::addListener(QJSValue callback)
{
    QxListener *listener = new QxListener(this);
    listener->setCallback(callback);

    return addListener(listener);
}

/*! \fn int QxAppDispatcher::addListener(QxListener *listener)

    It is private API. Do not use it.

 */
int QxDispatcher::addListener(QxListener *listener)
{
    listeners_[next_listener_id_] = listener;
    listener->setListenerId(next_listener_id_);
    return next_listener_id_++;
}

/*!
    \qmlmethod QxDispatcher::removeListener(int listenerId)
    \b{This method is deprecated}

    Remove a callback by the listenerId returned by addListener
 */

void QxDispatcher::removeListener(int id)
{
    if (listeners_.contains(id)) {
        QxListener *listener = listeners_[id].data();
        if (listener->parent() == this) {
            listener->deleteLater();
        }
        listeners_.remove(id);
    }
}


/*! \fn QxAppDispatcher::dispatch(const QString &type, const QVariant &message)

    Dispatch a message with type via the QxAppDispatcher.
    The message will be placed on a queue and delivery via the "dispatched" signal.
    Listeners may listen on the "dispatched" signal directly,
    or using helper components like QxAppListener / QxAppScript to capture signal.
 */

void QxDispatcher::dispatch(const QString &type, const QVariant &message)
{
    if (engine_.isNull()) {
        qWarning() << "QxAppDispatcher::dispatch() - Unexpected error: engine is not available.";
        return;
    }

    QJSValue value = engine_->toScriptValue<QVariant>(message);

    dispatch(type, value);
}


void QxDispatcher::send(QString type, QJSValue message)
{
    dispatching_message_ = message;
    dispatching_message_type_ = type;
    pending_listeners_.clear();
    waiting_listeners_.clear();

    QMapIterator<int, QPointer<QxListener> > iter(listeners_);
    QList<int> ids;
    while (iter.hasNext()) {
        iter.next();
        pending_listeners_[iter.key()] = true;
        ids << iter.key();
    }

    invokeListeners(ids);

    emit dispatched(type,message);
}

void QxDispatcher::invokeListeners(QList<int> ids)
{
    for (int i = 0 ; i < ids.size() ; i++) {
        int next = ids.at(i);

        if (waiting_listeners_.contains(next)) {
            qWarning() << "QxAppDispatcher: Cyclic dependency detected";
        }

        if (!pending_listeners_.contains(next))
            continue;

        pending_listeners_.remove(next);
        dispatching_listener_id_ = next;

        QxListener *listener = listeners_[next].data();

        if (listener) {
            listener->dispatch(this,dispatching_message_type_,dispatching_message_);
        }
    }
}

QxHook *QxDispatcher::hook() const
{
    return hook_;
}

void QxDispatcher::setHook(QxHook *hook)
{
    if (!hook_.isNull()) {
        hook_->disconnect(this);
    }

    hook_ = hook;

    if (!hook_.isNull()) {
        connect(hook_.data(), SIGNAL(dispatched(QString,QJSValue)), this,SLOT(send(QString,QJSValue)));
    }

}

/*! \fn QQmlEngine *QxAppDispatcher::engine() const

    Obtain the associated engine to this dispatcher.

 */

QQmlEngine *QxDispatcher::engine() const
{
    return engine_.data();
}

/*! \fn QxAppDispatcher::setEngine(QQmlEngine *engine)

    Set the associated \a engine. It is private API. Do not call it.

 */

void QxDispatcher::setEngine(QQmlEngine *engine)
{
    engine_ = engine;
}
