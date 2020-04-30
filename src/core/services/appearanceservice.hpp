#ifndef APPEARANCESERVICE_HPP
#define APPEARANCESERVICE_HPP

#include <QFileSelector>

#include "compat.hpp"
#include "interfaces/services/iappearanceservice.hpp"

class ADDLE_CORE_EXPORT AppearanceService : public IAppearanceService
{
public:
    AppearanceService();
    virtual ~AppearanceService() = default;

    QIcon icon(QString identifier) const override;

private: 
    bool isDarkTheme = false;

    QFileSelector iconSelector;
};

#endif // APPEARANCESERVICE_HPP