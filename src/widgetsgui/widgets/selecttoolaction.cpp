#include "selecttoolaction.hpp"

SelectToolAction::SelectToolAction(IToolPresenter& presenter, QWidget* parent)
    : QAction(parent), _presenter(presenter)
{
    setCheckable(true);
    setChecked(_presenter.isSelected());
    setIcon(_presenter.getIcon());
    setText(_presenter.getName());
    setToolTip(_presenter.getToolTip());

    connect_interface(
        &_presenter,
        SIGNAL(selectionChanged(bool)),
        this,
        SLOT(onSelectedChanged(bool))
    );

    connect_interface(
        this,
        SIGNAL(triggered()),
        &_presenter,
        SLOT(select())
    );
}