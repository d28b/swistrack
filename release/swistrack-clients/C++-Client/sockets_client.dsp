# Microsoft Developer Studio Project File - Name="client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sockets_client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sockets_client.mak" CFG="client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "client - Win32 DLL Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 DLL Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 DLL Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 DLL Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 DLL Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 DLL Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Universal Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Universal Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Universal Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Universal Debug" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "client - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "client - Win32 DLL Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivudll"
# PROP BASE Intermediate_Dir "vc_mswunivudll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivudll"
# PROP Intermediate_Dir "vc_mswunivudll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswunivu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivudll\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswunivu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivudll\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswunivu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswunivu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivudll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivudll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 DLL Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivuddll"
# PROP BASE Intermediate_Dir "vc_mswunivuddll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivuddll"
# PROP Intermediate_Dir "vc_mswunivuddll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswunivud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivuddll\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswunivud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivuddll\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswunivud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswunivud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivuddll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivuddll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 DLL Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivdll"
# PROP BASE Intermediate_Dir "vc_mswunivdll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivdll"
# PROP Intermediate_Dir "vc_mswunivdll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswuniv" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivdll\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswuniv" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivdll\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswuniv" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswuniv" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivdll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivdll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 DLL Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivddll"
# PROP BASE Intermediate_Dir "vc_mswunivddll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivddll"
# PROP Intermediate_Dir "vc_mswunivddll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswunivd" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivddll\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswunivd" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivddll\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswunivd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswunivd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivddll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmswuniv26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivddll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 DLL Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswudll"
# PROP BASE Intermediate_Dir "vc_mswudll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswudll"
# PROP Intermediate_Dir "vc_mswudll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswudll\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswudll\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswudll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswudll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 DLL Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswuddll"
# PROP BASE Intermediate_Dir "vc_mswuddll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswuddll"
# PROP Intermediate_Dir "vc_mswuddll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuddll\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuddll\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswuddll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswuddll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswdll"
# PROP BASE Intermediate_Dir "vc_mswdll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswdll"
# PROP Intermediate_Dir "vc_mswdll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\msw" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswdll\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_dll\msw" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswdll\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\msw" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\msw" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswdll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswdll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswddll"
# PROP BASE Intermediate_Dir "vc_mswddll\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswddll"
# PROP Intermediate_Dir "vc_mswddll\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswd" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswddll\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_dll\mswd" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswddll\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_dll\mswd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "WXUSINGDLL" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswddll\client.exe" /libpath:".\..\..\lib\vc_dll"
# ADD LINK32 wxmsw26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswddll\client.exe" /libpath:".\..\..\lib\vc_dll"

!ELSEIF  "$(CFG)" == "client - Win32 Universal Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswunivu"
# PROP BASE Intermediate_Dir "vc_mswunivu\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswunivu"
# PROP Intermediate_Dir "vc_mswunivu\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswunivu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivu\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswunivu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivu\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswunivu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswunivu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivu\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswunivu\client.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "client - Win32 Universal Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivud"
# PROP BASE Intermediate_Dir "vc_mswunivud\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivud"
# PROP Intermediate_Dir "vc_mswunivud\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswunivud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivud\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswunivud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivud\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswunivud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswunivud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivud\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivud\client.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "client - Win32 Universal Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswuniv"
# PROP BASE Intermediate_Dir "vc_mswuniv\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswuniv"
# PROP Intermediate_Dir "vc_mswuniv\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswuniv" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuniv\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswuniv" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswuniv\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswuniv" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswuniv" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswuniv\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswuniv\client.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "client - Win32 Universal Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswunivd"
# PROP BASE Intermediate_Dir "vc_mswunivd\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswunivd"
# PROP Intermediate_Dir "vc_mswunivd\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswunivd" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivd\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswunivd" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswunivd\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXUNIVERSAL__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswunivd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswunivd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXUNIVERSAL__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmswuniv26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivd\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmswuniv26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswunivd\client.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "client - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_mswu"
# PROP BASE Intermediate_Dir "vc_mswu\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_mswu"
# PROP Intermediate_Dir "vc_mswu\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswu\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswu" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswu\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswu" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswu\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw26u_core.lib wxbase26u_net.lib wxbase26u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_mswu\client.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "client - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswud"
# PROP BASE Intermediate_Dir "vc_mswud\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswud"
# PROP Intermediate_Dir "vc_mswud\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswud\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswud" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswud\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_UNICODE" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswud" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_UNICODE" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswud\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw26ud_core.lib wxbase26ud_net.lib wxbase26ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswud\client.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "client - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vc_msw"
# PROP BASE Intermediate_Dir "vc_msw\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vc_msw"
# PROP Intermediate_Dir "vc_msw\client"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\msw" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_msw\client.pdb" /FD /EHsc /c
# ADD CPP /nologo /MD /W4 /GR /O2 /I ".\..\..\include" /I ".\..\..\lib\vc_lib\msw" /I "." /I ".\..\..\samples" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_msw\client.pdb" /FD /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\msw" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\msw" /i "." /i ".\..\..\samples" /d "__WXMSW__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_msw\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw26_core.lib wxbase26_net.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /machine:I386 /out:"vc_msw\client.exe" /libpath:".\..\..\lib\vc_lib"

!ELSEIF  "$(CFG)" == "client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vc_mswd"
# PROP BASE Intermediate_Dir "vc_mswd\client"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vc_mswd"
# PROP Intermediate_Dir "vc_mswd\client"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I ".\..\..\include" /I ".\..\..\lib\vc_lib\mswd" /I "." /I ".\..\..\samples" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswd\client.pdb" /FD /GZ /EHsc /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /Zi /Od /I "L:\W2K\wxWidgets-2.6.1\include" /I "L:\W2K\wxWidgets-2.6.1\lib\vc_lib\mswd" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /Fd"vc_mswd\client.pdb" /FD /GZ /EHsc /c
# ADD BASE MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "_WINDOWS" /D "NOPCH" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
# ADD RSC /l 0x409 /i ".\..\..\include" /i ".\..\..\lib\vc_lib\mswd" /i "." /i ".\..\..\samples" /d "_DEBUG" /d "__WXMSW__" /d "__WXDEBUG__" /d "_WINDOWS" /d "NOPCH"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxmsw26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswd\client.exe" /libpath:".\..\..\lib\vc_lib"
# ADD LINK32 wxmsw26d_core.lib wxbase26d_net.lib wxbase26d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"vc_mswd\client.exe" /libpath:"L:\W2K\wxWidgets-2.6.1\lib\vc_lib"

!ENDIF 

# Begin Target

# Name "client - Win32 DLL Universal Unicode Release"
# Name "client - Win32 DLL Universal Unicode Debug"
# Name "client - Win32 DLL Universal Release"
# Name "client - Win32 DLL Universal Debug"
# Name "client - Win32 DLL Unicode Release"
# Name "client - Win32 DLL Unicode Debug"
# Name "client - Win32 DLL Release"
# Name "client - Win32 DLL Debug"
# Name "client - Win32 Universal Unicode Release"
# Name "client - Win32 Universal Unicode Debug"
# Name "client - Win32 Universal Release"
# Name "client - Win32 Universal Debug"
# Name "client - Win32 Unicode Release"
# Name "client - Win32 Unicode Debug"
# Name "client - Win32 Release"
# Name "client - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\client.cpp
# End Source File
# End Group
# End Target
# End Project
