/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef VIEWPORTSCROLLWIDGET_HPP
#define VIEWPORTSCROLLWIDGET_HPP

#include "compat.hpp"
#include <QWidget>
#include <QGridLayout>
#include <QAction>
#include <QScrollBar>

#include "interfaces/presenters/iviewportpresenter.hpp"

namespace Addle {

class ViewPort;

/**
 * It's easier to just make new scroll bars than to get the ones that come 
 * built into QGraphicsView to play nice with the presenter.
 */
class ADDLE_WIDGETSGUI_EXPORT ViewPortScrollWidget : public QWidget
{
    Q_OBJECT
public:
    ViewPortScrollWidget(IViewPortPresenter& presenter, QWidget* parent = nullptr);
    virtual ~ViewPortScrollWidget() = default;

    ViewPort* viewPort() { return _viewPort; }
    void setViewPort(ViewPort* viewPort);

    QSize sizeHint() const { return QSize(640, 480); }

private slots: 
    void onScrollStateChanged();

private:
    IViewPortPresenter& _presenter;

    ViewPort* _viewPort = nullptr;

    QGridLayout* _layout;

    QScrollBar* _scrollbar_horizontal;
    QScrollBar* _scrollbar_vertical;
};

} // namespace Addle

#endif // VIEWPORTSCROLLWIDGET_HPP
