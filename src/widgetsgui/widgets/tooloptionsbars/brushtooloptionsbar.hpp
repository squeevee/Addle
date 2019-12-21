#ifndef BRUSHTOOLOPTIONSBAR_HPP
#define BRUSHTOOLOPTIONSBAR_HPP

#include "helpers/enumactiongroup.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "tooloptionbarbase.hpp"

class BrushToolOptionsBar : public ToolOptionBarBase 
{
    Q_OBJECT 
    typedef IBrushPresenter::SizeOption SizeOption;
public: 
    BrushToolOptionsBar(IBrushToolPresenter& presenter, QMainWindow* parent);

    QAction* _action_brush_smoothCircle;
    QAction* _action_brush_aliasedCircle;
    QAction* _action_brush_square;
    OptionActionGroup* _actionGroup_brush;

    QAction* _brush_size;
    OptionActionGroup* _actionGroup_size;

private:
    IBrushToolPresenter& _presenter;
};

#endif // BRUSHTOOLOPTIONSBAR_HPP