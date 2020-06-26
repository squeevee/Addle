#include "colorwell.hpp"
#include <QPaintEvent>
#include <QPainter>

ColorWell::ColorWell(QWidget* parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _color = Qt::transparent;
}

void ColorWell::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}

QSize ColorWell::sizeHint() const
{
    return QSize(16, 16);
}

void ColorWell::setColor(QColor color)
{
    _color = color;
    update();
}

void ColorWell::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.fillRect(e->rect().intersected(contentsRect()), _color);

    QFrame::paintEvent(e);
}