#ifndef IDINFO_HPP
#define IDINFO_HPP

#include "idtypes/addleid.hpp"

#include <QHash>
#include <QSet>

#include "interfaces/traits.hpp"
#include "utilities/collections.hpp"

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
        
        //alright the *next thing* is linq: this is embarrassing
        
        QSet<IdType> result;
        
        for (AddleId id : noDetach(_idsByType.value(qMetaTypeId<IdType>())))
        {
            result.insert(static_cast<IdType>(id));
        }
        
        return result;
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
