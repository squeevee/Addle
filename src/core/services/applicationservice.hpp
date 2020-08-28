/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef APPLICATIONSERVICE_HPP
#define APPLICATIONSERVICE_HPP

#include "compat.hpp"
#include <QObject>
#include <QUrl>
#include "interfaces/services/iapplicationsservice.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT ApplicationService : public QObject, public IApplicationService
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:

    virtual ~ApplicationService();

    bool start();

    StartupMode startupMode() { return _startupMode; }

    int exitCode() { return _exitCode; }

    void registerMainEditorPresenter(IMainEditorPresenter* presenter);
    QSet<IMainEditorPresenter*> mainEditorPresenters() const { return _mainEditorPresenters; }

public slots:
    void quitting();

private slots:
    void onMainEditorPresenterDestroyed();

private:
    void parseCommandLine();
    void startGraphicalApplication();

    QUrl _startingUrl;
    QString _startingFilename;

    StartupMode _startupMode;
    int _exitCode;

    QSet<IMainEditorPresenter*> _mainEditorPresenters;
    QHash<QObject*, IMainEditorPresenter*> _mainEditorPresenters_byQObjects;
};

} // namespace Addle

#endif // APPLICATIONSERVICE_HPP