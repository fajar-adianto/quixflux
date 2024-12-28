#ifndef QUIX_FUNCTIONS_H
#define QUIX_FUNCTIONS_H

#include <QJSValue>

namespace QuixFlux {

void printException(QJSValue value);

}

#define QX_PRECHECK_DISPATCH(engine, type, message) Q_UNUSED(engine)

#endif // QUIX_FUNCTIONS_H
