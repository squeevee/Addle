#ifndef REGISTERQMETATYPES_HPP
#define REGISTERQMETATYPES_HPP

#include <QSharedPointer>
#include "common/interfaces/models/idocument.hpp"

#include "common/interfaces/presenters/iraiseerrorpresenter.hpp"

void registerQMetaTypes()
{
    qRegisterMetaType<QSharedPointer<IDocument>>();
    qRegisterMetaType<QSharedPointer<IErrorPresenter>>();
}

#endif // REGISTERQMETATYPES_HPP