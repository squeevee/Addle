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

class IBrushLikeToolPresenter : public virtual IToolPresenter, public virtual IToolWithAssetsPresenter
{
public:
    virtual ~IBrushLikeToolPresenter() = default;

    virtual BrushId getSelectedBrush() = 0;
    virtual QSharedPointer<IBrushPresenter> getSelectedBrushPresenter() = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IBrushLikeToolPresenter)

#endif // IBRUSHLIKETOOLPRESENTER_HPP