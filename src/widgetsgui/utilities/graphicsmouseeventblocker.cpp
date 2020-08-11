#include "graphicsmouseeventblocker.hpp"

#include <QEvent>

using namespace Addle;

bool GraphicsMouseEventBlocker::eventFilter(QObject* object, QEvent* event)
{
    return event->type() == QEvent::Type::GraphicsSceneMouseMove;
}