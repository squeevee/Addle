/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PRESETMAP_HPP
#define PRESETMAP_HPP

#include <initializer_list>
#include <type_traits>
#include <limits>
#include <cmath> // for std::floor

#include <QHash>
#include <QMap>
namespace Addle {

template<typename IdType = int, typename ValueType = double>
class PresetMap
{
    static_assert(
        std::is_integral<IdType>::value ||
        (std::is_enum<IdType>::value && std::is_convertible<IdType, int>::value),
        "Unsupported IdType"
    );

    static inline ValueType valueCap()
    {
        if (std::numeric_limits<ValueType>::has_infinity)
            return std::numeric_limits<ValueType>::infinity();
        else
            return std::numeric_limits<ValueType>::max();
    }

public:
    static constexpr IdType INVALID_ID = static_cast<IdType>(-1);

    PresetMap() = default;

    PresetMap(QHash<IdType, ValueType> presets, bool cyclic = false, ValueType gap = valueCap())
        : _values(presets), _cyclic(cyclic), _gap(gap), _stale(true)
    {
    }

    PresetMap(std::initializer_list<std::pair<IdType, ValueType>> presets, bool cyclic = false, ValueType gap = valueCap())
        : _values(presets), _cyclic(cyclic), _gap(gap), _stale(true)
    {
    }

    PresetMap(std::initializer_list<ValueType> values, bool cyclic = false, ValueType gap = valueCap())
        : _cyclic(cyclic), _gap(gap), _stale(true)
    {
        IdType index = 0;
        for (ValueType value : values)
        {
            _values[index] = value;
            index++;
        }
    }

    void setPresets(QList<ValueType> values)
    {
        _values.clear();
        _stale = true;

        IdType index = 0;
        for (ValueType value : values)
        {
            _values[index] = value;
            index++;
        }
    }

    void setPresets(QHash<IdType, ValueType> presets)
    {
        _values = presets;
        _stale = true;
    }

    void setCyclic(bool cyclic, ValueType gap = valueCap())
    {
        _cyclic = cyclic;
        if (gap != valueCap())
        {
            _gap = gap;
            _stale = true;
        }
    }

    ValueType valueOf(IdType id) const { return _values[id]; }

    IdType nearest(ValueType target, ValueType margin = valueCap()) const
    {
        if (_stale) rebuildIndex();

        if (_index_byValue.isEmpty()) return INVALID_ID;

        target = normalizeTarget(target);

        auto iter = _index_byValue.upperBound(target);

        auto&& indexBegin = _index_byValue.begin();
        auto&& indexEnd = _index_byValue.end();

        if (!_cyclic && iter == indexBegin)
        {
            //The given value is smaller than all of the presets.

            ValueType lowestValue = _index_byValue.first();

            if ( qAbs(target - lowestValue) < margin )
                return _index_byValue.first();
            else 
                return INVALID_ID;
        }
        else if (!_cyclic && iter == indexEnd)
        {
            //The given value is larger than all of the presets.

            ValueType highestValue = _index_byValue.last();

            if ( qAbs(target - highestValue) < margin )
                return _index_byValue.last();
            else
                return INVALID_ID;
        }
        else
        {
            //The given value is between two presets, or equal to one preset.
            ValueType lower;
            ValueType upper;

            IdType lowerId;
            IdType upperId;

            auto lowerIter = iter - 1;

            if (_cyclic && iter == indexEnd)
            {
                lower = lowerIter.key();
                lowerId = *lowerIter;

                upper = lowerIter.key() + _gap;
                upperId = _index_byValue.first();
            }
            else
            {
                lower = lowerIter.key();
                lowerId = *lowerIter;

                upper = iter.key();
                upperId = *iter;
            }

            ValueType lowerDiff = qAbs(target - lower);
            ValueType upperDiff = qAbs(target - upper);

            if (lowerDiff <= upperDiff && lowerDiff <= margin)
                return lowerId;
            else if (lowerDiff > upperDiff && upperDiff <= margin)
                return upperId;
            else 
                return INVALID_ID;
        }
    }

    IdType nextUp(ValueType target) const
    {
        if (_stale) rebuildIndex();
        if (_index_byValue.isEmpty()) return INVALID_ID;

        target = normalizeTarget(target);

        auto iter = _index_byValue.upperBound(target);
        if (iter == _index_byValue.end())
        {
            //The given value is greater than all of the presets.

            return _cyclic ? _index_byValue.first() : INVALID_ID;
        }
        else
        {
            return *iter;
        }
    }

    IdType nextUp(IdType targetId) const
    {
        if (!_values.contains(targetId))
            return INVALID_ID;
        else
            return nextUp(_values[targetId]);
    }

    IdType nextDown(ValueType target) const
    {
        if (_stale) rebuildIndex();
        if (_index_byValue.isEmpty()) return INVALID_ID;

        target = normalizeTarget(target);

        auto iter = _index_byValue.lowerBound(target);
        if (iter == _index_byValue.begin())
        {
            //The given value is lower than all of the presets.

            return _cyclic ? _index_byValue.last() : INVALID_ID;
        }
        else
        {
            return *(iter - 1);
        }
    }

    IdType nextDown(IdType targetId) const
    {
        if (!_values.contains(targetId))
            return INVALID_ID;
        else
            return nextDown(_values[targetId]);
    }

    IdType lowest() const
    {
        if (_stale) rebuildIndex();
        if (_index_byValue.isEmpty()) return INVALID_ID;
        return _index_byValue.first();
    }

    IdType highest() const
    {
        if (_stale) rebuildIndex();
        if (_index_byValue.isEmpty()) return INVALID_ID;
        return _index_byValue.last();
    }

private:

    void rebuildIndex() const
    {
        _index_byValue.clear();
        for (auto iter = _values.begin(); iter != _values.end(); iter++)
        {
            IdType id = iter.key();
            ValueType value = iter.value();
            _index_byValue.insert(value, id);
        }

        if (_cyclic && _gap == valueCap() && _index_byValue.size() >= 2)
        {
            auto i = _index_byValue.end();
            ValueType last = (--i).key();
            ValueType secondToLast = (--i).key();
            _gap = last - secondToLast;
        }

        _span = _index_byValue.lastKey() - _index_byValue.firstKey() + _gap;
    }

    ValueType normalizeTarget(ValueType target) const
    {
        if (!_cyclic)
            return target;

        ValueType lowest = _index_byValue.firstKey();
        target = target - _span * std::floor(target / (_span - lowest)) + lowest;

        return target;
    }

    bool _cyclic;
    mutable ValueType _gap;
    mutable ValueType _span;

    QHash<IdType, ValueType> _values;

    mutable QMap<ValueType, IdType> _index_byValue;
    mutable bool _stale = false;
};

typedef PresetMap<> BasicPresetMap;

} // namspace Addle

#endif // PRESETMAP_HPP