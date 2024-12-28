#ifndef QX_LISTENER_H
#define QX_LISTENER_H

#include <QObject>
#include <QJSValue>

class QxDispatcher;

class QxListener : public QObject
{
    Q_OBJECT
public:
    explicit QxListener(QObject *parent = nullptr);

    QJSValue callback() const;

    void setCallback(const QJSValue &callback);

    void dispatch(QxDispatcher *dispatcher, QString type, QJSValue message);

    int listenerId() const;

    void setListenerId(int listener_id);

    QList<int> waitFor() const;

    void setWaitFor(const QList<int> &wait_for);

signals:
    void dispatched(QString type, QJSValue message);

private:
    QJSValue callback_;
    int listener_id_;
    QList<int> wait_for_;
};

#endif // QX_LISTENER_H
