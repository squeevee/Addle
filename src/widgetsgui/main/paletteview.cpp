/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "paletteview.hpp"

#include "utilities/colorwell.hpp"
#include <QGridLayout>

#include <QMouseEvent>
#include <QHelpEvent>
#include <QEvent>
#include <QToolTip>

#include <cmath>
#include <QtDebug>

using namespace Addle;

PaletteView::PaletteView(QWidget* parent)
    : QWidget(parent), _lastColorEmitted(-1, -1)
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

void PaletteView::setSelectionEnabled(bool enabled)
{
    if (enabled != _selectionEnabled)
    {
        _selectionEnabled = enabled;
        update();
    }
}

bool PaletteView::event(QEvent* e)
{
    // Handle these events here instead of in the well so:
        // 1) spacing between the wells isn't an annoying input dead-space to
        // the user (but can still look good by being there)
        // 2) click and dragging across the pallete selects all colors along
        // the way.

    switch (e->type())
    {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
        {
            QPoint colorPos = colorPosAt(static_cast<QMouseEvent*>(e)->pos());

            if (_selectionEnabled) break;
            if (e->type() == QEvent::MouseMove && colorPos == _lastColorEmitted) break;

            emit colorSelected(_presenter->colors().at(colorPos));
            _lastColorEmitted = colorPos;
            return true;
        }
    }

    return QWidget::event(e);
}

ColorWell* PaletteView::colorWellAt(QPoint pos)
{
    QLayoutItem* item = static_cast<QGridLayout*>(layout())->itemAtPosition(pos.y(), pos.x());
    if (!item || !item->widget()) return nullptr;
    return qobject_cast<ColorWell*>(item->widget());
}

void PaletteView::updatePalette()
{
    if (!_presenter) return;

    QGridLayout* layout = new QGridLayout(this);
    layout->setSpacing(3);
    setLayout(layout);

    for (auto row : ConstRowViewer<ColorInfo>(_presenter->colors()))
    {
        for (auto info : row)
        {
            QPoint pos = *info.pos();
            ColorWell* well = new ColorWell(this);
            well->setInfo(info);
            layout->addWidget(well, pos.y(), pos.x());
            _wells_byColor[info] = well;
        }
    }
}

QPoint PaletteView::colorPosAt(QPoint pos)
{
    //todo: clean this up
    QRect contents = layout()->contentsRect();
    double spacing = layout()->spacing();
    double cx = (double)contents.x() - spacing / 2;
    double cy = (double)contents.y() - spacing / 2;
    double cw = (double)contents.width() + spacing;
    double ch = (double)contents.height() + spacing;
    int width = _presenter->colors().width();
    int height = _presenter->colors().height();

    int x = std::floor(
        (((double)pos.x() - cx) / cw ) * width
    );
    int y = std::floor(
        (((double)pos.y() - cy) / ch ) * height
    );

    return QPoint(qBound(0, x, width - 1), qBound(0, y, height - 1));
}
