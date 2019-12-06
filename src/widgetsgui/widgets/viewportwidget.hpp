#ifndef VIEWPORTWIDGET_HPP
#define VIEWPORTWIDGET_HPP

#include <QWidget>
#include <QGridLayout>
#include <QAction>
#include <QScrollBar>

#include "interfaces/views/iviewport.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class ViewPortWidget : public QWidget
{
    Q_OBJECT
public:
    ViewPortWidget(IViewPortPresenter& presenter, QWidget* parent = nullptr);
    virtual ~ViewPortWidget() = default;

    IViewPort* getViewPort() { return _presenter.getViewPort(); }

public slots:
    void updateScrollBars();

private slots: 
    void onPresenterScrollStateChanged();

private:
    IViewPortPresenter& _presenter;

    QGridLayout* _layout;

    QScrollBar* _scrollbar_horizontal;
    QScrollBar* _scrollbar_vertical;
};

#endif // VIEWPORTWIDGET_HPP