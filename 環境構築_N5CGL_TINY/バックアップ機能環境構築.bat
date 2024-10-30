@echo off

rem #===================================================
rem # 入力値
rem #   ・コピー先のPC名称
rem #===================================================
rem # set /p PCNAME=対象の判定PCのコンピュータ名(大文字)を入力してください。(例:HANTEI01):
set PCNAME=BACKUP


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
mkdir %ToPath%Logic
mkdir %ToPath%IniSetting_%BaseName%
mkdir %ToPath%環境構築_%BaseName%
mkdir %ToPath%背景

mkdir %ToPath%iPortViewer


rem # 本体フォルダ
xcopy %FromPath%EXE 				%ToPath%EXE					/E /Y /R
xcopy %FromPath%Logic 				%ToPath%Logic					/E /Y /R
xcopy %FromPath%IniSetting_%BaseName%	 	%ToPath%IniSetting_%BaseName%		/E /Y /R
xcopy %FromPath%環境構築_%BaseName%	 	%ToPath%環境構築_%BaseName%		/E /Y /R
xcopy %FromPath%背景	 	%ToPath%背景		/E /Y /R

xcopy %FromPath%iPortViewer	 	%ToPath%iPortViewer		/E /Y /R



rem # 本体ファイル & バッチファイル
copy "%FromPath%IniSetting_%BaseName%\p統括バックアップPC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y


copy "%FromPath%ロジックファイル整理ツールVer02.xls"	"%ToPath%ロジックファイル整理ツールVer02.xls"		/Y
copy "%FromPath%ファイルコピーバッチ(%BaseName%).bat"	"%ToPath%ファイルコピーバッチ(%BaseName%).bat"		/Y
copy "%FromPath%ファイルコピーバッチ(%BaseName%) 判定のみ.bat"	"%ToPath%ファイルコピーバッチ(%BaseName%) 判定のみ.bat"		/Y


rem # ショートカット等々
copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_STARTx64.exe - ショートカット.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - ショートカット.lnk"		/Y

copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_BACKUP.exe - ショートカット.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\バックアップ切替支援.lnk"


echo # 
echo # コピーが正常に完了しました。
echo # 
pause