#ifndef QX_APP_LISTENER_H
#define QX_APP_LISTENER_H

#include <QQuickItem>

#include "qx_dispatcher.h"

class QxAppListener : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QStringList filters READ filters WRITE setFilters NOTIFY filtersChanged)
    Q_PROPERTY(bool alwaysOn READ alwaysOn WRITE setAlwaysOn NOTIFY alwaysOnChanged)
    Q_PROPERTY(int listenerId READ listenerId WRITE setListenerId NOTIFY listenerIdChanged)
    Q_PROPERTY(QList<int> waitFor READ waitFor WRITE setWaitFor NOTIFY waitForChanged)
    QML_ELEMENT
public:
    explicit QxAppListener(QQuickItem *parent = nullptr);
    ~QxAppListener();

    /// Get the listening target.
    QObject *target() const;

    /// Set the listening target. If the class is constructed by QQmlComponent. It will be set automatically.
    void setTarget(QxDispatcher *target);

    /// Add a listener to the end of the listeners array for the specified message.  Multiple calls passing the same combination of event and listener will result in the listener being added multiple times.
    Q_INVOKABLE QxAppListener *on(QString type, QJSValue callback);

    /// Remove a listener from the listener array for the specified message.
    Q_INVOKABLE void removeListener(QString type, QJSValue callback);

    /// Remove all the listeners for a message with type. If type is empty, it will remove all the listeners.
    Q_INVOKABLE void removeAllListener(QString type = QString());

    /// Get the filter for incoming message
    QString filter() const;

    /// Set a filter to incoming message. Only message with type matched with the filter will emit "dispatched" signal.
    void setFilter(const QString &filter);

    /// Get a list of filter for incoming message
    QStringList filters() const;

    /// Set a list of filter to incoming message. Only message with type matched with the filters will emit "dispatched" signal.
    void setFilters(const QStringList &filters);

    bool alwaysOn() const;
    void setAlwaysOn(bool always_on);

    int listenerId() const;
    void setListenerId(int listener_id);

    QList<int> waitFor() const;
    void setWaitFor(const QList<int> &wait_for);

private:
    virtual void componentComplete();

    Q_INVOKABLE void onMessageReceived(QString type, QJSValue message);

    void setListenerWaitFor();

    QPointer<QxDispatcher> target_;

    QMap<QString, QList<QJSValue>> mapping_;

    QString filter_;
    QStringList filters_;
    bool always_on_;

    int listener_id_;
    QxListener *listener_;

    QList<int> wait_for_;

signals:
    /// It is emitted whatever it has received a dispatched message from AppDispatcher.
    Q_SIGNAL void dispatched(QString type, QJSValue message);

    void filterChanged();

    void filtersChanged();

    void alwaysOnChanged();

    void listenerIdChanged();

    void waitForChanged();
};

#endif // QX_APP_LISTENER_H
