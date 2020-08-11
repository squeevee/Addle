#ifndef IAPPLICATIONSERVICE_HPP
#define IAPPLICATIONSERVICE_HPP

#include "iservice.hpp"

#include <QStringList>

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/traits.hpp"


namespace Addle {

class IApplicationService : public virtual IService
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

//#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_OPTION "editor"
//#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_SHORT_OPTION "e"
//#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_OPTION "browser"
//#define ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_SHORT_OPTION "b"

    //static const QString CMD_EDITOR_OPTION;
    //static const QString CMD_EDITOR_SHORT_OPTION;
    //static const QString CMD_BROWSER_OPTION;
    //static const QString CMD_BROWSER_SHORT_OPTION;

public slots:
    virtual void quitting() = 0;
};

DECL_MAKEABLE(IApplicationService)
DECL_IMPLEMENTED_AS_QOBJECT(IApplicationService)

} // namespace Addle
#endif // IAPPLICATIONSERVICE_HPP
