#ifndef VIEWPORTSCROLLWIDGET_HPP
#define VIEWPORTSCROLLWIDGET_HPP

#include <QWidget>
#include <QGridLayout>
#include <QAction>
#include <QScrollBar>

#include "interfaces/views/iviewport.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class ViewPortScrollWidget : public QWidget
{
    Q_OBJECT
public:
    ViewPortScrollWidget(IViewPortPresenter& presenter, QWidget* parent = nullptr);
    virtual ~ViewPortScrollWidget() = default;

    IViewPort* getViewPort() { return _presenter.getViewPort(); }

    QSize sizeHint() const { return QSize(640, 480); }

private slots: 
    void onScrollStateChanged();

private:
    IViewPortPresenter& _presenter;

    QGridLayout* _layout;

    QScrollBar* _scrollbar_horizontal;
    QScrollBar* _scrollbar_vertical;
};

#endif // VIEWPORTSCROLLWIDGET_HPP