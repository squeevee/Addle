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
#include "exceptions/factoryexception.hpp"
#include "exceptions/fileexception.hpp"
// #include "exceptions/formatexception.hpp"
#include "exceptions/initializeexceptions.hpp"
#include "exceptions/servicelocatorexceptions.hpp"

#include "utilities/editing/rasterengineparams.hpp"
#include "utilities/model/documentbuilder.hpp"
// #include "utilities/format/importexportinfo.hpp"
#include "utilities/model/layernodebuilder.hpp"
#include "utilities/genericeventfilter.hpp"

#include "utilities/presenter/filerequest.hpp"
#include "utilities/config3/repository.hpp"

namespace Addle {

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
// #include "exceptions/moc_formatexception.cpp"
#include "exceptions/moc_fileexception.cpp"
#include "utilities/moc_genericeventfilter.cpp"
#include "utilities/datatree/moc_aux.cpp"
#include "utilities/presenter/moc_filerequest.cpp"
#include "utilities/editing/moc_rasterengineparams.cpp"
#include "utilities/config3/moc_genericrepository.cpp"
#include "interfaces/presenters/tools/moc_inavigatetoolpresenteraux.cpp"
