#include "brushoperationpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/editing/irastersurface.hpp"
#include "utilities/editing/brushstroke.hpp"
#include "interfaces/models/ilayer.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/editing/irasterdiff.hpp"
using namespace Addle;

void BrushOperationPresenter::initialize(QWeakPointer<BrushStroke> BrushStroke, QWeakPointer<ILayerPresenter> layerPresenter)
{
    auto s_layerPresenter = layerPresenter.toStrongRef();
    auto s_BrushStroke = BrushStroke.toStrongRef();
    
    _operation = ServiceLocator::makeShared<IRasterDiff>(
        std::ref(*s_BrushStroke->getBuffer()),
        s_layerPresenter->getModel()->getRasterSurface()
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