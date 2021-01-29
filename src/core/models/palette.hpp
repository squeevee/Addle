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

#include "utilities/initializehelper.hpp"

#include <boost/di.hpp>

namespace Addle {
class ADDLE_CORE_EXPORT Palette : public QObject, public IPalette
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    BOOST_DI_INJECT(
            Palette, 
            /*(named = aux_IPalette::id_)*/ PaletteId id, 
            /*(named = aux_IPalette::builder_)*/ const PaletteBuilder& builder
        )
        : _id(id)
    {
        initialize(builder);
    }
    virtual ~Palette() = default;

    PaletteId id() const { ASSERT_INIT(); return _id; }

    void initialize(const PaletteBuilder& builder);

    ColorArray colors() const { ASSERT_INIT(); return _colors; }

    bool contains(QColor color) const { return _index.contains(color.rgb()); }
    ColorInfo infoFor(QColor color) const { return _index[color.rgb()]; }

public slots:
    void setColors(ColorArray colors);

signals: 
    void colorsChanged(ColorArray colors);

private:
    void buildIndex();

    const PaletteId _id;

    ColorArray _colors;
    QHash<QRgb, ColorInfo> _index;

    InitializeHelper _initHelper;
};

} // namespace Addle

#endif // PALETTE_HPP
