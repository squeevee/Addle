#ifndef IASSETPRESENTER_HPP
#define IASSETPRESENTER_HPP

#include <QString>
#include <QSharedPointer>
#include "idtypes/persistentid.hpp"
namespace Addle {

class IAssetPresenter
{
public:
    virtual ~IAssetPresenter() = default;

    virtual PersistentId id() = 0;

    // virtual IToolWithAssetsPresenter* ownerTool() = 0;
    // virtual void setOwnerTool(IToolWithAssetsPresenter* owner) = 0;

    virtual QString name() = 0;
    virtual QIcon icon() = 0;

    virtual void selectInTool() = 0;
};

// Q_DECLARE_METATYPE(QSharedPointer<IAssetPresenter>)

} // namespace Addle
#endif // IASSETPRESENTER_HPP