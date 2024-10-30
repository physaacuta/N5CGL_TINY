'*********************************************************************************
'SQL�T�[�o�[�Ɛڑ����s����{�N���X
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
    ''' SQL�T�[�o�[�Ɛڑ����s����{�N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_SQLServer
        Implements IDisposable                          ' �f�X�g���N�^��`

        '********************************************************************
        '�����o�[ ��`
        '********************************************************************
        Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�
        Protected m_bValid As Boolean = False
        Protected m_Connect As SqlClient.SqlConnection          '�f�[�^�x�[�X�ɐڑ����s���C���X�^���X
        Protected m_Command As SqlClient.SqlCommand             '�R�}���h
        Protected m_SQLTimeOut As Integer                       'SQL���s���̃^�C���A�E�g [�b]
        Protected m_Transact As SqlClient.SqlTransaction        '�g�����U�N�V��������

        Private Const mc_BinalyFileOneReadSize As Integer = 2 * 1024 * 1024 ' �o�C�i���t�@�C��1��̍ő�ǂݍ��݃T�C�Y

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
        '�X�^�e�B�b�N
        '********************************************************************
        ''' <summary>
        ''' DB�Ɛڑ��ł��邩�m�F����
        ''' </summary>
        ''' <param name="iniFPath">ini�t�@�C���t���p�X</param>
        ''' <param name="iniSession">�Z�b�V����</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function OpenCheck(ByVal iniFPath As String, ByVal iniSession As String) As Boolean
            Dim db As tClass_SQLServer = New tClass_SQLServer(iniFPath, iniSession)

            Try
                '' �ڑ����Ă݂�
                If db.Open() Then
                    db.Close()
                    Return True
                End If

            Catch ex As Exception
            Finally
                db.Dispose()
            End Try
            Return False
        End Function

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="strDataSource">�f�[�^�x�[�X�ڑ��A�h���X</param>
        ''' <param name="strDatabase">�f�[�^�x�[�X����</param>
        ''' <param name="strUser">���O�C�����[�U�[</param>
        ''' <param name="strPass">���O�C���p�X���[�h</param>
        ''' <param name="nConnectTimeOut">�ȸ�����ѱ��(��̫��5�b)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal strDataSource As String, ByVal strDatabase As String, ByVal strUser As String, ByVal strPass As String, Optional ByVal nConnectTimeOut As Integer = 5)
            '''' �ڑ��ݒ�
            Dim strLogin As String
            strLogin = "Persist Security Info=True;" & _
              "Integrated Security=False;" & _
              "server=mySQLServer;" & _
              "Connection Timeout=" & nConnectTimeOut & ";" & _
              "Data Source=" & strDataSource & ";" & _
              "database=" & strDatabase & ";" & _
              "User ID=" & strUser & ";" & _
              "Password=" & strPass & ";"

            Connection(strLogin)

            If String.IsNullOrEmpty(strDatabase) Or String.IsNullOrEmpty(strDataSource) Then
                m_bValid = False
            End If
        End Sub
        ''' <param name="iniFPath">ini�t�@�C���t���p�X</param>
        ''' <param name="iniSession">�Z�b�V����</param>
        ''' <param name="nConnectTimeOut">�ȸ�����ѱ��(��̫��5�b)</param>
        Public Sub New(ByVal iniFPath As String, ByVal iniSession As String, Optional ByVal nConnectTimeOut As Integer = 5)
            Me.new( _
              ReadIniStr(iniSession, "DBADDLESS", iniFPath), _
              ReadIniStr(iniSession, "DSNAME", iniFPath), _
              ReadIniStr(iniSession, "USERID", iniFPath), _
              ReadIniStr(iniSession, "PASSWORD", iniFPath), _
              nConnectTimeOut _
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
            m_Connect = New SqlClient.SqlConnection(strLogin)
            m_Command = New SqlClient.SqlCommand
            m_Command.Connection = m_Connect
            m_bValid = True
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                Call Close()
                m_Command = Nothing
                m_Connect = Nothing
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region


        ''' <summary>
        ''' DB�Ɛڑ����J�n����
        ''' </summary>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Function Open() As Boolean
            If m_bValid = False Then
                Return False
            End If

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
                ''�g�����U�N�V�������� �J��
                If Not m_Transact Is Nothing Then
                    m_Transact.Dispose()
                End If
                '' �ؒf
                m_Connect.Close()
                Return True
            Catch ex As Exception
                Return False
            End Try
        End Function

        ''' <summary>
        ''' DB�̐ڑ��𒆒f����
        ''' </summary>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Function Cancel() As Boolean

            Try

                '' �ڑ���Ԋm�F
                If m_Connect.State <> ConnectionState.Open Then Return True

                '�L�����Z��
                m_Command.Cancel()
                Return True
            Catch ex As Exception
                Return False
            End Try

        End Function
        ''' <summary>
        ''' �R�}���h���s�^�C���A�E�g����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub SetCommandTimeOut(ByVal nInterval As Integer)
            m_Command.CommandTimeout = nInterval
        End Sub

        ''' <summary>
        ''' SELECT���𔭍s����
        ''' </summary>
        ''' <param name="strSQL">���s����SQL��</param>
        ''' <returns>��ڑ�</returns>
        ''' <remarks>��O����Nothing��������</remarks>
        Public Function SelectAdapter(ByVal strSQL As String) As SqlClient.SqlDataAdapter
            '' ���s����SQL���Z�b�g
            m_Command.CommandText = strSQL

            Try
                '' SQL�����s
                Return New SqlClient.SqlDataAdapter(m_Command)
            Catch ex As Exception
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' SELECT���𔭍s����B��O����т���B
        ''' </summary>
        ''' <param name="strSQL">���s����SQL��</param>
        ''' <returns>�e�[�u���A�_�v�^</returns>
        Public Function SelectAdapterEx(ByVal strSQL As String) As SqlClient.SqlDataAdapter
            '' ���s����SQL���Z�b�g
            m_Command.CommandText = strSQL
            '' SQL�����s
            Return New SqlClient.SqlDataAdapter(m_Command)
        End Function

        ''' <summary>
        ''' SELECT���𔭍s����
        ''' </summary>
        ''' <param name="strSQL">���s����SQL��</param>
        ''' <returns>�f�[�^ �\�[�X����f�[�^�s�̑O�����X�g���[����ǂ݃N���X</returns>
        ''' <remarks></remarks>
        Public Function SelectExecute(ByVal strSQL As String, Optional ByVal CommandBehavior As Data.CommandBehavior = Data.CommandBehavior.Default) As SqlClient.SqlDataReader
            '' ���s����SQL���Z�b�g
            m_Command.CommandText = strSQL

            Try
                '' SQL�����s
                Return m_Command.ExecuteReader(CommandBehavior)
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
        Public Overridable Function SelectExecute(ByVal strSQL As String, ByVal para() As SqlClient.SqlParameter) As SqlClient.SqlDataReader
            Try
                '' �p�����[�^�ǉ�
                m_Command.Parameters.Clear()

                For Each p As SqlClient.SqlParameter In para
                    m_Command.Parameters.Add(p)
                Next p

                '' SQL�����s
                Return SelectExecute(strSQL)
            Catch ex As Exception
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' SELECT���𔭍s����B�ُ펞�͗�O����
        ''' </summary>
        ''' <param name="strSQL">���s����SQL��</param>
        ''' <returns>�f�[�^ �\�[�X����f�[�^�s�̑O�����X�g���[����ǂ݃N���X</returns>
        ''' <remarks></remarks>
        Public Function SelectExecuteEx(ByVal strSQL As String, Optional ByVal CommandBehavior As Data.CommandBehavior = Data.CommandBehavior.Default) As SqlClient.SqlDataReader
            '' ���s����SQL���Z�b�g
            m_Command.CommandText = strSQL

            '' SQL�����s
            Return m_Command.ExecuteReader(CommandBehavior)
        End Function


        ''' <summary>
        ''' SELECT���𔭍s���� (�o�C���h�Ή�)�@�ُ펞�͗�O����
        ''' </summary>
        ''' <param name="strSQL">���s����SQL�� (@*** �̃p�����[�^�t����SQL��)</param>
        ''' <param name="para">�p�����[�^</param>
        ''' <returns>�e�����󂯂��s����Ԃ�   -1:DML�����s</returns>
        ''' <remarks>"SELECT CustomerID, CompanyName FROM Customers WHERE Country = @Country AND City = @City"</remarks>
        Public Overridable Function SelectExecuteEx(ByVal strSQL As String, ByVal para() As SqlClient.SqlParameter) As SqlClient.SqlDataReader
            '' �p�����[�^�ǉ�
            m_Command.Parameters.Clear()

            m_Command.CommandText = strSQL
            Dim p As SqlClient.SqlParameter
            For Each p In para
                m_Command.Parameters.Add(p)
            Next p

            '' SQL�����s
            Return m_Command.ExecuteReader(Data.CommandBehavior.Default)
        End Function


        ''' <summary>
        ''' ����ȃo�C�i���f�[�^�̏����Ǎ������s���A���̂܂܎w��t�@�C���ɏ������� (�o�C�i���f�[�^�݂̂̎擾)
        ''' </summary>
        ''' <param name="bCancelFlg">���f�p�t���O (True�Œ��f�����) (�ʃX���b�h����True�ɂ���)</param>
        ''' <param name="strSQL">1�s,1���ڂ̂݃o�C�i���f�[�^��Ԃ�SQL���Ɍ��肷�鎖</param>
        ''' <param name="strFPath">�o�C�i���f�[�^���i�[����t���p�X</param>
        ''' <param name="nBaynarySize">�o�C�i���f�[�^�̃T�C�Y</param>
        ''' <returns>-99:���炩�̎��s -98:SQL���s���s  -4:�擾�ُ� -3:NULL�f�[�^ -2:0�� -1:�r�����f 0:����</returns>
        ''' <remarks></remarks>
        Public Overridable Function SelectBigBaynaryExecute(ByVal strSQL As String, ByRef bCancelFlg As Boolean, ByVal strFPath As String, ByVal nBaynarySize As Integer, Optional ByVal nReadSize As Integer = mc_BinalyFileOneReadSize) As Integer
            '' ���O����

            Dim readByte(nReadSize - 1) As Byte         ' �o�C�i���f�[�^�ǂݍ��ݗ̈�
            Dim nReadIndex As Long                      ' �ǂݍ��݈ʒu

            '' ���s����SQL���Z�b�g
            m_Command.CommandText = strSQL
            Try
                '' SQL�����s (�����ǂݏo���\�I�v�V�����w��) �� �ʏ�̏ꍇ�́A���s�����^�C�~���O�őS�f�[�^������Ă��邩��A����ȃf�[�^�ɂ͌����Ȃ�
                Using reader As SqlClient.SqlDataReader = m_Command.ExecuteReader(CommandBehavior.SequentialAccess)
                    If reader Is Nothing Then Return -98

                    '' 1�s�݂̂̓ǂݍ���
                    If Not reader.Read() Then Return -2       ' �s����
                    If reader.IsDBNull(0) Then Return -3      ' NULL�f�[�^�H

                    ' �ǂݍ��񂾃o�C�i���f�[�^���t�@�C���o�͂���ׂ̃X�g���[���N���X
                    Using stream As New IO.FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.Write)         ' �t�@�C���X�g���[��
                        '' ������ƂÂf�[�^��ǂݍ���
                        nReadIndex = 0
                        While (True)
                            ' �e�[�u������o�C�i���f�[�^�̕����ǂݏo��
                            Dim nOneReadByte As Long = reader.GetBytes(0, nReadIndex, readByte, 0, nReadSize)
                            If nOneReadByte <= 0 Then
                                ' �f�[�^����
                                Return -4
                            End If

                            ' �t�@�C���ɏ�������
                            stream.Write(readByte, 0, CInt(nOneReadByte))

                            ' �S�� �ǂݍ��񂾁H
                            nReadIndex += nOneReadByte
                            If nBaynarySize <= nReadIndex Then
                                Exit While
                            End If

                            ' �L�����Z���`�F�b�N
                            If bCancelFlg Then
                                Return -1
                            End If
                        End While
                    End Using
                End Using
                Return 0
            Catch ex As Exception
                Return -99
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
                    '''' 20140514  �o�C�g�z�񎞂� CStr(m_Command.Parameters.Item(ii).Value) ��Throw�����B�΍�Ƃ��āA�R�����g���B���Ƃ���syslog�͎~�߂Ă����̂�
                    'Dim strVal As String = ""
                    'For ii As Integer = 0 To m_Command.Parameters.Count - 1
                    '    strVal &= m_Command.Parameters.Item(ii).ToString & "=" & CStr(m_Command.Parameters.Item(ii).Value) & ", "
                    'Next
                    ''tClass_SysLog.Syslog("�� [tClass_SQLServer::DMLExecute] (param)" & strVal)
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
        Public Overridable Function DMLExecute(ByVal strSQL As String, ByVal para() As SqlClient.SqlParameter) As Integer
            Try
                '' �p�����[�^�ǉ�
                m_Command.Parameters.Clear()
                Dim p As SqlClient.SqlParameter
                For Each p In para
                    m_Command.Parameters.Add(p)
                Next p

                Return DMLExecute(strSQL)
            Catch ex As Exception
                Return Nothing
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

        '********************************************************************
        'SQL������ �⏕
        '********************************************************************
        '��肩��
        'Public Shared Sub sqlColumnInsert(ByVal strSql As String, ByVal val As String, ByVal size As Integer)
        '    If val.Length = 0 Then
        '        strSql &= "NULL,"
        '    Else
        '        strSql &= "'" & val.Substring(0, size) & "'"
        '    End If
        'End Sub
        'Public Shared Sub sqlColumnInsert(ByVal strSql As String, ByVal val As Integer)
        '    strSql &= val.ToString
        'End Sub

        'Public Shared Sub sqlColumnUpdate(ByVal strSql As String, ByVal name As String, ByVal val As String, ByVal size As Integer)
        '    If val.Length = 0 Then
        '        strSql &= name & "=NULL,"
        '    Else
        '        strSql &= name & "='" & val.Substring(0, size) & "'"
        '    End If
        'End Sub

        ''' <summary>
        ''' �C���T�[�g�̕⏕
        ''' </summary>
        Public Shared Function sqlAddInsert(ByVal val As Integer) As String
            Return String.Format("{0},", val)
        End Function
        Public Shared Function sqlAddInsert(ByVal val As Double) As String
            Return String.Format("{0:0.000000},", val)
        End Function
        Public Shared Function sqlAddInsert(ByVal val As String) As String
            Return String.Format("'{0}',", Val)
        End Function
        Public Shared Function sqlAddInsert_Text(ByVal val As String) As String
            Return String.Format("{0},", val)
        End Function
        ''' <summary>
        ''' �A�b�v�f�[�g�̕⏕
        ''' </summary>
        Public Shared Function sqlAddUpdate(ByVal name As String, ByVal val As Integer) As String
            Return String.Format("{0}={1},", name, val)
        End Function
        Public Shared Function sqlAddUpdate(ByVal name As String, ByVal val As String) As String
            Return String.Format("{0}='{1}',", name, val)
        End Function
        Public Shared Function sqlAddUpdate_Text(ByVal name As String, ByVal val As String) As String
            Return String.Format("{0}={1},", name, val)
        End Function
        ''' <summary>
        ''' �����ǉ��̕⏕
        ''' </summary>
        Public Shared Function sqlColumnAddTime() As String
            Return String.Format("'{0:yyyy/MM/dd HH:mm:ss}'", Now)
        End Function



        ''' <summary>
        ''' UpDate�̗�R�s�[���̃T�|�[�g
        ''' </summary>
        ''' <param name="name">��</param>
        ''' <param name="AsName">�R�s�[���̗񖼂�As</param>
        ''' <returns>����������</returns>
        ''' <remarks></remarks>
        Public Shared Function sqlColumnUpdate(ByVal name As String, ByVal AsName As String) As String
            Return String.Format("[{0}]={1}.[{0}],", name, AsName)
        End Function

        ''' <summary>
        ''' �Ō�� "," ����菜��
        ''' </summary>
        ''' <param name="strSQL">SQL��</param>
        ''' <remarks></remarks>
        Public Shared Sub sqlColumnTrim(ByRef strSQL As String)
            If strSQL.EndsWith(",") Then
                strSQL = strSQL.Remove(strSQL.Length - 1, 1)
            End If
        End Sub




        '********************************************************************
        '�ȈՎ��s
        '********************************************************************
        ''' <summary>
        ''' �P��s��Select���̎��s
        ''' </summary>
        ''' <param name="strSQL"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overridable Function GetSelectKey(ByVal strSQL As String, ByRef Ans As Integer) As Boolean
            Ans = 0
            Dim sqlRead As SqlClient.SqlDataReader = SelectExecute(strSQL)
            If sqlRead Is Nothing Then Return False
            Try
                sqlRead.Read()
                Ans = sqlRead.GetInt32(0)
                Return True
            Catch ex As Exception
                Return False
            Finally
                sqlRead.Close()
            End Try
        End Function
        Public Overridable Function GetSelectKey(ByVal strSQL As String, ByRef Ans As String) As Boolean
            Ans = ""
            Dim sqlRead As SqlClient.SqlDataReader = SelectExecute(strSQL)
            If sqlRead Is Nothing Then Return False
            Try
                sqlRead.Read()
                Ans = sqlRead.GetString(0)
                Return True
            Catch ex As Exception
                Return False
            Finally
                sqlRead.Close()
            End Try
        End Function
        Public Overridable Function GetSelectKey(ByVal strSQL As String) As Object
            Dim sqlRead As SqlClient.SqlDataReader = SelectExecute(strSQL)
            If sqlRead Is Nothing Then Return False
            Try
                sqlRead.Read()
                Return sqlRead.GetValue(0)
            Catch ex As Exception
                Return Nothing
            Finally
                sqlRead.Close()
            End Try
        End Function


        '********************************************************************
        ' NULL�`�F�b�N���݃��[�_�[�ǂݍ���
        '********************************************************************

        Public Overloads Shared Function GetDbValT(Of T)(ByVal sqlRead As System.Data.Common.DbDataReader, ByVal nIndex As Integer, ByVal DefVal As T) As T
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return CType(sqlRead.GetValue(nIndex), T)
        End Function

        Public Overloads Shared Function GetDbValT(Of T)(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As T) As T
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbValT(sqlRead, nIndex, DefVal)
        End Function


        ''' <summary>
        ''' ���[�_�[����NULL���`�F�b�N���Ēl��ǂݍ���Ō��ʂ�Ԃ�
        ''' </summary>
        ''' <param name="sqlRead">���[�_�[</param>
        ''' <param name="nIndex">��No(0�I���W��)</param>
        ''' <param name="DefVal">�f�t�H���g�l</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As String) As String
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetString(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Integer) As Integer
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetInt32(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Short) As Short
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetInt16(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Double) As Double
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetDouble(nIndex)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal nIndex As Integer, ByVal DefVal As Single) As Single
            If 0 > nIndex Then Return DefVal
            If sqlRead.IsDBNull(nIndex) Then Return DefVal
            Return sqlRead.GetFloat(nIndex)
        End Function
        Public Overloads Shared Function GetDbNameToIndex(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String) As Integer
            Try
                Return sqlRead.GetOrdinal(sName)
            Catch ex As Exception
                Return -1
            End Try
        End Function
        ''' <summary>
        ''' ���[�_�[����NULL���`�F�b�N���Ēl��ǂݍ���Ō��ʂ�Ԃ�
        ''' </summary>
        ''' <param name="sqlRead">���[�_�[</param>
        ''' <param name="sName">��</param>
        ''' <param name="DefVal">�f�t�H���g�l</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As String) As String
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Integer) As Integer
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Short) As Short
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Double) As Double
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function
        Public Overloads Shared Function GetDbVal(ByVal sqlRead As Data.SqlClient.SqlDataReader, ByVal sName As String, ByVal DefVal As Single) As Single
            Dim nIndex As Integer = GetDbNameToIndex(sqlRead, sName)
            Return GetDbVal(sqlRead, nIndex, DefVal)
        End Function


    End Class
End Namespace