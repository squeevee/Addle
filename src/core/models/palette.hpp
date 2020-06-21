#ifndef PALETTE_HPP
#define PALETTE_HPP

#include "compat.hpp"
#include <QObject>
#include "interfaces/models/ipalette.hpp"

#include "utilities/initializehelper.hpp"

class ADDLE_CORE_EXPORT Palette : public QObject, public IPalette
{
    Q_OBJECT
public:
    virtual ~Palette() = default;

    PaletteId id() const { _initHelper.check(); return _id; }

    void initialize(const PaletteBuilder& builder);

    RectangularArray<ColorInfo> colors() const { _initHelper.check(); return _colors; }
    void setColors(RectangularArray<ColorInfo> colors);

signals: 
    void colorsChanged();

private:
    PaletteId _id;

    RectangularArray<ColorInfo> _colors;

    InitializeHelper<Palette> _initHelper;
};

#endif // PALETTE_HPP