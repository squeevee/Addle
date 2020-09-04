/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IAPPLICATIONERRORVIEW_HPP
#define IAPPLICATIONERRORVIEW_HPP

#include "itoplevelview.hpp"

namespace Addle {

class IErrorPresenter;
class IApplicationErrorView : public ITopLevelView
{
public:
    virtual ~IApplicationErrorView() = default;

    virtual void initialize(IErrorPresenter& presenter) = 0;
    virtual IErrorPresenter& presenter() const = 0;
};

DECL_MAKEABLE(IApplicationErrorView);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IApplicationErrorView, "org.addle.IApplicationErrorView")

#endif // IAPPLICATIONERRORVIEW_HPP