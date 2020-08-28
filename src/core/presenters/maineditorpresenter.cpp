/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "QtDebug"
#include <QApplication>
#include <QtConcurrent>

#include "maineditorpresenter.hpp"

#include "utils.hpp"

#include "servicelocator.hpp"

//#include "interfaces/services/itaskservice.hpp"
#include "interfaces/services/iformatservice.hpp"

//#include "interfaces/tasks/itaskcontroller.hpp"
//#include "interfaces/tasks/iloaddocumentfiletask.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"

#include "interfaces/models/idocument.hpp"
#include "utilities/qobject.hpp"

#include "exceptions/fileexceptions.hpp"

#include "interfaces/services/iapplicationsservice.hpp"

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/icolorselectionpresenter.hpp"

#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"

#include "interfaces/views/imaineditorview.hpp"

#include <QFile>
#include <QImage>

#include "interfaces/presenters/ipalettepresenter.hpp"
#include "interfaces/models/ipalette.hpp"
#include "globals.hpp"

#include "utilities/presenter/genericerrorpresenter.hpp"
#include "exceptions/formatexceptions.hpp"

using namespace Addle;

void MainEditorPresenter::initialize(Mode mode)
{
    const Initializer init(_initHelper);
    
    ServiceLocator::get<IApplicationService>().registerMainEditorPresenter(this);

    _mode = mode;

    _viewPortPresenter = ServiceLocator::makeUnique<IViewPortPresenter>(this);
    _initHelper.setCheckpoint(InitCheck_ViewPortPresenter);

    _canvasPresenter = ServiceLocator::makeUnique<ICanvasPresenter>(std::ref(*this));
    _initHelper.setCheckpoint(InitCheck_CanvasPresenter);
    
    _palettes = { 
        ServiceLocator::makeShared<IPalettePresenter>(CorePalettes::BasicPalette)
    };

    _colorSelection = ServiceLocator::makeUnique<IColorSelectionPresenter>(_palettes);
    _colorSelection->setPalette(_palettes.first());
    _initHelper.setCheckpoint(InitCheck_ColorSelection);

    _brushTool = ServiceLocator::makeShared<IBrushToolPresenter>(
        this,
        IBrushToolPresenter::Mode::Brush
    );

    _eraserTool = ServiceLocator::makeShared<IBrushToolPresenter>(
        this,
        IBrushToolPresenter::Mode::Eraser
    );

    _navigateTool = ServiceLocator::makeShared<INavigateToolPresenter>(
        this
    );

    _tools = {{
        Mode::Editor,
        {
            //{ DefaultTools::Select, nullptr },
            { CoreTools::Brush, _brushTool },
            { CoreTools::Eraser, _eraserTool },
            //{ DefaultTools::Text, nullptr },
            //{ DefaultTools::Shapes, nullptr },
            //{ DefaultTools::Stickers, nullptr },
            //{ DefaultTools::Eyedrop, nullptr },
            { CoreTools::Navigate, _navigateTool }
            //{ DefaultTools::Measure, nullptr }
        }
    }};

    _view = ServiceLocator::makeUnique<IMainEditorView>(std::ref(*this));
    _initHelper.setCheckpoint(InitCheck_View);

    _loadDocumentTask = new LoadDocumentTask(this);
    connect(_loadDocumentTask, &AsyncTask::completed, this, &MainEditorPresenter::onLoadDocumentCompleted);
    connect(_loadDocumentTask, &AsyncTask::failed, this, &MainEditorPresenter::onLoadDocumentFailed);
}

void MainEditorPresenter::setDocumentPresenter(QSharedPointer<IDocumentPresenter> documentPresenter)
{
    ASSERT_INIT;

    auto oldTopSelectedLayer = topSelectedLayer();

    _documentPresenter = documentPresenter;

    if (_connection_topSelectedLayer)
        disconnect(_connection_topSelectedLayer);

    if (_documentPresenter)
        _connection_topSelectedLayer = connect_interface(
            _documentPresenter,
            SIGNAL(topSelectedLayerChanged(QSharedPointer<ILayerPresenter>)),
            this,
            SIGNAL(topSelectedLayerChanged(QSharedPointer<ILayerPresenter>))
        );
    
    _isEmptyCache.recalculate();
    emit documentPresenterChanged(_documentPresenter);

    auto newTopSelectedLayer = topSelectedLayer();
    if (newTopSelectedLayer != oldTopSelectedLayer)
        emit topSelectedLayerChanged(newTopSelectedLayer);
}

QSharedPointer<ILayerPresenter> MainEditorPresenter::topSelectedLayer() const
{
    ASSERT_INIT;
    if (_documentPresenter)
        return _documentPresenter->topSelectedLayer();
    else
        return nullptr;
}

void MainEditorPresenter::setMode(Mode mode)
{
    ASSERT_INIT; 
    _mode = mode;
    //emit
}

void MainEditorPresenter::newDocument()
{
    try
    {
        ASSERT_INIT; 
        if (_mode == Editor && !isEmpty())
        {
            IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
            newPresenter->newDocument();
            newPresenter->view().show();
        }
        else
        {
            //leak
            setDocumentPresenter(
                ServiceLocator::makeShared<IDocumentPresenter>(IDocumentPresenter::initBlankDefaults)
            );
        }
    }
    ADDLE_SLOT_CATCH
}

void MainEditorPresenter::loadDocument(QUrl url)
{
    try
    {
        ASSERT_INIT; 
        if (_mode == Editor && !isEmpty())
        {
            IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
            newPresenter->loadDocument(url);
        }
        else
        {            
            if (!_loadDocumentTask->isRunning())
            {
                _loadDocumentTask->setUrl(url);
                _loadDocumentTask->start();
            }
        }
    }
    ADDLE_SLOT_CATCH
}

void MainEditorPresenter::onLoadDocumentCompleted()
{
    try
    {
        ASSERT_INIT; 
        setDocumentPresenter(_loadDocumentTask->documentPresenter());
        view().show();
    }
    ADDLE_SLOT_CATCH
}

void MainEditorPresenter::onLoadDocumentFailed()
{
    try
    {
        ASSERT_INIT;

        {
            const auto& mainEditorPresenters = ServiceLocator::get<IApplicationService>().mainEditorPresenters();

            ADDLE_ASSERT(mainEditorPresenters.contains(this));

            if (isEmpty() && mainEditorPresenters.size() > 1)
                deleteLater();
        }

        if (typeid(*_loadDocumentTask->error()) == typeid(FormatException))
        {
            auto ex = static_cast<FormatException&>(*_loadDocumentTask->error());
            
            GenericErrorPresenter* errorPresenter = new GenericErrorPresenter();
            errorPresenter->setException(_loadDocumentTask->error());

            QString filename = QFileInfo(_loadDocumentTask->url().toLocalFile()).fileName();

            switch(ex.why())
            {
            case FormatException::FormatNotRecognized:
            case FormatException::WrongModelType:
                errorPresenter->setMessage(
                    //: %1 = the name of the file
                    qtTrId("ui.open-document.invalid-format")
                        .arg(filename)
                );
                break;
            }

            emit error(QSharedPointer<IErrorPresenter>(errorPresenter));
        }
        else
        {
            ADDLE_THROW(*_loadDocumentTask->error());
        }
    }
    ADDLE_SLOT_CATCH
}

void MainEditorPresenter::setCurrentTool(ToolId tool)
{
    ASSERT_INIT; 
    if (tool == _currentTool)
        return;

    const auto& toolPresenters = _tools.value(_mode);
    ADDLE_ASSERT(!tool || toolPresenters.contains(tool));

    _currentTool = tool;
    auto previousTool = _currentToolPresenter;
    _currentToolPresenter = toolPresenters.value(tool);
    emit currentToolChanged(_currentTool);
    emit _currentToolPresenter->setSelected(true);
    if (previousTool)
        emit previousTool->setSelected(false);
}

void LoadDocumentTask::doTask()
{
    QUrl loadedUrl = url();

    if (loadedUrl.isLocalFile())
    {
        QFile file(loadedUrl.toLocalFile());

        DocumentImportExportInfo info;
        info.setFilename(loadedUrl.toLocalFile());

        auto doc = QSharedPointer<IDocument>(ServiceLocator::get<IFormatService>().importModel(file, info));
        setDocumentPresenter(ServiceLocator::makeShared<IDocumentPresenter>(doc));
    }
    else
    {
        //% "Loading a document from a remote URL is not yet supported."
        qWarning() << qUtf8Printable(qtTrId("debug-messages.remote-url-not-supported"));
    }
}