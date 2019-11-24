#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>

#include "common/interfaces/presenters/ieditorpresenter.hpp"
#include "common/interfaces/presenters/icanvaspresenter.hpp"

#include "common/interfaces/views/iviewport.hpp"
#include "common/utilities/initializehelper.hpp"

class ViewPort : public QGraphicsView, public virtual IViewPort
{
    Q_OBJECT
public:
    ViewPort() : _initHelper(this)
    {
    }
    virtual ~ViewPort() = default;

    void initialize(IViewPortPresenter* presenter);

protected:
    void resizeEvent(QResizeEvent *event);
    
private slots:
    void onPresenterTransformsChanged();

private:
    IViewPortPresenter* _presenter;
    ICanvasPresenter* _canvasPresenter;

    InitializeHelper<ViewPort> _initHelper;
};

class MouseEventBlocker : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* object, QEvent* event) override;
};

#endif // VIEWPORT_HPP