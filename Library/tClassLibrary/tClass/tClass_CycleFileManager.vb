'*********************************************************************************
'	�e�L�X�g�t�@�C���ɃV�[�P���V�����o�͂��s�����N���X
'	[Ver]
'		Ver.01	2009/02/26	vs2005 �Ή�
'
'	[����]
'		Log�t�@�C�������݂ɓ��������N���X
'		�t�@�C���̑�̗̂e�ʂŐؑ�
'		�����̏������݂̓_�� (���b�N��������)
'*********************************************************************************

Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.IO								' IO����


Namespace tClass
    Public Class tClass_CycleFileManager


        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************

        Private m_cNowFPath As String						' ���ݏo�͒��̃t�@�C���t���p�X
        Private m_Encoding As System.Text.Encoding			' �G���R�[�f�B���O


        '���O�t�@�C���֘A
        Private m_cBaseFolder As String						' �x�[�X�̃t�H���_
        Private m_cBaseFile As String						' �x�[�X�̃t�@�C����
        Private m_cType As String							' �g���q (.log)

        Private m_nMaxChangeSize As Integer					' �ő�t�@�C���ؑփT�C�Y [KByte]
        Private m_nMaxCycleCnt As Integer					' �ő�T�C�N���b�N�t�@�C������


        '********************************************************************
        ' ����
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="filename">�t�@�C���� (�ʏ�^�X�N��)  (Nothing�����O�o�͂��Ȃ�)</param>
        ''' <param name="folder">�t�H���_�p�X�w�� (�Ō��\�K�{) (�f�t�H���g�w��̎��́ANULL��OK)</param>	
        ''' <param name="sType">�g���q (.�͕K�{)</param>
        ''' <param name="ChangSize">�t�@�C���ؑփT�C�Y(�ڈ�) [KByte]</param>
        ''' <param name="cycleCnt">�J��Ԃ��t�@�C����</param>
        Public Sub New(ByVal filename As String, Optional ByVal folder As String = "", Optional ByVal sType As String = ".log", Optional ByVal ChangSize As Integer = 2048, Optional ByVal cycleCnt As Integer = 3)
            ''�t�H���_�L��
            If folder = "" Then folder = ".\LOG\"
            If Not IO.Directory.Exists(folder) Then
                IO.Directory.CreateDirectory(folder)
            End If
            m_cBaseFolder = folder
            m_cBaseFile = filename
            m_cType = sType
            m_nMaxChangeSize = ChangSize
            m_nMaxCycleCnt = cycleCnt

            '' ���̑����ڐ���
            m_cNowFPath = String.Format("{0}{1}_0{2}", m_cBaseFolder, m_cBaseFile, m_cType)
            m_Encoding = System.Text.Encoding.GetEncoding("shift-jis")

        End Sub


        ''' <summary>
        ''' �t�@�C����1�s��������
        ''' </summary>
        ''' <param name="sVal">1�s���̃f�[�^ (�I�[�̉��s�R�[�h�͕K�v)</param>
        Public Function Write(ByVal sVal As String) As Boolean
            ' �t�@�C���`�F�b�N
            If CheckFileSize() Then
                ' �t�@�C���̈ړ�
                ChangeFileCycle()
            End If

            ' ���ꂩ�J���Ă�������߂�
            Try
                ' �t�@�C���ɏ�������
                IO.File.AppendAllText(m_cNowFPath, sVal, m_Encoding)
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function


        ''' <summary>
        ''' �t�@�C���T�C�Y�`�F�b�N
        ''' </summary>
        ''' <returns>true:�؂�ւ�</returns>
        ''' <remarks></remarks>
        Private Function CheckFileSize() As Boolean
            Try
                Dim f As New IO.FileInfo(m_cNowFPath)
                If Not f.Exists Then Return False ' ���������t�@�C���������Ȃ�A�ؑ֕s�v

                Dim size As Long = f.Length
                If size / 1024 > m_nMaxChangeSize Then Return True ' �w��T�C�Y�����傫���̂Ńt�@�C���֑ؑΏ�

                Return False
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' �t�@�C���̈ړ�
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ChangeFileCycle()
            Dim sPath1 As String
            Dim sPath2 As String

            ' �ŌÂ�No������
            sPath1 = String.Format("{0}{1}_{2}{3}", m_cBaseFolder, m_cBaseFile, m_nMaxCycleCnt, m_cType)
            IO.File.Delete(sPath1)

            ' 8��9�ɕύX
            For ii As Integer = m_nMaxCycleCnt - 1 To 0 Step -1
                sPath1 = String.Format("{0}{1}_{2}{3}", m_cBaseFolder, m_cBaseFile, ii, m_cType)
                sPath2 = String.Format("{0}{1}_{2}{3}", m_cBaseFolder, m_cBaseFile, ii + 1, m_cType)

                ' ���t�@�C��������H
                If IO.File.Exists(sPath1) Then
                    ' ���t�@�C�����ړ�
                    IO.File.Move(sPath1, sPath2)
                End If
            Next ii

        End Sub

    End Class
End Namespace

