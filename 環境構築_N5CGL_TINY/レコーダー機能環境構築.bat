@echo off

rem #===================================================
rem # 入力値
rem #   ・コピー先のPC番号
rem #===================================================

echo # 
echo # 下記のパソコンが対象です。
echo # 
echo # 機能番号 : パソコン名称
echo #  1 : レコーダーPC01   (RECORDER01)
echo #  2 : レコーダーPC02   (RECORDER02)
echo #  3 : レコーダーPC03   (RECORDER03)
echo #  4 : レコーダーPC04   (RECORDER04)
echo # 

set /p SELECTNO=対象の機能番号を入力してください。(例:1):

rem #===================================================
rem # 対象コンピュータ決定
rem #===================================================
IF /i %SELECTNO% == 1	set PCNAME=RECORDER01
IF /i %SELECTNO% == 2	set PCNAME=RECORDER02
IF /i %SELECTNO% == 3	set PCNAME=RECORDER03
IF /i %SELECTNO% == 4	set PCNAME=RECORDER04

rem #===================================================
rem # 定数定義
rem #===================================================
rem # システム略称
set BaseName=N5CGL_TINY

rem # コピー元のベースパス
set FromPath=\\TOKATU\c$\%BaseName%\

rem # コピー先のベースパス
set ToPath=\\%PCNAME%\c$\%BaseName%\


echo # 
echo # 下記の設定でコピーを開始します。問題があれば、×ボタンで強制終了して下さい。
echo # 
echo # システム略称=%BaseName%
echo # コピー元のベースパス=%FromPath%
echo # コピー先のベースパス=%ToPath%
echo # 
pause


rem #===================================================
rem # コピー
rem #===================================================
rem # フォルダの作成
mkdir %ToPath%
mkdir %ToPath%EXE
mkdir %ToPath%背景


rem # 本体フォルダ
rem xcopy %FromPath%EXE 				%ToPath%EXE					/E /Y /R
xcopy %FromPath%背景	 	%ToPath%背景		/E /Y /R


rem # 本体ファイル & バッチファイル
copy "%FromPath%EXE\KS_STARTx64.exe"			"%ToPath%EXE\KS_STARTx64.exe"								/Y
copy "%FromPath%EXE\KizuLibCLIx64.dll"			"%ToPath%EXE\KizuLibCLIx64.dll"								/Y
copy "%FromPath%EXE\KizuLibCLIx32.dll"			"%ToPath%EXE\KizuLibCLIx32.dll"								/Y

IF /i %SELECTNO% == 1		copy "%FromPath%IniSetting_%BaseName%\pレコーダーPC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 2		copy "%FromPath%IniSetting_%BaseName%\pレコーダーPC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 3		copy "%FromPath%IniSetting_%BaseName%\pレコーダーPC03 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 4		copy "%FromPath%IniSetting_%BaseName%\pレコーダーPC04 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y


rem # ショートカット等々
rem copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_STARTx64.exe - ショートカット.lnk"		"\\%PCNAME%\c$\Documents and Settings\Administrator\スタート メニュー\プログラム\スタートアップ\KS_STARTx64.exe - ショートカット.lnk"		/Y

copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_STARTx64.exe - ショートカット.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - ショートカット.lnk"		/Y

echo # 
echo # コピーが正常に完了しました。
echo # 
pause