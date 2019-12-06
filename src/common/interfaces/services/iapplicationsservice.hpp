#ifndef IAPPLICATIONSERVICE_HPP
#define IAPPLICATIONSERVICE_HPP

#include "iservice.hpp"

#include <QStringList>

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class IApplicationService : public virtual IService, public virtual IMakeable
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

#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_OPTION "editor"
#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_SHORT_OPTION "e"
#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_OPTION "browser"
#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_SHORT_OPTION "b"

    static const QString CMD_EDITOR_OPTION;
    static const QString CMD_EDITOR_SHORT_OPTION;
    static const QString CMD_BROWSER_OPTION;
    static const QString CMD_BROWSER_SHORT_OPTION;

public slots:
    virtual void quitting() = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IApplicationService)

#endif // IAPPLICATIONSERVICE_HPP