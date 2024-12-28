#ifndef QX_MIDDLEWARE_H
#define QX_MIDDLEWARE_H

#include <QQuickItem>
#include <QJSValue>

class QxMiddleware : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool filterFunctionEnabled MEMBER filter_function_enabled_ NOTIFY filterFunctionEnabledChanged)
    Q_PROPERTY(QJSValue _nextCallback READ nextCallback WRITE setNextCallback NOTIFY _nextCallbackChanged)
    QML_ELEMENT
public:
    QxMiddleware(QQuickItem* parent = nullptr);

    QJSValue nextCallback() const;
    void setNextCallback(const QJSValue &next_callback);

public slots:
    void next(QString type, QJSValue message = QJSValue());

private:
    bool filter_function_enabled_;
    QJSValue next_callback_;

signals:
    void dispatched(QString type, QJSValue message);
    void filterFunctionEnabledChanged();
    void _nextCallbackChanged();

};

#endif // QX_MIDDLEWARE_H
