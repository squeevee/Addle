#ifndef BASEDOCUMENTPRESENTER_HPP
#define BASEDOCUMENTPRESENTER_HPP

#include <QObject>
#include <QList>
#include <QSet>
#include <QHash>

#include "interfaces/tasks/itask.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"
//#include "interfaces/presenters/itoolpresenter.hpp"

class BaseDocumentPresenter : public QObject, public virtual IDocumentPresenter
{
    Q_OBJECT

public:
    virtual ~BaseDocumentPresenter();

    ICanvasView* getCanvasView();

    IViewPortPresenter* getViewPortPresenter();

    //virtual IToolOptionsPresenter* getToolOptionsPresenter() { _toolOptionsPresenter; }

    ToolId getCurrentTool() { return _currentTool; }
    void setCurrentTool(ToolId tool);
    QList<ToolId> getTools() { return _tools; }

    IToolPresenter* getToolPresenter(ToolId id) { return _toolPresenters.value(id); }
    IToolPresenter* getCurrentToolPresenter() { return _currentToolPresenter; }


    bool isEmpty() { return !_document; }
    QSize getCanvasSize() { return _document->getSize(); }
    QColor getBackgroundColor() { return Qt::GlobalColor::white; } // todo

    QList<ILayerPresenter*> getLayerPresenters() { return _layerPresenters; }

signals:
    void raiseError(QSharedPointer<IErrorPresenter> error);
    void documentChanged(QSharedPointer<IDocument> document);
    void currentToolChanged(ToolId tool);

public slots:
    void loadDocument(QFileInfo fileInfo);
    void loadDocument(QUrl url);

private slots:

    void onLoadDocumentTaskDone(ITask* task);

protected:
    QList<ToolId> _tools;
    QHash<ToolId, IToolPresenter*> _toolPresenters;
    ToolId _currentTool;
    IToolPresenter* _currentToolPresenter = nullptr;

    virtual void setDocument(QSharedPointer<IDocument> document);

    QSharedPointer<IDocument> _document;

    QList<ILayerPresenter*> _layerPresenters;

    //IToolOptionsPresenter* _toolOptionsPresenter;

private:
    IViewPortPresenter* _viewPortPresenter = nullptr;
    ICanvasView* _canvasView = nullptr;
};

#endif // BASEDOCUMENTPRESENTER_HPP