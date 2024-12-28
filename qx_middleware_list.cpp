#include <QtDebug>
#include <QQmlExpression>
#include <QQmlContext>

#include "qx_middleware_list.h"
#include "private/quix_functions.h"
#include "private/qx_middlewares_hook.h"

/*!
    \qmltype QxMiddlewareList
    \inqmlmodule QuixFlux

    \code
        import QuixFlux 1.1
    \endcode

    QxMiddlewareList groups a list of QxMiddleware and install to target QxDispatcher / QxActionCreator

    Example Code

    \code
        import QtQuick
        import QuixFlux
        import "./actions"
        import "./middlewares"

        // main.qml

        Item {
            MiddlewareList {
                applyTarget: AppDispatcher

                Middleware {
                    id: debouncerMiddleware
                }

                Middleware {
                    id: logger
                }
            }
        }
    \endcode
 */

/*! \qmlproperty var QxMiddlewareList::applyTarget
    This property specific the target object that of middlewares should be applied.
    It can be either of an ActionCreator or Dispatcher object.
 */

QxMiddlewareList::QxMiddlewareList(QQuickItem *parent)
    : QQuickItem{parent}
    , engine_(nullptr)
{
    // Intentionally left empty.
}

QObject *QxMiddlewareList::applyTarget() const
{
    return apply_target_;
}

void QxMiddlewareList::setApplyTarget(QObject *apply_target)
{
    apply_target_ = apply_target;
    if (!engine_.isNull()) {
        setup();
    }

    emit applyTargetChanged();
}

void QxMiddlewareList::apply(QObject *target)
{
    setApplyTarget(target);
}

void QxMiddlewareList::next(int sender_index, QString type, QJSValue message)
{
    QJSValueList args;

    args << QJSValue(sender_index + 1);
    args << QJSValue(type);
    args << message;
    QJSValue result = invoke_.call(args);
    if (result.isError()) {
        QuixFlux::printException(result);
    }
}

void QxMiddlewareList::classBegin()
{
    // Intentionally left empty.
}

void QxMiddlewareList::componentComplete()
{
    engine_ = qmlEngine(this);

    if (!apply_target_.isNull()) {
        setup();
    }
}

void QxMiddlewareList::setup()
{
    QxActionCreator *creator = nullptr;
    QxDispatcher *dispatcher = nullptr;

    creator = qobject_cast<QxActionCreator *>(apply_target_.data());

    if (creator) {
        dispatcher = creator->dispatcher();
    } else {
        dispatcher = qobject_cast<QxDispatcher *>(apply_target_.data());
    }

    if (creator == nullptr && dispatcher == nullptr) {
        qWarning() << "Middlewares.apply(): Invalid input";
    }

    if (action_creator_.data() == creator &&
        dispatcher_.data() == dispatcher) {
        // Nothing changed.
        return;
    }

    if (!action_creator_.isNull() &&
        action_creator_.data() != creator) {
        // in case the action creator is not changed, do nothing.
        action_creator_->disconnect(this);
    }

    if (!dispatcher_.isNull() &&
        dispatcher_.data() != dispatcher) {
        QxHook *hook = dispatcher_->hook();
        dispatcher_->setHook(nullptr);
        dispatcher_->disconnect(this);
        if (hook) {
            delete hook;
        }
    }

    action_creator_ = creator;
    dispatcher_ = dispatcher;

    if (!action_creator_.isNull()) {
        connect(action_creator_.data(),SIGNAL(dispatcherChanged()),
                this,SLOT(setup()));
    }

    if (!dispatcher_.isNull()) {
        QxMiddlewaresHook *hook = new QxMiddlewaresHook();
        hook->setParent(this);
        hook->setup(engine_.data(), this);

        if (!dispatcher_.isNull()) {
            dispatcher_->setHook(hook);
        }
    }
}
