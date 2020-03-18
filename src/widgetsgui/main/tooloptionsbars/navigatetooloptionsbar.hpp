#ifndef NAVIGATETOOLOPTIONSBAR_HPP
#define NAVIGATETOOLOPTIONSBAR_HPP

#include <QActionGroup>

#include "widgetsgui/utilities/optionaction.hpp"
#include "widgetsgui/utilities/optiongroup.hpp"
#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"
#include "tooloptionbarbase.hpp"

class NavigateToolOptionsBar : public ToolOptionBarBase 
{
    typedef INavigateToolPresenter::NavigateOperationOptions NavigateOperationOptions;
    Q_OBJECT 
public: 
    typedef INavigateToolPresenter PresenterType;

    NavigateToolOptionsBar(INavigateToolPresenter& presenter, QWidget* parent);

    OptionAction* _action_navigateOperation_gripPan;
    OptionAction* _action_navigateOperation_gripPivot;
    OptionAction* _action_navigateOperation_rectangleZoomTo;

    OptionGroup* _optionGroup_navigateOperation;

private:
    INavigateToolPresenter& _presenter;
};


#endif // NAVIGATETOOLOPTIONSBAR_HPP