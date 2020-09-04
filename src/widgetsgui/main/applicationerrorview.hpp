/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef APPLICATIONERRORVIEW_HPP
#define APPLICATIONERRORVIEW_HPP

#include <memory>

#include <QObject>
#include <QMessageBox>

#include "compat.hpp"

#include "interfaces/views/iapplicationerrorview.hpp"
#include "interfaces/presenters/errors/ierrorpresenter.hpp"

#include "utilities/initializehelper.hpp"

namespace Addle {

class ADDLE_WIDGETSGUI_EXPORT ModalErrorDialog : public QMessageBox
{
    Q_OBJECT
public:
    ModalErrorDialog(IErrorPresenter& presenter, QWidget* parent = nullptr);
    virtual ~ModalErrorDialog() = default;

protected:
    void closeEvent(QCloseEvent* event);

private:
    Q_SIGNAL void closeEventAccepted();

    IErrorPresenter& _presenter;

    friend class ApplicationErrorView;
};

class ADDLE_WIDGETSGUI_EXPORT ApplicationErrorView : public QObject, public IApplicationErrorView
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    virtual ~ApplicationErrorView() = default;

    void initialize(IErrorPresenter& presenter);
    IErrorPresenter& presenter() const { ASSERT_INIT(); return *_presenter; }

public slots:
    void show();
    void close();

signals:
    void closed();

private:
    IErrorPresenter* _presenter;
    std::unique_ptr<ModalErrorDialog> _dialog;

    InitializeHelper _initHelper;
};

} // namespace Addle

#endif // APPLICATIONERRORVIEW_HPP