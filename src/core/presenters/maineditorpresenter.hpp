/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef MAINEDITORPRESENTER_HPP
#define MAINEDITORPRESENTER_HPP

#include <boost/di.hpp>

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

#include "interfaces/services/ifactory.hpp"
#include "interfaces/services/irepository.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/asynctask.hpp"
#include "utilities/lazyvalue.hpp"


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

class ADDLE_CORE_EXPORT MainEditorPresenter : public QObject, public IMainEditorPresenter
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

public:
        
    MainEditorPresenter(
            Mode mode,
            std::unique_ptr<IColorSelectionPresenter> colorSelection,
            QSharedPointer<ICanvasPresenter> canvas,
            std::unique_ptr<IMessageContext> messageContext,
            std::unique_ptr<IRepository<IPalettePresenter>> palettes,
            std::unique_ptr<IRepository<IToolPresenter>> tools,
            const IFactory<IViewPortPresenter>& viewPortFactory,
            const IFactory<IDocumentPresenter>& documentFactory
        );
    virtual ~MainEditorPresenter() = default;

    IViewPortPresenter& viewPortPresenter() const override {  return *_viewPortPresenter; }
    IColorSelectionPresenter& colorSelection() const override { return *_colorSelection; }
    IMessageContext& messageContext() const override { return *_messageContext; }
    
    void setMode(Mode mode) override;
    Mode mode() const override { return _mode; }

    QSharedPointer<ICanvasPresenter> canvas() const override { return _canvas; }
    
    QSharedPointer<IDocumentPresenter> documentPresenter() const override {  return _document; }
    bool isEmpty() const override { return _isEmptyCache.value(); } // TODO: rename "hasDocument"?

    QSharedPointer<ILayerPresenter> topSelectedLayer() const override;
    
    QSharedPointer<FileRequest> pendingDocumentFileRequest() const override
    {
        return _pendingDocumentFileRequest;
    }

signals:
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>) override;
    void documentPresenterChanged(QSharedPointer<Addle::IDocumentPresenter> documentPresenter) override;
    void isEmptyChanged(bool) override;

public slots:
    void newDocument() override;
    void loadDocument(QSharedPointer<FileRequest> request) override;

public:
    ToolId currentTool() const override { return _currentTool; }
    void setCurrentTool(ToolId tool) override;
    const IRepository<IToolPresenter>& tools() const override { return *_tools; }

    QSharedPointer<IToolPresenter> currentToolPresenter() const override { return _currentToolPresenter; }

signals:
    void currentToolChanged(ToolId tool) override;

public:
    bool canUndo() const override { return _undoStackHelper.canUndo(); }
    bool canRedo() const override { return _undoStackHelper.canRedo(); }

    //rename?
    void push(QSharedPointer<IUndoOperationPresenter> undoable) override { _undoStackHelper.push(undoable); }

public slots: 
    void undo() override { try { _undoStackHelper.undo(); } ADDLE_SLOT_CATCH }
    void redo() override { try { _undoStackHelper.redo(); } ADDLE_SLOT_CATCH }

signals: 
    void undoStateChanged() override;

private:
    void onLoadDocumentCompleted();
    void onLoadDocumentFailed();
    
    void setDocument(QSharedPointer<IDocumentPresenter> document);
    bool isEmpty_p() const { return !_document; }

    Mode _mode;

    std::unique_ptr<IColorSelectionPresenter> _colorSelection;
    QSharedPointer<ICanvasPresenter> _canvas;
    std::unique_ptr<IViewPortPresenter> _viewPortPresenter;
    std::unique_ptr<IMessageContext> _messageContext;
    
    std::unique_ptr<IRepository<IPalettePresenter>> _palettes;
    
    std::unique_ptr<IMainEditorView> _view;

    QSharedPointer<IDocumentPresenter> _document;
    LazyProperty<bool> _isEmptyCache;
    
    std::unique_ptr<IRepository<IToolPresenter>> _tools;
    
    ToolId _currentTool;
    QSharedPointer<IToolPresenter> _currentToolPresenter;

    UndoStackHelper _undoStackHelper;

    QMetaObject::Connection _connection_topSelectedLayer;

    QMetaObject::Connection _connection_onSaveDocumentUrlUpdated;
    QMetaObject::Connection _connection_onSaveDocumentComplete;

    QSharedPointer<FileRequest> _pendingDocumentFileRequest;
    
    const IFactory<IDocumentPresenter>& _documentFactory;
    
    LoadHelper<IDocument, IDocumentPresenter> _loadDocumentHelper;
};

} // namespace Addle

#endif // MAINEDITORPRESENTER_HPP
