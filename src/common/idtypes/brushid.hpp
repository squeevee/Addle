#ifndef BRUSHID_HPP
#define BRUSHID_HPP

#include "persistentid.hpp"
#include "brushengineid.hpp"

class ADDLE_COMMON_EXPORT BrushId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(BrushId)
public:
    BrushId(const char* key,
        BrushEngineId defaultEngine = BrushEngineId(),
        QVariantHash defaultParameters = QVariantHash())
        : BrushId(key, {
            { "defaultEngine", defaultEngine },
            { "defaultParameters", defaultParameters }
    })
    {
    }

    inline BrushEngineId defaultEngine() const
    {
        return isNull() ? BrushEngineId() : getMetadata().value("defaultEngine").value<BrushEngineId>();
    }

    inline QVariantHash defaultParameters() const
    {
        return isNull() ? QVariantHash() : getMetadata().value("defaultParameters").toHash();
    }
};
Q_DECLARE_METATYPE(BrushId)

#endif // BRUSHID_HPP