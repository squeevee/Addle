/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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

    QIcon icon(const char* identifier) const override;

    const QFileSelector& selector() { return _selector; }

private: 
    bool _isDarkTheme = false;
    QFileSelector _selector;
};

} // namespace Addle
#endif // APPEARANCESERVICE_HPP