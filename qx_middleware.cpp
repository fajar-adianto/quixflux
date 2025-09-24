#include "qx_middleware.h"
#include "private/quix_functions.h"

/*!
    \qmltype QxMiddleware
    \inqmlmodule QuixFlux

    \code
        import QuixFlux
    \endcode

    The middleware in QuixFlux is similar to the one in Redux and those from server libraries like Express and Koa.
    It is some code you can put between the QxDispatcher and QxStores.
    It could modify/defer/remove received actions and insert new actions that will dispatch to QxStore components.
    Users may use it for logging, crash reporting, talking to asynchronous components like FileDialog / Camera etc.
    So that Store components remain “pure”, it holds application logic only and always return the same result for the same input.
    It is easier to write test cases.

    Event Flow

    \image middleware-data-flow.png "Concept"

    \e{Example Code}

    \code
        // Action Logger
        import QuixFlux 1.1

        QxMiddleware {
            function dispatch(type, message) {
                console.log(type, JSON.string(message));
                next(type, message); // propagate the action to next QxMiddleware or QxMiddleware component type. If you don’t call it, the action will be dropped.
            }
        }
    \endcode

    Whatever the middleware received a new action, it will invoke the "dispatch" function written by user.
    If the middleare accept the action, it should call the next() to propagate the action to anothter middleware.
    User may modify/insert/delay or remove the action.

    \code
        // Confirmation Dialog

        import QuixFlux
        import QtQuick.Dialogs

        QxMiddleware {
            FileDialog {
                id: fileDialog
                onAccepted: {
                    next(ActionTypes.removeItem);
                }
            }

            function dispatch(type, message) {
                if (type === ActionTypes.removeItem) {
                    fileDialog.open();
                } else {
                    next(type, message);
                }
            }
        }
    \endcode
*/

QxMiddleware::QxMiddleware(QQuickItem *parent)
    : QQuickItem{parent}
    , filter_function_enabled_(false)
{
    // Intentionally left empty.
}

QJSValue QxMiddleware::nextCallback() const
{
    return next_callback_;
}

void QxMiddleware::setNextCallback(const QJSValue &next_callback)
{
    next_callback_ = next_callback;
    emit _nextCallbackChanged();
}

/*! \qmlmethod QxMiddleware::next(string type, object message)
    Pass an action message to next middleware. If it is already the last middleware, the action will be dispatched to QxStore component.
 */

/*! \qmlproperty bool QxMiddleware::filterFunctionEnabled
    If this property is true, whatever the middleware component received a new action.
    Beside to invoke a dispatch signal, it will search for a function with a name as the action.
    If it exists, it will call also call the function.

    \code
        Middleware {
            filterFunctionEnabled: true

            function addItem(message) {
                //process
                next("addItem", message);
            }
        }
    \endcode

    The default value is false
 */

void QxMiddleware::next(QString type, QJSValue message)
{
    QQmlEngine* engine = qmlEngine(this);
    QX_PRECHECK_DISPATCH(engine, type, message);

    if (next_callback_.isCallable()) {
        QJSValueList args;
        args << type;
        args << message;
        QJSValue result = next_callback_.call(args);
        if (result.isError()) {
            QuixFlux::printException(result);
        }
    }
}
