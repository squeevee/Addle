#ifndef BASEDOCUMENTPRESENTER_HPP
#define BASEDOCUMENTPRESENTER_HPP

#include <QObject>
#include <QSet>

#include "common/interfaces/tasks/itask.hpp"
#include "common/interfaces/presenters/iviewportpresenter.hpp"
#include "common/interfaces/presenters/idocumentpresenter.hpp"
//#include "common/interfaces/presenters/itooloptionspresenter.hpp"

class BaseDocumentPresenter : public QObject, public virtual IDocumentPresenter
{
    Q_OBJECT

public:
    virtual ~BaseDocumentPresenter();

    IViewPortPresenter* getViewPortPresenter();
    ICanvasPresenter* getCanvasPresenter();

    //virtual IToolOptionsPresenter* getToolOptionsPresenter() { _toolOptionsPresenter; }

    void setCurrentTool(IToolPresenter* tool);
    IToolPresenter* getCurrentTool() { return _currentTool; }
    QList<IToolPresenter*> getTools() { return _tools; }

signals:
    void raiseError(QSharedPointer<IErrorPresenter> error);
    void documentChanged(QSharedPointer<IDocument> document);
    void currentToolChanged(IToolPresenter* tool);

public slots:
    void loadDocument(QFileInfo fileInfo);
    void loadDocument(QUrl url);

private slots:

    void onLoadDocumentTaskDone(ITask* task);

protected:

    void setTools(const QList<IToolPresenter*>& tools);


    IToolPresenter* _currentTool = nullptr;

    void setDocument(QSharedPointer<IDocument> document);

    QSharedPointer<IDocument> _document;

    //IToolOptionsPresenter* _toolOptionsPresenter;

private:
    QList<IToolPresenter*> _tools;
    QSet<IToolPresenter*> _toolsSet;

    IViewPortPresenter* _viewPortPresenter = nullptr;
    ICanvasPresenter* _canvasPresenter = nullptr;
};

#endif // BASEDOCUMENTPRESENTER_HPP