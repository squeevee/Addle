#ifndef IMAGEUTILS_HPP
#define IMAGEUTILS_HPP

#include <QImage>
#include <QPainter>

namespace ImageUtils
{

    QImage compositeByMask(QImage mask, QImage image0, QImage image1);

}

#endif // IMAGEUTILS_HPP