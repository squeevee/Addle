/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef COLORSELECTOR_HPP
#define COLORSELECTOR_HPP

#include "compat.hpp"
#include <QDockWidget>
#include <QToolButton>

#include "interfaces/presenters/icolorselectionpresenter.hpp"

namespace Addle {

class PaletteView;
class ColorWell;
class ColorSelectionIndicator;
class ADDLE_WIDGETSGUI_EXPORT ColorSelector : public QDockWidget
{
    Q_OBJECT
public:
    ColorSelector(IColorSelectionPresenter& presenter, QWidget* parent = nullptr);
    virtual ~ColorSelector() = default;

private slots:
    void setActiveColor(int active);

    void onColor1Click();
    void onColor2Click();
    
    void onPaletteColorSelected(ColorInfo color);
    void onPresenterColorSelected(int which, ColorInfo info);
    void onButton_transparent();

private:
    PaletteView* _palette;
    ColorSelectionIndicator* _indicator;

    ColorWell* _palette_well1 = nullptr;
    ColorWell* _palette_well2 = nullptr;

    IColorSelectionPresenter& _presenter;
};

class ADDLE_WIDGETSGUI_EXPORT ColorSelectionIndicator : public QWidget
{
    Q_OBJECT
public:
    ColorSelectionIndicator(ColorSelector& owner);
    virtual ~ColorSelectionIndicator() = default;

    QSize sizeHint() const override;

private:
    ColorWell* _well1;
    ColorWell* _well2;

    QToolButton* _button_transparent;

    QList<ColorWell*> _history;

    ColorSelector& _owner;

    friend class ColorSelector;
};

} // namespace Addle

#endif // COLORSELECTOR_HPP
