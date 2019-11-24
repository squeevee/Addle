/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "serviceconfiguration.hpp"

#include "common/utilities/configuration/tfactory.hpp"
#include "common/utilities/configuration/qobjectfactory.hpp"

#include "core/models/layer.hpp"
#include "core/models/document.hpp"

#include "core/presenters/canvaspresenter.hpp"
#include "core/presenters/editorpresenter.hpp"
#include "core/presenters/viewportpresenter.hpp"
#include "core/presenters/errorpresenter.hpp"
#include "core/presenters/tools/navigatetoolpresenter.hpp"

#include "core/services/applicationservice.hpp"
#include "core/services/formatservice.hpp"
#include "core/services/taskservice.hpp"

#include "core/format/qtformatdrivers/qtpngformatdriver.hpp"
#include "core/format/qtformatdrivers/qtjpegformatdriver.hpp"

#include "core/tasks/taskcontroller.hpp"
#include "core/tasks/loaddocumentfiletask.hpp"

#include "widgetsgui/views/editorview.hpp"
#include "widgetsgui/views/viewport.hpp"
#include "widgetsgui/views/canvasview.hpp"

void ServiceConfiguration::configure()
{
    // # Models
    REGISTER_TFACTORY(ILayer, Layer);
    REGISTER_TFACTORY(IDocument, Document);

    // # Presenters
    REGISTER_TFACTORY(ICanvasPresenter, CanvasPresenter);
    REGISTER_TFACTORY(IEditorPresenter, EditorPresenter);
    REGISTER_TFACTORY(IViewPortPresenter, ViewPortPresenter);
    REGISTER_TFACTORY(IErrorPresenter, ErrorPresenter);

    // ## Tool presenters
    REGISTER_QOBJECT_FACTORY(INavigateToolPresenter, NavigateToolPresenter);

    // # Services
    REGISTER_QOBJECT_FACTORY(IApplicationService, ApplicationService);
    REGISTER_TFACTORY(IFormatService, FormatService);
    REGISTER_QOBJECT_FACTORY(ITaskService, TaskService);

    // # Tasks
    REGISTER_QOBJECT_FACTORY(ITaskController, TaskController);
    REGISTER_TFACTORY(ILoadDocumentFileTask, LoadDocumentFileTask);

    // # Formats
    REGISTER_TFACTORY(IPNGFormatDriver, QtPNGFormatDriver);
    REGISTER_TFACTORY(IJpegFormatDriver, QtJpegFormatDriver);

    // # Views
    REGISTER_QOBJECT_FACTORY(IEditorView, EditorView);
    REGISTER_QOBJECT_FACTORY(IViewPort, ViewPort);
    REGISTER_QOBJECT_FACTORY(ICanvasView, CanvasView);
}
