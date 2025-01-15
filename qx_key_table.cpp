#include <QMetaObject>
#include <QPointF>
#include <QRectF>
#include <QtDebug>

#include "qx_key_table.h"

static QMap<int,QString> createTypes() {
    QMap<int,QString> types;
    types[QMetaType::QString] = "QString";
    types[QMetaType::Int] = "int";
    types[QMetaType::Double] = "qreal";
    types[QMetaType::Bool] = "bool";
    types[QMetaType::QPointF] = "QPointF";
    types[QMetaType::QRectF] = "QRectF";

    return types;
}

/*!
    \qmltype QxKeyTable
    \brief Defines a key table.

    QxKeyTable is an object with properties equal to its key name.
    Once it's construction is completed, it will search all of its string property.
    If it is a string type and not assigned to any value, it will set its value by its name.
    It can be used to create ActionTypes.qml in QuixFlux Application.

    Example
    \code
        QxKeyTable {

            // It will be set to "customField1" in Component.onCompleted callback.
            property string customField1;

            // Since it is already assigned a value, QxKeyTable will not modify this property.
            property string customField2 : "value";
        }
    \endcode
 */

QxKeyTable::QxKeyTable(QObject *parent)
    : QObject{parent}
{
    // Intentionally left empty.
}

QString QxKeyTable::genHeaderFile(const QString &class_name)
{
    QStringList header;
    QStringList clazz;
    bool included_point_header = false;
    bool included_rect_header = false;

    header << "#pragma once";
    header << "#include <QString>\n";

    clazz << QString("class %1 {\n").arg(class_name);
    clazz << "public:\n";

    const QMetaObject *meta = metaObject();

    QMap<int,QString> types = createTypes();

    int count = meta->propertyCount();
    for (int i = 0 ; i < count ; i++) {
        const QMetaProperty p = meta->property(i);

        QString name(p.name());

        if (name == "objectName") {
            continue;
        }

        if (types.contains(p.typeId())) {
            clazz << QString("    static %2 %1;\n").arg(name).arg(types[p.typeId()]);

            if (p.typeId() == QMetaType::QPointF && !included_point_header) {
                included_point_header = true;
                header << "#include <QPointF>";
            } else if (p.typeId() == QMetaType::QRectF && !included_rect_header) {
                included_rect_header = true;
                header << "#include <QRectF>";
            }
        }
    }

    clazz << "};\n";

    QStringList content;
    content << header << clazz;

    return content.join("\n");
}

QString QxKeyTable::genSourceFile(const QString &class_name, const QString &header_file)
{
    QMap<int,QString> types = createTypes();

    QStringList source;

    source << QString("#include \"%1\"\n").arg(header_file);

    const QMetaObject *meta = metaObject();

    int count = meta->propertyCount();
    for (int i = 0 ; i < count ; i++) {
        const QMetaProperty p = meta->property(i);
        QString name(p.name());
        if (name == "objectName") {
            continue;
        }

        if (types.contains(p.typeId())) {
            QVariant v = property(p.name());

            if (p.typeId() == QMetaType::QString) {
                source << QString("%4 %1::%2 = \"%3\";\n")
                .arg(class_name)
                    .arg(p.name())
                    .arg(v.toString())
                    .arg(types[p.typeId()]);

            } else if (p.typeId() == QMetaType::QPointF) {
                QPointF pt = v.toPointF();

                source << QString("QPointF %1::%2 = QPointF(%3,%4);\n")
                              .arg(class_name)
                              .arg(p.name())
                              .arg(pt.x())
                              .arg(pt.y());

            } else if (p.typeId() == QMetaType::QRectF) {

                QRectF rect = v.toRectF();

                source << QString("QRectF %1::%2 = QRect(%3,%4,%5,%6);\n")
                              .arg(class_name)
                              .arg(p.name())
                              .arg(rect.x())
                              .arg(rect.y())
                              .arg(rect.width())
                              .arg(rect.height());

            } else {

                source << QString("%4 %1::%2 = %3;\n")
                .arg(class_name)
                    .arg(p.name())
                    .arg(v.toString())
                    .arg(types[p.typeId()]);
            }
        }
    }

    return source.join("\n");
}

void QxKeyTable::classBegin()
{
    // Intentionally left empty.
}

void QxKeyTable::componentComplete()
{
    const QMetaObject *meta = metaObject();

    int count = meta->propertyCount();
    for (int i = 0 ; i < count ; i++) {
        const QMetaProperty p = meta->property(i);
        QString name(p.name());
        if (p.typeId() != QMetaType::QString ||
            name == "objectName") {
            continue;
        }

        QVariant v = property(p.name());
        if (!v.isValid()) {
            continue;
        }

        if (!v.value<QString>().isEmpty()) {
            continue;
        }

        setProperty(p.name(), name);
    }
}


