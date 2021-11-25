/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "pathbrushengine.hpp"
#include "globals.hpp"

// #include "interfaces/editing/irastersurface.hpp"
#include <QPainter>
#include <QPen>
#include <QtDebug>
#include "utils.hpp"

#include <QColor>
using namespace Addle;

QPainterPath PathBrushEngine::indicatorShape(const BrushStroke& painter) const
{
    return QPainterPath();
}

void PathBrushEngine::paint(BrushStroke& brushStroke) const
{
    if (brushStroke.positions().isEmpty()) return;

    const QPointF pos = brushStroke.positions().last();
    const double size = brushStroke.size();

//     const bool eraser = brushStroke.brush().eraserMode();

    if (size == 0 || qIsNaN(size)) return;

//     if (eraser)
//     {
// //         brushStroke.buffer()->setReplaceMode(true);
//     }

    if (!brushStroke.isMarkedPainted())
    {
        const QRect nibBound = coarseBoundRect(pos, size);

        const double halfSize = size / 2;

//         auto handle = brushStroke.buffer()->paintHandle(nibBound);
//         QPainter& painter = handle.painter();

//         painter.setRenderHint(QPainter::Antialiasing, true);
//         painter.setPen(Qt::NoPen);
//         painter.setBackground(Qt::transparent);
//         painter.setCompositionMode(eraser ? QPainter::CompositionMode_DestinationOut : QPainter::CompositionMode_Source);
// 
//         painter.setBrush(eraser ? Qt::black : brushStroke.color());
// 
//         //painter.eraseRect(nibBound);
//         painter.drawEllipse(QRectF(
//             pos - QPointF(halfSize, halfSize),
//             QSizeF(size, size)
//         ));
// 
//         brushStroke.setBound(nibBound);
//         brushStroke.markPainted();
    }
    else 
    {
        const QRect lastNibBound = brushStroke.lastPaintedBound();

        QRect nibBound = coarseBoundRect(pos, size);
        QRect bound = nibBound.united(lastNibBound);

        QPainterPath path(*(brushStroke.positions().rbegin() + 1));
        path.lineTo(brushStroke.positions().last());

        QRect alphaBound;
        QImage destAlpha;

        {
//             auto bitReader = brushStroke.buffer()->bitReader(bound);
//             alphaBound = bitReader.area();
// 
//             destAlpha = QImage(alphaBound.size(), QImage::Format_Alpha8);
//             destAlpha.fill(Qt::transparent);
// 
//             for (int line = 0; line < alphaBound.height(); line++)
//             {
//                 const QRgb* s = reinterpret_cast<const QRgb*>(bitReader.scanLine(line));
//                 uchar* d = destAlpha.scanLine(line);
// 
//                 for (int x = 0; x < alphaBound.width(); x++)
//                 {
//                     *d = qAlpha(*s);
// 
//                     d++;
//                     s++;
//                 }
//             }
        }

        QImage sourceAlpha;

        {
//             auto paintHandle = brushStroke.buffer()->paintHandle(bound);
//             paintHandle.painter().setCompositionMode(eraser
//                 ? QPainter::CompositionMode_DestinationOut
//                 : QPainter::CompositionMode_Source);
// 
//             sourceAlpha = QImage(alphaBound.size(), QImage::Format_Alpha8);
//             sourceAlpha.fill(Qt::transparent);
// 
//             QPainter alphaPainter(&sourceAlpha);
//             alphaPainter.translate(-alphaBound.topLeft());
//             alphaPainter.setCompositionMode(QPainter::CompositionMode_Source);

//             for (QPainter* painter : { &paintHandle.painter(), &alphaPainter })
//             {
//                 painter->setRenderHint(QPainter::Antialiasing, true);
//                 painter->setClipRect(bound);
// 
//                 painter->setPen(QPen(
//                     eraser ? Qt::black : brushStroke.color(),
//                     size,
//                     Qt::SolidLine,
//                     Qt::RoundCap,
//                     Qt::RoundJoin
//                 ));
//             
//                 painter->drawPath(path);
//             }
        }

//         if (!eraser) //TODO
//         {
//             auto bitWriter = brushStroke.buffer()->bitWriter(alphaBound);
            
//             for (int line = 0; line < alphaBound.height(); line++)
//             {
//                 const uchar* a = sourceAlpha.scanLine(line);
//                 const uchar* b = destAlpha.scanLine(line);
//                 QRgb* d = reinterpret_cast<QRgb*>(bitWriter.scanLine(line));
// 
//                 for (int x = 0; x < alphaBound.width(); x++)
//                 {
//                     uchar alpha;
//                     alpha = qMax(*a, *b);
// 
//                     if (alpha == 0) *d = 0;
//                     *d = qRgba(qRed(*d), qGreen(*d), qBlue(*d), alpha);
// 
//                     d++;
//                     a++;
//                     b++;
//                 }
//             }
//         }

        brushStroke.setBound(nibBound);
        brushStroke.markPainted();
    }
}
