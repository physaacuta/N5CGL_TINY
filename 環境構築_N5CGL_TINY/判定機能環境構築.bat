@echo off

rem #===================================================
rem # ���͒l
rem #   �E�R�s�[���PC����
rem #===================================================

echo # ���͗�
echo # 
echo #  ����PC01 (HANTEI01)
echo #  �`
echo #  ����PC16 (HANTEI16)
echo # 

set /p PCNAME=�Ώۂ̔���PC�̃R���s���[�^��(�啶��)����͂��Ă��������B(��:HANTEI01):


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
mkdir %ToPath%Logic
mkdir %ToPath%�w�i

rem # �{�̃t�H���_
xcopy %FromPath%Logic 				%ToPath%Logic					/E /Y /R
xcopy %FromPath%�w�i	 	%ToPath%�w�i		/E /Y /R

rem # �{�̃t�@�C�� & �o�b�`�t�@�C��
copy "%FromPath%EXE\KS_STARTx64.exe"			"%ToPath%EXE\KS_STARTx64.exe"								/Y
IF /i %PCNAME% == HANTEI01		copy "%FromPath%IniSetting_%BaseName%\p����PC01 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI02		copy "%FromPath%IniSetting_%BaseName%\p����PC02 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI03		copy "%FromPath%IniSetting_%BaseName%\p����PC03 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI04		copy "%FromPath%IniSetting_%BaseName%\p����PC04 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI05		copy "%FromPath%IniSetting_%BaseName%\p����PC05 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI06		copy "%FromPath%IniSetting_%BaseName%\p����PC06 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI07		copy "%FromPath%IniSetting_%BaseName%\p����PC07 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI08		copy "%FromPath%IniSetting_%BaseName%\p����PC08 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI09		copy "%FromPath%IniSetting_%BaseName%\p����PC09 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI10		copy "%FromPath%IniSetting_%BaseName%\p����PC10 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI11		copy "%FromPath%IniSetting_%BaseName%\p����PC11 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI12		copy "%FromPath%IniSetting_%BaseName%\p����PC12 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI13		copy "%FromPath%IniSetting_%BaseName%\p����PC13 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI14		copy "%FromPath%IniSetting_%BaseName%\p����PC14 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI15		copy "%FromPath%IniSetting_%BaseName%\p����PC15 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y
IF /i %PCNAME% == HANTEI16		copy "%FromPath%IniSetting_%BaseName%\p����PC16 KizuPc.ini"	"%ToPath%EXE\KizuPc.ini"		/Y

rem # �X�N���v�g�� �t�уt�@�C��
rem copy "%FromPath%EXE\startup.dcd"			"%ToPath%Logic\startup.dcd"								/Y
rem copy "%FromPath%EXE\startup_pd.dcd"			"%ToPath%Logic\startup_pd.dcd"								/Y
rem copy "%FromPath%EXE\startup.dcd"			"%ToPath%EXE\startup.dcd"								/Y
rem copy "%FromPath%EXE\startup_pd.dcd"			"%ToPath%EXE\startup_pd.dcd"								/Y


rem # �V���[�g�J�b�g���X
copy "%FromPath%���\�z_%BaseName%\�V���[�g�J�b�g\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\KS_STARTx64.exe - �V���[�g�J�b�g.lnk"		/Y
copy "%FromPath%�摜�����o�b�`\deloldlog_%BaseName%.bat"					"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\deloldlog_%BaseName%.bat"		/Y
rem copy "%FromPath%�摜�����o�b�`\���W�X�g���o�^_����o�^_%BaseName%.bat"		"\\%PCNAME%\c$\Users\Administrator\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\���W�X�g���o�^_����o�^_%BaseName%.bat"		/Y



rem # iPortViewer�֌W
mkdir %ToPath%iPortViewer
xcopy %FromPath%iPortViewer	 	%ToPath%iPortViewer		/E /Y /R
copy "%FromPath%iPortViewer\iPortViewer.exe - �V���[�g�J�b�g.lnk"	"\\%PCNAME%\c$\Users\Administrator\Desktop\iPortViewer.exe - �V���[�g�J�b�g.lnk"
								  

echo # 
echo # �R�s�[������Ɋ������܂����B
echo # 
pause