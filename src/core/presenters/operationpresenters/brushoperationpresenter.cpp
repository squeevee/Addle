#include "brushoperationpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/editing/ibrushpainter.hpp"
#include "interfaces/models/ilayer.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"

void BrushOperationPresenter::initialize(QWeakPointer<IBrushPainter> brushPainter, QWeakPointer<ILayerPresenter> layerPresenter)
{
    auto s_layerPresenter = layerPresenter.toStrongRef();
    auto s_layerModel = s_layerPresenter->getModel().toStrongRef();

    auto s_brushPainter = brushPainter.toStrongRef();
    
    QPoint offset;
    QImage data = s_brushPainter->getBuffer()->copy(&offset);

    _operation = ServiceLocator::makeShared<IRasterOperation>(
        s_layerModel->getRasterSurface(),
        data,
        offset
    );
}

void BrushOperationPresenter::do_()
{
    _operation->do_();
}

void BrushOperationPresenter::undo()
{
    _operation->undo();
}