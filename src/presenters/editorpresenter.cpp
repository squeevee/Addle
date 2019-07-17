#include "editorpresenter.h"

#include "servicelocator.h"
#include "interfaces/services/ifileservice.h"

#include "interfaces/presenters/icanvaspresenter.h"

#include <QImage>

EditorPresenter::EditorPresenter()
{    
    _canvasPresenter = ServiceLocator::make<ICanvasPresenter>(this);
    _viewPortPresenter = ServiceLocator::make<IEditorViewPortPresenter>(this);


    //resetView();
}
