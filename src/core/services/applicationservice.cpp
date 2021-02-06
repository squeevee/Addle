/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QCoreApplication>
#include <QCommandLineParser>

#include <QtDebug>
#include <iostream>
#include <cstdlib>

#include "utils.hpp"

#include "applicationservice.hpp"

#include "servicelocator.hpp"

#include "interfaces/services/ierrorservice.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/views/imaineditorview.hpp"

#include "utilities/qobject.hpp"
#include "globals.hpp"
#include "exceptions/commandlineexceptions.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging/debugbehavior.hpp"
#endif

using namespace Addle;
ApplicationService::~ApplicationService()
{   
}

bool ApplicationService::start()
{
#ifdef ADDLE_DEBUG
    //% "Starting ApplicationService."
    qDebug() << qUtf8Printable(qtTrId("debug-messages.application-service.starting"));
#endif

    //ServiceLocator::get<const IErrorService>();

    try
    {
        parseCommandLine();
    }
    catch (CommandLineException& ex)
    {
        _startupMode = StartupMode::terminal;
        _exitCode = ErrorCodes::COMMAND_LINE_ERROR_CODE;

#ifdef ADDLE_DEBUG
        std::cerr << qPrintable(
            //% "A command line error was encountered."
            qtTrId("debug-messages.application-service.command-line-error")
        ) << std::endl;
        std::cerr << ex.what() << std::endl;
        if (DebugBehavior::test(DebugBehavior::AbortOnStartupError))
            std::terminate();
#else
        std::cerr << qPrintable(ex.errorText()) << std::endl;
#endif

    }

    if (_startupMode == StartupMode::terminal)
    {
        quitting();
        return false;
    }
    else
    {
        try
        {
            startGraphicalApplication();
        }
        catch (...)
        {       
            std::cerr << qPrintable(
                //% "An error occurred attempting to start the graphical application."
                qtTrId("cli-messages.graphical-startup-error")
            ) << std::endl;

#ifdef ADDLE_DEBUG
            try
            {
                std::rethrow_exception(std::current_exception());
            }
            catch (std::exception& ex)
            {
                std::cerr << qPrintable(
                    //% "  exception thrown:"
                    qtTrId("debug-messages.application-service.standard-exception")
                ) << std::endl;
                std::cerr << ex.what() << std::endl;
                if (DebugBehavior::test(DebugBehavior::AbortOnStartupError))
                    std::terminate();
            }
            catch(...)
            {
                std::cerr << qPrintable(
                    //% "The error was not of type std::exception"
                    qtTrId("debug-messages.application-service.non-standard-exception")
                ) << std::endl;
            }
            if (DebugBehavior::test(DebugBehavior::AbortOnStartupError))
                std::terminate();
#endif
            _exitCode = ErrorCodes::UNKNOWN_ERROR_CODE;
            quitting();
            return false;
        }
        return true;
    }
}

void ApplicationService::parseCommandLine()
{
    QStringList args = QCoreApplication::arguments();

    QCommandLineParser parser;

    parser.setApplicationDescription(qtTrId(ADDLE_TAGLINE_TRID));

    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();

    QCommandLineOption editorOption(
        {
            "e",
            "editor"
        }, 
        //% "Explicitly start Addle in editor mode."
        qtTrId("cli-messages.options.editor-mode-description")
    );
    parser.addOption(editorOption);

    QCommandLineOption browserOption(
        {
            "b",
            "browser"
        },
        //% "Explicitly start Addle in browser mode."
        qtTrId("cli-messages.options.browser-mode-description")
    );
    parser.addOption(browserOption);

    parser.addPositionalArgument(
        //% "open"
        qtTrId("cli-messages.options.open-name"),
        //% "The file or url to open."
        qtTrId("cli-messages.options.open.description")
    );

    if (!parser.parse(args))
        ADDLE_THROW(CommandLineParserException(parser.errorText()));

    if (parser.isSet(helpOption))
    {
        _startupMode = StartupMode::terminal;
        _exitCode = 0;
        std::cout << qPrintable(parser.helpText());
        return;
    }

    if (parser.isSet(versionOption))
    {
        _startupMode = StartupMode::terminal;
        _exitCode = 0;
        std::cout << qPrintable(
            QCoreApplication::applicationName()
            % " " % QCoreApplication::applicationVersion()
            ) << std::endl;
        return;
    }

    if (!parser.positionalArguments().isEmpty())
    {
        QString openString = parser.positionalArguments()[0];
        QUrl openUrl = QUrl(openString);
        if (openUrl.isValid() && !openUrl.isRelative())
        {
            _startingUrl = openUrl;
#ifdef ADDLE_DEBUG
            qDebug() << qUtf8Printable(
                //% "Command line arguments specified to open url \"%1\""
                qtTrId("debug-messages.application-service.cli-open-url")
                    .arg(_startingUrl.toString())
            );
#endif
        }
        else
        {
            _startingFilename = openString;
#ifdef ADDLE_DEBUG
            qDebug() << qUtf8Printable(
                //% "Command line arguments specified to open file \"%1\""
                qtTrId("debug-messages.application-service.cli-open-file")
                    .arg(_startingFilename)
            );
#endif
        }

        if (parser.positionalArguments().size() > 1)
        {
            qWarning() << qUtf8Printable(
                //% "Multiple positional arguments were given, but only the first will be used."
                qtTrId("cli-messages.multiple-positional-arguments-warning")
            );
        }
    }

    if (parser.isSet(editorOption) && parser.isSet(browserOption))
    {
        ADDLE_THROW(MultipleStartModesException());
    }
    else if (parser.isSet(editorOption))
    {
        _startupMode = StartupMode::editor;

#ifdef ADDLE_DEBUG
        qDebug() << qUtf8Printable(
            //% "Explicitly starting in editor mode."
            qtTrId("debug-messages.application-service.starting-editor-explicit")
        );
#endif
    }
    else if (parser.isSet(browserOption))
    {
        _startupMode = StartupMode::browser;

#ifdef ADDLE_DEBUG
        qDebug() << qUtf8Printable(
            //% "Explicitly starting in browser mode."
            qtTrId("debug-messages.application-service.starting-browser-explicit")
        );
#endif
    }
    else if (!parser.isSet(editorOption) && !parser.isSet(browserOption))
    {
        if (_startingFilename.isNull() || _startingUrl.isEmpty())
        {
            _startupMode = StartupMode::editor;
#ifdef ADDLE_DEBUG
            qDebug() << qUtf8Printable(
                //% "Implicitly starting in editor mode."
                qtTrId("debug-messages.application-service.starting-editor-implicit")
            );
#endif
        }
        else
        {
            _startupMode = StartupMode::browser;
#ifdef ADDLE_DEBUG
            qDebug() << qUtf8Printable(
                //% "Implicitly starting in browser mode."
                qtTrId("debug-messages.application-service.starting-browser-implicit")
            );
#endif
        }
    }
}

void ApplicationService::startGraphicalApplication()
{
    IMainEditorPresenter* presenter;
    //if (_startupMode == StartupMode::editor) always use editor view for now.
    //{
    presenter = _mainEditorPresenterFactory.make(IMainEditorPresenter::Editor);

    //}

    if (!_startingFilename.isNull())
    {
        //presenter->loadDocument(QUrl::fromLocalFile(_startingFilename));
    }
    else if (!_startingUrl.isEmpty())
    {
        //presenter->loadDocument(_startingUrl);
    }

     _mainEditorViewRepository.add(*presenter);
     _mainEditorViewRepository.get(*presenter).open();
}

void ApplicationService::quitting()
{
#ifdef ADDLE_DEBUG
    qDebug() << qUtf8Printable(
        //% "Quitting application service"
        qtTrId("debug-messages.application-service.quitting")
    );
#endif
}

void ApplicationService::registerMainEditorPresenter(IMainEditorPresenter* presenter)
{
    _mainEditorPresenters.insert(presenter);
    _mainEditorPresenters_byQObjects[qobject_interface_cast<QObject*>(presenter)] = presenter;
    connect_interface(presenter, SIGNAL(destroyed()), this, SLOT(onMainEditorPresenterDestroyed()));
}

void ApplicationService::onMainEditorPresenterDestroyed()
{
    try 
    {
        // can't qobject_cast on a destroyed() signal
        auto presenter = _mainEditorPresenters_byQObjects[sender()];
        
        ADDLE_ASSERT(presenter);
        ADDLE_ASSERT(_mainEditorPresenters.contains(presenter));
        _mainEditorPresenters.remove(presenter);
        _mainEditorPresenters_byQObjects.remove(sender());
    }
    ADDLE_SLOT_CATCH
}
