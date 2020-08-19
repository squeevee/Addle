<?xml version='1.0' encoding='UTF-8'?>
<TS version="2.1" language="en_US">
<context>
    <name />
    <message id="debug-messages.starting-addle">
        <location filename="../../src/addle/addle.cpp" line="49" />
        <source>Starting Addle. This is a debug build.</source>
        <translation type="finished">Starting Addle. This is a debug build.</translation>
    </message>
    <message id="debug-messages.behavior-flags-set">
        <location filename="../../src/addle/addle.cpp" line="54" />
        <source>Debug behavior flag(s) set</source>
        <translation type="finished">Debug behavior flag(s) set</translation>
    </message>
    <message id="units.pixels">
        <location filename="../../src/common/common.cpp" line="202" />
        <location filename="../../src/widgetsgui/main/sizeselector.cpp" line="41" />
        <source>px</source>
        <translation type="finished">px</translation>
    </message>
    <message id="debug-messages.logic-error-occurred">
        <location filename="../../src/common/exceptions/addleexception.cpp" line="35" />
        <source>A logic error occurred:
%1</source>
        <translation type="finished">A logic error occurred:
%1</translation>
    </message>
    <message id="debug-messages.exception-location-formatter">
        <location filename="../../src/common/exceptions/addleexception.cpp" line="60" />
        <source>%1 (%2:%3)
</source>
        <translation type="finished">%1 (%2:%3)
</translation>
    </message>
    <message id="debug-messages.exception-location-list-formatter">
        <location filename="../../src/common/exceptions/addleexception.cpp" line="69" />
        <source>%1
thrown from:
%2</source>
        <translation type="finished">%1
thrown from:
%2</translation>
    </message>
    <message id="error-messages.command-line.parser">
        <location filename="../../src/common/exceptions/commandlineexceptions.hpp" line="44" />
        <source>The command-line parser encountered an error: "%1"</source>
        <translation type="finished">The command-line parser encountered an error: "%1"</translation>
    </message>
    <message id="error-messages.command-line.multiple-modes">
        <location filename="../../src/common/exceptions/commandlineexceptions.hpp" line="61" />
        <source>You may not specify multiple modes to start Addle in.</source>
        <translation type="finished">You may not specify multiple modes to start Addle in.</translation>
    </message>
    <message id="debug-messages.file-error.does-not-exist">
        <location filename="../../src/common/exceptions/fileexceptions.hpp" line="49" />
        <source>The requested file at "%1" does not exist.</source>
        <translation type="finished">The requested file at "%1" does not exist.</translation>
    </message>
    <message id="debug-messages.format-error.model-not-supported">
        <location filename="../../src/common/exceptions/formatexceptions.hpp" line="60" />
        <source>Importing is not supported for the given format model.
 Model name: "%1"</source>
        <translation type="finished">Importing is not supported for the given format model.
 Model name: "%1"</translation>
    </message>
    <message id="debug-messages.format-error.format-not-supported">
        <location filename="../../src/common/exceptions/formatexceptions.hpp" line="92" />
        <source>There is no driver for the given format.
 format type: "%1"
    filename: "%2"</source>
        <translation type="finished">There is no driver for the given format.
 format type: "%1"
    filename: "%2"</translation>
    </message>
    <message id="debug-messages.format-error.import-not-supported">
        <location filename="../../src/common/exceptions/formatexceptions.hpp" line="124" />
        <source>The driver for the format does not support importing.
format type: "%1"
   filename: "%2"</source>
        <translation type="finished">The driver for the format does not support importing.
format type: "%1"
   filename: "%2"</translation>
    </message>
    <message id="debug-messages.format-error.inferrence-failed">
        <location filename="../../src/common/exceptions/formatexceptions.hpp" line="155" />
        <source>A format could not be inferred from the information given.
   filename: "%1"</source>
        <translation type="finished">A format could not be inferred from the information given.
   filename: "%1"</translation>
    </message>
    <message id="debug-messages.initialize-error.not-initialized">
        <location filename="../../src/common/exceptions/initializeexceptions.hpp" line="58" />
        <source>Attempted operation on object that was not yet initialized.</source>
        <translation type="finished">Attempted operation on object that was not yet initialized.</translation>
    </message>
    <message id="debug-messages.initialize-error.already-initialized">
        <location filename="../../src/common/exceptions/initializeexceptions.hpp" line="86" />
        <source>Attempted to initialize object that was already initialized.</source>
        <translation type="finished">Attempted to initialize object that was already initialized.</translation>
    </message>
    <message id="debug-messages.initialize-error.other">
        <location filename="../../src/common/exceptions/initializeexceptions.hpp" line="117" />
        <source>A problem was encountered during initialization.
Why code:    %2</source>
        <translation type="finished">A problem was encountered during initialization.
Why code:    %2</translation>
    </message>
    <message id="debug-messages.service-locator-error.not-initialized">
        <location filename="../../src/common/exceptions/servicelocatorexceptions.hpp" line="52" />
        <source>The service locator is not initialized.</source>
        <translation type="finished">The service locator is not initialized.</translation>
    </message>
    <message id="debug-messages.service-locator-error.already-initialized">
        <location filename="../../src/common/exceptions/servicelocatorexceptions.hpp" line="77" />
        <source>The service locator has already been initialized.</source>
        <translation type="finished">The service locator has already been initialized.</translation>
    </message>
    <message id="debug-messages.service-locator-error.factory-not-found">
        <location filename="../../src/common/exceptions/servicelocatorexceptions.hpp" line="106" />
        <source>No factory was found for the requested interface.
requestedInterfaceName: "%1"
          factoryCount: %2</source>
        <translation type="finished">No factory was found for the requested interface.
requestedInterfaceName: "%1"
          factoryCount: %2</translation>
    </message>
    <message id="debug-messages.service-locator-error.service-not-found">
        <location filename="../../src/common/exceptions/servicelocatorexceptions.hpp" line="145" />
        <source>No service was found for the requested interface.
requestedInterfaceName: "%1"
          serviceCount: %2
          factoryCount: %3</source>
        <translation type="finished">No service was found for the requested interface.
requestedInterfaceName: "%1"
          serviceCount: %2
          factoryCount: %3</translation>
    </message>
    <message id="debug-messages.service-locator-error.invalid-product">
        <location filename="../../src/common/exceptions/servicelocatorexceptions.hpp" line="188" />
        <source>An invalid factory product was created.
requestedInterfaceName: "%1"
    factoryProductName: "%2"
           factoryName: "%3"</source>
        <translation type="finished">An invalid factory product was created.
requestedInterfaceName: "%1"
    factoryProductName: "%2"
           factoryName: "%3"</translation>
    </message>
    <message id="debug-messages.service-locator-error.factory-exception">
        <location filename="../../src/common/exceptions/servicelocatorexceptions.hpp" line="229" />
        <source>An error occurred in a factory.
    factoryProductName: "%1"
           factoryName: "%2"
              why code: %3</source>
        <translation type="finished">An error occurred in a factory.
    factoryProductName: "%1"
           factoryName: "%2"
              why code: %3</translation>
    </message>
    <message id="addle-tagline">
        <location filename="../../src/common/globals.hpp" line="22" />
        <source>Pretty little drawing program and image utility</source>
        <translation type="finished">Pretty little drawing program and image utility</translation>
    </message>
    <message id="debug-messages.service-locator.initializing">
        <location filename="../../src/common/utilities/configuration/serviceconfigurationbase.hpp" line="42" />
        <source>Initializing service locator.</source>
        <translation type="finished">Initializing service locator.</translation>
    </message>
    <message id="debug-messages.service-locator.destroying">
        <location filename="../../src/common/utilities/configuration/serviceconfigurationbase.hpp" line="58" />
        <source>Destroying service locator.</source>
        <translation type="finished">Destroying service locator.</translation>
    </message>
    <message id="debug-messages.unknown-dynamic-trid">
        <location filename="../../src/common/utilities/strings.hpp" line="49" />
        <source>Unknown dynamic TrId requested: "%1"</source>
        <translation type="finished">Unknown dynamic TrId requested: "%1"</translation>
    </message>
    <message id="unit-affix-formatter">
        <location filename="../../src/common/utilities/strings.hpp" line="65" />
        <location filename="../../src/common/utilities/strings.hpp" line="75" />
        <source>%L1 %2</source>
        <translation type="finished">%L1 %2</translation>
    </message>
    <message id="debug-messages.unhandled-exception">
        <location filename="../../src/common/utilities/unhandledexceptionrouter.hpp" line="131" />
        <source>An unhandled exception occurred.</source>
        <translation type="finished">An unhandled exception occurred.</translation>
    </message>
    <message id="temporary.remote-url-not-supported-message">
        <location filename="../../src/core/presenters/maineditorpresenter.cpp" line="236" />
        <source>Loading a document from a remote URL is not yet supported.</source>
        <translation type="finished">Loading a document from a remote URL is not yet supported.</translation>
    </message>
    <message id="debug-messages.application-service.starting">
        <location filename="../../src/core/services/applicationservice.cpp" line="32" />
        <source>Starting ApplicationService.</source>
        <translation type="finished">Starting ApplicationService.</translation>
    </message>
    <message id="debug-messages.application-service.command-line-error">
        <location filename="../../src/core/services/applicationservice.cpp" line="47" />
        <source>A command line error was encountered.</source>
        <translation type="finished">A command line error was encountered.</translation>
    </message>
    <message id="cli-messages.graphical-startup-error">
        <location filename="../../src/core/services/applicationservice.cpp" line="73" />
        <source>An error occurred attempting to start the graphical application.</source>
        <translation type="finished">An error occurred attempting to start the graphical application.</translation>
    </message>
    <message id="debug-messages.application-service.standard-exception">
        <location filename="../../src/core/services/applicationservice.cpp" line="85" />
        <source>Exception thrown:</source>
        <translation type="finished">Exception thrown:</translation>
    </message>
    <message id="debug-messages.application-service.non-standard-exception">
        <location filename="../../src/core/services/applicationservice.cpp" line="95" />
        <source>The error was not of type std::exception</source>
        <translation type="finished">The error was not of type std::exception</translation>
    </message>
    <message id="cli-messages.option-description.editor">
        <location filename="../../src/core/services/applicationservice.cpp" line="126" />
        <source>Explicitly start Addle in editor mode.</source>
        <translation type="finished">Explicitly start Addle in editor mode.</translation>
    </message>
    <message id="cli-messages.option-description.browser">
        <location filename="../../src/core/services/applicationservice.cpp" line="136" />
        <source>Explicitly start Addle in browser mode.</source>
        <translation type="finished">Explicitly start Addle in browser mode.</translation>
    </message>
    <message id="cli-messages.option-description.open-arg-name">
        <location filename="../../src/core/services/applicationservice.cpp" line="142" />
        <source>open</source>
        <translation type="finished">open</translation>
    </message>
    <message id="cli-messages.option-description.open">
        <location filename="../../src/core/services/applicationservice.cpp" line="144" />
        <source>The file or url to open.</source>
        <translation type="finished">The file or url to open.</translation>
    </message>
    <message id="debug-messages.application-service.cli-open-url">
        <location filename="../../src/core/services/applicationservice.cpp" line="183" />
        <source>Command line arguments specified to open url "%1"</source>
        <translation type="finished">Command line arguments specified to open url "%1"</translation>
    </message>
    <message id="debug-messages.application-service.cli-open-file">
        <location filename="../../src/core/services/applicationservice.cpp" line="194" />
        <source>Command line arguments specified to open file "%1"</source>
        <translation type="finished">Command line arguments specified to open file "%1"</translation>
    </message>
    <message id="cli-messages.multiple-positional-arguments-warning">
        <location filename="../../src/core/services/applicationservice.cpp" line="204" />
        <source>Multiple positional arguments were given, but only the first will be used.</source>
        <translation type="finished">Multiple positional arguments were given, but only the first will be used.</translation>
    </message>
    <message id="debug-messages.application-service.starting-editor-explicit">
        <location filename="../../src/core/services/applicationservice.cpp" line="220" />
        <source>Explicitly starting in editor mode.</source>
        <translation type="finished">Explicitly starting in editor mode.</translation>
    </message>
    <message id="debug-messages.application-service.starting-browser-explicit">
        <location filename="../../src/core/services/applicationservice.cpp" line="231" />
        <source>Explicitly starting in browser mode.</source>
        <translation type="finished">Explicitly starting in browser mode.</translation>
    </message>
    <message id="debug-messages.application-service.starting-editor-implicit">
        <location filename="../../src/core/services/applicationservice.cpp" line="243" />
        <source>Implicitly starting in editor mode.</source>
        <translation type="finished">Implicitly starting in editor mode.</translation>
    </message>
    <message id="debug-messages.application-service.starting-browser-implicit">
        <location filename="../../src/core/services/applicationservice.cpp" line="253" />
        <source>Implicitly starting in browser mode.</source>
        <translation type="finished">Implicitly starting in browser mode.</translation>
    </message>
    <message id="debug-messages.application-service.quitting">
        <location filename="../../src/core/services/applicationservice.cpp" line="286" />
        <source>Quitting application service</source>
        <translation type="finished">Quitting application service</translation>
    </message>
    <message id="layer-name-template">
        <location filename="../../src/core/presenters/helpers/documentlayershelper.hpp" line="41" />
        <location filename="../../src/core/presenters/helpers/documentlayershelper.hpp" line="117" />
        <source>Layer %1</source>
        <translation type="finished">Layer %1</translation>
    </message>
    <message id="layer-group-name-template">
        <location filename="../../src/core/presenters/helpers/documentlayershelper.hpp" line="137" />
        <source>Layer Group %1</source>
        <translation type="finished">Layer Group %1</translation>
    </message>
    </context>
</TS>