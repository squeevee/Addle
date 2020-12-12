/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef TOOLSETUPHELPER_HPP
#define TOOLSETUPHELPER_HPP

#include <QToolBar>

#include "utilities/addle_icon.hpp"
#include "utilities/optionaction.hpp"
#include "utilities/optiongroup.hpp"

#include "utilities/strings.hpp"

#include "../maineditorview.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

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
//         typedef typename ToolBarType::PresenterType PresenterType;
//         auto presenter = _mainEditorPresenter.tools().value(tool).dynamicCast<PresenterType>();
// 
//         OptionAction* selectAction = new OptionAction(tool, _owner);
//         selectAction->setText(dynamic_qtTrId({"tools", tool.key(), "name"}));
//         selectAction->setToolTip(dynamic_qtTrId({"tools", tool.key(), "description"}));
//         selectAction->setIcon(ADDLE_ICON(tool.key()));
//         *selectActionptr = selectAction;
// 
//         _selectGroup->addOption(selectAction);
// 
//         ToolBarType* optionsToolBar = new ToolBarType(*presenter, _owner);
//         *toolbarptr = optionsToolBar;
// 
//         QObject::connect(
//             optionsToolBar, 
//             &ToolBarType::needsShown,
//             _owner, 
//             &MainEditorView::onToolBarNeedsShown
//         );
//         QObject::connect(
//             optionsToolBar, 
//             &ToolBarType::needsHidden,
//             _owner, 
//             &MainEditorView::onToolBarNeedsHidden
//         );
    }

private:
    MainEditorView* _owner;
    QToolBar* _selectToolBar;
    OptionGroup* _selectGroup;

    IMainEditorPresenter& _mainEditorPresenter;
};

} // namespace Addle

#endif // TOOLSETUPHELPER_HPP
