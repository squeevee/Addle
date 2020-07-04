#ifndef BRUSHTOOLOPTIONSBAR_HPP
#define BRUSHTOOLOPTIONSBAR_HPP

#include "compat.hpp"
#include "widgetsgui/utilities/optionaction.hpp"
#include "widgetsgui/utilities/optiongroup.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "tooloptionbarbase.hpp"
#include "../sizeselector.hpp"
#include "../assetselector.hpp"

class PopupButton;
class SizeSelectorButton;
class ADDLE_WIDGETSGUI_EXPORT BrushToolOptionsBar : public ToolOptionBarBase 
{
    Q_OBJECT 
    //typedef IBrushPresenter::SizeOption SizeOption;

public:
    typedef IBrushToolPresenter PresenterType;

    BrushToolOptionsBar(IBrushToolPresenter& presenter, QWidget* parent);

    OptionAction* _action_brush_basic;
    OptionAction* _action_brush_soft;
    OptionGroup* _optionGroup_brush;

    AssetSelector* _brushSelector;
    PopupButton* _button_brushSelector;

    SizeSelectorButton* _button_sizeSelector;

private slots:
    void onBrushChanged();

private:
    IBrushToolPresenter& _presenter;
};

#endif // BRUSHTOOLOPTIONSBAR_HPP