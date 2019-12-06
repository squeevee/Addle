#ifndef TOOLPRESENTER_HPP
#define TOOLPRESENTER_HPP

#include <QObject>
#include <QVariant>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QStringBuilder>

#include "helpers/toolpathhelper.hpp"

#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

// Notice: this is meant to help with tooltip options, and is not suitable for
// general use.
uint qHash( const QVariant& var);

class ToolPresenterBase : public QObject, public virtual IToolPresenter
{
    Q_OBJECT
public:
    ToolPresenterBase(ToolPathHelper::TrackingOptions pathTracking) :
        _toolPathHelper(pathTracking)
    {
    }
    virtual ~ToolPresenterBase() = default;

    IDocumentPresenter* getOwner() { _initHelper.assertInitialized(); return _documentPresenter; }

    QCursor getCursorHint();
    QIcon getIcon() { _initHelper.assertInitialized(); return _icon; }
    QString getName() { _initHelper.assertInitialized(); return _name; }
    QString getToolTip() { _initHelper.assertInitialized(); return _toolTip; }

    void pointerEngage(QPointF canvasPos);
    void pointerMove(QPointF canvasPos);
    void pointerDisengage(QPointF canvasPos);

    bool isSelected()
    {
        return _documentPresenter->getCurrentToolPresenter() == this;
    }

    QIcon getOptionIcon(const char* name)
    {
        QString q_name = QString::fromLatin1(name);
        if (_optionIconsByProperty.contains(q_name))
        {
            return _optionIconsByProperty.value(q_name);
        }
        return QIcon();
    }

    QIcon getOptionIcon(const char* name, QVariant value)
    {
        QString q_name = QString::fromLatin1(name);
        if (_optionIconsByValue.contains(q_name))
        {
            auto& propertyIcons = _optionIconsByValue[q_name];
            if (propertyIcons.contains(value))
            {
                return propertyIcons.value(value);
            }
        }
        return QIcon();
    }

    QString getOptionToolTip(const char* name)
    {
        QString q_name = QString::fromLatin1(name);
        if (_optionToolTipsByProperty.contains(name))
        {
            return QCoreApplication::translate(_translationContext, _optionToolTipsByProperty.value(q_name).toLatin1().constData() );
        }
        return QString();
    }

    QString getOptionToolTip(const char* name, QVariant value)
    {
        QString q_name = QString::fromLatin1(name);
        if (_optionToolTipsByValue.contains(q_name))
        {
            auto& propertyToolTips = _optionToolTipsByValue[q_name];
            if (propertyToolTips.contains(value))
            {
                return QCoreApplication::translate(_translationContext, propertyToolTips.value(value).toLatin1().constData() );
            }
        }
        return QString();
    }

    QString getOptionText(const char* name)
    {
        QString q_name = QString::fromLatin1(name);
        if (_optionTextByProperty.contains(name))
        {
            return QCoreApplication::translate(_translationContext, _optionTextByProperty.value(q_name).toLatin1().constData() );
        }
        return QString();
    }

    QString getOptionText(const char* name, QVariant value)
    {
        QString q_name = QString::fromLatin1(name);
        if (_optionTextByValue.contains(q_name))
        {
            auto& propertyText = _optionTextByValue[q_name];
            if (propertyText.contains(value))
            {
                return QCoreApplication::translate(_translationContext, propertyText.value(value).toLatin1().constData() );
            }
        }
        return QString();
    }

public slots: 
    void select();

signals: 
    void selectionChanging(bool willBeSelected);
    void selectionChanged(bool selected);

    void cursorHintChanging();
    void cursorHintChanged();

protected:
    void initialize_p(IDocumentPresenter* documentPresenter);
    IDocumentPresenter* _documentPresenter;
    IViewPortPresenter* _viewPortPresenter;

    QIcon _icon;
    QCursor _defaultCursor;
    QCursor _engagedCursor;

    QString _toolTip;
    QString _name;

    template<typename EnumType>
    void initializeEnumOptionText(const char* propertyName)
    {
        //todo: assert enum type

        QMetaEnum metaEnum = QMetaEnum::fromType<EnumType>();
        QString q_propertyName = QString::fromLatin1(propertyName);

        _optionTextByProperty[q_propertyName] = q_propertyName;
        _optionToolTipsByProperty[q_propertyName] = QStringLiteral("%1-tooltip")
            .arg(q_propertyName);
        
        auto& textByValue = _optionTextByValue[q_propertyName];
        auto& toolTipByValue = _optionToolTipsByValue[q_propertyName];

        for (int index = 0; index < metaEnum.keyCount(); index++)
        {
            EnumType value = static_cast<EnumType>(metaEnum.value(index));

            textByValue[value] = QStringLiteral("%1-%2")
                .arg(q_propertyName)
                .arg(QString::fromLatin1(metaEnum.valueToKey(value)));
            toolTipByValue[value] = QStringLiteral("%1-%2-tooltip")
                .arg(q_propertyName)
                .arg(QString::fromLatin1(metaEnum.valueToKey(value)));
        }
    }

    template<typename IdType>
    void initializeIdOptionText(const char* propertyName, const QList<IdType>& values)
    {
        QString q_propertyName = QString::fromLatin1(propertyName);
        _optionTextByProperty[q_propertyName] = q_propertyName;
        _optionToolTipsByProperty[q_propertyName] = QStringLiteral("%1-tooltip")
            .arg(q_propertyName);

        auto& textByValue = _optionTextByValue[q_propertyName];
        auto& toolTipByValue = _optionToolTipsByValue[q_propertyName];

        for (const IdType& value : values)
        {
            textByValue[value] = QStringLiteral("%1-%2")
                .arg(q_propertyName)
                .arg(QString::fromLatin1(value.getName()));
            toolTipByValue[value] = QStringLiteral("%1-%2-tooltip")
                .arg(q_propertyName)
                .arg(QString::fromLatin1(value.getName()));
        }
    }

    QHash<QString, QIcon> _optionIconsByProperty;
    QHash<QString, QHash<QVariant, QIcon>> _optionIconsByValue;
    QHash<QString, QString> _optionToolTipsByProperty;
    QHash<QString, QHash<QVariant, QString>> _optionToolTipsByValue;
    QHash<QString, QString> _optionTextByProperty;
    QHash<QString, QHash<QVariant, QString>> _optionTextByValue;

    const char* _translationContext = "toolpresenterbase";

    virtual void onPointerEngage() { }
    virtual void onPointerMove() { }
    virtual void onPointerDisengage() { }

    ToolPathHelper _toolPathHelper;

private:
    InitializeHelper<ToolPresenterBase> _initHelper;
};

#endif // TOOLTOOLPRESENTER_HPP