/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef REGISTERQMETATYPES_HPP
#define REGISTERQMETATYPES_HPP

#include <QSharedPointer>
#include <QMetaType>

#include "exceptions/addleexception.hpp"
#include "exceptions/unhandledexception.hpp"

#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"

#include "interfaces/models/ipalette.hpp"

#include "utilities/indexvariant.hpp"
#include "idtypes/addleid.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/toolid.hpp"
#include "idtypes/formatid.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"
//#include "utilities/presenter/saveevent.hpp"
#include "utilities/model/brushbuilder.hpp"
namespace Addle {

// TODO: move this into a library constructor
inline void registerQMetaTypes()
{
    qRegisterMetaType<QSharedPointer<AddleException>>("QSharedPointer<AddleException>");
    qRegisterMetaType<QSharedPointer<UnhandledException>>("QSharedPointer<UnhandledException>");

    qRegisterMetaType<INavigateToolPresenter::NavigateOperationOptions>("NavigateOperationOptions");

    qRegisterMetaType<AddleId>();
    IndexVariant::registerHasher<AddleId>();

    qRegisterMetaType<BrushId>();
    QMetaType::registerConverter<BrushId, AddleId>();
    IndexVariant::registerHasher<BrushId, AddleId>();

    qRegisterMetaType<ToolId>();
    QMetaType::registerConverter<ToolId, AddleId>();
    IndexVariant::registerHasher<ToolId, AddleId>();

    qRegisterMetaType<DocumentFormatId>();
    QMetaType::registerConverter<DocumentFormatId, AddleId>();
    IndexVariant::registerHasher<DocumentFormatId, AddleId>();

    CanvasMouseEvent::_type = QEvent::registerEventType(CanvasMouseEvent::_type);
    //SaveEvent::_type = QEvent::registerEventType(SaveEvent::_type);

    qRegisterMetaType<IPalette::ColorArray>("ColorArray");
}

} // namespace Addle
#endif // REGISTERQMETATYPES_HPP