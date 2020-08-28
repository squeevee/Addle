/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "utilities/qobject.hpp"

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