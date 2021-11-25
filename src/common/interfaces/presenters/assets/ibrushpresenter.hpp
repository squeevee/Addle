/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IBRUSHPRESENTER_HPP
#define IBRUSHPRESENTER_HPP

#include <QObject>
#include <QSharedPointer>

// #include "idtypes/brushid.hpp"
#include "iassetpresenter.hpp"

#include "interfaces/models/ibrush.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {


namespace IBrushPresenterAux
{
    static constexpr double DEFAULT_SIZES[] = { 
        4.0,
        7.0,
        12.0,
        21.0,
        36.0,
        60.0,
        100.0,
        180.0,
        320.0,
        600.0 
    };

    static constexpr double DEFAULT_START_SIZE = 12.0;
}

// Brushes 
class ISizeSelectionPresenter;
class IBrush;
class IBrushPresenter : public IAssetPresenter, public virtual IAmQObject
{
public:
    class PreviewInfoProvider
    {
    public:
        virtual ~PreviewInfoProvider() = default;

        virtual QColor color() const = 0;
        virtual double scale() const = 0;
    };

    virtual ~IBrushPresenter() = default;
    
    virtual void initialize(IBrush& model, QSharedPointer<const PreviewInfoProvider> info = nullptr) = 0;
//     virtual void initialize(BrushId id, QSharedPointer<const PreviewInfoProvider> info = nullptr) = 0;

//     virtual BrushId brushId() = 0;
    virtual IBrush& model() const = 0;

    virtual ISizeSelectionPresenter& sizeSelection() = 0;

    virtual double size() = 0;
    virtual void setSize(double size) = 0;

    virtual void setPreviewInfo(QSharedPointer<const PreviewInfoProvider> info) = 0;
};

namespace aux_IBrushPresenter {
    using namespace config_detail::generic_id_parameter;
}

// ADDLE_DECL_UNIQUE_REPO_MEMBER(IBrushPresenter, BrushId)

} // namespace Addle
#endif // IBRUSHPRESENTER_HPP
