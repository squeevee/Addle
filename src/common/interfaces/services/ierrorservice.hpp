/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IERRORSERVICE_HPP
#define IERRORSERVICE_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "exceptions/unhandledexception.hpp"

namespace Addle {

/**
 * @class IErrorService
 * Manages application-wide errors.
 */
class IErrorService : public virtual IAmQObject
{
public:
    typedef UnhandledException::Severity Severity;

    virtual ~IErrorService() = default;

    // These methods are called by ADDLE_SLOT_CATCH, etc. They should generally
    // not be called elsewhere.
    virtual void reportUnhandledError(const AddleException& ex, Severity severity = Severity::Normal) = 0;
    virtual void reportUnhandledError(const std::exception& ex, Severity severity = Severity::Normal) = 0;

signals:
    virtual void unhandledErrorReported(QSharedPointer<UnhandledException> ex) = 0;
};

DECL_SERVICE(IErrorService)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IErrorService, "org.addle.IErrorService")

#endif // IERRORSERVICE_HPP