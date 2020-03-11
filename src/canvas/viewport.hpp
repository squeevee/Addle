#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "common/interfaces/views/iviewport.hpp"
#include "common/utilities/initializehelper.hpp"

#include <QObject>
#include <QQuickWidget>

class ViewPort : public QQuickWidget, public IViewPort
{
    Q_OBJECT
public:
    ViewPort(QWidget* parent = nullptr)
        : QQuickWidget(parent), _initHelper(this)
    {
    }
    virtual ~ViewPort() = default;

    void initialize(IViewPortPresenter* presenter, ICanvasPresenter* canvasPresenter);

private:
    InitializeHelper<ViewPort> _initHelper;
};

#endif // VIEWPORT_HPP