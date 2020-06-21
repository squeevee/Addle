#ifndef BRUSHMODEL_HPP
#define BRUSHMODEL_HPP

#include "compat.hpp"

#include "interfaces/models/ibrushmodel.hpp"
#include "utilities/initializehelper.hpp"

class ADDLE_CORE_EXPORT BrushModel : public IBrushModel
{
public:

    void initialize(const BrushBuilder& builder);

    BrushId id() const { _initHelper.check(); return _id; }
    BrushEngineId engineId() const { _initHelper.check(); return _engineId; }
    QVariantHash customEngineParameters() const { _initHelper.check(); return _customEngineParameters; }

    QIcon icon() const { _initHelper.check(); return _icon; }

    BrushInfo info() const { _initHelper.check(); return _info; }

private:
    BrushId _id;
    BrushEngineId _engineId;
    BrushInfo _info;
    QVariantHash _customEngineParameters;

    QIcon _icon; 

    InitializeHelper<BrushModel> _initHelper;
};

#endif // BRUSHMODEL_HPP