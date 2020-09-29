/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ERRORPRESENTER_HPP
#define ERRORPRESENTER_HPP

#include <QObject>

#include "compat.hpp"
#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/views/imessageview.hpp"

#include "exceptions/addleexception.hpp"

#include "utilities/initializehelper.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT NotificationPresenter : public QObject, public INotificationPresenter
{
    Q_OBJECT
    Q_INTERFACES(Addle::INotificationPresenter Addle::IMessagePresenter)
    IAMQOBJECT_IMPL
public:
    virtual ~NotificationPresenter() = default;

    void initialize(
        QString text,
        Tone tone,
        bool isUrgent = false,
        IMessageContext* context = nullptr,
        QSharedPointer<AddleException> exception = nullptr);

    IMessageContext* context() const { ASSERT_INIT(); return _context; }

    void setContext(IMessageContext* context) {ASSERT_INIT(); _context = context; }

    Tone tone() const { ASSERT_INIT(); return _tone; }
    QString text() const { ASSERT_INIT(); return _text; }
    bool isUrgent() const { ASSERT_INIT(); return _isUrgent; }
    
    QSharedPointer<AddleException> exception() const { ASSERT_INIT(); return _exception; }
#ifdef ADDLE_DEBUG
    QString debugText() const { return _debugText; }
#endif
private:
    Tone _tone;
    QString _text;
    bool _isUrgent;
    IMessageContext* _context;
    QSharedPointer<AddleException> _exception;
#ifdef ADDLE_DEBUG
   QString _debugText;
#endif

    InitializeHelper _initHelper;
};

} // namespace Addle

#endif // APPLICATIONERRORPRESENTER_HPP
