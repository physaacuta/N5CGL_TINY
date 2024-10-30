@echo off

rem #===================================================
rem # 入力値
rem #   ・コピー先のPC番号
rem #===================================================

echo # 
echo # 下記のパソコンが対象です。
echo # 
echo # 機能番号 : パソコン名称
echo #  1 : 表 検査台表示PC  (HYOZI01)
echo #  2 : 裏 検査台表示PC  (HYOZI02)
echo #  3 : 操作PC           (SOUSA)
echo #  4 : 全長画像処理PC01 (RPIEXEC01)
echo #  5 : 全長画像処理PC02 (RPIEXEC02)
echo #  6 : 全長画像処理PC03 (RPIEXEC03)
echo #  7 : 全長画像保存PC   (RPISAVE)
echo #  8 : 調整用表示PC01   (HYOZIOFF01)
echo #  9 : 解析PC          （KAISEKI01）
rem # 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
echo # 10 : 長周期PC        （LONGPD）
rem # 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
echo # 

set /p SELECTNO=対象の機能番号を入力してください。(例:1):

rem #===================================================
rem # 対象コンピュータ決定
rem #===================================================
IF /i %SELECTNO% == 1	set PCNAME=HYOZI01
IF /i %SELECTNO% == 2	set PCNAME=HYOZI02
IF /i %SELECTNO% == 3	set PCNAME=SOUSA
IF /i %SELECTNO% == 4	set PCNAME=RPIEXEC01
IF /i %SELECTNO% == 5	set PCNAME=RPIEXEC02
IF /i %SELECTNO% == 6	set PCNAME=RPIEXEC03
IF /i %SELECTNO% == 7	set PCNAME=RPISAVE
IF /i %SELECTNO% == 8	set PCNAME=HYOZIOFF01
IF /i %SELECTNO% == 9 	set PCNAME=KAISEKI01
rem # 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
IF /i %SELECTNO% == 10 	set PCNAME=LONGPD
rem # 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

rem # 立ち上げ中画面を登録するPC
set DSP=0
IF /i %SELECTNO% == 1	set DSP=1
IF /i %SELECTNO% == 2	set DSP=1
IF /i %SELECTNO% == 3	set DSP=1
IF /i %SELECTNO% == 8	set DSP=1

rem # 全長画像処理するPC
set RPI=0
IF /i %SELECTNO% == 4	set RPI=1
IF /i %SELECTNO% == 5	set RPI=1
IF /i %SELECTNO% == 6	set RPI=1

rem # ショートカットリンクをデスクトップに登録するPC
set BK=0
IF /i %SELECTNO% == 3	set BK=1
IF /i %SELECTNO% == 6	set BK=1
IF /i %SELECTNO% == 8	set BK=1
IF /i %SELECTNO% == 9	set BK=1


rem # ショートカットリンクをデスクトップに登録するPC
set LNK=0
IF /i %SELECTNO% == 1	set LNK=1
IF /i %SELECTNO% == 2	set LNK=1
IF /i %SELECTNO% == 3	set LNK=1
IF /i %SELECTNO% == 8	set LNK=1
IF /i %SELECTNO% == 9	set LNK=1

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

IF /i %SELECTNO% == 1		copy "%FromPath%IniSetting_%BaseName%\p表示PC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 2		copy "%FromPath%IniSetting_%BaseName%\p表示PC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 3		copy "%FromPath%IniSetting_%BaseName%\p操作PC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 4		copy "%FromPath%IniSetting_%BaseName%\p全長画像処理PC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 5		copy "%FromPath%IniSetting_%BaseName%\p全長画像処理PC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 6		copy "%FromPath%IniSetting_%BaseName%\p全長画像処理PC03(自動更新) KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
IF /i %SELECTNO% == 7		copy "%FromPath%IniSetting_%BaseName%\p全長画像保存PC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 8		copy "%FromPath%IniSetting_%BaseName%\p表示PCオフライン01(自動更新) KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
IF /i %SELECTNO% == 9		copy "%FromPath%IniSetting_%BaseName%\p解析PC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
rem # 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
IF /i %SELECTNO% == 10		copy "%FromPath%IniSetting_%BaseName%\t長周期PC KizuTask.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
rem # 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

rem # ショートカット等々
rem copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_STARTx64.exe - ショートカット.lnk"		"\\%PCNAME%\c$\Documents and Settings\Administrator\スタート メニュー\プログラム\スタートアップ\KS_STARTx64.exe - ショートカット.lnk"		/Y

copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_STARTx64.exe - ショートカット.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - ショートカット.lnk"		/Y
IF /i %BK% == 1		copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_BACKUP.exe - ショートカット.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\バックアップ切替支援.lnk"


IF /i %DSP% == 1	copy "%FromPath%EXE\KS_WAKEUP.exe"	"%ToPath%EXE\KS_WAKEUP.exe"			/Y
IF /i %DSP% == 1	copy "%FromPath%環境構築_%BaseName%\ショートカット\KS_WAKEUP.exe - ショートカット.lnk"	"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_WAKEUP.exe - ショートカット.lnk"		/Y


IF /i %LNK% == 1	copy "%FromPath%環境構築_%BaseName%\ショートカット\SO_GAMENN.exe - ショートカット.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\SO_GAMENN.exe - ショートカット.lnk"
IF /i %LNK% == 1	copy "%FromPath%環境構築_%BaseName%\ショートカット\KA_PARSET.exe - ショートカット.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\KA_PARSET.exe - ショートカット.lnk"
IF /i %LNK% == 1	copy "%FromPath%環境構築_%BaseName%\ショートカット\KA_PARCOLOR.exe - ショートカット.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\KA_PARCOLOR.exe - ショートカット.lnk"


IF /i %RPI% == 1	mkdir "\\%PCNAME%\f$\RPIWK"
IF /i %RPI% == 1	mkdir "\\%PCNAME%\f$\RPI_TEST"

echo # 
echo # コピーが正常に完了しました。
echo # 
pause