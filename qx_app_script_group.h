#ifndef QX_APP_SCRIPT_GROUP_H
#define QX_APP_SCRIPT_GROUP_H

#include <QQuickItem>

#include "qx_app_script.h"

class QxAppScriptGroup : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QJSValue scripts READ scripts WRITE setScripts NOTIFY scriptsChanged)
    QML_ELEMENT
public:
    explicit QxAppScriptGroup(QQuickItem *parent = nullptr);

    QJSValue scripts() const;

    void setScripts(const QJSValue &scripts);

public slots:
    void exitAll();

private:
    QJSValue scripts_;
    QList<QPointer<QxAppScript>> objects_;

private slots:
    void onStarted();

signals:
    void scriptsChanged();

};

#endif // QX_APP_SCRIPT_GROUP_H
