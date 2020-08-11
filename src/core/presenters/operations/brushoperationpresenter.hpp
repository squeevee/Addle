#ifndef BRUSHOPERATIONPRESENTER_HPP
#define BRUSHOPERATIONPRESENTER_HPP

#include "compat.hpp"
#include <QObject>

#include "interfaces/presenters/operations/ibrushoperationpresenter.hpp"
#include "interfaces/editing/irasterdiff.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT BrushOperationPresenter : public QObject, public IBrushOperationPresenter
{
    Q_OBJECT
public: 
    virtual ~BrushOperationPresenter() = default;

    void initialize(
        QWeakPointer<BrushStroke> BrushStroke,
        QWeakPointer<ILayerPresenter> layer
    );

    QString getText() { return tr("Brush Stroke"); }

public slots: 
    void do_();
    void undo();

private:
    QWeakPointer<ILayerPresenter> _layer;

    QSharedPointer<IRasterDiff> _operation;

    friend class BrushOperationPreview;
};

} // namespace Addle
#endif // BRUSHOPERATIONPRESENTER_HPP