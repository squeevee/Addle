#include "guiutils.hpp"

#include <QPainter>

QPixmap checkerBoardTexture(int width, QColor color1, QColor color2)
{
    QPixmap texture(QSize(2 * width, 2 * width));
    texture.fill(color1);

    QPainter p(&texture);
    p.fillRect(QRect(0,0, width, width), color2);
    p.fillRect(QRect(width, width, width * 2, width * 2), color2);

    p.end();

    return texture;
}