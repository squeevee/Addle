/**
 * Addle source code
 * Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "globals.hpp"
#include "serviceconfiguration.hpp"

#include "utilities/configuration/autofactory.hpp"
#include "utilities/configuration/customfactory.hpp"

#include "interfaces/models/ibrush.hpp"

#include "core/editing/brushengines/pathbrushengine.hpp"
#include "core/editing/brushengines/rasterbrushengine.hpp"

#include "core/editing/rasterdiff.hpp"
#include "core/editing/rastersurface.hpp"

#include "core/models/layer.hpp"
#include "core/models/document.hpp"
#include "core/models/brush.hpp"
#include "core/models/palette.hpp"

#include "core/presenters/maineditorpresenter.hpp"
#include "core/presenters/documentpresenter.hpp"
#include "core/presenters/canvaspresenter.hpp"
#include "core/presenters/viewportpresenter.hpp"
#include "core/presenters/errorpresenter.hpp"
#include "core/presenters/layerpresenter.hpp"
#include "core/presenters/colorselectionpresenter.hpp"
#include "core/presenters/palettepresenter.hpp"
#include "core/presenters/assets/brushpresenter.hpp"
#include "core/presenters/tools/assetselectionpresenter.hpp"
#include "core/presenters/tools/navigatetoolpresenter.hpp"
#include "core/presenters/tools/brushtoolpresenter.hpp"
#include "core/presenters/tools/sizeselectionpresenter.hpp"
#include "core/presenters/operations/brushoperationpresenter.hpp"

#include "core/rendering/renderstack.hpp"

#include "core/services/appearanceservice.hpp"
#include "core/services/applicationservice.hpp"
#include "core/services/formatservice.hpp"

#include "core/format/qtimageformatdriver.hpp"

#include "widgetsgui/main/maineditorview.hpp"

#include "core/models/corebrushbuilders.hpp"
#include "core/models/corepalettebuilders.hpp"

using namespace Addle;

void ServiceConfiguration::configure()
{

    CONFIG_AUTOFACTORY_BY_ID(IBrushEngine, PathBrushEngine::ID, PathBrushEngine);
    CONFIG_AUTOFACTORY_BY_ID(IBrushEngine, RasterBrushEngine::ID, RasterBrushEngine);

    CONFIG_AUTOFACTORY_BY_TYPE(IRasterDiff, RasterDiff);

    // ## Surfaces
    CONFIG_AUTOFACTORY_BY_TYPE(IRasterSurface, RasterSurface);

    // # Models
    CONFIG_AUTOFACTORY_BY_TYPE(ILayer, Layer);
    CONFIG_AUTOFACTORY_BY_TYPE(IDocument, Document);

    CONFIG_AUTOFACTORY_BY_TYPE(IBrush, Brush);

    buildPersistentObject<IBrush>(CoreBrushes::BasicBrush, CoreBrushBuilders::basic);
    buildPersistentObject<IBrush>(CoreBrushes::SoftBrush, CoreBrushBuilders::soft);
    buildPersistentObject<IBrush>(CoreBrushes::BasicEraser, CoreBrushBuilders::basicEraser);

    CONFIG_AUTOFACTORY_BY_TYPE(IPalette, Palette);

    buildPersistentObject<IPalette>(CorePalettes::BasicPalette, CorePaletteBuilders::basic);

    // # Presenters
    CONFIG_AUTOFACTORY_BY_TYPE(IMainEditorPresenter, MainEditorPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(ICanvasPresenter, CanvasPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IColorSelectionPresenter, ColorSelectionPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IViewPortPresenter, ViewPortPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IErrorPresenter, ErrorPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(ILayerPresenter, LayerPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IDocumentPresenter, DocumentPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IBrushOperationPresenter, BrushOperationPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IPalettePresenter, PalettePresenter);

    // ## Asset presenters
    CONFIG_AUTOFACTORY_BY_TYPE(IBrushPresenter, BrushPresenter);

    // ## Tool presenters
    CONFIG_AUTOFACTORY_BY_TYPE(IAssetSelectionPresenter, AssetSelectionPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(INavigateToolPresenter, NavigateToolPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(IBrushToolPresenter, BrushToolPresenter);
    CONFIG_AUTOFACTORY_BY_TYPE(ISizeSelectionPresenter, SizeSelectionPresenter);

    // # Rendering
    CONFIG_AUTOFACTORY_BY_TYPE(IRenderStack, RenderStack);

    // # Services
    CONFIG_AUTOFACTORY_BY_TYPE(IAppearanceService, AppearanceService);
    CONFIG_AUTOFACTORY_BY_TYPE(IApplicationService, ApplicationService);
    CONFIG_AUTOFACTORY_BY_TYPE(IFormatService, FormatService);

    // # Formats
    CONFIG_CUSTOMFACTORY_BY_ID(IFormatDriver, CoreFormats::JPEG, 
        []() -> IFormatDriver* { return new QtImageFormatDriver(CoreFormats::JPEG, "JPEG"); }
    );
    CONFIG_CUSTOMFACTORY_BY_ID(IFormatDriver, CoreFormats::PNG, 
        []() -> IFormatDriver* { return new QtImageFormatDriver(CoreFormats::PNG, "PNG"); }
    );

    // # Views
    CONFIG_AUTOFACTORY_BY_TYPE(IMainEditorView, MainEditorView);
}