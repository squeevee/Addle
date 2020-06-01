#ifndef PRESETHELPER2_HPP
#define PRESETHELPER2_HPP

// I'm thinking it's a better approach for presets to be collections of numeric
// values, rather than the quasi-static correlated enum thing I was trying to do
// before.
//
// Presenters that handle presets will expose this thorugh their interfaces.
// This is a helper class for presenter implementations that provides common
// preset-related logic. The old way is obsolete and should be removed at the
// earliest convenience, but this is PresetHelper2 for the brief time being.

template<typename ValueType = double>
class PresetHelper2
{
public:
    //PresetHelper2(ValueType margin)

};

#endif // PRESETHELPER2_HPP