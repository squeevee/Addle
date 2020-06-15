// Definitions of Qt string and collection constants

#include <QString>
#include <QByteArray>

#include "servicelocator.hpp"
ServiceLocator* ServiceLocator::_instance = nullptr;

#include "globalconstants.hpp"

#include "interfaces/editing/rasterengineparams.hpp"
#include "interfaces/editing/moc_rasterengineparams.cpp"

const BrushEngineId GlobalConstants::CoreBrushEngines::PathEngine = BrushEngineId(
    "path-engine"
);

const BrushEngineId GlobalConstants::CoreBrushEngines::RasterEngine = BrushEngineId(
    "raster-engine"
);

const BrushId GlobalConstants::CoreBrushes::BasicBrush = BrushId(
    "basic-brush",
    GlobalConstants::CoreBrushEngines::PathEngine
);

const BrushId GlobalConstants::CoreBrushes::SoftBrush = BrushId(
    "soft-brush",
    GlobalConstants::CoreBrushEngines::RasterEngine,
    {
        { "mode", "Gradient" },
        { "hardness", 1.0 },
        { "spacing", 0.12 }
    }
);

#include "idtypes/persistentid.hpp"
#include "idtypes/brushengineid.hpp"
STATIC_PERSISTENT_ID_BOILERPLATE(BrushEngineId)

#include "idtypes/brushid.hpp"
STATIC_PERSISTENT_ID_BOILERPLATE(BrushId)

#include "idtypes/formatid.hpp"
STATIC_PERSISTENT_ID_BOILERPLATE(FormatId)

#include "idtypes/toolid.hpp"
STATIC_PERSISTENT_ID_BOILERPLATE(ToolId)

#include "utilities/canvas/canvasmouseevent.hpp"
int CanvasMouseEvent::_type = QEvent::User;

const QColor GlobalConstants::DefaultBackgroundColor = Qt::white;

#include "interfaces/models/idocument.hpp"

const FormatId GlobalConstants::CoreFormats::PNG = FormatId(
    "png-format-id",
    QStringLiteral("image/png"),
    typeid(IDocument),
    QStringLiteral("png"),
    QByteArrayLiteral("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A")
);

const FormatId GlobalConstants::CoreFormats::JPEG = FormatId(
    "jpeg-format-id",
    QStringLiteral("image/jpeg"),
    typeid(IDocument),
    {
        QStringLiteral("jpg"),
        QStringLiteral("jpeg"),
        QStringLiteral("jpe"),
        QStringLiteral("jfif"),
        QStringLiteral("jif")
    },
    QByteArrayLiteral("\xFF\xD8\xFF")
);

#include "interfaces/presenters/toolpresenters/iselecttoolpresenter.hpp"
const ToolId ISelectToolPresenter::SELECT_TOOL_ID = ToolId(
    "select-tool"
);

#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
const ToolId IBrushToolPresenterAux::ID = ToolId(
    "brush-tool"
);

#include "interfaces/presenters/toolpresenters/ierasertoolpresenter.hpp"
const ToolId IEraserToolPresenter::ERASER_TOOL_ID = ToolId(
    "eraser-tool"
);

#include "interfaces/presenters/toolpresenters/ifilltoolpresenter.hpp"
const ToolId IFillToolPresenter::FILL_TOOL_ID = ToolId(
    "fill-tool"
);

#include "interfaces/presenters/toolpresenters/itexttoolpresenter.hpp"
const ToolId ITextToolPresenter::TEXT_TOOL_ID = ToolId(
    "text-tool"
);

#include "interfaces/presenters/toolpresenters/ishapestoolpresenter.hpp"
const ToolId IShapesToolPresenter::SHAPES_TOOL_ID = ToolId(
    "shapes-tool"
);

#include "interfaces/presenters/toolpresenters/istickerstoolpresenter.hpp"
const ToolId IStickersToolPresenter::STICKERS_TOOL_ID = ToolId(
    "stickers-tool"
);

#include "interfaces/presenters/toolpresenters/ieyedroptoolpresenter.hpp"
const ToolId IEyedropToolPresenter::EYEDROP_TOOL_ID = ToolId(
    "eyedrop-tool"
);

#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/toolpresenters/moc_inavigatetoolpresenteraux.cpp"
const ToolId INavigateToolPresenterAux::ID = ToolId(
    "navigate-tool"
);

#include "interfaces/presenters/toolpresenters/imeasuretoolpresenter.hpp"
const ToolId IMeasureToolPresenter::MEASURE_TOOL_ID = ToolId(
    "measure-tool"
);

#include "interfaces/presenters/imaineditorpresenter.hpp"
const ToolId IMainEditorPresenterAux::DefaultTools::SELECT   = ISelectToolPresenter::SELECT_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::BRUSH    = IBrushToolPresenterAux::ID;
const ToolId IMainEditorPresenterAux::DefaultTools::ERASER   = IEraserToolPresenter::ERASER_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::FILL     = IFillToolPresenter::FILL_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::TEXT     = ITextToolPresenter::TEXT_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::SHAPES   = IShapesToolPresenter::SHAPES_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::STICKERS = IStickersToolPresenter::STICKERS_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::EYEDROP  = IEyedropToolPresenter::EYEDROP_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::NAVIGATE = INavigateToolPresenterAux::ID;
const ToolId IMainEditorPresenterAux::DefaultTools::MEASURE  = IMeasureToolPresenter::MEASURE_TOOL_ID;

#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
const BrushId IBrushToolPresenterAux::DefaultBrushes::Basic  = GlobalConstants::CoreBrushes::BasicBrush;
const BrushId IBrushToolPresenterAux::DefaultBrushes::Soft  = GlobalConstants::CoreBrushes::SoftBrush;

const BrushId IBrushToolPresenterAux::DEFAULT_BRUSH = IBrushToolPresenterAux::DefaultBrushes::Basic;

// Make sure to include all header-only ADDLE_COMMON_EXPORT classes, so that
// their symbols are defined on libcommon.dll

#include "exceptions/commandlineexceptions.hpp"
#include "exceptions/fileexceptions.hpp"
#include "exceptions/formatexceptions.hpp"
#include "exceptions/initializeexceptions.hpp"
#include "exceptions/servicelocatorexceptions.hpp"

#include "utilities/editing/brushinfo.hpp"
#include "utilities/configuration/baseserviceconfiguration.hpp"
#include "utilities/model/documentbuilder.hpp"
#include "utilities/model/importexportinfo.hpp"
#include "utilities/model/layerbuilder.hpp"
