#ifndef QX_FILTER_H
#define QX_FILTER_H

#include <QJSValue>
#include <QQmlParserStatus>
#include <QObject>
#include <QPointer>
#include <QQmlEngine>
#include <QQmlListProperty>
#include <QVariant>

// Filter represents a filter rule in QxAppListener
class QxFilter : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QStringList types READ types WRITE setTypes NOTIFY typesChanged)
    Q_PROPERTY(QQmlListProperty<QObject> __children READ children)
    Q_INTERFACES(QQmlParserStatus)
    Q_CLASSINFO("DefaultProperty", "__children")
    QML_ELEMENT
public:
    explicit QxFilter(QObject *parent = nullptr);

    QString type() const;

    void setType(const QString &type);

    QStringList types() const;

    void setTypes(const QStringList &types);

    QQmlListProperty<QObject> children();

protected:
    void classBegin();
    void componentComplete();

private:
    QStringList types_;
    QList<QObject *> children_;
    QPointer<QQmlEngine> engine_;

private slots:
    void filter(QString type, QJSValue message);
    void filter(QString type, QVariant message);

signals:
    void dispatched(QString type, QJSValue message);

    void typeChanged();

    void typesChanged();
};

#endif // QX_FILTER_H
