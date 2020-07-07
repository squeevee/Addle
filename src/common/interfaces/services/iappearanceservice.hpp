#ifndef IAPPEARANCESERVICE_HPP
#define IAPPEARANCESERVICE_HPP

#include <QIcon>
#include <QCursor>
#include <QFileSelector>

#include "interfaces/traits/makeable_trait.hpp"
#include "iservice.hpp"

class IAppearanceService : public virtual IService
{
public:
    virtual ~IAppearanceService() = default;

    virtual bool isDarkTheme() const = 0;
    
    virtual QIcon icon(QString identifier) const = 0;
    virtual const QFileSelector& selector() = 0;
};

DECL_MAKEABLE(IAppearanceService);

#endif // IAPPEARANCESERVICE_HPP