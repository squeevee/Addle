
# Interfaces guide

Addle's extensive use of interfaces is one of the more unusual aspects of its
design compared to other Qt projects I've encountered. I anticipate the need for 
some explanation of how they work and how to use them.

For context, in Addle, certain classes have separated interfaces and
implementations. Interfaces refer to pure abstract classes defined in
"src/common/interfaces", and the implementations are located variously in
"src/core" etc. As a rule, code should refer to the interface rather than the
implementation.

For more context see the "Dependency inversion" section of
[Overview.md](./Overview.md).

The first thing to note is that the separation of interface and implementation
is only required for select classes.

**Yes interface required:**

- "Domain classes": any class that is primarily responsible for some
functionality and/or state that is significant to Addle's domain. This includes 
all Models and Presenters, though exception is given to some Views (see the
"Views" section below).
- Helper interfaces: some domain interfaces define one or more secondary 
interfaces. A user of the class might be expected to implement the secondary 
interface so

**No interface required:**

- Classes that hold data but only perform a trivial amount of logic.
- "Utility classes", including ones that perform a non-trivial amount of logic, 
but do so within a generic or highly reusable context. These must not depend on
any implementations.
- "Helper classes" that are owned and used by implementations to perform a 
reusable or relativley segregated task. These may perform domain functionality, 
and own domain state on behalf of an implementation. These must not depend on
any implementation.
- Classes provided by STL, Qt, or Boost (most of which fall under one of the
first two camps).
- Classes and structs whose creation or access are in performance-critical
scenarios. (These should probably fall into one of the first two camps anyway).
- Most UI components (again see the "Views" section below).

## ServiceLocator

The service locator pattern, provided in the global class ServiceLocator,
is used to acquire instances of desired interface types. The name refers to its
ability to acquire (locate) "Services", which are singleton-like objects that
provide some global functionality or own global state.

```c++
#include "servicelocator.hpp"

// concrete reference
Freem::instance().serviceMethod();

// interface reference
ServiceLocator::get<IFreem>().serviceMethod();
```

In Addle, the ServiceLocator is combined with a generic Abstract Factory, and
can be used to construct new objects of various interface types.

```c++
#include "servicelocator.hpp"

// concrete reference
Spiff* spiff = new Spiff();

// interface reference
ISpiff* spiff = ServiceLocator::make<ISpiff>();
```

Service locator is configured at the beginning of runtime to map interface types
to implementation factories. Only *it* needs to know about these mappings, the
rest of the code can just go about using the interfaces.

The interfaces which are allowed to be "gotten" or "made" are marked with
respective traits ("interfaces/traits.hpp"). You will see the macro
DECL_MAKEABLE in the header that defines the interface if it is meant for use
with `ServiceLocator::make<...>()`, and DECL_SERVICE or
DECL_PERSISTENT_OBJECT if it is meant for use with
`ServiceLocator::get<...>()`

Some interfaces need to be initialized with some data when they are made,
analogous to calling a constructor with arguments. In these cases, the interface
will be marked with DECL_EXPECTS_INITIALIZE in its header. The interface will
also expose one or more void member functions called `initialize`.
`ServiceLocator::make()` will accept arguments, call `initialize`
and automatically pass along the arguments.

```c++
ISpiff* spiff = ServiceLocator::make<ISpiff>(QString("Hello world"), 99);

// is essentially equivalent to
ISpiff* spiff = new Spiff();
spiff->initialize(QString("Hello world"), 99);
```

Note that due to C++ arugment type inference rules, if an initialize function
calls for a reference, you will need to wrap that argument in `std::ref()`
when calling `ServiceLocator::make()`, or you'll get "cannot convert" build
errors.

```c++
#include <functional>

ISpiff* spiff = ServiceLocator::make<ISpiff>(std::ref(*_freem));
```

Also note that you will need to `#include` the header that defines the desired 
interface before calling a ServiceLocator function on it. Calling ServiceLocator 
functions on an incomplete type results in build errors (with unhelpful messages.)

## AddleId

Instances of AddleId can be used in conjunction with ServiceLocator to
differentiate between multiple implementations of the same interface.

For example a derived type of AddleId might be `FruitId` for which some
instances are globally defined. If `IFruit` is marked with the macro
`DECL_PERSISTENT_OBJECT(IFruit, FruitId)` then it can be gotten by instances of
`FruitId`.

```c++
namespace Fruits
{
    constexpr FruitId PeachId = 0x998;
    constexpr FruitId PearId  = 0x999;
}

IFruit& peach = ServiceLocator::get<IFruit>(Fruits::PeachId);
IFruit& pear = ServiceLocator::get<IFruit>(Fruits::PearId);
```

Peach and Pear might have totally different implementations, or they might be
the same class with different initialized values.

## Interfaces implemented as QObject

For some interfaces, in addition to pure virtual methods, it is desirable to 
also expose access to QObject features, such as signals, slots, and
properties. It is not possible to virtually or multiply inherit QObject so
rather than interfaces directly deriving from QObject, they derive from a
special interface called IAmQObject, which promises that the implementation
will be derived from QObject.

In order to access the QObject, call qobject_interface_cast on the interface
object. Or use the convenience wrapper connect_interface to connect signals
or slots to an interface. The trait Traits::implemented_as_QObject will be true
for interfaces deriving from IAmQObject.

Interfaces deriving IAmQObject should also register themselves with Qt using
the Q_DECLARE_INTERFACE macro. Addle interfaces are identified with the D-Bus
style namespace "org.addle.\<interface name\>".

```c++
namespace Addle {

class ISpiff : public virtual IAmQObject
{
public:
    virtual ~ISpiff() = default;

    //...
};

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ISpiff, "org.addle.ISpiff")
```

A class implementing an IAmQObject interface should list the interface in the
Q_INTERFACES macro of its definition. It should also use the convenience macro
IAMQOBJECT_IMPL to provide IAmQObject functions.

```c++
namespace Addle {

class ADDLE_EXPORT_CORE Spiff : public QObject, public ISpiff
{
    Q_OBJECT
    Q_INTERFACES(Addle::ISpiff)
    IAMQOBJECT_IMPL
public:
    virtual ~Spiff() = default;

    //...
};

} // namespace Addle
```

For an interface, the `signals:` and `public slots:` labels are technically
meaningless and not processed by MOC -- but the pure virtual methods given under
them are hints of corresponding (overriding) methods in the implementation that
will be recognized by the Meta Object system using the same signature.

Note: signals and slots on an interface can only be connected by string and not
by functor. Errors in the signature will not be caught by the compiler.

Some interfaces -- mainly Presenters -- also expose a set of QObject Properties,
the names of which are accessible as the members of
`<interface name>::Meta::Properties`, and are declared using the macros 
DECL_INTERFACE_META_PROPERTIES, etc. This is a correctness-assuring
convenience for things like property bindings that operate on properties by
name. Regular getters and setters should be exposed by the interface and used in
all other contexts. 

## Views

See the "Model-Presenter-View" section of [Overview](./Overview.md).

Views are the major exception to the practice of only referencing interfaces and
not implementations. Most Views are implemented as component objects in some GUI 
framework -- i.e., (Aug 2020) `QWidget`s and `QGraphicsItem`s, but possibly more
in the future. The respective APIs require access to objects of these component
types in order to function. Creating interfaces that are abstract and flexible,
but also efficiently interoperate with the necessary frameworks, rapidly 
becomes far too complex to be worth the trouble.

The few Views that need to be exposed to other components of the code, e.g., the
Main Editor which is created by its presenter, have proper interfaces for the
purposes of being accessed from other modules. Beyond this, Views are
responsible for creating each other and providing each other with access to
Presenters. And thus the rest of Addle doesn't even need to know what Views
exist or what they do.

This is a less strict interpretation of Dependency Inversion, but sufficient for
our purposes.