#ifndef EDITORPRESENTER_HPP
#define EDITORPRESENTER_HPP

#include "basedocumentpresenter.hpp"

#include "helpers/undostackhelper.hpp"

#include "interfaces/views/ieditorview.hpp"
#include "interfaces/presenters/ieditorpresenter.hpp"

#include "interfaces/presenters/tools/iselecttoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/tools/ierasertoolpresenter.hpp"
#include "interfaces/presenters/tools/ifilltoolpresenter.hpp"
#include "interfaces/presenters/tools/itexttoolpresenter.hpp"
#include "interfaces/presenters/tools/ishapestoolpresenter.hpp"
#include "interfaces/presenters/tools/istickerstoolpresenter.hpp"
#include "interfaces/presenters/tools/ieyedroptoolpresenter.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/tools/imeasuretoolpresenter.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"

class EditorPresenter : public BaseDocumentPresenter, public virtual IEditorPresenter
{
    Q_OBJECT
public:
    EditorPresenter();
    virtual ~EditorPresenter() = default;

    IDocumentView* getDocumentView() { return getEditorView(); }
    IEditorView* getEditorView();

    ILayerPresenter* getSelectedLayer() { return _selectedLayer; }
    void setSelectedLayer(int index) { _selectedLayer = _layerPresenters[index]; }

    bool canUndo() { return _undoStackHelper.canUndo(); }
    bool canRedo() { return _undoStackHelper.canRedo(); }

    void doOperation(QSharedPointer<IUndoableOperation> undoable) { _undoStackHelper.doOperation(undoable); }

public slots: 
    void onActionLoadDocument(QString filename);

    void undo() { _undoStackHelper.undo(); }
    void redo() { _undoStackHelper.redo(); }

signals: 
    void undoStateChanged();

protected:
    QList<IToolPresenter*> initializeTools();
    void setDocument(QSharedPointer<IDocument> document);

private:
    ISelectToolPresenter* _selectTool = nullptr;
    IBrushToolPresenter* _brushTool = nullptr;
    IEraserToolPresenter* _eraserTool = nullptr;
    IFillToolPresenter* _fillTool = nullptr;
    ITextToolPresenter* _textTool = nullptr;
    IShapesToolPresenter* _shapesTool = nullptr;
    IStickersToolPresenter* _stickersTool = nullptr;
    IEyedropToolPresenter* _eyedropTool = nullptr;
    INavigateToolPresenter* _navigateTool = nullptr;
    IMeasureToolPresenter* _measureTool = nullptr;

    IEditorView* _view = nullptr;

    ILayerPresenter* _selectedLayer = nullptr;

    UndoStackHelper _undoStackHelper;
};

#endif // EDITORPRESENTER_HPP