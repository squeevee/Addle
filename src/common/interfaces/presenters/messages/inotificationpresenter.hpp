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
class NotificationPresenterBuilder;
class INotificationPresenter : public IMessagePresenter
{
public:
    virtual ~INotificationPresenter() = default;

    virtual void initialize(const NotificationPresenterBuilder& builder) = 0;

    virtual QSharedPointer<AddleException> exception() const = 0;
#ifdef ADDLE_DEBUG
    virtual QString debugText() const = 0;
#endif
};

ADDLE_DECL_MAKEABLE(INotificationPresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::INotificationPresenter, "org.addle.INotificationPresenter");

#endif // INOTIFICATIONPRESENTER_HPP
