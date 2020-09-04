# Addle code overview

Addle is implemented mainly in C++11 with Qt 5. It is an event-driven graphical 
user application, object-oriented, and more or less subscribing to SOLID design
principles. I expect its code to be farily navigable by those with experience in
similar projects.

This document outlines some of the basic and general need-to-know. It is meant
to be sparse enough not to require much maintenance but still useful for any
newcomers to the project.

For building instructions see [Building](./Building.md)

## Layout of this project

Addle's runtime code is comprised of (September 2020) four binaries: three shared
libraries and an executable. The subdirectories of "src" correspond to these
binaries and provide the source code for them.

- "addle" - the executable containing `main()`.
- "common" - the library containing interfaces, utilities, global constants,
configuration tools, resources, and other things needed by all Addle code.
- "core" - the library containing the core functionality of Addle including the
basic Presenters, tools, Services, and formats.
- "widgetsgui" - the library implementing the GUI (Views) using Qt Widgets.

"src/common" is in the include path of all Addle source code. This allows the
following headers to be easily included:
- `#include "compat.hpp"` - Compatibility code for supporting different
platforms, compilers, and library versions.
    - `ADDLE_EXPORT_<...>` - These macros must be placed in the declarations of
    classes, global variables, `Q_NAMESPACE` declarations and global functions. 
    It resolves to nothing on Unix-like targets and to appropriate DLL 
    import/export directives on Windows targets. (It shouldn't be used for 
    template classes or template functions, and is not needed for static or 
    inline functions, or classes with no static variables whose member functions 
    are all inline, default, or pure virtual.)
- `#include "globals.hpp"` - Global static constants, const expressions, and 
enumerations. Similar constants relevant to particular contexts are also defined
in interface headers.
- `#include "servicelocator.hpp"`  - see [Interfaces](./Interfaces.md) for
more info about the ServiceLocator. Only include this in source files and not 
headers.
- `#include "utils.hpp"` - a convenience header that includes a variety of
commonly used utilities. Only include this in source files and not headers.
- `#include "exceptions/*"` - Exceptions describing global errors including
various global runtime errors. See also "utilities/errors.hpp".
- `#include "idtypes/*"` - Identifier object types, usable with ServiceLocator.
Global static instances can be found in "globals.hpp".
- `#include "interfaces/*"` - see [Interfaces](./Interfaces.md) for more info
about interfaces in Addle.
- `#include "utilities/*"` - Utility functions and classes. Note that the source 
directory for each binary can have a "utilities" directory containing utilities 
specific to its context, which (within that binary's code) is also included as 
`#include "utilities/*"`. Make sure all utility headers have unique names to 
avoid conflicts.

Other directories of note include:
- "build" - not present in the repo, but this is the directory you're expected
to create to build into. It's useful to name this directory "build" because then
it will be ignored by Git.
- "docs" - these top-level guides and the output of Doxygen
- "etc" - various files relevant to but not directly used by the project
    - "installer" - files for generating a Windows installer for Addle
    - "l10n" - Qt Linguist .ts files containing text localizations.
    - "tools" - scripts that perform automated tasks on code. These may be
    helpful during development but are not required for building Addle. They are
    typically implemented in Python and may have their own dependencies.
- "resources" - resources to be embedded into Addle's binaries
(accessible through the Qt resource system in any code linked to common)
- "tests" - automated unit and integration tests.

## Qt, in brief

Qt is a framework for cross platform event-driven (usually graphical)
applications. It is a sprawling and involved set of tools, including not just
C++ libraries but its own supplements/replacements for standard library
features, syntax extensions, and code generation tools. If you know C++ but not
Qt, you may want to do some reading. 

`QObject` is the class at the heart of many of Qt's most important features.
Classes deriving from `QObject` can be given signals and slots, i.e., special
functions that can participate in Qt's event system. `QObject`s also have 
introspection information, with mechanisms in place to dynamically and
abstractly access and set properties, invoke methods, and even construct
objects.

Qt also provides a variety of libraries for implementing graphical user
interfaces on supported platforms. Of these, Addle currently (September 2020) uses 
Widgets and the Graphics Framework.

Qt also provides a platform independent system for embedding resource files into
the application. Addle uses this system to embed icons and text localizations.
These file are accessible from within Addle code as file paths beginning with
":" or URLs beginning with "qrc:/". Addle documentation sometimes calls this
system "QRC" as shorthand.

Further reading:
- https://doc.qt.io/qt-5/topics-core.html
- https://doc.qt.io/qt-5/signalsandslots.html
- https://doc.qt.io/qt-5/qtwidgets-index.html
- https://doc.qt.io/qt-5/graphicsview.html
- https://doc.qt.io/qt-5/resources.html

## Dependency inversion

Dependency inversion is a strategy that can help to keep code flexible and 
maintainable as it grows in complexity. The general idea involves separating 
interfaces from implementations and only allowing components to access each 
others' interfaces.

Dependency inversion is a part of the so-called SOLID principles, but it bears 
mentioning in particular because its presence in Addle has a more significant 
impact on the way code is written than the other principles, and probably takes 
the most getting-used-to.

In Addle, generally speaking interfaces are pure abstract classes whose names
begin with the capital letter "I" (in the style of C# and similar), and
implementations are concrete classes that derive from the interfaces and
override their virtual methods.

Interfaces are defined in headers located in "src/common/interfaces". There is 
often a one-to-one correspondence of interfaces to implementations (e.g.,
`ISpiff` to `Spiff`), which can be helpful to know when debugging or getting a
feel for what different interfaces do and how they are expected to work. This is
however not a guaranteed relationship.

Addle has a special class `ServiceLocator` that is used to acquire objects
of desired interface types without needing access to their implementations. For 
more detailed information about how interfaces are used in Addle, see
[Interfaces](./Interfaces.md).

Further reading:
- https://en.wikipedia.org/wiki/SOLID
- https://en.wikipedia.org/wiki/Dependency_inversion_principle

## Model-Presenter-View

Addle follows a pattern called Model-Presenter-View to separate concerns related
to the user interface. The pattern introduces three categories of classes:
Models, Presenters, and Views, which have specific responsibilities and 
relationships to each other. It is similar to Model-View-Controller.

**Models** are responsible for owning and managing the *data* that Addle is
consuming or operating on, without being concerned about the user interface or 
the user. Models can be used by the user interface via Presenters, but they can 
also be used in other contexts, such as serialization and I/O.

**Presenters** are responsible for the *state* of the application as presented 
to the user, and the *logic* of managing that state. Presenters may only be used
in contexts that are aware of the user. Some Presenters have a one-to-one 
association to a Model, i.e., in cases where the state owned by the Presenter is 
comprised of data represented by the Model (for example, documents and layers),
in which case the Presenter may own the Model. Other Presenters correspond to a 
concept that is exposed to the user and has state, but that state is not Model 
data (for example, the main editor, tools, and the undo/redo stack). Some 
Presenters are responsible for creating their associated View (see e.g.
`ITopLevelView`), but most are not.

**Views** essentially comprise the user interface, and are responsible for 
conveying the application state to the user and accepting user input. A View is 
associated to a Presenter, and is responsible for conforming itself to the state 
of the Presenter. In Addle, (September 2020) Views are primarily implemented using
QWidget or QGraphicsItem.

Models, Presenters, and Views may freely own or communicate with other objects
in the same category (i.e., Models may own other Models, etc.). Ownership and 
communication across categories should be limited to the cases described above. 
Models, Views, and Presenters in Addle are often implemented as `QObject` and 
can communicate via signals and slots. 

## Other topics

- [Internationalization and localization](./I18n.md)
- [Error handling](./Errors.md)

Some of Addle's code is annotated with Doxygen commands, and a Doxyfile is
provided. These may be used if desired to generate developer documentation of
the code.

For more information see https://www.doxygen.nl/index.html