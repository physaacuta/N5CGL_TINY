'*********************************************************************************
'�N���X�̏�Ԃ����̂܂܃o�C�i���t�@�C���ŕۑ�/�ǂݍ��݂��s�����N���X
'	[Ver]
'		Ver.01    2005/12/24  vs2005 �Ή�
'		Ver.02    2006/03/04  �o�b�t�@�Ƀo�C�i���Z�b�g
'
'	[����]
'		�V���A�������s���N���X��<System.Serializable()>�����ɐݒ肷��K�v������
'*********************************************************************************
Option Strict On                'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Namespace tClass
    ''' <summary>
    ''' �N���X�̏�Ԃ����̂܂܃o�C�i���t�@�C���ŕۑ�/�ǂݍ��݂��s�����N���X
    ''' </summary>
    ''' <remarks>�V���A�������s���N���X��"System.Serializable()"�����ɐݒ肷��K�v������</remarks>
    Public Class tClass_BinarySerializ
        '********************************************************************
        '���\�b�h��`
        '********************************************************************

        ''' <summary>
        ''' �V���A��������
        ''' </summary>
        ''' <param name="objVal">�V���A��������C���X�^���X</param>
        ''' <param name="strFPath">�ۑ��t���p�X</param>
        ''' <returns>���A���</returns>
        ''' <remarks>�g���q�͊�{�I��.bin�ɂ��Ă�����</remarks>
        Public Overloads Shared Function Save(ByRef objVal As Object, ByVal strFPath As String) As Boolean
            Dim strFolder As String = IO.Path.GetDirectoryName(strFPath)
            Dim stream As IO.FileStream                                         '�X�g���[��

            '''' �w��t�H���_�`�F�b�N
            Call tMod.CheckFolder(strFolder)
            '''' �V���A����
            Try
                '' �X�g���[�� �I�[�v��
                stream = New IO.FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.Write)
            Catch ex As Exception
                Return False
            End Try
            Try
                '' �o�C�i���t�H�[�}�b�^ �쐬
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                '' �V���A����
                formatter.Serialize(stream, objVal)
                Return True
            Catch ex As Exception
                Return False
            Finally
                '' �N���[�Y
                stream.Close()
            End Try
        End Function

        ''' <param name="objVal">�V���A��������C���X�^���X</param>
        ''' <param name="strFolder">�ۑ��t�H���_�p�X</param>
        ''' <param name="strName">�ۑ��t�@�C����</param>
        ''' <returns>���A���</returns>
        Public Overloads Shared Function Save(ByRef objVal As Object, ByVal strFolder As String, ByVal strName As String) As Boolean
            Dim strFPath As String = IO.Path.Combine(strFolder, strName)        '�t���p�X�ɕϊ�
            Return Save(objVal, strFPath)
        End Function

        ''' <summary>
        ''' �t�V���A��������
        ''' </summary>
        ''' <param name="objVal">�t�V���A���������C���X�^���X���i�[����I�u�W�F�N�g</param>
        ''' <param name="strFPath">�ۑ��t���p�X</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function Load(ByRef objVal As Object, ByVal strFPath As String) As Boolean
            Dim stream As IO.FileStream                                         '�X�g���[��

            '''' �t�@�C���`�F�b�N
            objVal = Nothing
            If Not IO.File.Exists(strFPath) Then Return False

            '''' �t�V���A����
            Try
                '' �X�g���[�� �I�[�v��
                stream = New IO.FileStream(strFPath, IO.FileMode.Open, IO.FileAccess.Read)
            Catch ex As Exception
                Return False
            End Try
            Try
                '' �o�C�i���t�H�[�}�b�^ �쐬
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                objVal = formatter.Deserialize(stream)
                Return True
            Catch ex As Exception
                Return False
            Finally
                '' �N���[�Y
                stream.Close()
            End Try
        End Function

        ''' <param name="objVal">�t�V���A���������C���X�^���X���i�[����I�u�W�F�N�g</param>
        ''' <param name="strFolder">�ۑ��t�H���_�p�X</param>
        ''' <param name="strName">�ۑ��t�@�C����</param>
        ''' <returns>���A���</returns>
        Public Overloads Shared Function Load(ByRef objVal As Object, ByVal strFolder As String, ByVal strName As String) As Boolean
            Dim strFPath As String = IO.Path.Combine(strFolder, strName)        '�t���p�X�ɕϊ�
            Return Load(objVal, strFPath)
        End Function

        ''' <summary>
        ''' MemoryStream�ɃV���A�������������Z�b�g����
        ''' </summary>
        ''' <param name="objVal">�V���A����������</param>
        ''' <param name="memS">MemoryStream�I�u�W�F�N�g</param>
        ''' <returns>���A���</returns>
        ''' <remarks>MemoryStream�́A�Ăь��ŃC���X�^���X�𐶐����Ă�����</remarks>
        Public Overloads Shared Function SaveBuffer(ByRef objVal As Object, ByVal memS As IO.MemoryStream) As Boolean
            '' �X�g���[�������ς݁H
            If memS Is Nothing Then Return False

            Try
                '' �o�C�i���t�H�[�}�b�^ �쐬
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                '' �V���A����
                formatter.Serialize(memS, objVal)
                memS.Position = 0
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' MemoryStream�̏����t�V���A��������
        ''' </summary>
        ''' <param name="objVal">�t�V���A��������C���X�^���X</param>
        ''' <param name="memS">MemoryStream�I�u�W�F�N�g</param>
        ''' <returns>���A���</returns>
        ''' <remarks>�g�p��́AmemS���N���[�Y���鎖�BMemoryStream��Position�𐳊m�Ȉʒu�ɂ��Ă�����</remarks>
        Public Overloads Shared Function LoadBuffer(ByRef objVal As Object, ByVal memS As IO.MemoryStream) As Boolean
            Try
                '' �o�C�i���t�H�[�}�b�^ �쐬
                Dim formatter As New Runtime.Serialization.Formatters.Binary.BinaryFormatter(Nothing, New Runtime.Serialization.StreamingContext(Runtime.Serialization.StreamingContextStates.File))
                objVal = formatter.Deserialize(memS)
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function
    End Class
End Namespace
