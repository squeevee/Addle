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

    void drawBackground(QPainter* painter, const QRectF& rect);

private slots:
    void onTransformsChanged();

private:
    //keep local cached copies of some presenter properties for fast drawing
    bool _cache_mainPresenterIsEmpty = true;
    QTransform _cache_ontoCanvasTransform;
    QTransform _cache_fromCanvasTransform;
    QRect _cache_viewPortRect;

    CanvasScene* _canvasScene;

    QPixmap _backgroundTexture;

    IViewPortPresenter* _presenter;
};



#endif // VIEWPORT_HPP