#ifndef COREBRUSHMODEL_HPP
#define COREBRUSHMODEL_HPP

#include "core/compat.hpp"
#include "interfaces/models/ibrushmodel.hpp"

class ADDLE_CORE_EXPORT CoreBrushModel : public IBrushModel
{
public: 
    CoreBrushModel(BrushId id, BrushPainterInfo info)
        : _id(id), _info(info)
    {
    }

	virtual ~CoreBrushModel() = default;

    BrushId getId() const { return _id; }
    BrushPainterInfo getPainterInfo() const { return _info; }

    template<class Painter>
    static IBrushModel* make()
    {
        return new CoreBrushModel(Painter::ID, Painter::INFO);
    }

private:
    BrushId _id;
    BrushPainterInfo _info;
};

#endif // COREBRUSHMODEL_HPP