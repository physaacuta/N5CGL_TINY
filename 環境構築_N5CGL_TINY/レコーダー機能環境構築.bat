@echo off

rem #===================================================
rem # ���͒l
rem #   �E�R�s�[���PC�ԍ�
rem #===================================================

echo # 
echo # ���L�̃p�\�R�����Ώۂł��B
echo # 
echo # �@�\�ԍ� : �p�\�R������
echo #  1 : ���R�[�_�[PC01   (RECORDER01)
echo #  2 : ���R�[�_�[PC02   (RECORDER02)
echo #  3 : ���R�[�_�[PC03   (RECORDER03)
echo #  4 : ���R�[�_�[PC04   (RECORDER04)
echo # 

set /p SELECTNO=�Ώۂ̋@�\�ԍ�����͂��Ă��������B(��:1):

rem #===================================================
rem # �ΏۃR���s���[�^����
rem #===================================================
IF /i %SELECTNO% == 1	set PCNAME=RECORDER01
IF /i %SELECTNO% == 2	set PCNAME=RECORDER02
IF /i %SELECTNO% == 3	set PCNAME=RECORDER03
IF /i %SELECTNO% == 4	set PCNAME=RECORDER04

rem #===================================================
rem # �萔��`
rem #===================================================
rem # �V�X�e������
set BaseName=N5CGL_TINY

rem # �R�s�[���̃x�[�X�p�X
set FromPath=\\TOKATU\c$\%BaseName%\

rem # �R�s�[��̃x�[�X�p�X
set ToPath=\\%PCNAME%\c$\%BaseName%\


echo # 
echo # ���L�̐ݒ�ŃR�s�[���J�n���܂��B��肪����΁A�~�{�^���ŋ����I�����ĉ������B
echo # 
echo # �V�X�e������=%BaseName%
echo # �R�s�[���̃x�[�X�p�X=%FromPath%
echo # �R�s�[��̃x�[�X�p�X=%ToPath%
echo # 
pause


rem #===================================================
rem # �R�s�[
rem #===================================================
rem # �t�H���_�̍쐬
mkdir %ToPath%
mkdir %ToPath%EXE
mkdir %ToPath%�w�i


rem # �{�̃t�H���_
rem xcopy %FromPath%EXE 				%ToPath%EXE					/E /Y /R
xcopy %FromPath%�w�i	 	%ToPath%�w�i		/E /Y /R


rem # �{�̃t�@�C�� & �o�b�`�t�@�C��
copy "%FromPath%EXE\KS_STARTx64.exe"			"%ToPath%EXE\KS_STARTx64.exe"								/Y
copy "%FromPath%EXE\KizuLibCLIx64.dll"			"%ToPath%EXE\KizuLibCLIx64.dll"								/Y
copy "%FromPath%EXE\KizuLibCLIx32.dll"			"%ToPath%EXE\KizuLibCLIx32.dll"								/Y

IF /i %SELECTNO% == 1		copy "%FromPath%IniSetting_%BaseName%\p���R�[�_�[PC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 2		copy "%FromPath%IniSetting_%BaseName%\p���R�[�_�[PC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 3		copy "%FromPath%IniSetting_%BaseName%\p���R�[�_�[PC03 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 4		copy "%FromPath%IniSetting_%BaseName%\p���R�[�_�[PC04 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y


rem # �V���[�g�J�b�g���X
rem copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Documents and Settings\Administrator\�X�^�[�g ���j���[\�v���O����\�X�^�[�g�A�b�v\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		/Y

copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		/Y

echo # 
echo # �R�s�[������Ɋ������܂����B
echo # 
pause