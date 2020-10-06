/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef PROPERTYBINDING_HPP
#define PROPERTYBINDING_HPP

#include "compat.hpp"
#include <QObject>
#include <functional>

#include "utilities/qobject.hpp"
#include "propertyobserver.hpp"
namespace Addle {

class ADDLE_COMMON_EXPORT BindingConverter
{
public:
    BindingConverter() = default;
    BindingConverter(const BindingConverter& other) = default;
    BindingConverter(std::function<QVariant(QVariant)> in, std::function<QVariant(QVariant)> out)
        : _in(in), _out(out)
    {
    }

    inline bool hasIn() const { return (bool)_in; }
    inline bool hasOut() const { return (bool)_out; }
    inline bool isNull() const { return !_in && !_out; }

    template<typename T = double>
    static BindingConverter scale(double coeff)
    {
        return BindingConverter(
            [coeff] (QVariant in) -> QVariant { return QVariant(in.value<T>() * coeff); },
            [coeff] (QVariant out) -> QVariant { return QVariant(out.value<T>() / coeff); }
        );
    }

    static BindingConverter negate()
    {
        const auto func = [] (QVariant a) -> QVariant { return QVariant(!a.toBool()); };
        return BindingConverter(func, func);
    }

private:
    std::function<QVariant(QVariant)> _in;
    std::function<QVariant(QVariant)> _out;

    friend class PropertyBinding;
};


/**
 * @class PropertyBinding
 * @brief Binds the value of one QObject property to another
 */
// TODO: error/warning on invalid property names
class ADDLE_COMMON_EXPORT PropertyBinding : public QObject
{
    Q_OBJECT
public:
    typedef BindingConverter Converter;

    enum Direction
    {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

    static constexpr const char* BINDINGS_PROPERTY_NAME = "_bindings";

    PropertyBinding(
        QObject* local,
        const char* localProperty,
        QObject* target,
        const char* targetProperty,
        Direction direction = ReadWrite,
        const Converter& converter = Converter());
    
    template<class Interface>
    PropertyBinding(
        Interface* local,
        const char* localProperty,
        QObject* target,
        const char* targetProperty,
        Direction direction = ReadWrite,
        const Converter& converter = Converter())
        : PropertyBinding(
            qobject_interface_cast<QObject*>(local),
            localProperty,
            target,
            targetProperty,
            direction,
            converter)
    {
    }
    
    template<class Interface>
    PropertyBinding(
        QObject* local,
        const char* localProperty,
        Interface* target,
        const char* targetProperty,
        Direction direction = ReadWrite,
        const Converter& converter = Converter())
        : PropertyBinding(
            local,
            localProperty,
            qobject_interface_cast<QObject*>(target),
            targetProperty,
            direction,
            converter)
    {
    }

    virtual ~PropertyBinding();

    void enable();
    void disable();

    inline bool isValid() const { return (reads() && _inConnection) || (writes() && _outConnection); }

    //async safety?
    void setConverter(Converter converter) { _converter = converter; }

private slots:
    void onTargetValueChanged(QVariant value);
    void onLocalValueChanged(QVariant value);

    void onTargetDestroyed();

private:
    inline bool reads() const { return _direction == ReadOnly || _direction == ReadWrite; }
    inline bool writes() const { return _direction == WriteOnly || _direction == ReadWrite; }

    const Direction _direction;
    
    Converter _converter;

    QObject* _local;
    QObject* _target;

    PropertyObserver* _inObserver = nullptr;
    PropertyObserver* _outObserver = nullptr;

    QMetaObject::Connection _inConnection;
    QMetaObject::Connection _outConnection;

    const char* _localProperty;
    const char* _targetProperty;
};
} // namespace Addle

#endif // PROPERTYBINDING_HPP
