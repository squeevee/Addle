#include "enumactiongroup.hpp"
#include "utilities/qt_extensions/qobject.hpp"

OptionActionGroup::OptionActionGroup(QObject* parent, IToolPresenter& presenter, const char* propertyName)
    : QActionGroup(parent),
    _presenter(presenter),
    _propertyName(propertyName)
{
    _propertyObserver = new PropertyObserver(
        qobject_interface_cast(&presenter),
        _propertyName,
        this
    );

    connect(this, &QActionGroup::triggered, this, &OptionActionGroup::onActionTriggered);
}

QAction* OptionActionGroup::createAction(QVariant value)
{
    QAction* action = new QAction(this);

    _values[action] = value;
    _actions[value.toInt()] = action;

    action->setCheckable(true);
    action->setChecked(_propertyObserver->get() == value);
    action->setIcon(_presenter.getOptionIcon(_propertyName, value));
    action->setText(_presenter.getOptionText(_propertyName, value));
    action->setToolTip(_presenter.getOptionToolTip(_propertyName, value));

    QActionGroup::addAction(action);
    return action;
}

void OptionActionGroup::onActionTriggered(QAction* action)
{
    QVariant value = _values.value(action);
    _propertyObserver->set(value);
}

void OptionActionGroup::onPropertyChanged(QVariant value)
{
    _actions[value.toInt()]->setChecked(true);
}