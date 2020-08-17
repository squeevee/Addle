/**
 * Addle source code
 * 
 * @file initializeexceptions.h
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef INITIALIZEEXCEPTIONS_HPP
#define INITIALIZEEXCEPTIONS_HPP

#include <QObject>
#include <QStringBuilder>

#include "globals.hpp"
#include "utilities/qtextensions/translation.hpp"
#include "addleexception.hpp"

namespace Addle {

DECL_LOGIC_ERROR(InitializeException)
class ADDLE_COMMON_EXPORT InitializeException : public AddleException
{
#ifdef ADDLE_DEBUG
public:
    InitializeException(const char* senderTypeName, const QString what)
        : AddleException(what),
        _senderTypeName(senderTypeName)
    {
    }

private:
    const char* _senderTypeName;

#else
public:
    InitializeException() = default;
#endif

public:
    virtual ~InitializeException() = default;

};

DECL_LOGIC_ERROR(NotInitializedException)

/**
 * @class NotInitializedException
 * @brief Thrown by objects that expect initialization, if a method was called
 * on an uninitialized object.
 * @note This behavior is automatically handled by InitializeHelper
 * @sa InitializeHelper, expects_initialize
 */
class ADDLE_COMMON_EXPORT NotInitializedException : public InitializeException
{
    ADDLE_EXCEPTION_BOILERPLATE(NotInitializedException)

public:
#ifdef ADDLE_DEBUG
    NotInitializedException(const char* senderTypeName = DEBUG__NONE_TYPE_NAME)
        : InitializeException(
            senderTypeName,
            fallback_translate(
                "NotInitializedException",
                "what",
                QStringLiteral(
                    "Attempted operation on object of type \"%1\" that was not yet initialized."
                ).arg(senderTypeName)
            )
        )
    {
    }
#else
    NotInitializedException() = default;
#endif
    virtual ~NotInitializedException() = default;
};

DECL_LOGIC_ERROR(AlreadyInitializedException)

/**
 * @class NotInitializedException
 * @brief Thrown by objects that expect initialization, if the object had
 * already been initialized and an attempt was made to initialize it again.
 * @note This behavior is automatically handled by InitializeHelper
 * @sa InitializeHelper, expects_initialize
 */
class ADDLE_COMMON_EXPORT AlreadyInitializedException : public InitializeException
{
    ADDLE_EXCEPTION_BOILERPLATE(AlreadyInitializedException)

public:
#ifdef ADDLE_DEBUG
    AlreadyInitializedException(const char* senderTypeName = DEBUG__NONE_TYPE_NAME)
        : InitializeException(
            senderTypeName,
            fallback_translate(
                "AlreadyInitializedException",
                "what",
                QStringLiteral(
                    "Attempted to initialize object of type \"%1\" that was already initialized."
                ).arg(senderTypeName)
            )
        )
    {
    }
#else
    AlreadyInitializedException() = default;
#endif
    virtual ~AlreadyInitializedException() = default;
};

DECL_LOGIC_ERROR(InvalidInitializeException)

/**
 * @class InvalidInitializeException
 * @brief Thrown by InitializeHelper if a logic error was encountered during
 * initialization.
 */
class ADDLE_COMMON_EXPORT InvalidInitializeException : public InitializeException
{
    ADDLE_EXCEPTION_BOILERPLATE(InvalidInitializeException)
public:
    enum Why
    {
        improper_order // 0
    };

#ifdef ADDLE_DEBUG
    InvalidInitializeException(Why why, const char* senderTypeName = DEBUG__NONE_TYPE_NAME)
        : InitializeException(
            senderTypeName,
            fallback_translate(
                "InvalidInitializeException",
                "what",
                QStringLiteral(
                    "A problem was encountered during initialization.\n"
                    "Sender type: \"%1\"\n"
                    "Why code:    %2")
                .arg(senderTypeName)
                .arg(why)
            )
        ), _why(why)
    {
    }
#else
public:
    InvalidInitializeException(Why why)
        :_why(why)
    {
    }
#endif
public:
    virtual ~InvalidInitializeException() = default;

private:
    const Why _why;
};

} // namespace Addle

#endif // INITIALIZEEXCEPTIONS_HPP