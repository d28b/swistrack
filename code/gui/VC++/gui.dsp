# Microsoft Developer Studio Project File - Name="gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=gui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak" CFG="gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "gui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "I:\temp\release"
# PROP Intermediate_Dir "I:\temp\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../include" /I "../../libblob/include" /I "../../core/include" /I "../../calibration/include" /I "L:\W2K\wxWidgets\include" /I "I:\library\wxWidgets-2.6.1\lib\mswd" /I "../core" /I "../calibration/src" /I "I:\library\opencv\otherlibs\highgui" /I "I:\library\opencv\cv\include" /I "I:\library\libblob\include" /I "I:\library\opencv\cxcore\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WINDOWS__" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxmsw26_html.lib wxmsw26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib objecttracker.lib calibration.lib libblob.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"../../../release/swistrack.exe" /libpath:"I:\library\wxWidgets\lib\vc_lib" /libpath:"../../core/lib" /libpath:"I:\library\wxWidgets\contrib\lib" /libpath:"../../calibration/lib" /libpath:"I:\library\1394camera\lib" /libpath:"../../libblob/lib"
# SUBTRACT LINK32 /profile /debug

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "I:\temp\debug"
# PROP Intermediate_Dir "I:\temp\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GR /GX /ZI /Od /I "../../xmlcfg/include" /I "../include" /I "../../libblob/include" /I "../../core/include" /I "../../calibration/include" /I "I:\library\wxWidgets\include" /I "I:\library\wxWidgets\lib\mswd" /I "../core" /I "../calibration/src" /I "I:\library\opencv\otherlibs\highgui" /I "I:\library\opencv\cv\include" /I "I:\library\libblob\include" /I "I:\library\opencv\cxcore\include" /I "I:\library\libxml++" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libxml++d.lib libxml2.lib wxbase26d_net.lib wxmsw26d_html.lib wxmsw26d_adv.lib wxmsw26d_core.lib wxbase26d.lib kernel32.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib objecttrackerd.lib calibrationd.lib libblobd.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libci.lib" /nodefaultlib:"msvcrt.lib" /out:"../../../release/swistrack.exe" /pdbtype:sept /libpath:"I:\library\wxWidgets\lib\vc_lib" /libpath:"../../core/lib" /libpath:"I:\library\wxWidgets\contrib\lib" /libpath:"../../calibration/lib" /libpath:"I:\library\1394camera\lib" /libpath:"../../libblob/lib" /libpath:"I:\library\libxml++\lib" /libpath:"I:\library\libxml2\lib"
# SUBTRACT LINK32 /incremental:no

!ENDIF 

# Begin Target

# Name "gui - Win32 Release"
# Name "gui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\AviWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Canvas.cpp
# End Source File
# Begin Source File

SOURCE=..\src\gui.cpp
# End Source File
# Begin Source File

SOURCE=..\src\InterceptionPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SegmenterPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SegmenterPanelCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SettingsDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SocketServer.cpp
# End Source File
# Begin Source File

SOURCE=..\src\SwisTrack.cpp
# End Source File
# Begin Source File

SOURCE=..\src\TrackingPanel.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=..\include\AviWriter.h
# End Source File
# Begin Source File

SOURCE=..\include\Canvas.h
# End Source File
# Begin Source File

SOURCE=..\include\constants.h
# End Source File
# Begin Source File

SOURCE=..\include\GUIApp.h
# End Source File
# Begin Source File

SOURCE=..\include\InterceptionPanel.h
# End Source File
# Begin Source File

SOURCE=..\include\resource.h
# End Source File
# Begin Source File

SOURCE=..\include\SegmenterPanel.h
# End Source File
# Begin Source File

SOURCE=..\include\SegmenterPanelCanvas.h
# End Source File
# Begin Source File

SOURCE=..\include\SettingsDialog.h
# End Source File
# Begin Source File

SOURCE=..\include\SocketServer.h
# End Source File
# Begin Source File

SOURCE=..\include\SwisTrack.h
# End Source File
# Begin Source File

SOURCE=..\include\TrackingPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\xmlcfg\include\xmlcfg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmaps\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\colours.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\finger.bmp
# End Source File
# Begin Source File

SOURCE=.\gui.ico
# End Source File
# Begin Source File

SOURCE=.\gui.rc
# End Source File
# Begin Source File

SOURCE=.\bitmaps\hand.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\new.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\open.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\pause.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\play.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\rewind.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\save.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\singlestep.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmaps\singlestepback.bmp
# End Source File
# End Group
# Begin Group "libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=I:\library\opencv\lib\highgui.lib
# End Source File
# Begin Source File

SOURCE=I:\library\opencv\lib\cxcore.lib
# End Source File
# Begin Source File

SOURCE=I:\library\opencv\lib\cv.lib
# End Source File
# Begin Source File

SOURCE=I:\library\opencv\lib\cvaux.lib
# End Source File
# Begin Source File

SOURCE=I:\library\1394camera\lib\1394camera.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\changelog.txt
# End Source File
# Begin Source File

SOURCE=.\documentation.oxy
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
