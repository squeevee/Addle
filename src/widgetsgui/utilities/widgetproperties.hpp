#ifndef WIDGETPROPERTIES_HPP
#define WIDGETPROPERTIES_HPP

// Names of properties of QWidgets and other objects relevant to the gui, for
// convenience / consistencty in property bindings. Not exhaustive, may be
// expanded as desired.
namespace WidgetProperties
{
    //common
    const char* const enabled = "enabled";
    const char* const visible = "visible";

    const char* const icon = "icon";
    const char* const cursor = "cursor";
    const char* const text = "text";
    const char* const toolTip = "toolTip";

    const char* const value = "value";

    // sliders
    const char* const minimum = "minimum";
    const char* const maximum = "maximum";
    const char* const pageStep = "pageStep";
    const char* const singleStep = "singleStep";
}

#endif // WIDGETPROPERTIES_HPP