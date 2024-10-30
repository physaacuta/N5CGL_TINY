@echo off

rem #===================================================
rem # 入力値
rem #   ・コピー先のPC名称
rem #===================================================

echo # 入力例
echo # 
echo #  判定PC01 (HANTEI01)
echo #  ～
echo #  判定PC16 (HANTEI16)
echo # 

set /p PCNAME=対象の判定PCのコンピュータ名(大文字)を入力してください。(例:HANTEI01):


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
mkdir %ToPath%背景

rem # 本体フォルダ
xcopy %FromPath%Logic 				%ToPath%Logic					/E /Y /R
xcopy %FromPath%背景	 	%ToPath%背景		/E /Y /R

rem # 本体ファイル & バッチファイル
copy "%FromPath%EXE\KS_STARTx64.exe"			"%ToPath%EXE\KS_STARTx64.exe"								/Y
IF /i %PCNAME% == HANTEI01		copy "%FromPath%IniSetting_%BaseName%\p判定PC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI02		copy "%FromPath%IniSetting_%BaseName%\p判定PC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI03		copy "%FromPath%IniSetting_%BaseName%\p判定PC03 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI04		copy "%FromPath%IniSetting_%BaseName%\p判定PC04 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI05		copy "%FromPath%IniSetting_%BaseName%\p判定PC05 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI06		copy "%FromPath%IniSetting_%BaseName%\p判定PC06 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI07		copy "%FromPath%IniSetting_%BaseName%\p判定PC07 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI08		copy "%FromPath%IniSetting_%BaseName%\p判定PC08 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI09		copy "%FromPath%IniSetting_%BaseName%\p判定PC09 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI10		copy "%FromPath%IniSetting_%BaseName%\p判定PC10 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI11		copy "%FromPath%IniSetting_%BaseName%\p判定PC11 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI12		copy "%FromPath%IniSetting_%BaseName%\p判定PC12 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI13		copy "%FromPath%IniSetting_%BaseName%\p判定PC13 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI14		copy "%FromPath%IniSetting_%BaseName%\p判定PC14 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI15		copy "%FromPath%IniSetting_%BaseName%\p判定PC15 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI16		copy "%FromPath%IniSetting_%BaseName%\p判定PC16 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y

rem # スクリプト等 付帯ファイル
rem copy "%FromPath%EXE\startup.dcd"			"%ToPath%Logic\startup.dcd"								/Y
rem copy "%FromPath%EXE\startup_pd.dcd"			"%ToPath%Logic\startup_pd.dcd"								/Y
rem copy "%FromPath%EXE\startup.dcd"			"%ToPath%EXE\startup.dcd"								/Y
rem copy "%FromPath%EXE\startup_pd.dcd"			"%ToPath%EXE\startup_pd.dcd"								/Y


rem # ショートカット等々
copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_STARTx64.exe - ショートカット.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - ショートカット.lnk"		/Y
copy "%FromPath%画像処理バッチ\deloldlog_%BaseName%.bat"					"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\deloldlog_%BaseName%.bat"		/Y
rem copy "%FromPath%画像処理バッチ\レジストリ登録_判定登録_%BaseName%.bat"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\レジストリ登録_判定登録_%BaseName%.bat"		/Y



rem # iPortViewer関係
mkdir %ToPath%iPortViewer
xcopy %FromPath%iPortViewer	 	%ToPath%iPortViewer		/E /Y /R
copy "%FromPath%iPortViewer\iPortViewer.exe - ショートカット.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\iPortViewer.exe - ショートカット.lnk"
								  

echo # 
echo # コピーが正常に完了しました。
echo # 
pause