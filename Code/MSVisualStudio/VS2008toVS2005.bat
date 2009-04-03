@echo off
setlocal enabledelayedexpansion


REM Modify the SLN project file
 
FOR /F "usebackq delims= tokens=*" %%G IN ("SwisTrack.sln") DO (
  Set Line=%%G
  Set Line=!Line:Format Version 10.00=Format Version 9.00!
  Set Line=!Line:Visual Studio 2008=Visual Studio 2005!
  echo !Line!>>"tmpFile.txt"
)
move tmpFile.txt SwisTrack.sln


FOR %%G in ("..\gui\MSVisualStudio\SwisTrack-GUI.vcproj" "..\core\MSVisualStudio\SwisTrack-Core.vcproj" "..\..\3rdParty\camshift_wrapper\MSVisualStudio\camshift_wrapper.vcproj" "..\..\3rdParty\cvblobslib_v6p1\cvblobslib\cvblobslib.vcproj" "..\..\3rdParty\libtsai\libtsai.vcproj") DO CALL:MODIFYFILES %%G

GOTO:EOF


:MODIFYFILES
FOR /F "usebackq delims= tokens=*" %%G IN (%1) DO (
  Set Line=%%G
  Set Line=!Line:"9.00"="8.00"!
  echo !Line!>>tmpFile.txt
)
move tmpFile.txt %1


GOTO:EOF



