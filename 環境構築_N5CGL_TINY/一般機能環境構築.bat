@echo off

rem #===================================================
rem # ���͒l
rem #   �E�R�s�[���PC�ԍ�
rem #===================================================

echo # 
echo # ���L�̃p�\�R�����Ώۂł��B
echo # 
echo # �@�\�ԍ� : �p�\�R������
echo #  1 : �\ ������\��PC  (HYOZI01)
echo #  2 : �� ������\��PC  (HYOZI02)
echo #  3 : ����PC           (SOUSA)
echo #  4 : �S���摜����PC01 (RPIEXEC01)
echo #  5 : �S���摜����PC02 (RPIEXEC02)
echo #  6 : �S���摜����PC03 (RPIEXEC03)
echo #  7 : �S���摜�ۑ�PC   (RPISAVE)
echo #  8 : �����p�\��PC01   (HYOZIOFF01)
echo #  9 : ���PC          �iKAISEKI01�j
rem # 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
echo # 10 : ������PC        �iLONGPD�j
rem # 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
echo # 

set /p SELECTNO=�Ώۂ̋@�\�ԍ�����͂��Ă��������B(��:1):

rem #===================================================
rem # �ΏۃR���s���[�^����
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
rem # 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
IF /i %SELECTNO% == 10 	set PCNAME=LONGPD
rem # 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

rem # �����グ����ʂ�o�^����PC
set DSP=0
IF /i %SELECTNO% == 1	set DSP=1
IF /i %SELECTNO% == 2	set DSP=1
IF /i %SELECTNO% == 3	set DSP=1
IF /i %SELECTNO% == 8	set DSP=1

rem # �S���摜��������PC
set RPI=0
IF /i %SELECTNO% == 4	set RPI=1
IF /i %SELECTNO% == 5	set RPI=1
IF /i %SELECTNO% == 6	set RPI=1

rem # �V���[�g�J�b�g�����N���f�X�N�g�b�v�ɓo�^����PC
set BK=0
IF /i %SELECTNO% == 3	set BK=1
IF /i %SELECTNO% == 6	set BK=1
IF /i %SELECTNO% == 8	set BK=1
IF /i %SELECTNO% == 9	set BK=1


rem # �V���[�g�J�b�g�����N���f�X�N�g�b�v�ɓo�^����PC
set LNK=0
IF /i %SELECTNO% == 1	set LNK=1
IF /i %SELECTNO% == 2	set LNK=1
IF /i %SELECTNO% == 3	set LNK=1
IF /i %SELECTNO% == 8	set LNK=1
IF /i %SELECTNO% == 9	set LNK=1

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

IF /i %SELECTNO% == 1		copy "%FromPath%IniSetting_%BaseName%\p�\��PC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 2		copy "%FromPath%IniSetting_%BaseName%\p�\��PC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 3		copy "%FromPath%IniSetting_%BaseName%\p����PC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 4		copy "%FromPath%IniSetting_%BaseName%\p�S���摜����PC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 5		copy "%FromPath%IniSetting_%BaseName%\p�S���摜����PC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 6		copy "%FromPath%IniSetting_%BaseName%\p�S���摜����PC03(�����X�V) KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
IF /i %SELECTNO% == 7		copy "%FromPath%IniSetting_%BaseName%\p�S���摜�ۑ�PC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %SELECTNO% == 8		copy "%FromPath%IniSetting_%BaseName%\p�\��PC�I�t���C��01(�����X�V) KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
IF /i %SELECTNO% == 9		copy "%FromPath%IniSetting_%BaseName%\p���PC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
rem # 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
IF /i %SELECTNO% == 10		copy "%FromPath%IniSetting_%BaseName%\t������PC KizuTask.ini"	"%ToPath%EXE\KizuPc.ini"		/Y	
rem # 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

rem # �V���[�g�J�b�g���X
rem copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Documents and Settings\Administrator\�X�^�[�g ���j���[\�v���O����\�X�^�[�g�A�b�v\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		/Y

copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		/Y
IF /i %BK% == 1		copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_BACKUP.exe - �V���[�g�J�b�g.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\�o�b�N�A�b�v�ؑ֎x��.lnk"


IF /i %DSP% == 1	copy "%FromPath%EXE\KS_WAKEUP.exe"	"%ToPath%EXE\KS_WAKEUP.exe"			/Y
IF /i %DSP% == 1	copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_WAKEUP.exe - �V���[�g�J�b�g.lnk"	"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_WAKEUP.exe - �V���[�g�J�b�g.lnk"		/Y


IF /i %LNK% == 1	copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\SO_GAMENN.exe - �V���[�g�J�b�g.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\SO_GAMENN.exe - �V���[�g�J�b�g.lnk"
IF /i %LNK% == 1	copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KA_PARSET.exe - �V���[�g�J�b�g.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\KA_PARSET.exe - �V���[�g�J�b�g.lnk"
IF /i %LNK% == 1	copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KA_PARCOLOR.exe - �V���[�g�J�b�g.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\KA_PARCOLOR.exe - �V���[�g�J�b�g.lnk"


IF /i %RPI% == 1	mkdir "\\%PCNAME%\f$\RPIWK"
IF /i %RPI% == 1	mkdir "\\%PCNAME%\f$\RPI_TEST"

echo # 
echo # �R�s�[������Ɋ������܂����B
echo # 
pause