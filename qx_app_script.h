#ifndef QX_APP_SCRIPT_H
#define QX_APP_SCRIPT_H

#include <QQmlScriptString>
#include <QQuickItem>

#include "qx_app_dispatcher.h"

class QxListener;
class QxAppScriptRunnable;

class QxAppScript : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQmlScriptString script READ script WRITE setScript NOTIFY scriptChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QString runWhen READ runWhen WRITE setRunWhen NOTIFY runWhenChanged)
    Q_PROPERTY(QJSValue message READ message NOTIFY messageChanged)
    Q_PROPERTY(int listenerId READ listenerId WRITE setListenerId NOTIFY listenerIdChanged)
    Q_PROPERTY(QList<int> waitFor READ waitFor WRITE setWaitFor NOTIFY waitForChanged)
    Q_PROPERTY(bool autoExit READ autoExit WRITE setAutoExit NOTIFY autoExitChanged)
    QML_ELEMENT
public:
    explicit QxAppScript(QQuickItem *parent = nullptr);

    QQmlScriptString script() const;
    void setScript(const QQmlScriptString &script);

    bool running() const;

    QString runWhen() const;
    void setRunWhen(const QString &run_when);

    QJSValue message() const;
    void setMessage(const QJSValue &message);

    int listenerId() const;
    void setListenerId(int listener_id);

    QList<int> waitFor() const;
    void setWaitFor(const QList<int> &wait_for);

    bool autoExit() const;
    void setAutoExit(bool auto_exit);

public slots:
    void exit(int returnCode = 0);
    void run(QJSValue message = QJSValue());

    QxAppScriptRunnable *once(QJSValue condition, QJSValue script);
    void on(QJSValue condition, QJSValue script);

private:
    virtual void componentComplete();
    void abort();
    void clear();
    void setRunning(bool running);

    void setListenerWaitFor();

    QQmlScriptString script_;
    QList<QxAppScriptRunnable *> runnables_;
    QPointer<QxAppDispatcher> dispatcher_;
    QString run_when_;

    bool running_;
    bool processing_;

    int listener_id_;

    bool auto_exit_;

    // The message object passed to run()
    QJSValue message_;
    QxListener *listener_;

    QList<int> wait_for_;

private slots:
    void onDispatched(QString type, QJSValue message);

signals:
    void started();
    void finished(int returnCode);

    void scriptChanged();
    void runningChanged();
    void runWhenChanged();
    void messageChanged();
    void listenerIdChanged();
    void waitForChanged();
    void autoExitChanged();

};

#endif // QX_APP_SCRIPT_H
