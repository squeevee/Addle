#ifndef METAOBJECTINFO_HPP
#define METAOBJECTINFO_HPP

/**
 * These macros and classes expose information from interfaces about their
 * QObject implementations. This serves as:
 * - A promise at the interface level of underlying Qt meta-object features.
 *   (note: these promises are not enforced.)
 * - A measure to use the compiler for error checking when handling meta-
 *   object features by name.
 * - "Markup" to help code generation scripts (i.e., for automated testing)
 *   produce mock implementations with the correct meta-object structure.
 */

namespace InterfaceMetaObjectInfo
{ 
    template <class Interface> struct Properties { };
    template <class Interface> struct Signals { };
    template <class Interface> struct Slots { };
    template <class Interface> struct Methods { };
}

/**
 * Place this macro in the interface class beneath the `public:` label to
 * provide a convenience accessor to meta info.
 */
#define INTERFACE_META(Interface) \
struct Meta \
{ \
    typedef InterfaceMetaObjectInfo::Properties<Interface> Properties; \
    typedef InterfaceMetaObjectInfo::Signals<Interface> Signals; \
    typedef InterfaceMetaObjectInfo::Slots<Interface> Slots; \
    typedef InterfaceMetaObjectInfo::Methods<Interface> Methods; \
};

#define DECL_INTERFACE_META_PROPERTIES(Interface, ...) \
namespace InterfaceMetaObjectInfo \
{ \
    template<> struct Properties<Interface> \
    { \
        __VA_ARGS__ \
    }; \
}

#define DECL_INTERFACE_PROPERTY(name) static constexpr const char* name = #name;

#define DECL_INTERFACE_META_SIGNALS(Interface, ...) \
namespace InterfaceMetaObjectInfo \
{ \
    template<> struct Signals<Interface> \
    { \
        __VA_ARGS__ \
    }; \
}

#define DECL_INTERFACE_SLOT(name, signature) static constexpr const char* name = #signature;

#define DECL_INTERFACE_META_SLOTS(Interface, ...) \
namespace InterfaceMetaObjectInfo \
{ \
    template<> struct Slots<Interface> \
    { \
        __VA_ARGS__ \
    }; \
}

#define DECL_INTERFACE_SIGNAL(name, signature) static constexpr const char* name = #signature;

#define DECL_INTERFACE_META_METHODS(Interface, ...) \
namespace InterfaceMetaObjectInfo \
{ \
    template<> struct Methods<Interface> \
    { \
        __VA_ARGS__ \
    }; \
}

#define DECL_INTERFACE_METHOD(name, signature) static constexpr const char* name = #signature;

#endif // METAOBJECTINFO_HPP