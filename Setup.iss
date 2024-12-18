[Setup]
OutputBaseFilename=PicaSim-Installer
AppName=PicaSim
AppVerName=PicaSim
DefaultGroupName=PicaSim
DefaultDirName={pf}\PicaSim
UninstallDisplayIcon=PicaSim.exe
Compression=lzma
SolidCompression=yes
WizardImageFile=E:\code\PicaSim/source/Art/Installer.bmp
SourceDir=E:\code\PicaSim\build_picasim_vc11x\deployments\Windows\win32\release
OutputDir=E:\code\PicaSim\build_picasim_vc11x\deployments\Windows\win32
SetupIconFile=E:\code\PicaSim\build_picasim_vc11x\deployments\Windows\win32\release\generated_images\Win32_Icon_48x48_0.ico
PrivilegesRequired=admin
[Messages]
WelcomeLabel2=PicaSim - Free Flight Simulator for R/C Aeroplanes
[InstallDelete]
Type: filesandordirs; Name:"{app}\SystemData"
Type: filesandordirs; Name:"{app}\SystemSettings"
[Dirs]
Name:"{app}\UserData"; Permissions: users-modify
Name:"{app}\UserSettings"; Permissions: users-modify
Name:"{app}"; Permissions: users-modify
[Files]
Source: "*.*"; DestDir: "{app}"; Flags:replacesameversion
Source: "angle/*"; DestDir: "{app}/angle"; Flags:replacesameversion
Source: "pvr/*"; DestDir: "{app}/pvr"; Flags:replacesameversion
Source: "pvr3/*"; DestDir: "{app}/pvr3"; Flags:replacesameversion
Source: "Extensions/*.*"; DestDir: "{app}/Extensions"; Flags:replacesameversion recursesubdirs
Source: "Menus/*.*"; DestDir: "{app}/Menus"; Flags:replacesameversion recursesubdirs
Source: "SystemData/*.*"; DestDir: "{app}/SystemData"; Flags:replacesameversion recursesubdirs
Source: "SystemSettings/*"; DestDir: "{app}/SystemSettings"; Flags:replacesameversion recursesubdirs
[Icons]
Name: "{group}\PicaSim"; Filename: "{app}\PicaSim.exe"
Name: "{commondesktop}\PicaSim"; Filename: "{app}\PicaSim.exe"
Name: "{group}\Uninstall PicaSim"; Filename: "{uninstallexe}"
[Run]
Filename: "{app}\PicaSim.exe"; Description: "Launch PicaSim"; Flags: postinstall nowait skipifsilent