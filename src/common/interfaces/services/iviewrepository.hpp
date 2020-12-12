#ifndef IVIEWREPOSITORY_HPP
#define IVIEWREPOSITORY_HPP

#include <list>
#include <tuple>

#include <boost/type_traits/is_detected_convertible.hpp>

#include <QMultiHash>

#include "interfaces/traits.hpp"

#include "utilities/collections.hpp"
#include "utilities/lazyvalue.hpp"
#include "utilities/qobject.hpp"

namespace Addle {
namespace detail { namespace view {
    
template<class View>
using presenter_arg_is_reference = std::is_same<
    typename std::remove_const<
        typename std::tuple_element<0, typename Traits::init_params<View>::type>::type
    >::type,
    typename View::PresenterType&
>;

template<class View>
using presenter_arg_is_shared_pointer = std::is_convertible<
    typename std::tuple_element<0, typename Traits::init_params<View>::type>::type,
    QSharedPointer<typename View::PresenterType>
>;

template<class View>
using presenter_arg_t = typename std::enable_if<
    std::tuple_size<typename Traits::init_params<View>::type>::value == 1
    && (presenter_arg_is_reference<View>::value || presenter_arg_is_shared_pointer<View>::value),
    typename std::tuple_element<0, typename Traits::init_params<View>::type>::type
>::type;

template<class View>
using presenter_key_t = typename std::conditional<
    presenter_arg_is_reference<View>::value,
    typename View::PresenterType*,
    QSharedPointer<typename View::PresenterType>
>::type;
    
}}

template<class View>
class IViewRepository 
{
    static_assert(
        Traits::implemented_as_QObject<View>::value,
        "IViewRepository is only supported for views implemented as QObject"
    );
    
    template<class View_>
    using presenter_getter_t = decltype(std::declval<View_>().presenter());
    
protected:
    typedef typename View::PresenterType PresenterType;
    typedef detail::view::presenter_arg_t<View> presenter_arg_t;
    typedef QMultiHash<detail::view::presenter_key_t<View>, InterfacePointer<View>> data_t;
    
public:
    class Iterator
    {
        typedef typename data_t::const_iterator inner_t;
        
    public:
        Iterator() = default;
        Iterator(const Iterator&) = default;
        Iterator& operator=(const Iterator&) = default;
        
        inline View& operator*() const { return **_inner; }
        inline View* operator->() const { return (*_inner).data(); }
        
        inline View& view() const { return **_inner; }
        inline InterfacePointer<View> viewPointer() const { return *_inner; }
        inline presenter_arg_t presenter() const { return presenter_impl<View>(); }
        
        inline Iterator& operator++() { ++_inner; return *this; }
        inline Iterator operator++(int) { Iterator(_inner++); }
        
        inline Iterator& operator--() { --_inner; return *this; }
        inline Iterator operator--(int) { Iterator(_inner--); }
        
        inline bool operator==(const Iterator& other) const { return _inner == other._inner; }
        inline bool operator!=(const Iterator& other) const { return _inner != other._inner; }
        
    private:
        Iterator(inner_t inner)
            : _inner(inner)
        {
        }
        
        template<class View_>
        inline typename std::enable_if<
            detail::view::presenter_arg_is_reference<View_>::value,
            PresenterType&
        >::type presenter_impl() const
        {
            return *_inner.key();
        }
        
        template<class View_>
        inline typename std::enable_if<
            detail::view::presenter_arg_is_shared_pointer<View_>::value,
            QSharedPointer<PresenterType>
        >::type presenter_impl() const
        {
            return _inner.key();
        }
        
        inner_t _inner;
        
        friend class IViewRepository;
    };
    
    virtual ~IViewRepository() = default;
    
    inline View& get(presenter_arg_t presenter) const
    {
        return get_impl<View>(presenter);
    }
    
    inline View& get(presenter_arg_t presenter)
    {
        if (!contains(presenter))
            add(presenter);
        return get_impl<View>(presenter);
    }
    
    inline View& operator[](presenter_arg_t presenter)
    {
        return get(presenter);
    }
    
    inline bool contains(presenter_arg_t presenter)
    {
        return contains_impl<View>(presenter);
    }
    
    inline int count() const
    {
        return data_p().count();
    }
    
    inline int count(presenter_arg_t presenter)
    {
        return count_impl<View>(presenter);
    }
    
    inline Iterator begin() const { return Iterator(data_p().begin()); }
    inline Iterator end() const { return Iterator(data_p().end()); }
    
    inline Iterator find(presenter_arg_t presenter) const
    {
        return find_impl<View>(presenter);
    }
    
    // inline cpplinq::Yield<View&> findAll(presenter_arg_t presenter) const
    
    presenter_arg_t presenter(View& view) const
    {
        return presenter_impl(view);
    }
    
    virtual void add(presenter_arg_t presenter) = 0;
    virtual void remove(presenter_arg_t presenter) = 0;
    
    
protected:
    virtual const data_t& data_p() const = 0;
    
private:
    template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_reference<View_>::value,
        View_&
    >::type get_impl(PresenterType& presenter) const
    {
        return *data_p()[&presenter];
    }
    
    template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_shared_pointer<View_>::value,
        View_&
    >::type get_impl(const QSharedPointer<PresenterType>& presenter) const
    {
        return *data_p()[presenter];
    }
    
    template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_reference<View_>::value,
        Iterator
    >::type find_impl(PresenterType& presenter) const
    {
        return Iterator(data_p().find(&presenter));
    }
    
        template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_shared_pointer<View_>::value,
        View_&
    >::type find_impl(const QSharedPointer<PresenterType>& presenter) const
    {
        return Iterator(data_p().find(presenter));
    }
    
    template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_reference<View_>::value,
        bool
    >::type contains_impl(PresenterType& presenter) const
    {
        return data_p().contains(&presenter);
    }
    
    template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_shared_pointer<View_>::value,
        bool
    >::type contains_impl(const QSharedPointer<PresenterType>& presenter) const
    {
        return data_p().contains(presenter);
    }
    
    template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_reference<View_>::value,
        int
    >::type count_impl(PresenterType& presenter) const
    {
        return data_p().count(&presenter);
    }
    
    template<class View_>
    inline typename std::enable_if<
        detail::view::presenter_arg_is_shared_pointer<View_>::value,
        int
    >::type count_impl(const QSharedPointer<PresenterType>& presenter) const
    {
        return data_p().count(presenter);
    }
    
    template<class View_>
    inline typename std::enable_if<
        boost::is_detected_convertible<presenter_arg_t, presenter_getter_t, View_>::value,
        presenter_arg_t
    >::type presenter_impl(View_& view) const 
    {
        presenter_arg_t presenter = view.presenter();
        // assert contains
        return presenter;
    }
    // if presenter method not detected, do a linear search
    
};

namespace Traits
{
    template<typename Arg> struct is_singleton_gettable<IViewRepository<Arg>> : std::true_type {};
    template<typename Arg> struct is_service<IViewRepository<Arg>> : std::true_type {};
}

}

#endif // IVIEWREPOSITORY_HPP
