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
#include "interfaces/services/iapplicationservice.hpp"

#include "interfaces/services/ifactory.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/services/ierrorservice.hpp"

#include "interfaces/services/iviewrepository.hpp"
#include "interfaces/views/imaineditorview.hpp"

namespace Addle {

class IErrorService;
class ADDLE_CORE_EXPORT ApplicationService : public QObject, public IApplicationService
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    ApplicationService(
        const IFactory<IMainEditorPresenter>& mainEditorPresenterFactory/*,
        IViewRepository<IMainEditorView>& mainEditorViewRepository*/
    )
        : _mainEditorPresenterFactory(mainEditorPresenterFactory)/*,
        _mainEditorViewRepository(mainEditorViewRepository)*/
    {
    }
    
    virtual ~ApplicationService();

    bool start() override;

    StartupMode startupMode() override { return _startupMode; }

    int exitCode() override { return _exitCode; }

    void registerMainEditorPresenter(IMainEditorPresenter* presenter) override;
    QSet<IMainEditorPresenter*> mainEditorPresenters() const override { return _mainEditorPresenters; }

public slots:
    void quitting() override;

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
    
    const IFactory<IMainEditorPresenter>& _mainEditorPresenterFactory;
//     IViewRepository<IMainEditorView>& _mainEditorViewRepository;
};

} // namespace Addle

#endif // APPLICATIONSERVICE_HPP
