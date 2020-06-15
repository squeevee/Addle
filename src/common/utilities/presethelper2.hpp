#ifndef PRESETHELPER2_HPP
#define PRESETHELPER2_HPP

#include <initializer_list>
#include <type_traits>
#include <QHash>
#include <QMap>

template<typename IdType = int, typename ValueType = double>
class PresetHelper2
{
    static_assert(
        std::is_integral<IdType>::value ||
        (std::is_enum<IdType>::value && std::is_convertible<IdType, int>::value),
        "Unsupported IdType"
    );

public:
    static constexpr int INVALID_ID = -1;

    PresetHelper2() = default;

    PresetHelper2(QHash<IdType, ValueType> presets, bool cyclic = false)
        : _values(presets), _cyclic(cyclic), _stale(true)
    {
    }

    PresetHelper2(std::initializer_list<ValueType> values, bool cyclic = false)
        : _cyclic(cyclic), _stale(true)
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

    void setCyclic(bool cyclic)
    {
        _cyclic = cyclic;
    }

    IdType nearest(ValueType target, ValueType margin) const
    {
        if (_stale) rebuildIndex();

        if (_index.isEmpty()) return INVALID_ID;

        auto iter = _index.upperBound(target);

        if (iter == _index.begin())
        {
            //The given value is smaller than all of the presets.

            IdType lowest = _index.first();
            if ( qAbs(target - lowest) < margin )
                return lowest;
            else if (_cyclic)
                return _index.last();
            else 
                return INVALID_ID;
        }
        else if (iter == _index.end())
        {
            //The given value is larger than all of the presets.

            IdType highest = _index.last();
            if ( qAbs(target - highest) < margin )
                return highest;
            else if (_cyclic)
                return _index.first();
            else
                return INVALID_ID;
        }
        else
        {
            //The given value is between two presets, or equal to one preset.

            ValueType lowerDiff = qAbs(target - (iter - 1).key());
            ValueType upperDiff = qAbs(target - iter.key());

            if (lowerDiff <= upperDiff && lowerDiff <= margin)
                return *(iter - 1);
            else if (lowerDiff > upperDiff && upperDiff <= margin)
                return *iter;
            else 
                return INVALID_ID;
        }
    }

    IdType nextUp(ValueType target) const
    {

    }

    IdType nextDown(ValueType target) const
    {

    }

    IdType lowest() const
    {
        if (_stale) rebuildIndex();
        if (_index.isEmpty()) return INVALID_ID;
        return _index.first();
    }

    IdType highest() const
    {
        if (_stale) rebuildIndex();
        if (_index.isEmpty()) return INVALID_ID;
        return _index.last();
    }

private:

    void rebuildIndex() const
    {
        _index.clear();
        for (auto iter = _values.begin(); iter != _values.end(); iter++)
        {
            IdType id = iter.key();
            ValueType value = iter.value();
            _index.insert(value, id);
        }
    }

    bool _cyclic;

    QHash<IdType, ValueType> _values;

    mutable QMap<ValueType, IdType> _index;
    mutable bool _stale = false;
};

#endif // PRESETHELPER2_HPP