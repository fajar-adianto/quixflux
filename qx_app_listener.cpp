#include <QtDebug>
#include <QtQml>

#include "qx_app_listener.h"
#include "qx_app_dispatcher.h"

/*!
    \qmltype QxAppListener
    \inqmlmodule QuixFlux

    QxAppListener is a helper class to listen dispatched signal from QxAppDispatcher.
    It provides an alternative solution other then using Connections component.
    Moreover, it offers a way to control the order of signal delivery through waitFor property.

    Example:

    \code
        // Only listen for specific message
        QxAppListener {
            filter: "messageType1";
            onDispatched: {
              // Your code here
            }
        }

        // Listen for multiple messages.
        QxAppListener {
            onDispatched: {
                switch (type) {
                    case "messageType1":
                        // ...
                        break;
                    case "messageType2":
                        // ...
                        break;
                }
            }
        }

        // Alternative method to listen for multiple messages

        QxAppListener {

          Component.onCompleted: {
            on("messageType1",function() {
               /// Your code here.
            });
            on("messageType2",function() {
               /// Your code here.
            });
          }
        }
    \endcode

 */

/*!
  \qmlsignal AppListener::dispatched(string type, object message)

  It is a proxy of AppDispatcher.dispatched signal.
  If the enabled property is set to false, this signal will not be emitted.
 */

/*! \qmlproperty bool AppListener::enabled

  This property holds whether the listener receives message.
  If it is false, it won't emit "dispatched" signal and trigger callback registered via "on" function.
  By default this is true.

  The value can be controlled by parent component.
  Setting this property directly affects the enabled value of child items.
  When set to false, the enabled values of all child items also become false.
  When set to true,
  the enabled values of child items are returned to true,
  unless they have explicitly been set to false.
 */

QxAppListener::QxAppListener(QQuickItem *parent)
    : QQuickItem{parent}
    , always_on_(false)
    , listener_(nullptr)
    , listener_id_(0)
{
    // Intentionally left empty.
}

QxAppListener::~QxAppListener()
{
    if (!target_.isNull()) {
        target_->removeListener(listener_id_);
    }
}

QObject *QxAppListener::target() const
{
    return target_;
}

void QxAppListener::setTarget(QxDispatcher *target)
{
    if (!target_.isNull()) {
        target_->removeListener(listener_id_);
        listener_->disconnect(this);
        listener_->deleteLater();
        listener_ = 0;
        setListenerId(0);
    }

    target_ = target;

    if (!target_.isNull()) {

        listener_ = new QxListener(this);

        setListenerId(target_->addListener(listener_));

        setListenerWaitFor();

        connect(listener_, SIGNAL(dispatched(QString,QJSValue)),
                this, SLOT(onMessageReceived(QString,QJSValue)));
    }
}

QxAppListener *QxAppListener::on(QString type, QJSValue callback)
{
    QList<QJSValue> list;

    if (mapping_.contains(type)) {
        list = mapping_[type];
    }

    list.append(callback);

    mapping_[type] = list;

    return this;
}

/*! \qmlmethod QxAppListener::removeListener(string type, func callback)

  Remove a listener from the listener array for the specified message.
 */

void QxAppListener::removeListener(QString type, QJSValue callback)
{
    if (!mapping_.contains(type)) {
        return;
    };

    QList<QJSValue> list;
    list = mapping_[type];

    int index = -1;
    for (int i = 0 ; i < list.size() ;i++) {
        if (list.at(i).equals(callback)) {
            index = i;
            break;
        }
    }

    if (index >=0 ) {
        list.removeAt(index);
        mapping_[type] = list;
    }
}

/*! \qmlmethod AppListener::removeAllListener(string type)
   Remove all the listeners for a message with type. If type is empty, it will remove all the listeners.
 */

void QxAppListener::removeAllListener(QString type)
{
    if (type.isEmpty()) {
        mapping_.clear();
    } else {
        mapping_.remove(type);
    }
}

/*! \qmlproperty string QxAppListener::filter

  Set a filter to incoming messages. Only message with type matched with the filter will emit "dispatched" signal.
  If it is not set, it will dispatch every message.
 */

QString QxAppListener::filter() const
{
    return filter_;
}

void QxAppListener::setFilter(const QString &filter)
{
    filter_ = filter;
    emit filterChanged();
}

/*! \qmlproperty array QxAppListener::filters
    Set a list of filter to incoming messages. Only message with type matched by the filters will emit "dispatched" signal.
    If it is not set, it will dispatch every message.
 */
QStringList QxAppListener::filters() const
{
    return filters_;
}

void QxAppListener::setFilters(const QStringList &filters)
{
    filters_ = filters;
    emit filtersChanged();
}

/*! \qmlproperty bool QxAppListener::alwaysOn
    This property holds a value to indicate if the listener should remain listening message when it is not enabled.
 */

bool QxAppListener::alwaysOn() const
{
    return always_on_;
}

void QxAppListener::setAlwaysOn(bool always_on)
{
    always_on_ = always_on;
    emit alwaysOnChanged();
}

/*! \qmlproperty int QxAppListener::listenerId
    The listener ID of this component.
    It could be used with QxAppListener::waitFor / QxAppDispatcher::waitFor to control the order of message delivery.
 */

int QxAppListener::listenerId() const
{
    return listener_id_;
}

void QxAppListener::setListenerId(int listener_id)
{
    listener_id_ = listener_id;
    emit listenerIdChanged();
}

/*! \qmlproperty array QxAppListener::waitFor

    If it is set, it will block the emission of dispatched signal until all the specificed listeners has been invoked.

    Example code:

    \code
        QxAppListener {
          id: listener1
        }

        QxAppListener {
           id: listener2
           waitFor: [listener1.listenerId]
        }
    \endcode

 */

QList<int> QxAppListener::waitFor() const
{
    return wait_for_;
}

void QxAppListener::setWaitFor(const QList<int> &wait_for)
{
    wait_for_ = wait_for;
    setListenerWaitFor();
    emit waitForChanged();
}

void QxAppListener::componentComplete()
{
    QQuickItem::componentComplete();

    QQmlEngine *engine = qmlEngine(this);
    Q_ASSERT(engine);

    QxAppDispatcher* dispatcher = QxAppDispatcher::instance(engine);
    if (!dispatcher) {
        qWarning() << "Unknown error: Unable to access QxAppDispatcher";
    } else {
        setTarget(dispatcher);
    }
}

void QxAppListener::onMessageReceived(QString type, QJSValue message)
{
    if (!isEnabled() && !always_on_)
        return;

    bool dispatch = true;

    QStringList rules = filters_;
    if (!filter_.isEmpty()) {
        rules.append(filter_);
    }

    if (rules.size() > 0) {
        dispatch = false;

        for (int i = 0 ; i < rules.size() ; i++) {
            if (type == rules.at(i)) {
                dispatch = true;
                break;
            }
        }
    }

    if (dispatch) {
        emit dispatched(type,message);
    }

    // Listener registered with on() should not be affected by filter.

    if (!mapping_.contains(type))
        return;

    QList<QJSValue> list = mapping_[type];

    QList<QJSValue> arguments;
    arguments << message;

    for (const auto &value : list) {
        if (value.isCallable()) {
            value.call(arguments);
        }
    }
}

void QxAppListener::setListenerWaitFor()
{
    if (!listener_) {
        return;
    }

    listener_->setWaitFor(wait_for_);
}
