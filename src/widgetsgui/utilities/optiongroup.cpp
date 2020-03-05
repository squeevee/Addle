#include "optiongroup.hpp"

OptionGroup::OptionGroup(QObject* parent)
    : QObject(parent)
{
    _group = new QActionGroup(this);
    _group->setExclusive(true);

    connect(_group, &QActionGroup::triggered, this, &OptionGroup::onActionTriggered);
}

void OptionGroup::addOption(OptionAction* option)
{
    IndexVariant index(option->getValue());
    //assert

    _valuesByOption[option] = option->getValue();
    _optionsByValue[index] = option;

    option->setCheckable(true);
    _group->addAction(option);
}

void OptionGroup::selectOption(OptionAction* option)
{
    if (_valuesByOption.contains(option))
    {
        _selectedOption = option;
        _selectedValue = _valuesByOption[option];
        
        option->setChecked(true);

        emit selectedOptionChanged(option);
        emit valueChanged(_selectedValue);
    }
}

void OptionGroup::selectValue(QVariant value)
{
    IndexVariant index(value);
    //Assert
    if (_optionsByValue.contains(index))
    {
        _selectedValue = value;
        _selectedOption = _optionsByValue[index];
        
        _selectedOption->setChecked(true);

        emit selectedOptionChanged(_selectedOption);
        emit valueChanged(value);
    }
}

void OptionGroup::onActionTriggered(QAction* action)
{
    selectOption(static_cast<OptionAction*>(action));
}

#include "moc_optionaction.cpp"