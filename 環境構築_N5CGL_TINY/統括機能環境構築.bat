@echo off

rem #===================================================
rem # ���͒l
rem #   �E�R�s�[���PC����
rem #===================================================
set PCNAME=TOKATU


rem #===================================================
rem # �萔��`
rem #===================================================
rem # �V�X�e������
set BaseName=N5CGL_TINY

rem # �R�s�[���̃x�[�X�p�X
set FromPath=\\BACKUP\c$\%BaseName%\

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
mkdir %ToPath%Logic
mkdir %ToPath%IniSetting_%BaseName%
mkdir %ToPath%���\�z_%BaseName%
mkdir %ToPath%�w�i



rem # �{�̃t�H���_
xcopy %FromPath%EXE 				%ToPath%EXE					/E /Y /R
xcopy %FromPath%Logic 				%ToPath%Logic					/E /Y /R
xcopy %FromPath%IniSetting_%BaseName%	 	%ToPath%IniSetting_%BaseName%		/E /Y /R
xcopy %FromPath%���\�z_%BaseName%	 	%ToPath%���\�z_%BaseName%		/E /Y /R
xcopy %FromPath%�w�i	 	%ToPath%�w�i		/E /Y /R


rem # �{�̃t�@�C�� & �o�b�`�t�@�C��
copy "%FromPath%IniSetting_%BaseName%\p����PC KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y

rem # copy "%FromPath%���W�b�N�t�@�C�������c�[��.xls"	"%ToPath%���W�b�N�t�@�C�������c�[��.xls"		/Y
copy "%FromPath%�t�@�C���R�s�[�o�b�`(%BaseName%).bat"	"%ToPath%�t�@�C���R�s�[�o�b�`(%BaseName%).bat"		/Y
copy "%FromPath%�t�@�C���R�s�[�o�b�`(%BaseName%) ����̂�.bat"	"%ToPath%�t�@�C���R�s�[�o�b�`(%BaseName%) ����̂�.bat"		/Y


rem # �V���[�g�J�b�g���X
copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		/Y

copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\SO_GAMENN.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Users\Administrator\Desktop\SO_GAMENN.exe - �V���[�g�J�b�g.lnk"
copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KA_PARSET.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Users\Administrator\Desktop\KA_PARSET.exe - �V���[�g�J�b�g.lnk"
copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KA_PARCOLOR.exe - �V���[�g�J�b�g.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\KA_PARCOLOR.exe - �V���[�g�J�b�g.lnk"


echo # 
echo # �R�s�[������Ɋ������܂����B
echo # 
pause