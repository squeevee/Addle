#include <QSet>
#include "propertybinding.hpp"

using namespace Addle;

PropertyBinding::PropertyBinding(
        QObject* local,
        const char* localProperty,
        QObject* target,
        const char* targetProperty,
        Direction direction,
        const Converter& converter)
        : _local(local),
        _target(target),
        _localProperty(localProperty),
        _targetProperty(targetProperty),
        _direction(direction),
        _converter(converter)
{
    setParent(local);

    {
        QSet<QObject*> bindings;
        QVariant bindingsProperty = _local->property(BINDINGS_PROPERTY_NAME);
        if (bindingsProperty.isValid())
            bindings = bindingsProperty.value<QSet<QObject*>>();
        bindings.insert(this);
        _local->setProperty(BINDINGS_PROPERTY_NAME, QVariant::fromValue(bindings));
    }
    
    connect(_target, &QObject::destroyed, this, &PropertyBinding::onTargetDestroyed);

    if (reads())
    {
        _inObserver = new PropertyObserver(_target, _targetProperty, this);

        QVariant value = _inObserver->get();

        if (_converter._in)
            value = _converter._in(value);

        _local->setProperty(_localProperty, value);
    }
    if (writes())
    {
        _outObserver = new PropertyObserver(_local, _localProperty, this);
    }

    enable();
}

PropertyBinding::~PropertyBinding()
{
    //assumes _local is the parent of this and therefore isn't deleted.

    QSet<QObject*> bindings;
    QVariant bindingsProperty = _local->property(BINDINGS_PROPERTY_NAME);
    if (bindingsProperty.isValid())
        bindings = bindingsProperty.value<QSet<QObject*>>();
    bindings.remove(this);
    _local->setProperty(BINDINGS_PROPERTY_NAME, QVariant::fromValue(bindings));
}

void PropertyBinding::enable()
{
    if (reads() && !_inConnection)
    {
        _inConnection = connect(
            _inObserver,
            &PropertyObserver::changed,
            this,
            &PropertyBinding::onTargetValueChanged
        );
    }

    if (writes() && !_outConnection)
    {
        _outConnection = connect(
            _outObserver,
            &PropertyObserver::changed,
            this,
            &PropertyBinding::onLocalValueChanged
        );
    }
}

void PropertyBinding::disable()
{
    if (_inConnection)
    {
        QObject::disconnect(_inConnection);
        _inConnection = QMetaObject::Connection();
    }

    if (_outConnection)
    {
        QObject::disconnect(_outConnection);
        _outConnection = QMetaObject::Connection();
    }
}

void PropertyBinding::onTargetValueChanged(QVariant value)
{
    // Assert reads

    if (_converter._in)
        value = _converter._in(value);

    if (_outObserver)
        _outObserver->blockSignals(true);

    _local->setProperty(_localProperty, value);

    if (_outObserver)
        _outObserver->blockSignals(false);
}

void PropertyBinding::onLocalValueChanged(QVariant value)
{
    // Assert writes

    if (_converter._out)
        value = _converter._out(value);

    if (_inObserver)
        _inObserver->blockSignals(true);

    _target->setProperty(_targetProperty, value);

    if (_inObserver)
        _inObserver->blockSignals(false);
}

void PropertyBinding::onTargetDestroyed()
{
    _target = nullptr;
    disable();
}