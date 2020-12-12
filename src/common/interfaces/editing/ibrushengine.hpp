/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IBRUSHENGINE_HPP
#define IBRUSHENGINE_HPP

#include <QList>
#include <QPainterPath>
#include <QRect>

#include "idtypes/brushengineid.hpp"

#include "utilities/editing/brushstroke.hpp"

#include "interfaces/traits.hpp"
namespace Addle {

class IBrush;
class IBrushEngine
{
public:
    virtual ~IBrushEngine() = default;

    virtual BrushEngineId id() const = 0;

    virtual QPainterPath indicatorShape(const BrushStroke& painter) const = 0;
    virtual void paint(BrushStroke& painter) const = 0;
};

DECL_GLOBAL_REPO_MEMBER(IBrushEngine, BrushEngineId);
} // namespace Addle

#endif // IBRUSHENGINE_HPP
