#include "qx_app_script_group.h"

/*! \qmltype QxAppScriptGroup
    \inqmlmodule QuixFlux

    QxAppScriptGroup hold a group of QxAppScript objects which are mutually exclusive in execution.
    Whatever a QxAppScript is going to start, it will terminate all other QxAppScript objects.
    So that only one QxAppScript is running at a time.

    \code
        Item {

            QxAppScript {
                id: script1
                script: {
                    // write script here
                }
            }

            QxAppScript {
                id: script2
                script: {
                    // write script here
                }
            }

            QxAppScriptGroup {
                scripts: [script1, script2]
            }

            Component.onCompleted: {

                script1.run();

                script2.run();

                // At this point, AppScriptGroup will force script1 to terminate since script2 has been started.

            }

        }
    \endcode

 */

QxAppScriptGroup::QxAppScriptGroup(QQuickItem *parent)
    : QQuickItem{parent}
{
    // Intentionally left empty.
}

/*! \qmlproperty array QxAppScriptGroup::scripts
    This property hold an array of QxAppScript object.
    They are mutually exclusive in execution.

    \code
        QxAppScript {
            id: script1
        }

        QxAppScript {
            id: script2
        }

        QxAppScriptGroup {
            scripts: [script1, script2]
        }
    \endcode
 */

QJSValue QxAppScriptGroup::scripts() const
{
    return scripts_;
}

void QxAppScriptGroup::setScripts(const QJSValue &scripts)
{
    for (int i = 0 ; i < objects_.count() ; i++) {
        if (objects_.at(i).data()) {
            objects_.at(i)->disconnect(this);
        }
    }

    objects_.clear();
    scripts_ = scripts;

    if (!scripts.isArray()) {
        qWarning() << "QxAppScriptGroup: Invalid scripts property";
        return;
    }

    int count = scripts.property("length").toInt();

    for (int i = 0 ; i < count ; i++) {
        QJSValue item = scripts.property(i);

        if (!item.isQObject()) {
            qWarning() << "QxAppScriptGroup: Invalid scripts property";
            continue;
        }

        QxAppScript *object = qobject_cast<QxAppScript*>(item.toQObject());

        if (!object) {
            qWarning() << "QxAppScriptGroup: Invalid scripts property";
            continue;
        }
        objects_ << object;
        connect(object,SIGNAL(started()),
                this,SLOT(onStarted()));
    }

    emit scriptsChanged();
}

/*! \qmlmethod QxAppScriptGroup::exitAll()
    Terminate all AppScript objects_
 */

void QxAppScriptGroup::exitAll()
{
    for (int i = 0 ; i < objects_.count() ; i++) {
        if (objects_.at(i).data()) {
            objects_.at(i)->exit();
        }
    }
}

void QxAppScriptGroup::onStarted()
{
    QxAppScript *source = qobject_cast<QxAppScript*>(sender());

    for (int i = 0 ; i < objects_.count() ; i++) {
        QPointer<QxAppScript> object = objects_.at(i);
        if (object.isNull())
            continue;

        if (object.data() != source) {
            object->exit();
        }
    }
}
