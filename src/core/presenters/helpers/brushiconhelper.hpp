#ifndef BRUSHICONHELPER_HPP
#define BRUSHICONHELPER_HPP

#include <QSharedPointer>

#include <QIcon>
#include <QIconEngine>
#include <QPixmap>

#include "interfaces/editing/ibrushpainter.hpp"
#include "idtypes/brushid.hpp"

// This could probably be made smarter by subclassing QIconEngine, but
// that presents a slight challenge in pairing high-level Addle stuff with
// low-level QtGui stuff in a safe way. It's not currently a priority.

class BrushIconHelper
{
public:
    BrushIconHelper(QSharedPointer<IBrushPainter> painter);
    virtual ~BrushIconHelper() = default;

    QIcon icon() const;

private:
    mutable QSharedPointer<IBrushPainter> _painter;
};

#endif // BRUSHICONHELPER_HPP