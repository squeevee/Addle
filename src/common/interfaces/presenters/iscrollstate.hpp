#ifndef ISCROLLSTATE_HPP
#define ISCROLLSTATE_HPP
namespace Addle {

class IScrollState
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

} // namespace Addle
#endif // ISCROLLSTATE_HPP