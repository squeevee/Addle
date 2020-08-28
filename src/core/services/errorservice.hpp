/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ERRORSERVICE_HPP
#define ERRORSERVICE_HPP

#include "compat.hpp"

#include "interfaces/services/ierrorservice.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT ErrorService : public QObject, public IErrorService
{
    Q_OBJECT 
    Q_INTERFACES(Addle::IErrorService)
    IAMQOBJECT_IMPL

public:
    ErrorService();

    virtual ~ErrorService() = default;

    void reportUnhandledError(const AddleException& ex, Severity severity = Severity::Normal);
    void reportUnhandledError(const std::exception& ex, Severity severity = Severity::Normal);
    
signals:
    void unhandledErrorReported(QSharedPointer<UnhandledException> ex);

private slots:
    void displayError(QSharedPointer<UnhandledException> ex);
};

}

#endif // ERRORSERVICE_HPP