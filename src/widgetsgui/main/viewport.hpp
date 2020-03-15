#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "utilities/initializehelper.hpp"

class CanvasScene;
class IViewPortPresenter;
class IDocumentPresenter;
class ViewPort : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(
        bool _mainPresenterIsEmpty
        MEMBER _cache_mainPresenterIsEmpty
    )
public:
    ViewPort(IViewPortPresenter* presenter);
    virtual ~ViewPort() = default;

protected:
    void resizeEvent(QResizeEvent* event);
    void moveEvent(QMoveEvent *event);

private slots:
    void onMainEditorPresenter_documentChanged(IDocumentPresenter* documentPresenter);
    void onTransformsChanged();

private:
    //keep local cached copies of some presenter properties for fast drawing
    bool _cache_mainPresenterIsEmpty = true;
    QTransform _cache_ontoCanvasTransform;
    QTransform _cache_fromCanvasTransform;
    QRect _cache_viewPortRect;

    CanvasScene* _canvasScene;

    IViewPortPresenter* _presenter;
    IMainEditorPresenter* _mainEditorPresenter;
};



#endif // VIEWPORT_HPP