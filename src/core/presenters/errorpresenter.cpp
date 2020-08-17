#include "errorpresenter.hpp"
using namespace Addle;

void ErrorPresenter::initialize(QString message, Severity severity)
{
    const Initializer init(_initHelper);

    _message = message;
    _severity = severity;
}