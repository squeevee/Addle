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

#include "utilities/propertycache.hpp"

#include "helpers/propertydecorationhelper.hpp"

class BaseDocumentPresenter : public QObject, public virtual IDocumentPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        ToolId currentTool 
        READ getCurrentTool 
        WRITE setCurrentTool
        NOTIFY currentToolChanged
    )
    Q_PROPERTY(
        bool empty
        READ isEmpty
        NOTIFY isEmptyChanged
    )
public:
    BaseDocumentPresenter()
        : _isEmptyCache(*this, &BaseDocumentPresenter::isEmpty_p, &BaseDocumentPresenter::isEmptyChanged),
        _propertyDecorationHelper(this)
    {
    }
    virtual ~BaseDocumentPresenter();

    ICanvasPresenter* getCanvasPresenter();

    IViewPortPresenter* getViewPortPresenter();

    //virtual IToolOptionsPresenter* getToolOptionsPresenter() { _toolOptionsPresenter; }

    ToolId getCurrentTool() { return _currentTool; }
    void setCurrentTool(ToolId tool);
    QList<ToolId> getTools() { return _tools; }

    IToolPresenter* getToolPresenter(ToolId id) { return _toolPresenters.value(id); }
    IToolPresenter* getCurrentToolPresenter() { return _currentToolPresenter; }


    bool isEmpty() { return _isEmptyCache.getValue(); }
    QSize getCanvasSize() { return _document ? _document->getSize() : QSize(); }
    QColor getBackgroundColor() { return _document ? _document->getBackgroundColor() : IDocument::DEFAULT_BACKGROUND_COLOR; }

    QList<ILayerPresenter*> getLayers() { return _layerPresenters; }

    PropertyDecoration getPropertyDecoration(const char* propertyName) const
    { 
        return _propertyDecorationHelper.getPropertyDecoration(propertyName);
    }

signals:
    void raiseError(QSharedPointer<IErrorPresenter> error);
    void documentChanged(QSharedPointer<IDocument> document);
    void currentToolChanged(ToolId tool);

    void isEmptyChanged(bool);

public slots:
    virtual void loadDocument(QUrl url);

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

    PropertyDecorationHelper _propertyDecorationHelper;

private:
    bool isEmpty_p();

    PropertyCache<BaseDocumentPresenter, bool> _isEmptyCache;

    IViewPortPresenter* _viewPortPresenter = nullptr;
    ICanvasPresenter* _canvasPresenter = nullptr;
};

#endif // BASEDOCUMENTPRESENTER_HPP