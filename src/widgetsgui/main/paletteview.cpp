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

    QGridLayout* layout = new QGridLayout(this);
    setLayout(layout);

    int y = 0;
    for (auto row : ConstRowViewer<ColorInfo>(_presenter->colors()))
    {
        int x = 0;
        for (auto info : row)
        {
            ColorWell* colorWell = new ColorWell(this);
            colorWell->setColor(info.color());
            colorWell->setToolTip(info.name());
            layout->addWidget(colorWell, y, x);
            x++;
        }
        y++;
    }

}