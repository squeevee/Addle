#ifndef IMAGEUTILS_HPP
#define IMAGEUTILS_HPP

#include <QImage>
#include <QPainter>
#include "interfaces/traits/compat.hpp"

namespace ImageUtils
{

    QImage ADDLE_COMMON_EXPORT compositeByMask(QImage mask, QImage image0, QImage image1);

}

#endif // IMAGEUTILS_HPP