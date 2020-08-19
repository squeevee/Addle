#ifndef RASTERENGINEPARAMS_HPP
#define RASTERENGINEPARAMS_HPP

#include <QObject>
#include <QString>

#include "interfaces/models/ibrush.hpp"

#include "compat.hpp"
#include "utilities/qtextensions/qmeta.hpp"

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

        // assert valid?
        return enumValueFromKey<Mode>(modeKey.toLatin1());
    }

    inline double hardness() const
    {
        // assert mode == Gradient
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