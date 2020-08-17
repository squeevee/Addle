#ifndef IAPPEARANCESERVICE_HPP
#define IAPPEARANCESERVICE_HPP

#include <QIcon>
#include <QCursor>
#include <QFileSelector>

#include "interfaces/traits.hpp"
namespace Addle {

class IAppearanceService
{
public:
    virtual ~IAppearanceService() = default;

    virtual bool isDarkTheme() const = 0;
    
    virtual QIcon icon(QString identifier) const = 0;
    virtual const QFileSelector& selector() = 0;
};

DECL_SERVICE(IAppearanceService);
} // namespace Addle

#endif // IAPPEARANCESERVICE_HPP