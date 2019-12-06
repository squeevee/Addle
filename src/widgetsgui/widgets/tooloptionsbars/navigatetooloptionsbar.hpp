#ifndef NAVIGATETOOLOPTIONSBAR_HPP
#define NAVIGATETOOLOPTIONSBAR_HPP

#include <QActionGroup>

#include "helpers/enumactiongroup.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
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

    OptionActionGroup* _actionGroup_navigateOperation;

private:
    INavigateToolPresenter& _presenter;
};


#endif // NAVIGATETOOLOPTIONSBAR_HPP