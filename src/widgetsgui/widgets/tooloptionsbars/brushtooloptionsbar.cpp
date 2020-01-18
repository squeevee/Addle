#include "brushtooloptionsbar.hpp"

BrushToolOptionsBar::BrushToolOptionsBar(IBrushToolPresenter& presenter, QMainWindow* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{
    _actionGroup_brush = new OptionActionGroup(this, _presenter, "brush");
    _action_brush_basic = _actionGroup_brush->createAction(IBrushToolPresenter::DefaultBrushes::Basic);
    //_action_brush_aliasedCircle = _actionGroup_brush->createAction(IBrushToolPresenter::DefaultBrushes::AliasedCircle);
    //_action_brush_square = _actionGroup_brush->createAction(IBrushToolPresenter::DefaultBrushes::Square);

    QToolBar::addAction(_action_brush_basic);
    //QToolBar::addAction(_action_brush_aliasedCircle);
    //QToolBar::addAction(_action_brush_square);

    QToolBar::addSeparator();
}