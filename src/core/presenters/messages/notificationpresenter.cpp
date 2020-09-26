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
}
