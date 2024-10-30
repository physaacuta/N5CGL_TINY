'*********************************************************************************
'�o�C�i���t�@�C���ǂݏ����֌W�̏������s����{�N���X (��{�I��VB6�Ɠ����悤�ɍ���Ă���)
'	[Ver]
'		Ver.01    2003/06/18  vs2002 �Ή�
'		Ver.02    2007/03/06  vs2005 �Ή�
'
'	[����]
'		���Ȃ菉���ɍ��������A����܂�M�p���Ȃ��ŉ������B
'
'		�\���̂��g�p����ꍇ�́A�ȉ��̃T���v���̗l�ɍ\���̂��w�肷��
'
    ' '' �\���̃T���v��
    '<StructLayout(LayoutKind.Sequential, Size:=1200, pack:=1)> _
    'Structure SAMPLE
    '	<VBFixedArray(99), MarshalAs(UnmanagedType.ByValArray, SizeConst:=100)> _
    '	Dim dataL() As Integer

    '	<VBFixedString(400), MarshalAs(UnmanagedType.ByValTStr, SizeConst:=400)> _
    '	Dim dataS As String

    '	<VBFixedArray(399), MarshalAs(UnmanagedType.ByValArray, SizeConst:=400)> _
    '	Dim dataB() As Byte

    '	Public Sub Inisialize()
    '		ReDim dataL(99)
    '		ReDim dataB(399)
    '	End Sub
    'End Structure
'*********************************************************************************
Option Strict On				'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Namespace tClass
    Public Class tClass_FileBinary

        Private m_nFreeNo As Integer				' �t���[�t�@�C���m��

        ''' <summary>
        ''' �t���[�t�@�C���m�����擾����
        ''' </summary>
        ''' <returns>0�ȏ�:�t���[�t�@�C���m��   -1:�ُ�</returns>
        ''' <remarks></remarks>
        Public Shared Function intFreeFileNo() As Integer
            '�t���[�t�@�C���m�����擾����
            Try
                Return FreeFile()
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' �o�C�i���t�@�C�����J��
        ''' </summary>
        ''' <param name="strFullPath">�t���p�X</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileOpen(ByVal strFullPath As String) As Integer

            '�����_���A�N�Z�X���[�h�Ńt�@�C�����J��
            Try
                m_nFreeNo = intFreeFileNo()
                If m_nFreeNo = -1 Then Return -9

                FileOpen(m_nFreeNo, strFullPath, OpenMode.Binary, OpenAccess.ReadWrite, OpenShare.Shared)
                Return 0
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' �t�@�C�������
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileClose() As Integer
            '�t�@�C�������
            Try
                FileClose(m_nFreeNo)
                Return 0
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' �t�@�C���f�[�^��Ǎ���
        ''' </summary>
        ''' <param name="objData">�l�^�̊�{�N���X</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileRead(ByRef objData As System.ValueType) As Integer
            '�����_���A�N�Z�X���[�h�Ńt�@�C����Ǎ���
            Try
                FileGet(m_nFreeNo, objData)
                Return 0
            Catch
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' �����_���A�N�Z�X���[�h�Ńt�@�C���f�[�^��������
        ''' </summary>
        ''' <param name="objData">�l�^�̊�{�N���X</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Function intFileWrite(ByVal objData As System.ValueType) As Integer
            '�����_���A�N�Z�X���[�h�Ńt�@�C����������
            Try
                FilePut(m_nFreeNo, objData)
                Return 0
            Catch
                Return -1
            End Try
        End Function

    End Class
End Namespace