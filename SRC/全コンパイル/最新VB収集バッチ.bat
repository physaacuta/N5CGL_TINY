REM #======================
@echo on

REM # Debug Copy
REM # ----- DLL -----
copy "..\HY_TANSHO\bin\Debug\KizuForm.dll" 		".\debug\"	/Y
copy "..\HY_TANSHO\bin\Debug\tClassForm.dll" 		".\debug\"	/Y
copy "..\HY_TANSHO\bin\Debug\tClassLibrary.dll" 	".\debug\"	/Y

copy "..\HY_TANSHO\bin\Debug\KizuForm.pdb" 		".\debug\"	/Y
copy "..\HY_TANSHO\bin\Debug\tClassForm.pdb" 		".\debug\"	/Y
copy "..\HY_TANSHO\bin\Debug\tClassLibrary.pdb" 	".\debug\"	/Y

REM # ----- EXE -----
copy "..\HY_TANSHO\bin\Debug\HY_TANSHO.exe" 		".\debug\"	/Y
copy "..\HY_DEFLIST\bin\Debug\HY_DEFLIST.exe" 		".\debug\"	/Y
copy "..\SO_DEFMAP\bin\Debug\SO_DEFMAP.exe" 		".\debug\"	/Y
copy "..\SO_TRAND\bin\Debug\SO_TRAND.exe" 		".\debug\"	/Y
copy "..\KA_PARCOLOR\bin\Debug\KA_PARCOLOR.exe" 	".\debug\"	/Y
copy "..\KA_PARSET\bin\Debug\KA_PARSET.exe" 		".\debug\"	/Y
copy "..\..\SRC_LIB\KS_WAKEUP\bin\Debug\KS_WAKEUP.exe" 		".\debug\"	/Y
copy "..\SO_GAMENN\bin\Debug\SO_GAMENN.exe" 		".\debug\"	/Y
copy "..\KS_BACKUP\bin\Debug\KS_BACKUP.exe" 		".\debug\"	/Y

copy "..\HY_TANSHO\bin\Debug\HY_TANSHO.pdb" 		".\debug\"	/Y
copy "..\HY_DEFLIST\bin\Debug\HY_DEFLIST.pdb" 		".\debug\"	/Y
copy "..\SO_DEFMAP\bin\Debug\SO_DEFMAP.pdb" 		".\debug\"	/Y
copy "..\SO_TRAND\bin\Debug\SO_TRAND.pdb" 		".\debug\"	/Y
copy "..\KA_PARCOLOR\bin\Debug\KA_PARCOLOR.pdb" 	".\debug\"	/Y
copy "..\KA_PARSET\bin\Debug\KA_PARSET.pdb" 		".\debug\"	/Y
copy "..\..\SRC_LIB\KS_WAKEUP\bin\Debug\KS_WAKEUP.pdb" 		".\debug\"	/Y
copy "..\SO_GAMENN\bin\Debug\SO_GAMENN.pdb" 		".\debug\"	/Y
copy "..\KS_BACKUP\bin\Debug\KS_BACKUP.pdb" 		".\debug\"	/Y

REM # Release Copy
REM # ----- DLL -----
copy "..\HY_TANSHO\bin\Release\KizuForm.dll" 		".\release\"	/Y
copy "..\HY_TANSHO\bin\Release\tClassForm.dll" 		".\release\"	/Y
copy "..\HY_TANSHO\bin\Release\tClassLibrary.dll" 	".\release\"	/Y

copy "..\HY_TANSHO\bin\Release\KizuForm.pdb" 		".\release\"	/Y
copy "..\HY_TANSHO\bin\Release\tClassForm.pdb" 		".\release\"	/Y
copy "..\HY_TANSHO\bin\Release\tClassLibrary.pdb" 	".\release\"	/Y

REM # ----- EXE -----
copy "..\HY_TANSHO\bin\Release\HY_TANSHO.exe" 		".\release\"	/Y
copy "..\HY_DEFLIST\bin\Release\HY_DEFLIST.exe" 		".\release\"	/Y
copy "..\SO_DEFMAP\bin\Release\SO_DEFMAP.exe" 		".\release\"	/Y
copy "..\SO_TRAND\bin\Release\SO_TRAND.exe" 		".\release\"	/Y
copy "..\KA_PARCOLOR\bin\Release\KA_PARCOLOR.exe" 	".\release\"	/Y
copy "..\KA_PARSET\bin\Release\KA_PARSET.exe" 		".\release\"	/Y
copy "..\..\SRC_LIB\KS_WAKEUP\bin\Release\KS_WAKEUP.exe" 		".\release\"	/Y
copy "..\SO_GAMENN\bin\Release\SO_GAMENN.exe" 		".\release\"	/Y
copy "..\KS_BACKUP\bin\Release\KS_BACKUP.exe" 		".\release\"	/Y

copy "..\HY_TANSHO\bin\Release\HY_TANSHO.pdb" 		".\release\"	/Y
copy "..\HY_DEFLIST\bin\Release\HY_DEFLIST.pdb" 		".\release\"	/Y
copy "..\SO_DEFMAP\bin\Release\SO_DEFMAP.pdb" 		".\release\"	/Y
copy "..\SO_TRAND\bin\Release\SO_TRAND.pdb" 		".\release\"	/Y
copy "..\KA_PARCOLOR\bin\Release\KA_PARCOLOR.pdb" 	".\release\"	/Y
copy "..\KA_PARSET\bin\Release\KA_PARSET.pdb" 		".\release\"	/Y
copy "..\..\SRC_LIB\KS_WAKEUP\bin\Release\KS_WAKEUP.pdb" 		".\release\"	/Y
copy "..\SO_GAMENN\bin\Release\SO_GAMENN.pdb" 		".\release\"	/Y
copy "..\KS_BACKUP\bin\Release\KS_BACKUP.pdb" 		".\release\"	/Y


pause
