#include "brushrepository.hpp"
#include "interfaces/models/ibrush.hpp"
#include "interfaces/editing/brushpainters/ibasicbrushpainter.hpp"

#include "servicelocator.hpp"

BrushRepository::BrushRepository()
    : _repoHelper(*this)
{
    QSharedPointer<IBrush> basicBrush = ServiceLocator::makeShared<IBrush>(
        IBasicBrushPainter::Id,
        ServiceLocator::makeShared<IBasicBrushPainter>()
    );

    _repoHelper.setCollection({
        { IBasicBrushPainter::Id, basicBrush }
        //{ CoreBrushes::IAliasedCircleBrush::Id, ServiceLocator::makeShared<CoreBrushes::IAliasedCircleBrush>() },
        //{ CoreBrushes::ISquareBrush::Id, ServiceLocator::makeShared<CoreBrushes::ISquareBrush>() }
    });
}