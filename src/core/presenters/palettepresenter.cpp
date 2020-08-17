#include "palettepresenter.hpp"
#include "interfaces/models/ipalette.hpp"

#include "servicelocator.hpp"

using namespace Addle;

void PalettePresenter::initialize(IPalette& model)
{
    const Initializer init(_initHelper);

    _model = &model;
}

void PalettePresenter::initialize(PaletteId id)
{
    const Initializer init(_initHelper);

    _model = &ServiceLocator::get<IPalette>(id);
}