#ifndef PRESETHELPERS_HPP
#define PRESETHELPERS_HPP

#include "interfaces/presenters/iviewportpresenter.hpp"

#include <map>
#include <set>
//We're using STL for this helper class because Qt does not provide a direct
//equivalent to std::set , and I don't want to use two slightly-different API's
//all throughout the same class.

template<typename PresetType>
class PresetHelper
{
protected:
    PresetHelper(
        bool cyclic,
        const PresetType nullPreset,
        const PresetType lowestPreset,
        const PresetType highestPreset,
        const std::set<PresetType>&& presets,
        const std::map<double, PresetType>&& presetsByValue
    ) : _cyclic(cyclic),
        _nullPreset(nullPreset),
        _highestPreset(highestPreset),
        _lowestPreset(lowestPreset),
        _presets(presets),
        _presets_byValue(presetsByValue)
    {
    }

    PresetType nearest_p(double value, double threshold) const
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

            double upperDiff = upper.first - value;
            double lowerDiff = value - lower.first;
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

    PresetType nextUp_p(double value) const
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

    PresetType nextUp_p(PresetType preset) const
    {
        auto iter = _presets.find(preset);
        iter++;
        if (iter == _presets.end())
            return _cyclic ? _lowestPreset : _nullPreset;
        else 
            return *iter;
    }

    PresetType nextDown_p(double value) const
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

    PresetType nextDown_p(PresetType preset) const
    {
        auto iter = _presets.find(preset);
        if (iter == _presets.begin())
            return _cyclic ? _highestPreset : _nullPreset;
        else 
            return *(--iter);
    }

private:
    const bool _cyclic;
    const PresetType _nullPreset;
    const PresetType _highestPreset;
    const PresetType _lowestPreset;
    const std::set<PresetType> _presets;
    const std::map<double, PresetType> _presets_byValue;
};

#endif // PRESETHELPERS_HPP