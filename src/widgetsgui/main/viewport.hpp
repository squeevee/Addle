#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "compat.hpp"
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "utilities/initializehelper.hpp"

class CanvasScene;
class ICanvasPresenter;
class IViewPortPresenter;
class IDocumentPresenter;
class ADDLE_WIDGETSGUI_EXPORT ViewPort : public QGraphicsView
{
    Q_OBJECT
public:
    ViewPort(IViewPortPresenter* presenter);
    virtual ~ViewPort() = default;

protected:
    void resizeEvent(QResizeEvent* event);
    void moveEvent(QMoveEvent *event);

private slots:
    void setDocument(IDocumentPresenter* documentPresenter);
    void onTransformsChanged();

private:
    CanvasScene* _canvasScene;

    IViewPortPresenter* _presenter;
    IMainEditorPresenter* _mainEditorPresenter;
    ICanvasPresenter* _canvasPresenter;
    IDocumentPresenter* _documentPresenter = nullptr;
};



#endif // VIEWPORT_HPP