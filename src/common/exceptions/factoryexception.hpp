#ifndef FACTORYEXCEPTION_HPP
#define FACTORYEXCEPTION_HPP

#include <QObject>
#include <QMetaEnum>

#include "addleexception.hpp"
// #include "idtypes/addleid.hpp"

namespace Addle {
    
class FactoryException : public AddleException
{
    Q_GADGET
    ADDLE_EXCEPTION_BOILERPLATE(FactoryException)
public:
    static constexpr bool IsLogicError = true;
    enum Why
    {
        FactoryNotFound
    };
    Q_ENUM(Why)
    
#ifdef ADDLE_DEBUG 
public:
    FactoryException(Why why, const char* interface = nullptr)
        : AddleException(
            //% "A factory error occurred.\n"
            //% "       why: %1\n"
            //% " interface: %2\n"
            //% "        id: %3\n"
            qtTrId("debug-messages.factory-exception")
                .arg(QMetaEnum::fromType<Why>().valueToKey(why))
                .arg(interface)
//                 .arg(id.key())
        ),
        _interface(interface), _why(why)
    {
    }
private:
    const char* _interface;
#else
public:
    FactoryException(Why why)
        : _why(why)
    {
    }   
#endif
private: 
    const Why _why;
};
    
} // namespace Addle

#endif // FACTORYEXCEPTION_HPP
