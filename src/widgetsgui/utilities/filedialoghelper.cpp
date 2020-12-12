/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QtDebug>
#include <QTimer>
#include <QStringBuilder>

#include <QCloseEvent>
#include <QMetaObject>
#include "filedialoghelper.hpp"
#include "utils.hpp"

#include "utilities/collatingstring.hpp"
#include "utilities/idinfo.hpp"

using namespace Addle;

void FileDialogHelper::setRequest(QSharedPointer<FileRequest> request)
{
    _request = request;

    if (_connection_onUrlNeeded)
        disconnect(_connection_onUrlNeeded);
    
    if (_connection_onUrlAccepted)
        disconnect(_connection_onUrlAccepted);

    if (_connection_onRequestDone)
        disconnect(_connection_onRequestDone);

    if (_connection_onUrlChanged)
        disconnect(_connection_onUrlChanged);

    if (_dialog)
        delete _dialog;

    if (_request)
    {
        _connection_onUrlNeeded = connect(
            request.data(), &FileRequest::urlNeeded,
                       this, &FileDialogHelper::onUrlNeeded);

        _connection_onRequestDone = connect(
            request.data(), &FileRequest::done,
                       this, &FileDialogHelper::onRequestDone);

        _connection_onUrlChanged = connect(
            request.data(), &FileRequest::urlChanged,
                this, &FileDialogHelper::onUrlChanged);
    }
}

void FileDialogHelper::onUrlNeeded()
{
    ADDLE_ASSERT(_request);

    if (!_dialog)
        makeDialog();

    _dialog->open();
}

void FileDialogHelper::onRequestDone()
{
    setRequest(nullptr);

    if (_dialog)
        _dialog->close();
}
void FileDialogHelper::onUrlChanged(QUrl url)
{
    if (_dialog && _dialog->isVisible())
    {
        _dialog->selectUrl(url);
        _dialog->open();
    }
}

void FileDialogHelper::onAccepted()
{
//     _request->setUrl(
//             cpplinq::from(_dialog->selectedUrls())
//         >>  cpplinq::first_or_default()
//     );
}

void FileDialogHelper::onRejected()
{

}

void FileDialogHelper::makeDialog()
{
    ADDLE_ASSERT(_request);
    if (_dialog) return;

    _dialog = new QFileDialog(qobject_cast<QWidget*>(QObject::parent()));

    _dialog->setAttribute(Qt::WA_DeleteOnClose);

    switch(_request->action())
    {
    case FileRequest::Save:
        _dialog->setAcceptMode(QFileDialog::AcceptSave);
        _dialog->setOption(QFileDialog::DontConfirmOverwrite);
        break;
    case FileRequest::Load:
        _dialog->setAcceptMode(QFileDialog::AcceptOpen);
        break;
    default:
        ADDLE_LOGICALLY_UNREACHABLE();
    }
    
    if (!_request->url().isEmpty())
        _dialog->selectUrl(_request->url());
    else if (!_request->directory().isEmpty())
        _dialog->setDirectory(_request->directory());
    
    _dialog->setNameFilters(makeNameFilters());
    
    connect(_dialog, &QFileDialog::accepted, this, &FileDialogHelper::onAccepted);
    connect(_dialog, &QFileDialog::rejected, this, &FileDialogHelper::onRejected);
}

QStringList FileDialogHelper::makeNameFilters()
{
    static const char* const FILE_FILTER_TRID = 
        //% "%1 (%2)"
        QT_TRID_NOOP("general.templates.file-filter");

    ADDLE_ASSERT(_request);

    QStringList result;
    QStringList allExtensions;  

//     for (auto format :
//             cpplinq::from(_request->availableFormats())
//         >>  cpplinq::orderby_descending(
//                 [this] (GenericFormatId format) {
//                     return format == _request->favoriteFormat();
//                 })
//         >>  cpplinq::thenby_ascending(
//                 [] (GenericFormatId format) -> CollatingString {
//                     return format.name();
//                 }))
//     {
//         QStringList filterExtensions = 
//                 cpplinq::from(format.fileExtensions())
//             >>  cpplinq::select(
//                     [] (const QString& ext) -> QString {
//                         return "*." % ext;
//                     })
//             >>  cpplinq::to_QList();
// 
//         result <<
//             qtTrId(FILE_FILTER_TRID)
//                 .arg(format.name())
//                 .arg(filterExtensions.join(' '));
// 
//         allExtensions.append(filterExtensions);
//     }

    QString allFormatsFilter = 
        qtTrId(FILE_FILTER_TRID)
            .arg(_request->allFormatsLabelTrId() ? 
                qtTrId(_request->allFormatsLabelTrId()) :
                //% "All supported formats"
                qtTrId("ui.file-dialog.generic-all-formats-filter"))
            .arg(allExtensions.join(' '));

    if (_request->action() == FileRequest::Save)
        result.append(allFormatsFilter);
    else
        result.prepend(allFormatsFilter);

    result <<
        qtTrId(FILE_FILTER_TRID)
            .arg(
                //% "All files"
                qtTrId("ui.file-dialog.all-files-filter")
            )
            .arg('*');

    return result;
}
