#include <QGridLayout>

#include "viewportwidget.hpp"
#include "zoomrotatewidget.hpp"

#include "utilities/qt_extensions/qobject.hpp"

ViewPortWidget::ViewPortWidget(IViewPortPresenter& presenter, QWidget* parent)
    : QWidget(parent), _presenter(presenter)
{
    _layout = new QGridLayout(this);
    QWidget::setLayout(_layout);

    _layout->addWidget(dynamic_cast<QWidget*>(presenter.getViewPort()), 0, 0);

    _scrollbar_horizontal = new QScrollBar(Qt::Orientation::Horizontal, this);
    _scrollbar_horizontal->setSingleStep(10);
    _layout->addWidget(_scrollbar_horizontal, 1, 0);

    _scrollbar_vertical = new QScrollBar(Qt::Orientation::Vertical, this);
    _scrollbar_vertical->setSingleStep(10);
    _layout->addWidget(_scrollbar_vertical, 0, 1);

    connect_interface(&_presenter, SIGNAL(scrollStateChanged()), this, SLOT(onPresenterScrollStateChanged()));

    connect(_scrollbar_horizontal, &QScrollBar::valueChanged, [&](int x){ _presenter.scrollX(x); });
    connect(_scrollbar_vertical, &QScrollBar::valueChanged, [&](int y){ _presenter.scrollY(y); });

    QWidget::resize(640, 480); //todo

    updateScrollBars();
}

void ViewPortWidget::updateScrollBars()
{
    const QSignalBlocker block_h(_scrollbar_horizontal);
    const QSignalBlocker block_v(_scrollbar_vertical);

    if (_presenter.canScroll())
    {
        QRect scrollRect = _presenter.getScrollRect();

        int width = _presenter.getViewPortSize().width();
        _scrollbar_horizontal->setEnabled(true);
        _scrollbar_horizontal->setMinimum(scrollRect.x());
        _scrollbar_horizontal->setMaximum(scrollRect.x() + scrollRect.width());
        _scrollbar_horizontal->setPageStep(width);
        _scrollbar_horizontal->setValue(0);

        int height = _presenter.getViewPortSize().height();
        _scrollbar_vertical->setEnabled(true);
        _scrollbar_vertical->setMinimum(scrollRect.y());
        _scrollbar_vertical->setMaximum(scrollRect.y() + scrollRect.height());
        _scrollbar_vertical->setPageStep(height);
        _scrollbar_vertical->setValue(0);
    }
    else
    {
        _scrollbar_horizontal->setEnabled(false);
        _scrollbar_horizontal->setMinimum(0);
        _scrollbar_horizontal->setMaximum(0);
        _scrollbar_horizontal->setPageStep(0);

        _scrollbar_vertical->setEnabled(false);
        _scrollbar_vertical->setMinimum(0);
        _scrollbar_vertical->setMaximum(0);
        _scrollbar_vertical->setPageStep(0);
    }
}

void ViewPortWidget::onPresenterScrollStateChanged()
{
    updateScrollBars();
}