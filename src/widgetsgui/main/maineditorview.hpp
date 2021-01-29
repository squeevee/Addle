/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BASEDOCUMENTVIEW_HPP
#define BASEDOCUMENTVIEW_HPP

#include "compat.hpp"
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QScrollBar>
#include <QAction>
#include <QActionGroup>

#include <memory>

#include "helpers/toplevelviewhelper.hpp"

#include "utilities/optiongroup.hpp"

#include "interfaces/services/iapplicationservice.hpp"

#include "interfaces/views/imaineditorview.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utils.hpp"
#include "utilities/initializehelper.hpp"
#include "utilities/qobject.hpp"
#include "utilities/view/messageviewhelper.hpp"

namespace Addle {

class BrushToolOptionsBar;
class NavigateToolOptionsBar;

class ViewPort;
class ViewPortScrollWidget;
class ZoomRotateWidget;
class ColorSelector;
class LayersManager;

class FileDialogHelper;

class IMessagePresenter;

class ADDLE_WIDGETSGUI_EXPORT MainEditorView : public QMainWindow, public IMainEditorView
{
    Q_OBJECT
    Q_INTERFACES(Addle::IMainEditorView Addle::ITopLevelView)
    IAMQOBJECT_IMPL    
public:
//     MainEditorView(/*IApplicationService&, IMainEditorPresenter& presenter*/);
    virtual ~MainEditorView() = default;

    //void initialize(IMainEditorPresenter& presenter) override;
    IMainEditorPresenter& presenter() const override{ ASSERT_INIT(); /*return _presenter;*/ assert(false); }
    
public slots:
    void tlv_open() override { try { ASSERT_INIT(); /*_tlvHelper.open();*/ } ADDLE_SLOT_CATCH }
    void tlv_close() override { try { ASSERT_INIT(); /*_tlvHelper.close();*/ } ADDLE_SLOT_CATCH } 

signals:
    void tlv_opened() override;
    void tlv_closed() override;
    
private slots:
    void onAction_open();
    void onAction_save();

    void onMessagePosted(QSharedPointer<Addle::IMessagePresenter> message)
    {
        try {
            ASSERT_INIT();
            _messageViewHelper.onMessagePosted(message);
        } ADDLE_SLOT_CATCH
    }

    void onUndoStateChanged();

    void onToolBarNeedsShown();
    void onToolBarNeedsHidden();

    void onPresenterEmptyChanged(bool);

    void onDocumentChanged(QSharedPointer<Addle::IDocumentPresenter> document);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupUi();
    void onUrgentMessageMade(ITopLevelView* view);
    
//     IMainEditorPresenter& _presenter;
    
    QMenuBar* _menuBar;
    QToolBar* _toolBar_documentActions;
    QToolBar* _toolBar_editorToolSelection;
    //QToolBar* _toolBar_viewerToolSelection;
    QStatusBar* _statusBar;

    ViewPort* _viewPort;
    ViewPortScrollWidget* _viewPortScrollWidget;
    ZoomRotateWidget* _zoomRotateWidget;

    QAction* _action_new;
    QAction* _action_open;
    QAction* _action_save;
    QAction* _action_undo;
    QAction* _action_redo;
    
    QAction* _action_close;

    OptionGroup* _optionGroup_toolSelection;

    //SelectToolAction* action_selectSelectTool;
    QToolBar* _optionsToolBar_select;

    OptionAction* _action_selectBrushTool;
    BrushToolOptionsBar* _optionsToolBar_brush;

    OptionAction* _action_selectEraserTool;
    BrushToolOptionsBar* _optionsToolBar_eraser;

    //SelectToolAction* action_selectFillTool;
    QToolBar* _optionsToolBar_fill;

    //SelectToolAction* action_selectTextTool;
    QToolBar* _optionsToolBar_text;

    //SelectToolAction* action_selectShapesTool;
    QToolBar* _optionsToolBar_shapes;

    //SelectToolAction* action_selectStickersTool;
    QToolBar* _optionsToolBar_stickers;

    //SelectToolAction* action_selectEyedropTool;
    QToolBar* _optionsToolBar_eyedrop;

    OptionAction* _action_selectNavigateTool;
    NavigateToolOptionsBar* _optionsToolBar_navigate;

    //SelectToolAction* action_selectMeasureTool;
    QToolBar* _optionsToolBar_measure;

    QToolBar* _optionsToolBar_currentTool = nullptr;

    LayersManager* _layersManager;
    ColorSelector* _colorSelector;

    FileDialogHelper* _fileDialogHelper;
    
//     TopLevelViewHelper _tlvHelper;
    
    MessageViewHelper _messageViewHelper;
    
    bool _uiIsSetup = false;

    InitializeHelper _initHelper;
    
    friend class ToolSetupHelper;
};

} // namespace Addle

#endif // BASEDOCUMENTVIEW_HPP
