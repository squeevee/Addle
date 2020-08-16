#ifndef MAINEDITORPRESENTER_HPP
#define MAINEDITORPRESENTER_HPP

#include "compat.hpp"
#include <QFutureWatcher>
#include <QObject>
#include <QList>
#include <QSet>
#include <QHash>
#include <QUrl>

#include "helpers/undostackhelper.hpp"

//#include "interfaces/tasks/itask.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/asynctask.hpp"
#include "utilities/presenter/propertycache.hpp"

#include "utilities/initializehelper.hpp"

// #include "helpers/propertydecorationhelper.hpp"
namespace Addle {

class IMainEditorView;

class ISelectToolPresenter;
class IBrushToolPresenter;
class IEraserToolPresenter;
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
        WRITE selectTool
        NOTIFY currentToolChanged
    )
    Q_PROPERTY(
        bool empty
        READ isEmpty
        NOTIFY isEmptyChanged
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
        : //_propertyDecorationHelper(this),
        _undoStackHelper(*this)
    {
        _isEmptyCache.calculateBy(&MainEditorPresenter::isEmpty_p, this);
        _isEmptyCache.onChange.bind(&MainEditorPresenter::isEmptyChanged, this);
    }
    virtual ~MainEditorPresenter();

    // # IMainEditorPresenter

    void initialize(Mode mode);

    IMainEditorView* view() { _initHelper.check(Check_View); return _view; }

    ICanvasPresenter* canvasPresenter() { _initHelper.check(Check_CanvasPresenter); return _canvasPresenter; }
    IViewPortPresenter* viewPortPresenter() { _initHelper.check(Check_ViewPortPresenter); return _viewPortPresenter; }
    IColorSelectionPresenter& colorSelection() { _initHelper.check(Check_ColorSelection); return *_colorSelection; }

    void setMode(Mode mode);
    Mode mode() { return _mode; }

    // # IHaveDocumentPresenter

    IDocumentPresenter* documentPresenter() { _initHelper.check(); return _documentPresenter; }
    bool isEmpty() { _initHelper.check(); return _isEmptyCache.value(); }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const;

signals:
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>);
    void documentPresenterChanged(IDocumentPresenter* documentPresenter);
    void isEmptyChanged(bool);

public slots:
    void newDocument();
    void loadDocument(QUrl url);

    // # IHaveToolsPresenter
public:
    ToolId currentTool() { _initHelper.check(); return _currentTool; }
    void selectTool(ToolId tool);
    QList<ToolId> tools() { _initHelper.check(); return _tools.value(_mode); }

    IToolPresenter* toolPresenter(ToolId id) { _initHelper.check(); return _toolPresenters.value(id); }
    IToolPresenter* currentToolPresenter() { _initHelper.check(); return _currentToolPresenter; }

signals:
    void currentToolChanged(ToolId tool);

    // # IHaveUndoStackPresenter
public:
    bool canUndo() { _initHelper.check(); return _undoStackHelper.canUndo(); }
    bool canRedo() { _initHelper.check(); return _undoStackHelper.canRedo(); }

    void push(QSharedPointer<IUndoOperationPresenter> undoable) { _undoStackHelper.push(undoable); }

public slots: 
    void undo() { _initHelper.check(); _undoStackHelper.undo(); }
    void redo() { _initHelper.check(); _undoStackHelper.redo(); }

signals: 
    void undoStateChanged();

    // # IRaiseErrorPresenter
signals:
    void raiseError(QSharedPointer<IErrorPresenter> error);

    // # IDecoratedPresenter
public:
    // PropertyDecoration propertyDecoration(const char* propertyName) const
    // { 
    //     return _propertyDecorationHelper.propertyDecoration(propertyName);
    // }

private slots:
    void onLoadDocumentCompleted();

private:
    void setDocumentPresenter(IDocumentPresenter* document);
    bool isEmpty_p() { return !_documentPresenter; }
    QList<ILayerPresenter*> layers_p();

    Mode _mode = (Mode)NULL;

    IMainEditorView* _view = nullptr;
    IViewPortPresenter* _viewPortPresenter = nullptr;
    ICanvasPresenter* _canvasPresenter = nullptr;

    IColorSelectionPresenter* _colorSelection = nullptr;

    IDocumentPresenter* _documentPresenter = nullptr;
    PropertyCache<bool> _isEmptyCache;

    ISelectToolPresenter* _selectTool;
    IBrushToolPresenter* _brushTool;
    IEraserToolPresenter* _eraserTool;
    IFillToolPresenter* _fillTool;
    ITextToolPresenter* _textTool;
    IShapesToolPresenter* _shapesTool;
    IStickersToolPresenter* _stickersTool;
    IEyedropToolPresenter* _eyedropTool;
    INavigateToolPresenter* _navigateTool;
    IMeasureToolPresenter* _measureTool;

    QHash<Mode, QList<ToolId>> _tools;
    QHash<ToolId, IToolPresenter*> _toolPresenters;
    ToolId _currentTool;
    IToolPresenter* _currentToolPresenter = nullptr;

    LoadDocumentTask* _loadDocumentTask;

    // PropertyDecorationHelper _propertyDecorationHelper;
    UndoStackHelper _undoStackHelper;

    QList<QSharedPointer<IPalettePresenter>> _palettes;

    QMetaObject::Connection _connection_topSelectedLayer;

    InitializeHelper<MainEditorPresenter> _initHelper;
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

    QUrl url() { const auto lock = lockIO(); return _url; }
    void setUrl(QUrl url) { const auto lock = lockIO(); _url = url; }

    IDocumentPresenter* documentPresenter()
    { 
        const auto lock = lockIO();
        return _documentPresenter;
    }

protected:
    void doTask();

private:
    void setDocumentPresenter(IDocumentPresenter* documentPresenter)
    { 
        const auto lock = lockIO();
        _documentPresenter = documentPresenter;
    }

    QUrl _url;
    IDocumentPresenter* _documentPresenter;
};

} // namespace Addle
#endif // MAINEDITORPRESENTER_HPP