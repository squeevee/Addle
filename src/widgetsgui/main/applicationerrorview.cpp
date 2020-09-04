/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QCloseEvent>

#ifdef ADDLE_DEBUG
#include "exceptions/unhandledexception.hpp"
#endif

#include "applicationerrorview.hpp"

using namespace Addle;

ModalErrorDialog::ModalErrorDialog(IErrorPresenter& presenter, QWidget* parent)
    : QMessageBox(parent), _presenter(presenter)
{
    switch(_presenter.severity())
    {
    case IErrorPresenter::Info:
        setWindowModality(Qt::NonModal);
        setIcon(QMessageBox::Icon::Information);
        break;
    case IErrorPresenter::Warning:
        if (parentWidget())
            setWindowModality(Qt::WindowModal);
        else
            setWindowModality(Qt::NonModal);
        setIcon(QMessageBox::Icon::Warning);
        break;
    case IErrorPresenter::Critical:
        setWindowModality(Qt::ApplicationModal);
        setIcon(QMessageBox::Icon::Critical);
        break;
    }

    setText(presenter.message());

#ifdef ADDLE_DEBUG
    if (presenter.exception() && presenter.exception()->isLogicError())
    {
        // thanks https://stackoverflow.com/a/38371503/2808947
        setStyleSheet(
        "QTextEdit {"
            "font-family: monospace;"
            "min-width: 720px;"
        "}");

        if (typeid(*presenter.exception()) == typeid(UnhandledException))
        {
            auto ex = dynamic_cast<const UnhandledException&>(*presenter.exception());
            if (ex.addleException())
            {
                setDetailedText(ex.addleException()->what());
            }
            else if (ex.stdException())
            {
                setDetailedText(ex.stdException()->what());
            }
        }
    }
#endif
}

void ModalErrorDialog::closeEvent(QCloseEvent* event)
{
    event->accept();
    emit closeEventAccepted();
}

void ApplicationErrorView::initialize(IErrorPresenter& presenter)
{
    const Initializer init(_initHelper);

    _presenter = &presenter;
    _dialog = std::unique_ptr<ModalErrorDialog>(new ModalErrorDialog(presenter));

    connect(_dialog.get(), &ModalErrorDialog::closeEventAccepted, this, &ApplicationErrorView::closed);
    connect(_dialog.get(), &ModalErrorDialog::destroyed, this, &ApplicationErrorView::deleteLater);
}

void ApplicationErrorView::show()
{
    ASSERT_INIT();
    _dialog->show();
}

void ApplicationErrorView::close()
{
    ASSERT_INIT();
    _dialog->close();
}