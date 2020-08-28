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
#include <QSharedPointer>

namespace Addle {

class AddleException;
class IErrorPresenter 
{
public:
    enum Severity
    {
        Info,
        Warning,
        Critical
    };

    virtual ~IErrorPresenter() = default;

    virtual Severity severity() const = 0;
    virtual QString message() const = 0;

    virtual QSharedPointer<AddleException> exception() const = 0;
};

} // namespace Addle

#endif // IERRORPRESENTER_HPP