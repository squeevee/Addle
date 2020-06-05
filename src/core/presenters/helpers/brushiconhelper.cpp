#include "brushiconhelper.hpp"
#include "servicelocator.hpp"

#include <QPainter>
#include <QPainterPath>

// BrushIconHelper::BrushIconHelper(QSharedPointer<IBrushPainter> painter)
//     : _painter(painter)
// {
// }

// QIcon BrushIconHelper::icon() const
// {
//     const double size = _painter->getSize();

//     QPixmap pixmap(64, 64);
//     pixmap.fill(Qt::white);

//     QPainter painter(&pixmap);
//     painter.setRenderHints(QPainter::Antialiasing);
//     QPainterPath path;
//     path.addEllipse(QPoint(32,32), size / 2, size / 2);

//     painter.fillPath(path, Qt::blue);

//     return QIcon(pixmap);
// }