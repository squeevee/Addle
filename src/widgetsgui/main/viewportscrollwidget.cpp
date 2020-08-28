/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QGridLayout>
#include <QSignalBlocker>

#include "viewportscrollwidget.hpp"
#include "zoomrotatewidget.hpp"

#include "utilities/qobject.hpp"

#include "utilities/presenter/propertybinding.hpp"
#include "utilities/widgetproperties.hpp"

#include "viewport.hpp"

using namespace Addle;

ViewPortScrollWidget::ViewPortScrollWidget(IViewPortPresenter& presenter, QWidget* parent)
    : QWidget(parent), _presenter(presenter)
{
    new PropertyBinding(
        this,
        WidgetProperties::enabled,
        qobject_interface_cast(&_presenter),
        IViewPortPresenter::Meta::Properties::canNavigate,
        PropertyBinding::ReadOnly
    );

    _layout = new QGridLayout(this);
    QWidget::setLayout(_layout);

    _scrollbar_horizontal = new QScrollBar(Qt::Orientation::Horizontal, this);
    _scrollbar_horizontal->setSingleStep(100);
    _scrollbar_horizontal->setMinimum(0);
    _scrollbar_horizontal->setMaximum(0);
    _layout->addWidget(_scrollbar_horizontal, 1, 0);

    _scrollbar_vertical = new QScrollBar(Qt::Orientation::Vertical, this);
    _scrollbar_vertical->setSingleStep(100);
    _scrollbar_vertical->setMinimum(0);
    _scrollbar_vertical->setMaximum(0);
    _layout->addWidget(_scrollbar_vertical, 0, 1);

    connect_interface(&_presenter, SIGNAL(scrollStateChanged()), this, SLOT(onScrollStateChanged()));

    connect_interface(_scrollbar_horizontal, SIGNAL(valueChanged(int)), &_presenter, SLOT(scrollX(int)));
    connect_interface(_scrollbar_vertical, SIGNAL(valueChanged(int)), &_presenter, SLOT(scrollY(int)));
}

void ViewPortScrollWidget::setViewPort(ViewPort* viewPort)
{
    _viewPort = viewPort;

    QWidget* viewPortWidget = dynamic_cast<QWidget*>(viewPort);
    //assert

    viewPortWidget->setParent(this);
    _layout->addWidget(viewPortWidget, 0, 0);
}

void ViewPortScrollWidget::onScrollStateChanged()
{
    const IViewPortPresenter::IScrollState& state = _presenter.scrollState();

    const QSignalBlocker horizontalBlocker(_scrollbar_horizontal);
    const QSignalBlocker verticalBlocker(_scrollbar_vertical);

    _scrollbar_horizontal->setMinimum(state.horizontalMinimum());
    _scrollbar_horizontal->setMaximum(state.horizontalMaximum());
    _scrollbar_horizontal->setPageStep(state.pageWidth());
    _scrollbar_horizontal->setValue(state.horizontalValue());

    _scrollbar_vertical->setMinimum(state.verticalMinimum());
    _scrollbar_vertical->setMaximum(state.verticalMaximum());
    _scrollbar_vertical->setPageStep(state.pageHeight());
    _scrollbar_vertical->setValue(state.verticalValue());
}
