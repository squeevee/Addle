#include "colorselector.hpp"

#include "widgetsgui/utilities/colorwell.hpp"

#include <QEvent>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "paletteview.hpp"

#include "globalconstants.hpp"

#include "utilities/presenter/propertybinding.hpp"

ColorSelector::ColorSelector(IColorSelectionPresenter& presenter, QWidget* parent)
    : QDockWidget(parent), _presenter(presenter)
{
    QWidget* widget = new QWidget(this);
    setWidget(widget);

    QHBoxLayout* layout = new QHBoxLayout;
    widget->setLayout(layout);

    _indicator = new ColorSelectionIndicator(*this);

    layout->addWidget(_indicator);

    _palette = new PaletteView(this);
    if (_presenter.palette())
    {
        _palette->setPresenter(_presenter.palette());
    }

    layout->addWidget(_palette);

    layout->addItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    new PropertyBinding(
        _indicator->_well1,
        "info",
        qobject_interface_cast(&_presenter),
        IColorSelectionPresenter::Meta::Properties::color1,
        PropertyBinding::ReadOnly
    );

    new PropertyBinding(
        _indicator->_well2,
        "info",
        qobject_interface_cast(&_presenter),
        IColorSelectionPresenter::Meta::Properties::color2,
        PropertyBinding::ReadOnly
    );

    connect_interface(&_presenter, SIGNAL(activeChanged(int)), this, SLOT(setActiveColor(int)));
    connect_interface(&_presenter, SIGNAL(colorChanged(int, ColorInfo)), this, SLOT(onPresenterColorSelected(int, ColorInfo)));
    setActiveColor(_presenter.active());

    connect(_indicator->_well1, &ColorWell::clicked, this, &ColorSelector::onColor1Click);
    connect(_indicator->_well2, &ColorWell::clicked, this, &ColorSelector::onColor2Click);
    connect(_indicator->_button_transparent, &QAbstractButton::clicked, this, &ColorSelector::onButton_transparent);

    connect(_palette, &PaletteView::colorSelected, this, &ColorSelector::onPaletteColorSelected);
    connect(_palette, &PaletteView::colorSelected, this, &ColorSelector::onPaletteColorSelected);

}

void ColorSelector::setActiveColor(int active)
{
    //warn if invalid active
    active = qBound(1, active, 2);
    if (active == 1)
    {
        _indicator->_well1->setHighlighted(true);
        _indicator->_well2->setHighlighted(false);
    }
    else 
    {
        _indicator->_well1->setHighlighted(false);
        _indicator->_well2->setHighlighted(true);
    }
}

void ColorSelector::onColor1Click()
{
    _presenter.setActive(1);
}

void ColorSelector::onColor2Click()
{
    _presenter.setActive(2);
}

void ColorSelector::onPaletteColorSelected(ColorInfo color)
{
    _presenter.setActiveColor(color);
}

void ColorSelector::onButton_transparent()
{
    _presenter.setActiveColor(GlobalConstants::Transparent);
}

void ColorSelector::onPresenterColorSelected(int which, ColorInfo info)
{
    switch (which)
    {
    case 1:
        {
            if (_palette_well1 && _palette_well1 != _palette_well2)
                _palette_well1->setHighlighted(false);

            if (info.pos())
                _palette_well1 =_palette->colorWellAt(*info.pos());
            else 
                _palette_well1 = nullptr;

            if (_palette_well1)
                _palette_well1->setHighlighted(true);
            break;
        }
    case 2:
        {
            if (_palette_well2 && _palette_well1 != _palette_well2)
                _palette_well2->setHighlighted(false);
                
            if (info.pos())
                _palette_well2 =_palette->colorWellAt(*info.pos());
            else
                _palette_well2 = nullptr;

            if (_palette_well2)
                _palette_well2->setHighlighted(true);
            break;
        }
    }
}

ColorSelectionIndicator::ColorSelectionIndicator(ColorSelector& owner)
    : QWidget(&owner), _owner(owner)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _button_transparent = new QToolButton(this);
    _button_transparent->setMaximumSize(QSize(16, 16));
    _button_transparent->move(45, 3);

    _well2 = new ColorWell(this);
    _well2->setFrameStyle(QFrame::Raised | QFrame::Box);
    _well2->setSize(QSize(32, 32));
    _well2->move(4, 13);

    _well1 = new ColorWell(this);
    _well1->setFrameStyle(QFrame::Raised | QFrame::Box);
    _well1->setSize(QSize(34, 34));
    _well1->move(16, 25);
}

QSize ColorSelectionIndicator::sizeHint() const
{
    return QSize(64, 64);
}
