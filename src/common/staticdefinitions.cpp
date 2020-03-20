// Definitions of Qt string and collection constants

#include <QString>
#include <QByteArray>

#include "servicelocator.hpp"
ServiceLocator* ServiceLocator::_instance = nullptr;

#include "idtypes/brushid.hpp"
STATIC_PERSISTENT_ID_BOILERPLATE(BrushId)

#include "idtypes/formatid.hpp"
STATIC_PERSISTENT_ID_BOILERPLATE(FormatId)

#include "idtypes/toolid.hpp"
STATIC_PERSISTENT_ID_BOILERPLATE(ToolId)

#include "utilities/canvas/canvasmouseevent.hpp"
int CanvasMouseEvent::_type = QEvent::User;

#include "interfaces/services/iservice.hpp"
const QString IService::SERVICE_THREAD_NAME_TEMPLATE = QStringLiteral(ADDLE_STRING__ISERVICE__SERVICE_THREAD_NAME_TEMPLATE);

#include "interfaces/services/iapplicationsservice.hpp"
const QString IApplicationService::CMD_EDITOR_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_OPTION);
const QString IApplicationService::CMD_EDITOR_SHORT_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_SHORT_OPTION);
const QString IApplicationService::CMD_BROWSER_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_OPTION);
const QString IApplicationService::CMD_BROWSER_SHORT_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_SHORT_OPTION);

#include "interfaces/models/idocument.hpp"
const QColor IDocument::DEFAULT_BACKGROUND_COLOR = Qt::white;

#include "interfaces/format/drivers/ipngformatdriver.hpp"
const QString IPNGFormatDriver::PNG_FILE_EXTENSION = QStringLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_EXTENSION);
const QString IPNGFormatDriver::PNG_MIME_TYPE = QStringLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_MIME_TYPE);
const QByteArray IPNGFormatDriver::PNG_FILE_SIGNATURE = QByteArrayLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_SIGNATURE);
const FormatId IPNGFormatDriver::PNG_FORMAT_ID = FormatId(
    "png-format-id",
    IPNGFormatDriver::PNG_MIME_TYPE,
    typeid(IDocument)
);

#include "interfaces/format/drivers/ijpegformatdriver.hpp"
const QString IJPEGFormatDriver::JPEG_FILE_EXTENSION = QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_EXTENSION);
const QStringList IJPEGFormatDriver::JPEG_FILE_EXTENSIONS = {
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_EXTENSION),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_1),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_2),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_3),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_4)
};
const QString IJPEGFormatDriver::JPEG_MIME_TYPE = QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_MIME_TYPE);
const QByteArray IJPEGFormatDriver::JPEG_FILE_SIGNATURE = QByteArrayLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_SIGNATURE);
const FormatId IJPEGFormatDriver::JPEG_FORMAT_ID = FormatId(
    "jpeg-format-id",
    IJPEGFormatDriver::JPEG_MIME_TYPE,
    typeid(IDocument)
);

#include "interfaces/presenters/toolpresenters/iselecttoolpresenter.hpp"
const ToolId ISelectToolPresenter::SELECT_TOOL_ID = ToolId(
    "select-tool"
);

#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
const ToolId IBrushToolPresenter::BRUSH_TOOL_ID = ToolId(
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
#include "interfaces/presenters/toolpresenters/moc_inavigatetoolpresenter.cpp"
const ToolId INavigateToolPresenter::NAVIGATE_TOOL_ID = ToolId(
    "navigate-tool"
);

#include "interfaces/presenters/toolpresenters/imeasuretoolpresenter.hpp"
const ToolId IMeasureToolPresenter::MEASURE_TOOL_ID = ToolId(
    "measure-tool"
);

#include "interfaces/presenters/imaineditorpresenter.hpp"
const ToolId IMainEditorPresenter::DefaultTools::SELECT   = ISelectToolPresenter::SELECT_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::BRUSH    = IBrushToolPresenter::BRUSH_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::ERASER   = IEraserToolPresenter::ERASER_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::FILL     = IFillToolPresenter::FILL_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::TEXT     = ITextToolPresenter::TEXT_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::SHAPES   = IShapesToolPresenter::SHAPES_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::STICKERS = IStickersToolPresenter::STICKERS_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::EYEDROP  = IEyedropToolPresenter::EYEDROP_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::NAVIGATE = INavigateToolPresenter::NAVIGATE_TOOL_ID;
const ToolId IMainEditorPresenter::DefaultTools::MEASURE  = IMeasureToolPresenter::MEASURE_TOOL_ID;

#include "interfaces/editing/brushpainters/ibasicbrushpainter.hpp"

const BrushId IBasicBrushPainter::Id = BrushId("basic-brush");

#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
const BrushId IBrushToolPresenter::DefaultBrushes::Basic  = IBasicBrushPainter::Id;

const BrushId IBrushToolPresenter::DEFAULT_BRUSH = DefaultBrushes::Basic;

#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/assets/moc_ibrushpresenter.cpp"