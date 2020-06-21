#include "palettepresenter.hpp"

void PalettePresenter::initialize(IPalette& model)
{
    _initHelper.initializeBegin();

    _model = &model;

    _initHelper.initializeEnd();
}