#ifndef BRUSHOPERATIONPRESENTER_HPP
#define BRUSHOPERATIONPRESENTER_HPP

#include <QObject>

#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"
#include "interfaces/editing/operations/irasteroperation.hpp"

class BrushOperationPresenter : public QObject, public IBrushOperationPresenter
{
    Q_OBJECT
public: 
    virtual ~BrushOperationPresenter() = default;

    void initialize(
        QWeakPointer<IBrushPainter> brushPainter,
        QWeakPointer<ILayerPresenter> layer
    );

    QString getText() { return tr("Brush Stroke"); }

public slots: 
    void do_();
    void undo();

private:
    QWeakPointer<ILayerPresenter> _layer;

    QSharedPointer<IRasterOperation> _operation;

    friend class BrushOperationPreview;
};

#endif // BRUSHOPERATIONPRESENTER_HPP