#include "graphicsmouseeventblocker.hpp"

#include <QEvent>

bool GraphicsMouseEventBlocker::eventFilter(QObject* object, QEvent* event)
{
    return event->type() == QEvent::Type::GraphicsSceneMouseMove;
}