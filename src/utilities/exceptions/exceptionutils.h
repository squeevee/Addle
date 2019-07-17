#ifndef EXCEPTIONUTILS_H
#define EXCEPTIONUTILS_H

#include <QObject>

#define QEXCEPTION_BOILERPLATE(T) \
public: \
    T* clone() const { return new T(*this); } \
    void raise() const { throw *this; }

#endif //EXCEPTIONUTILS_H

