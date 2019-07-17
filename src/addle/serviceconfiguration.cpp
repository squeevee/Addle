/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "serviceconfiguration.h"

#include "models/layer.h"
#include "models/document.h"

#include "presenters/canvaspresenter.h"
#include "presenters/editorpresenter.h"
#include "presenters/editorviewportpresenter.h"

#include "services/fileservice.h"

#include "utilities/tfactory.h"

void configure::ServiceConfiguration::configure()
{
    //Models
    REGISTER_TFACTORY(ILayer, Layer);
    REGISTER_TFACTORY(IDocument, Document);

    //Presenters
    REGISTER_TFACTORY(ICanvasPresenter, CanvasPresenter);
    REGISTER_TFACTORY(IEditorPresenter, EditorPresenter);
    REGISTER_TFACTORY(IEditorViewPortPresenter, EditorViewPortPresenter);

    //Services
    REGISTER_TFACTORY(IFileService, FileService);
}
