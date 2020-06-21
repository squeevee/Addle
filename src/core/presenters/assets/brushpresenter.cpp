#include "brushpresenter.hpp"
#include "servicelocator.hpp"

#include "utilities/addle_text.hpp"

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

    QList<double> presets = _model->info().getPreferredSizes();
    if (presets.isEmpty())
        presets = qListFromArray(IBrushPresenterAux::DEFAULT_SIZES);

    _sizeSelection->setPresets(presets);

    connect_interface(_sizeSelection.get(), SIGNAL(scaleChanged(double)), this, SLOT(onSizeSelectionScaleChanged()));
    
    if (!_model->icon().isNull())
    {
        _assetIcon = _model->icon();
    }
    else 
    {
        _assetIcon = _iconHelper.icon(_id, Qt::blue);
    }

    _iconHelper.setBackground(Qt::white);
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
    double scale = _sizeSelection->scale();

    QList<QIcon> icons;
    for(double preset : _sizeSelection->presets())
    {
        icons.append(_iconHelper.icon(_id, Qt::blue, preset, scale));
    }
    _sizeSelection->setPresetIcons(icons);
}

QIcon BrushPresenter::icon()
{
    return _assetIcon;
}

QString BrushPresenter::name()
{
    return ADDLE_TEXT(QString("brushes.%1.text").arg(_id.getKey()));
}