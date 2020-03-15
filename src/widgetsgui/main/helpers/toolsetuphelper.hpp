#ifndef TOOLSETUPHELPER_HPP
#define TOOLSETUPHELPER_HPP

#include <QToolBar>

#include "widgetsgui/utilities/decorationhelper.hpp"
#include "widgetsgui/utilities/optionaction.hpp"
#include "widgetsgui/utilities/optiongroup.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"

class ToolSetupHelper
{
public: 
    ToolSetupHelper(
            QWidget* owner,
            IMainEditorPresenter& mainEditorPresenter,
            QToolBar* selectToolBar, 
            OptionGroup* selectGroup
        )
        : _owner(owner),
         _selectToolBar(selectToolBar),
        _selectGroup(selectGroup),
        _mainEditorPresenter(mainEditorPresenter),
        _select_decorHelper("currentTool", mainEditorPresenter)
    {
    }

    template<class ToolBarType, class PresenterType>
    void addTool(
            ToolId tool,
            OptionAction** selectActionptr,
            ToolBarType** optionsptr,
            PresenterType** presenterptr
        )
    {
        PresenterType* toolPresenter = dynamic_cast<PresenterType*>(_mainEditorPresenter.getToolPresenter(tool));
        *presenterptr = toolPresenter;

        OptionAction* selectAction = new OptionAction(tool, _owner);
        _select_decorHelper.decorateOption(selectAction);
        *selectActionptr = selectAction;

        _selectToolBar->addAction(selectAction);
        _selectGroup->addOption(selectAction);

        ToolBarType* optionsToolBar = new ToolBarType(*toolPresenter, _owner);
        *optionsptr = optionsToolBar;
    }

private:
    DecorationHelper _select_decorHelper;

    QWidget* _owner;
    QToolBar* _selectToolBar;
    OptionGroup* _selectGroup;

    IMainEditorPresenter& _mainEditorPresenter;
};

#endif // TOOLSETUPHELPER_HPP