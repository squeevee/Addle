#ifndef TOOLID_HPP
#define TOOLID_HPP

#include <QObject>
#include <QString>

#include <QSharedData>
#include <QSharedDataPointer>

/**
 * @class ToolId
 * @brief An identifier of a tool
 */
class ToolId 
{
    class ToolIdData : public QSharedData
    {
    public:
        ToolIdData(const char* nameKey, const char* descriptionKey)
            : nameKey(nameKey),
            descriptionKey(descriptionKey)
        {
        }

        const char* const nameKey;
        const char* const descriptionKey;
    };
public:
    ToolId()
        : _data(nullptr)
    {
    }
    ToolId(
        const char* nameKey,
        const char* descriptionKey = nullptr
        )
        : _data(new ToolIdData(nameKey, descriptionKey))
    {
    }

    const char* getDescriptionKey() const { return _data ? _data->descriptionKey : nullptr; }
    const inline bool isNull() const { return !_data; }

    inline operator bool() const
    {
        return !isNull();
    }

    inline bool operator!() const
    {
        return isNull();
    }

    inline bool operator==(const ToolId& other) const
    {
        return _data == other._data;
    }
    inline bool operator!=(const ToolId& other) const
    {
        return _data != other._data;
    }

private:
    QSharedDataPointer<const ToolIdData> _data;

    friend uint qHash(const ToolId& tool);
};

Q_DECLARE_METATYPE(ToolId);

#endif // TOOLID_HPP