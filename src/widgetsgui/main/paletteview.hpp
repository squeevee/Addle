/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PALETTEVIEW_HPP
#define PALETTEVIEW_HPP

#include <QWidget>
#include <QDockWidget>
#include <QHash>

#include "interfaces/presenters/ipalettepresenter.hpp"
#include "utilities/presenter/presenterassignment.hpp"

#include "utilities/model/colorinfo.hpp"

namespace Addle {

class ColorWell;
class PaletteView : public QWidget
{
    Q_OBJECT
public:
    PaletteView(QWidget* parent = nullptr);
    virtual ~PaletteView() = default;

    void setPresenter(PresenterAssignment<IPalettePresenter> presenter);

    bool selectionIsEnabled() const { return _selectionEnabled; }
    void setSelectionEnabled(bool enabled);

    bool event(QEvent* e);

    ColorWell* colorWellAt(QPoint pos);

signals:
    void colorSelected(ColorInfo color);
    void colorDoubleClicked(ColorInfo color);

private:
    QPoint colorPosAt(QPoint pos);

    void updatePalette();

    QPoint _lastColorEmitted;
    //QHash<QPoint, ColorWell*> _wells_byPosition;
    QHash<ColorInfo, ColorWell*> _wells_byColor;

    PresenterAssignment<IPalettePresenter> _presenter;

    bool _selectionEnabled = false;

    friend class ColorSelector;
    class ColorWellMouseFilter;
};

} // namespace Addle

#endif // PALETTEVIEW_HPP
