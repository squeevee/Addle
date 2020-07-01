#ifndef PALETTE_HPP
#define PALETTE_HPP

#include "compat.hpp"
#include <QObject>
#include <QHash>
#include <QPair>
#include "interfaces/models/ipalette.hpp"

#include "utilities/initializehelper.hpp"

class ADDLE_CORE_EXPORT Palette : public QObject, public IPalette
{
    Q_OBJECT
public:
    virtual ~Palette() = default;

    PaletteId id() const { _initHelper.check(); return _id; }

    void initialize(const PaletteBuilder& builder);

    QMultiArray<ColorInfo, 2> colors() const { _initHelper.check(); return _colors; }
    void setColors(QMultiArray<ColorInfo, 2> colors);

    bool contains(QColor color) const { return _index.contains(color.rgb()); }
    ColorInfo infoFor(QColor color) const { return _index[color.rgb()]; }

signals: 
    void colorsChanged();

private:
    void buildIndex();

    PaletteId _id;

    QMultiArray<ColorInfo, 2> _colors;
    QHash<QRgb, ColorInfo> _index;

    InitializeHelper<Palette> _initHelper;
};

#endif // PALETTE_HPP