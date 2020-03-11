#ifndef ZOOMROTATEWIDGET_HPP
#define ZOOMROTATEWIDGET_HPP

#include <QWidget>
#include <QAction>
#include <QToolButton>
#include <QDoubleSpinBox>

#include <QToolBar>

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "widgetsgui/utilities/zoomslider.hpp"

class ViewPortWidget;
class ZoomRotateWidget : public QToolBar //QWidget 
{
    Q_OBJECT
public:
    ZoomRotateWidget(IViewPortPresenter& presenter, QWidget* parent = nullptr);
    virtual ~ZoomRotateWidget() = default;

    void connectViewPortActions(ViewPortWidget& viewPortWidget);

    QAction* getAction_zoomIn() { return _action_zoomIn; }
    QAction* getAction_zoomOut() { return _action_zoomOut; }
    QAction* getAction_turntableCW() { return _action_turntableCW; }
    QAction* getAction_turntableCCW() { return _action_turntableCCW; }
    QAction* getAction_reset() { return _action_reset; }

private:
    IViewPortPresenter& _presenter;

    QAction* _action_zoomIn;
    QAction* _action_zoomOut;
    QAction* _action_turntableCW;
    QAction* _action_turntableCCW;
    QAction* _action_reset;

    QToolButton* _button_zoomIn;
    QToolButton* _button_zoomOut;
    QToolButton* _button_turntableCW;
    QToolButton* _button_turntableCCW;
    QToolButton* _button_reset;

    ZoomSlider* _slider_zoom;
    QDoubleSpinBox* _spinBox_zoomPercent;
};

#endif // ZOOMROTATEWIDGET_HPP