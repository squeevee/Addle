/**
 * Addle source code
 * @file qhash.h
 * @copyright Copyright 2019 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 * 
 * Declares overloads of uint qHash(...), to extend the types usable as keys in
 * QHash and QSet
 */

#ifndef QT_EXTENSIONS__QHASH_HPP
#define QT_EXTENSIONS__QHASH_HPP

#include <typeindex>
#include <QHash>

#include "data/formatid.hpp"
#include "data/toolid.hpp"
#include "data/brushtipid.hpp"

namespace std
{
    uint qHash(const std::type_index& key);
}

uint qHash(const FormatId& format);
uint qHash(const ToolId& tool);
uint qHash(const BrushTipId& brush);

#endif //QT_EXTENSIONS__QHASH_HPP