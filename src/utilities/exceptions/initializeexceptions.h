#ifndef INITIALIZEEXCEPTIONS_H
#define INITIALIZEEXCEPTIONS_H

#include <QException>
#include <QObject>

#include "exceptionutils.h"

class NotInitializedException : public QException
{
    QEXCEPTION_BOILERPLATE(NotInitializedException)

public:
    NotInitializedException(const NotInitializedException& clone)
        : sender(clone.sender)
    {
    }

    NotInitializedException(const QObject* sender = nullptr)
        : sender(sender)
    {
    }

    const QObject* const sender;

};

class AlreadyInitializedException : public QException
{
    QEXCEPTION_BOILERPLATE(AlreadyInitializedException)

public:
    AlreadyInitializedException(const NotInitializedException& clone)
        : sender(clone.sender)
    {
    }

    AlreadyInitializedException(const QObject* sender = nullptr)
        : sender(sender)
    {
    }

    const QObject* const sender;

};

class InvalidInitializeException : public QException
{
    QEXCEPTION_BOILERPLATE(InvalidInitializeException)

public:
    InvalidInitializeException(const InvalidInitializeException& clone)
        : what(clone.what)
    {
    }
    
    InvalidInitializeException(const QString what)
        : what(what)
    {
    }

    const QString what;
};

#endif //INITIALIZEEXCEPTIONS_H