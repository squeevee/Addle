#include "canvaspresenter.hpp"

void CanvasPresenter::initialize(IDocumentPresenter* documentPresenter)
{
    _initHelper.initializeBegin();

    _documentPresenter = documentPresenter;

    _initHelper.initializeEnd();
}