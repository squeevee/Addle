#ifndef PROPERTYDECORATION_HPP
#define PROPERTYDECORATION_HPP

#include <QSharedData>
#include <QSharedDataPointer>
#include <QString>
#include <QIcon>

#include <QHash>
#include "utilities/qtextensions/qlist.hpp"
#include "utilities/indexvariant.hpp"

class OptionDecoration
{
    struct OptionDecorationData : QSharedData
    {
        OptionDecorationData() = default;
        OptionDecorationData(QIcon arg_icon, QString arg_text, QString arg_toolTip)
            : icon(arg_icon), text(arg_text), toolTip(arg_toolTip)
        {
        }

        QIcon icon;
        QString text;
        QString toolTip;
    };

public:
    OptionDecoration()
        : _data(new OptionDecorationData)
    {
    }

    OptionDecoration(QIcon icon, QString text, QString toolTip = QString())
        : _data(new OptionDecorationData(icon, text, toolTip))
    {
    }

    OptionDecoration(const OptionDecoration& other)
        : _data(other._data)
    {
    }

    QIcon getIcon() const { return _data->icon; }
    void setIcon(QIcon icon) { _data->icon = icon; }

    QString getText() const { return _data->text; }
    void setText(QString text) { _data->text = text; }

    QString getToolTip() const { return _data->toolTip; }
    void setToolTip(QString toolTip) { _data->toolTip = toolTip; }

private: 
    QSharedDataPointer<OptionDecorationData> _data;
};

class PropertyDecoration
{
    struct PropertyDecorationData : QSharedData
    {
        PropertyDecorationData(const char* arg_decorations)
            : propertyName(arg_decorations)
        {
        }

        PropertyDecorationData(
            const char* arg_propertyName,
            QKeyValueList<IndexVariant, OptionDecoration> arg_decorations,
            QIcon arg_icon,
            QString arg_text,
            QString arg_tooltip)
            : propertyName(arg_propertyName),
            options(arg_decorations.keys()),
            decorations(arg_decorations.toHash()),
            icon(arg_icon),
            text(arg_text),
            toolTip(arg_tooltip)
        {
        }

        const char* propertyName;

        QIcon icon;
        QString text;
        QString toolTip;

        QList<IndexVariant> options;
        QHash<IndexVariant, OptionDecoration> decorations;
    };

public:
    PropertyDecoration()
    {
    }

    PropertyDecoration(const char* propertyName)
        : _data(new PropertyDecorationData(propertyName))
    {
    }

    PropertyDecoration(
            const char* propertyName,
            QKeyValueList<IndexVariant, OptionDecoration> decorations,
            QIcon icon = QIcon(),
            QString text = QString(),
            QString toolTip = QString())
        : _data(new PropertyDecorationData(
            propertyName,
            decorations, 
            icon,
            text,
            toolTip
        ))
    {
    }
    PropertyDecoration(const PropertyDecoration& other)
        : _data(other._data)
    {
    }



    bool isNull() const { return !_data; }

    const char* getPropertyName() const { return _data ? _data->propertyName : nullptr; }

    QIcon getIcon() const { return _data ? _data->icon : QIcon(); }
    void setIcon(QIcon icon) { if (_data) _data->icon = icon; }

    QString getText() const { return _data ? _data->text : QString(); }
    void setText(QString text) { if (_data) _data->text = text; }

    QString getToolTip() const { return _data ? _data->toolTip : QString(); }
    void setToolTip(QString toolTip) { if (_data) _data->toolTip = toolTip; }
    
    OptionDecoration getDecoration(QVariant option) const { return !isNull() ? _data->decorations.value(option) : OptionDecoration(); }
    void setDecoration(QVariant option, const OptionDecoration& decor)
    {
        if (_data)
        {
            if (!_data->decorations.contains(option))
            {
                _data->options.append(option);
            }
            _data->decorations[option] = decor;
        }
    }

    QList<QVariant> getOptions() const { return _data ? qListCast<QVariant>(_data->options) : QList<QVariant>(); }

    // iterator?
    // [] operator?

private:
    QSharedDataPointer<PropertyDecorationData> _data;
};

Q_DECLARE_TYPEINFO(OptionDecoration, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(PropertyDecoration, Q_MOVABLE_TYPE);

#endif // PROPERTYDECORATION_HPP