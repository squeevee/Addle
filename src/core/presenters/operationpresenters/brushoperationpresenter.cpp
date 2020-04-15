#include "brushoperationpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/editing/irastersurface.hpp"
#include "interfaces/editing/ibrushpainter.hpp"
#include "interfaces/models/ilayer.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/editing/irasterdiff.hpp"

void BrushOperationPresenter::initialize(QWeakPointer<IBrushPainter> brushPainter, QWeakPointer<ILayerPresenter> layerPresenter)
{
    auto s_layerPresenter = layerPresenter.toStrongRef();
    auto s_layerModel = s_layerPresenter->getModel().toStrongRef();
    auto s_brushPainter = brushPainter.toStrongRef();
    
    _operation = ServiceLocator::makeShared<IRasterDiff>(
        std::ref(*s_brushPainter->getBuffer()),
        s_layerModel->getRasterSurface()
    );
}

void BrushOperationPresenter::do_()
{ 
    _operation->apply();
}

void BrushOperationPresenter::undo()
{
    _operation->apply();
}