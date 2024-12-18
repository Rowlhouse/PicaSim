[Setup]
OutputBaseFilename=PicaSimDevTest-Installer
AppName=PicaSimDevTest
AppVerName=PicaSimDevTest
DefaultGroupName=PicaSimDevTest
DefaultDirName={pf}\PicaSimDevTest
UninstallDisplayIcon=PicaSim-DevTest.exe
Compression=lzma
SolidCompression=yes
WizardImageFile=E:\code\PicaSim/source/Art/Installer.bmp
SourceDir=E:\code\PicaSim\build_picasim_vc11x\deployments\DevTest\win32\release
OutputDir=E:\code\PicaSim\build_picasim_vc11x\deployments\DevTest\win32
SetupIconFile=E:\code\PicaSim\build_picasim_vc11x\deployments\DevTest\win32\release\generated_images\Win32_Icon_48x48_0.ico
PrivilegesRequired=admin
[Messages]
WelcomeLabel2=PicaSimDevTest - Free Flight Simulator for R/C Aeroplanes
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
Name: "{group}\PicaSimDevTest"; Filename: "{app}\PicaSim-DevTest.exe"
Name: "{commondesktop}\PicaSimDevTest"; Filename: "{app}\PicaSim-DevTest.exe"
Name: "{group}\Uninstall PicaSimDevTest"; Filename: "{uninstallexe}"
[Run]
Filename: "{app}\PicaSim-DevTest.exe"; Description: "Launch PicaSimDevTest"; Flags: postinstall nowait skipifsilent