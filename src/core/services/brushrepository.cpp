#include "brushrepository.hpp"
#include "interfaces/models/ibrush.hpp"
#include "interfaces/editing/brushrenderers/ibasicbrushrenderer.hpp"

#include "servicelocator.hpp"

BrushRepository::BrushRepository()
    : _repoHelper(*this)
{
    QSharedPointer<IBrush> basicBrush = ServiceLocator::makeShared<IBrush>(
        IBasicBrushRenderer::Id,
        ServiceLocator::makeShared<IBasicBrushRenderer>()
    );

    _repoHelper.setCollection({
        { IBasicBrushRenderer::Id, basicBrush }
        //{ CoreBrushes::IAliasedCircleBrush::Id, ServiceLocator::makeShared<CoreBrushes::IAliasedCircleBrush>() },
        //{ CoreBrushes::ISquareBrush::Id, ServiceLocator::makeShared<CoreBrushes::ISquareBrush>() }
    });
}