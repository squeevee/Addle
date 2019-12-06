#ifndef BRUSHTOOLOPTIONSBAR_HPP
#define BRUSHTOOLOPTIONSBAR_HPP

#include "helpers/enumactiongroup.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "tooloptionbarbase.hpp"

class BrushToolOptionsBar : public ToolOptionBarBase 
{
    Q_OBJECT 
    typedef IBrushLikeToolPresenter::SizeOption SizeOption;
public: 
    BrushToolOptionsBar(IBrushToolPresenter& presenter, QMainWindow* parent);

    QAction* _action_brushTip_pixel;
    QAction* _action_brushTip_TwoSquare;
    QAction* _action_brushTip_square;
    QAction* _action_brushTip_hardCircle;
    QAction* _action_brushTip_softCircle;
    OptionActionGroup* _actionGroup_brushTip;

    QAction* _brush_size;
    OptionActionGroup* _actionGroup_size;

private:
    IBrushToolPresenter& _presenter;
};

#endif // BRUSHTOOLOPTIONSBAR_HPP