#ifndef TOOLSETUPHELPER_HPP
#define TOOLSETUPHELPER_HPP

#include <QToolBar>

#include "utilities/addle_icon.hpp"
#include "utilities/addle_text.hpp"
#include "utilities/optionaction.hpp"
#include "utilities/optiongroup.hpp"

#include "../maineditorview.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/qtextensions/translation.hpp"

namespace Addle {

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
        auto presenter = _mainEditorPresenter.tools().value(tool).dynamicCast<PresenterType>();

        OptionAction* selectAction = new OptionAction(tool, _owner);
        selectAction->setText(ADDLE_TEXT(QStringLiteral("tools.%1.text").arg(tool.key())));
        selectAction->setToolTip(ADDLE_TEXT(QStringLiteral("tools.%1.toolTip").arg(tool.key())));
        selectAction->setIcon(ADDLE_ICON(tool.key()));
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

} // namespace Addle

#endif // TOOLSETUPHELPER_HPP