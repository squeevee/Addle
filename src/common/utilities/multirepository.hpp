#ifndef MULTIREPOSITORY_HPP
#define MULTIREPOSITORY_HPP

// #include "interfaces/services/irepository.hpp"

#include <boost/mpl/vector.hpp>
#include <boost/mpl/transform.hpp>

#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/container/vector.hpp>

#include <boost/variant.hpp>

/**
 * A MultiRepository is a conjoined view of multiple IRepository for different
 * interfaces. 
 */

// namespace Addle {
// 
// namespace detail { namespace config {
//     
// template<class Interface>
// struct _wrap_repository
// {
//     typedef const IRepository<Interface>& type;
// };
// 
// template<typename TypeSequence>
// class MultiRepositoryImpl
// {
// public:
//     typedef TypeSequence types;
//     static_assert(
//         boost::mpl::is_sequence<TypeSequence>::value,
//         "TypeSequence must be an MPL-compatible type sequence."
//     );
//     
//     typedef typename boost::fusion::result_of::as_vector<
//         typename boost::mpl::transform<
//             types,
//             _wrap_repository<boost::mpl::placeholders::_1>
//         >::type
//     >::type repos_t;
//     
//     typedef typename boost::make_variant_over<
//         typename boost::mpl::transform<
//             types,
//             std::add_lvalue_reference<boost::mpl::placeholders::_1>
//         >::type
//     >::type value_t;
//     
//     MultiRepositoryImpl(const repos_t& repos)
//         : _repos(repos)
//     {
//     }
//     
// private:
//     repos_t _repos;
// };
// 
// }} // namespace detail::config
// 
// template<typename... Interfaces>
// using MultiRepository = detail::config::MultiRepositoryImpl<boost::mpl::vector<Interfaces...>>;
// 
template<typename Sequence>
struct make_multirepository_over
{
//     typedef detail::config::MultiRepositoryImpl<Sequence> type;
    struct type { using value_t = int; }; // HACK
};
// 
// } // namespace Addle

#endif // MULTIREPOSITORY_HPP
