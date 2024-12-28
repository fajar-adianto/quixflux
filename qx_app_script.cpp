#include <QQmlExpression>
#include <QtDebug>
#include <QtQml>

#include "qx_app_script.h"
#include "qx_app_listener.h"
#include "private/qx_app_script_runnable.h"

/*! \qmltype QxAppScript
    \inqmlmodule QuixFlux

QxAppScript is a helper component to handle asynchronous sequential workflow.
The immediate benefit of using QxAppScript  is the centralisation of code in a place.
Instead of placing them within onXXXX code block in several components in several places.

QxAppScript also manage the life cycle of registered callbacks. You could remove them by a single function. Nothing will leave in memory.


\b {Remarks: QxAppScript is not a solution for making Store component}

\code
    QxAppScript {
        // Run this script if "Pick Image" button is clicked.
        runWhen: ActionTypes.askToPickPhoto

        script: {
            // Step 1. Open file dialog
            dialog.open();

            // Register a set of callbacks as workflow
            // Registered callbacks will be executed once only per script execution
            once(dialog.onAccepted, function() {

                // Step 2. Once user picked an image, launch preview window and ask for confirmation.
                AppActions.navigateTo(imagePreview,
                                      {source: dialog.fileUrl});

            }).then(ActionTypes.pickPhoto, function(message) {
                // The function of then() is same as once() but it won't
                // trigger the callback until once() is triggered.

                // Step 3. Add picked image to store and go back to previous page.

                PhotoStore.add(String(message.url));

                AppActions.navigateBack();

            }); // <-- You may chain then() function again.

            // Condition to terminate the workflow:
            // Force to terminate if dialog is rejected / or navigateBack is dispatched
            // That will remove all the registered callbacks

            once(dialog.onRejected,exit.bind(this,0));

            once(ActionTypes.navigateBack,exit.bind(this,0));
        }
    }
\endcode

\b {Benefit of using QxAppScript}

\list
\li Centralize your workflow code in one place
\li Highly integrated with AppDispatcher. The order of callback execution is guaranteed in sequence order.
\li Only one script can be executed at a time. Registered callbacks by previous script will be removed before starting.
\li exit() will remove all the registered callbacks. No callback leave after termination.
\endlist

\b {Why not just use Promise?}

\list
\li 1. You need another library. (e.g QuickPromise)
\li 2. You need to set reject condition per callback/promise
\endlist

\b Explanation:

Coding in a promise way requires you to handle every reject condition correctly. Otherwise,
promise will leave in memory and their behaviour will be unexpected.
QxAppScript::run() / QxAppScript::exit() clear all the registered callback completely. You can write less code.

 */

/*! \qmlsignal QxAppScript::started()
    This signal is emitted when the script is started.
*/

/*! \qmlsignal QxAppScript::finished(int returnCode)
    This signal is emitted when the script is finished.
 */

QxAppScript::QxAppScript(QQuickItem *parent)
    : QQuickItem{parent}
    , running_(false)
    , processing_(false)
    , listener_id_(0)
    , listener_(nullptr)
    , auto_exit_(true)
{
    // Intentionally left empty.
}

/*! \qmlproperty script AppScript::script
    This property holds the script to run.
 */

QQmlScriptString QxAppScript::script() const
{
    return script_;
}

void QxAppScript::setScript(const QQmlScriptString &script)
{
    script_ = script;
    emit scriptChanged();
}

/*! \qmlproperty bool QxAppScript::running
    This property hold a value to indicate is the script still running.
    If there has any registered callback leave, it will be considered as running.
 */

bool QxAppScript::running() const
{
    return running_;
}

/*! \qmlproperty string QxAppScript::runWhen
    This property hold a string of message type.
    Whatever a dispatched message matched, it will trigger to call run() immediately.
 */

QString QxAppScript::runWhen() const
{
    return run_when_;
}

void QxAppScript::setRunWhen(const QString &run_when)
{
    run_when_ = run_when;
    emit runWhenChanged();
}

QJSValue QxAppScript::message() const
{
    return message_;
}

void QxAppScript::setMessage(const QJSValue &message)
{
    message_ = message;
    emit messageChanged();
}

int QxAppScript::listenerId() const
{
    return listener_id_;
}

void QxAppScript::setListenerId(int listener_id)
{
    listener_id_ = listener_id;
    emit listenerIdChanged();
}

QList<int> QxAppScript::waitFor() const
{
    return wait_for_;
}

void QxAppScript::setWaitFor(const QList<int> &wait_for)
{
    wait_for_ = wait_for;
    setListenerWaitFor();
    emit waitForChanged();
}

bool QxAppScript::autoExit() const
{
    return auto_exit_;
}

void QxAppScript::setAutoExit(bool auto_exit)
{
    auto_exit_ = auto_exit;
    emit autoExitChanged();
}

/*! \qmlmethod QxAppScript::exit(int returnCode)
    Terminate current executing script by removing all the registered callbacks.
 */

void QxAppScript::exit(int returnCode)
{
    clear();
    setRunning(false);
    emit finished(returnCode);
}

/*! \qmlmethod QxAppScript::run()
    Call this function to execute script.
    If the previous script is still running.
    QxAppScript will terminate previous script by removing all the registered callbacks.
 */

void QxAppScript::run(QJSValue message)
{
    if (processing_) {
        qWarning() << "QxAppScript::run(): Don't call run() within script / wait callback";
        return;
    }

    processing_ = true;
    clear();
    setMessage(message);

    if (dispatcher_.isNull()) {
        qWarning() << "QxAppScript::run() - Missing QxAppDispatcher. Aborted.";
        processing_ = false;
        return;
    }

    setRunning(true);

    emit started();

    QQmlExpression expr(script_);

    if (!script_.isEmpty()) {
        expr.evaluate();
    }

    if (expr.hasError()) {
        qWarning() << expr.error();
    }

    if (runnables_.size() == 0) {
        exit(0);
    }

    processing_ = false;
}

/*! \qmlmethod chian AppScript::once(var type, func callback)

    Register a callback to be triggered when a matched message type is dispatched or a signal is emitted.
    It will be triggered once only.
    User should call this function within the script code block.

    The callback will be removed on script termination.

    Moreover, this function is chainable:

    Example

    \code
        AppScript {

            script: {

                once(ActionTypes.askToRemoveSelectedItem, funciton() {

                    removeConfirmationDialog.open();

                }).then(removeConfirmationDialog.onAccepted, function() {

                    AppActions.removeSelectedItem();

                }); // <-- You may chain then() function again.
            }
        }
  \endcode

    The callback in then() will not be registrated immediately.
    It is deferred until the previouew callback triggered.
 */
QxAppScriptRunnable *QxAppScript::once(QJSValue condition, QJSValue script)
{
    QxAppScriptRunnable *runnable = new QxAppScriptRunnable(this);
    runnable->setEngine(qmlEngine(this));
    runnable->setCondition(condition);
    runnable->setScript(script);
    runnables_.append(runnable);
    return runnable;
}

/*! \qmlmethod AppScript::on(var type, func callback)
    Register a callback to be triggered when a matched message type is dispatched or a signal is emitted.
    User should call this function within the script code block.

    The callback will be removed on script termination.
 */

void QxAppScript::on(QJSValue condition, QJSValue script)
{
    QxAppScriptRunnable *runnable = once(condition,script);
    runnable->setIsOnceOnly(false);
}

void QxAppScript::componentComplete()
{
    QQuickItem::componentComplete();

    QQmlEngine *engine = qmlEngine(this);
    Q_ASSERT(engine);


    dispatcher_ = QxAppDispatcher::instance(engine);

    listener_ = new QxListener(this);

    setListenerId(dispatcher_->addListener(listener_));

    setListenerWaitFor();

    connect(listener_, SIGNAL(dispatched(QString,QJSValue)),
            this, SLOT(onDispatched(QString,QJSValue)));
}

void QxAppScript::abort()
{
    exit(-1);
}

void QxAppScript::clear()
{
    for (int i = 0 ; i < runnables_.size(); i++) {
        runnables_[i]->deleteLater();
    }
    runnables_.clear();
}

void QxAppScript::setRunning(bool running)
{
    if (running_ == running) {
        return;
    }
    running_ = running;
    emit runningChanged();
}

void QxAppScript::setListenerWaitFor()
{
    if (!listener_) {
        return;
    }

    listener_->setWaitFor(wait_for_);
}

void QxAppScript::onDispatched(QString type, QJSValue message)
{
    if (!run_when_.isEmpty() &&
        type == run_when_ &&
        !processing_) {

        if (running_) {
            abort();
        }
        run(message);
        return;
    }

    if (!running_) {
        return;
    }

    processing_ = true;

    // Mark for removeal
    QList<int> marked;

    for (int i = 0 ; i < runnables_.size() ; i++) {
        if (runnables_[i]->type() == type) {
            runnables_[i]->run(message);

            if (!running_) {
                // If exit() is called in runnable. It shoud not process any more.
                break;
            }

            if (runnables_[i]->isOnceOnly()) {
                marked << i;
            }
        }
    }

    if (!running_) {
        // Terminate if exit() is called in runnable
        processing_ = false;
        return;
    }

    for (int i = marked.size() - 1 ; i >= 0 ; i--) {
        int idx = marked[i];
        QxAppScriptRunnable *runnable = runnables_.takeAt(idx);

        QxAppScriptRunnable *next = runnable->next();
        if (next) {
            next->setParent(this);
            runnables_.append(next);
        }
        runnable->release();
        runnable->deleteLater();
    }

    processing_ = false;

    // All the tasks are finished
    if (runnables_.size() == 0 && auto_exit_) {
        exit(0);
    }
}
