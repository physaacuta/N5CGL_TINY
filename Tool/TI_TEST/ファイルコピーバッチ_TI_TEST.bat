@echo off

rem #===================================================
rem # �Œ�萔
rem #===================================================
rem # �x�[�X�t�H���_
set BaseName=N5CGL_TINY

rem # �R�s�[���̃x�[�X�p�X
set BASE_FROM=C:\%BaseName%\Tool\TI_TEST\x64\Release\TI_TEST.exe
rem # �R�s�[��̃x�[�X�p�X
set BASE_TO=c$\%BaseName%\ToolExe\


rem #===================================================
rem # ���͒l
rem #   �E�R�s�[��
rem #===================================================
rem # set CopyName=EXE\KS_START.EXE
rem set /p CopyName=�R�s�[���̃t�@�C������͂��Ă�������(%BASE_FROM%):





rem #===================================================
rem # �w��o�b�ɃR�s�[
rem #===================================================
@echo on

copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_1\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_2\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_3\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_4\."	/Y
copy "%BASE_FROM%" "\\192.9.203.31\%BASE_TO%TI_TEST_5\."	/Y

copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_1\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_2\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_3\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_4\."	/Y
copy "%BASE_FROM%" "\\192.9.203.32\%BASE_TO%TI_TEST_5\."	/Y

copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_1\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_2\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_3\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_4\."	/Y
copy "%BASE_FROM%" "\\127.0.0.1\%BASE_TO%TI_TEST_5\."	/Y


pause