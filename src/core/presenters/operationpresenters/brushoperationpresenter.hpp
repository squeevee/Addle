#ifndef BRUSHOPERATIONPRESENTER_HPP
#define BRUSHOPERATIONPRESENTER_HPP

#include <QObject>
#include <memory>

#include "interfaces/editing/surfaces/irastersurface.hpp"
#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"
#include "interfaces/rendering/irenderstep.hpp"

#include "interfaces/editing/ibrushpainter.hpp"

class BrushOperationPresenter : public QObject, public IBrushOperationPresenter
{
    Q_OBJECT
public: 
    virtual ~BrushOperationPresenter() = default;

    void initialize(QWeakPointer<ILayerPresenter> layer, QSharedPointer<IBrushPresenter> brush);

    QString getText() { return tr("Brush Stroke"); }

    bool isFinalized() { return _isFinalized; }
    void finalize();

    QWeakPointer<IBrushPainter> getBrushPainter() { return _brushPainter; }
    QSharedPointer<IRenderStep> getPreview() { return _preview; }

public slots: 
    void do_();
    void undo();

signals: 
    void finalized();

private:

    QSharedPointer<IBrushPainter> _brushPainter;
    QSharedPointer<IRasterSurface> _buffer;
    QSharedPointer<IRenderStep> _preview;

    QWeakPointer<ILayerPresenter> _layer;
    QSharedPointer<IBrushPresenter> _brushPresenter;

    bool _isFinalized = false;


    friend class BrushOperationPreview;
};

#endif // BRUSHOPERATIONPRESENTER_HPP