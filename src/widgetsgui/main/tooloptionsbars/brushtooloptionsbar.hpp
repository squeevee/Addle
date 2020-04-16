#ifndef BRUSHTOOLOPTIONSBAR_HPP
#define BRUSHTOOLOPTIONSBAR_HPP

#include "widgetsgui/utilities/optionaction.hpp"
#include "widgetsgui/utilities/optiongroup.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
#include "tooloptionbarbase.hpp"
#include "widgetsgui/compat.hpp"

class ADDLE_WIDGETSGUI_EXPORT BrushToolOptionsBar : public ToolOptionBarBase
{
    Q_OBJECT 
    typedef IBrushPresenter::SizeOption SizeOption;

public:
    typedef IBrushToolPresenter PresenterType;

    BrushToolOptionsBar(IBrushToolPresenter& presenter, QWidget* parent);

    OptionAction* _action_brush_basic;
    //QAction* _action_brush_aliasedCircle;
    //QAction* _action_brush_square;
    OptionGroup* _optionGroup_brush;

    //QAction* _brush_size;
    //OptionGroup* _optionGroup_size;

private:
    IBrushToolPresenter& _presenter;
};

#endif // BRUSHTOOLOPTIONSBAR_HPP