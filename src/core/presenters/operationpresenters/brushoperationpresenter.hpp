#ifndef BRUSHOPERATIONPRESENTER_HPP
#define BRUSHOPERATIONPRESENTER_HPP

#include <QObject>

#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"
#include "interfaces/rendering/irenderstep.hpp"

class BrushOperationPresenter;
class BrushOperationPreview : public QObject, public IRenderStep
{
    Q_OBJECT
public: 
    BrushOperationPreview(BrushOperationPresenter& owner)
        : _owner(owner)
    {
    }
    virtual ~BrushOperationPreview() = default;

    void before(RenderData& data);
    void after(RenderData& data);

signals: 
    void changed(QRect area);

private: 
    BrushOperationPresenter& _owner;

    friend class BrushOperationPresenter;
};

class BrushOperationPresenter : public QObject, public IBrushOperationPresenter
{
    Q_OBJECT
public: 
    virtual ~BrushOperationPresenter() = default;

    void initialize(QWeakPointer<ILayerPresenter> layer, QSharedPointer<IBrushPresenter> brush);

    void addPainterData(BrushPainterData data);
    
    QString getText() { return tr("Brush Stroke"); }

    bool isFinalized() { return _isFinalized; }
    void finalize();

    QSharedPointer<IRenderStep> getPreview() { return _preview; }

public slots: 
    void do_();
    void undo();

signals: 
    void isFinalizedChanged(bool isFinalized);

private:
    QSharedPointer<BrushOperationPreview> _preview;

    QImage _buffer;

    QWeakPointer<ILayerPresenter> _layer;
    QSharedPointer<IBrushPresenter> _brush;

    bool _isFinalized = false;

    friend class BrushOperationPreview;
};

#endif // BRUSHOPERATIONPRESENTER_HPP