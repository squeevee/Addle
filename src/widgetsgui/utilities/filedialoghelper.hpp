/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FILEDIALOGHELPER_HPP
#define FILEDIALOGHELPER_HPP

#include <QObject>
#include <QFileDialog>

#include "utilities/presenter/filerequest.hpp"
#include "utilities/presenter/presenterassignment.hpp"

namespace Addle {

/**
 * @class FileDialogHelper
 * 
 * Follows the progress of a FileRequest and deploys one or more file dialogs as
 * they are needed. Automatically populates the dialogs with updated information
 * from the Request -- e.g., if the Presenter has suggested names, locations, or
 * knowledge of more specific intent from the user.
 * 
 * Native file dialogs are generally preferred, because on most modern platforms
 * (i.e., since 1999) they provide thumbnails for image files (a feature that is
 * indispensable for quality of life in an image editor).
 * 
 * However, what programmatic functionality the native file dialog supports
 * varies wildly by QPA plugin implementation and in many cases the underlying
 * platform. As a rule the following cannot be assumed to work:
 * - Modifying the state of the file dialog after it has been opened (and
 *   expecting the changes to be perpetuated to the UI).
 * - Preventing the file dialog from closing itself when a file is selected or
 *   when the "Cancel" or "X" button are pushed.
 * 
 * This is annoying for our purposes because the validity of a file / filename
 * for loading or saving can't be known until partway through the request.
 * 
 * Unless/until QPA platform plugins widely support the above async features
 * (unlikely) or Qt's Widget-based file dialog achieves a user experience on par
 * with native ones (unlikely), the best solution will be to simply open a new 
 * QFileDialog every time something changes.
 */
class FileDialogHelper : public QObject
{
    Q_OBJECT
public:
    FileDialogHelper(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    virtual ~FileDialogHelper() = default;

    void setRequest(QSharedPointer<FileRequest> request);

private slots:
    void onUrlNeeded();
    void onRequestDone();
    void onUrlChanged(QUrl url);

    void onAccepted();
    void onRejected();

private:
    void makeDialog();
    QStringList makeNameFilters();

    QSharedPointer<FileRequest> _request;

    QPointer<QFileDialog> _dialog;

    QMetaObject::Connection _connection_onUrlNeeded;
    QMetaObject::Connection _connection_onUrlAccepted;
    QMetaObject::Connection _connection_onUrlChanged;
    QMetaObject::Connection _connection_onRequestDone;
};

} // namespace Addle

#endif // FILEDIALOGHELPER_HPP