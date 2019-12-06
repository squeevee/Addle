#include <QCoreApplication>
#include <QCommandLineParser>

#include <QtDebug>
#include <iostream>
#include <cstdlib>

#include "applicationservice.hpp"

#include "servicelocator.hpp"

#include "utilities/qt_extensions/qobject.hpp"
#include "globalconstants.hpp"
#include "exceptions/commandlineexceptions.hpp"

#ifdef ADDLE_DEBUG
#include "utilities/debugging.hpp"
#endif

ApplicationService::~ApplicationService()
{   
}

bool ApplicationService::start()
{
#ifdef ADDLE_DEBUG
    qDebug() << qUtf8Printable(QCoreApplication::translate(
        "ApplicationService",
        "Starting ApplicationService."
    ));
#endif

    try
    {
        parseCommandLine();
    }
    catch (CommandLineException& ex)
    {
        _startupMode = StartupMode::terminal;
        _exitCode = GlobalConstants::ErrorCodes::COMMAND_LINE_ERROR_CODE;

#ifdef ADDLE_DEBUG
        std::cerr << qPrintable(QCoreApplication::translate(
            "handleCommandLineError",
            "A command line error was encountered."
        )) << std::endl;
        std::cerr << ex.what() << std::endl;
        if (DebugBehavior::test(DebugBehavior::abort_on_startup_error))
            std::abort();
#else
        std::cerr << qPrintable(ex.getErrorText()) << std::endl;
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
            std::cerr << qPrintable(QCoreApplication::translate(
                "ApplicationService",
                "An error occurred attempting to start the graphical application."
            )) << std::endl;

#ifdef ADDLE_DEBUG
            try
            {
                std::rethrow_exception(std::current_exception());
            }
            catch (std::exception& ex)
            {
                std::cerr << qPrintable(QCoreApplication::translate(
                    "ApplicationService",
                    "Exception thrown:"
                )) << std::endl;
                std::cerr << ex.what() << std::endl;
                if (DebugBehavior::test(DebugBehavior::abort_on_startup_error))
                    std::abort();
            }
            catch(...)
            {
                std::cerr << qPrintable(QCoreApplication::translate(
                    "ApplicationService",
                    "The error was not of type std::exception"
                )) << std::endl;
            }
            if (DebugBehavior::test(DebugBehavior::abort_on_startup_error))
                std::abort();
#endif
            _exitCode = GlobalConstants::ErrorCodes::UNKNOWN_ERROR_CODE;
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

    parser.setApplicationDescription(
        QCoreApplication::translate(
            "ApplicationService",
            "Quick & simple drawing program / image utility"
        )
    );

    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();

    QCommandLineOption editorOption(
        {
            IApplicationService::CMD_EDITOR_SHORT_OPTION,
            IApplicationService::CMD_EDITOR_OPTION
        }, 
        QCoreApplication::translate("ApplicationService", "Explicitly start Addle in editor mode.")
    );
    parser.addOption(editorOption);

    QCommandLineOption viewerOption(
        {
            IApplicationService::CMD_BROWSER_SHORT_OPTION,
            IApplicationService::CMD_BROWSER_OPTION
        },
        QCoreApplication::translate("ApplicationService", "Explicitly start Addle in browser mode.")
    );
    parser.addOption(viewerOption);

    parser.addPositionalArgument(
        QCoreApplication::translate("ApplicationService", "open"),
        QCoreApplication::translate("ApplicationService", "The file or url to open.")
    );

    bool success = parser.parse(args);

    if (!success)
    {
        ADDLE_THROW(CommandLineParserException(parser.errorText()));
    }

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
            qDebug() << qUtf8Printable(QCoreApplication::translate(
                "ApplicationService",
                "Command line arguments specified to open url \"%1\""
            ).arg(_startingUrl.toString()));
#endif
        }
        else
        {
            _startingFilename = openString;
#ifdef ADDLE_DEBUG
            qDebug() << qUtf8Printable(QCoreApplication::translate(
                "ApplicationService",
                "Command line arguments specified to open file \"%1\""
            ).arg(_startingFilename));
#endif
        }

        if (parser.positionalArguments().size() > 1)
        {
            qWarning() << qUtf8Printable(QCoreApplication::translate(
                "ApplicationService",
                "Multiple positional arguments were given, but only the first will be used."
            ));
        }
    }

    if (parser.isSet(editorOption) && parser.isSet(viewerOption))
    {
        ADDLE_THROW(MultipleStartModesException());
    }
    else if (parser.isSet(editorOption))
    {
        _startupMode = StartupMode::editor;

#ifdef ADDLE_DEBUG
        qDebug() << qUtf8Printable(QCoreApplication::translate(
            "ApplicationService",
            "Explicitly starting in editor mode."
        ));
#endif
    }
    else if (parser.isSet(viewerOption))
    {
        _startupMode = StartupMode::browser;

#ifdef ADDLE_DEBUG
        qDebug() << qUtf8Printable(QCoreApplication::translate(
            "ApplicationService",
            "Explicitly starting in browser mode."
        ));
#endif
    }
    else if (!parser.isSet(editorOption) && !parser.isSet(viewerOption))
    {
        if (_startingFilename.isNull() || _startingUrl.isEmpty())
        {
            _startupMode = StartupMode::editor;
#ifdef ADDLE_DEBUG
            qDebug() << qUtf8Printable(QCoreApplication::translate(
                "ApplicationService",
                "Implicitly starting in editor mode."
            ));
#endif
        }
        else
        {
            _startupMode = StartupMode::browser;
#ifdef ADDLE_DEBUG
            qDebug() << qUtf8Printable(QCoreApplication::translate(
                "ApplicationService",
                "Implicitly starting in browser mode."
            ));
#endif
        }
    }
}

void ApplicationService::startGraphicalApplication()
{
    IDocumentPresenter* presenter;
    //if (_startupMode == StartupMode::editor) always use editor view for now.
    //{
    presenter = ServiceLocator::make<IEditorPresenter>();

    //}

    if (!_startingFilename.isNull())
    {
        presenter->loadDocument(QFileInfo(_startingFilename));
    }
    else if (!_startingUrl.isEmpty())
    {
        presenter->loadDocument(_startingUrl);
    }

    presenter->getDocumentView()->start();
}

void ApplicationService::quitting()
{
#ifdef ADDLE_DEBUG
    qDebug() << qUtf8Printable(QCoreApplication::translate(
        "ApplicationService",
        "quitting() called"
    ));
#endif
}