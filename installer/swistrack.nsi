; Script generated with the Venis Install Wizard

; Define your application name
!define APPNAME "SwisTrack"
!define APPNAMEANDVERSION "SwisTrack 3.0.0beta"


; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\SwisTrack"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
;OutFile "K:\WWW\Temp\Nikolaus\SWISTrack_v3.exe"
OutFile "../SWISTrack_v3.exe"


; Modern interface settings
!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\swistrack.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\release\license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

UninstallIcon gui.ico
Icon gui.ico
; AddBrandingImage left 0 swislogo_small.png

Section "SwisTrack" Section1

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "..\release\1394camera.dll"
	File "..\release\calibrationpattern.pdf"
	File "..\release\cv099.dll"
	File "..\release\cxcore099.dll"
	File "..\release\highgui099.dll"
	File "..\release\intellicense.txt"
	File "..\release\license.txt"
	File "..\release\suppfile.txt"
	File "..\release\swistrack.exe"
	File "..\changelog.txt"
	CreateShortCut "$DESKTOP\SwisTrack.lnk" "$INSTDIR\swistrack.exe"
	CreateDirectory "$SMPROGRAMS\SwisTrack"
	CreateShortCut "$SMPROGRAMS\SwisTrack\SwisTrack.lnk" "$INSTDIR\swistrack.exe"
	CreateShortCut "$SMPROGRAMS\SwisTrack\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd


Section "Example" Section2

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\example\"
	File "..\release\example\bg.bmp"
	File "..\release\example\bg_calibration.bmp"
	File "..\release\example\calibration.bmp"
	File "..\release\example\example.avi"
	File "..\release\example\example.cfg"
	File "..\release\example\mask.bmp"
File "..\release\example\Thumbs.db"

SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} ""
	!insertmacro MUI_DESCRIPTION_TEXT ${Section2} ""
	!insertmacro MUI_DESCRIPTION_TEXT ${Section3} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$DESKTOP\SWISTrack.lnk"
	Delete "$SMPROGRAMS\SWISTrack\SWISTrack.lnk"
	Delete "$SMPROGRAMS\SWISTrack\Uninstall.lnk"

	; Clean up SWISTrack
	Delete "$INSTDIR\1394camera.dll"
	Delete "$INSTDIR\calibrationpattern.pdf"
	Delete "$INSTDIR\cv097.dll"
	Delete "$INSTDIR\cxcore097.dll"
	Delete "$INSTDIR\highgui097.dll"
	Delete "$INSTDIR\intellicense.txt"
	Delete "$INSTDIR\license.txt"
	Delete "$INSTDIR\suppfile.txt"
	Delete "$INSTDIR\swistrack.exe"

	; Clean up Example
	Delete "$INSTDIR\bg.bmp"
	Delete "$INSTDIR\bg_calibration.bmp"
	Delete "$INSTDIR\calibration.bmp"
	Delete "$INSTDIR\example.avi"
	Delete "$INSTDIR\example.cfg"
	Delete "$INSTDIR\example_nocalibandmask.cfg"
	Delete "$INSTDIR\mask.bmp"
	Delete "$INSTDIR\Thumbs.db"

	; Remove remaining directories
	RMDir "$SMPROGRAMS\SWISTrack"
	RMDir "$INSTDIR\"

SectionEnd
BrandingText "Swarm Intelligent Systems Group, EPFL, Switzerland"
; eof
