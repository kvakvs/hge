# Microsoft Developer Studio Project File - Name="texasm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=texasm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "texasm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "texasm.mak" CFG="texasm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "texasm - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "texasm - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "texasm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib hge.lib hgehelp.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "texasm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib hge.lib hgehelp.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "texasm - Win32 Release"
# Name "texasm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ZLIB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ZLIB\adler32.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\compress.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\crc32.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\deflate.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\gzio.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\infback.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\inffast.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\inflate.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\trees.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\ZLIB\zutil.c
# End Source File
# End Group
# Begin Group "libPNG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libPNG\png.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngerror.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngget.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngmem.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngpread.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngread.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngrio.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngset.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngwio.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngwrite.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngwtran.c
# End Source File
# Begin Source File

SOURCE=.\libPNG\pngwutil.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\optimized_texture.cpp
# End Source File
# Begin Source File

SOURCE=.\RectPlacement.cpp
# End Source File
# Begin Source File

SOURCE=.\sprite_object.cpp
# End Source File
# Begin Source File

SOURCE=.\texasm.cpp
# End Source File
# Begin Source File

SOURCE=.\texture_assembler.cpp
# End Source File
# Begin Source File

SOURCE=.\writePNG.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gfx_object.h
# End Source File
# Begin Source File

SOURCE=.\optimized_texture.h
# End Source File
# Begin Source File

SOURCE=.\RectPlacement.h
# End Source File
# Begin Source File

SOURCE=.\sprite_object.h
# End Source File
# Begin Source File

SOURCE=.\texasm.h
# End Source File
# Begin Source File

SOURCE=.\texture_assembler.h
# End Source File
# End Group
# End Target
# End Project
