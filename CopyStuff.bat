copy /Y data\SystemData\Aeroplanes\RTEraserFive\Texture2048.jpg %2\deployments\%1\win32\release\SystemData\Aeroplanes\RTEraserFive\Texture.jpg
copy /Y data\SystemData\Aeroplanes\Extra3D\Texture2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\Extra3D\Texture.png
copy /Y data\SystemData\Aeroplanes\GeeBee\GeeBee2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\GeeBee\GeeBee.png
copy /Y data\SystemData\Aeroplanes\Spitfire\Spitfire2048.jpg %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spitfire\Spitfire.jpg
copy /Y data\SystemData\Aeroplanes\SopwithPup\SopwithPup2048.jpg %2\deployments\%1\win32\release\SystemData\Aeroplanes\SopwithPup\SopwithPup.jpg
copy /Y data\SystemData\Aeroplanes\Yak54\Yak2048.jpg %2\deployments\%1\win32\release\SystemData\Aeroplanes\Yak\Yak.jpg
copy /Y data\SystemData\Aeroplanes\Minimoa\Minimoa2048.jpg %2\deployments\%1\win32\release\SystemData\Aeroplanes\Minimoa\Minimoa.jpg
copy /Y data\SystemData\Aeroplanes\Spirit26\Spirit-Danny2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spirit26\Spirit-Danny.png
copy /Y data\SystemData\Aeroplanes\Spirit26\Spirit-Markus2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spirit26\Spirit-Markus.png
copy /Y data\SystemData\Aeroplanes\Icarus\DLG2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\Icarus\DLG.png
copy /Y data\SystemData\Aeroplanes\MaxBee\MaxBee2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\MaxBee\MaxBee.png
copy /Y data\SystemData\Aeroplanes\Jackdaw\Jackdaw2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\Jackdaw\Jackdaw.png
copy /Y data\SystemData\Aeroplanes\Quartz\Quartz2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\Quartz\Quartz.png
copy /Y data\SystemData\Aeroplanes\Quartz\QuartzDanny2048.png %2\deployments\%1\win32\release\SystemData\Aeroplanes\Quartz\QuartzDanny.png

source\CompressAC3D\Release\CompressAC3D.exe data\SystemData\Aeroplanes\GeeBee\GeeBee.ac %2\deployments\%1\win32\release\SystemData\Aeroplanes\GeeBee\GeeBee.ac.p
del /Q %2\deployments\%1\win32\release\SystemData\Aeroplanes\GeeBee\GeeBee.ac

source\CompressAC3D\Release\CompressAC3D.exe data\SystemData\Aeroplanes\Spitfire\Spitfire.ac %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spitfire\Spitfire.ac.p
del /Q %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spitfire\Spitfire.ac

source\CompressAC3D\Release\CompressAC3D.exe data\SystemData\Aeroplanes\SopwithPup\SopwithPup.ac %2\deployments\%1\win32\release\SystemData\Aeroplanes\SopwithPup\SopwithPup.ac.p
del /Q %2\deployments\%1\win32\release\SystemData\Aeroplanes\SopwithPup\SopwithPup.ac

source\CompressAC3D\Release\CompressAC3D.exe data\SystemData\Aeroplanes\Spirit26\Spirit.ac %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spirit26\Spirit.ac.p
del /Q %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spirit26\Spirit.ac

source\CompressAC3D\Release\CompressAC3D.exe data\SystemData\Aeroplanes\Spirit26\SpiritAlt.ac %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spirit26\SpiritAlt.ac.p
del /Q %2\deployments\%1\win32\release\SystemData\Aeroplanes\Spirit26\SpiritAlt.ac

copy /Y data\Windows.icf %2\deployments\%1\win32\release\app.icf

xcopy /Y /i C:\Marmalade\8.6\s3e\win32\pvr %2\deployments\%1\win32\release\pvr
xcopy /Y /i C:\Marmalade\8.6\s3e\win32\pvr3 %2\deployments\%1\win32\release\pvr3
xcopy /Y /i C:\Marmalade\8.6\s3e\win32\angle %2\deployments\%1\win32\release\angle

