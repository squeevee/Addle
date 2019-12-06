#ifndef REGISTERQMETATYPES_HPP
#define REGISTERQMETATYPES_HPP

#include <QSharedPointer>
#include "interfaces/models/idocument.hpp"
#include "interfaces/presenters/iraiseerrorpresenter.hpp"
#include "data/toolid.hpp"

#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushliketoolpresenter.hpp"

void registerQMetaTypes()
{
    qRegisterMetaType<QSharedPointer<IDocument>>();
    qRegisterMetaType<QSharedPointer<IErrorPresenter>>();
    qRegisterMetaType<ToolId>();
    
    qRegisterMetaType<INavigateToolPresenter::NavigateOperationOptions>("NavigateOperationOptions");
    qRegisterMetaType<IBrushLikeToolPresenter::SizeOption>("SizeOption");
    qRegisterMetaType<BrushTipId>();
}

#endif // REGISTERQMETATYPES_HPP