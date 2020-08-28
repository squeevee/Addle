/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef RASTERENGINEPARAMS_HPP
#define RASTERENGINEPARAMS_HPP

#include <QObject>
#include <QString>
#include <QMetaEnum>

#include "compat.hpp"

#include "interfaces/models/ibrush.hpp"

#include "utilities/errors.hpp"

#define RASTER_PARAM_MODE "mode"
#define RASTER_PARAM_HARDNESS "hardness"
#define RASTER_PARAM_SPACING "spacing"

namespace Addle {

class ADDLE_COMMON_EXPORT RasterEngineParams final
{
    Q_GADGET
public:

    RasterEngineParams(const IBrush& model)
        : _model(model)
    {
    }

    enum Mode
    {
        Shape,
        Gradient,
        AlphaRaster,
        PixelRaster
    };
    Q_ENUM(Mode);

    inline Mode mode() const
    {
        QString modeKey = _model.customEngineParameters()[QStringLiteral(RASTER_PARAM_MODE)].toString();

        bool ok;
        Mode result = static_cast<Mode>(QMetaEnum::fromType<Mode>().keyToValue(qPrintable(modeKey), &ok));

        ADDLE_ASSERT(ok);

        return result;
    }

    inline double hardness() const
    {
        ADDLE_ASSERT(mode() == Gradient);
        return _model.customEngineParameters()[QStringLiteral(RASTER_PARAM_HARDNESS)].toDouble();
    }

    inline double spacing() const
    {
        return _model.customEngineParameters()[QStringLiteral(RASTER_PARAM_SPACING)].toDouble();
    }

private:
    const IBrush& _model;
};

} // namespace Addle

#endif // RASTERENGINEPARAMS_HPP