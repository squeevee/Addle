#ifndef IBRUSHLIKETOOLPRESENTER_HPP
#define IBRUSHLIKETOOLPRESENTER_HPP

#include <QObject>
#include <QString>
#include <QIcon>
#include <QSet>
#include <QSharedPointer>

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

#include "itoolwithassetspresenter.hpp"

#include "../assets/ibrushpresenter.hpp"
#include "idtypes/brushid.hpp"

#include "interfaces/traits/metaobjectinfo.hpp"

class IBrushLikeToolPresenter : public virtual IToolPresenter, public virtual IToolWithAssetsPresenter
{
public:
    INTERFACE_META(IBrushLikeToolPresenter)

    virtual ~IBrushLikeToolPresenter() = default;

    virtual BrushId getSelectedBrush() = 0;
    virtual QSharedPointer<IBrushPresenter> getSelectedBrushPresenter() = 0;
};

DECL_INTERFACE_META_PROPERTIES(
    IBrushLikeToolPresenter,
    DECL_INTERFACE_PROPERTY(brush)
)

DECL_IMPLEMENTED_AS_QOBJECT(IBrushLikeToolPresenter)

#endif // IBRUSHLIKETOOLPRESENTER_HPP