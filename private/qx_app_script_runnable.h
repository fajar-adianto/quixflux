#ifndef QX_APP_SCRIPT_RUNNABLE_H
#define QX_APP_SCRIPT_RUNNABLE_H

#include <QObject>
#include <QJSValue>
#include <QQmlEngine>
#include <QPointer>

/// QxAppScriptRunnable handles registered callback in QxAppScript
class QxAppScriptRunnable : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
public:
    explicit QxAppScriptRunnable(QObject *parent = nullptr);
    ~QxAppScriptRunnable();

    QJSValue script() const;
    void setScript(const QJSValue &script);

    QString type() const;

    void run(QJSValue message);

    QxAppScriptRunnable *next() const;
    void setNext(QxAppScriptRunnable *next);

    void setCondition(QJSValue condition);

    void setEngine(QQmlEngine *engine);

    void release();

    bool isOnceOnly() const;
    void setIsOnceOnly(bool is_once_only);

signals:

public slots:
    QxAppScriptRunnable *then(QJSValue condition, QJSValue value);

private:
    void setType(const QString &type);

    QJSValue script_;
    QString type_;
    QxAppScriptRunnable *next_;
    QPointer<QQmlEngine> engine_;

    QJSValue condition_;
    QJSValue callback_;
    bool is_signal_condition_;
    bool is_once_only_;
};

#endif // QX_APP_SCRIPT_RUNNABLE_H
