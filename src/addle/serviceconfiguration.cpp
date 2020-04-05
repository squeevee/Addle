/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "serviceconfiguration.hpp"

#include "utilities/configuration/autofactory.hpp"
#include "utilities/configuration/customfactory.hpp"

#include "interfaces/models/ibrushmodel.hpp"

#include "core/editing/brushpainters/basicbrushpainter.hpp"
#include "core/editing/operations/rasteroperation.hpp"
#include "core/editing/surfaces/rastersurface.hpp"

#include "core/models/layer.hpp"
#include "core/models/document.hpp"
#include "core/models/brushmodels/corebrushmodel.hpp"

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

CONFIG_STATIC_AUTOFACTORY(IRasterOperation, RasterOperation);

// ## Surfaces
CONFIG_STATIC_AUTOFACTORY(IRasterSurface, RasterSurface);

// # Models
CONFIG_STATIC_AUTOFACTORY(ILayer, Layer);
CONFIG_STATIC_AUTOFACTORY(IDocument, Document);

// # Presenters
CONFIG_STATIC_AUTOFACTORY(IMainEditorPresenter, MainEditorPresenter);
CONFIG_STATIC_AUTOFACTORY(ICanvasPresenter, CanvasPresenter);
CONFIG_STATIC_AUTOFACTORY(IViewPortPresenter, ViewPortPresenter);
CONFIG_STATIC_AUTOFACTORY(IErrorPresenter, ErrorPresenter);
CONFIG_STATIC_AUTOFACTORY(ILayerPresenter, LayerPresenter);
CONFIG_STATIC_AUTOFACTORY(IDocumentPresenter, DocumentPresenter);
CONFIG_STATIC_AUTOFACTORY(IBrushOperationPresenter, BrushOperationPresenter);

// ## Asset presenters
CONFIG_STATIC_AUTOFACTORY(IBrushPresenter, BrushPresenter);

// ## Tool presenters
CONFIG_STATIC_AUTOFACTORY(INavigateToolPresenter, NavigateToolPresenter);
CONFIG_STATIC_AUTOFACTORY(IBrushToolPresenter, BrushToolPresenter);

// # Rendering
CONFIG_STATIC_AUTOFACTORY(IRenderStack, RenderStack);

// # Services
CONFIG_STATIC_AUTOFACTORY(IApplicationService, ApplicationService);
CONFIG_STATIC_AUTOFACTORY(IFormatService, FormatService);
CONFIG_STATIC_AUTOFACTORY(ITaskService, TaskService);

// # Tasks
CONFIG_STATIC_AUTOFACTORY(ITaskController, TaskController);
CONFIG_STATIC_AUTOFACTORY(ILoadDocumentFileTask, LoadDocumentFileTask);

// # Formats
CONFIG_STATIC_AUTOFACTORY(IPNGFormatDriver, QtPNGFormatDriver);
CONFIG_STATIC_AUTOFACTORY(IJPEGFormatDriver, QtJPEGFormatDriver);

// # Views
CONFIG_STATIC_AUTOFACTORY(IMainEditorView, MainEditorView);


void ServiceConfiguration::configure()
{
    CONFIG_DYNAMIC_AUTOFACTORY(IBrushPainter, IBrushModel::CoreBrushes::BasicBrush, BasicBrushPainter);
    CONFIG_DYNAMIC_CUSTOMFACTORY(IBrushModel, IBrushModel::CoreBrushes::BasicBrush, &CoreBrushModel::make<BasicBrushPainter>);
}