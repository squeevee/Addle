#ifndef IMAGEUTILS_HPP
#define IMAGEUTILS_HPP

#include "compat.hpp"
#include <QImage>
#include <QPainter>
namespace Addle {

namespace ImageUtils
{

    ADDLE_COMMON_EXPORT QImage compositeByMask(QImage mask, QImage image0, QImage image1);

}

} // namespace Addle
#endif // IMAGEUTILS_HPP