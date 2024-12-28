#ifndef QX_APP_LISTENER_GROUP_H
#define QX_APP_LISTENER_GROUP_H

#include <QQuickItem>

class QxListener;

class QxAppListenerGroup : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QList<int> listenerIds READ listenerIds WRITE setListenerIds NOTIFY listenerIdsChanged)
    Q_PROPERTY(QList<int> waitFor READ waitFor WRITE setWaitFor NOTIFY waitForChanged)
    QML_ELEMENT
public:
    QxAppListenerGroup(QQuickItem *parent = nullptr);

    QList<int> listenerIds() const;

    void setListenerIds(const QList<int> &listener_ids);

    QList<int> waitFor() const;

    void setWaitFor(const QList<int> &wait_for);

private:
    virtual void componentComplete();

    QList<int> search(QQuickItem *item);

    void setListenerWaitFor();

    QList<int> wait_for_;

    QList<int> listener_ids_;
    int listener_id_;
    QxListener *listener_;

signals:
    void listenerIdsChanged();
    void waitForChanged();
};

#endif // QX_APP_LISTENER_GROUP_H
