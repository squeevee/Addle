/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHTOOLOPTIONSBAR_HPP
#define BRUSHTOOLOPTIONSBAR_HPP

#include "compat.hpp"
#include "utilities/optionaction.hpp"
#include "utilities/optiongroup.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "tooloptionbarbase.hpp"
#include "../sizeselector.hpp"
#include "../assetselector.hpp"

namespace Addle {

class PopupButton;
class SizeSelectorButton;
class FavoriteAssetsPicker;
class ADDLE_WIDGETSGUI_EXPORT BrushToolOptionsBar : public ToolOptionBarBase 
{
    Q_OBJECT 
    //typedef IBrushPresenter::SizeOption SizeOption;

public:
    typedef IBrushToolPresenter PresenterType;

    BrushToolOptionsBar(IBrushToolPresenter& presenter, QWidget* parent);

    // OptionAction* _action_brush_basic;
    // OptionAction* _action_brush_soft;
    // OptionGroup* _optionGroup_brush;

    FavoriteAssetsPicker* _favoriteBrushes;

    AssetSelector* _brushSelector;
    PopupButton* _button_brushSelector;

    SizeSelectorButton* _button_sizeSelector;

private slots:
    void onBrushChanged();
    void onRefreshPreviews();

private:
    IBrushToolPresenter& _presenter;
};

} // namespace Addle

#endif // BRUSHTOOLOPTIONSBAR_HPP