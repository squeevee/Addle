// Definitions of Qt string and collection constants

// These constants are compilation static, and therefore should not contain any
// strings that need to be translated.

#include <QString>
#include <QByteArray>

#include "interfaces/services/iservice.hpp"
const QString IService::SERVICE_THREAD_NAME_TEMPLATE = QStringLiteral(ADDLE_STRING__ISERVICE__SERVICE_THREAD_NAME_TEMPLATE);

#include "interfaces/services/iapplicationsservice.hpp"
const QString IApplicationService::CMD_EDITOR_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_OPTION);
const QString IApplicationService::CMD_EDITOR_SHORT_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_SHORT_OPTION);
const QString IApplicationService::CMD_BROWSER_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_OPTION);
const QString IApplicationService::CMD_BROWSER_SHORT_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_SHORT_OPTION);

#include "interfaces/models/idocument.hpp"
#include "interfaces/format/drivers/ipngformatdriver.hpp"
const QString IPNGFormatDriver::PNG_FILE_EXTENSION = QStringLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_EXTENSION);
const QString IPNGFormatDriver::PNG_MIME_TYPE = QStringLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_MIME_TYPE);
const QByteArray IPNGFormatDriver::PNG_FILE_SIGNATURE = QByteArrayLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_SIGNATURE);
const FormatId IPNGFormatDriver::PNG_FORMAT_ID = FormatId(
    "jpeg-format-id",
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

#include "interfaces/presenters/tools/iselecttoolpresenter.hpp"
const ToolId ISelectToolPresenter::SELECT_TOOL_ID = ToolId(
    "Select Tool" //""
);

#include "interfaces/presenters/tools/moc_ibrushliketoolpresenter.cpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
const ToolId IBrushToolPresenter::BRUSH_TOOL_ID = ToolId(
    "Brush Tool" //""
);

#include "interfaces/presenters/tools/ierasertoolpresenter.hpp"
const ToolId IEraserToolPresenter::ERASER_TOOL_ID = ToolId(
    "Eraser Tool" //""
);

#include "interfaces/presenters/tools/ifilltoolpresenter.hpp"
const ToolId IFillToolPresenter::FILL_TOOL_ID = ToolId(
    "Fill Tool" //""
);

#include "interfaces/presenters/tools/itexttoolpresenter.hpp"
const ToolId ITextToolPresenter::TEXT_TOOL_ID = ToolId(
    "Text Tool" //""
);

#include "interfaces/presenters/tools/ishapestoolpresenter.hpp"
const ToolId IShapesToolPresenter::SHAPES_TOOL_ID = ToolId(
    "Shapes Tool" //""
);

#include "interfaces/presenters/tools/istickerstoolpresenter.hpp"
const ToolId IStickersToolPresenter::STICKERS_TOOL_ID = ToolId(
    "Stickers Tool" //""
);

#include "interfaces/presenters/tools/ieyedroptoolpresenter.hpp"
const ToolId IEyedropToolPresenter::EYEDROP_TOOL_ID = ToolId(
    "Eyedrop Tool" //""
);

#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/tools/moc_inavigatetoolpresenter.cpp"
const ToolId INavigateToolPresenter::NAVIGATE_TOOL_ID = ToolId(
    "Navigate Tool", //""
    "Pan, rotate, and zoom the canvas" //""
);

#include "interfaces/presenters/tools/imeasuretoolpresenter.hpp"
const ToolId IMeasureToolPresenter::MEASURE_TOOL_ID = ToolId(
    "Measure Tool" //""
);

#include "interfaces/presenters/ieditorpresenter.hpp"
const ToolId IEditorPresenter::DefaultTools::SELECT   = ISelectToolPresenter::SELECT_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::BRUSH    = IBrushToolPresenter::BRUSH_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::ERASER   = IEraserToolPresenter::ERASER_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::FILL     = IFillToolPresenter::FILL_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::TEXT     = ITextToolPresenter::TEXT_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::SHAPES   = IShapesToolPresenter::SHAPES_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::STICKERS = IStickersToolPresenter::STICKERS_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::EYEDROP  = IEyedropToolPresenter::EYEDROP_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::NAVIGATE = INavigateToolPresenter::NAVIGATE_TOOL_ID;
const ToolId IEditorPresenter::DefaultTools::MEASURE  = IMeasureToolPresenter::MEASURE_TOOL_ID;


#include "utilities/presethelpers/zoompresethelper.hpp"
const ZoomPresetHelper ZoomPresetHelper::_instance = ZoomPresetHelper();

#include "utilities/presethelpers/rotatepresethelper.hpp"
const RotatePresetHelper RotatePresetHelper::_instance = RotatePresetHelper();

#include "utilities/presethelpers/brushsizepresethelper.hpp"
const BrushSizePresetHelper::PxPresetHelper BrushSizePresetHelper::_instance_px = PxPresetHelper();
const BrushSizePresetHelper::PercentPresetHelper BrushSizePresetHelper::_instance_percent = PercentPresetHelper();

#include "interfaces/presenters/tools/ibrushliketoolpresenter.hpp"
const BrushTipId IBrushLikeToolPresenter::CommonBrushTips::Pixel      = BrushTipId("Pixel");
const BrushTipId IBrushLikeToolPresenter::CommonBrushTips::TwoSquare  = BrushTipId("TwoSquare");
const BrushTipId IBrushLikeToolPresenter::CommonBrushTips::Square     = BrushTipId("Square");
const BrushTipId IBrushLikeToolPresenter::CommonBrushTips::HardCircle = BrushTipId("HardCircle");
const BrushTipId IBrushLikeToolPresenter::CommonBrushTips::SoftCircle = BrushTipId("SoftCircle");

#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
const BrushTipId IBrushToolPresenter::DEFAULT_BRUSH_TIP = IBrushLikeToolPresenter::CommonBrushTips::TwoSquare;
