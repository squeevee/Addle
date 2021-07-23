#include "renderroutine.hpp"

using namespace Addle;

RenderRoutineChangedEvent& RenderRoutineChangedEvent::moveRoot(DataTreeNodeAddress rel)
{
    _nodeEvent.moveRoot(rel);
    
    sub_routine_map_t newSubRoutineEvents;
    
    while (!_subRoutineEvents.empty())
    {
        auto i = _subRoutineEvents.begin();
        newSubRoutineEvents[rel + i.key()] = std::move(*i);
        _subRoutineEvents.erase(i);
    }
    
    _subRoutineEvents.swap(newSubRoutineEvents);
    
    assert(!_subRoutineEvents.contains(rel));
    auto& atRel = _subRoutineEvents[rel];
    
    atRel._opacity                  = _opacity;
    atRel._opacityChanged           = _opacityChanged;
    atRel._compositionMode          = _compositionMode;
    atRel._compositionModeChanged   = _compositionModeChanged;
    atRel._passThroughMode          = _passThroughMode;
    atRel._passThroughModeChanged   = _passThroughModeChanged;
    atRel._background               = std::move(_background);
    atRel._backgroundChanged        = _backgroundChanged;
    
    _entityFlags.swap(atRel._entityFlags);
    
    _opacityChanged         = false;
    _compositionModeChanged = false;
    _passThroughModeChanged = false;
    _backgroundChanged      = false;
    
    return *this;
}


RenderRoutineChangedEvent& 
RenderRoutineChangedEvent::populateZValues(const RenderRoutine& routine)
{
    auto searchRoot = aux_datatree::tree_root(routine);
        
    for ( auto parentAddress : noDetach(_nodeEvent.affectedParentsFromEnd()) )
    {
        auto parentNode = aux_datatree::node_lookup_address(
                searchRoot,
                parentAddress
            );
        
        assert(parentNode && (*parentNode).isSubRoutine());
        auto subRoutine = (*parentNode).asSubRoutine();
        
        QList<double> values;
        values.reserve(subRoutine.components().size());
        for (auto&& c : subRoutine.components())
            values.append(c.z());
        
        setZValues(parentAddress, std::move(values));
    }
    
    return *this;
}
