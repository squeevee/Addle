#ifndef REGISTERQMETATYPES_HPP
#define REGISTERQMETATYPES_HPP

#include <QSharedPointer>
#include "interfaces/models/idocument.hpp"
#include "interfaces/presenters/iraiseerrorpresenter.hpp"

#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushliketoolpresenter.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/tools/itoolwithassetspresenter.hpp"

#include "data/persistentid.hpp"
#include "data/brushid.hpp"
#include "data/toolid.hpp"
#include "data/formatid.hpp"

void registerQMetaTypes()
{
    qRegisterMetaType<QSharedPointer<IDocument>>();
    qRegisterMetaType<QSharedPointer<IErrorPresenter>>();
    
    qRegisterMetaType<INavigateToolPresenter::NavigateOperationOptions>("NavigateOperationOptions");
    qRegisterMetaType<IBrushPresenter::SizeOption>("SizeOption");
    qRegisterMetaType<QSharedPointer<IAssetPresenter>>();

    qRegisterMetaType<PersistentId>();
    qRegisterMetaType<BrushId>();
    qRegisterMetaType<ToolId>();
    qRegisterMetaType<FormatId>();
}

#endif // REGISTERQMETATYPES_HPP