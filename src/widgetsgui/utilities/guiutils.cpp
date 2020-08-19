/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "guiutils.hpp"

#include <QPainter>

using namespace Addle;

QPixmap Addle::checkerBoardTexture(int width, QColor color1, QColor color2)
{
    QPixmap texture(QSize(2 * width, 2 * width));
    texture.fill(color1);

    QPainter p(&texture);
    p.fillRect(QRect(0,0, width, width), color2);
    p.fillRect(QRect(width, width, width * 2, width * 2), color2);

    p.end();

    return texture;
}

CanvasMouseEvent Addle::graphicsMouseToCanvasMouseEvent(const QGraphicsSceneMouseEvent* event)
{
    CanvasMouseEvent::Action action = (CanvasMouseEvent::Action)NULL;
    switch (event->type())
    {
    case QEvent::GraphicsSceneMousePress:
        action = CanvasMouseEvent::down;
        break;
    case QEvent::GraphicsSceneMouseRelease:
        action = CanvasMouseEvent::up;
        break;
    case QEvent::GraphicsSceneMouseMove:
        action = CanvasMouseEvent::move;
        break;
    };

    return CanvasMouseEvent(
        action,
        event->scenePos(),
        event->button(),
        event->buttons(),
        event->flags(),
        event->source(),
        event
    );
}
