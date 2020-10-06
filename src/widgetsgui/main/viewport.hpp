/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef VIEWPORT_HPP
#define VIEWPORT_HPP

#include "compat.hpp"
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTransform>

#include <QSharedPointer>

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "utilities/initializehelper.hpp"

namespace Addle {

class CanvasScene;
class ICanvasPresenter;
class IViewPortPresenter;
class IDocumentPresenter;
class ADDLE_WIDGETSGUI_EXPORT ViewPort : public QGraphicsView
{
    Q_OBJECT
public:
    ViewPort(IViewPortPresenter& presenter);
    virtual ~ViewPort() = default;

protected:
    void resizeEvent(QResizeEvent* event);
    void moveEvent(QMoveEvent *event);

    void focusInEvent(QFocusEvent* focusEvent);
    void focusOutEvent(QFocusEvent* focusEvent);

private slots:
    void setDocument(QSharedPointer<Addle::IDocumentPresenter> documentPresenter);
    void onTransformsChanged();
    void updateCursor();

private:
    CanvasScene* _canvasScene;

    IViewPortPresenter& _presenter;
    QSharedPointer<IDocumentPresenter> _documentPresenter;
};

} // namespace Addle

#endif // VIEWPORT_HPP
