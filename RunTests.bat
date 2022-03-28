


set Plugins=C:\Users\Archimedes\work\gamekit\Plugins\Gamekit\
set ProjectFolder=C:\Users\Archimedes\work\gamekit\
set Project=C:\Users\Archimedes\work\gamekit\Chessy.uproject
set Map=/Game/Levels/FlatLandscape.umap
set Args=-log -Unattended -NullRHI -NoSplash -NoSound -NoPause
set TestArgs=ABSLOG="%ProjectFolder%/Saved/Automation/log.txt" -ReportOutputPath="%ProjectFolder%/Saved/Automation/Report" -TestExit="Automation Test Queue Empty"

set UEPATH=C:\opt\UnrealEngine\Engine\Binaries\Win64\UE4Editor.exe
set UATPATH=C:\opt\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat

%UEPATH% %Project% %Map% %Args% %TestArgs% -ExecCmds="Automation RunTests Gamekit"

