# Microsoft Developer Studio Project File - Name="libtsai" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libtsai - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtsai.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtsai.mak" CFG="libtsai - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtsai - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libtsai - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libtsai - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libtsai - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /Ze /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DMINPACK" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libtsai - Win32 Release"
# Name "libtsai - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\TSAISRC\CAL_EVAL.C
# End Source File
# Begin Source File

SOURCE=.\TSAISRC\CAL_MAIN.C
# End Source File
# Begin Source File

SOURCE=.\TSAISRC\CAL_TRAN.C
# End Source File
# Begin Source File

SOURCE=.\TSAISRC\CAL_UTIL.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MINPACK\DPMPAR.C
# End Source File
# Begin Source File

SOURCE=.\TSAISRC\ECALMAIN.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MINPACK\ENORM.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MINPACK\FDJAC2.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MINPACK\LMDIF.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MINPACK\LMPAR.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MATRIX.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MINPACK\QRFAC.C
# End Source File
# Begin Source File

SOURCE=.\MATHSRC\MINPACK\QRSOLV.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\INCLUDE\CAL_DEFS.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\F2C.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\GASDEV.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\libtsai.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\LMDIF.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\MATRIX.H
# End Source File
# End Group
# End Target
# End Project
