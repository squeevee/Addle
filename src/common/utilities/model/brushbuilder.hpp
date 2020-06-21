#ifndef BRUSHBUILDER_HPP
#define BRUSHBUILDER_HPP

#include <QMetaType>
#include <QSharedData>
#include <QIcon>

#include "idtypes/brushid.hpp"
#include "idtypes/brushengineid.hpp"

class BrushBuilder
{
    struct BrushBuilderData : QSharedData
    {
        BrushId id;
        BrushEngineId engine;
        QVariantHash engineParameters;

        QString name;
        QIcon icon;
    };

public:

    BrushBuilder()
        : _data(new BrushBuilderData)
    {
    }

    BrushId id() const { return _data->id; }
    BrushBuilder& setId(BrushId id) { _data->id = id; return *this; }

    BrushEngineId engine() const { return _data->engine; }
    BrushBuilder& setEngine(BrushEngineId engine) { _data->engine = engine; return *this; }

    QVariantHash engineParameters() const { return _data->engineParameters; }
    BrushBuilder& setEngineParameters(QVariantHash engineParameters) { _data->engineParameters = engineParameters; return *this; }

    QIcon icon() const { return _data->icon; }
    BrushBuilder& setIcon(QIcon icon) { _data->icon = icon; return *this; }

private:
    QSharedDataPointer<BrushBuilderData> _data;
};

#endif // BRUSHBUILDER_HPP