@echo off

rem #===================================================
rem # 固定定数
rem #===================================================
rem # ベースフォルダ
set BaseName=N5CGL_TINY

rem # コピー元のベースパス
set BASE_FROM=C:\%BaseName%\Tool\TI_TEST\x64\Release\TI_TEST.exe
rem # コピー先のベースパス
set BASE_TO=c$\%BaseName%\ToolExe\


rem #===================================================
rem # 入力値
rem #   ・コピー元
rem #===================================================
rem # set CopyName=EXE\KS_START.EXE
rem set /p CopyName=コピー元のファイルを入力してください(%BASE_FROM%):





rem #===================================================
rem # 指定ＰＣにコピー
rem #===================================================
@echo on

copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_1\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_2\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_3\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_4\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_5\."	/Y

copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_1\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_2\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_3\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_4\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_5\."	/Y

copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_1\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_2\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_3\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_4\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_5\."	/Y


pause