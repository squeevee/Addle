#include <QGridLayout>
#include <QSignalBlocker>

#include "viewportscrollwidget.hpp"
#include "zoomrotatewidget.hpp"

#include "utilities/qtextensions/qobject.hpp"

#include "utilities/presenter/propertybinding.hpp"
#include "widgetsgui/utilities/widgetproperties.hpp"

#include "viewport.hpp"

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

    connect(_scrollbar_horizontal, SIGNAL(valueChanged(int)), qobject_interface_cast(&_presenter), SLOT(scrollX(int)));
    connect(_scrollbar_vertical, SIGNAL(valueChanged(int)), qobject_interface_cast(&_presenter), SLOT(scrollY(int)));
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
    const IScrollState& state = _presenter.getScrollState();

    const QSignalBlocker horizontalBlocker(_scrollbar_horizontal);
    const QSignalBlocker verticalBlocker(_scrollbar_vertical);

    _scrollbar_horizontal->setMinimum(state.getHorizontalMinimum());
    _scrollbar_horizontal->setMaximum(state.getHorizontalMaximum());
    _scrollbar_horizontal->setPageStep(state.getPageWidth());
    _scrollbar_horizontal->setValue(state.getHorizontalValue());

    _scrollbar_vertical->setMinimum(state.getVerticalMinimum());
    _scrollbar_vertical->setMaximum(state.getVerticalMaximum());
    _scrollbar_vertical->setPageStep(state.getPageHeight());
    _scrollbar_vertical->setValue(state.getVerticalValue());
}