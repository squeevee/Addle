#include "propertyobserver.hpp"

#include <QMetaMethod>
#include <QMetaProperty>

PropertyObserver::PropertyObserver(QObject* target, const char* propertyName, QObject* parent)
    : QObject(parent), _target(target), _propertyName(propertyName)
{
    _metaTarget = _target->metaObject();
    _propertyIndex = _metaTarget->indexOfProperty(_propertyName);

    if (isValid())
    {
        int notifySignalIndex = _metaTarget->property(_propertyIndex).notifySignalIndex();
        int onChangeSlotIndex = this->metaObject()->indexOfSlot("onChange()");

        if (notifySignalIndex == -1 || onChangeSlotIndex == -1)
            return;

        connect(
            _target,
            _metaTarget->method(notifySignalIndex),
            this,
            this->metaObject()->method(onChangeSlotIndex)
        );
    }
}

void PropertyObserver::onChange()
{
    emit changed(get());
}