#ifndef BRUSHTIPID_HPP
#define BRUSHTIPID_HPP

#include <QSharedData>
#include <QSharedDataPointer>
#include <QVariant>

class BrushTipId
{
    struct BrushTipIdData : public QSharedData
    {
        BrushTipIdData(const char* name)
            :name(name)
        {
        }

        const char* name;
    };

public:
    BrushTipId() : _data(nullptr) { }

    BrushTipId(const char* name)
        : _data(new BrushTipIdData(name))
    {
    }

    BrushTipId(const BrushTipId& other)
        : _data(other._data)
    {
    }

    const char* getName() const { return _data->name; }


    const inline bool isNull() const { return !_data; }

    inline operator bool() const
    {
        return !isNull();
    }

    inline bool operator!() const
    {
        return isNull();
    }

    inline bool operator==(const BrushTipId& other) const
    {
        return _data == other._data;
    }
    inline bool operator!=(const BrushTipId& other) const
    {
        return _data != other._data;
    }

    inline operator QVariant() const
    {
        QVariant v;
        v.setValue(*this);
        return v;
    }

private:
    QSharedDataPointer<const BrushTipIdData> _data;

    friend uint qHash(const BrushTipId& brush);
};

Q_DECLARE_METATYPE(BrushTipId)

#endif // BRUSHTIPID_HPP