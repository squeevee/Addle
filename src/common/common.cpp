/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <boost/mpl/for_each.hpp>
#include <QString>
#include <QByteArray>

#include "servicelocator.hpp"
#include "globals.hpp"

#include "idtypes/addleid.hpp"
#include "idtypes/brushengineid.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/moduleid.hpp"
#include "idtypes/paletteid.hpp"
#include "idtypes/formatid.hpp"
#include "idtypes/toolid.hpp"
#include "utilities/strings.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"
#include "utilities/idinfo.hpp"
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
#include "exceptions/factoryexception.hpp"
#include "exceptions/fileexception.hpp"
#include "exceptions/formatexception.hpp"
#include "exceptions/initializeexceptions.hpp"
#include "exceptions/servicelocatorexceptions.hpp"

#include "utilities/editing/rasterengineparams.hpp"
#include "utilities/model/documentbuilder.hpp"
#include "utilities/format/importexportinfo.hpp"
#include "utilities/model/layernodebuilder.hpp"
#include "utilities/genericeventfilter.hpp"

#include "utilities/presenter/filerequest.hpp"

namespace Addle {

///@cond FALSE
    
#define DEFINE_STATIC_ID_METADATA_CUSTOM(x, DataType, ...) \
const QSharedPointer<const AddleId::BaseMetaData> GET_STATIC_ID_METADATA(x)::_metaData \
    = QSharedPointer<const AddleId::BaseMetaData>(new DataType( \
    GET_STATIC_ID_METADATA(x)::key, \
    ::qMetaTypeId<std::remove_const<decltype(x)>::type>(), \
    __VA_ARGS__ \
));
#define DEFINE_STATIC_ID_METADATA(x) \
const QSharedPointer<const AddleId::BaseMetaData> GET_STATIC_ID_METADATA(x)::_metaData \
    = QSharedPointer<const AddleId::BaseMetaData>(new AddleId::BaseMetaData( \
    GET_STATIC_ID_METADATA(x)::key, \
    ::qMetaTypeId<std::remove_const<decltype(x)>::type>(), \
    QUuid() \
));

// Static ID metadata definitions

DEFINE_STATIC_ID_METADATA(Modules::Core)
DEFINE_STATIC_ID_METADATA(Modules::WidgetsGui)

DEFINE_STATIC_ID_METADATA(CoreBrushEngines::PathEngine)
DEFINE_STATIC_ID_METADATA(CoreBrushEngines::RasterEngine)

DEFINE_STATIC_ID_METADATA(CoreBrushes::BasicBrush)
DEFINE_STATIC_ID_METADATA(CoreBrushes::SoftBrush)
DEFINE_STATIC_ID_METADATA(CoreBrushes::BasicEraser)

DEFINE_STATIC_ID_METADATA_CUSTOM(CoreFormats::PNG, DocumentFormatId::MetaData,
                    QUuid(),
    /*mime type:*/  QStringLiteral("image/png"),
    /*file ext:*/   QStringLiteral("png"),          
    /*file sig:*/   QByteArrayLiteral("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A")
)

DEFINE_STATIC_ID_METADATA_CUSTOM(CoreFormats::JPEG, DocumentFormatId::MetaData,
                    QUuid(),
    /*mime type:*/  QStringLiteral("image/jpeg"),
    /*file exts:*/  {
                        QStringLiteral("jpg"),
                        QStringLiteral("jpeg"),
                        QStringLiteral("jpe"),
                        QStringLiteral("jfif"),
                        QStringLiteral("jif")
                    },          
    /*file sig:*/   { QByteArrayLiteral("\xFF\xD8\xFF") }
)

DEFINE_STATIC_ID_METADATA_CUSTOM(CoreFormats::GIF, DocumentFormatId::MetaData,
                    QUuid(),
    /*mime type:*/  QStringLiteral("image/gif"),
    /*file ext:*/   { QStringLiteral("gif") },
    /*file sig:*/   QByteArrayList({ 
                        QByteArrayLiteral("GIF87a"),
                        QByteArrayLiteral("GIF89a")
                    })
)

DEFINE_STATIC_ID_METADATA_CUSTOM(CoreFormats::WebP, DocumentFormatId::MetaData,
                    QUuid(),
    /*mime type:*/  QStringLiteral("image/webp"),
    /*file ext:*/   QStringLiteral("webp"),
    /*file sig:*/   QByteArrayLiteral("WEBP")
)

DEFINE_STATIC_ID_METADATA_CUSTOM(CoreFormats::ORA, DocumentFormatId::MetaData,
                    QUuid(),
    /*mime type:*/  QStringLiteral("image/openraster"),
    /*file ext:*/   QStringLiteral("ora")
)

DEFINE_STATIC_ID_METADATA(CorePalettes::BasicPalette)

DEFINE_STATIC_ID_METADATA(CoreTools::Select)
DEFINE_STATIC_ID_METADATA(CoreTools::Brush)
DEFINE_STATIC_ID_METADATA(CoreTools::Eraser)
DEFINE_STATIC_ID_METADATA(CoreTools::Fill)
DEFINE_STATIC_ID_METADATA(CoreTools::Text)
DEFINE_STATIC_ID_METADATA(CoreTools::Shapes)
DEFINE_STATIC_ID_METADATA(CoreTools::Stickers)
DEFINE_STATIC_ID_METADATA(CoreTools::Eyedrop)
DEFINE_STATIC_ID_METADATA(CoreTools::Navigate)
DEFINE_STATIC_ID_METADATA(CoreTools::Measure)

#undef DEFINE_STATIC_METADATA_CUSTOM
#undef DEFINE_STATIC_METADATA

///@endcond

AddleId::metaData_t AddleId::_dynamicMetaData = AddleId::MetaDataBuilder()
    .reserve(200)
    .initFor<CoreBrushEngines::all_brush_engines>()
    .initFor<CoreBrushes::all_brushes>()
    .initFor<CoreBrushes::all_erasers>()
    .initFor<CoreFormats::all_formats>()
    .initFor<CorePalettes::all_palettes>()
    .initFor<CoreTools::all_tools>();

QHash<int, QSet<AddleId>> IdInfo::_idsByType = QHash<int, QSet<AddleId>>();

int CanvasMouseEvent::_type = QEvent::User;

const QColor DefaultBackgroundColor = Qt::white;

const ColorInfo GlobalColorInfo::Black = ColorInfo(
                        //: Name of the color black (#000000)
    Qt::black,          QT_TRID_NOOP("global-color-names.black")
);

const ColorInfo GlobalColorInfo::White = ColorInfo(
                        //: Name of the color white (#FFFFFF)
    Qt::white,          QT_TRID_NOOP("global-color-names.white")
);

const ColorInfo GlobalColorInfo::Transparent = ColorInfo(
                        //: Name for a fully transparent color value
    Qt::transparent,    QT_TRID_NOOP("global-color-names.transparent")
);

const QHash<QRgb, ColorInfo> GlobalColorInfo::ByRGB = {
    { QColor(Qt::black).rgba(),         GlobalColorInfo::Black },
    { QColor(Qt::white).rgba(),         GlobalColorInfo::White },
    { QColor(Qt::transparent).rgba(),   GlobalColorInfo::Transparent }
};


#ifdef ADDLE_DEBUG

// Qt's i18n workflow isn't exactly optimized for "dynamic TrIds". Here we can
// list ones we anticipate needing so that `lupdate` can be aware of them, and we
// can assign metadata if desired.

// The debug implementation of `dynamic_qtTrId()` will also check against this
// set and warn about requests for ones not listed here.

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
    
    QT_TRID_NOOP("brushes.basic-eraser.name"),

    //% "PNG images"
    QT_TRID_NOOP("formats.format-png.name"),
    //% "JPEG images"
    QT_TRID_NOOP("formats.format-jpeg.name"),
    //% "GIF images and animations"
    QT_TRID_NOOP("formats.format-gif.name"),
    //% "WebP images"
    QT_TRID_NOOP("formats.format-webp.name"),
    //% "OpenRaster images"
    QT_TRID_NOOP("formats.format-ora.name")
};

#endif // ADDLE_DEBUG

} // namespace Addle

#include "moc_globals.cpp"
#include "exceptions/moc_factoryexception.cpp"
#include "exceptions/moc_formatexception.cpp"
#include "exceptions/moc_fileexception.cpp"
#include "utilities/moc_genericeventfilter.cpp"
#include "utilities/datatree/moc_aux.cpp"
#include "utilities/presenter/moc_filerequest.cpp"
#include "utilities/editing/moc_rasterengineparams.cpp"
#include "interfaces/presenters/tools/moc_inavigatetoolpresenteraux.cpp"
