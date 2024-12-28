#ifndef QX_MIDDLEWARE_LIST_H
#define QX_MIDDLEWARE_LIST_H

#include <QQuickItem>

#include "qx_action_creator.h"

class QxMiddlewareList : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QObject *applyTarget READ applyTarget WRITE setApplyTarget NOTIFY applyTargetChanged)
    QML_ELEMENT
public:
    QxMiddlewareList(QQuickItem *parent = nullptr);

    QObject *applyTarget() const;
    void setApplyTarget(QObject *apply_target);

public slots:
    void apply(QObject *target);

    void next(int sender_index, QString type, QJSValue message);

protected:
    void classBegin();
    void componentComplete();

private:

    QPointer<QQmlEngine> engine_;

    QPointer<QxActionCreator> action_creator_;
    QPointer<QxDispatcher> dispatcher_;
    QJSValue invoke_;

    QPointer<QObject> apply_target_;

private slots:
    void setup();

signals:
    void applyTargetChanged();

};

#endif // QX_MIDDLEWARE_LIST_H
