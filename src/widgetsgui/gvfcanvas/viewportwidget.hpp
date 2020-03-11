#ifndef VIEWPORTWIDGET_HPP
#define VIEWPORTWIDGET_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>

#include "canvasscene.hpp"

#include "interfaces/views/iviewport.hpp"
#include "interfaces/presenters/ieditorpresenter.hpp"

#include "utilities/initializehelper.hpp"

namespace GVFCanvas
{
    class ViewPortWidget : public QGraphicsView, public IViewPort
    {
        Q_OBJECT
    public:
        ViewPortWidget()
            : _initHelper(this)
        {
        }
        virtual ~ViewPortWidget() = default;

        void initialize(IViewPortPresenter* presenter, ICanvasPresenter* canvasPresenter);

    protected:
        void resizeEvent(QResizeEvent* event);
        void moveEvent(QMoveEvent *event);
        
    private slots:
        void onPresenterTransformsChanged();

    private:
        QTransform calculateTransform();

        CanvasScene* _canvasScene;

        IViewPortPresenter* _presenter;
        InitializeHelper<ViewPortWidget> _initHelper;
    };

    class MouseEventBlocker : public QObject
    {
        Q_OBJECT
    protected:
        bool eventFilter(QObject* object, QEvent* event) override;
    };
}

#endif // VIEWPORTWIDGET_HPP