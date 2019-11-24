#ifndef FORMATID_HPP
#define FORMATID_HPP

#include <typeinfo>

#include <QString>

#include <QSharedData>
#include <QSharedDataPointer>

/**
 * @class FormatId
 * @brief An identifier of a format
 * 
 * An object of this type identifies one format known to and used by Addle.
 * These objects can be quickly copied, assigned, and compared, as they are 
 * implicitly shared.
 * 
 * The underlying data type should only be instantiated once for every format
 * known to Addle, generally as static const variables, and all other objects
 * should be copied from these. The idea is for this to have similar usage to an
 * enum type, but extensible (even during runtime) and with more information
 * baked-in.
 * 
 * Each format should have a unique mime type to act as a human-readable
 * identifier (e.g., for debugging) and as a representation in serialized form.
 * If multiple FormatId data instances have the same mime type, then Addle's
 * state is malformed.
 * 
 * A FormatId object constructed with no arguments is the "null" reference. It
 * does not refer to any format and does not contain any information.
 */
class FormatId 
{
    class FormatIdData : public QSharedData
    {
    public:
        FormatIdData(const QString mimeType, const std::type_info& modelType, const char* descriptionIdentifier)
            : mimeType(mimeType),
            modelType(modelType),
            descriptionIdentifier(descriptionIdentifier)
        {
        }

        const QString mimeType;
        const std::type_info& modelType;

        const char* const descriptionIdentifier;
    };
public:
    FormatId()
        : _data(nullptr)
    {
    }
    FormatId(
        const QString mimeType,
        const std::type_info& modelType,
        const char* descriptionIdentifier = nullptr
        )
        : _data(new FormatIdData(mimeType, modelType, descriptionIdentifier))
    {
    }

    const QString getMimeType() const { return _data ? _data->mimeType : QString(); }
    const std::type_info& getModelType() const { return _data ? _data->modelType : typeid(void); }

    //the description identifier is a short string that identifies the
    //description of the format in the translation database
    const char* getDescriptionIdentifier() const { return _data ? _data->descriptionIdentifier : nullptr; }
    const inline bool isNull() const { return !_data; }

    inline operator bool() const
    {
        return !isNull();
    }

    inline bool operator!() const
    {
        return isNull();
    }

    inline bool operator==(const FormatId& other) const
    {
        return _data == other._data;
    }
    inline bool operator!=(const FormatId& other) const
    {
        return _data != other._data;
    }

private:
    QSharedDataPointer<const FormatIdData> _data;

    friend uint qHash(const FormatId& format);
};

#endif // FORMATID_HPP