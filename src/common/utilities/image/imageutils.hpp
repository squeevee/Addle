/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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