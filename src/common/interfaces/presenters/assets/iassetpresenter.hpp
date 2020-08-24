/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IASSETPRESENTER_HPP
#define IASSETPRESENTER_HPP

#include <QString>
#include <QSharedPointer>
#include "idtypes/addleid.hpp"
namespace Addle {

class IAssetPresenter
{
public:
    virtual ~IAssetPresenter() = default;

    virtual AddleId id() = 0;

    // virtual IToolWithAssetsPresenter* ownerTool() = 0;
    // virtual void setOwnerTool(IToolWithAssetsPresenter* owner) = 0;

    virtual QString name() = 0;
    virtual QIcon icon() = 0;

    virtual void selectInTool() = 0;
};



} // namespace Addle
#endif // IASSETPRESENTER_HPP