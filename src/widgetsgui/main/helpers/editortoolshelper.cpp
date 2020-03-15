#include <QAction>

#include "editortoolshelper.hpp"
#include "toolsetuphelper.hpp"

#include "../tooloptionsbars/brushtooloptionsbar.hpp"
#include "../tooloptionsbars/navigatetooloptionsbar.hpp"

#include "../maineditorview.hpp"

EditorToolsHelper::EditorToolsHelper(MainEditorView& view)
    : _view(view)
{
    QToolBar* selectToolBar = _view._toolBar_toolSelection;
    OptionGroup* selectGroup = _view._optionGroup_toolSelection;

    ToolSetupHelper setupHelper(
        &_view,
        *_view.getPresenter(),
        selectToolBar,
        selectGroup
    );

    // addTool(
    //     IMainEditorPresenter::DefaultTools::SELECT,
    //     &_action_selectSelectTool,
    //     &_optionsToolBar_select,
    //     &_toolPresenter_select
    // );
    // _action_selectSelectTool->setIcon(QIcon(":/icons/select.png"));
    // {
    // }

    selectToolBar->addAction(new QAction(QIcon(":/icons/select.png"), "", &_view));

    selectToolBar->addSeparator();

    setupHelper.addTool(
        IMainEditorPresenter::DefaultTools::BRUSH,
        &action_selectBrushTool,
        &optionsToolBar_brush,
        &toolPresenter_brush
    );

    // addTool(
    //     IMainEditorPresenter::DefaultTools::ERASER,
    //     &_action_selectEraserTool,
    //     &_optionsToolBar_eraser,
    //     &_toolPresenter_eraser
    // );
    // _action_selectEraserTool->setIcon(QIcon(":/icons/eraser.png"));
    // {
    // }

    selectToolBar->addAction(new QAction(QIcon(":/icons/eraser.png"), "", &_view));

    // addTool(
    //     IMainEditorPresenter::DefaultTools::FILL,
    //     &_action_selectFillTool,
    //     &_optionsToolBar_fill,
    //     &_toolPresenter_fill
    // );
    // _action_selectFillTool->setIcon(QIcon(":/icons/fill.png"));
    // {
    // }
    
    selectToolBar->addAction(new QAction(QIcon(":/icons/fill.png"), "", &_view));

    selectToolBar->addSeparator();


    // addTool(
    //     IMainEditorPresenter::DefaultTools::TEXT,
    //     &_action_selectTextTool,
    //     &_optionsToolBar_text, 
    //     &_toolPresenter_text
    // );
    // _action_selectTextTool->setIcon(QIcon(":/icons/text_en.png")); //i18n
    // {
    // }

    selectToolBar->addAction(new QAction(QIcon(":/icons/text_en.png"), "", &_view));

    // addTool(
    //     IMainEditorPresenter::DefaultTools::SHAPES,
    //     &_action_selectShapesTool,
    //     &_optionsToolBar_shapes, 
    //     &_toolPresenter_shapes
    // );
    // _action_selectShapesTool->setIcon(QIcon(":/icons/shapes.png"));
    // {
    // }

    selectToolBar->addAction(new QAction(QIcon(":/icons/shapes.png"), "", &_view));

    // addTool(
    //     IMainEditorPresenter::DefaultTools::STICKERS,
    //     &_action_selectStickersTool,
    //     &_optionsToolBar_stickers, 
    //     &_toolPresenter_stickers
    // );
    // _action_selectStickersTool->setIcon(QIcon(":/icons/stickers.png"));
    // {
    // }

    selectToolBar->addAction(new QAction(QIcon(":/icons/stickers.png"), "", &_view));

    selectToolBar->addSeparator();


    // addTool(
    //     IMainEditorPresenter::DefaultTools::EYEDROP,
    //     &_action_selectEyedropTool,
    //     &_optionsToolBar_eyedrop,
    //     &_toolPresenter_eyedrop
    // );
    // _action_selectEyedropTool->setIcon(QIcon(":/icons/eyedrop.png"));
    // {
    // }

    selectToolBar->addAction(new QAction(QIcon(":/icons/eyedrop.png"), "", &_view));

    setupHelper.addTool(
        IMainEditorPresenter::DefaultTools::NAVIGATE,
        &action_selectNavigateTool,
        &optionsToolBar_navigate,
        &toolPresenter_navigate
    );

    // addTool(
    //     IMainEditorPresenter::DefaultTools::MEASURE,
    //     &_action_selectMeasureTool,
    //     &_optionsToolBar_measure,
    //     &_toolPresenter_measure
    // );
    // _action_selectMeasureTool->setIcon(QIcon(":/icons/measure.png"));
    // {
    // }

    selectToolBar->addAction(new QAction(QIcon(":/icons/measure.png"), "", &_view));
}