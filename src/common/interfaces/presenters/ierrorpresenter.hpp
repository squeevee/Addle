/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IERRORPRESENTER_HPP
#define IERRORPRESENTER_HPP

#include <QString>

#include "interfaces/traits.hpp"
namespace Addle {


class IErrorPresenter 
{
public:

    enum Severity
    {
        info,
        warning,
        critial
    };

    virtual ~IErrorPresenter() = default;

    virtual void initialize(QString message = QString(), Severity severity = Severity::warning) = 0;

    virtual Severity severity() = 0;
    virtual void setSeverity(Severity severity) = 0;

    virtual QString message() = 0;
    virtual void setMessage(QString message) = 0;
};

DECL_MAKEABLE(IErrorPresenter)
DECL_EXPECTS_INITIALIZE(IErrorPresenter)

} // namespace Addle
#endif // IERRORPRESENTER_HPP