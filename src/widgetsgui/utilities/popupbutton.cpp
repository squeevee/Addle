#include "popupbutton.hpp"

#include <QMenu>
#include <QWidgetAction>
#include <QtDebug>

using namespace Addle;

PopupButton::PopupButton(
        QWidget* parent
    ) : QToolButton(parent)
{
    setPopupMode(QToolButton::InstantPopup);

    _menu = new QMenu(this);
    setMenu(_menu);
}

void PopupButton::setPopup(QWidget* popup)
{   
    if (popup == _popup) return;

    if (_action)
        delete _action;

    _popup = popup;

    if (_popup)
    {
        _action = new QWidgetAction(this);
        _action->setDefaultWidget(_popup);
        _menu->addAction(_action);
    }
    else
    {
        _action = nullptr;
    }
}

void PopupButton::closePopup()
{
    _menu->close();
}