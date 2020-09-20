/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PATHENGINE_HPP
#define PATHENGINE_HPP

#include "compat.hpp"
#include "globals.hpp"
#include <QObject>

#include "interfaces/editing/ibrushengine.hpp"
#include "interfaces/models/ibrush.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT PathBrushEngine : public IBrushEngine
{
public:
    virtual ~PathBrushEngine() = default;

    BrushEngineId id() const { return CoreBrushEngines::PathEngine; }

    //BrushInfo info(const IBrush& model) const { return BrushInfo::fromDefaults(); }

    QPainterPath indicatorShape(const BrushStroke& painter) const;
    void paint(BrushStroke& painter) const;

private:
    QRect boundingRect(QPointF pos, double size) const;
};

} // namespace Addle

#endif // PATHENGINE_HPP
