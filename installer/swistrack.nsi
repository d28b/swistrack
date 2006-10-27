; Script generated with the Venis Install Wizard

; Define your application name
!define APPNAME "SwisTrack"
!define APPNAMEANDVERSION "SwisTrack"


; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\SwisTrack"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "swistrack_installer.exe"
Icon "gui.ico"
UninstallIcon "gui.ico"
WindowIcon on

LicenseData "..\release\license.txt"
LicenseText "If you accept the terms of the agreement, click I Agree to continue. You must accept the agreement to install ${APPNAMEANDVERSION}."
LicenseForceSelection checkbox "I accept"

ComponentText "Choose which features of ${APPNAMEANDVERSION} you want to install."

DirText "Choose the folder in which to install ${APPNAMEANDVERSION}."

Section "SwisTrack"

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "..\release\1394camera.dll"
	File "..\release\calibrationpattern.pdf"
	File "..\release\cv100.dll"
	File "..\release\cxcore100.dll"
	File "..\release\default.cfg"
	File "..\release\highgui100.dll"
	File "..\release\iconv.dll"
	File "..\release\intellicense.txt"
	File "..\release\libgeom_dll.dll"
	File "..\release\libguide40.dll"
	File "..\release\libxml2.dll"
	File "..\release\license.txt"
	File "..\release\suppfile.txt"
	File "..\release\swistrack.exe"
	File "..\release\swistrack.exp"
	File "..\release\vispolygon_dll.dll"
	File "..\release\zlib1.dll"
	File "..\release\vcredist_x86.exe"
	CreateShortCut "$DESKTOP\SwisTrack.lnk" "$INSTDIR\swistrack.exe"
	CreateDirectory "$SMPROGRAMS\SwisTrack"
	CreateShortCut "$SMPROGRAMS\SwisTrack\SwisTrack.lnk" "$INSTDIR\swistrack.exe"
	CreateShortCut "$SMPROGRAMS\SwisTrack\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

Section "SwisTrack TCP/IP clients"

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "..\release\swistrack-clients\README.TXT"
	SetOutPath "$INSTDIR\C++-Client\"
	File "..\release\swistrack-clients\C++-Client\client.cpp"
	File "..\release\swistrack-clients\C++-Client\client.rc"
	File "..\release\swistrack-clients\C++-Client\connect.ico"
	File "..\release\swistrack-clients\C++-Client\README.TXT"
	File "..\release\swistrack-clients\C++-Client\sockets_client.dsp"
	File "..\release\swistrack-clients\C++-Client\sockets_client.dsw"
	File "..\release\swistrack-clients\C++-Client\sockets_client.ncb"
	File "..\release\swistrack-clients\C++-Client\sockets_client.opt"
	File "..\release\swistrack-clients\C++-Client\sockets_client.plg"
	SetOutPath "$INSTDIR\Java-Client\"
	File "..\release\swistrack-clients\Java-Client\.classpath"
	File "..\release\swistrack-clients\Java-Client\.project"
	SetOutPath "$INSTDIR\Java-Client\classes\client\"
	File "..\release\swistrack-clients\Java-Client\classes\client\ClientThread.class"
	File "..\release\swistrack-clients\Java-Client\classes\client\Runner.class"
	SetOutPath "$INSTDIR\Java-Client\src\client\"
	File "..\release\swistrack-clients\Java-Client\src\client\ClientThread.java"
	File "..\release\swistrack-clients\Java-Client\src\client\Runner.java"
	SetOutPath "$INSTDIR\MATLAB-Client\"
	File "..\release\swistrack-clients\MATLAB-Client\README.asv"
	File "..\release\swistrack-clients\MATLAB-Client\swistrackclient.asv"
	File "..\release\swistrack-clients\MATLAB-Client\swistrackclient.m"

SectionEnd

Section "Example"

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\example\"
	File "..\release\example\example.cfg"
  File "..\release\example\bg.bmp"
	File "..\release\example\bg_calibration.bmp"
	File "..\release\example\calibration.bmp"
	File "..\release\example\example.avi"
	File "..\release\example\mask.bmp"
	File "..\release\example\segmentationMask.bmp"
	File "..\release\example\Thumbs.db"

SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

	Exec "$INSTDIR\vcredist_x86.exe"
	Delete "$INSTDIR\vcredist_x86.exe"
	

SectionEnd

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$DESKTOP\SwisTrack.lnk"
	Delete "$SMPROGRAMS\SwisTrack\SwisTrack.lnk"
	Delete "$SMPROGRAMS\SwisTrack\Uninstall.lnk"

	; Clean up SwisTrack
	Delete "$INSTDIR\1394camera.dll"
	Delete "$INSTDIR\calibrationpattern.pdf"
	Delete "$INSTDIR\cv100.dll"
	Delete "$INSTDIR\cxcore100.dll"
	Delete "$INSTDIR\default.cfg"
	Delete "$INSTDIR\highgui100.dll"
	Delete "$INSTDIR\iconv.dll"
	Delete "$INSTDIR\intellicense.txt"
	Delete "$INSTDIR\libgeom_dll.dll"
	Delete "$INSTDIR\libguide40.dll"
	Delete "$INSTDIR\libxml2.dll"
	Delete "$INSTDIR\license.txt"
	Delete "$INSTDIR\suppfile.txt"
	Delete "$INSTDIR\swistrack.exe"
	Delete "$INSTDIR\swistrack.exp"
	Delete "$INSTDIR\vispolygon_dll.dll"
	Delete "$INSTDIR\zlib1.dll"

	; Clean up SwisTrack TCP/IP clients
	Delete "$INSTDIR\README.TXT"
	Delete "$INSTDIR\C++-Client\client.cpp"
	Delete "$INSTDIR\C++-Client\client.rc"
	Delete "$INSTDIR\C++-Client\connect.ico"
	Delete "$INSTDIR\C++-Client\README.TXT"
	Delete "$INSTDIR\C++-Client\sockets_client.dsp"
	Delete "$INSTDIR\C++-Client\sockets_client.dsw"
	Delete "$INSTDIR\C++-Client\sockets_client.ncb"
	Delete "$INSTDIR\C++-Client\sockets_client.opt"
	Delete "$INSTDIR\C++-Client\sockets_client.plg"
	Delete "$INSTDIR\Java-Client\.classpath"
	Delete "$INSTDIR\Java-Client\.project"
	Delete "$INSTDIR\Java-Client\classes\client\ClientThread.class"
	Delete "$INSTDIR\Java-Client\classes\client\Runner.class"
	Delete "$INSTDIR\Java-Client\src\client\ClientThread.java"
	Delete "$INSTDIR\Java-Client\src\client\Runner.java"
	Delete "$INSTDIR\MATLAB-Client\README.asv"
	Delete "$INSTDIR\MATLAB-Client\swistrackclient.asv"
	Delete "$INSTDIR\MATLAB-Client\swistrackclient.m"

	; Clean up Example
	Delete "$INSTDIR\bg.bmp"
	Delete "$INSTDIR\bg_calibration.bmp"
	Delete "$INSTDIR\calibration.bmp"
	Delete "$INSTDIR\example.avi"
	Delete "$INSTDIR\example.cfg"
	Delete "$INSTDIR\mask.bmp"
	Delete "$INSTDIR\segmentationMask.bmp"
	Delete "$INSTDIR\Thumbs.db"

	; Remove remaining directories
	RMDir "$SMPROGRAMS\SwisTrack"
	RMDir "$INSTDIR\MATLAB-Client\"
	RMDir "$INSTDIR\Java-Client\src\client\"
	RMDir "$INSTDIR\Java-Client\src\"
	RMDir "$INSTDIR\Java-Client\classes\client\"
	RMDir "$INSTDIR\Java-Client\classes\"
	RMDir "$INSTDIR\Java-Client\"
	RMDir "$INSTDIR\C++-Client\"
	RMDir "$INSTDIR\"

SectionEnd

Function un.onInit

	MessageBox MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION "Remove ${APPNAMEANDVERSION} and all of its components?" IDYES DoUninstall
		Abort
	DoUninstall:

FunctionEnd

BrandingText "Swarm-Intelligent Systems Group 2006"

; eof