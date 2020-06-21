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

bool ColorWell::event(QEvent* e)
{

    return QWidget::event(e);
}
QSize ColorWell::sizeHint() const
{
    return QSize(25, 25);
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