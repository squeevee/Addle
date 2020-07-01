#include "colorwell.hpp"
#include <QPaintEvent>
#include <QPainter>
#include <QEvent>
#include <QMetaMethod>

#include "guiutils.hpp"

ColorWell::ColorWell(QWidget* parent)
    : QFrame(parent), _size(16, 16)
{
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _checkerTexture = checkerBoardTexture(8, Qt::lightGray, Qt::gray);
}

void ColorWell::mousePressEvent(QMouseEvent* e)
{
    e->ignore();
    if (isSignalConnected(QMetaMethod::fromSignal(&ColorWell::clicked)))
    {
        emit clicked();
        e->accept();
    }
}

void ColorWell::mouseDoubleClickEvent(QMouseEvent* e)
{
    e->ignore();
    if (isSignalConnected(QMetaMethod::fromSignal(&ColorWell::doubleClicked)))
    {
        emit doubleClicked();
        e->accept();
    }
}

QSize ColorWell::sizeHint() const
{
    return _size;
}

void ColorWell::setInfo(ColorInfo info)
{
    _isEmpty = false;
    _info = info;
    setToolTip(_info.name());
    update();
}

void ColorWell::paintEvent(QPaintEvent* e)
{
    if (!_isEmpty)
    {
    QPainter painter(this);
        QRect area = e->rect().intersected(contentsRect());

        if (_info.color().alpha() < 0xFF)
        {
            painter.drawTiledPixmap(area, _checkerTexture);
        }
        painter.fillRect(area, _info.color());
    }

    QFrame::paintEvent(e);

    if (_isHighlighted)
    {
        QPainter painter(this);
        painter.setPen(QPen(palette().highlight(), 1.0));
        //painter.setBrush(Qt::transparent);
        painter.drawRect(0, 0, width() - 1, height() - 1);
    }
}

void ColorWell::setSize(QSize size)
{
    _size = size;
    update();
}

void ColorWell::setHighlighted(bool value)
{
    _isHighlighted = value;
    update();
}