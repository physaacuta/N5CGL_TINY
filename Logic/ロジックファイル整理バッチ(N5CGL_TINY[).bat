@echo off

setlocal ENABLEDELAYEDEXPANSION

rem #===================================================
rem # ���W�b�N�t�@�C���̊��S�����o�b�`
rem #   �E�Ώ�OS�́Awin7�ȍ~�BXP�̏ꍇ�́Arobocpy.exe���R�s�[���Ă����K�v�L��
rem #   �E�������t�H���_���Ɗ��S��v�����܂��B
rem #       (������ɖ����t�@�C���̓R�s�[�B������ɂ��������t�@�C���͍폜)
rem #===================================================

rem #===================================================
rem # �萔��`
rem #===================================================
rem # �V�X�e������
set BaseName=N5CGL_TINY


rem # ������PC
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

rem # PC����
set /A MAXPC=19



echo #===================================================
echo # ���W�b�N�t�@�C���̊��S�����o�b�`
echo #   �E�������R���s���[�^���́A��PC�������\�����Ă��܂��B
echo #   �E�������t�H���_���Ɗ��S��v�����܂��B
echo #       (������ɖ����t�@�C���̓R�s�[�B������ɂ��������t�@�C���͍폜)
echo #   �E�Ώ�OS�́Awin7�ȍ~
echo #===================================================
echo #

rem #��PC���� �������Z�b�g
set PCNAME=%COMPUTERNAME%

echo �������̃R���s���[�^��(�啶��)����͂��Ă��������B
set /p PCNAME=(%PCNAME%�Ŗ��Ȃ���΁A���̂܂�Enter)�F



rem # �R�s�[���̃x�[�X�p�X
set FromPath=\\%PCNAME%\c$\%BaseName%\Logic\



echo #
echo #
echo #===================================================
echo �������F%FromPath%

  
echo #---------------------------------
for /L %%i in (1,1,%MAXPC%) do (
  echo ������F\\!TOPC[%%i]!\c$\%BaseName%\Logic\
)
echo #
echo #
echo # ������t�H���_�ɂ��������t�@�C���͍폜����܂��̂ŁA�����ӂ��������B
echo # NG�̏ꍇ�́A�~�{�^���ŏI�������ĉ������B
echo #
pause
 





rem #===================================================
rem # �����J�n
rem # /R:���g���C����
rem # /W:���g���C��


for /L %%i in (1,1,%MAXPC%) do (
  ROBOCOPY  %FromPath% \\!TOPC[%%i]!\c$\%BaseName%\Logic\ /R:1 /W:1 /MIR

)


pause