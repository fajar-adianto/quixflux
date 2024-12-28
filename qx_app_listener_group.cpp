#include "qx_app_listener_group.h"
#include "qx_app_dispatcher.h"
#include "qx_app_listener.h"

QxAppListenerGroup::QxAppListenerGroup(QQuickItem *parent)
    : QQuickItem{parent}
    , listener_id_(0)
    , listener_(nullptr)
{
    // Intentionally left empty.
}

QList<int> QxAppListenerGroup::listenerIds() const
{
    return listener_ids_;
}

void QxAppListenerGroup::setListenerIds(const QList<int> &listener_ids)
{
    listener_ids_ = listener_ids;
    emit listenerIdsChanged();
}

QList<int> QxAppListenerGroup::waitFor() const
{
    return wait_for_;
}

void QxAppListenerGroup::setWaitFor(const QList<int> &wait_for)
{
    wait_for_ = wait_for;
    setListenerWaitFor();
    emit waitForChanged();
}

void QxAppListenerGroup::componentComplete()
{
    QQuickItem::componentComplete();

    QQmlEngine *engine = qmlEngine(this);
    Q_ASSERT(engine);

    QxAppDispatcher *dispatcher = QxAppDispatcher::instance(engine);

    listener_ = new QxListener(this);
    listener_id_ = dispatcher->addListener(listener_);
    setListenerWaitFor();

    QList<int> ids = search(this);
    setListenerIds(ids);
}

QList<int> QxAppListenerGroup::search(QQuickItem *item)
{
    QList<int> res;

    QxAppListener *listener = qobject_cast<QxAppListener *>(item);

    if (listener) {
        res.append(listener->listenerId());
        listener->setWaitFor(QList<int>() << listener_id_);
    }

    QList<QQuickItem *> childs = item->childItems();

    for (int i = 0 ; i < childs.size() ; i++) {
        QList<int> subRes = search(childs.at(i));
        if (subRes.size() > 0) {
            res.append(subRes);
        }
    }
    return res;
}

void QxAppListenerGroup::setListenerWaitFor()
{
    listener_->setWaitFor(wait_for_);
}
