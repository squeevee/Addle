#include <QtDebug>

#include "interfaces/presenters/ilayerpresenter.hpp"

#include "interfaces/editing/operations/ibrushoperation.hpp"

#include "utilities/unhandledexceptionrouter.hpp"

#include "brushtoolpresenter.hpp"
#include "servicelocator.hpp"
#include "utilities/mathutils.hpp"

// #include "../helpers/brushsizepresethelper.hpp"
// const BrushSizePresetHelper::PxPresetHelper BrushSizePresetHelper::_instance_px = PxPresetHelper();
// const BrushSizePresetHelper::PercentPresetHelper BrushSizePresetHelper::_instance_percent = PercentPresetHelper();

void BrushToolPresenter::initialize(IMainEditorPresenter* owner)
{
    _initHelper.initializeBegin();

    _mainEditorPresenter = owner;
    ToolPresenterBase::initialize_p(owner);

    _assetsHelper.setAssetList({
        ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Basic)
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::AliasedCircle),
        //ServiceLocator::makeShared<IBrushPresenter>(DefaultBrushes::Square)
    });

    
    _propertyDecorationHelper.initializeIdProperty<BrushId>(
        "brush",
        _assetsHelper.getAssetIds()
    );

    _initHelper.initializeEnd();
}

void BrushToolPresenter::onPointerEngage()
{
    // try
    // {
            
    // ILayerPresenter* layer = _mainEditorPresenter->getSelectedLayer();
    // QSharedPointer<IBrushPresenter> brush = getSelectedBrushPresenter();
    // QSharedPointer<IBrushRenderer> renderer = brush->getModel()->getRenderer();

    // _operation = ServiceLocator::makeShared<IBrushOperation>(
    //     layer->getModel(),
    //     renderer
    // );

    // //layer->setRasterOperation(_operation.staticCast<IRasterOperation>());

    // BrushPathSegment segment( Qt::black, 10, _toolPathHelper.getLastCanvasPosition() );
    // _operation->addPathSegment(segment);

    // QRect bound = renderer->getSegmentBoundingRect(segment);

    // layer->renderChanged(bound);

    // }
    // ADDLE_FALLBACK_CATCH
}

void BrushToolPresenter::onPointerMove()
{
    // try
    // {

    // ILayerPresenter* layer = _mainEditorPresenter->getSelectedLayer();
    // QSharedPointer<IBrushPresenter> brush = getSelectedBrushPresenter();
    // QSharedPointer<IBrushRenderer> renderer = brush->getModel()->getRenderer();

    // BrushPathSegment segment( Qt::black, 10, _toolPathHelper.getPreviousCanvasPosition(), _toolPathHelper.getLastCanvasPosition() );
    // _operation->addPathSegment(segment);

    // QRect bound = renderer->getSegmentBoundingRect(segment);

    // layer->renderChanged(bound);
    
    // }
    // ADDLE_FALLBACK_CATCH
}

void BrushToolPresenter::onPointerDisengage()
{
    // try
    // {

    // _mainEditorPresenter->doOperation(_operation.staticCast<IUndoableOperation>());
    // _operation.clear();

    // }
    // ADDLE_FALLBACK_CATCH
}
