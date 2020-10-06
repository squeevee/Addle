#include "iconlabel.hpp"

#include <QResizeEvent>

using namespace Addle;

IconLabel::IconLabel(const QIcon& icon, QWidget* parent)
    : QLabel(parent), _icon(icon)
{
    if (!size().isNull() && !_icon.isNull())
    {
        setPixmap(_icon.pixmap(size()));
    }
}

void IconLabel::setIcon(QIcon icon)
{
    _icon = icon;
    setPixmap(QPixmap());
}

void IconLabel::resizeEvent(QResizeEvent* event)
{
    if (!_icon.isNull())
    {
        setPixmap(_icon.pixmap(event->size()));
    }
}
