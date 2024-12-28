#ifndef QX_OBJECT_H
#define QX_OBJECT_H

#include <QObject>
#include <QQmlListProperty>

class QxObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> children READ children)
    Q_CLASSINFO("DefaultProperty", "children")

public:
    explicit QxObject(QObject *parent = nullptr);

    QQmlListProperty<QObject> children();

private:
    QObjectList children_;
};

#endif // QX_OBJECT_H
