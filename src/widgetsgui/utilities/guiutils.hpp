#ifndef GUIUTILS_HPP
#define GUIUTILS_HPP

#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include "utilities/canvas/canvasmouseevent.hpp"

// todo: draw checkerboard on demand ?
QPixmap checkerBoardTexture(int width, QColor color1, QColor color2);

CanvasMouseEvent graphicsMouseToCanvasMouseEvent(const QGraphicsSceneMouseEvent* event);

#endif // GUIUTILS_HPP