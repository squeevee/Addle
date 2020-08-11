#ifndef INAVIGATEPRESENTERAUX_HPP
#define INAVIGATEPRESENTERAUX_HPP

#include "compat.hpp"
#include "idtypes/toolid.hpp"
#include <QObject>

// Separte file from INavigateToolPresenter for the benefit of AutoMoc
namespace Addle {

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

} // namespace Addle
#endif // INAVIGATEPRESENTERAUX_HPP