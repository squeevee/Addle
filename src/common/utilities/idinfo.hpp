#ifndef IDINFO_HPP
#define IDINFO_HPP

#include "idtypes/addleid.hpp"

#include <QHash>
#include <QSet>

#include "interfaces/traits.hpp"

#include "utils.hpp"
#include "utilities/format/genericformat.hpp"

namespace Addle {

class IdInfo 
{
public:
    
    template<class IdType>
    static QSet<IdType> getIds()
    {
        if (_idsByType.isEmpty() && !AddleId::_dynamicMetaData.isEmpty())
        {
            rebuildCache();
        }

        return cpplinq::from(_idsByType.value(qMetaTypeId<IdType>()))
            >> cpplinq::cast<IdType>()
            >> cpplinq::to_QSet();
    }
    
    template<class Interface>
    static QSet<typename Traits::id_type<Interface>::type> getIdsFor()
    {
        return getIds<typename Traits::id_type<Interface>::type>();
    }
    
private:
    
    static void rebuildCache()
    {
        _idsByType.clear();
        
        {
            auto&& begin = AddleId::_dynamicMetaData.keyBegin();
            auto&& end = AddleId::_dynamicMetaData.keyEnd();
            for (auto i = begin; i != end; ++i)
            {
                _idsByType[(*i).metaTypeId()].insert(*i);
            }
        }
    }
    
    static QHash<int, QSet<AddleId>> _idsByType;
};

} // namespace Addle

#endif // IDINFO_HPP
