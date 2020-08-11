#ifndef REGISTERQMETATYPES_HPP
#define REGISTERQMETATYPES_HPP

#include <QSharedPointer>
#include <QMetaType>

#include "interfaces/iaddleexception.hpp"

#include "interfaces/models/idocument.hpp"
#include "interfaces/presenters/iraiseerrorpresenter.hpp"

#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"

#include "utilities/indexvariant.hpp"
#include "idtypes/persistentid.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/toolid.hpp"
#include "idtypes/formatid.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"
#include "utilities/model/brushbuilder.hpp"
namespace Addle {

inline void registerQMetaTypes()
{
    //qRegisterMetaType<QSharedPointer<IAddleException>>();

    //qRegisterMetaType<QSharedPointer<IDocument>>();
    //qRegisterMetaType<QSharedPointer<IErrorPresenter>>();
    
    qRegisterMetaType<INavigateToolPresenter::NavigateOperationOptions>("NavigateOperationOptions");
    // qRegisterMetaType<IBrushPresenter::SizeOption>("SizeOption");
    //qRegisterMetaType<QSharedPointer<IAssetPresenter>>();

    qRegisterMetaType<PersistentId>();
    IndexVariant::registerHasher<PersistentId>();

    qRegisterMetaType<BrushId>();
    QMetaType::registerConverter<BrushId, PersistentId>();
    IndexVariant::registerHasher<BrushId, PersistentId>();

    qRegisterMetaType<ToolId>();
    QMetaType::registerConverter<ToolId, PersistentId>();
    IndexVariant::registerHasher<ToolId, PersistentId>();

    qRegisterMetaType<FormatId>();
    QMetaType::registerConverter<FormatId, PersistentId>();
    IndexVariant::registerHasher<FormatId, PersistentId>();

    CanvasMouseEvent::_type = QEvent::registerEventType(CanvasMouseEvent::_type);
}

} // namespace Addle
#endif // REGISTERQMETATYPES_HPP