#ifndef QX_KEY_TABLE_H
#define QX_KEY_TABLE_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlParserStatus>

class QxKeyTable : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    QML_ELEMENT
public:
    QxKeyTable(QObject *parent = nullptr);

public slots:
    QString genHeaderFile(const QString& class_name);
    QString genSourceFile(const QString& class_name, const QString& header_file);

protected:
    void classBegin();
    void componentComplete();
};

#endif // QX_KEY_TABLE_H
