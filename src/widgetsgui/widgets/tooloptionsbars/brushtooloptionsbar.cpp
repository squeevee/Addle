#include "brushtooloptionsbar.hpp"

BrushToolOptionsBar::BrushToolOptionsBar(IBrushToolPresenter& presenter, QMainWindow* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{
    _actionGroup_brushTip = new OptionActionGroup(this, _presenter, "brushTip");
    _action_brushTip_pixel = _actionGroup_brushTip->createAction(IBrushLikeToolPresenter::CommonBrushTips::Pixel);
    _action_brushTip_TwoSquare = _actionGroup_brushTip->createAction(IBrushLikeToolPresenter::CommonBrushTips::TwoSquare);
    _action_brushTip_square = _actionGroup_brushTip->createAction(IBrushLikeToolPresenter::CommonBrushTips::Square);
    _action_brushTip_hardCircle = _actionGroup_brushTip->createAction(IBrushLikeToolPresenter::CommonBrushTips::HardCircle);
    _action_brushTip_softCircle = _actionGroup_brushTip->createAction(IBrushLikeToolPresenter::CommonBrushTips::SoftCircle);

    QToolBar::addAction(_action_brushTip_pixel);
    QToolBar::addAction(_action_brushTip_TwoSquare);
    QToolBar::addAction(_action_brushTip_square);
    QToolBar::addAction(_action_brushTip_hardCircle);
    QToolBar::addAction(_action_brushTip_softCircle);

    QToolBar::addSeparator();
}