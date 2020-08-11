#ifndef GUIUTILS_HPP
#define GUIUTILS_HPP

#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include "utilities/canvas/canvasmouseevent.hpp"

namespace Addle {

// todo: draw checkerboard on demand ?
QPixmap checkerBoardTexture(int width, QColor color1, QColor color2);

CanvasMouseEvent graphicsMouseToCanvasMouseEvent(const QGraphicsSceneMouseEvent* event);

} // namespace Addle

#endif // GUIUTILS_HPP