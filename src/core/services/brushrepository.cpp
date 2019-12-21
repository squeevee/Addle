#include "brushrepository.hpp"

#include "interfaces/models/brushes/corebrushes.hpp"
#include "servicelocator.hpp"

BrushRepository::BrushRepository()
    : _repoHelper(*this)
{
    _repoHelper.setCollection({
        { CoreBrushes::ISmoothCircleBrush::Id, ServiceLocator::makeShared<CoreBrushes::ISmoothCircleBrush>() }
        //{ CoreBrushes::IAliasedCircleBrush::Id, ServiceLocator::makeShared<CoreBrushes::IAliasedCircleBrush>() },
        //{ CoreBrushes::ISquareBrush::Id, ServiceLocator::makeShared<CoreBrushes::ISquareBrush>() }
    });
}