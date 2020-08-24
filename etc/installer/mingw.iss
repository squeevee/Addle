; Inno Setup script
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Addle"
#define MyAppVersion "0.0.1"
#define MyAppExeName "addle.exe"

#define AddleBuildDir "C:\msys64\home\Dev\Addle\build\mingw64\bin"
#define MinGWBinDir "C:\msys64\mingw64\bin"
#define QtPluginsDir "C:\msys64\mingw64\share\qt5\plugins"

[Setup]
AppId={{22223323-CBD5-4612-A393-7AC246ED74A8}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
DefaultDirName={commonpf64}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=addle-gpl-3.0.txt
OutputBaseFilename=addlesetup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#AddleBuildDir}\addle.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#AddleBuildDir}\libaddlecommon.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#AddleBuildDir}\libaddlecore.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#AddleBuildDir}\libaddlewidgetsgui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libbrotlidec.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libbrotlicommon.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libbz2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libdouble-conversion.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libfreetype-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libglib-2.0-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libgraphite2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libharfbuzz-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libiconv-2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libicudt67.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libicuin67.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libicuuc67.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libintl-8.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libpcre-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libpcre2-16-0.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libpng16-16.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\libzstd.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MinGWBinDir}\zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtPluginsDir}\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#QtPluginsDir}\styles\qwindowsvistastyle.dll"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

