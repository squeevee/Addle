#ifndef INAVIGATEPRESENTERAUX_HPP
#define INAVIGATEPRESENTERAUX_HPP

#include "compat.hpp"
#include "idtypes/toolid.hpp"
#include <QObject>

// Separte file for the benefit of AutoMoc

namespace INavigateToolPresenterAux
{
    Q_NAMESPACE_EXPORT(ADDLE_COMMON_EXPORT)

    extern ADDLE_COMMON_EXPORT const ToolId ID;

    enum NavigateOperationOptions {
        gripPan,
        gripPivot,
        rectangleZoomTo
    };
    Q_ENUM_NS(NavigateOperationOptions)

    const NavigateOperationOptions DEFAULT_NAVIGATE_OPERATION_OPTION = gripPan;
}

#endif // INAVIGATEPRESENTERAUX_HPP