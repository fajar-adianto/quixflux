#include "qx_object.h"

QxObject::QxObject(QObject *parent)
    : QObject{parent}
{
    // Intentionally left empty.
}

QQmlListProperty<QObject> QxObject::children()
{
    return QQmlListProperty<QObject>(qobject_cast<QObject *>(this), &children_);
}
