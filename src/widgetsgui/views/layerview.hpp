#ifndef LAYERVIEW_HPP
#define LAYERVIEW_HPP

#include <QObject>
#include <QGraphicsItem>

#include "interfaces/views/ilayerview.hpp"
#include "utilities/initializehelper.hpp"

class LayerView: public QObject, public QGraphicsItem, public ILayerView
{
    Q_OBJECT 
public:
    LayerView() : _initHelper(this) { }
    virtual ~LayerView() = default; 

    void initialize(ILayerPresenter* presenter);

    ILayerPresenter* getPresenter() { _initHelper.assertInitialized(); return _presenter; }

    QRectF boundingRect() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots: 
    void onRenderChanged(QRect area);

private: 

    ILayerPresenter* _presenter;
    InitializeHelper<LayerView> _initHelper;
};

#endif // LAYERVIEW_HPP