; PicaSim Inno Setup Script
; Builds a Windows installer for PicaSim

#define MyAppName "PicaSim"
#ifndef MyAppVersion
  #define MyAppVersion "1.0.0"
#endif
#define MyAppPublisher "Rowlhouse"
#define MyAppURL "http://www.rowlhouse.co.uk/PicaSim/"
#define MyAppExeName "PicaSim.exe"

; Version directory name (matches CMake install dir)
#define MyAppVersionDir MyAppVersion

[Setup]
; NOTE: The AppId uniquely identifies this application.
; Do not use the same AppId in other installers.
AppId={{B8F4E9A2-7C3D-4E5F-9A1B-2C3D4E5F6A7B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=..\LICENSE.txt
OutputDir=..\dist
OutputBaseFilename=PicaSim-{#MyAppVersion}-Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "cleaninstall"; Description: "Clean install (remove previous installation files first)"; GroupDescription: "Installation options:"; Flags: unchecked
Name: "cleanuserdata"; Description: "Delete user data and settings during install"; GroupDescription: "Installation options:"; Flags: unchecked

[Files]
; Install all files from the versioned dist directory
Source: "..\dist\PicaSim-{#MyAppVersionDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssInstall then
  begin
    // Clean install: remove previous installation files if task selected
    if WizardIsTaskSelected('cleaninstall') then
    begin
      DelTree(ExpandConstant('{app}'), True, True, True);
    end;
    // Delete user data and settings if task selected
    if WizardIsTaskSelected('cleanuserdata') then
    begin
      DelTree(ExpandConstant('{userappdata}\Rowlhouse\PicaSim'), True, True, True);
      RemoveDir(ExpandConstant('{userappdata}\Rowlhouse'));
    end;
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  UserDataPath: String;
begin
  if CurUninstallStep = usPostUninstall then
  begin
    UserDataPath := ExpandConstant('{userappdata}\Rowlhouse\PicaSim');
    if DirExists(UserDataPath) then
    begin
      if MsgBox('Do you want to delete your user data and settings?' + #13#10 + #13#10 +
                'Location: ' + UserDataPath, mbConfirmation, MB_YESNO) = IDYES then
      begin
        DelTree(UserDataPath, True, True, True);
        // Also try to remove parent Rowlhouse folder if empty
        RemoveDir(ExpandConstant('{userappdata}\Rowlhouse'));
      end;
    end;
  end;
end;
