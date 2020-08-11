#include "imageutils.hpp"

using namespace Addle;

using namespace ImageUtils;

QImage compositeByMask(QImage mask, QImage image0, QImage image1)
{
    //assert mask, image0, and image1 are the same size
    //assert image0 and image1 are ARGB32_premultiplied, that's the only
    //format I intend to support for now
    //assert mask is mono

    QImage result = QImage(mask.size(), QImage::Format_ARGB32_Premultiplied);

    for (int y = 0; y < mask.height(); y++)
    {
        for (int x = 0; x < mask.width(); x++)
        {
            QRgb value;
            switch (mask.pixelIndex(x, y))
            {
            case 0:
                value = image0.pixel(x, y);
            case 1:
                value = image1.pixel(x, y);
            }
            result.setPixel(x, y, value);
        }
    }
}