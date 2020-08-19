/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "errorpresenter.hpp"
using namespace Addle;

void ErrorPresenter::initialize(QString message, Severity severity)
{
    const Initializer init(_initHelper);

    _message = message;
    _severity = severity;
}