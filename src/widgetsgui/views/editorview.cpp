#include "editorview.hpp"
#include <QLabel>

#include "utilities/qtextensions/qobject.hpp"
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "widgetsgui/utilities/decorationhelper.hpp"
#include "helpers/setuptoolshelper.hpp"

void EditorView::initialize(IEditorPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;
    BaseDocumentView::initialize(presenter);

    connect_interface(_presenter, SIGNAL(undoStateChanged()), this, SLOT(onUndoStateChanged()));

    _initHelper.initializeEnd();
}

void EditorView::setupUi()
{
    BaseDocumentView::setupUi();

    _action_undo = new QAction(this);
    _action_undo->setIcon(QIcon(":/icons/undo.png"));
    _action_undo->setEnabled(_presenter->canUndo());
    connect_interface(_action_undo, SIGNAL(triggered()), _presenter, SLOT(undo()));

    _action_redo = new QAction(this);
    _action_redo->setIcon(QIcon(":/icons/redo.png"));
    _action_redo->setEnabled(_presenter->canRedo());
    connect_interface(_action_redo, SIGNAL(triggered()), _presenter, SLOT(redo()));
    
    _toolBar_documentActions->addAction(_action_open);
    _toolBar_documentActions->addSeparator();
    _toolBar_documentActions->addAction(_action_undo);
    _toolBar_documentActions->addAction(_action_redo);

    SetupToolsHelper setupToolsHelper(*this, *_presenter);

    // addTool(
    //     IEditorPresenter::DefaultTools::SELECT,
    //     &_action_selectSelectTool,
    //     &_optionsToolBar_select,
    //     &_toolPresenter_select
    // );
    // _action_selectSelectTool->setIcon(QIcon(":/icons/select.png"));
    // {
    // }

    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/select.png"), "", this));

    _toolBar_toolSelection->addSeparator();

    setupToolsHelper.addTool(
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

    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/eraser.png"), "", this));

    // addTool(
    //     IEditorPresenter::DefaultTools::FILL,
    //     &_action_selectFillTool,
    //     &_optionsToolBar_fill,
    //     &_toolPresenter_fill
    // );
    // _action_selectFillTool->setIcon(QIcon(":/icons/fill.png"));
    // {
    // }
    
    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/fill.png"), "", this));

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

    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/text_en.png"), "", this));

    // addTool(
    //     IEditorPresenter::DefaultTools::SHAPES,
    //     &_action_selectShapesTool,
    //     &_optionsToolBar_shapes, 
    //     &_toolPresenter_shapes
    // );
    // _action_selectShapesTool->setIcon(QIcon(":/icons/shapes.png"));
    // {
    // }

    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/shapes.png"), "", this));

    // addTool(
    //     IEditorPresenter::DefaultTools::STICKERS,
    //     &_action_selectStickersTool,
    //     &_optionsToolBar_stickers, 
    //     &_toolPresenter_stickers
    // );
    // _action_selectStickersTool->setIcon(QIcon(":/icons/stickers.png"));
    // {
    // }

    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/stickers.png"), "", this));

    _toolBar_toolSelection->addSeparator();


    // addTool(
    //     IEditorPresenter::DefaultTools::EYEDROP,
    //     &_action_selectEyedropTool,
    //     &_optionsToolBar_eyedrop,
    //     &_toolPresenter_eyedrop
    // );
    // _action_selectEyedropTool->setIcon(QIcon(":/icons/eyedrop.png"));
    // {
    // }

    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/eyedrop.png"), "", this));

    setupToolsHelper.addTool(
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

    _toolBar_toolSelection->addAction(new QAction(QIcon(":/icons/measure.png"), "", this));
}

void EditorView::onUndoStateChanged()
{
    _initHelper.assertInitialized();

    _action_undo->setEnabled(_presenter->canUndo());
    _action_redo->setEnabled(_presenter->canRedo());
}