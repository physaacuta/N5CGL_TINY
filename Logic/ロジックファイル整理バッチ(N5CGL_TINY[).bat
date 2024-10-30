@echo off

setlocal ENABLEDELAYEDEXPANSION

rem #===================================================
rem # ロジックファイルの完全同期バッチ
rem #   ・対象OSは、win7以降。XPの場合は、robocpy.exeをコピーしておく必要有り
rem #   ・同期元フォルダ内と完全一致させます。
rem #       (同期先に無いファイルはコピー。同期先にしか無いファイルは削除)
rem #===================================================

rem #===================================================
rem # 定数定義
rem #===================================================
rem # システム略称
set BaseName=N5CGL_TINY


rem # 同期先PC
set TOPC[1]=TOKATU
set TOPC[2]=BACKUP
set TOPC[3]=HANTEI01
set TOPC[4]=HANTEI02
set TOPC[5]=HANTEI03
set TOPC[6]=HANTEI04
set TOPC[7]=HANTEI05
set TOPC[8]=HANTEI06
set TOPC[9]=HANTEI07
set TOPC[10]=HANTEI08
set TOPC[11]=HANTEI09
set TOPC[12]=HANTEI10
set TOPC[13]=HANTEI11
set TOPC[14]=HANTEI12
set TOPC[15]=HANTEI13
set TOPC[16]=HANTEI14
set TOPC[17]=HANTEI15
set TOPC[18]=HANTEI16
set TOPC[19]=KAISEKI01

rem # PC件数
set /A MAXPC=19



echo #===================================================
echo # ロジックファイルの完全同期バッチ
echo #   ・同期元コンピュータ名は、自PCを初期表示しています。
echo #   ・同期元フォルダ内と完全一致させます。
echo #       (同期先に無いファイルはコピー。同期先にしか無いファイルは削除)
echo #   ・対象OSは、win7以降
echo #===================================================
echo #

rem #自PC名称 を初期セット
set PCNAME=%COMPUTERNAME%

echo 同期元のコンピュータ名(大文字)を入力してください。
set /p PCNAME=(%PCNAME%で問題なければ、そのままEnter)：



rem # コピー元のベースパス
set FromPath=\\%PCNAME%\c$\%BaseName%\Logic\



echo #
echo #
echo #===================================================
echo 同期元：%FromPath%

  
echo #---------------------------------
for /L %%i in (1,1,%MAXPC%) do (
  echo 同期先：\\!TOPC[%%i]!\c$\%BaseName%\Logic\
)
echo #
echo #
echo # 同期先フォルダにしか無いファイルは削除されますので、ご注意ください。
echo # NGの場合は、×ボタンで終了させて下さい。
echo #
pause
 





rem #===================================================
rem # 同期開始
rem # /R:リトライ時間
rem # /W:リトライ回数


for /L %%i in (1,1,%MAXPC%) do (
  ROBOCOPY  %FromPath% \\!TOPC[%%i]!\c$\%BaseName%\Logic\ /R:1 /W:1 /MIR

)


pause