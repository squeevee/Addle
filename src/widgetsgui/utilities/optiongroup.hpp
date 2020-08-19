/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef OPTIONGROUP_HPP
#define OPTIONGROUP_HPP

#include <QActionGroup>
#include <QAction>

#include "optionaction.hpp"
#include "utilities/indexvariant.hpp"

namespace Addle {

// Keeps an (exclusive) QActionGroup and associates its (checkable) contents to
// QVariant values. The group's collective value corresponds to the value
// associated to the selected action.
class OptionGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        OptionAction* selectedOption
        READ selectedOption
        WRITE selectOption
        NOTIFY selectedOptionChanged
    )
    Q_PROPERTY(
        QVariant value
        READ value 
        WRITE selectValue 
        NOTIFY valueChanged
    )
    Q_PROPERTY(
        bool enabled
        READ isEnabled 
        WRITE setEnabled
    )
public:
    OptionGroup(QObject* parent = nullptr);
    virtual ~OptionGroup() = default;

    void addOption(OptionAction* option);

    QList<QVariant> values() const { return _valuesByOption.values(); }
    QList<OptionAction*> options() const { return _valuesByOption.keys(); }
    QHash<IndexVariant, OptionAction*> optionsByValue() const { return _optionsByValue; }

    OptionAction* selectedOption() const { return _selectedOption; }
    void selectOption(OptionAction* option);

    QVariant value() const { return _selectedValue; }
    void selectValue(QVariant value);

    bool isEnabled() const { return _group->isEnabled(); }
    void setEnabled(bool enabled) { _group->setEnabled(enabled); }

signals:
    void selectedOptionChanged(OptionAction* option);
    void valueChanged(QVariant value);

private slots: 
    void onActionTriggered(QAction* action);

private:
    OptionAction* _selectedOption = nullptr;
    QVariant _selectedValue;

    QHash<IndexVariant, OptionAction*> _optionsByValue;
    QHash<OptionAction*, QVariant> _valuesByOption;

    QActionGroup* _group;
};

} // namespace Addle

#endif // OPTIONGROUP_HPP
