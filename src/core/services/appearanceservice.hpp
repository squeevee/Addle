#ifndef APPEARANCESERVICE_HPP
#define APPEARANCESERVICE_HPP

#include <QFileSelector>

#include "compat.hpp"
#include "interfaces/services/iappearanceservice.hpp"
namespace Addle {
class ADDLE_CORE_EXPORT AppearanceService : public IAppearanceService
{
public:
    AppearanceService();
    virtual ~AppearanceService() = default;

    bool isDarkTheme() const { return _isDarkTheme; }

    QIcon icon(QString identifier) const override;

    const QFileSelector& selector() { return _selector; }

private: 
    bool _isDarkTheme = false;
    QFileSelector _selector;
};

} // namespace Addle
#endif // APPEARANCESERVICE_HPP