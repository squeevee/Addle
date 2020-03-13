#include "utilities/qtextensions/qobject.hpp"

#include "tooloptionbarbase.hpp"

ToolOptionBarBase::ToolOptionBarBase(IToolPresenter& presenter, QMainWindow* parent)
    : QToolBar(parent),
    _presenter(presenter),
    _owner(*parent)
{
    _q_presenter = qobject_interface_cast(&_presenter);

    connect_interface(
        &_presenter,
        SIGNAL(selectionChanged(bool)),
        this,
        SLOT(onSelectedChanged(bool))
    );
    QToolBar::hide();
}

void ToolOptionBarBase::onSelectedChanged(bool selected)
{
    if (selected)
    {
        _owner.addToolBar(Qt::ToolBarArea::TopToolBarArea, this);
        QToolBar::show();
    }
    else
    {
        _owner.removeToolBar(this);
        QToolBar::hide();
    }
}