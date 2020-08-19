/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef CHECKERBOARD_HPP
#define CHECKERBOARD_HPP

#include <QtDebug>
#include <QColor>
#include <QPainter>
#include <QPixmap>

#include <cmath>
namespace Addle {

class CheckerBoard
{
public:
    CheckerBoard(int width = 24, QColor color1 = Qt::gray, QColor color2 = Qt::lightGray)
        : _width(width), _color1(color1), _color2(color2)
    {
    }
    CheckerBoard(const CheckerBoard&) = default;
    
    inline QColor color1() const { return _color1; }
    inline void setColor1(QColor color1) { _color1 = color1; }

    inline QColor color2() const { return _color2;}
    inline void setColor2(QColor color2) { _color2 = color2; }

    inline int width() const { return _width; }
    inline void setWidth(int width) { _width = width; }

    inline QPoint origin() const { return _origin; }
    inline void setOrigin(QPoint origin) { _origin = origin; }

    inline void paint(QPainter& painter, const QRect& rect)
    {
        painter.save();

        painter.setClipRect(rect);
        painter.fillRect(rect, _color1);

        painter.setBrush(_color2);
        painter.setPen(Qt::NoPen);

        QPoint offset = _origin - rect.topLeft();

        int hSpan = rect.width() / _width;
        if (rect.width() + (_origin.x() % _width) % _width)
            ++hSpan;
        if (_origin.x() % _width)
            ++hSpan;

        int vSpan = rect.height() / _width;
        if (rect.height() + (_origin.y() % _width) % _width)
            ++vSpan;
        if (_origin.y() % _width)
            ++vSpan;

        int y = rect.top();
        if (offset.y() % _width)
            y += (offset.y() % _width) - _width;

        for (int i = 0; i < vSpan; ++i)
        {
            int x = rect.left();
            if (offset.x() % _width)
                x += (offset.x() % _width) - _width;

            for (int j = 0; j < hSpan; ++j)
            {
                if ((i + j) % 2)
                {
                    painter.drawRect(x, y, _width, _width);
                }

                x += _width;
            }
            y += _width;
        }

        painter.restore();
    }

    inline QPixmap tile()
    {
        QPixmap pixmap(2 * _width, 2 * _width);

        QPainter painter(&pixmap);
        paint(painter, QRect(0, 0, 2 * _width, 2 * _width));

        return pixmap;
    }

private:
    QColor _color1;
    QColor _color2;

    int _width;
    QPoint _origin;
};

} // namespace Addle
#endif // CHECKERBOARD_HPP