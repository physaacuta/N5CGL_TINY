'*********************************************************************************
'ORACLE�Ɛڑ����s����{�N���X
'	[Ver]
'		Ver.01    2006/01/28  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Data
Imports tClassLibrary.tMod.tMod_IniFile			' ini�t�@�C���̓ǂݍ���

#Const tDEBUG = False

Namespace tClass
    ''' <summary>
    ''' ORACLE�Ɛڑ����s����{�N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_Oracle
        '********************************************************************
        '�����o�[ ��`
        '********************************************************************
        Protected m_Connect As OleDb.OleDbConnection			'�f�[�^�x�[�X�ɐڑ����s���C���X�^���X
        Protected m_Command As OleDb.OleDbCommand				'�R�}���h
        Protected m_SQLTimeOut As Integer						'SQL���s���̃^�C���A�E�g [�b]
        Protected m_Transact As OleDb.OleDbTransaction			'�g�����U�N�V��������

        '********************************************************************
        '�v���p�e�B�[��`
        '********************************************************************
        ''' <summary>
        ''' �^�C���A�E�g�ݒ� (�������ݐ�p)
        ''' </summary>
        ''' <value>�^�C���A�E�g�b</value>
        ''' <remarks></remarks>
        Public WriteOnly Property SQLTimeOut() As Integer
            Set(ByVal Value As Integer)
                If m_Command Is Nothing Then Exit Property
                '' SQL���s���̃^�C���A�E�g�b�Z�b�g
                m_Command.CommandTimeout = Value
            End Set
        End Property

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="strDataSource">�f�[�^�x�[�X�ڑ��A�h���X</param>
        ''' <param name="strUser">���O�C�����[�U�[</param>
        ''' <param name="strPass">���O�C���p�X���[�h</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal strDataSource As String, ByVal strUser As String, ByVal strPass As String)
            '' �ڑ��ݒ�
            Dim strLogin As String
            strLogin = "Persist Security Info=True;" & _
             "server=oralce;" & _
             "Connection Timeout=60;" & _
             "Provider=MSDAORA;" & _
             "Data Source=" & strDataSource & ";" & _
             "User ID=" & strUser & ";" & _
             "Password=" & strPass & ";"

            Connection(strLogin)
        End Sub

        ''' <param name="iniFPath">ini�t�@�C���t���p�X</param>
        ''' <param name="iniSession">�Z�b�V����</param>
        Public Sub New(ByVal iniFPath As String, ByVal iniSession As String)
            Me.new( _
              ReadIniStr(iniSession, "DBADDLESS", iniFPath), _
              ReadIniStr(iniSession, "USERID", iniFPath), _
              ReadIniStr(iniSession, "PASSWORD", iniFPath) _
            )
        End Sub
        ''' <param name="strLogin">�ڑ�������w��</param>
        Public Sub New(ByVal strLogin As String)
            Connection(strLogin)
        End Sub
        ''' <summary>
        ''' �ڑ�����
        ''' </summary>
        ''' <param name="strLogin"></param>
        ''' <remarks></remarks>
        Private Sub Connection(ByVal strLogin As String)
            '' �e�����o�ϐ��̃C���X�^���X����
            m_Connect = New OleDb.OleDbConnection(strLogin)
            m_Command = New OleDb.OleDbCommand
            m_Command.Connection = m_Connect
        End Sub

        ''' <summary>
        ''' DB�Ɛڑ����J�n����
        ''' </summary>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Function Open() As Boolean
            '' �ڑ���Ԋm�F
            If m_Connect.State <> ConnectionState.Closed Then Return True

            Try
                '' DB�Ɛڑ����s��
                m_Connect.Open()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' DB�Ɛڑ����I������
        ''' </summary>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Function Close() As Boolean
            Try
                '' �ڑ���Ԋm�F
                If m_Connect.State <> ConnectionState.Open Then Return True

                '' �ؒf
                m_Connect.Close()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' �p�����[�^���N���A�[����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ParamClear()
            '' ���s��p�����[�^�폜
            m_Command.Parameters.Clear()
        End Sub

        ''' <summary>
        ''' SELECT���𔭍s����
        ''' </summary>
        ''' <param name="strSQL">���s����SQL��</param>
        ''' <returns>�f�[�^ �\�[�X����f�[�^�s�̑O�����X�g���[����ǂ݃N���X</returns>
        ''' <remarks></remarks>
        Public Function SelectExecute(ByVal strSQL As String) As OleDb.OleDbDataReader
            '' ���s����SQL���Z�b�g
            m_Command.CommandText = strSQL

            Try
                '' SQL�����s
                Return m_Command.ExecuteReader()
            Catch ex As Exception
                If m_Command.Parameters.Count <> 0 Then
                    Dim strVal As String = ""
                    For ii As Integer = 0 To m_Command.Parameters.Count - 1
                        strVal &= m_Command.Parameters.Item(ii).ToString & "=" & CStr(m_Command.Parameters.Item(ii).Value) & ", "
                    Next
                End If
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' SELECT���𔭍s���� (�o�C���h�Ή�)
        ''' </summary>
        ''' <param name="strSQL">���s����SQL�� (@*** �̃p�����[�^�t����SQL��)</param>
        ''' <param name="para">�p�����[�^</param>
        ''' <returns>�e�����󂯂��s����Ԃ�   -1:DML�����s</returns>
        ''' <remarks>"SELECT CustomerID, CompanyName FROM Customers WHERE Country = @Country AND City = @City"</remarks>
        Public Overridable Function SelectExecute(ByVal strSQL As String, ByVal para() As OleDb.OleDbParameter) As OleDb.OleDbDataReader
            Try
                '' �p�����[�^�ǉ�
                Dim p As OleDb.OleDbParameter
                For Each p In para
                    m_Command.Parameters.Add(p)
                Next p

                '' SQL�����s
                Return SelectExecute(strSQL)
            Catch ex As Exception
                m_Command.Parameters.Clear()
                Return Nothing
            Finally
            End Try
        End Function

        ''' <summary>
        ''' DML�������s����
        ''' </summary>
        ''' <param name="strSQL">���s����SQL��</param>
        ''' <returns>�e�����󂯂��s����Ԃ�   -1:DML�����s</returns>
        ''' <remarks></remarks>
        Public Function DMLExecute(ByVal strSQL As String) As Integer
            '' ���s����SQL���Z�b�g
            m_Command.CommandText = strSQL

            Try
                '' DML�����s
                Return m_Command.ExecuteNonQuery()
            Catch ex As Exception
                If m_Command.Parameters.Count <> 0 Then
                    Dim strVal As String = ""
                    For ii As Integer = 0 To m_Command.Parameters.Count - 1
                        strVal &= m_Command.Parameters.Item(ii).ToString & "=" & CStr(m_Command.Parameters.Item(ii).Value) & ", "
                    Next
                End If
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' DML�������s���� (�o�C���h�Ή�)
        ''' </summary>
        ''' <param name="strSQL">���s����SQL�� (@*** �̃p�����[�^�t����SQL��)</param>
        ''' <param name="para">�p�����[�^</param>
        ''' <returns>�e�����󂯂��s����Ԃ�   -1:DML�����s</returns>
        ''' <remarks>"SELECT CustomerID, CompanyName FROM Customers WHERE Country = @Country AND City = @City"</remarks>
        Public Overridable Function DMLExecute(ByVal strSQL As String, ByVal para() As OleDb.OleDbParameter) As Integer
            Try
                '' �p�����[�^�ǉ�
                Dim p As OleDb.OleDbParameter
                For Each p In para
                    m_Command.Parameters.Add(p)
                Next p

                Return DMLExecute(strSQL)
            Catch ex As Exception
                Return Nothing
            Finally
            End Try
        End Function

        ''' <summary>
        ''' Transaction ���J�n���鏈�� Commit,Rollback���s���ꍇ�͎��s�����܂��傤
        ''' </summary>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Function TransactStart() As Boolean
            Try
                '' �g�����U�N�V�����������J�n
                m_Transact = m_Connect.BeginTransaction

                m_Command.Transaction = m_Transact
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' Commit ���s��
        ''' </summary>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Function TransactCommint() As Boolean
            Try
                '' �R�~�b�g���s��
                m_Transact.Commit()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' Rollback���s���ꍇ�͎��s�����܂��傤
        ''' </summary>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Function TransactRollback() As Boolean
            Try
                '' ���[���o�b�N���s��
                m_Transact.Rollback()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function
    End Class
End Namespace