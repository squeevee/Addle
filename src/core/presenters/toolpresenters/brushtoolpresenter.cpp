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

void BrushToolPresenter::onEngage()
{
    auto layer = _mainEditorPresenter->getSelectedLayer()->getModel();
    auto painter = _assetsHelper.getSelectedAssetPresenter()->getModel()->getPainter();

    _operation = ServiceLocator::makeShared<IBrushOperation>(
        layer,
        painter,
        IBrushOperation::paint
    );
    BrushPainterData painterData(
        Qt::blue, 25, _mouseHelper.getLatestPosition()
    );
    _operation->addPainterData(painterData);
}

void BrushToolPresenter::onMove()
{
    BrushPainterData painterData(
        Qt::blue, 25,
        _mouseHelper.getPreviousPosition(),
        _mouseHelper.getLatestPosition()
    );
    _operation->addPainterData(painterData);
}

void BrushToolPresenter::onDisengage()
{
    _mainEditorPresenter->doOperation(_operation.staticCast<IUndoableOperation>());
}