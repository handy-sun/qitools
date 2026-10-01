; QiTools Windows installer (Inno Setup 6)
;
; CI:   iscc /DMyAppVersion=<x.y.z> /DMyArtifact=qitools-win-x64 scripts\qitools.iss
; Local: stage the windeployqt tree at dist\qitools-win-x64 (repo root), then run iscc.
; All paths are anchored to this script's location, so CWD does not matter.

#define RepoRoot SourcePath + "\.."

#ifndef MyAppVersion
#define MyAppVersion "0.0.0"
#endif
#ifndef MyAppExeName
#define MyAppExeName "qitools-msvc.exe"
#endif
#ifndef MyArtifact
#define MyArtifact "qitools-win-x64"
#endif
#define DeployDir RepoRoot + "\dist\" + MyArtifact

[Setup]
AppId={{3000e6e1-944c-42af-9371-9998f120934b}
AppName=QiTools
AppVersion={#MyAppVersion}
AppVerName=QiTools {#MyAppVersion}
AppPublisher=sooncheer
DefaultDirName={autopf}\QiTools
DefaultGroupName=QiTools
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
UninstallDisplayName=QiTools
UninstallDisplayIcon={app}\{#MyAppExeName}
MinVersion=10.0
OutputDir={#RepoRoot}\dist
OutputBaseFilename={#MyArtifact}-setup
SetupIconFile={#RepoRoot}\resource\toolsimage.ico
LicenseFile={#RepoRoot}\LICENSE
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64compatible

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#DeployDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\QiTools"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,QiTools}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\QiTools"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,QiTools}"; Flags: nowait postinstall skipifsilent
