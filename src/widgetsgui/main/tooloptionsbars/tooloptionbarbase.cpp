#include "utilities/qtextensions/qobject.hpp"

#include "tooloptionbarbase.hpp"

using namespace Addle;

ToolOptionBarBase::ToolOptionBarBase(IToolPresenter& presenter, QWidget* parent)
    : QToolBar(parent),
    _presenter(presenter),
    _owner(parent)
{
    connect_interface(
        &_presenter,
        SIGNAL(isSelectedChanged(bool)),
        this,
        SLOT(onSelectedChanged(bool))
    );
    QToolBar::hide();
}

void ToolOptionBarBase::onSelectedChanged(bool selected)
{
    if (selected)
        emit needsShown();
    else
        emit needsHidden();
}