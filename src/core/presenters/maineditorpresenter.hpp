#ifndef MAINEDITORPRESENTER_HPP
#define MAINEDITORPRESENTER_HPP

#include <QFutureWatcher>
#include <QObject>
#include <QList>
#include <QSet>
#include <QHash>
#include <QUrl>

#include "core/compat.hpp"
#include "helpers/undostackhelper.hpp"

//#include "interfaces/tasks/itask.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/asynctask.hpp"
#include "utilities/presenter/propertycache.hpp"

#include "helpers/propertydecorationhelper.hpp"

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
        ToolId currentTool 
        READ getCurrentTool 
        WRITE selectTool
        NOTIFY currentToolChanged
    )
    Q_PROPERTY(
        bool empty
        READ isEmpty
        NOTIFY isEmptyChanged
    )
public:
    MainEditorPresenter()
        : _propertyDecorationHelper(this),
        _undoStackHelper(*this)
    {
        _isEmptyCache.calculateBy(&MainEditorPresenter::isEmpty_p, this);
        _isEmptyCache.onChange(&MainEditorPresenter::isEmptyChanged, this);
    }
    virtual ~MainEditorPresenter();

    // # IMainEditorPresenter

    void initialize(Mode mode);

    IMainEditorView* getView();

    ICanvasPresenter* getCanvasPresenter();
    IViewPortPresenter* getViewPortPresenter();

    QWeakPointer<ILayerPresenter> getSelectedLayer() { return _selectedLayer; }
    void selectLayer(QWeakPointer<ILayerPresenter> layer);
    void selectLayerAt(int index);

    void setMode(Mode mode);
    Mode getMode() { return _mode; }

    // # IHaveDocumentPresenter

    IDocumentPresenter* getDocumentPresenter() { return _documentPresenter; }
    bool isEmpty() { return _isEmptyCache.getValue(); }

signals:
    void documentPresenterChanged(IDocumentPresenter* documentPresenter);
    void selectedLayerChanged(QWeakPointer<ILayerPresenter> layer);
    void isEmptyChanged(bool);

public slots:
    void newDocument();
    void loadDocument(QUrl url);

    // # IHaveToolsPresenter
public:
    ToolId getCurrentTool() { return _currentTool; }
    void selectTool(ToolId tool);
    QList<ToolId> getTools() { return _tools.value(_mode); }

    IToolPresenter* getToolPresenter(ToolId id) { return _toolPresenters.value(id); }
    IToolPresenter* getCurrentToolPresenter() { return _currentToolPresenter; }

signals:
    void currentToolChanged(ToolId tool);

    // # IHaveUndoStackPresenter
public:
    bool canUndo() { return _undoStackHelper.canUndo(); }
    bool canRedo() { return _undoStackHelper.canRedo(); }

    void push(QSharedPointer<IUndoOperationPresenter> undoable) { _undoStackHelper.push(undoable); }

public slots: 
    void undo() { _undoStackHelper.undo(); }
    void redo() { _undoStackHelper.redo(); }

signals: 
    void undoStateChanged();

    // # IRaiseErrorPresenter
signals:
    void raiseError(QSharedPointer<IErrorPresenter> error);

    // # IPropertyDecoratedPresenter
public:
    PropertyDecoration getPropertyDecoration(const char* propertyName) const
    { 
        return _propertyDecorationHelper.getPropertyDecoration(propertyName);
    }

private slots:
    void onLoadDocumentCompleted();

private:
    void setDocumentPresenter(IDocumentPresenter* document);
    bool isEmpty_p() { return !_documentPresenter; }
    QList<ILayerPresenter*> getLayers_p();

    Mode _mode = (Mode)NULL;

    IMainEditorView* _view = nullptr;
    IViewPortPresenter* _viewPortPresenter = nullptr;
    ICanvasPresenter* _canvasPresenter = nullptr;

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

    QWeakPointer<ILayerPresenter> _selectedLayer;

    LoadDocumentTask* _loadDocumentTask;

    PropertyDecorationHelper _propertyDecorationHelper;
    UndoStackHelper _undoStackHelper;
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

    QUrl getUrl() { const auto lock = lockIO(); return _url; }
    void setUrl(QUrl url) { const auto lock = lockIO(); _url = url; }

    IDocumentPresenter* getDocumentPresenter()
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

#endif // MAINEDITORPRESENTER_HPP