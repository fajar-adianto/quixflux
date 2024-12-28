#ifndef QX_SIGNAL_PROXY_H
#define QX_SIGNAL_PROXY_H

#include <QVector>

#include "../qx_dispatcher.h"

class QxSignalProxy : public QObject
{
public:
    explicit QxSignalProxy(QObject *parent = nullptr);

    void bind(QObject *source, int signal_index, QQmlEngine *engine, QxDispatcher *dispatcher);

    int qt_metacall(QMetaObject::Call _c, int _id, void **_a);

    QxDispatcher *dispatcher() const;

    void setDispatcher(QxDispatcher *dispatcher);

private:
    void dispatch(const QVariantMap &message);

    QString type;
    QVector<int> parameter_types_;
    QVector<QString> parameter_names_;
    QPointer<QQmlEngine> engine_;
    QPointer<QxDispatcher> dispatcher_;
};

#endif // QX_SIGNAL_PROXY_H
