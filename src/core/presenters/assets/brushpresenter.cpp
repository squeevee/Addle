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

    _iconHelper.setBrush(_id);
    _iconHelper.setBackground(Qt::white);

    _sizeSelection = ServiceLocator::makeUnique<ISizeSelectionPresenter>(_iconHelper.sizeIconProvider());

    QList<double> presets = _model->preferredSizes();
    if (presets.isEmpty())
    {
        for (double preset : IBrushPresenterAux::DEFAULT_SIZES)
        {
            if (preset >= _model->minSize() && preset <= _model->maxSize())
                presets.append(preset);
            // probably a way to do this in one line with iterators
            // try it if you're feeling spicy.
        }
    }
    _sizeSelection->setPresets(presets);
    
    _sizeSelection->setMin(_model->minSize());
    _sizeSelection->setMax(_model->maxSize());
    _sizeSelection->setStrictSizing(_model->strictSizing());

    if (!qIsNaN(_model->preferredStartingSize()))
        _sizeSelection->set(_model->preferredStartingSize());
    else
        _sizeSelection->set(qBound(_model->minSize(), IBrushPresenterAux::DEFAULT_START_SIZE, _model->maxSize()));
    
    if (!_model->icon().isNull())
    {
        _assetIcon = _model->icon();
    }
    else 
    {
        //_assetIcon = _iconHelper.icon(_id, Qt::blue);
    }

    
    _initHelper.initializeEnd();
}

void BrushPresenter::initialize(BrushId id)
{
    _initHelper.initializeBegin();

    initialize(ServiceLocator::get<IBrushModel>(id));

    _initHelper.initializeEnd();
}

QIcon BrushPresenter::icon()
{
    return _assetIcon;
}

QString BrushPresenter::name()
{
    return ADDLE_TEXT(QString("brushes.%1.text").arg(_id.getKey()));
}

void BrushPresenter::setPreviewColor(QColor color)
{
    _iconHelper.setColor(color);
    if (_sizeSelection) emit _sizeSelection->iconsChanged();
}

void BrushPresenter::setPreviewScale(double scale)
{
    _iconHelper.setScale(scale);
    if (_sizeSelection) emit _sizeSelection->iconsChanged();
}