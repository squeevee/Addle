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
#include "utilities/qmultiarray.hpp"

namespace Addle {

class PaletteBuilder
{
    struct PaletteBuilderData : QSharedData
    {
        PaletteId id;
        QMultiArray<ColorInfo, 2> colors;
    };

public:
    PaletteBuilder()
        : _data(new PaletteBuilderData)
    {
    }

    inline PaletteBuilder& setId(PaletteId id)
    {
        _data->id = id;
        return *this;
    }

    inline PaletteId id() const { return _data->id; }

    inline PaletteBuilder& setSize(QSize size)
    {
        _data->colors.resize(size);
        return *this;
    }

    inline PaletteBuilder& setColor(int x, int y, QColor color, TranslatedString name = TranslatedString())
    {
        if (x >= _data->colors.width() || y >= _data->colors.height())
            _data->colors.resize(QSize(x - 1, y - 1));
        
        _data->colors.insert(QPoint(x, y), ColorInfo(color, name));
        return *this;
    }

    inline QMultiArray<ColorInfo, 2> colors() const { return _data->colors; }

private:
    QSharedDataPointer<PaletteBuilderData> _data;
};

} // namespace Addle

#endif // PALETTEBUILDER_HPP