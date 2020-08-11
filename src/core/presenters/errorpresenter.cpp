#include "errorpresenter.hpp"
using namespace Addle;

void ErrorPresenter::initialize(QString message, Severity severity)
{
    _initHelper.initializeBegin();

    _message = message;
    _severity = severity;

    _initHelper.initializeEnd();
}