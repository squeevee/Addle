#include "brushpresenter.hpp"
#include "servicelocator.hpp"

#include "interfaces/models/ibrushmodel.hpp"

#include "utilities/qtextensions/qlist.hpp"
#include "globalconstants.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include <QtDebug>

void BrushPresenter::initialize(IBrushModel& model)
{
    _initHelper.initializeBegin();

    _model = &model;
    _id = _model->id();

    _sizeSelection = ServiceLocator::makeUnique<ISizeSelectionPresenter>();

    connect_interface(_sizeSelection.get(), SIGNAL(scaleChanged(double)), this, SLOT(onSizeSelectionScaleChanged()));
    
    updateSizeSelectionIcons();
    
    _initHelper.initializeEnd();
}

void BrushPresenter::initialize(BrushId id)
{
    _initHelper.initializeBegin();

    initialize(ServiceLocator::get<IBrushModel>(id));

    _initHelper.initializeEnd();
}

void BrushPresenter::onSizeSelectionScaleChanged()
{
    updateSizeSelectionIcons();
}

void BrushPresenter::updateSizeSelectionIcons()
{
    _iconHelper.setBackground(Qt::white);
    _iconHelper.setScale(_sizeSelection->scale());

    QList<double> presets = _model->info().getPreferredSizes();
    if (presets.isEmpty())
        presets = qListFromArray(IBrushPresenterAux::DEFAULT_SIZES);

    _sizeSelection->setPresets(presets);

    QList<QIcon> icons;
    for(double preset : presets)
    {
        icons.append(_iconHelper.icon(_id, Qt::blue, preset));
    }
    _sizeSelection->setPresetIcons(icons);
}