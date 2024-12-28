#ifndef QX_STORE_H
#define QX_STORE_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlListProperty>

#include "qx_action_creator.h"
#include "qx_dispatcher.h"

class QxStore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *bindSource READ bindSource WRITE setBindSource NOTIFY bindSourceChanged)
    Q_PROPERTY(QQmlListProperty<QObject> children READ children)
    Q_PROPERTY(QQmlListProperty<QObject> redispatchTargets READ redispatchTargets)
    Q_PROPERTY(bool filterFunctionEnabled MEMBER filter_function_enabled_ NOTIFY filterFunctionEnabledChanged)
    Q_CLASSINFO("DefaultProperty", "children")
    QML_ELEMENT
public:
    explicit QxStore(QObject *parent = nullptr);

    QQmlListProperty<QObject> children();

    QObject *bindSource() const;
    void setBindSource(QObject *source);

    QQmlListProperty<QObject> redispatchTargets();

public slots:
    void dispatch(QString type, QJSValue message = QJSValue());

    void bind(QObject *source);

protected:
    void classBegin();
    void componentComplete();

private:
    QObjectList children_;

    QPointer<QObject> bind_source_;

    QPointer<QxActionCreator> action_creator_;

    QPointer<QxDispatcher> dispatcher_;

    QObjectList redispatch_targets_;

    bool filter_function_enabled_;

private slots:
    void setup();

signals:
    void dispatched(QString type, QJSValue message);

    void bindSourceChanged();

    void filterFunctionEnabledChanged();

};

#endif // QX_STORE_H
