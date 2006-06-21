# Microsoft Developer Studio Project File - Name="objecttrackerlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=objecttrackerlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "objecttrackerlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "objecttrackerlib.mak" CFG="objecttrackerlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "objecttrackerlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "objecttrackerlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "objecttrackerlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "c:\temp"
# PROP Intermediate_Dir "c:\temp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "L:\W2K\libxml++" /I "../../xmlcfg/include" /I "H:\work\swistrack\code\libblob\include" /I "../include" /I "../../calibration/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/objecttracker.lib"

!ELSEIF  "$(CFG)" == "objecttrackerlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\temp"
# PROP Intermediate_Dir "c:\temp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../xmlcfg/include" /I "H:\work\swistrack\code\libblob\include" /I "../include" /I "../../calibration/include" /I "L:\W2K\libxml++" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/objecttrackerd.lib"

!ENDIF 

# Begin Target

# Name "objecttrackerlib - Win32 Release"
# Name "objecttrackerlib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\DataLogger.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mask.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ObjectTracker.cpp
# End Source File
# Begin Source File

SOURCE=..\src\polygon.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Segmenter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Track.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Tracker.cpp
# End Source File
# Begin Source File

SOURCE=..\src\TrackingImage.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\Component.h
# End Source File
# Begin Source File

SOURCE=..\include\DataLogger.h
# End Source File
# Begin Source File

SOURCE=..\include\mask.h
# End Source File
# Begin Source File

SOURCE=..\include\ObjectTracker.h
# End Source File
# Begin Source File

SOURCE=..\include\polygon.h
# End Source File
# Begin Source File

SOURCE=..\include\Segmenter.h
# End Source File
# Begin Source File

SOURCE=..\include\Track.h
# End Source File
# Begin Source File

SOURCE=..\include\Tracker.h
# End Source File
# Begin Source File

SOURCE=..\include\TrackingImage.h
# End Source File
# Begin Source File

SOURCE=..\include\ui.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\core\readme.txt
# End Source File
# End Target
# End Project
