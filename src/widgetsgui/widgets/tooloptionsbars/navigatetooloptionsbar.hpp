#ifndef NAVIGATETOOLOPTIONSBAR_HPP
#define NAVIGATETOOLOPTIONSBAR_HPP

#include <QActionGroup>

#include "../propertyactiongroup.hpp"
#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"
#include "tooloptionbarbase.hpp"

class NavigateToolOptionsBar : public ToolOptionBarBase 
{
    typedef INavigateToolPresenter::NavigateOperationOptions NavigateOperationOptions;
    Q_OBJECT 
public: 
    NavigateToolOptionsBar(INavigateToolPresenter& presenter, QMainWindow* parent);

    QAction* _action_navigateOperation_gripPan;
    QAction* _action_navigateOperation_gripPivot;
    QAction* _action_navigateOperation_rectangleZoomTo;

    PropertyActionGroup* _actionGroup_navigateOperation;

private:
    INavigateToolPresenter& _presenter;
};


#endif // NAVIGATETOOLOPTIONSBAR_HPP