/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "serviceconfiguration.hpp"

#include "utilities/configuration/tfactory.hpp"
//#include "utilities/configuration/qobjectfactory.hpp"

#include "core/editing/brushpainters/basicbrushpainter.hpp"
#include "core/editing/operations/rasteroperation.hpp"
#include "core/editing/surfaces/rastersurface.hpp"

#include "core/models/layer.hpp"
#include "core/models/document.hpp"

#include "core/presenters/maineditorpresenter.hpp"
#include "core/presenters/documentpresenter.hpp"
#include "core/presenters/canvaspresenter.hpp"
#include "core/presenters/viewportpresenter.hpp"
#include "core/presenters/errorpresenter.hpp"
#include "core/presenters/layerpresenter.hpp"
#include "core/presenters/assets/brushpresenter.hpp"
#include "core/presenters/toolpresenters/navigatetoolpresenter.hpp"
#include "core/presenters/toolpresenters/brushtoolpresenter.hpp"
#include "core/presenters/operationpresenters/brushoperationpresenter.hpp"

#include "core/rendering/renderstack.hpp"

#include "core/services/applicationservice.hpp"
#include "core/services/formatservice.hpp"
#include "core/services/taskservice.hpp"

#include "core/format/qtformatdrivers/qtpngformatdriver.hpp"
#include "core/format/qtformatdrivers/qtjpegformatdriver.hpp"

#include "core/tasks/taskcontroller.hpp"
#include "core/tasks/loaddocumentfiletask.hpp"

#include "widgetsgui/main/maineditorview.hpp"

void ServiceConfiguration::configure()
{
    REGISTER_TFACTORY(IBrushPainter, BasicBrushPainter, IBrushPainter::CoreBrushes::BasicBrush);
}

DEFINE_STATIC_FACTORY(IRasterOperation, RasterOperation);

// ## Surfaces
DEFINE_STATIC_FACTORY(IRasterSurface, RasterSurface);

// # Models
DEFINE_STATIC_FACTORY(ILayer, Layer);
DEFINE_STATIC_FACTORY(IDocument, Document);

// # Presenters
DEFINE_STATIC_FACTORY(IMainEditorPresenter, MainEditorPresenter);
DEFINE_STATIC_FACTORY(ICanvasPresenter, CanvasPresenter);
DEFINE_STATIC_FACTORY(IViewPortPresenter, ViewPortPresenter);
DEFINE_STATIC_FACTORY(IErrorPresenter, ErrorPresenter);
DEFINE_STATIC_FACTORY(ILayerPresenter, LayerPresenter);
DEFINE_STATIC_FACTORY(IDocumentPresenter, DocumentPresenter);
DEFINE_STATIC_FACTORY(IBrushOperationPresenter, BrushOperationPresenter);

// ## Asset presenters
DEFINE_STATIC_FACTORY(IBrushPresenter, BrushPresenter);

// ## Tool presenters
DEFINE_STATIC_FACTORY(INavigateToolPresenter, NavigateToolPresenter);
DEFINE_STATIC_FACTORY(IBrushToolPresenter, BrushToolPresenter);

// # Rendering
DEFINE_STATIC_FACTORY(IRenderStack, RenderStack);

// # Services
DEFINE_STATIC_FACTORY(IApplicationService, ApplicationService);
DEFINE_STATIC_FACTORY(IFormatService, FormatService);
DEFINE_STATIC_FACTORY(ITaskService, TaskService);

// # Tasks
DEFINE_STATIC_FACTORY(ITaskController, TaskController);
DEFINE_STATIC_FACTORY(ILoadDocumentFileTask, LoadDocumentFileTask);

// # Formats
DEFINE_STATIC_FACTORY(IPNGFormatDriver, QtPNGFormatDriver);
DEFINE_STATIC_FACTORY(IJPEGFormatDriver, QtJPEGFormatDriver);

// # Views
DEFINE_STATIC_FACTORY(IMainEditorView, MainEditorView);