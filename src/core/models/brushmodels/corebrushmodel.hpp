#ifndef COREBRUSHMODEL_HPP
#define COREBRUSHMODEL_HPP

#include "interfaces/models/ibrushmodel.hpp"

class CoreBrushModel : public IBrushModel
{
public: 
    CoreBrushModel(BrushId id, BrushPainterInfo info)
        : _id(id), _info(info)
    {
    }

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