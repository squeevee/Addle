#include "paletteview.hpp"

#include "widgetsgui/utilities/colorwell.hpp"
#include <QGridLayout>

PaletteView::PaletteView(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void PaletteView::setPresenter(PresenterAssignment<IPalettePresenter> presenter)
{
    if (presenter != _presenter)
    {
        _presenter = presenter;
        updatePalette();
    }
}

void PaletteView::updatePalette()
{
    if (!_presenter) return;

    const int height = _presenter->colors().height();
    const int width = _presenter->colors().width();

    QGridLayout* layout = new QGridLayout(this);
    setLayout(layout);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            const auto info = _presenter->colors().at(QPoint(x, y));

            ColorWell* colorWell = new ColorWell(this);
            colorWell->setColor(info.color());
            colorWell->setToolTip(info.name());
            layout->addWidget(colorWell, y, x);
        }
    }
}