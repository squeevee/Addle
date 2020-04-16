#ifndef IASSETPRESENTER_HPP
#define IASSETPRESENTER_HPP

#include <QString>
#include <QSharedPointer>
#include "idtypes/persistentid.hpp"
#include "interfaces/traits/compat.hpp"

class IToolWithAssetsPresenter;
class ADDLE_COMMON_EXPORT IAssetPresenter
{
public:
    virtual ~IAssetPresenter() = default;

    virtual PersistentId getId() = 0;

    virtual IToolWithAssetsPresenter* getOwnerTool() = 0;
    virtual void setOwnerTool(IToolWithAssetsPresenter* owner) = 0;

    virtual void selectInTool() = 0;
};

Q_DECLARE_METATYPE(QSharedPointer<IAssetPresenter>)

#endif // IASSETPRESENTER_HPP