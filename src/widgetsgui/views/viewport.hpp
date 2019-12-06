#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>

#include "interfaces/presenters/ieditorpresenter.hpp"

#include "interfaces/views/iviewport.hpp"
#include "utilities/initializehelper.hpp"

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
    void resizeEvent(QResizeEvent* event);
    void moveEvent(QMoveEvent *event);
    
private slots:
    void onPresenterTransformsChanged();

private:
    QTransform calculateTransform();

    IViewPortPresenter* _presenter;

    InitializeHelper<ViewPort> _initHelper;
};

class MouseEventBlocker : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* object, QEvent* event) override;
};

#endif // VIEWPORT_HPP