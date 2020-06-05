#ifndef BRUSHMODEL_HPP
#define BRUSHMODEL_HPP

#include "interfaces/models/ibrushmodel.hpp"

class BrushModel : public IBrushModel
{
public:
    BrushId id() const { return _id; }
    BrushEngineId engineId() const { return _engineId; }
    BrushInfo info() const { return _info; }
    QVariantHash customEngineParameters() const { return _customEngineParameters; }

    static BrushModel* fromId(BrushId id)
    {
        BrushModel* model = new BrushModel;
        model->_id = id;
        model->_engineId = id.defaultEngine();
        model->_customEngineParameters = id.defaultParameters();

        return model;
    }

private:
    BrushId _id;
    BrushEngineId _engineId;
    BrushInfo _info;
    QVariantHash _customEngineParameters;
};

#endif // BRUSHMODEL_HPP