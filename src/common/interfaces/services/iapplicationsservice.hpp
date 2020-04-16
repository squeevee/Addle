#ifndef IAPPLICATIONSERVICE_HPP
#define IAPPLICATIONSERVICE_HPP

#include "iservice.hpp"

#include <QStringList>

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ADDLE_COMMON_EXPORT IApplicationService : public virtual IService
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

    virtual StartupMode getStartupMode() = 0;

    virtual int getExitCode() = 0;

public slots:
    virtual void quitting() = 0;
};

DECL_MAKEABLE(IApplicationService)
DECL_IMPLEMENTED_AS_QOBJECT(IApplicationService)

#endif // IAPPLICATIONSERVICE_HPP
