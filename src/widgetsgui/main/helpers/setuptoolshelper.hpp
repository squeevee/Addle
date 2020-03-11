#ifndef SETUPTOOLSHELPER_HPP
#define SETUPTOOLSHELPER_HPP

#include "interfaces/presenters/idocumentpresenter.hpp"

#include "../basedocumentview.hpp"

#include "widgetsgui/utilities/decorationhelper.hpp"
#include "widgetsgui/utilities/optionaction.hpp"

class SetupToolsHelper
{
public: 
    SetupToolsHelper(BaseDocumentView& view, IDocumentPresenter& presenter)
        : _view(view),
        _presenter(presenter),
        _select_decorHelper("currentTool", presenter)
    {
    }

    template<class ToolBarType, class PresenterType>
    void addTool(ToolId tool, OptionAction** selectActionptr, ToolBarType** optionsptr, PresenterType** presenterptr)
    {
        PresenterType* toolPresenter = dynamic_cast<PresenterType*>(_presenter.getToolPresenter(tool));
        *presenterptr = toolPresenter;

        OptionAction* selectAction = new OptionAction(tool, &_view);
        _select_decorHelper.decorateOption(selectAction);
        *selectActionptr = selectAction;

        _view._toolBar_toolSelection->addAction(selectAction);
        _view._optionGroup_toolSelection->addOption(selectAction);

        ToolBarType* optionsToolBar = new ToolBarType(*toolPresenter, &_view);
        *optionsptr = optionsToolBar;
    }

private:

    DecorationHelper _select_decorHelper;
    IDocumentPresenter& _presenter;
    BaseDocumentView& _view;
};

#endif // SETUPTOOLSHELPER_HPP