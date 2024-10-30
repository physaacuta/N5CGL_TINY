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
rem # 入力値
rem #   ・コピー元
rem #===================================================
rem # set CopyName=EXE\KS_START.EXE
set /p CopyName=コピー元のファイルを入力してください(%BASE_FROM%):





rem #===================================================
rem # 指定ＰＣにコピー
rem #===================================================
@echo on

copy "%BASE_FROM%%CopyName%" "\\BACKUP\%BASE_TO%%CopyName%"	/Y

copy "%BASE_FROM%%CopyName%" "\\RPIEXEC01\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\RPIEXEC02\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\RPIEXEC03\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\RPISAVE\%BASE_TO%%CopyName%"	/Y

copy "%BASE_FROM%%CopyName%" "\\HYOZI01\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HYOZI02\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HYOZIOFF01\%BASE_TO%%CopyName%"	/Y

copy "%BASE_FROM%%CopyName%" "\\SOUSA\%BASE_TO%%CopyName%"	/Y

copy "%BASE_FROM%%CopyName%" "\\KAISEKI01\%BASE_TO%%CopyName%"	/Y

copy "%BASE_FROM%%CopyName%" "\\HANTEI01\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI02\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI03\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI04\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI05\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI06\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI07\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI08\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI09\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI10\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI11\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI12\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI13\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI14\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI15\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI16\%BASE_TO%%CopyName%"	/Y

copy "%BASE_FROM%%CopyName%" "\\RECORDER01\%BASE_TO%%CopyName%"	/Y


pause