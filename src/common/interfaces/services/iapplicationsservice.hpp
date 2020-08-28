/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IAPPLICATIONSERVICE_HPP
#define IAPPLICATIONSERVICE_HPP

#include <QSet>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {

class IMainEditorPresenter;
class IApplicationService : public virtual IAmQObject
{
public:
    enum StartupMode
    {
        editor,
        browser,
        terminal
    };

    virtual ~IApplicationService() = default;

    virtual bool start() = 0;

    virtual StartupMode startupMode() = 0;

    virtual int exitCode() = 0;

    virtual void registerMainEditorPresenter(IMainEditorPresenter* presenter) = 0;
    virtual QSet<IMainEditorPresenter*> mainEditorPresenters() const = 0;

public slots:
    virtual void quitting() = 0;
};

DECL_SERVICE(IApplicationService)


} // namespace Addle
#endif // IAPPLICATIONSERVICE_HPP
