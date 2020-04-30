#ifndef IAPPEARANCESERVICE_HPP
#define IAPPEARANCESERVICE_HPP

#include <QIcon>
#include <QCursor>

#include "interfaces/traits/makeable_trait.hpp"
#include "iservice.hpp"

class IAppearanceService : public virtual IService
{
public:
    virtual ~IAppearanceService() = default;

    virtual QIcon icon(QString identifier) const = 0;
};

#define ADDLE_ICON(identifier) ServiceLocator::get<IAppearanceService>().icon(identifier)

DECL_MAKEABLE(IAppearanceService);

#endif // IAPPEARANCESERVICE_HPP