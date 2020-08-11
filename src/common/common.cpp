// Definitions of Qt string and collection constants

#include <QString>
#include <QByteArray>

#include "servicelocator.hpp"
#include "globals.hpp"

#include "interfaces/editing/rasterengineparams.hpp"

#include "idtypes/persistentid.hpp"
#include "idtypes/brushengineid.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/paletteid.hpp"
#include "idtypes/formatid.hpp"
#include "idtypes/toolid.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"
#include "interfaces/models/idocument.hpp"

#include "interfaces/presenters/tools/iselecttoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/tools/ifilltoolpresenter.hpp"
#include "interfaces/presenters/tools/itexttoolpresenter.hpp"
#include "interfaces/presenters/tools/ishapestoolpresenter.hpp"
#include "interfaces/presenters/tools/istickerstoolpresenter.hpp"
#include "interfaces/presenters/tools/ieyedroptoolpresenter.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/tools/imeasuretoolpresenter.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "exceptions/commandlineexceptions.hpp"
#include "exceptions/fileexceptions.hpp"
#include "exceptions/formatexceptions.hpp"
#include "exceptions/initializeexceptions.hpp"
#include "exceptions/servicelocatorexceptions.hpp"

#include "utilities/configuration/baseserviceconfiguration.hpp"
#include "utilities/model/documentbuilder.hpp"
#include "utilities/model/importexportinfo.hpp"
#include "utilities/model/layerbuilder.hpp"

namespace Addle {

ServiceLocator* ServiceLocator::_instance = nullptr;

STATIC_PERSISTENT_ID_BOILERPLATE(BrushEngineId)

STATIC_PERSISTENT_ID_BOILERPLATE(BrushId)

STATIC_PERSISTENT_ID_BOILERPLATE(PaletteId)

STATIC_PERSISTENT_ID_BOILERPLATE(FormatId)

STATIC_PERSISTENT_ID_BOILERPLATE(ToolId)

const BrushEngineId CoreBrushEngines::PathEngine = BrushEngineId(
    "path-engine"
);

const BrushEngineId CoreBrushEngines::RasterEngine = BrushEngineId(
    "raster-engine"
);

const BrushId CoreBrushes::BasicBrush = BrushId(
    "basic-brush"
);

const BrushId CoreBrushes::SoftBrush = BrushId(
    "soft-brush"
);

const BrushId CoreBrushes::BasicEraser = BrushId(
    "basic-eraser"
);

const PaletteId CorePalettes::BasicPalette = PaletteId(
    "basic-palette"
);

int CanvasMouseEvent::_type = QEvent::User;

const QColor DefaultBackgroundColor = Qt::white;


const FormatId CoreFormats::PNG = FormatId(
    "png-format-id",
    QStringLiteral("image/png"),
    typeid(IDocument),
    QStringLiteral("png"),
    QByteArrayLiteral("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A")
);

const FormatId CoreFormats::JPEG = FormatId(
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
const ToolId ISelectToolPresenter::SELECT_TOOL_ID = ToolId(
    "select-tool"
);

const ToolId IBrushToolPresenterAux::BRUSH_ID = ToolId(
    "brush-tool"
);

const ToolId IBrushToolPresenterAux::ERASER_ID = ToolId(
    "eraser-tool"
);

const ToolId IFillToolPresenter::FILL_TOOL_ID = ToolId(
    "fill-tool"
);

const ToolId ITextToolPresenter::TEXT_TOOL_ID = ToolId(
    "text-tool"
);

const ToolId IShapesToolPresenter::SHAPES_TOOL_ID = ToolId(
    "shapes-tool"
);

const ToolId IStickersToolPresenter::STICKERS_TOOL_ID = ToolId(
    "stickers-tool"
);

const ToolId IEyedropToolPresenter::EYEDROP_TOOL_ID = ToolId(
    "eyedrop-tool"
);

const ToolId INavigateToolPresenterAux::ID = ToolId(
    "navigate-tool"
);

const ToolId IMeasureToolPresenter::MEASURE_TOOL_ID = ToolId(
    "measure-tool"
);

const ToolId IMainEditorPresenterAux::DefaultTools::SELECT   = ISelectToolPresenter::SELECT_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::BRUSH    = IBrushToolPresenterAux::BRUSH_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::ERASER   = IBrushToolPresenterAux::ERASER_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::FILL     = IFillToolPresenter::FILL_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::TEXT     = ITextToolPresenter::TEXT_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::SHAPES   = IShapesToolPresenter::SHAPES_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::STICKERS = IStickersToolPresenter::STICKERS_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::EYEDROP  = IEyedropToolPresenter::EYEDROP_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::NAVIGATE = INavigateToolPresenterAux::ID;
const ToolId IMainEditorPresenterAux::DefaultTools::MEASURE  = IMeasureToolPresenter::MEASURE_TOOL_ID;

const BrushId IBrushToolPresenterAux::DefaultBrushes::Basic  = CoreBrushes::BasicBrush;
const BrushId IBrushToolPresenterAux::DefaultBrushes::Soft   = CoreBrushes::SoftBrush;

const BrushId IBrushToolPresenterAux::DEFAULT_BRUSH = IBrushToolPresenterAux::DefaultBrushes::Basic;

const BrushId IBrushToolPresenterAux::DefaultErasers::Basic  = CoreBrushes::BasicEraser;

const BrushId IBrushToolPresenterAux::DEFAULT_ERASER = IBrushToolPresenterAux::DefaultErasers::Basic;

const ColorInfo Transparent = ColorInfo(Qt::transparent, "Transparent"); //i18n?

} // namespace Addle

// Make sure to include all header-only ADDLE_COMMON_EXPORT classes, so that
// their symbols are defined on libcommon.dll


#include "interfaces/editing/moc_rasterengineparams.cpp"
#include "interfaces/presenters/tools/moc_inavigatetoolpresenteraux.cpp"