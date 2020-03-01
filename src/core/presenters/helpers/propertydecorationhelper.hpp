#ifndef PROPERTYDECORATIONHELPER_HPP
#define PROPERTYDECORATIONHELPER_HPP

#include <QString>
#include <QIcon>
#include <QHash>
#include <QVariant>
#include <QMetaType>
#include <QMetaEnum>

#include <QCoreApplication>

#include "utilities/propertydecoration.hpp"
#include "utilities/qt_extensions/translation.hpp"
#include "utilities/indexvariant.hpp"

class PropertyDecorationHelper
{
public:
    PropertyDecorationHelper(const QObject* owner)
        : _owner(owner)
    {
    }

    void setIconPool(QHash<IndexVariant, QIcon> icons) { _iconPool = icons; }

    PropertyDecoration getPropertyDecoration(const char* propertyName) const { return _decorations[QByteArray(propertyName)]; }
    void setPropertyDecoration(const char* propertyName, PropertyDecoration decor) { _decorations[QByteArray(propertyName)] = decor; }
    
    template<typename EnumType>
    void initializeEnumProperty(
            const char* propertyName,
            QIcon icon = QIcon(),
            QString text = QString(),
            QString tooltip = QString()
        )
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<EnumType>();
        QByteArray q_propertyName(propertyName);

        QKeyValueList<IndexVariant, OptionDecoration> optionDecorations;
        for (int index = 0; index < metaEnum.keyCount(); index++)
        {
            EnumType value = static_cast<EnumType>(metaEnum.value(index));

            QString optionText = QStringLiteral("%1.%2.text")
                .arg(QString::fromLatin1(q_propertyName))
                .arg(QString::fromLatin1(metaEnum.valueToKey(value)));
            QString optionToolTip = QStringLiteral("%1.%2.toolTip")
                .arg(QString::fromLatin1(q_propertyName))
                .arg(QString::fromLatin1(metaEnum.valueToKey(value)));

            IndexVariant v_index(value);
            optionDecorations.append(v_index, OptionDecoration(_iconPool.value(v_index), optionText, optionToolTip));
        }

        if (text.isNull())
            text = QStringLiteral("%1.text")
                .arg(QString::fromLatin1(q_propertyName));
        if (tooltip.isNull())
            tooltip = QStringLiteral("%1.toolTip")
                .arg(QString::fromLatin1(q_propertyName));

        _decorations[q_propertyName] = PropertyDecoration(
            propertyName,
            optionDecorations,
            icon,
            text,
            tooltip
        );
    }

    template<typename IdType>
    void initializeIdProperty(
            const char* propertyName,
            const QList<IdType>& values,
            QIcon icon = QIcon(),
            QString text = QString(),
            QString tooltip = QString()
        )
    {
        QByteArray q_propertyName(propertyName);

        QKeyValueList<IndexVariant, OptionDecoration> optionDecorations;
        for (const IdType& value : values)
        {
            QString optionText = QStringLiteral("%1.%2.text")
                .arg(QString::fromLatin1(q_propertyName))
                .arg(value.getKey());
            QString optionToolTip = QStringLiteral("%1.%2.toolTip")
                .arg(QString::fromLatin1(q_propertyName))
                .arg(value.getKey());

            IndexVariant v_index(QVariant::fromValue(value));
            optionDecorations.append(v_index, OptionDecoration(_iconPool.value(v_index), optionText, optionToolTip));
        }

        if (text.isNull())
            text = QStringLiteral("%1.text")
                .arg(QString::fromLatin1(q_propertyName));
        if (tooltip.isNull())
            tooltip = QStringLiteral("%1.toolTip")
                .arg(QString::fromLatin1(q_propertyName));

        _decorations.insert(q_propertyName, PropertyDecoration(
            propertyName,
            optionDecorations,
            icon,
            text,
            tooltip
        ));
    }

private:
    QHash<IndexVariant, QIcon> _iconPool;
    QHash<QByteArray, PropertyDecoration> _decorations;
    const QObject* const _owner;
};

#endif // PROPERTYDECORATIONHELPER_HPP