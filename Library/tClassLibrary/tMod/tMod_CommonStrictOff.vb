'*********************************************************************************
'���ʏ����̃��W���[���Q
'	[Ver]
'		Ver.01    2005/12/25  vs2005 �Ή�
'
'	[����]
'		Option Strict Off ��p
'*********************************************************************************
Option Strict Off								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Runtime.InteropServices			' DllImport


Namespace tMod
    ''' <summary>
    ''' ���ʏ����̃��W���[���Q
    ''' </summary>
    ''' <remarks></remarks>
    Public Module tMod_CommonStrictOff
        ''' <summary>
        ''' �V���[�g�J�b�g�쐬
        ''' </summary>
        ''' <param name="strCreateLinkName">�V���[�g�J�b�g�̐����t�@�C���� (�t���p�X) (�g���q��.lnk)</param>
        ''' <param name="strTagetPath">�V���[�g�J�b�g��̃p�X</param>
        ''' <param name="strTagetExe">�V���[�g�J�b�g���EXE�t�@�C����</param>
        ''' <param name="strcomment">�V���[�g�J�b�g�̃R�����g</param>
        ''' <remarks></remarks>
        Public Function CreateShortCut(ByVal strCreateLinkName As String, ByVal strTagetPath As String, ByVal strTagetExe As String, Optional ByVal strcomment As String = "") As Boolean
            'Dim objShell As Object = CreateObject("WScript.Shell")
            'Dim objLink As Object = objShell.CreateShortcut("C:\test.lnk") '�V���[�g�J�b�g�̃t�@�C����

            'With objLink
            '	.targetpath = "c:\install\KS_INSTAL.exe"
            '	.description = "testtest"
            '	.iconlocation = "c:\install\KS_INSTAL.exe"
            '	.workingdirectory = "c:\install"
            '	.save()
            'End With

            Try
                Dim objShell As Object = CreateObject("WScript.Shell")
                Dim objLink As Object = objShell.CreateShortcut(strCreateLinkName) '�V���[�g�J�b�g�̃t�@�C����

                With objLink
                    .targetpath = IO.Path.Combine(strTagetPath, strTagetExe)
                    .iconlocation = IO.Path.Combine(strTagetPath, strTagetExe)
                    .description = strcomment
                    .workingdirectory = strTagetPath
                    .save()
                End With
                Return True

            Catch ex As Exception
                Return False
            End Try
        End Function
    End Module
End Namespace