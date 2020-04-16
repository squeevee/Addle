#ifndef PRESETHELPERS_HPP
#define PRESETHELPERS_HPP

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/traits/compat.hpp"

#include <initializer_list>
#include <utility>
#include <map>
#include <set>
//We're using STL for this helper class because Qt does not provide a direct
//equivalent to std::set

template<typename PresetType, typename ValueType>
class PresetHelper
{
private:

    inline static std::set<PresetType> initPresets(const std::initializer_list<std::pair<PresetType, ValueType>>& presets)
    {
        std::set<PresetType> result;
        for (auto entry : presets)
        {
            result.insert(entry.first);
        }
        return result;
    }

    inline static std::map<ValueType, PresetType> initPresetsByValue(const std::initializer_list<std::pair<PresetType, ValueType>>& presets)
    {
        std::map<ValueType, PresetType> result;
        for (auto entry : presets)
        {
            result[entry.second] = entry.first;
        }
        return result;
    }

    inline static std::map<PresetType, ValueType> initValuesByPreset(const std::initializer_list<std::pair<PresetType, ValueType>>& presets)
    {
        std::map<PresetType, ValueType> result;
        for (auto entry : presets)
        {
            result[entry.first] = entry.second;
        }
        return result;
    }

    inline static PresetType initLowestPreset(const std::set<PresetType>& presets)
    {
        return *(presets.begin());
    }

    inline static PresetType initHighestPreset(const std::set<PresetType>& presets)
    {
        return *(presets.rbegin());
    }

public:
    PresetHelper(
        bool cyclic,
        const PresetType nullPreset,
        std::initializer_list<std::pair<PresetType, ValueType>> presetsAndValues
    ) : _cyclic(cyclic),
        _nullPreset(nullPreset),
        _presets(initPresets(presetsAndValues)),
        _presets_byValue(initPresetsByValue(presetsAndValues)),
        _values_byPreset(initValuesByPreset(presetsAndValues)),
        _lowestPreset(initLowestPreset(_presets)),
        _highestPreset(initHighestPreset(_presets))

    {
    }

    PresetType nearest(ValueType value, ValueType threshold = 0) const
    {
        auto iter = _presets_byValue.upper_bound(value);
        auto upper = *iter;
        if (iter == _presets_byValue.begin())
        {
            //The given value is smaller than all of the presets.

            if (threshold == 0.0 || value + (threshold * value) >= upper.first) {
                return upper.second;
            }
            else
            {
                return _cyclic ? _highestPreset : _nullPreset;
            }
        }
        else if (iter == _presets_byValue.end())
        {
            //The given value is larger than all of the presets.

            iter--;
            auto lower = *iter;

            if (threshold == 0.0 || value - (threshold * value) <= lower.first) {
                return lower.second;
            }
            else
            {
                return _cyclic ? _lowestPreset : _nullPreset;
            }
        }
        else
        {
            //The given value is between two presets, or equal to one preset.
            iter--;
            auto lower = *iter;

            auto nearer = _nullPreset;

            ValueType upperDiff = upper.first - value;
            ValueType lowerDiff = value - lower.first;
            if (upperDiff >= lowerDiff)
                nearer = upper.second;
            else
                nearer = lower.second;
            
            bool withinUpper = value + (threshold * value) >= upper.first;
            bool withinLower = value - (threshold * value) <= lower.first;

            if (threshold == 0.0 || (withinUpper && withinLower))
                return nearer;
            else if (withinUpper)
                return upper.second;
            else if (withinLower)
                return lower.second;
            else
                return _nullPreset;
        }
    }

    PresetType nextUp(ValueType value) const
    {
        auto iter = _presets_byValue.upper_bound(value);
        if (iter == _presets_byValue.end())
        {
            //The given value is greater than all of the presets.

            return _cyclic ? _lowestPreset : _nullPreset;
        }
        else
        {
            return iter->second;
        }
    }

    PresetType nextUp(PresetType preset) const
    {
        auto iter = _presets.find(preset);
        iter++;
        if (iter == _presets.end())
            return _cyclic ? _lowestPreset : _nullPreset;
        else 
            return *iter;
    }

    PresetType nextDown(ValueType value) const
    {
        auto iter = _presets_byValue.lower_bound(value);
        if (iter == _presets_byValue.begin())
        {
            //The given value is smaller than all of the presets.

            return _cyclic ? _highestPreset : _nullPreset;
        }
        else
        {
            return (--iter)->second;
        }
    }

    PresetType nextDown(PresetType preset) const
    {
        auto iter = _presets.find(preset);
        if (iter == _presets.begin())
            return _cyclic ? _highestPreset : _nullPreset;
        else 
            return *(--iter);
    }

    ValueType valueOf(PresetType preset) const
    {
        return _values_byPreset.at(preset);
    }

private:
    const std::set<PresetType> _presets;
    const std::map<ValueType, PresetType> _presets_byValue;
    const std::map<PresetType, ValueType> _values_byPreset;

    const bool _cyclic;
    const PresetType _nullPreset;
    const PresetType _highestPreset;
    const PresetType _lowestPreset;
};

#endif // PRESETHELPERS_HPP