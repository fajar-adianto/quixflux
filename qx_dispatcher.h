#ifndef QX_DISPATCHER_H
#define QX_DISPATCHER_H

#include <QObject>
#include <QVariantMap>
#include <QJSValue>
#include <QQueue>
#include <QPair>
#include <QQmlEngine>
#include <QPointer>

#include "private/qx_listener.h"
#include "private/qx_hook.h"

class QxDispatcher : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit QxDispatcher(QObject *parent = nullptr);

    void dispatch(const QString &type, const QVariant &message);

    int addListener(QxListener *listener);

    QQmlEngine *engine() const;

    void setEngine(QQmlEngine *engine);

    QxHook *hook() const;

    void setHook(QxHook *hook);

public slots:
    /// Dispatch a message via QxDispatcher
    /**
        @param type The type of the message
        @param message The message content
        @reentrant

        Dispatch a message with type via the QxAppDispatcher. Listeners should listen on the "dispatched" signal to be notified.

        Usually, it will emit "dispatched" signal immediately after calling dispatch().
        However, if QxAppDispatcher is still dispatching messages, the new messages will be placed on a queue, and wait until it is finished.
        It guarantees the order of messages are arrived in sequence to listeners
    */
    Q_INVOKABLE void dispatch(QString type, QJSValue message = QJSValue());

    Q_INVOKABLE void waitFor(QList<int> ids);

    Q_INVOKABLE int addListener(QJSValue callback);

    Q_INVOKABLE void removeListener(int id);

private:
    void invokeListeners(QList<int> ids);

    bool is_dispatching_;

    QPointer<QQmlEngine> engine_;

    // Queue for dispatching messages
    QQueue<QPair<QString, QJSValue>> queue_;

    // Next id for listener.
    int next_listener_id_;

    // Registered listener
    QMap<int, QPointer<QxListener>> listeners_;

    // Current dispatching listener id
    int dispatching_listener_id_;

    // Current dispatching message
    QJSValue dispatching_message_;

    // Current dispatching message type
    QString dispatching_message_type_;

    // List of listeners pending to be invoked.
    QMap<int, bool> pending_listeners_;

    // List of listeners blocked in waitFor()
    QMap<int, bool> waiting_listeners_;

    QPointer<QxHook> hook_;

private slots:
    // Invoke listener and emit the dispatched signal
    void send(QString type, QJSValue message);

signals:
    // This signal is emitted when a message is ready to dispatch by QxAppDispatcher.
    Q_SIGNAL void dispatched(QString type, QJSValue message);

};

#endif // QX_DISPATCHER_H
