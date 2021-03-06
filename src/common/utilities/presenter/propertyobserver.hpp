/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PROPERTYOBSERVER_HPP
#define PROPERTYOBSERVER_HPP

#include "compat.hpp"
#include <QObject>
#include <QVariant>
#include <QMetaObject>
namespace Addle {

// TODO: merge wtih PropertyBinding
class ADDLE_COMMON_EXPORT PropertyObserver : public QObject
{
    Q_OBJECT
public: 
    PropertyObserver(QObject* target, const char* propertyName, QObject* parent = nullptr);
    virtual ~PropertyObserver() = default;

    inline bool isValid() { return _propertyIndex != -1; }

    QVariant get() { return isValid() ? _target->property(_propertyName) : QVariant(); }

signals:
    void changed(QVariant value);

public slots:
    void set(QVariant value) { if (isValid()) _target->setProperty(_propertyName, value); }

private slots:
    void onChange();

private:
    QObject* _target;
    const QMetaObject* _metaTarget;

    const char* _propertyName;
    int _propertyIndex;
};
} // namespace Addle

#endif // PROPERTYOBSERVER_HPP