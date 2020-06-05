#ifndef PATHENGINE_HPP
#define PATHENGINE_HPP

#include "compat.hpp"
#include <QObject>

#include "interfaces/editing/ibrushengine.hpp"
#include "interfaces/models/ibrushmodel.hpp"

class ADDLE_CORE_EXPORT PathBrushEngine : public IBrushEngine
{
public:
    static const BrushEngineId ID;

    virtual ~PathBrushEngine() = default;

    BrushEngineId id() const { return ID; }

    BrushInfo info(const IBrushModel& model) const { return BrushInfo(); }

    QPainterPath indicatorShape(const BrushStroke& painter) const;
    void paint(BrushStroke& painter) const;

private:

    QRect boundingRect(QPointF pos, double size) const;
};

#endif // PATHENGINE_HPP