#pragma once

#include <unordered_map>
#include <stdexcept>
#include <cstring>
#include <cstdlib>  // std::strlen, std::memcpy
#include <optional>

#include <boost/mp11.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_detected_convertible.hpp>

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QMetaType>
#include <QSharedData>
#include <QAtomicInteger>

#include <QCoreApplication>

#include "interfaces/traits.hpp"
#include "utilities/ranges.hpp"
#include "utilities/qobject.hpp"
#include "utilities/coordinatedreadwritelock.hpp"
#include "utilities/metaprogramming.hpp"


#ifdef ADDLE_DEBUG
#include <QDebug> // operator<< for GenericRepoMemberRef_impl
#include "utilities/debugging/qdebug_extensions.hpp"
#include "utilities/debugging/debugstring.hpp"
#endif

#include "./configexceptions.hpp"
#include "./aux.hpp"
#include "./genericrepository.hpp"
#include "./factory.hpp"

// TODO: I'm using noexcept to indicate methods that don't throw "userspace" 
// exceptions but it's possible they could through exceptions associated with 
// dynamic memory. I should examine that case and make a decision of how I want
// to handle it.

namespace Addle {
namespace aux_config3 {

template<typename Interface>
using _repo_hint_general_infer_defaults_t = decltype(Traits::repo_hints<Interface>::infer_defaults);

template<typename Interface, typename Key>
using _repo_hint_specific_infer_defaults_t = decltype(Traits::repo_hints<Interface>::template for_<Key>::infer_defaults);

template<typename Interface, typename Key = void, 
    bool = boost::mp11::mp_valid<_repo_hint_specific_infer_defaults_t, Interface, Key>::value,
    bool = boost::mp11::mp_valid<_repo_hint_general_infer_defaults_t, Interface>::value
>
struct repo_infer_defaults_impl { static constexpr bool value = true; };

template<typename Interface, typename Key, bool _>
struct repo_infer_defaults_impl<Interface, Key, true, _>
{
    static constexpr bool value = Traits::repo_hints<Interface>::template for_<Key>::infer_defaults;
};

template<typename Interface, typename _>
struct repo_infer_defaults_impl<Interface, _, false, true>
{
    static constexpr bool value = Traits::repo_hints<Interface>::infer_defaults;
};

template<typename Interface>
using _repo_hint_key_type = typename Traits::repo_hints<Interface>::key_type;

template<typename Interface>
using _repo_hint_key_types = typename Traits::repo_hints<Interface>::key_types;

template<typename Interface>
using _interface_id_t = decltype(std::declval<const Interface>().id());

template<typename Interface>
using _repo_hint_general_key_getter_t = decltype(Traits::repo_hints<Interface>::key_getter);

template<typename Interface, typename Key, bool = boost::mp11::mp_valid<_repo_hint_general_key_getter_t, Interface>::value>
struct _repo_hint_general_key_getter_is_convertible
    : std::is_convertible<std::invoke_result_t<_repo_hint_general_key_getter_t<Interface>, const Interface&>, Key>
{
};

template<typename Interface, typename Key>
struct _repo_hint_general_key_getter_is_convertible<Interface, Key, false> : std:: false_type {};

template<typename Interface, typename Key>
using _repo_hint_specific_key_getter_t = decltype(Traits::repo_hints<Interface>::template for_<Key>::key_getter);

template<typename Interface, typename Key, 
    bool = boost::is_detected<_repo_hint_specific_key_getter_t, Interface, Key>::value,
    bool = _repo_hint_general_key_getter_is_convertible<Interface, Key>::value,
    bool = (
        repo_infer_defaults_impl<Interface, Key>::value
        && boost::is_detected_convertible<Key, _interface_id_t, Interface>::value
    )
>
struct repo_key_getter_impl
{
    static constexpr bool is_valid = false;
    
    struct invalid_getter_t {};
    static constexpr invalid_getter_t getter = {};
};

template<typename Interface, typename Key, bool _1, bool _2>
struct repo_key_getter_impl<Interface, Key, true, _1, _2>
{
    static constexpr bool is_valid = true;
    static constexpr auto& getter = Traits::repo_hints<Interface>::template for_<Key>::key_getter;
    
    static_assert(std::is_convertible_v<std::invoke_result_t<decltype(getter), const Interface&>, Key>);
};

template<typename Interface, typename Key, bool _>
struct repo_key_getter_impl<Interface, Key, false, true, _>
{
    static constexpr bool is_valid = true;
    static constexpr auto& getter = Traits::repo_hints<Interface>::key_getter;
};

template<typename Interface, typename Key>
struct repo_key_getter_impl<Interface, Key, false, false, true>
{
    static constexpr bool is_valid = true;
    static inline auto getter(const Interface& obj) { return obj.id(); }
};

template<typename Interface>
using _repo_hint_general_key_param_t = decltype(Traits::repo_hints<Interface>::key_param);

template<typename Interface, typename Key>
using _repo_hint_specific_key_param_t = decltype(Traits::repo_hints<Interface>::template for_<Key>::key_param);

// Examines the factory parameters for Interface and determines if one of the 
// following is true:
// - Exactly one parameter has the exact type Key
// - No parameter has the exact type Key and exactly one parameter can be 
//   converted to Key
// Yields the keyword type of that parameter or void if the inferrence is not 
// successful (i.e.., no types matched, or too many types matched)
//
// TODO this seems like the kind of thing that could be simplified by leveraging
// actual function overload deduction
template<typename Interface, typename Key>
struct repo_infer_key_param_impl
{
private:
    using _params = boost::mp11::mp_eval_or<boost::mp11::mp_list<>, config_detail::factory_params_t, Interface>;
    using _keyword_types = boost::mp11::mp_transform<boost::mp11::mp_first, _params>;
    using _param_types = boost::mp11::mp_transform<boost::mp11::mp_second, _params>;
    using _is_exact_q = boost::mp11::mp_bind<std::is_same, boost::mp11::_1, Key>;
    using _is_convertible_q = boost::mp11::mp_bind<std::is_convertible, boost::mp11::_1, Key>;
    
    using _index = boost::mp11::mp_if_c<
            boost::mp11::mp_count_if_q<_param_types, _is_exact_q>::value == 1,
            boost::mp11::mp_find_if_q<_param_types, _is_exact_q>,
            boost::mp11::mp_find_if_q<_param_types, _is_convertible_q>
        >;
        
public:
    static constexpr bool success = 
        config_detail::has_factory_params<Interface>::value
        && (
            boost::mp11::mp_count_if_q<_param_types, _is_exact_q>::value == 1
            || (
                boost::mp11::mp_count_if_q<_param_types, _is_exact_q>::value == 0
                && boost::mp11::mp_count_if_q<_param_types, _is_convertible_q>::value == 1
            )
        );
        
    using keyword_t = boost::mp11::mp_eval_if_c<!success, void, boost::mp11::mp_at, _keyword_types, _index>;
};

template<typename Interface, typename Key, 
    bool = boost::mp11::mp_valid<_repo_hint_specific_key_param_t, Interface, Key>::value,
    bool = boost::mp11::mp_valid<_repo_hint_general_key_param_t, Interface>::value,
    bool = (
        repo_infer_defaults_impl<Interface, Key>::value
        && repo_infer_key_param_impl<Interface, Key>::success
    )
>
struct repo_key_param_impl
{
    static constexpr bool is_valid = false;
    
    struct invalid_name_t {};
    static constexpr invalid_name_t name = {};
};

template<typename Interface, typename Key, bool _1, bool _2>
struct repo_key_param_impl<Interface, Key, true, _1, _2>
{
    static_assert(config_detail::has_factory_params<Interface>::value);
    static constexpr bool is_valid = true;
    static constexpr const auto& name = Traits::repo_hints<Interface>::template for_<Key>::key_param;
};

template<typename Interface, typename Key, bool _>
struct repo_key_param_impl<Interface, Key, false, true, _>
{
    static_assert(config_detail::has_factory_params<Interface>::value);
    static constexpr bool is_valid = true;
    static constexpr const auto& name = Traits::repo_hints<Interface>::key_param;
};

template<typename Interface, typename Key>
struct repo_key_param_impl<Interface, Key, false, false, true>
{
    using keyword_t = typename repo_infer_key_param_impl<Interface, Key>::keyword_t;
    static_assert(!std::is_void_v<keyword_t>);
    
    static constexpr bool is_valid = true;
    static constexpr const auto& name = boost::parameter::keyword<keyword_t>::instance;
};

template<typename Interface,
    bool = boost::mp11::mp_valid<_repo_hint_key_type, Interface>::value,
    bool = boost::mp11::mp_valid<_repo_hint_key_types, Interface>::value,
    bool = (
        repo_infer_defaults_impl<Interface>::value
        && boost::mp11::mp_valid<_repo_hint_general_key_getter_t, Interface>::value
    ),
    bool = (
        repo_infer_defaults_impl<Interface>::value
        && boost::mp11::mp_valid<_interface_id_t, Interface>::value
    )
>
struct repo_default_key { using type = void; };

template<typename Interface, bool _1, bool _2, bool _3>
struct repo_default_key<Interface, true, _1, _2, _3>
{
    using type = _repo_hint_key_type<Interface>;
};

template<typename Interface, bool _1, bool _2>
struct repo_default_key<Interface, false, true, _1, _2>
{
    using type = boost::mp11::mp_eval_or<
            void,
            boost::mp11::mp_first,
            _repo_hint_key_types<Interface>
        >;
};

template<typename Interface, bool _>
struct repo_default_key<Interface, false, false, true, _>
{
    using type = std::invoke_result_t<_repo_hint_general_key_getter_t<Interface>, const Interface&>;
};

template<typename Interface>
struct repo_default_key<Interface, false, false, false, true>
{
    using type = _interface_id_t<Interface>;
};

template<class Interface>
class RepoInterfaceInitHelper
{
    template<class Interface_, typename Key>
    using _make_with_key_param_t = decltype(std::declval<Factory<Interface_>>().make(
            repo_key_param_impl<Interface_, boost::remove_cv_ref_t<Key>>::name = std::declval<Key>()
        ));
    
    template<class Interface_>
    using _make_default_t = decltype(std::declval<Factory<Interface_>>().make());
    
public:
    template<typename Key>
    static constexpr bool LazySupported = boost::mp11::mp_or<
            boost::mp11::mp_valid<_make_with_key_param_t, Interface, Key&&>,
            boost::mp11::mp_valid<_make_default_t, Interface>
        >::value;
        
    template<typename Key>
    static constexpr bool MakeAndGetKeySupported = 
            repo_key_getter_impl<Interface, Key>::is_valid
            || repo_key_param_impl<Interface, Key>::is_valid;
    
    RepoInterfaceInitHelper(Factory<Interface> factory)
        : _factory(std::move(factory))
    {
    }
    
    template<typename Key, std::enable_if_t<
            boost::mp11::mp_valid<_make_with_key_param_t, Interface, Key&&>::value
        , void*> = nullptr>
    QSharedPointer<Interface> makeLazy(Key&& key) const
    {
        return _factory.makeShared(
                repo_key_param_impl<
                        Interface, 
                        boost::remove_cv_ref_t<Key>
                    >::name = std::forward<Key>(key)
            );
    }
    
    template<typename Key, std::enable_if_t<
            !boost::mp11::mp_valid<_make_with_key_param_t, Interface, Key&&>::value
            && boost::mp11::mp_valid<_make_default_t, Interface>::value
        , void*> = nullptr>
    QSharedPointer<Interface> makeLazy(Key&&) const
    {
        return _factory.makeShared();
    }
    
    template<typename Key, typename... Args, std::enable_if_t<
            repo_key_getter_impl<Interface, Key>::is_valid
        , void*> = nullptr>
    std::pair<Key, QSharedPointer<Interface>> makeAndGetKey(Args&&... args) const
    {
        auto result = _factory.makeShared(
                std::forward<Args>(args)...
            );
        Key key = std::invoke(repo_key_getter_impl<Interface, Key>::getter, qAsConst(*result));
        return { key, std::move(result)};
    }
    
    template<typename Key, typename... Args, std::enable_if_t<
            !repo_key_getter_impl<Interface, Key>::is_valid
            && repo_key_param_impl<Interface, Key>::is_valid
        , void*> = nullptr>
    std::pair<Key, QSharedPointer<Interface>> makeAndGetKey(Args&&... args) const
    {
        const auto& name = repo_key_param_impl<Interface, Key>::name;
        Key key;
        
        auto result = config_detail::make_factory_param_dispatcher<Interface>(
            [&](const auto& params) {
                key = params[name];
                return _factory.makeShared(params);
            })(std::forward<Args>(args)...);
            
        return { key, std::move(result) };
    }
    
private:
    Factory<Interface> _factory;
};

constexpr std::size_t REPO_RESULT_ARRAY_SIZE = 64;

// Normal repository
template<class Interface, class Key_, bool SupportsLazyInit = false>
class Repository_impl
{
protected:    
    using key_array_t = QVarLengthArray<Key_>;
    
    struct Entry
    {
        Entry() = default;
        Entry(const Entry& other)
            : repo(other.repo)
            , key(other.key)
        {
            const QReadLocker lock_(&lock);
            member = other.member;
        }
        
        Entry& operator=(Entry other)
        {
            const QWriteLocker lock_(&lock);
            repo = other.repo;
            key = other.key;
            member = other.member;
            return *this;
        }
        
        friend void swap(Entry& lhs, Entry& rhs)
        {
#ifdef ADDLE_DEBUG
            assert(lhs.lock.tryLockForWrite());
            assert(rhs.lock.tryLockForWrite());
            lhs.lock.unlock();
            rhs.lock.unlock();
#endif
            using std::swap;
            swap(lhs.repo, rhs.repo);
            swap(lhs.key, rhs.key);
            swap(lhs.member, rhs.member);
        }
        
        Repository_impl* repo = nullptr;
        
        Key_ key;
        QSharedPointer<Interface> member;
        
        QReadWriteLock lock;
    };
    
    using entries_t = std::list<Entry>;
    using entry_array_t = QVarLengthArray<Entry*>;
    
    Repository_impl(Factory<Interface> factory) : _factory(std::move(factory)) {}
    
    Repository_impl(const Repository_impl&) = delete;
    Repository_impl(Repository_impl&&) = delete;
    Repository_impl& operator=(const Repository_impl&) = delete;
    Repository_impl& operator=(Repository_impl&&) = delete;
    
    struct GetOneResult
    {
        Entry* found = nullptr;
    };
    
    auto members_impl() const 
    {
        using namespace boost::adaptors;
        
        struct MemberGetter
        {
            const CoordinatedReadWriteLock::AutoHandle lock_;
            QSharedPointer<Interface> operator()(const Entry& entry) const noexcept
            {
                return entry.member;
            }
        };
        
        auto lock = _lock.lockAuto();
        return _entries 
            | transformed(MemberGetter { std::move(lock) });
    }
    
    GetOneResult getOne_impl(Key_ key) const;
    
    Factory<Interface> _factory;
    
    entries_t _entries;
    QHash<Key_, typename entries_t::iterator> _forwardIndex;
    QHash<const void*, typename entries_t::iterator> _backwardIndex;
    
    mutable CoordinatedReadWriteLock _lock;
};

template<class Interface, class Key_, bool SupportsLazyInit>
typename Repository_impl<Interface, Key_, SupportsLazyInit>::GetOneResult 
Repository_impl<Interface, Key_, SupportsLazyInit>::getOne_impl(Key_ key) const
{
    Q_UNIMPLEMENTED();
}

// Lazy-initializing member repository
template<class Interface, class Key_>
class Repository_impl<Interface, Key_, true>
{
protected:    
    using key_array_t = QVarLengthArray<Key_, REPO_RESULT_ARRAY_SIZE>;
    
    struct Entry
    {
        enum InitState : unsigned
        {
            Uninitialized = 0,
            Initialized,
            InitError
        };
        
        Entry() = default;
        Entry(Repository_impl* repo_, Key_ key_, 
            QSharedPointer<Interface> member_ = {})
            : repo(repo_), key(key_), member(member_), 
            initState(member ? Initialized : Uninitialized)
        {
        }
        
        Entry(const Entry& other)
            : repo(other.repo)
            , key(other.key)
        {
            const QReadLocker lock_(&other.lock);
            
            member = other.member;
            initState = other.initState;
            initException = other.initException;
        }
        
        Entry& operator=(Entry other) // pass by copy avoids possibility of mutual assign deadlock
        {
            const QWriteLocker lock_(&lock);
            
            repo        = other.repo;
            key         = std::move(other.key);
            member      = std::move(other.member);
            initState   = other.initState;
            initException = std::move(other.initException);
            return *this;
        }
        
        // WARNING swap is strictly to support std::sort of entries in a
        // container owned by a ...Result object. locking is assumed to be 
        // irrelevant and is disregarded. (lock-safe swap is not supported)
        friend void swap(Entry& lhs, Entry& rhs)
        {
            using std::swap;
#ifdef ADDLE_DEBUG
            assert(lhs.lock.tryLockForWrite());
            assert(rhs.lock.tryLockForWrite());
            lhs.lock.unlock();
            rhs.lock.unlock();
#endif
            swap(lhs.repo, rhs.repo);
            swap(lhs.key, rhs.key);
            swap(lhs.member, rhs.member);
            swap(lhs.initState, rhs.initState);
            swap(lhs.initException, rhs.initException);
        }
        
        bool lazyInit(bool silenceErrors = false) const;
        
        Repository_impl* repo = nullptr;
        
        Key_ key;
        mutable QSharedPointer<Interface> member;
        
        mutable QAtomicInteger<unsigned> initState;
        mutable AddleException::inner_exception_t initException;
        mutable QMutex initMutex;
        mutable QReadWriteLock lock;
    };
    
    using entries_t = std::list<Entry>;
    using entry_array_t = QVarLengthArray<Entry, REPO_RESULT_ARRAY_SIZE>;
    
    struct GetResult
    {
        entry_array_t found;
        QList<Key_> notFound;
        
        QList<std::pair<Key_, AddleException::inner_exception_t>> initErrorsFound;
        QList<const Entry*> newlyInitialized;
                
        bool isError() const { return !notFound.empty() && !initErrorsFound.empty(); }
    };
    
    struct AddKeysResult
    {
        entry_array_t added;
        QList<Key_> duplicated;
        
        bool isError() const { return !duplicated.empty(); }
    };
    
    struct AddMembersResult
    {
        entry_array_t added;
        key_array_t keysAdded;
        
        QList<Key_> duplicated;
        QList<QSharedPointer<Interface>> replaced;
        
        bool isError() const { return !duplicated.empty(); }
    };
    
    using member_pair_list_t = QList< std::pair<Key_, QSharedPointer<Interface>> >;
    
    Repository_impl(Factory<Interface> factory) : _factory(std::move(factory)) {}
    
    Repository_impl(const Repository_impl&) = delete;
    Repository_impl(Repository_impl&&) = delete;
    Repository_impl& operator=(const Repository_impl&) = delete;
    Repository_impl& operator=(Repository_impl&&) = delete;
    
    template<typename OnNew>
    auto members_impl(OnNew&& onNew) const
    {
        using namespace boost::adaptors;
        
        struct MemberGetter
        {
            const CoordinatedReadWriteLock::AutoHandle lock_;
            const OnNew onNew_;
            QSharedPointer<Interface> operator()(const Entry& entry) const
            {
                bool isNew = entry.lazyInit();
                if (isNew) std::invoke(onNew_, entry.member);
                const QReadLocker entryLock(&entry.lock);
                return QSharedPointer<Interface>(entry.member);
            }
        };
        
        auto lock = _lock.lockAuto();
        return _entries 
            | transformed(MemberGetter { std::move(lock), std::forward<OnNew>(onNew) });
    }
    
    template<typename KeyRange>
    GetResult get_impl(KeyRange&& keys) const;

    template<typename KeyRange>
    AddKeysResult addKeys_impl(KeyRange&& keys);
    
    template<typename MemberRange>
    AddMembersResult addMembers_impl(MemberRange&& members, bool replace = false);
    
    Factory<Interface> _factory;
    
    entries_t _entries;
    QHash<Key_, typename entries_t::iterator> _forwardIndex;
    
    mutable QHash<const void*, typename entries_t::iterator> _backwardIndex;
    mutable QMutex _backwardIndexMutex;
    
    mutable CoordinatedReadWriteLock _lock;
};

template<class Interface, typename Key_>
template<typename KeyRange>
typename Repository_impl<Interface, Key_, true>::GetResult
Repository_impl<Interface, Key_, true>::get_impl(KeyRange&& keys) const
{
    GetResult result;
    reserve_for_size_if_forward_range2(keys, result.found);
    
    {
        const auto lock = _lock.lockAuto();    
        for (auto&& key : std::forward<KeyRange>(keys))
        {
            auto find = noDetach(_forwardIndex).find(key);
            if (Q_LIKELY(find != _forwardIndex.cend()))
            {
                bool isNewlyInitialized = false;
                if (Q_UNLIKELY((**find).initState.loadRelaxed() == Entry::Uninitialized))
                    isNewlyInitialized = (**find).lazyInit(true);
                
                result.found.append(**find);
                const auto& entry = result.found.last();
                
                if (Q_UNLIKELY(entry.initState.loadRelaxed() == Entry::InitError))
                {
                    result.initErrorsFound.append({
                                entry.key
                            , entry.initException
                        });
                    result.found.removeLast();
                }
                else if (isNewlyInitialized)
                {
                    result.newlyInitialized.append(&entry);
                }
            }
            else
            {
                result.notFound.append(key);
            }
        }
    }
    
    if constexpr (boost::has_less<Key_>::value)
    {
        std::sort(result.found.begin(), result.found.end(), 
            [] (const auto& lhs, const auto& rhs) { return lhs.key < rhs.key; });
        std::sort(result.notFound.begin(), result.notFound.end());
    }
    
    return result;
}
    
template<class Interface, typename Key_>
template<typename KeyRange>
typename Repository_impl<Interface, Key_, true>::AddKeysResult 
Repository_impl<Interface, Key_, true>::addKeys_impl(KeyRange&& keys)
{
    AddKeysResult result;
    reserve_for_size_if_forward_range2(keys, result.added);
    
    {
        const auto lock = _lock.lockForWrite();
        for (auto&& key : std::forward<KeyRange>(keys))
        {
            auto find = noDetach(_forwardIndex).find(key);
            if (Q_LIKELY(find == _forwardIndex.cend()))
            {
                auto entryIt = _entries.emplace(_entries.end(), this, key);
                _forwardIndex.insert(key, entryIt);
                result.added.append(*entryIt);
            }
            else
            {
                result.duplicated.append(key);
            }
        }
    }
    
    if constexpr (boost::has_less<Key_>::value)
    {
        std::sort(result.added.begin(), result.added.end(), 
            [](const auto& lhs, const auto& rhs) { return lhs.key < rhs.key; });
        std::sort(result.duplicated.begin(), result.duplicated.end());
    }
    
    return result;
}

template<class Interface, typename Key_>
template<typename MemberRange>
typename Repository_impl<Interface, Key_, true>::AddMembersResult 
Repository_impl<Interface, Key_, true>::addMembers_impl(MemberRange&& members, bool replace)
{
    static_assert(
        boost::mp11::mp_valid<_validate_pair_t, 
            typename boost::range_value<boost::remove_cv_ref_t<MemberRange>>::type
        >::value,
        "Members must be added as a range of key/member pairs."
    );
    AddMembersResult result;
    QSet<Key_> keysAlreadySeen;
    
    reserve_for_size_if_forward_range2(members, result.added, keysAlreadySeen);
    
    {
        const auto lock = _lock.lockForWrite();
        
        for (auto&& newEntry : std::forward<MemberRange>(members))
        {
            Key_ key = std::forward<decltype(newEntry.first)>(newEntry.first);
            QSharedPointer<Interface> member = std::forward<decltype(newEntry.second)>(newEntry.second);
            
            // validate against backward map?
            // debug builds only?
            
            auto find = noDetach(_forwardIndex).find(key);
            if (Q_LIKELY(find == noDetach(_forwardIndex).end()))
            {
                auto entryIt = _entries.emplace(_entries.end(), this, key, member);
                _forwardIndex.insert(key, entryIt);
                _backwardIndex.insert(member.data(), entryIt);
                
                result.added.append(*entryIt);
                result.keysAdded.append(key);
                continue;
            }
            
            auto& entry = **find;
            
            const QWriteLocker lock(&entry.lock);
            unsigned state = entry.initState.loadRelaxed();
            switch(state)
            {
                case Entry::Initialized:
                    if (!replace)
                    {
                        result.duplicated.append(entry.key);
                        break; // switch
                    }
                    
                    _backwardIndex.remove(entry.member.data());
                    result.replaced.append(entry.member);
                    
                    Q_FALLTHROUGH();
                
                case Entry::Uninitialized:
                case Entry::InitError:
                    entry.member = member;
                    entry.initException = {};
                    _backwardIndex.insert(member.data(), *find);
                    break; // switch
                    
                default:
                    Q_UNREACHABLE();
            }
        }
    }
    
    return result;
}

template<class Interface, class Key_>
bool Repository_impl<Interface, Key_, true>::Entry::lazyInit(bool silenceErrors) const
{
    assert(repo);
    if (Q_LIKELY(initState.loadRelaxed() != Uninitialized)) return false;
    
    const QMutexLocker memberLock(&initMutex);
    if (Q_LIKELY(initState.loadRelaxed() == Uninitialized))
    {
        assert(!member);
        RepoInterfaceInitHelper<Interface> initHelper(repo->_runtimeConfig);
        
        try
        {
            member = initHelper.makeLazy(key);
            initState.storeRelaxed(Initialized);
            
            auto find = repo->_forwardIndex.find(key);
            assert(find != repo->_forwardIndex.end());
            
            repo->_backwardIndexMutex.lock();
            repo->_backwardIndex.insert(member.data(), *find);
            repo->_backwardIndexMutex.unlock();
        }
        catch(const AddleException& ex)
        {
            initException = QSharedPointer<AddleException>(ex.clone());
            initState.storeRelaxed(InitError);
        }
        catch(...)
        {
            initException = std::current_exception();
            initState.storeRelaxed(InitError);
        }
        
        if (!silenceErrors && initState.loadRelaxed() == InitError)
            ADDLE_THROW(RepoLazyInitError(key, initException));
        
        return true;
    }
    
    return false;
}

// Keyless repository
template<class Interface>
class Repository_impl<Interface, void>
{
protected:
    Repository_impl(const Repository_impl&) = delete;
    Repository_impl(Repository_impl&&) = delete;
    Repository_impl& operator=(const Repository_impl&) = delete;
    Repository_impl& operator=(Repository_impl&&) = delete;
    
    using entries_t = QSet<QSharedPointer<Interface>>;
    
    entries_t _entries;
    
    mutable CoordinatedReadWriteLock _lock;
};

template<class EntryRef_, class Entry_, class Interface, typename Key, bool /* SupportsLazyInit = false */>
class EntryRef_impl
{
public:
    Key key() const { assert(_entry); return _entry->key; }
    
    QSharedPointer<Interface> member() const noexcept
    {
        assert(_entry);
        
        QReadLocker lock(&_entry->lock);
        return QSharedPointer<Interface>(_entry->member);
    }
    
    operator QSharedPointer<Interface>() const noexcept { return member(); }
    
    void setMember(QSharedPointer<Interface> member) const
    {
        assert(_entry && member);
        
        // if supports key deduction, validate key
        
        QWriteLocker lock(&_entry->lock);
        
        auto oldMember = std::exchange(_entry->member, std::move(member));
        
        if (oldMember != _entry->member) static_cast<const EntryRef_*>(this)->onMemberSet_p(oldMember.data());
    }
    
    const EntryRef_& operator=(QSharedPointer<Interface> member) const
    { 
        setMember(std::move(member)); 
        return static_cast<const EntryRef_&>(*this);
    }
    
protected:
    EntryRef_impl(Entry_& entry)
        : _entry(&entry)
    {
    }
    
    Entry_* _entry = nullptr;
};

template<class EntryRef_, class Entry_, class Interface, typename Key>
class EntryRef_impl<EntryRef_, Entry_, Interface, Key, true>
{
public:
     Key key() const { assert(_entry); return _entry->key; }
    
    QSharedPointer<Interface> member() const
    {
        assert(_entry);
        if (Q_UNLIKELY(_entry->lazyInit())) static_cast<const EntryRef_*>(this)->onMemberSet_p();
        
        QReadLocker lock(&_entry->lock);
        
        if (Q_UNLIKELY(memberIsError()))
            ADDLE_THROW(RepoLazyInitError(_entry->key, _entry->initException, true));
        
        assert(_entry->member);
        return QSharedPointer<Interface>(_entry->member);
    }
    
    QSharedPointer<Interface> tryMember() const noexcept
    {
        assert(_entry);
        if (Q_UNLIKELY(_entry->lazyInit(true))) static_cast<const EntryRef_*>(this)->onMemberSet_p();
        
        QReadLocker lock(&_entry->lock);
        return QSharedPointer<Interface>(_entry->member);
    }
    
    operator QSharedPointer<Interface>() const noexcept { return member(); }
    
    void setMember(QSharedPointer<Interface> member) const
    {
        assert(_entry && member);
        
        // if supports key deduction, validate key
        
        QWriteLocker lock(&_entry->lock);
        
        auto oldMember = std::exchange(_entry->member, std::move(member));
        
        _entry->initState = Entry_::InitState::Initialized;
        _entry->initException = {};
        
        if (oldMember != _entry->member) static_cast<const EntryRef_*>(this)->onMemberSet_p(oldMember.data());
    }
    
    const EntryRef_& operator=(QSharedPointer<Interface> member) const
    { 
        setMember(std::move(member)); 
        return static_cast<const EntryRef_&>(*this);
    }
    
    bool memberIsInitialized() const noexcept
    {
        return Q_LIKELY(_entry) && _entry->initState.loadRelaxed() == Entry_::InitState::Initialized;
    }
    
    bool memberIsError() const noexcept
    {
        return Q_LIKELY(_entry) && _entry->initState.loadRelaxed() == Entry_::InitState::InitError;
    }
    
    AddleException::inner_exception_t initException() const noexcept 
    {
        if (Q_UNLIKELY(!_entry)) return {};
        
        QReadLocker lock(&_entry->lock);
        return AddleException::inner_exception_t(_entry->initException);
    }
    
protected:
    EntryRef_impl(Entry_& entry)
        : _entry(&entry)
    {
    }
    
    Entry_* _entry = nullptr;
};

template<class Interface, class Key>
struct repository_traits
{
    static constexpr bool SupportsLazyInit = RepoInterfaceInitHelper<Interface>::template LazySupported<Key>;
    static constexpr bool SupportsKeyDeduction = repo_key_getter_impl<Interface, Key>::is_valid;
};

} // namespace aux_config3

template<class Interface, class Key = typename aux_config3::repo_default_key<Interface>::type>
class Repository 
    : public GenericRepository
    , private aux_config3::Repository_impl<Interface, Key, aux_config3::repository_traits<Interface, Key>::SupportsLazyInit>
{
    using impl_t = aux_config3::Repository_impl<Interface, Key, aux_config3::repository_traits<Interface, Key>::SupportsLazyInit>;
    using entry_t = typename impl_t::Entry;
    
    using typename impl_t::key_array_t;
    
public:
    static constexpr bool SupportsLazyInit = aux_config3::repository_traits<Interface, Key>::SupportsLazyInit;
    static constexpr bool SupportsKeyDeduction = aux_config3::repository_traits<Interface, Key>::SupportsKeyDeduction;
    
    using interface_t = Interface;
    using key_t = Key;
    
    class EntryRef : public aux_config3::EntryRef_impl<EntryRef, entry_t, Interface, Key, SupportsLazyInit>
    {
    public:
        void releaseLock() { _lockHandle = {}; }
        
    private:
        using base_t = aux_config3::EntryRef_impl<EntryRef, entry_t, Interface, Key, SupportsLazyInit>;
        EntryRef(entry_t& entry, CoordinatedReadWriteLock::WriteHandle lockHandle)
            : base_t(entry), _lockHandle(std::move(lockHandle))
        {
        }
        
        void onMemberSet_p(const void* oldMember = nullptr) const 
        {
            static_cast<Repository*>(this->_entry->repo)->onEntryRefMemberSet_p(*this->_entry, oldMember);
        }
        
        CoordinatedReadWriteLock::WriteHandle _lockHandle;
        
        friend Repository;
        friend class aux_config3::EntryRef_impl<EntryRef, entry_t, Interface, Key, SupportsLazyInit>;
    };
    
    class ConstEntryRef : public aux_config3::EntryRef_impl<ConstEntryRef, const entry_t, Interface, Key, SupportsLazyInit>
    {
    public:
        void releaseLock() { _lockHandle = {}; }
        
    private:
        using base_t = aux_config3::EntryRef_impl<ConstEntryRef, const entry_t, Interface, Key, SupportsLazyInit>;
        ConstEntryRef(const entry_t& entry, CoordinatedReadWriteLock::AutoHandle lockHandle)
            : base_t(entry), _lockHandle(std::move(lockHandle))
        {
        }
        
        void onMemberSet_p(const void* oldMember = nullptr) const 
        {
            static_cast<Repository*>(this->_entry->repo)->onEntryRefMemberSet_p(*this->_entry, oldMember);
        }
        
        CoordinatedReadWriteLock::AutoHandle _lockHandle;
        
        friend Repository;
        friend class aux_config3::EntryRef_impl<ConstEntryRef, const entry_t, Interface, Key, SupportsLazyInit>;
    };
    
    class iterator : public boost::iterator_adaptor<
            iterator, typename impl_t::entries_t::iterator, EntryRef, boost::use_default, EntryRef
        >
    {
        using adaptor_t = boost::iterator_adaptor<
                iterator, typename impl_t::entries_t::iterator, EntryRef, boost::use_default, EntryRef
            >;
        using base_t = typename impl_t::entries_t::iterator;
        
    public:
        iterator() = default;
        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;
        
        void release() { _lockHandle = {}; }
        
    private:
        iterator(base_t baseIt, CoordinatedReadWriteLock::WriteHandle lockHandle)
            : adaptor_t(baseIt), _lockHandle(std::move(lockHandle))
        {
        }
        
        EntryRef dereference() const
        { 
            return EntryRef(*this->base(), _lockHandle);
        }
        
        CoordinatedReadWriteLock::WriteHandle _lockHandle;
        friend Repository;
        friend class boost::iterator_core_access;
    };
    
    class const_iterator : public boost::iterator_adaptor<
            const_iterator, typename impl_t::entries_t::const_iterator, ConstEntryRef, boost::use_default, ConstEntryRef
        >
    {
        using adaptor_t = boost::iterator_adaptor<
                const_iterator, typename impl_t::entries_t::const_iterator, ConstEntryRef, boost::use_default, ConstEntryRef
            >;
        using base_t = typename impl_t::entries_t::const_iterator;
        
    public:
        const_iterator() = default;
        const_iterator(const const_iterator&) = default;
        const_iterator(const_iterator&&) = default;
        
        const_iterator& operator=(const const_iterator&) = default;
        const_iterator& operator=(const_iterator&&) = default;
        
        void release() { _lockHandle = {}; }
        
    private:
        const_iterator(base_t baseIt, CoordinatedReadWriteLock::AutoHandle lockHandle)
            : adaptor_t(baseIt), _lockHandle(std::move(lockHandle))
        {
        }
        
        ConstEntryRef dereference() const
        { 
            return ConstEntryRef(*this->base(), _lockHandle);
        }
        
        CoordinatedReadWriteLock::AutoHandle _lockHandle;
        friend Repository;
        friend class boost::iterator_core_access;
    };
    
    friend bool operator==(const iterator& lhs, const const_iterator& rhs) { return lhs.base() == rhs.base(); }
    friend bool operator!=(const iterator& lhs, const const_iterator& rhs) { return lhs.base() != rhs.base(); }
    friend bool operator==(const const_iterator& lhs, const iterator& rhs) { return lhs.base() == rhs.base(); }
    friend bool operator!=(const const_iterator& lhs, const iterator& rhs) { return lhs.base() != rhs.base(); }
    
    Repository(Factory<Interface> factory) : impl_t(std::move(factory)) {}
    virtual ~Repository() noexcept = default;
    
    /// CAUTION the non-const overloads of begin and end lock the repository for 
    /// writing, do not to use them while the repository is already locked for 
    /// reading.
    
    iterator begin()
    {
        auto lock = this->_lock.lockForWrite();
        return iterator(this->_entries.begin(), std::move(lock));
    }
    iterator end()
    {
        auto lock = this->_lock.lockForWrite();
        return iterator(this->_entries.end(), std::move(lock));
    }
    
    const_iterator begin() const
    {
        auto lock = this->_lock.lockAuto();
        return const_iterator(this->_entries.begin(), std::move(lock));
    }
    
    const_iterator end() const
    {
        auto lock = this->_lock.lockAuto();
        return const_iterator(this->_entries.end(), std::move(lock));
    }
    
    auto keys() const
    {
        using namespace boost::adaptors;
        struct KeyGetter
        {
            const CoordinatedReadWriteLock::AutoHandle lock_;
            const Key& operator()(const entry_t& entry) const noexcept
            { return entry.key; }
        };
        
        auto lock = this->_lock.lockAuto();
        return noDetach(this->_entries) | transformed(KeyGetter { std::move(lock) });
    }
    
    auto members() const
    {
        if constexpr (SupportsLazyInit)
            return impl_t::members_impl(std::bind(&Repository::emitMemberAdded_p, this, std::placeholders::_1));
        else
            return impl_t::members_impl();
    }
    
    auto initializedMembers() const
    {
        using namespace boost::adaptors;
        static_assert(SupportsLazyInit);
        return (*this) 
            | filtered([](auto&& e) { return e.memberIsInitialized(); })
            | transformed([](auto&& e) { return e.member(); });
    }
    
    bool empty() const noexcept override
    { 
        const auto lock = this->_lock.lockAuto(); 
        return this->_entries.empty(); 
    }
    
    std::size_t size() const noexcept override
    { 
        const auto lock = this->_lock.lockAuto(); 
        return this->_entries.size();
    }
    
    iterator find(Key key) noexcept
    {
        const auto lock = this->_lock.lockForWrite();
        return iterator(findOne_p(std::move(key), true), std::move(lock));
    }
    
    const_iterator find(Key key) const noexcept
    {
        const auto lock = this->_lock.lockAuto();
        return const_iterator(findOne_p(std::move(key), true), std::move(lock));
    }
    
    EntryRef at(Key key) noexcept
    {
        const auto lock = this->_lock.lockForWrite();
        return EntryRef(*findOne_p(key), std::move(lock));
    }
    
    ConstEntryRef at(Key key) const noexcept
    {
        const auto lock = this->_lock.lockAuto();
        return ConstEntryRef(*findOne_p(key), std::move(lock));
    }
    
    EntryRef operator[](Key key) noexcept { return at(std::move(key)); }
    ConstEntryRef operator[](Key key) const noexcept { return at(std::move(key)); }
    
    QSharedPointer<Interface> get(Key key) const noexcept(!SupportsLazyInit) { return getOne_p(std::move(key)); }
    QSharedPointer<Interface> tryGet(Key key) const noexcept { return getOne_p(std::move(key), true); }
    
    bool contains(Key key) const noexcept
    {
        const auto lock = this->_lock.lockAuto();
        return this->_forwardIndex.contains(key);
    }
    
    void addKey(Key key)
    { 
        this->addKeys_p(make_inline_range(std::move(key)));
    }
    
    bool tryAddKey(Key key) noexcept
    {
        return this->addKeys_p(make_inline_range(std::move(key)), true);
    }
    
    template<typename KeyRange>
    void addKeys(KeyRange&& keys) 
    {
        this->addKeys_p(std::forward<KeyRange>(keys));
    }
    
    void addKeys(const std::initializer_list<Key>& keys)
    {
        this->addKeys_p(keys);
    }
    
    template<typename KeyRange>
    bool tryAddKeys(KeyRange&& keys) noexcept
    {
        return this->addKeys_p(std::forward<KeyRange>(keys), true);
    }
    
    bool tryAddKeys(const std::initializer_list<Key>& keys) noexcept
    {
        return this->addKeys_p(keys, true);
    }
    
    void addMember(Key key, QSharedPointer<Interface> member)
    {
#ifdef ADDLE_DEBUG
        if constexpr (SupportsKeyDeduction)
        {
            assert(key == std::invoke(aux_config3::repo_key_getter_impl<Interface, Key>::getter, qAsConst(*member) ));
        }
#endif
        this->addMembers_p(make_inline_range(std::make_pair(std::move(key), std::move(member))));
    }
    
    void addMember(QSharedPointer<Interface> member)
    {
        static_assert(SupportsKeyDeduction);
        this->addMembers_p(make_inline_range(std::make_pair( 
                std::invoke(aux_config3::repo_key_getter_impl<Interface, Key>::getter, qAsConst(*member) ),
                std::move(member)
            )));
    }
    
    bool tryAddMember(Key key, QSharedPointer<Interface> member) noexcept
    {
#ifdef ADDLE_DEBUG
        if constexpr (SupportsKeyDeduction)
        {
            assert(key == std::invoke(aux_config3::repo_key_getter_impl<Interface, Key>::getter, qAsConst(*member) ));
        }
#endif
        return this->addMembers_p(make_inline_range(std::make_pair(std::move(key), std::move(member))), true);
    }
    
    bool tryAddMember(QSharedPointer<Interface> member) noexcept
    {
        static_assert(SupportsKeyDeduction);
        return this->addMembers_p(make_inline_range(std::make_pair( 
                std::invoke(aux_config3::repo_key_getter_impl<Interface, Key>::getter, qAsConst(*member) ),
                std::move(member)
            )), true);
    }
    
    template<typename MemberRange>
    void addMembers(MemberRange&& members)
    {
        this->addMembers_p(std::forward<MemberRange>(members));
    }
    
    void addMembers(const std::initializer_list<boost::mp11::mp_if_c<
                SupportsKeyDeduction, QSharedPointer<Interface>,
                std::pair<Key, QSharedPointer<Interface>>
        >>& members)
    {
        this->addMembers_p(members);
    }
    
    template<typename MemberRange>
    bool tryAddMembers(MemberRange&& members)
    {
        return this->addMembers_p(std::forward<MemberRange>(members), true);
    }
    
    bool tryAddMembers(const std::initializer_list<boost::mp11::mp_if_c<
                SupportsKeyDeduction, QSharedPointer<Interface>,
                std::pair<Key, QSharedPointer<Interface>>
        >>& members)
    {
        return this->addMembers_p(members, true);
    }
    
    template<typename... Args>
    QSharedPointer<Interface> makeMember(Args&&... args)
    {
        static_assert(
            aux_config3::RepoInterfaceInitHelper<Interface>::template MakeAndGetKeySupported<Key>,
            "makeMember is not supported for the this Interface and Key combo."
            // makeMember needs to be able to determine the key of the member it 
            // makes, either through key deduction (preferred), or by inferring 
            // the key as a factory parameter from the given arguments.
            // 
            // Repository attempts to make some guesses by default, for example
            // if there is only one factory parameter with an appropriate type,
            // or if Interface has a public member function called id. This 
            // behavior can be controlled by specializing Traits::repo_hints
            // for the Interface.
            // 
            // In some cases, it may be more appropriate to keep a factory 
            // instance, manually instantiate Interface instances, then manage
            // them with explicit keys through e.g., addMembers
        );
        aux_config3::RepoInterfaceInitHelper<Interface> initHelper(this->_runtimeConfig);
        auto member = initHelper.template makeAndGetKey<Key>(std::forward<Args>(args)...);
        
        addMember(member.first, member.second);
        return member.second;
    }
    
    template<typename... Args>
    QSharedPointer<Interface> tryMakeMember(Args&&... args) noexcept
    {
        static_assert(
            aux_config3::RepoInterfaceInitHelper<Interface>::template MakeAndGetKeySupported<Key>,
            "makeMember is not supported for the this Interface and Key combo."
            // See comment in makeMember for more details.
        );
        aux_config3::RepoInterfaceInitHelper<Interface> initHelper(this->_runtimeConfig);
        std::pair<Key, QSharedPointer<Interface>> member;
        
        try
        {
            member = initHelper.template makeAndGetKey<Key>(std::forward<Args>(args)...);
        }
        catch(...)
        {
            return false;
        }
        
        bool success = tryAddMember(member.first, member.second);
        return success ? member.second : QSharedPointer<Interface>();
    }
    
    CoordinatedReadWriteLock::ReadHandle lockForRead(int timeout = -1) const override 
    { 
        return this->_lock.tryLockForRead(timeout);
    }
    
    CoordinatedReadWriteLock::WriteHandle lockForWrite(int timeout = -1) const override 
    { 
        return this->_lock.tryLockForWrite(timeout);
    }
    
private:
    class GenericMemberRef : public GenericRepoMemberRef
    {
    public:
        GenericMemberRef() = default;
        explicit GenericMemberRef(Repository* repo, iterator it)
        {
            new (&_buffer) Handle<false>(std::move(it));
            _initialized = true;
        }
        
        explicit GenericMemberRef(const Repository* repo, const_iterator it)
        {
            new (&_buffer) Handle<true>(std::move(it));
            _initialized = true;
        }
        
    private:
        template<bool IsConst>
        struct Handle : public IHandle
        {
            using iterator_t = boost::mp11::mp_if_c<IsConst, const_iterator, iterator>;
                
            Handle(iterator_t it_) noexcept : it(std::move(it_)) {}
            Handle(const Handle&) noexcept = default;
            
            virtual ~Handle() noexcept = default;
            
            QMetaType::Type keyType() const noexcept override 
            {
                return aux_config3::RepoKeyVariantHelper<Key>::type();
            }
            
            QByteArray interfaceName() const noexcept override 
            { 
                return aux_config3::InterfaceNameHelper<Interface>::name(); 
            }
            
            bool supportsLazyInitialization() const noexcept override
            {
                return SupportsLazyInit;
            }
            
            bool isReadOnly() const noexcept override
            {
                return IsConst;
            }
            
            virtual QObject* repository() const noexcept override
            {
                return static_cast<Repository*>((*it.base()).repo);
            }
    
            void clone(void* dest) const noexcept override { new (dest) Handle(*this); }
            bool equal(const IHandle* other) const noexcept override;
            
            void increment() noexcept override { ++it; }
            void decrement() noexcept override { --it; }
            
            void releaseLock() noexcept override { it.release(); }
        
            QVariant key() const noexcept override 
            { 
                return aux_config3::RepoKeyVariantHelper<Key>::toVariant((*it).key()); 
            }
            
            aux_config3::GenericRepoMemberCore getMemberCore() const noexcept override
            {
                aux_config3::GenericRepoMemberCore result;
                
                const QReadLocker lock(&(*it.base()).lock);
                result.member = aux_config3::RepoInterfaceVariantHelper<Interface>::toVariant((*it.base()).member);
                result.rawMember = (*it.base()).member.data();
                
                if constexpr (SupportsLazyInit)
                {
                    result.initException = (*it.base()).initException;
                }
                
                return result;
            }
            
            QVariant getMember(bool silenceErrors = false) const override
            {   
                if constexpr (SupportsLazyInit)
                    return aux_config3::RepoInterfaceVariantHelper<Interface>::toVariant(
                            silenceErrors ? (*it).tryMember() : (*it).member()
                        );
                else
                    return aux_config3::RepoInterfaceVariantHelper<Interface>::toVariant(
                            (*it).member()
                        );
            }
            
            void* getRawMember(bool silenceErrors = false) const override
            {
                if constexpr (SupportsLazyInit)
                    return silenceErrors ? (*it).tryMember().data(): (*it).member().data();
                else
                    return (*it).member().data();
            }
            
            void setMember(QVariant member) override
            {
                if constexpr (!IsConst)
                {
                    auto member_ = aux_config3::RepoInterfaceVariantHelper<Interface>::fromVariant(member);
                    (*it).setMember(std::move(member_));
                }
                Q_UNREACHABLE();
            }
                    
            bool memberIsInitialized() const noexcept override
            {
                if constexpr (SupportsLazyInit)
                    return (*it).memberIsInitialized();
                else
                    return true;
            }
            
            bool memberIsError() const noexcept override
            {
                if constexpr (SupportsLazyInit)
                    return (*it).memberIsError();
                else
                    return true;
            }
            
            AddleException::inner_exception_t getInitException() const noexcept override
            { 
                if constexpr (SupportsLazyInit)
                    return (*it).initException();
                else
                    return {};
            }
            
            iterator_t it;
        };
        
        static_assert(sizeof(Handle<true>) <= BUFFER_SIZE);
        static_assert(sizeof(Handle<false>) <= BUFFER_SIZE);
    };
    
    typename impl_t::entries_t::iterator findOne_p(Key key, bool silenceErrors = false)
    {
        assert(this->_lock.isLockedForWrite());
        auto it = noDetach(this->_forwardIndex.find(key));
        
        if (Q_UNLIKELY(it == this->_forwardIndex.cend()))
        {
            if (!silenceErrors) throw RepoKeysNotFoundException({ key });
            return this->_entries.end();
        }
        
        return *it;
    }
    
    typename impl_t::entries_t::const_iterator findOne_p(Key key, bool silenceErrors = false) const
    {
        assert(this->_lock.isLocked());
        auto it = noDetach(this->_forwardIndex.find(key));
                
        if (Q_UNLIKELY(it == this->_forwardIndex.cend()))
        {
            if (!silenceErrors) throw RepoKeysNotFoundException({ key });
            return this->_entries.end();
        }
        
        return *it;
    }
    
    QSharedPointer<Interface> getOne_p(Key key, bool silenceErrors = false) const;
    
    template<typename KeyRange>
    bool addKeys_p(KeyRange&& keys, bool silenceErrors = false);
    
    template<typename MemberRange>
    bool addMembers_p(MemberRange&& members, bool silenceErrors = false);
    
    void onEntryRefMemberSet_p(const entry_t& entry, const void* oldMember = nullptr)
    {
        bool scheduleEmit;
        
        {
            const auto lock = this->_lock.lockAuto();
                
            _recentChangesMutex.lock();
            _genericMembersCacheMutex.lock();
            if constexpr (SupportsLazyInit) this->_backwardIndexMutex.lock();
            
            auto it = this->_forwardIndex.find(entry.key);
            assert(it != this->_forwardIndex.cend());
            
            if (oldMember) this->_backwardIndex.remove(oldMember);
            this->_backwardIndex[oldMember] = *it;
            
            scheduleEmit = _recentMembersAdded.isEmpty();
            _recentMembersAdded[entry.key] = entry;
            
            _genericMembersCache.clear();
            
            if constexpr (SupportsLazyInit) this->_backwardIndexMutex.unlock();
            _genericMembersCacheMutex.unlock();
            _recentChangesMutex.unlock();
        }
        
        if (Q_LIKELY(this->_lock.isLocked()))
        {   
            assert(SupportsLazyInit || this->_lock.isLockedForWrite());
            if (scheduleEmit)
                this->_lock.onUnlock(std::bind(&Repository::emitRecentMembersAdded, this));
        }
        else
        {
            emitRecentMembersAdded();
        }
    }
    
    void emitRecentKeysAdded();
    void emitRecentKeysRemoved();
    
    void emitRecentMembersAdded();
    
    QMetaType::Type keyType() const noexcept override 
    {
        return aux_config3::RepoKeyVariantHelper<Key>::type();
    }
    
    QByteArray interfaceName() const noexcept override 
    { 
        return aux_config3::InterfaceNameHelper<Interface>::name(); 
    }
    
    bool supportsLazyInitialization() const noexcept override { return SupportsLazyInit; }
    bool supportsKeyDeduction() const noexcept override { return SupportsKeyDeduction; }
    
    bool contains(QVariant key) const noexcept override;
    GenericRepoMemberRef at(QVariant key) noexcept override;
    GenericRepoMemberRef at(QVariant key) const noexcept override;
    void addKeys(QVariantList keys) override;
    bool tryAddKeys(QVariantList keys) noexcept override;
    void addMembers(QList<Addle::GenericRepoMember> members) override;
    bool tryAddMembers(QList<Addle::GenericRepoMember> members) noexcept override;
    GenericRepoMemberRef beginGeneric() noexcept override;
    GenericRepoMemberRef endGeneric() noexcept override;
    GenericRepoMemberRef beginGeneric() const noexcept override;
    GenericRepoMemberRef endGeneric() const noexcept override;
    
    QList<Key> _recentKeysAdded;
    QList<Key> _recentKeysRemoved;
    
    using recent_members_added_t = boost::mp11::mp_if<
            boost::has_less<Key>,
            QMap<Key, entry_t>,
            QHash<Key, entry_t>
        >;
    mutable recent_members_added_t _recentMembersAdded;
    mutable QMutex _recentChangesMutex;
};

template<class Interface, class Key>
template<bool IsConst>
bool Repository<Interface, Key>::GenericMemberRef::Handle<IsConst>::equal(const IHandle* other) const noexcept
{
    assert(other);
    
    const Handle* other_1;
    const Handle<!IsConst>* other_2;
    
    if (Q_LIKELY(other_1 = dynamic_cast<const Handle*>(other)))
        return it == other_1->it;
    else if ((other_2 = dynamic_cast<const Handle<!IsConst>*>(other)))
        return it == other_2->it;
    else
        return false;
}

template<class Interface, class Key>
QSharedPointer<Interface> Repository<Interface, Key>::getOne_p(Key key, bool silenceErrors) const
{
    auto result = this->get_impl(make_inline_range(key));
    assert(result.found.size() + result.initErrorsFound.size() == 1);
    
    const entry_t* found = Q_LIKELY(!result.found.isEmpty()) ? &result.found.front() : nullptr;
    
    if constexpr (SupportsLazyInit)
    {
        if (!result.newlyInitialized.empty())
        {
            assert((result.newlyInitialized.front() == found) && (found->repo == this));
            const_cast<Repository*>(this)->onEntryRefMemberSet_p(*found);
        }
        
        if (!silenceErrors && !result.initErrorsFound.isEmpty())
        {
            ADDLE_THROW(RepoLazyInitError(
                key,
                std::move(result.initErrorsFound.front().second),
                true
            ));
        }
    }
    
    if (!silenceErrors && !result.notFound.isEmpty())
    {
        assert(result.notFound.front() == key);
        throw RepoKeysNotFoundException({ key });
    }
    
    assert(found && found->member);
    return found->member;
}

template<class Interface, class Key>
template<typename KeyRange>
bool Repository<Interface, Key>::addKeys_p(KeyRange&& keys, bool silenceErrors)
{
    static_assert(SupportsLazyInit,
        "This repository's interface type does not allow lazy initialization "
        "of members from keys -- members must be provided using e.g., "
        "addMembers"
    );
    
    _genericKeysCache.clear();
    auto result = this->addKeys_impl(std::forward<KeyRange>(keys));
    
    bool keysChangedIsWatched = receivers( SIGNAL(keysChanged(QVariantList)) );
    bool keysAddedIsWatched = receivers( SIGNAL(keysAdded(QVariantList)) );
    
    if (!result.added.empty() && (keysChangedIsWatched || keysAddedIsWatched))
    {
        if (this->_lock.isLocked())
        {
            const QMutexLocker lock(&_recentChangesMutex);
            bool scheduleEmit = _recentKeysAdded.isEmpty();
            
            _recentKeysAdded.reserve(_recentKeysAdded.size() + result.added.size());
            for (auto&& entry : result.added)
                _recentKeysAdded.append(entry.key);
            
            if (scheduleEmit)
                this->_lock.onUnlock(std::bind(&Repository::emitRecentKeysAdded, this));
        }
        else
        {
            if (keysChangedIsWatched) 
            {
                emit keysChanged(GenericRepository::keys());
            }
            
            if (keysAddedIsWatched)
            {
                QVariantList keysAdded_;
                keysAdded_.reserve(result.added.size());
                for (auto&& entry : result.added)
                    keysAdded_.append(aux_config3::RepoKeyVariantHelper<Key>::toVariant(entry.key));
                
                emit keysAdded(std::move(keysAdded_));
            }
        }
    }
    
    if (!silenceErrors && !result.duplicated.empty())
    {
        throw RepoKeysAlreadyAddedException(QList<Key>(
                std::make_move_iterator(result.duplicated.begin()),
                std::make_move_iterator(result.duplicated.end())
            ));
    }
    
    return !result.isError();
}
    
template<class Interface, class Key>
template<typename MemberRange>
bool Repository<Interface, Key>::addMembers_p(MemberRange&& members, bool silenceErrors)
{
    typename impl_t::AddMembersResult result;
    
    _genericKeysCache.clear();
    if constexpr (SupportsKeyDeduction)
    {
        using namespace boost::adaptors;
        const auto& getter = aux_config3::repo_key_getter_impl<Interface, Key>::getter;
        result = this->addMembers_impl(
                  members
                | transformed([] (const QSharedPointer<Interface>& member) { 
                    return std::make_pair(std::invoke(getter, qAsConst(*member)), member); 
                })
            );
    }
    else
    {
        result = this->addMembers_impl(std::forward<MemberRange>(members));
    }
    
    bool membersAddedIsWatched  = receivers( SIGNAL(membersAdded(QList<Addle::GenericRepoMember>)) );
    bool keysAddedIsWatched     = receivers( SIGNAL(keysAdded(QVariantList)) );
    bool keysChangedIsWatched   = receivers( SIGNAL(keysChanged(QVariantList)) );
    
    if (!result.added.empty() && (membersAddedIsWatched || keysAddedIsWatched || keysChangedIsWatched))
    {
        if (this->_lock.isLocked())
        {
            const QMutexLocker lock(&_recentChangesMutex);
            bool scheduleEmitMembers = _recentMembersAdded.isEmpty();
            bool scheduleEmitKeys = _recentKeysAdded.isEmpty();
            
            for (auto&& entry : result.added)
                _recentMembersAdded[entry.key] = entry;
            
            for (auto&& key : result.keysAdded)
                _recentKeysAdded.append(key);
            
            if (scheduleEmitMembers)
                this->_lock.onUnlock(std::bind(&Repository::emitRecentMembersAdded, this));
            
            if (scheduleEmitKeys)
                this->_lock.onUnlock(std::bind(&Repository::emitRecentKeysAdded, this));
        }
        else
        {
            if (membersAddedIsWatched && !result.added.isEmpty())
            {
                QList<GenericRepoMember> membersAdded_;
                membersAdded_.reserve(result.added.size());
                for (auto&& entry : result.added)
                {
                    membersAdded_.append(GenericRepoMember(
                             aux_config3::RepoKeyVariantHelper<Key>::toVariant(entry.key),
                             entry.member
                        ));
                }
                
                emit membersAdded(std::move(membersAdded_));
            }
            
            if (keysAddedIsWatched && !result.keysAdded.isEmpty())
            {
                QVariantList keysAdded_;
                keysAdded_.reserve(result.keysAdded.size());
                for (auto&& key : result.keysAdded)
                    keysAdded_.append(aux_config3::RepoKeyVariantHelper<Key>::toVariant(key));
                
                emit keysAdded(std::move(keysAdded_));
            }
            
            if (keysChangedIsWatched && !result.keysAdded.isEmpty()) 
            {
                emit keysChanged(GenericRepository::keys());
            }
        }
    }
    
    if (!silenceErrors && !result.duplicated.isEmpty())
    {
        throw RepoKeysAlreadyAddedException(QList<Key>(
                std::make_move_iterator(result.duplicated.begin()),
                std::make_move_iterator(result.duplicated.end())
            ));
    }
    
    return !result.isError();
}

template<class Interface, class Key>
void Repository<Interface, Key>::emitRecentKeysAdded()
{
    using namespace boost::adaptors;
    
    assert(!this->_lock.isLocked());
    QList<Key> recentKeysAdded;
    
    {
        const QMutexLocker lock(&_recentChangesMutex);
        recentKeysAdded = std::exchange(_recentKeysAdded, {});
    }
    
    if (Q_UNLIKELY(recentKeysAdded.isEmpty())) return;
    
    if constexpr (boost::has_less<Key>::value)
        std::sort(recentKeysAdded.begin(), recentKeysAdded.end());
    
    if (receivers( SIGNAL(keysChanged(QVariantList)) ))
    {
        emit keysChanged( GenericRepository::keys() );
    }
    
    if (receivers( SIGNAL(keysAdded(QVariantList)) ))
    {
        emit keysAdded( QVariantList(qToList( 
                  noDetach(recentKeysAdded)
                | transformed([] (const auto& key) {
                    return aux_config3::RepoKeyVariantHelper<Key>::toVariant(key);
                })
            )) );
    }
}

template<class Interface, class Key>
void Repository<Interface, Key>::emitRecentKeysRemoved()
{
    assert(!this->_lock.isLocked());
    
}

template<class Interface, class Key>
void Repository<Interface, Key>::emitRecentMembersAdded()
{
    assert(!this->_lock.isLocked());
    
    recent_members_added_t recentMembersAdded;
    
    {
        const QMutexLocker lock(&_recentChangesMutex);
        recentMembersAdded = std::exchange(_recentMembersAdded, {});
    }
    
    if (Q_UNLIKELY(recentMembersAdded.isEmpty())) return;
    
    bool membersAddedIsWatched = receivers( SIGNAL(membersAdded(QList<Addle::GenericRepoMember>)) );
    bool memberErrorsIsWatched = receivers( SIGNAL(memberErrors(QList<Addle::GenericRepoMember>)) );
    
    if (membersAddedIsWatched || memberErrorsIsWatched)
    {
        QList<GenericRepoMember> genericRecentAdded;
        QList<GenericRepoMember> genericRecentErrors;
        genericRecentAdded.reserve(recentMembersAdded.size());
        for (auto&& entry : recentMembersAdded)
        {
            if constexpr (SupportsLazyInit)
            {
                if (entry.initState.loadRelaxed() == entry_t::InitError)
                {
                    genericRecentErrors.append( GenericRepoMember(
                              entry.key
                            , std::move(entry.initException)
                        ));
                    continue;
                }
            }
            
            // "else"
            genericRecentAdded.append( GenericRepoMember(entry.key, std::move(entry.member)) );
        }
        
        if (membersAddedIsWatched && !genericRecentAdded.isEmpty())
            emit membersAdded(std::move(genericRecentAdded));
        
        if (memberErrorsIsWatched && !genericRecentErrors.isEmpty())
            emit memberErrors(std::move(genericRecentErrors));
    }
}

template<class Interface, class Key>
bool Repository<Interface, Key>::contains(QVariant key) const noexcept
{
    auto key_ = aux_config3::RepoKeyVariantHelper<Key>::fromVariant(key, true);
    return key_ && contains(*key_);
}

template<class Interface, class Key>
GenericRepoMemberRef Repository<Interface, Key>::at(QVariant key) noexcept
{
    auto key_ = aux_config3::RepoKeyVariantHelper<Key>::fromVariant(key, true);
    if (Q_UNLIKELY(!key_)) return {};
    
    auto lock = this->_lock.lockForWrite();
    auto find = this->_forwardIndex.find(*key_);
    
    return (find != this->_forwardIndex.end()) ? 
          GenericMemberRef(this, iterator(*find, std::move(lock)))
        : GenericMemberRef();
}

template<class Interface, class Key>
GenericRepoMemberRef Repository<Interface, Key>::at(QVariant key) const noexcept 
{
    auto key_ = aux_config3::RepoKeyVariantHelper<Key>::fromVariant(key, true);
    if (Q_UNLIKELY(!key_)) return {};
    
    auto lock = this->_lock.lockAuto();
    auto find = this->_forwardIndex.find(*key_);
    
    return (find != this->_forwardIndex.end()) ? 
          GenericMemberRef(this, const_iterator(*find, std::move(lock)))
        : GenericMemberRef();
}
    
template<class Interface, class Key>
void Repository<Interface, Key>::addKeys(QVariantList keys)
{
    if constexpr (SupportsLazyInit)
    {
        key_array_t keys_;
        keys_.reserve(keys.size());
        
        for (QVariant key : noDetach(keys))
        {
            auto key_ = aux_config3::RepoKeyVariantHelper<Key>::fromVariant(key, false);
            keys_.append(*key_);
        }
        
        this->addKeys_p(keys_);
    }
    else // constexpr (!SupportsLazyInit)
    {
        ADDLE_THROW(RepoLazyInitNotSupportedException());
    }
}

template<class Interface, class Key>
bool Repository<Interface, Key>::tryAddKeys(QVariantList keys) noexcept
{
    using namespace boost::adaptors;
    if constexpr (SupportsLazyInit)
    {
        auto type = (int)(aux_config3::RepoKeyVariantHelper<Key>::type());
        bool keyTypeSuccess = true;
        if (Q_UNLIKELY(!std::all_of(
                keys.cbegin(), keys.cend(), 
                [type] (const QVariant& v) { return v.type() == type; })
            ))
        {
            auto it = keys.begin();
            while (it != keys.end())
            {
                bool converted = (*it).convert(type);
                if (converted)
                    ++it;
                else
                    it = keys.erase(it);
                
                keyTypeSuccess &= converted;
            }
        }
        
        return keyTypeSuccess 
            // NOTE: (non short-circuiting & operator)
            & this->addKeys_p(
                  noDetach(keys) 
                | transformed([type](const QVariant& v) { 
                    assert(v.type() == type);
                    auto key_ = aux_config3::RepoKeyVariantHelper<Key>::fromVariant(v, true);
                    assert(key_);
                    return *key_;
                }), 
                true
            );
    }
    else // constexpr (!SupportsLazyInit)
    {
        return false;
    }
}

template<class Interface, class Key>
void Repository<Interface, Key>::addMembers(QList<Addle::GenericRepoMember> members)
{
    using member_type = boost::mp11::mp_if_c<
            SupportsKeyDeduction,
            QSharedPointer<Interface>,
            std::pair<Key, QSharedPointer<Interface>>
        >;
    
    QVarLengthArray<member_type> members_;
    members_.reserve(members.size());
    
    for (const GenericRepoMember& entry : noDetach(members))
    {
//         if constexpr (SupportsKeyDeduction)
//         {
//             aux_config3::conformType<member_type>(member);
//             members_.append(member.value<member_type>());
//         }
//         else
//         {
//             members_.append(
//                     aux_config3::RepoInterfaceVariantHelper<Interface>
//                         ::template pairFromVariant<Key>(member)
//                 );
//         }
        // TODO
    }
    
    this->addMembers_p(members_);
}

template<class Interface, class Key>
bool Repository<Interface, Key>::tryAddMembers(QList<Addle::GenericRepoMember> members) noexcept
{
//     using namespace boost::adaptors;
//     using expected_type = boost::mp11::mp_if_c<
//             SupportsKeyDeduction,
//             QSharedPointer<Interface>,
//             QPair<
//                 typename aux_config3::RepoKeyVariantHelper<Key>::intermediate_type, 
//                 QSharedPointer<Interface>
//             >
//         >;
//         
//     bool convertMembersSuccess = true;
//     if (Q_UNLIKELY(!std::all_of(
//             members.cbegin(), members.cend(), 
//             [] (const QVariant& v) { return v.type() == qMetaTypeId<expected_type>(); })
//         ))
//     {
//         auto it = members.begin();
//         while (it != members.end())
//         {
//             bool converted = (*it).convert(qMetaTypeId<expected_type>());
//             if (converted)
//                 ++it;
//             else
//                 it = members.erase(it);
//             
//             convertMembersSuccess &= converted;
//         }
//     }
//     
//     return convertMembersSuccess 
//         & this->addMembers_p(
//             noDetach(members) | transformed([](const QVariant& v) { 
//                 assert(v.type() == qMetaTypeId<expected_type>());
//                 if constexpr (SupportsKeyDeduction)
//                     return v.value<expected_type>();
//                 else
//                     return aux_config3::RepoInterfaceVariantHelper<Interface>
//                         ::template pairFromVariant<Key>(v, true);
//             }), 
//             true
//         );
}

template<class Interface, class Key>
GenericRepoMemberRef Repository<Interface, Key>::beginGeneric() noexcept 
{
    return GenericMemberRef(this, begin());
}

template<class Interface, class Key>
GenericRepoMemberRef Repository<Interface, Key>::endGeneric() noexcept
{
    return GenericMemberRef(this, end());
}

template<class Interface, class Key>
GenericRepoMemberRef Repository<Interface, Key>::beginGeneric() const noexcept 
{
    return GenericMemberRef(this, begin());
}

template<class Interface, class Key>
GenericRepoMemberRef Repository<Interface, Key>::endGeneric() const noexcept
{
    return GenericMemberRef(this, end());
}

template<class Interface>
class Repository<Interface, void>
    : private aux_config3::Repository_impl<Interface, void>
{
public:
    static constexpr bool SupportsLazyInit = false;
    static constexpr bool SupportsKeyDeduction = true;
    
    using interface_t = Interface;
    using key_t = void;
    
};

template<class Interface, typename Key>
struct Traits::is_singleton<Repository<Interface, Key>> : std::true_type {};

}
