/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef NAVIGATETOOLOPTIONSBAR_HPP
#define NAVIGATETOOLOPTIONSBAR_HPP

#include "compat.hpp"
#include <QActionGroup>

#include "utilities/optionaction.hpp"
#include "utilities/optiongroup.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "tooloptionbarbase.hpp"

namespace Addle {

class ADDLE_WIDGETSGUI_EXPORT NavigateToolOptionsBar : public ToolOptionBarBase 
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

} // namespace Addle

#endif // NAVIGATETOOLOPTIONSBAR_HPP