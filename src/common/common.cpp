/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
#include "utilities/strings.hpp"
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

#include "utilities/configuration/serviceconfigurationbase.hpp"
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

const ToolId IMainEditorPresenterAux::DefaultTools::Select   = ISelectToolPresenter::SELECT_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Brush    = IBrushToolPresenterAux::BRUSH_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Eraser   = IBrushToolPresenterAux::ERASER_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Fill     = IFillToolPresenter::FILL_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Text     = ITextToolPresenter::TEXT_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Shapes   = IShapesToolPresenter::SHAPES_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Stickers = IStickersToolPresenter::STICKERS_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Eyedrop  = IEyedropToolPresenter::EYEDROP_TOOL_ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Navigate = INavigateToolPresenterAux::ID;
const ToolId IMainEditorPresenterAux::DefaultTools::Measure  = IMeasureToolPresenter::MEASURE_TOOL_ID;

const BrushId IBrushToolPresenterAux::DefaultBrushes::Basic  = CoreBrushes::BasicBrush;
const BrushId IBrushToolPresenterAux::DefaultBrushes::Soft   = CoreBrushes::SoftBrush;

const BrushId IBrushToolPresenterAux::DEFAULT_BRUSH = IBrushToolPresenterAux::DefaultBrushes::Basic;

const BrushId IBrushToolPresenterAux::DefaultErasers::Basic  = CoreBrushes::BasicEraser;

const BrushId IBrushToolPresenterAux::DEFAULT_ERASER = IBrushToolPresenterAux::DefaultErasers::Basic;

const ColorInfo GlobalColorInfo::Black = ColorInfo(
                //: Name of the color black (#000000)
    Qt::black,  QT_TRID_NOOP("global-color-names.black")
);

const ColorInfo GlobalColorInfo::White = ColorInfo(
                //: Name of the color white (#FFFFFF)
    Qt::white,  QT_TRID_NOOP("global-color-names.white")
);

const ColorInfo GlobalColorInfo::Transparent = ColorInfo(
                        //: Name for a fully transparent color value
    Qt::transparent,    QT_TRID_NOOP("global-color-names.transparent")
);

const QHash<QRgb, ColorInfo> GlobalColorInfo::ByRGB = {
    { QColor(Qt::black).rgba(), Black },
    { QColor(Qt::white).rgba(), White },
    { QColor(Qt::transparent).rgba(), Transparent }
};


#ifdef ADDLE_DEBUG

// Qt's i18n workflow isn't exactly optimized for "dynamic TrIds". Here we can
// list ones we anticipate needing so that `lupdate` can be aware of them, and we
// can assign metadata if desired.

// The debug implementation of `dynamic_qtTrId()` will also check against this
// set and warn about requests for ones not listed here -- so we can ensure
// thorough l10ns.

const QSet<QByteArray> _DYNAMIC_TRIDS_REGISTRY = {

    //% "px"
    QT_TRID_NOOP("units.pixels"),

    QT_TRID_NOOP("tools.select-tool.name"),
    QT_TRID_NOOP("tools.select-tool.description"),
    QT_TRID_NOOP("tools.brush-tool.name"),
    QT_TRID_NOOP("tools.brush-tool.description"),
    QT_TRID_NOOP("tools.eraser-tool.name"),
    QT_TRID_NOOP("tools.eraser-tool.description"),
    QT_TRID_NOOP("tools.fill-tool.name"),
    QT_TRID_NOOP("tools.fill-tool.description"),
    QT_TRID_NOOP("tools.text-tool.name"),
    QT_TRID_NOOP("tools.text-tool.description"),
    QT_TRID_NOOP("tools.shapes-tool.name"),
    QT_TRID_NOOP("tools.shapes-tool.description"),
    QT_TRID_NOOP("tools.stickers-tool.name"),
    QT_TRID_NOOP("tools.stickers-tool.description"),
    QT_TRID_NOOP("tools.navigate-tool.name"),
    QT_TRID_NOOP("tools.navigate-tool.description"),
    QT_TRID_NOOP("tools.eyedrop-tool.name"),
    QT_TRID_NOOP("tools.eyedrop-tool.description"),
    QT_TRID_NOOP("tools.measure-tool.name"),
    QT_TRID_NOOP("tools.measure-tool.description"),

    QT_TRID_NOOP("brushes.basic-brush.name"),
    QT_TRID_NOOP("brushes.soft-brush.name"),
    
    QT_TRID_NOOP("brushes.basic-eraser.name")
};

#endif // ADDLE_DEBUG

} // namespace Addle

#include "moc_globals.cpp"
#include "interfaces/editing/moc_rasterengineparams.cpp"
#include "interfaces/presenters/tools/moc_inavigatetoolpresenteraux.cpp"