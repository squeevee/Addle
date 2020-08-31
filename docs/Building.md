# Building Addle

Addle is built using a tool called CMake, which allows it to be built for a
variety of platforms and toolchains as described in this guide.

General recommended reading:
- Using CMake: https://cmake.org/runningcmake/
- Using Qt: https://doc.qt.io/qt-5/gettingstarted.html
- Using CMake with Qt: https://doc.qt.io/qt-5/cmake-manual.html
- More on using CMake with Qt: https://wiki.qt.io/Using_CMake_build_system

## Linux

The supported toolchain for Linux is GNU Make with GCC as the compiler.

Dependencies can usually be installed via your software distro. For instance, on
Ubuntu (Debian, and similar), the packages you'll need can be installed with:
```sh
sudo apt-get install build-essential cmake qtbase5-dev libboost-all-dev

```

And on Arch Linux:
```sh
sudo pacman -S base-devel cmake qt5-base boost
```

Install the dependencies and download/clone the source code. Create a directory
called "build" under the source code directory, open a terminal to the build
directory and run the following:

```sh
cmake ..
make addle
```

Addle can be run directly from "build/bin", or can be installed with
```
sudo make install
```
Per CMake's default, this will place it in "usr/local/bin". A custom install
path can be specified with `-DCMAKE_INSTALL_PREFIX=<path>` when calling
`cmake`.

I have also found Ninja (alternative to GNU Make) and Clang (alternative to GCC)
work.

See also:

- GCC manual: https://gcc.gnu.org/onlinedocs/gcc-10.2.0/gcc/
- GNU Make manual: https://www.gnu.org/software/make/manual/make.html
- Ninja build system manaul: https://ninja-build.org/manual.html
- CMake generator for Ninja documentation: 
https://cmake.org/cmake/help/v3.0/generator/Ninja.html
- Clang manual: https://clang.llvm.org/docs/UsersManual.html
- Using Clang in place of GCC: 
https://stackoverflow.com/questions/7031126/switching-between-gcc-and-clang-llvm-using-cmake/7032021#7032021

### Qt Creator

Qt Creator is a free and open source IDE for Qt. It runs on Windows, Linux, and 
Mac OS. I haven't used it in a while but if memory serves it integrates pretty 
easily with its respective platform's supported build toolchain, and it also 
supports opening CMake projects.

Developers unused to Linux may find it a more-friendly alternative to the 
command line.

Refer to Qt Creator's manual: https://doc.qt.io/qtcreator/index.html

## Windows

There are two toolchains supported by Addle for building on Windows: Microsoft 
Visual Studio and MinGW-w64 with GCC.

See also the section "DLLs".

### Visual Studio

https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio

Visual Studio is a popular IDE made by Microsoft. A community version is 
available for free download and use by individuals and small projects. It 
provides MSVC, the de-facto standard C++ compiler for Windows.

Recent versions of Visual Studio support directly opening CMake projects by 
folder. CMake is also able to generate Visual Studio project files that can be 
opened by older versions of Visual Studio.

When the project is loaded into Visual Studio, it can be built and run from the 
Visual Studio user interface, the same as with any other Visual Studio project.

Qt and Boost can be installed from their respective project websites:

- https://www.qt.io/download-qt-installer
- https://www.boost.org/users/download/

When prompted during the installation of Qt, Addle is compatible and intended
for use with the LGPL-licensed (open source) version.

Alternatively, dependencies can be installed via a package manager. For Visual
Studio the preferred one seems to be vcpkg:

- Vcpkg overview: https://docs.microsoft.com/en-us/cpp/build/vcpkg
- Searchable web database of vcpkg packages: https://repology.org/projects/?inrepo=vcpkg

Addle is known to work with Visual Studio 2019, but may work with earlier
versions.

### MinGW-w64

MinGW-w64 is a free and open source set of tools that allows GCC to create
Windows binaries. It can be installed and run on Windows.

Note that it is different from MinGW, which has similar capabilities but is more
limited.

- MinGW only supports 32-bit hosts and targets.
- MinGW-w64 supports *both* 32-bit and 64-bit hosts and targets.
- Qt only officially supports MinGW and *not* MinGW-w64, i.e., the Qt installer
does not provide the binaries we need.

There are a few ways to install MinGW-w64 on Windows, but I recommend using
MSYS2, which is a free and open source software distribution for Windows that
comes with a package manager. In addition to MinGW-w64, MSYS2 also provides
packages for Boost and Qt. 

Note that MSYS2 is operated using a Unix-like command line terminal.

To install MSYS2, go to https://www.msys2.org/ and follow the instructions
there.

Once installed, you will have three executables that open your terminal. They
are essentially the same, but with some environment variables set up that make
them more convenient for different tasks:

- `msys2.exe` for general operation and maintenance of the MSYS2 system
- `mingw64.exe` for use of MinGW-w64 targeting 64-bit Windows
- `mingw32.exe` for use of MinGW-w64 targeting 32-bit Windows

For 64-bit Windows targets, the packages for building Addle are installed with:
```sh
pacman -S make mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-qt5 mingw-w64-x86_64-boost
```

(For 32-bit Windows targets, change `x86_64` to `i686`. The commands are the
same on 32-bit and 64-bit hosts.)

From here, the instructions are similar to "Linux".

MSYS2 has its own Unix-style filesystem, which is semi-isolated from the main 
Windows filesystem. The root directory "/" corresponds to the directory where
MSYS2 was installed. When you open the terminal the working directory is 
"/home/\<User\>/" where "\<User\>" is your Windows username. (This corresponds
to, e.g., "C:\\msys64\\home\\\<User\>".) You *can* access the Windows filesystem
in MSYS2 by converting a path like
"C:\\Users\\\<User\>\\Documents\\GitHub\\Addle" into
"/c/Users/\<User\>/Documents/GitHub/Addle".

This is terribly inconvenient to do frequently, so you may prefer to work out of
your MSYS2 home directory instead. (MSYS2 offers a `git` package that you can
use to clone Addle with the command line.)

OR if you want to use MSYS2 working out of your Windows user directory, for 
example, you can create a Windows shortcut that calls the following command:

```
C:\msys64\msys2_shell.cmd -where C:\Users\<username> -mingw64
```

(Substituting "msys64" with the directory where MSYS2 is installed, if it's
different, "\<username\>" with your windows username, and pass `-mingw32` if you
want to build for 32-bit targets instead.)

However and wherever you work, download/clone Addle source code and create a 
"build" directory inside it. Once there, run the following:

```sh
cmake -G "MSYS Makefiles" ..
make addle
```

Further reading:
- More detailed information about MinGW-w64, including other ways of installing
and operating it besides MSYS2: https://wiki.qt.io/MinGW-64-bit
- The manual for MSYS2's package manager: https://www.archlinux.org/pacman/pacman.8.html
- Searchable web database of MSYS2 packages: https://packages.msys2.org/search

### DLLs

However you build it, to run Addle from Windows, you must make sure that the 
necessary DLLs can be found. If you launch Addle from the Visual Studio or the 
MinGW-w64 terminal respectively, then this will automatically be taken care of.

You can can add the directories where the DLLs are installed to your PATH
environment variable (e.g., "C:\\Qt\\5.14.2\\msvc2017_64\\bin" or
"C:\\msys64\\mingw64\\bin"). If built with MSVC you may also want to download
and install the Microsoft Visual C++ Redistributable from 
https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads.

(Note that adding these directories to your path can create problems if you have
multiple versions of Qt installed, e.g., a 64-bit and 32-bit version.)

Another option is to copy the necessary DLLs into the same directory as
"addle.exe". If you launch "depends.exe" from the Visual Studio Command Prompt
or MinGW-w64 terminal respectively, you can load "addle.exe" into it and
acquire a comprehensive list of the DLLs required by your particular build.

(You can also refer to "etc/installer/mingw.iss" for a list of DLLs needed for
Addle with the MSYS2 build of Qt, but bear in mind that list may be outdated, as 
MSYS2's repository is constantly updated.)

Also note that in addition to the direct linked dependencies, Addle will also
need two Qt Plugins: "qwindows.dll" and "qwindowsvistastyle.dll". These can be
found in the plugins directory of your Qt installation
(e.g., "/mingw64/share/qt5/plugins" in MSYS2), in the "platforms" and "styles"
subdirectories. Instead of copying these directly into the directory that has
"addle.exe", copy them into subdirectories "platforms" and "styles" respectively.

IF YOU PLAN ON DISTRIBUTING THIS BUILD OF ADDLE, MAKE SURE YOU UNDERSTAND AND 
ARE COMPLIANT WITH THE LICENSE TERMS OF THE SOFTWARE YOU ARE REDISTRIBUTING.

Further reading:
- Dependency Walker ("depends.exe"), a popular freeware tool for recursively
listing the dependencies of a DLL or EXE: https://www.dependencywalker.com/
- ntldd, an open source alternative to Dependency Walker, similar in usage to 
Unix ldd. Provided by MSYS2 package `mingw-w64-ntldd-git`

## MinGW-w64 cross-compilation from Linux (advanced)

MinGW-w64 is likely to be a package provided by your distro. Ubuntu provides it,
and on Arch Linux it's in the AUR with a variety of related tools including a
wrapper for CMake.

I have found that MSYS2 packages can be installed on Linux and used to link
MinGW-w64 builds (`pacman` is, after all, the same package manager used by Arch
Linux). This is highly non-standard and not supported by anyone, but it's very
handy for installing binary libraries of dependencies instead of needing to
build them myself.

I created a wrapper script `msys2-pacman`:
```sh
fakeroot pacman $* --config path/to/msys2-pacman.conf
```

And "msys2-pacman.conf":
```conf
[options]
RootDir = /path/to/installation/of/msys2
DBPath = /path/to/installation/of/msys2/var/lib/pacman
CacheDir = /path/to/installation/of/msys2/cache/pacman/pkg
GPGDir = /path/to/installation/of/msys2/etc/pacman.d/gnupg
LogFile = /path/to/installation/of/msys2/var/log/pacman.log
Architecture = auto

[mingw32]
Include = /path/to/installation/of/msys2/etc/pacman.d/mirrorlist.mingw32

[mingw64]
Include = /path/to/installation/of/msys2/etc/pacman.d/mirrorlist.mingw64

[msys]
Include = /path/to/installation/of/msys2/etc/pacman.d/mirrorlist.msys
```

Where "/path/to/installation/of/msys2" corresponds to the location I installed
MSYS2 to, on a shared NTFS partition. (I did the installation itself on Windows,
though if I hadn't already done that, I could have simply found the 
corresponding pacman.d files on MSYS2's source code repo. The rest of MSYS2 is
not necessary when running from Linux.)

While MSYS2 is very useful for installing MinGW-w64 compatible libraries, 
obviously Windows binaries can't be natively run on Linux. And because Qt 
requries certain of its own binaries to run when building Addle, I needed to 
make sure the correct ones are being used. In the file "/usr/share/mingw/
toolchain-x86_64-w64-mingw32.cmake", I added the following:
```cmake
set (QT_QMAKE_EXECUTABLE /usr/bin/qmake)

set (QT_MOC_EXECUTABLE /usr/bin/moc)
add_executable(Qt5::moc IMPORTED)
set_property(TARGET Qt5::moc PROPERTY IMPORTED_LOCATION ${QT_MOC_EXECUTABLE})

set (QT_RCC_EXECUTABLE /usr/bin/rcc)
add_executable(Qt5::rcc IMPORTED)
set_property(TARGET Qt5::rcc PROPERTY IMPORTED_LOCATION ${QT_RCC_EXECUTABLE})

set (QT_UIC_EXECUTABLE /usr/bin/uic)
add_executable(Qt5::uic IMPORTED)
set_property(TARGET Qt5::uic PROPERTY IMPORTED_LOCATION ${QT_UIC_EXECUTABLE})
```

Overall this setup essentially works but is (no surprise) *extremely finnicky*
and produces a variety of warning and error messages.

Another thing to keep in mind in this is that a build of MinGW-w64 and all of
the binaries it produces will support one of three techniques for exception
stack unwinding on Windows. MSYS2 packages use SEH for 64-bit targets, and
DWARF2 for 32-bit targets. If these are different from your distro's MinGW-w64
build, then you will have difficulty linking.

See:
https://wiki.qt.io/MinGW-64-bit#Exception_handling:_SJLJ.2C_DWARF.2C_and_SEH
