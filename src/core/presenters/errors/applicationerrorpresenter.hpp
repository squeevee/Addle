/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef APPLICATIONERRORPRESENTER_HPP
#define APPLICATIONERRORPRESENTER_HPP

#include <QObject>

#include "compat.hpp"
#include "interfaces/presenters/errors/iapplicationerrorpresenter.hpp"
#include "interfaces/views/iapplicationerrorview.hpp"

#include "exceptions/addleexception.hpp"

#include "utilities/initializehelper.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT ApplicationErrorPresenter : public QObject, public IApplicationErrorPresenter
{
    Q_OBJECT

    static const int InitCheck_ErrorSet = 0;
public:
    virtual ~ApplicationErrorPresenter() = default;

    void initialize(QSharedPointer<AddleException> error);

    Severity severity() const { ASSERT_INIT_CHECKPOINT(InitCheck_ErrorSet); return Severity::Critical; }
    QString message() const;

    virtual IApplicationErrorView& view() const { ASSERT_INIT; return *_view; }

    QSharedPointer<AddleException> exception() const { ASSERT_INIT_CHECKPOINT(InitCheck_ErrorSet); return _error; }

private:
    QSharedPointer<AddleException> _error;

    IApplicationErrorView* _view;

    InitializeHelper _initHelper;
};

} // namespace Addle

#endif // APPLICATIONERRORPRESENTER_HPP