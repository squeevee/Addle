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

#include "globalconstants.hpp"

const QColor GlobalConstants::DEFAULT_BACKGROUND_COLOR = Qt::white;

const BrushId GlobalConstants::CoreBrushes::BasicBrush = BrushId("basic-brush");

#include "interfaces/models/idocument.hpp"
const FormatId GlobalConstants::CoreFormats::PNG = FormatId(
    "png-format-id",
    QStringLiteral("image/png"),
    typeid(IDocument)
);
const FormatId GlobalConstants::CoreFormats::JPEG = FormatId(
    "jpeg-format-id",
	QStringLiteral("image/jpeg"),
    typeid(IDocument)
);

const ToolId GlobalConstants::CoreTools::SELECT = ToolId("select-tool");
const ToolId GlobalConstants::CoreTools::BRUSH = ToolId("brush-tool");
const ToolId GlobalConstants::CoreTools::ERASER = ToolId("eraser-tool");
const ToolId GlobalConstants::CoreTools::FILL = ToolId("fill-tool");
const ToolId GlobalConstants::CoreTools::TEXT = ToolId("text-tool");
const ToolId GlobalConstants::CoreTools::SHAPES = ToolId("shapes-tool");
const ToolId GlobalConstants::CoreTools::STICKERS = ToolId("stickers-tool");
const ToolId GlobalConstants::CoreTools::EYEDROP = ToolId("eyedrop-tool");
const ToolId GlobalConstants::CoreTools::NAVIGATE = ToolId("navigate-tool");
const ToolId GlobalConstants::CoreTools::MEASURE = ToolId("measure-tool");

#include "interfaces/presenters/toolpresenters/iselecttoolpresenter.hpp"
const ToolId ISelectToolPresenter::ID = GlobalConstants::CoreTools::SELECT;

#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
const ToolId IBrushToolPresenter::ID = GlobalConstants::CoreTools::BRUSH;

#include "interfaces/presenters/toolpresenters/ierasertoolpresenter.hpp"
const ToolId IEraserToolPresenter::ID = GlobalConstants::CoreTools::ERASER;

#include "interfaces/presenters/toolpresenters/ifilltoolpresenter.hpp"
const ToolId IFillToolPresenter::ID = GlobalConstants::CoreTools::FILL;

#include "interfaces/presenters/toolpresenters/itexttoolpresenter.hpp"
const ToolId ITextToolPresenter::ID = GlobalConstants::CoreTools::TEXT;

#include "interfaces/presenters/toolpresenters/ishapestoolpresenter.hpp"
const ToolId IShapesToolPresenter::ID = GlobalConstants::CoreTools::SHAPES;

#include "interfaces/presenters/toolpresenters/istickerstoolpresenter.hpp"
const ToolId IStickersToolPresenter::ID = GlobalConstants::CoreTools::STICKERS;

#include "interfaces/presenters/toolpresenters/ieyedroptoolpresenter.hpp"
const ToolId IEyedropToolPresenter::ID = GlobalConstants::CoreTools::EYEDROP;

#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"
const ToolId INavigateToolPresenter::ID = GlobalConstants::CoreTools::NAVIGATE;

#include "interfaces/presenters/toolpresenters/imeasuretoolpresenter.hpp"
const ToolId IMeasureToolPresenter::ID = GlobalConstants::CoreTools::MEASURE;


#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/toolpresenters/moc_inavigatetoolpresenter.cpp"

#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/assets/moc_ibrushpresenter.cpp"

#include "utilities/configuration/baseserviceconfiguration.hpp"

#include "interfaces/views/imainview.hpp"
#include "interfaces/views/imaineditorview.hpp"

#include "utilities/debugging.hpp"

#include "interfaces/editing/ibrushpainter.hpp"
#include "interfaces/editing/irasterdiff.hpp"
#include "interfaces/editing/irastersurface.hpp"
#include "interfaces/format/iformatdriver.hpp"
#include "interfaces/format/iformatmodel.hpp"
#include "interfaces/format/drivers/ipngformatdriver.hpp"
#include "interfaces/format/drivers/ijpegformatdriver.hpp"
#include "interfaces/iaddleexception.hpp"
#include "interfaces/models/ibrushmodel.hpp"
#include "interfaces/models/icolorpalette.hpp"
#include "interfaces/models/idocument.hpp"
#include "interfaces/models/ilayer.hpp"
#include "interfaces/presenters/assets/iassetpresenter.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/assets/ishapepresenter.hpp"
#include "interfaces/presenters/assets/istickerpresenter.hpp"
#include "interfaces/presenters/operationpresenters/iundooperationpresenter.hpp"
#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/presenters/ierrorpresenter.hpp"
#include "interfaces/presenters/ihavedocumentpresenter.hpp"
#include "interfaces/presenters/ihavetoolspresenter.hpp"
#include "interfaces/presenters/ihaveundostackpresenter.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/ipropertydecoratedpresenter.hpp"
#include "interfaces/presenters/iraiseerrorpresenter.hpp"
#include "interfaces/presenters/iscrollstate.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/rendering/irenderstack.hpp"
#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/servicelocator/ifactory.hpp"
#include "interfaces/servicelocator/iservicelocator.hpp"
#include "interfaces/services/iapplicationsservice.hpp"
#include "interfaces/services/iformatservice.hpp"
#include "interfaces/services/ipersistentobjectrepository.hpp"
#include "interfaces/services/iservice.hpp"
#include "interfaces/views/imaineditorview.hpp"
#include "interfaces/views/imainview.hpp"