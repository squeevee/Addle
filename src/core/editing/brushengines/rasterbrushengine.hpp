/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef RASTERBRUSHENGINE_HPP
#define RASTERBRUSHENGINE_HPP

#include "interfaces/editing/ibrushengine.hpp"
#include "compat.hpp"
#include "globals.hpp"
#include "utils.hpp"

namespace Addle {

class RasterEngineParams;

class ADDLE_CORE_EXPORT RasterBrushEngine : public IBrushEngine
{
public:
    virtual ~RasterBrushEngine() = default;

//     BrushEngineId id() const { return CoreBrushEngines::RasterEngine; }

    //BrushInfo info(const IBrush& model) const { return BrushInfo(); }

    QPainterPath indicatorShape(const BrushStroke& painter) const;
    void paint(BrushStroke& painter) const;

private:
    static void paint_p(QPainter& painter, const RasterEngineParams& params, QPointF pos, QColor color, double size);
    static void paintGradient(QPainter& painter, QPointF pos, QColor color, double size, double hardness);
};

} // namespace Addle

#endif // RASTERBRUSHENGINE_HPP
