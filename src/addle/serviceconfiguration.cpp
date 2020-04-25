/**
 * Addle source code
 * Copyright 2019 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "globalconstants.hpp"
#include "serviceconfiguration.hpp"

#include "utilities/configuration/autofactory.hpp"
#include "utilities/configuration/customfactory.hpp"

#include "interfaces/models/ibrushmodel.hpp"

#include "core/editing/brushpainters/basicbrushpainter.hpp"
#include "core/editing/rasterdiff.hpp"
#include "core/editing/rastersurface.hpp"

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
//#include "core/services/taskservice.hpp"

#include "core/format/qtimageformatdriver.hpp"
//#include "core/tasks/taskcontroller.hpp"
//#include "core/tasks/loaddocumentfiletask.hpp"

#include "widgetsgui/main/maineditorview.hpp"

void ServiceConfiguration::configure()
{
    CONFIG_AUTOFACTORY_BY_TYPE(IRasterDiff, RasterDiff);

    // ## Surfaces
    CONFIG_AUTOFACTORY_BY_TYPE(IRasterSurface, RasterSurface);

    // # Models
    CONFIG_AUTOFACTORY_BY_TYPE(ILayer, Layer);
    CONFIG_AUTOFACTORY_BY_TYPE(IDocument, Document);

    // # Presenters
    CONFIG_AUTOFACTORY_BY_TYPE(IMainEditorPresenter, MainEditorPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(ICanvasPresenter, CanvasPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IViewPortPresenter, ViewPortPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IErrorPresenter, ErrorPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(ILayerPresenter, LayerPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IDocumentPresenter, DocumentPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IBrushOperationPresenter, BrushOperationPresenter);

    // ## Asset presenters
    CONFIG_AUTOFACTORY_BY_TYPE(IBrushPresenter, BrushPresenter);

    // ## Tool presenters
    CONFIG_AUTOFACTORY_BY_TYPE(INavigateToolPresenter, NavigateToolPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IBrushToolPresenter, BrushToolPresenter);

    // # Rendering
    CONFIG_AUTOFACTORY_BY_TYPE(IRenderStack, RenderStack);

    // # Services
    CONFIG_AUTOFACTORY_BY_TYPE(IApplicationService, ApplicationService);
    CONFIG_AUTOFACTORY_BY_TYPE(IFormatService, FormatService);
    //CONFIG_AUTOFACTORY_BY_TYPE(ITaskService, TaskService);

    // # Tasks
    //CONFIG_AUTOFACTORY_BY_TYPE(ITaskController, TaskController);
    //CONFIG_AUTOFACTORY_BY_TYPE(ILoadDocumentFileTask, LoadDocumentFileTask);

    // # Formats
    CONFIG_CUSTOMFACTORY_BY_ID(IFormatDriver, GlobalConstants::CoreFormats::JPEG, 
        []() -> IFormatDriver* { return new QtImageFormatDriver(GlobalConstants::CoreFormats::JPEG, "JPEG"); }
    );
    CONFIG_CUSTOMFACTORY_BY_ID(IFormatDriver, GlobalConstants::CoreFormats::PNG, 
        []() -> IFormatDriver* { return new QtImageFormatDriver(GlobalConstants::CoreFormats::PNG, "PNG"); }
    );

    // # Views
    CONFIG_AUTOFACTORY_BY_TYPE(IMainEditorView, MainEditorView);

    CONFIG_AUTOFACTORY_BY_ID(IBrushPainter, GlobalConstants::CoreBrushes::BasicBrush, BasicBrushPainter);
    CONFIG_CUSTOMFACTORY_BY_ID(IBrushModel, GlobalConstants::CoreBrushes::BasicBrush, &CoreBrushModel::make<BasicBrushPainter>);
}