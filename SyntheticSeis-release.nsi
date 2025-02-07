; Developed by Luiz Felipe Bertoldi de Oliveira
; author: Luiz Felipe Bertoldi de Oliveira
; email: feliperails@gmail.com
; date: January, 2025
;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "SyntheticSeismic 1.0.6"
  OutFile "SyntheticSeismic-1.0.6-release.exe"
  Unicode True

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\SyntheticSeis"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\SyntheticSeis" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user


;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING
  !define MUI_UNPAGE_CONFIRM_LEAVE none

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English" ; The first language is the default language

;--------------------------------
;Installer Sections

Section "SyntheticSeis" SecSyntheticSeis
  SectionIn RO
  SetOutPath "$INSTDIR"

  File /nonfatal /a /r "..\build\release\bin\python"
  File "..\build\release\bin\Qt5Core.dll"
  File "..\build\release\bin\Qt5Test.dll"
  File "..\build\release\bin\SyntheticSeismic.dll"
  File "..\build\release\bin\SyntheticSeismicConsole.exe"
  File "..\build\release\bin\VCOMP140.DLL"
  File "..\WindowsLibraries\gmp-10.dll"
  File "..\WindowsLibraries\Qt5Concurrent.dll"
  File "..\WindowsLibraries\Qt5Gui.dll"
  File "..\WindowsLibraries\Qt5Widgets.dll"
  File /nonfatal /a /r "..\WindowsLibraries\platforms"
  File "logo.ico"
  File "SyntheticSeisReadme.txt"

  ;Store installation folder
  WriteRegStr HKCU "Software\SyntheticSeis" "" $INSTDIR

  # Start Menu
  createDirectory "$SMPROGRAMS\SyntheticSeis"
  createShortCut "$SMPROGRAMS\SyntheticSeis.lnk" "$INSTDIR\SyntheticSeismicConsole.exe" "" "$INSTDIR\logo.ico"

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\SyntheticSeisUninstall.exe"

  SetRebootFlag false ;
  
SectionEnd

Section "SyntheticSeisPython" SecSyntheticSeisPython
  SectionIn RO
  SetOutPath "$INSTDIR"

  File /nonfatal /a /r "..\syntheticseis-python-windows\DLLs"
  File /nonfatal /a /r "..\syntheticseis-python-windows\include"
  File /nonfatal /a /r "..\syntheticseis-python-windows\Lib"
  File /nonfatal /a /r "..\syntheticseis-python-windows\libs"
  File /nonfatal /a /r "..\syntheticseis-python-windows\Scripts"
  File "..\syntheticseis-python-windows\python.exe"
  File "..\syntheticseis-python-windows\python3.dll"
  File "..\syntheticseis-python-windows\python312.dll"
  File "..\syntheticseis-python-windows\pythonw.exe"
  File "..\syntheticseis-python-windows\vcruntime140.dll"
  File "..\syntheticseis-python-windows\vcruntime140_1.dll"

  ;Store installation folder
  WriteRegStr HKCU "Software\SyntheticSeisPython" "" $INSTDIR

  SetRebootFlag false
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings - SyntheticSeis
  LangString DESC_SecSyntheticSeis ${LANG_ENGLISH} "SyntheticSeis 1.0"

  ;Language strings - SyntheticSeisPython
  LangString DESC_SecSyntheticSeisPython ${LANG_ENGLISH} "Python embeddable package (Python 3.12.8 + segyio +h5py)"
  

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	
	;Assign language strings to sections - SyntheticSeis
    !insertmacro MUI_DESCRIPTION_TEXT ${SecSyntheticSeis} $(DESC_SecSyntheticSeis)
	
	;Assign language strings to sections - SyntheticSeisPython
	!insertmacro MUI_DESCRIPTION_TEXT ${SecSyntheticSeisPython} $(DESC_SecSyntheticSeisPython)
  
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
  
;--------------------------------


# Uninstaller
Function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Are you sure you want to continue with the uninstallation of SyntheticSeis?" IDOK next
		Abort
	next:

FunctionEnd

;Uninstaller Section

Section "Uninstall"
  Delete "$INSTDIR\SyntheticSeisUninstall.exe"
  
  ;SyntheticSeis
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5Test.dll"
  Delete "$INSTDIR\SyntheticSeismic.dll"
  Delete "$INSTDIR\SyntheticSeismicConsole.exe"
  Delete "$INSTDIR\VCOMP140.DLL"
  Delete "$INSTDIR\SyntheticSeisReadme.txt"
  Delete "$INSTDIR\gmp-10.dll"
  
  RMDir /r /REBOOTOK "$INSTDIR\python"

  ;SyntheticSeisPython
  Delete "$INSTDIR\python.exe"
  Delete "$INSTDIR\python3.dll"
  Delete "$INSTDIR\python312.dll"
  Delete "$INSTDIR\pythonw.exe"
  Delete "$INSTDIR\vcruntime140.dll"
  Delete "$INSTDIR\vcruntime140_1.dll"
  
  RMDir /r /REBOOTOK "$INSTDIR\DLLs"
  RMDir /r /REBOOTOK "$INSTDIR\include"
  RMDir /r /REBOOTOK "$INSTDIR\Lib"
  RMDir /r /REBOOTOK "$INSTDIR\libs"
  RMDir /r /REBOOTOK "$INSTDIR\Scripts"
  RMDir /r /REBOOTOK "$INSTDIR\share"
  
  DeleteRegKey /ifempty HKCU "Software\SyntheticSeis"
  
  SetRebootFlag false

SectionEnd
