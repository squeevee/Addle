/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "notificationpresenter.hpp"

#include "utilities/presenter/messagebuilders.hpp"

#include "utils.hpp"

using namespace Addle;

void NotificationPresenter::initialize(const NotificationPresenterBuilder& builder)
{
    const Initializer init(_initHelper);

    _text = builder.textHint();
    
    ADDLE_ASSERT(builder.toneHint());
    _tone = *builder.toneHint();
    
    ADDLE_ASSERT(builder.isUrgentHint());
    _isUrgent = *builder.isUrgentHint();
    
    _context = builder.context();
    _exception = builder.exception();
    
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
