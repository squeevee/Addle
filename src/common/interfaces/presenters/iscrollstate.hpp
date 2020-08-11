#ifndef ISCROLLSTATE_HPP
#define ISCROLLSTATE_HPP
namespace Addle {

class IScrollState
{
public:
    virtual ~IScrollState() = default;

    virtual int pageWidth() const = 0;
    virtual int pageHeight() const = 0;

    virtual int horizontalValue() const = 0;
    virtual int verticalValue() const = 0;

    virtual int horizontalMinimum() const = 0;
    virtual int verticalMinimum() const = 0;

    virtual int horizontalMaximum() const = 0;
    virtual int verticalMaximum() const = 0;
};

} // namespace Addle
#endif // ISCROLLSTATE_HPP