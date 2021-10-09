#include <QtTest/QtTest>
#include <QObject>

#include <boost/di.hpp>

#include "utilities/config3/addleconfig.hpp"
#include "utilities/config3/factory.hpp"

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
}

ADDLE_DECL_MAKEABLE(IA);
ADDLE_DECL_FACTORY_PARAMETERS(IA,
    (optional ( s, (QString), "spiff" ))
)

class IB
{
public:
    virtual ~IB() = default;
    virtual int v() const = 0;
    virtual const IA& a() const = 0;
};

namespace aux_IB {
    ADDLE_FACTORY_PARAMETER_NAME(v)
}

ADDLE_DECL_SERVICE(IB);

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
    A(IB&)
        //: _s(s)
    {
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
// //     B(A* a, int v)
//     BOOST_DI_INJECT(B, IA* a, (named = aux_IB::v_) int v)
//         : _a(a), _v(v)
//     {
//     }
    
    B() = default;
    
    virtual ~B() = default;
    int v() const override { return _v; }
    const IA& a() const override { return *_a; }
    
private:
    std::unique_ptr<IA> _a;
    int _v;
};


}

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
    }
    
    void dev_1()
    {
        using namespace aux_config3;
        using namespace boost::di;
        using namespace boost::mp11;
        
        AddleConfig cfg;
        cfg.addInjector( bind<IA>().to<A>() );
        cfg.addInjector( bind<IB>().to<B>() );
        
        auto f = Factory<IA>(cfg);
        for (int i = 0; i < 100; ++i)
            delete f.make();
        
        auto g = Factory<A>(cfg);
        for (int i = 0; i < 100; ++i)
            delete g.make();
    }
};

QTEST_MAIN(Config3_UTest)

#include "config3_utest.moc"
