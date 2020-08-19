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

#include <memory>

#include "helpers/undostackhelper.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/asynctask.hpp"
#include "utilities/presenter/propertycache.hpp"

#include "utilities/initializehelper.hpp"

#include "interfaces/views/imaineditorview.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/icolorselectionpresenter.hpp"

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

class LoadDocumentTask;

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
        Addle::IRaiseErrorPresenter 
        Addle::IMainEditorPresenter
    )
    IAMQOBJECT_IMPL
    
    enum InitCheckpoints
    {
        Check_CanvasPresenter,
        Check_ViewPortPresenter,
        Check_ColorSelection,
        Check_View
    };

public:
    MainEditorPresenter()
    {
        _undoStackHelper.undoStateChanged.bind(&MainEditorPresenter::undoStateChanged, this);

        _isEmptyCache.calculateBy(&MainEditorPresenter::isEmpty_p, this);
        _isEmptyCache.onChange.bind(&MainEditorPresenter::isEmptyChanged, this);
    }
    virtual ~MainEditorPresenter() = default;

    void initialize(Mode mode);

    IMainEditorView& view() const { _initHelper.check(Check_View); return *_view; }

    ICanvasPresenter& canvasPresenter() const { _initHelper.check(Check_CanvasPresenter); return *_canvasPresenter; }
    IViewPortPresenter& viewPortPresenter() const { _initHelper.check(Check_ViewPortPresenter); return *_viewPortPresenter; }
    IColorSelectionPresenter& colorSelection() const { _initHelper.check(Check_ColorSelection); return *_colorSelection; }

    void setMode(Mode mode);
    Mode mode() const { return _mode; }

    // # IHaveDocumentPresenter

    QSharedPointer<IDocumentPresenter> documentPresenter() const { _initHelper.check(); return _documentPresenter; }
    bool isEmpty() const { _initHelper.check(); return _isEmptyCache.value(); }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const;

signals:
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>);
    void documentPresenterChanged(QSharedPointer<IDocumentPresenter> documentPresenter);
    void isEmptyChanged(bool);

public slots:
    void newDocument();
    void loadDocument(QUrl url);

public:
    ToolId currentTool() const { _initHelper.check(); return _currentTool; }
    void setCurrentTool(ToolId tool);
    QHash<ToolId, QSharedPointer<IToolPresenter>> tools() const { _initHelper.check(); return _tools.value(_mode); }

    QSharedPointer<IToolPresenter> currentToolPresenter() const { _initHelper.check(); return _currentToolPresenter; }

signals:
    void currentToolChanged(ToolId tool);

public:
    bool canUndo() const { _initHelper.check(); return _undoStackHelper.canUndo(); }
    bool canRedo() const { _initHelper.check(); return _undoStackHelper.canRedo(); }

    void push(QSharedPointer<IUndoOperationPresenter> undoable) { _undoStackHelper.push(undoable); }

public slots: 
    void undo() { try { _initHelper.check(); _undoStackHelper.undo(); } ADDLE_SLOT_CATCH }
    void redo() { try { _initHelper.check(); _undoStackHelper.redo(); } ADDLE_SLOT_CATCH }

signals: 
    void undoStateChanged();

signals:
    void raiseError(QSharedPointer<IErrorPresenter> error);

private slots:
    void onLoadDocumentCompleted();

private:
    void setDocumentPresenter(QSharedPointer<IDocumentPresenter> document);
    bool isEmpty_p() const { return !_documentPresenter; }

    Mode _mode = (Mode)NULL;

    std::unique_ptr<IMainEditorView> _view = nullptr;
    std::unique_ptr<IViewPortPresenter> _viewPortPresenter = nullptr;
    std::unique_ptr<ICanvasPresenter> _canvasPresenter = nullptr;

    std::unique_ptr<IColorSelectionPresenter> _colorSelection = nullptr;

    QSharedPointer<IDocumentPresenter> _documentPresenter;
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

    LoadDocumentTask* _loadDocumentTask;

    UndoStackHelper _undoStackHelper;

    QList<QSharedPointer<IPalettePresenter>> _palettes;

    QMetaObject::Connection _connection_topSelectedLayer;

    InitializeHelper _initHelper;
};

class LoadDocumentTask : public AsyncTask
{
    Q_OBJECT 
public:
    LoadDocumentTask(QObject* parent = nullptr)
        : AsyncTask(parent)
    {
    }
    virtual ~LoadDocumentTask() = default;

    QUrl url() const { const auto lock = lockIO(); return _url; }
    void setUrl(QUrl url) { const auto lock = lockIO(); _url = url; }

    QSharedPointer<IDocumentPresenter> documentPresenter() const 
    { 
        const auto lock = lockIO();
        return _documentPresenter;
    }

protected:
    void doTask();

private:
    void setDocumentPresenter(QSharedPointer<IDocumentPresenter> documentPresenter)
    { 
        const auto lock = lockIO();
        _documentPresenter = documentPresenter;
    }

    QUrl _url;
    QSharedPointer<IDocumentPresenter> _documentPresenter;
};

} // namespace Addle
#endif // MAINEDITORPRESENTER_HPP