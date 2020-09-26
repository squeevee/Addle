/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef INOTIFICATIONPRESENTER_HPP
#define INOTIFICATIONPRESENTER_HPP

#include <QString>
#include <QSharedPointer>

#include "interfaces/traits.hpp"

#include "imessagepresenter.hpp"

namespace Addle {

class AddleException;
class INotificationPresenter : public IMessagePresenter
{
public:
    virtual ~INotificationPresenter() = default;

    virtual void initialize(
        QString text,
        Tone tone,
        bool isUrgent = false,
        IMessageContext* context = nullptr,
        QSharedPointer<AddleException> exception = nullptr) = 0;

    virtual QSharedPointer<AddleException> exception() const = 0;
};

DECL_MAKEABLE(INotificationPresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::INotificationPresenter, "org.addle.INotificationPresenter");

#endif // INOTIFICATIONPRESENTER_HPP