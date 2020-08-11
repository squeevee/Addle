#ifndef II18NSERVICE_HPP
#define II18NSERVICE_HPP

/**
 * @file
 * @warning This file is included by "utils.hpp", and is thus included by large
 * amounts of Addle code. Be cautious about making changes, and especially about
 * adding `#include` directives.
 */

#include "iservice.hpp"
#include "interfaces/traits.hpp"

#include <QString>
namespace Addle {

// Qt's support for internationalized text seems only well-suited with static 
// (moc-time) hard-coded strings in source. I find this to be inflexible, and a
// massive unneeded entanglement, so we're going to rough it with a simple JSON
// string catalog. 
// If this proves to be insufficient, we may consider using garbageslam/spirit-pe
// or KDE Localization, which offer richer features but also appear to work better
// with an out-of-source text model than Qt does.
// I may also subclass QTranslator to serve as a Qt-compatible wrapper for this
// service -- i.e., if we end up using UIC or something where such a thing would
// be convenient

class II18nService : public IService
{
public:
    virtual ~II18nService() = default;

    virtual QString text(QString identifier) const = 0;
};


DECL_MAKEABLE(II18nService)
} // namespace Addle

#endif // II18NSERVICE_HPP