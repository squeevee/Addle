#include "editorview.hpp"
#include <QLabel>

#include "interfaces/presenters/tools/itoolpresenter.hpp"

void EditorView::initialize(IEditorPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;
    BaseDocumentView::initialize(presenter);

    _initHelper.initializeEnd();
}

void EditorView::setupUi()
{
    BaseDocumentView::setupUi();

    _toolBar_documentActions->addAction(_action_open);


    // addTool(
    //     IEditorPresenter::DefaultTools::SELECT,
    //     &_action_selectSelectTool,
    //     &_optionsToolBar_select,
    //     &_toolPresenter_select
    // );
    // _action_selectSelectTool->setIcon(QIcon(":/icons/select.png"));
    // {
    // }

    // _toolBar_toolSelection->addSeparator();

    addTool(
        IEditorPresenter::DefaultTools::BRUSH,
        &_action_selectBrushTool,
        &_optionsToolBar_brush,
        &_toolPresenter_brush
    );

    // addTool(
    //     IEditorPresenter::DefaultTools::ERASER,
    //     &_action_selectEraserTool,
    //     &_optionsToolBar_eraser,
    //     &_toolPresenter_eraser
    // );
    // _action_selectEraserTool->setIcon(QIcon(":/icons/eraser.png"));
    // {
    // }


    // addTool(
    //     IEditorPresenter::DefaultTools::FILL,
    //     &_action_selectFillTool,
    //     &_optionsToolBar_fill,
    //     &_toolPresenter_fill
    // );
    // _action_selectFillTool->setIcon(QIcon(":/icons/fill.png"));
    // {
    // }
    

    _toolBar_toolSelection->addSeparator();


    // addTool(
    //     IEditorPresenter::DefaultTools::TEXT,
    //     &_action_selectTextTool,
    //     &_optionsToolBar_text, 
    //     &_toolPresenter_text
    // );
    // _action_selectTextTool->setIcon(QIcon(":/icons/text_en.png")); //i18n
    // {
    // }


    // addTool(
    //     IEditorPresenter::DefaultTools::SHAPES,
    //     &_action_selectShapesTool,
    //     &_optionsToolBar_shapes, 
    //     &_toolPresenter_shapes
    // );
    // _action_selectShapesTool->setIcon(QIcon(":/icons/shapes.png"));
    // {
    // }


    // addTool(
    //     IEditorPresenter::DefaultTools::STICKERS,
    //     &_action_selectStickersTool,
    //     &_optionsToolBar_stickers, 
    //     &_toolPresenter_stickers
    // );
    // _action_selectStickersTool->setIcon(QIcon(":/icons/stickers.png"));
    // {
    // }


    // _toolBar_toolSelection->addSeparator();


    // addTool(
    //     IEditorPresenter::DefaultTools::EYEDROP,
    //     &_action_selectEyedropTool,
    //     &_optionsToolBar_eyedrop,
    //     &_toolPresenter_eyedrop
    // );
    // _action_selectEyedropTool->setIcon(QIcon(":/icons/eyedrop.png"));
    // {
    // }


    addTool(
        IEditorPresenter::DefaultTools::NAVIGATE,
        &_action_selectNavigateTool,
        &_optionsToolBar_navigate,
        &_toolPresenter_navigate
    );

    // addTool(
    //     IEditorPresenter::DefaultTools::MEASURE,
    //     &_action_selectMeasureTool,
    //     &_optionsToolBar_measure,
    //     &_toolPresenter_measure
    // );
    // _action_selectMeasureTool->setIcon(QIcon(":/icons/measure.png"));
    // {
    // }

}