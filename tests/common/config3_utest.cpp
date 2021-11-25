#include <QtTest/QtTest>
#include <QObject>

#include <boost/di.hpp>

#include "utilities/config3/interfacebindingconfig.hpp"
#include "utilities/config3/factory.hpp"
#include "utilities/config3/repository.hpp"

#include "utilities/coordinatedreadwritelock.hpp"

#include "exceptions/addleexception.hpp"
#include "utilities/config3/bindingcondition.hpp"
#include "utilities/config3/interfacebindingconfig.hpp"
#include "utilities/config3/moduleconfig.hpp"

namespace Addle {
class IA
{
public:
    virtual ~IA() = default;
    virtual QString s() const = 0;
    virtual void setS(QString) = 0;
};

namespace aux_IA {
    ADDLE_FACTORY_PARAMETER_NAME(s)
    ADDLE_FACTORY_PARAMETER_NAME(q)
}

ADDLE_DECL_SERVICE(IA)

// ADDLE_DECL_MAKEABLE(IA);
// ADDLE_DECL_FACTORY_PARAMETERS(IA,
//     (optional 
//         ( s, (QString), QString() )
//     )
// )

class IB
{
public:    
    virtual ~IB() = default;
    virtual QByteArray v() const = 0;
    virtual IA& a() = 0;
    virtual const IA& a() const = 0;
};

namespace aux_IB {
    ADDLE_FACTORY_PARAMETER_NAME(v)
}

ADDLE_DECL_MAKEABLE(IB);
ADDLE_DECL_FACTORY_PARAMETERS(IB,
    (optional
        ( v, (QByteArray), "spiff")
    )
)

template<>
struct Traits::repo_hints<IB>
{
//     static constexpr bool infer_defaults = false;
//     using key_type = int;
    static constexpr auto key_getter = &IB::v;
};

class IC
{
public:
    virtual ~IC() = default;
    virtual void _() const = 0;
};

ADDLE_DECL_SERVICE(IC);

class D {};

class A : public IA, public QEnableSharedFromThis<A>
{
public:
    A(IB* b)
    {
        qDebug() << b->v();
    }
    
    virtual ~A() = default;
    QString s() const override { return _s; }
    void setS(QString s) override { _s = s; }
    
private:
    QString _s;
};

class B : public IB
{
public:
    B(QByteArray v) : _v(v) {}
    
    virtual ~B() = default;
    QByteArray v() const override { return _v; }
    IA& a() override { Q_UNREACHABLE(); }
    const IA& a() const override { Q_UNREACHABLE(); }
    
private:
    QByteArray _v;
};

// class B2 : public IB
// {
// public:
//     B2(IA& a, QByteArray b) :_a(a) { assert(b == "gorz"); }
//     
//     virtual ~B2() = default;
//     int v() const override { return 2; }
//     IA& a() override { return _a; }
//     const IA& a() const override { return _a; }
//     
// private:
//     IA& _a;
// };
}

Q_DECLARE_INTERFACE(Addle::IB, "org.addle.testing.IB");

// Q_DECLARE_METATYPE(QSharedPointer<Addle::IA>);
Q_DECLARE_METATYPE(QSharedPointer<Addle::IB>);

using namespace Addle;

class Config3_UTest : public QObject
{
    Q_OBJECT
private:
    QTranslator fallbackTranslator;
        
private slots:
    void initTestCase()
    {
        fallbackTranslator.load(":/l10n/en_US.qm");
        QCoreApplication::instance()->installTranslator(&fallbackTranslator);
        
//         qRegisterMetaType<QSharedPointer<Addle::IA>>();
        qRegisterMetaType<QSharedPointer<Addle::IB>>();
        
//         qRegisterMetaType<Addle::aux_config3::GenericRepoMemberRef>();
//         qRegisterMetaType<Addle::aux_config3::GenericRepoMemberConstRef>();
    }
    
    void dev_1()
    {
        using namespace boost::di;
        using namespace boost::mp11;
        using namespace aux_config3;
        
        InterfaceBindingConfig config;
        
        {
            auto mc = config.addModule("test");
            mc.addBindings(
                    bind<IA>.to<A>(),
                    bind<IB>.to<B>()
                );
            
            mc.commit();
        }
        
        auto& a = *di_runtime_bindings_traits<InterfaceBindingConfig>::get_service<IA>(&config);
        
    }
};

QTEST_MAIN(Config3_UTest)

#include "config3_utest.moc"
