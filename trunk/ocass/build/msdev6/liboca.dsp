# Microsoft Developer Studio Project File - Name="liboca" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=liboca - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "liboca.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "liboca.mak" CFG="liboca - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "liboca - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "liboca - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "liboca - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "libocaRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOCA_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOCA_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 advapi32.lib dbghelp.lib /nologo /dll /machine:I386 /libpath:"../../support_libs/dbghlp" /libpath:"./release"

!ELSEIF  "$(CFG)" == "liboca - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "libocaDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOCA_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../liboca" /I "../../support_libs/dbghlp" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBOCA_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib dbghelp.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"../../support_libs/dbghlp" /libpath:"./debug"

!ENDIF 

# Begin Target

# Name "liboca - Win32 Release"
# Name "liboca - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\liboca\ca_base64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_cfg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_flock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_general.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_getopt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_inject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_misc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_mm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_ofc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_sr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_str.cpp
# End Source File
# Begin Source File

SOURCE=..\..\liboca\liboca.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\ca_base64.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_cfg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_errno.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_evts.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_flock.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_getopt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_inject.h
# End Source File
# Begin Source File

SOURCE=..\..\liboca\ca_inner.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_misc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_mm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_ofc.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_sr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_str.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_tlhlp.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ca_types.h
# End Source File
# Begin Source File

SOURCE=..\..\include\liboca.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
