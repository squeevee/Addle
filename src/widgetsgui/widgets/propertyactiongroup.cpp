#include "propertyactiongroup.hpp"
#include "utilities/qt_extensions/qobject.hpp"

PropertyActionGroup::PropertyActionGroup(QObject* parent, IPropertyDecoratedPresenter& presenter, const char* propertyName)
    : QActionGroup(parent),
    _presenter(presenter),
    _propertyName(propertyName),
    _decoration(presenter.getPropertyDecoration(_propertyName))
{
    _propertyObserver = new PropertyObserver(
        qobject_interface_cast(&presenter),
        _propertyName,
        this
    );

    connect(this, &QActionGroup::triggered, this, &PropertyActionGroup::onActionTriggered);
}

QAction* PropertyActionGroup::createAction(QVariant value)
{
    QAction* action = new QAction(this);

    _values[action] = value;
    _actions[value] = action;

    action->setCheckable(true);
    action->setChecked(_propertyObserver->get() == value);

    if (!_decoration.isNull())
    {
        const OptionDecoration& optionDecoration = _decoration.getDecoration(value);

        action->setIcon(optionDecoration.getIcon());
        action->setText(optionDecoration.getText());
        action->setToolTip(optionDecoration.getToolTip());
    }

    QActionGroup::addAction(action);
    return action;
}

void PropertyActionGroup::onActionTriggered(QAction* action)
{
    QVariant value = _values.value(action);
    _propertyObserver->set(value);
}

void PropertyActionGroup::onPropertyChanged(QVariant value)
{
    _actions[value]->setChecked(true);
}