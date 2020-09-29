/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "notificationpresenter.hpp"

#include "utils.hpp"

using namespace Addle;

void NotificationPresenter::initialize(
    QString text,
    Tone tone,
    bool isUrgent,
    IMessageContext* context,
    QSharedPointer<AddleException> exception)
{
    const Initializer init(_initHelper);

    _text = text;
    _tone = tone;
    _isUrgent = isUrgent;
    _context = context;
    _exception = exception;
    
#ifdef ADDLE_DEBUG
    if (_exception)
    {
        if (typeid(*_exception) == typeid(UnhandledException))
        {
            UnhandledException& ex = static_cast<UnhandledException&>(*_exception);
            if (ex.addleException())
            {
                _debugText = ex.addleException()->what();
            }
            else if (ex.stdException())
            {
                _debugText = ex.stdException()->what();
            }
        }
        else
        {
            _debugText = _exception->what();
        }
    }
#endif
}
