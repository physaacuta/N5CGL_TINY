@echo off

rem #===================================================
rem # 固定定数
rem #===================================================
rem # ベースフォルダ
set BaseName=N5CGL_TINY

rem # コピー元のベースパス
set BASE_FROM=C:\%BaseName%\
rem # コピー先のベースパス
set BASE_TO=c$\%BaseName%\


rem #===================================================
rem # 指定ＰＣにコピー
rem #===================================================
@echo on

xcopy %BASE_FROM%Logic \\hantei01\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei02\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei03\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei04\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei05\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei06\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei07\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei08\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei09\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei10\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei11\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei12\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei13\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei14\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei15\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\hantei16\%BASE_TO%Logic /E /Y


xcopy %BASE_FROM%Logic \\TOKATU\%BASE_TO%Logic /E /Y
xcopy %BASE_FROM%Logic \\BACKUP\%BASE_TO%Logic /E /Y
pause