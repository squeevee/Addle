#pragma once

#ifdef ADDLE_DEBUG
#include <functional> // std::function
#include <QtDebug>
#include "./qdebug_extensions.hpp"
#else
#warning "ThreadCrumbs is not intended for use in release builds"
#endif

#include <memory>

#include <QList>
#include <QSet>

namespace Addle {

// Recursion preventor, "push" and "pop" a unique identifier before and after a
// function call, and if ThreadCrumbs has already had that id pushed for the
// current thread, it will raise an error.
class ThreadCrumb
{
public:
    template<typename T>
    explicit ThreadCrumb(const T* crumb, unsigned allowance = 1)
        : _entry( push_p(Entry {
              crumb, allowance, !std::is_void_v<T> ? &typeid(T) : nullptr, false
#ifdef ADDLE_DEBUG
            , [crumb] (QDebug debug) { debug << crumb; }
#endif
        }) )
    {
    }
    
    template<class R, class... Args>
    explicit ThreadCrumb(R(*crumb)(Args...), unsigned allowance = 1) 
        : _entry( push_p(Entry {
              reinterpret_cast<const void*>(crumb), allowance, nullptr, true
#ifdef ADDLE_DEBUG
            , [crumb] (QDebug debug) { debug << crumb; }
#endif
        }) )
    {
    }
    
    ~ThreadCrumb()
    {
        if (_entry) 
            pop_p(_entry);
    }
    
    ThreadCrumb(const ThreadCrumb&) = delete;
        
    explicit operator bool () const { return _entry; }
    bool operator! () const { return !_entry; }
    
private:
    struct Entry
    {
        const void* data;
        unsigned allowance;
        
        const std::type_info* type;
        bool isFunction;
        
#ifdef ADDLE_DEBUG
        using debug_impl_t = std::function<void(QDebug)>;
        debug_impl_t debug_impl;
        
        friend QDebug operator<<(QDebug d, const Entry& entry) 
        { 
            if (entry.debug_impl) entry.debug_impl(d); 
            return d; 
        }
#endif
    };
    
    struct Data
    {
        ~Data();
        
        std::vector<Entry> _entries;
        QHash<const void*, std::size_t> _index;
    };
    
    static const Entry* push_p(Entry&& entry);
    static void pop_p(const Entry* entry);
    
    const Entry* _entry;
    
    static thread_local std::unique_ptr<Data> _data;
};

}
