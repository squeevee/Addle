/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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