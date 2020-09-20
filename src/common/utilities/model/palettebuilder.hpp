/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PALETTEBUILDER_HPP
#define PALETTEBUILDER_HPP

#include <QSharedData>

#include "colorinfo.hpp"
#include "utilities/multiarray.hpp"

namespace Addle {

class PaletteBuilder
{
    struct PaletteBuilderData : QSharedData
    {
    };

public:
    inline PaletteBuilder& setId(PaletteId id)
    {
        _id = id;
        return *this;
    }

    inline PaletteId id() const { return _id; }

    inline PaletteBuilder& setSize(QSize size)
    {
        _colors.resize(size);
        return *this;
    }

    inline PaletteBuilder& setColor(int x, int y, QColor color, TranslatedString name = TranslatedString())
    {
        if (x >= _colors.width() || y >= _colors.height())
            _colors.resize(QSize(x - 1, y - 1));
        
        _colors.insert(QPoint(x, y), ColorInfo(color, name));
        return *this;
    }

    inline MultiArray<ColorInfo, 2> colors() const { return _colors; }

private:
    PaletteId _id;
    MultiArray<ColorInfo, 2> _colors;
};

} // namespace Addle

#endif // PALETTEBUILDER_HPP
