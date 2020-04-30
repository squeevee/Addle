#ifndef TOOLSETUPHELPER_HPP
#define TOOLSETUPHELPER_HPP

#include <QToolBar>


#include "interfaces/services/iappearanceservice.hpp"
#include "widgetsgui/utilities/optionaction.hpp"
#include "widgetsgui/utilities/optiongroup.hpp"

#include "../maineditorview.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/qtextensions/translation.hpp"

class ToolSetupHelper
{
public: 
    ToolSetupHelper(
            MainEditorView* owner,
            IMainEditorPresenter& mainEditorPresenter,
            OptionGroup* selectGroup
        )
        : _owner(owner),
        _selectGroup(selectGroup),
        _mainEditorPresenter(mainEditorPresenter)
    {
    }

    template<class ToolBarType>
    void addTool(
            ToolId tool,
            OptionAction** selectActionptr,
            ToolBarType** toolbarptr
        )
    {
        typedef typename ToolBarType::PresenterType PresenterType;
        PresenterType* presenter = dynamic_cast<PresenterType*>(_mainEditorPresenter.getToolPresenter(tool));

        OptionAction* selectAction = new OptionAction(tool, _owner);
        selectAction->setText(template_translate("MainEditorPresenter", "%1.text", { tool.getKey() }));
        selectAction->setToolTip(template_translate("MainEditorPresenter", "%1.toolTip", { tool.getKey() }));
        selectAction->setIcon(ADDLE_ICON(tool.getKey()));
        *selectActionptr = selectAction;

        _selectGroup->addOption(selectAction);

        ToolBarType* optionsToolBar = new ToolBarType(*presenter, _owner);
        *toolbarptr = optionsToolBar;

        QObject::connect(
            optionsToolBar, 
            &ToolBarType::needsShown,
            _owner, 
            &MainEditorView::onToolBarNeedsShown
        );
        QObject::connect(
            optionsToolBar, 
            &ToolBarType::needsHidden,
            _owner, 
            &MainEditorView::onToolBarNeedsHidden
        );
    }

private:
    MainEditorView* _owner;
    QToolBar* _selectToolBar;
    OptionGroup* _selectGroup;

    IMainEditorPresenter& _mainEditorPresenter;
};

#endif // TOOLSETUPHELPER_HPP