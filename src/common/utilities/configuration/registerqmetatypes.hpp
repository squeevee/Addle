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

#include "interfaces/models/idocument.hpp"
#include "interfaces/presenters/iraiseerrorpresenter.hpp"

#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"

#include "interfaces/models/ipalette.hpp"

#include "utilities/indexvariant.hpp"
#include "idtypes/addleid.hpp"
#include "idtypes/brushid.hpp"
#include "idtypes/toolid.hpp"
#include "idtypes/formatid.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"
#include "utilities/model/brushbuilder.hpp"
namespace Addle {

inline void registerQMetaTypes()
{
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

    qRegisterMetaType<IPalette::ColorArray>("ColorArray");
}

} // namespace Addle
#endif // REGISTERQMETATYPES_HPP