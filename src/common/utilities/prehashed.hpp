#ifndef PREHASHED_HPP
#define PREHASHED_HPP

#include <QHash>

namespace Addle {
    
struct PreHashed
{
    inline PreHashed() : value(0) {}
    inline PreHashed(const PreHashed& x) : value(x.value) {}
    
    template<typename T>
    inline PreHashed(const T& x) : value(qHash(x)) {}
    
    inline PreHashed& operator=(const PreHashed& x) { value = x.value; return *this; }
    
    template<typename T>
    inline PreHashed& operator=(const T& x) { value = qHash(x); return *this; }
    
    inline bool operator==(const PreHashed& x) const { return value == x.value; }
    inline bool operator!=(const PreHashed& x) const { return value != x.value; }
    
    uint value;
    friend uint qHash(PreHashed p, uint seed = 0) { return p.value ^ seed; }
};
    
}

#endif // PREHASHED_HPP
