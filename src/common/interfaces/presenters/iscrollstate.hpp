#ifndef ISCROLLSTATE_HPP
#define ISCROLLSTATE_HPP
#include "interfaces/traits/compat.hpp"

class ADDLE_COMMON_EXPORT IScrollState
{
public:
    virtual ~IScrollState() = default;

    virtual int getPageWidth() const = 0;
    virtual int getPageHeight() const = 0;

    virtual int getHorizontalValue() const = 0;
    virtual int getVerticalValue() const = 0;

    virtual int getHorizontalMinimum() const = 0;
    virtual int getVerticalMinimum() const = 0;

    virtual int getHorizontalMaximum() const = 0;
    virtual int getVerticalMaximum() const = 0;
};

#endif // ISCROLLSTATE_HPP