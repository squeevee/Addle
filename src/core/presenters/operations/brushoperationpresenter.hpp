/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHOPERATIONPRESENTER_HPP
#define BRUSHOPERATIONPRESENTER_HPP

#include "compat.hpp"
#include <QObject>

#include "interfaces/presenters/operations/ibrushoperationpresenter.hpp"
#include "interfaces/editing/irasterdiff.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT BrushOperationPresenter : public QObject, public IBrushOperationPresenter
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public: 
    virtual ~BrushOperationPresenter() = default;

    void initialize(
        QWeakPointer<BrushStroke> BrushStroke,
        QWeakPointer<ILayerPresenter> layer
    );

    QString text() { return QString(); } // TODO

public slots: 
    void do_();
    void undo();

private:
    QWeakPointer<ILayerPresenter> _layer;

    QSharedPointer<IRasterDiff> _operation;

    friend class BrushOperationPreview;
};

} // namespace Addle
#endif // BRUSHOPERATIONPRESENTER_HPP