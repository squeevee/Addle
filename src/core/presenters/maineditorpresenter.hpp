/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef MAINEDITORPRESENTER_HPP
#define MAINEDITORPRESENTER_HPP

#include "compat.hpp"
#include <QObject>
#include <QList>
#include <QSet>
#include <QHash>
#include <QUrl>

#include <QWeakPointer>

#include <memory>

#include "helpers/undostackhelper.hpp"
#include "helpers/loadhelper.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/asynctask.hpp"
#include "utilities/presenter/propertycache.hpp"

#include "utilities/initializehelper.hpp"

#include "interfaces/views/imaineditorview.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/icolorselectionpresenter.hpp"
#include "interfaces/presenters/imessagecontext.hpp"

#include "utilities/presenter/filerequest.hpp"

#include "utils.hpp"

// #include "helpers/propertydecorationhelper.hpp"
namespace Addle {

class ISelectToolPresenter;
class IBrushToolPresenter;
class IFillToolPresenter;
class ITextToolPresenter;
class IShapesToolPresenter;
class IStickersToolPresenter;
class IEyedropToolPresenter;
class INavigateToolPresenter;
class IMeasureToolPresenter;

class ADDLE_CORE_EXPORT MainEditorPresenter : public QObject, public virtual IMainEditorPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        Addle::ToolId currentTool 
        READ currentTool 
        WRITE setCurrentTool
        NOTIFY currentToolChanged
    )
    Q_PROPERTY(
        bool empty
        READ isEmpty
        NOTIFY isEmptyChanged
    )
    Q_INTERFACES(
        Addle::IHaveUndoStackPresenter 
        Addle::IMainEditorPresenter
    )
    IAMQOBJECT_IMPL
    
    enum InitCheckpoints
    {
        InitCheck_CanvasPresenter,
        InitCheck_ViewPortPresenter,
        InitCheck_ColorSelection,
        InitCheck_MessageContext,
        InitCheck_View
    };

public:
    MainEditorPresenter()
    {
        _undoStackHelper.undoStateChanged.bind(&MainEditorPresenter::undoStateChanged, this);

        _isEmptyCache.calculateBy(&MainEditorPresenter::isEmpty_p, this);
        _isEmptyCache.onChange.bind(&MainEditorPresenter::isEmptyChanged, this);
        
        _loadDocumentHelper.onLoaded.bind(&MainEditorPresenter::onLoadDocumentCompleted, this);
    }
    virtual ~MainEditorPresenter() = default;

    void initialize(Mode mode);

    IMainEditorView& view() const { ASSERT_INIT_CHECKPOINT(InitCheck_View); return *_view; }

    ICanvasPresenter& canvasPresenter() const { ASSERT_INIT_CHECKPOINT(InitCheck_CanvasPresenter); return *_canvasPresenter; }
    IViewPortPresenter& viewPortPresenter() const { ASSERT_INIT_CHECKPOINT(InitCheck_ViewPortPresenter); return *_viewPortPresenter; }
    IColorSelectionPresenter& colorSelection() const { ASSERT_INIT_CHECKPOINT(InitCheck_ColorSelection); return *_colorSelection; }
    IMessageContext& messageContext() const { ASSERT_INIT_CHECKPOINT(InitCheck_MessageContext); return *_messageContext; }
    
    void setMode(Mode mode);
    Mode mode() const { return _mode; }

    // # IHaveDocumentPresenter

    QSharedPointer<IDocumentPresenter> documentPresenter() const { ASSERT_INIT(); return _document; }
    bool isEmpty() const { ASSERT_INIT(); return _isEmptyCache.value(); }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const;
    
    QSharedPointer<FileRequest> pendingDocumentFileRequest() const
    {
        ASSERT_INIT();
        return _pendingDocumentFileRequest;
    }

signals:
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>);
    void documentPresenterChanged(QSharedPointer<IDocumentPresenter> documentPresenter);
    void isEmptyChanged(bool);

public slots:
    void newDocument();
    void loadDocument(QSharedPointer<FileRequest> request);

public:
    ToolId currentTool() const { ASSERT_INIT(); return _currentTool; }
    void setCurrentTool(ToolId tool);
    QHash<ToolId, QSharedPointer<IToolPresenter>> tools() const { ASSERT_INIT(); return _tools.value(_mode); }

    QSharedPointer<IToolPresenter> currentToolPresenter() const { ASSERT_INIT(); return _currentToolPresenter; }

signals:
    void currentToolChanged(ToolId tool);

public:
    bool canUndo() const { ASSERT_INIT(); return _undoStackHelper.canUndo(); }
    bool canRedo() const { ASSERT_INIT(); return _undoStackHelper.canRedo(); }

    //rename?
    void push(QSharedPointer<IUndoOperationPresenter> undoable) { _undoStackHelper.push(undoable); }

public slots: 
    void undo() { try { ASSERT_INIT(); _undoStackHelper.undo(); } ADDLE_SLOT_CATCH }
    void redo() { try { ASSERT_INIT(); _undoStackHelper.redo(); } ADDLE_SLOT_CATCH }

signals: 
    void undoStateChanged();

signals:
    void messagePosted(QSharedPointer<IMessagePresenter> message);

private:
    void onLoadDocumentCompleted();
    void onLoadDocumentFailed();
    
    void setDocument(QSharedPointer<IDocumentPresenter> document);
    bool isEmpty_p() const { return !_document; }

    Mode _mode = (Mode)NULL;

    std::unique_ptr<IMainEditorView> _view;
    std::unique_ptr<IViewPortPresenter> _viewPortPresenter;
    std::unique_ptr<ICanvasPresenter> _canvasPresenter;
    std::unique_ptr<IMessageContext> _messageContext;

    std::unique_ptr<IColorSelectionPresenter> _colorSelection;

    QSharedPointer<IDocumentPresenter> _document;
    PropertyCache<bool> _isEmptyCache;

    QSharedPointer<ISelectToolPresenter> _selectTool;
    QSharedPointer<IBrushToolPresenter> _brushTool;
    QSharedPointer<IBrushToolPresenter> _eraserTool;
    QSharedPointer<IFillToolPresenter> _fillTool;
    QSharedPointer<ITextToolPresenter> _textTool;
    QSharedPointer<IShapesToolPresenter> _shapesTool;
    QSharedPointer<IStickersToolPresenter> _stickersTool;
    QSharedPointer<IEyedropToolPresenter> _eyedropTool;
    QSharedPointer<INavigateToolPresenter> _navigateTool;
    QSharedPointer<IMeasureToolPresenter> _measureTool;

    QHash<Mode, QHash<ToolId, QSharedPointer<IToolPresenter>>> _tools;
    ToolId _currentTool;
    QSharedPointer<IToolPresenter> _currentToolPresenter;

    UndoStackHelper _undoStackHelper;

    QList<QSharedPointer<IPalettePresenter>> _palettes;

    QMetaObject::Connection _connection_topSelectedLayer;

    QMetaObject::Connection _connection_onSaveDocumentUrlUpdated;
    QMetaObject::Connection _connection_onSaveDocumentComplete;

    QSharedPointer<FileRequest> _pendingDocumentFileRequest;
    
    LoadHelper<IDocument, IDocumentPresenter> _loadDocumentHelper;

    InitializeHelper _initHelper;
};

} // namespace Addle

#endif // MAINEDITORPRESENTER_HPP
