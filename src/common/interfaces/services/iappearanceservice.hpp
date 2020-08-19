/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
    
    virtual QIcon icon(const char* identifier) const = 0;
    virtual const QFileSelector& selector() = 0;
};

DECL_SERVICE(IAppearanceService);
} // namespace Addle

#endif // IAPPEARANCESERVICE_HPP