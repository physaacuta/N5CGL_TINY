# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Tcp_spVer2 - Win32 Debug
!MESSAGE 構成が指定されていません。ﾃﾞﾌｫﾙﾄの Tcp_spVer2 - Win32 Debug を設定します。
!ENDIF 

!IF "$(CFG)" != "Tcp_spVer2 - Win32 Release" && "$(CFG)" !=\
 "Tcp_spVer2 - Win32 Debug"
!MESSAGE 指定された ﾋﾞﾙﾄﾞ ﾓｰﾄﾞ "$(CFG)" は正しくありません。
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛ 'CFG' を定義することによって
!MESSAGE NMAKE 実行時にﾋﾞﾙﾄﾞ ﾓｰﾄﾞを指定できます。例えば:
!MESSAGE 
!MESSAGE NMAKE /f "Tcp_spVer2.mak" CFG="Tcp_spVer2 - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "Tcp_spVer2 - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "Tcp_spVer2 - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 
!ERROR 無効な構成が指定されています。
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Tcp_spVer2 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Tcp_spVer2.exe"

CLEAN : 
	-@erase ".\Release\Tcp_spVer2.exe"
	-@erase ".\Release\MakeDataSet.obj"
	-@erase ".\Release\tcp_setting.obj"
	-@erase ".\Release\Ap_commsub.obj"
	-@erase ".\Release\dump.obj"
	-@erase ".\Release\TcpSp2.obj"
	-@erase ".\Release\TcpSp2.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Tcp_spVer2.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/TcpSp2.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tcp_spVer2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Tcp_spVer2.pdb" /machine:I386 /out:"$(OUTDIR)/Tcp_spVer2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/MakeDataSet.obj" \
	"$(INTDIR)/tcp_setting.obj" \
	"$(INTDIR)/Ap_commsub.obj" \
	"$(INTDIR)/dump.obj" \
	"$(INTDIR)/TcpSp2.obj" \
	"$(INTDIR)/TcpSp2.res"

"$(OUTDIR)\Tcp_spVer2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Tcp_spVer2.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\Tcp_spVer2.exe"
	-@erase ".\Debug\MakeDataSet.obj"
	-@erase ".\Debug\tcp_setting.obj"
	-@erase ".\Debug\TcpSp2.obj"
	-@erase ".\Debug\Ap_commsub.obj"
	-@erase ".\Debug\dump.obj"
	-@erase ".\Debug\TcpSp2.res"
	-@erase ".\Debug\Tcp_spVer2.ilk"
	-@erase ".\Debug\Tcp_spVer2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/Tcp_spVer2.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
RSC_PROJ=/l 0x411 /fo"$(INTDIR)/TcpSp2.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tcp_spVer2.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Tcp_spVer2.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/Tcp_spVer2.exe" 
LINK32_OBJS= \
	"$(INTDIR)/MakeDataSet.obj" \
	"$(INTDIR)/tcp_setting.obj" \
	"$(INTDIR)/TcpSp2.obj" \
	"$(INTDIR)/Ap_commsub.obj" \
	"$(INTDIR)/dump.obj" \
	"$(INTDIR)/TcpSp2.res"

"$(OUTDIR)\Tcp_spVer2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Tcp_spVer2 - Win32 Release"
# Name "Tcp_spVer2 - Win32 Debug"

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\TcpSp2.c
DEP_CPP_TCPSP=\
	".\Ap_func.h"\
	".\MakeDataSet.h"\
	".\resource.h"\
	".\Dump.h"\
	".\TcpCtrl.h"\
	".\AP_File.h"\
	

"$(INTDIR)\TcpSp2.obj" : $(SOURCE) $(DEP_CPP_TCPSP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TcpCtrl.h

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tcp_setting.c
DEP_CPP_TCP_S=\
	".\resource.h"\
	".\Ap_func.h"\
	".\Dump.h"\
	".\TcpCtrl.h"\
	".\AP_File.h"\
	

"$(INTDIR)\tcp_setting.obj" : $(SOURCE) $(DEP_CPP_TCP_S) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.h

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MakeDataSet.h

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MakeDataSet.c
DEP_CPP_MAKED=\
	".\Ap_func.h"\
	".\MakeDataSet.h"\
	".\AP_File.h"\
	

"$(INTDIR)\MakeDataSet.obj" : $(SOURCE) $(DEP_CPP_MAKED) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Dump.h

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dump.c
DEP_CPP_DUMP_=\
	".\resource.h"\
	".\Dump.h"\
	

"$(INTDIR)\dump.obj" : $(SOURCE) $(DEP_CPP_DUMP_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ap_func.h

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AP_File.h

!IF  "$(CFG)" == "Tcp_spVer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Tcp_spVer2 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Ap_commsub.c
DEP_CPP_AP_CO=\
	".\Ap_func.h"\
	".\AP_File.h"\
	

"$(INTDIR)\Ap_commsub.obj" : $(SOURCE) $(DEP_CPP_AP_CO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TcpSp2.rc

"$(INTDIR)\TcpSp2.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
