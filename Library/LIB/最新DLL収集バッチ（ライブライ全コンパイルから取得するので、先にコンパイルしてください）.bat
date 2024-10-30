#======================
@echo on
#copy "..\KizuForm\bin\debug\KizuForm.dll" 		".\debug\"	/Y
#copy "..\KizuForm\bin\debug\tClassForm.dll" 		".\debug\"	/Y
#copy "..\KizuForm\bin\debug\tClassLibrary.dll" 		".\debug\"	/Y

#copy "..\KizuForm\bin\Release\KizuForm.dll" 		".\Release\"	/Y
#copy "..\KizuForm\bin\Release\tClassForm.dll" 		".\Release\"	/Y
#copy "..\KizuForm\bin\Release\tClassLibrary.dll" 	".\Release\"	/Y


copy "..\ライブラリ全コンパイル\debug\*.*" 		".\debug\"	/Y
copy "..\ライブラリ全コンパイル\Release\*.*" 		".\Release\"	/Y
