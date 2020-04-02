#ifndef IRASTEROPERATION_HPP
#define IRASTEROPERATION_HPP

#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QWeakPointer>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

#include "interfaces/editing/surfaces/irastersurface.hpp"
#include "iundooperation.hpp"

#include <QImage>
#include <QPoint>

class IRasterOperation : public IUndoOperation
{
public:
    enum Mode
    {
        Merge,
        Replace
    };

    virtual ~IRasterOperation() = default;

    virtual void initialize(
        QWeakPointer<IRasterSurface> surface,
        QImage data,
        QPoint offset,
        Mode mode = Merge
    ) = 0;
};

DECL_MAKEABLE(IRasterOperation)
DECL_EXPECTS_INITIALIZE(IRasterOperation)

#endif // IRASTEROPERATION_HPP