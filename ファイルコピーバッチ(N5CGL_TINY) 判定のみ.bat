@echo off

rem #===================================================
rem # �Œ�萔
rem #===================================================
rem # �x�[�X�t�H���_
set BaseName=N5CGL_TINY

rem # �R�s�[���̃x�[�X�p�X
set BASE_FROM=C:\%BaseName%\
rem # �R�s�[��̃x�[�X�p�X
set BASE_TO=c$\%BaseName%\


rem #===================================================
rem # ���͒l
rem #   �E�R�s�[��
rem #===================================================
rem # set CopyName=EXE\KS_START.EXE
set /p CopyName=�R�s�[���̃t�@�C������͂��Ă�������(%BASE_FROM%):





rem #===================================================
rem # �w��o�b�ɃR�s�[
rem #===================================================
@echo on

copy "%BASE_FROM%%CopyName%" "\\HANTEI01\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI02\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI03\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI04\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI05\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI06\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI07\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI08\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI09\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI10\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI11\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI12\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI13\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI14\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI15\%BASE_TO%%CopyName%"	/Y
copy "%BASE_FROM%%CopyName%" "\\HANTEI16\%BASE_TO%%CopyName%"	/Y


pause