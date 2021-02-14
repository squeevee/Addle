/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PALETTE_HPP
#define PALETTE_HPP

#include <QtDebug>
#include "compat.hpp"
#include <QObject>
#include <QHash>
#include <QPair>
#include "interfaces/models/ipalette.hpp"

#include <boost/di.hpp>

namespace Addle {
class ADDLE_CORE_EXPORT Palette : public QObject, public IPalette
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    Palette(PaletteId id, const PaletteBuilder& builder);
    virtual ~Palette() = default;

    PaletteId id() const override { return _id; }

    ColorArray colors() const override { return _colors; }

    bool contains(QColor color) const override { return _index.contains(color.rgb()); }
    ColorInfo infoFor(QColor color) const override { return _index[color.rgb()]; }

public slots:
    void setColors(ColorArray colors);

signals: 
    void colorsChanged(ColorArray colors);

private:
    void buildIndex();

    const PaletteId _id;

    ColorArray _colors;
    QHash<QRgb, ColorInfo> _index;
};

} // namespace Addle

#endif // PALETTE_HPP
