'*********************************************************************************
'�e�L�X�g�t�@�C���ǂݏ����֌W�̏������s����{�N���X
'	[Ver]
'		Ver.01    2003/09/15  vs2002 �Ή�
'		Ver.02    2007/03/06  vs2005 �Ή�
'
'	[����]
'		���Ȃ菉���ɍ��������A����܂�M�p���Ȃ��ŉ������B
'*********************************************************************************
Option Strict On				'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Namespace tClass
    Public Class tClass_FileStream
        Implements IDisposable							'�f�X�g���N�^��`

        '********************************************************************
        '�f���Q�[�g��`
        '********************************************************************
        Private Delegate Sub tDele_OpenDelegate()		'�I�[�v���f���Q�[�g
        Private m_OpenMethod As tDele_OpenDelegate		'�I�[�v�����̋N�����b�\�h

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private m_Writer As System.IO.StreamWriter		'�t�@�C�������݃N���X
        Private m_Reader As System.IO.StreamReader		'�t�@�C���Ǎ��݃N���X
        Private m_Stream As System.IO.FileStream		'�t�@�C���X�g���[���N���X

        Private m_FPath As String						'�t�@�C���t���p�X
        Private m_Mode As System.IO.FileMode			'�t�@�C���I�[�v�����[�h
        Private m_Access As System.IO.FileAccess		'�t�@�C���I�[�v�����̃A�N�Z�X��
        Private m_Share As System.IO.FileShare			'�t�@�C���I�[�v�����̑��v���Z�X����̋��L���

        Private m_Encoding As System.Text.Encoding

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="FPath">�t�@�C���t���p�X</param>
        ''' <param name="fMode">�t�@�C���I�[�v�����[�h</param>
        ''' <param name="fAccess">�t�@�C���I�[�v�����̃A�N�Z�X��</param>
        ''' <param name="fShare">�t�@�C���I�[�v�����̑��v���Z�X����̋��L���</param>
        ''' <param name="encoding">�G���R�[�h</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal FPath As String, _
                        Optional ByVal fMode As System.IO.FileMode = System.IO.FileMode.OpenOrCreate, _
                        Optional ByVal fAccess As System.IO.FileAccess = System.IO.FileAccess.Read, _
                        Optional ByVal fShare As System.IO.FileShare = System.IO.FileShare.Read, _
                        Optional ByVal encoding As System.text.Encoding = Nothing)

            '' �v���C�x�[�g�ϐ��ɃZ�b�g
            m_FPath = FPath
            m_Mode = fMode
            m_Access = fAccess
            m_Share = fShare
            If encoding Is Nothing Then
                m_Encoding = System.Text.Encoding.GetEncoding("shift-jis")
            Else
                m_Encoding = encoding
            End If

            '' �I�[�v�����̃��b�\�h���蓖��
            Select Case fAccess
                Case IO.FileAccess.Read
                    m_OpenMethod = New tDele_OpenDelegate(AddressOf OpenRead)
                Case IO.FileAccess.Write
                    m_OpenMethod = New tDele_OpenDelegate(AddressOf OpenWrite)
                Case IO.FileAccess.ReadWrite
                    m_OpenMethod = New tDele_OpenDelegate(AddressOf OpenReadWrite)
            End Select
        End Sub

        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Dispose() Implements IDisposable.Dispose
            m_OpenMethod = Nothing
            '' �t�@�C���N���[�Y
            Call Close()
            '' GC��Finalize���s��Ȃ��悤�ɒʒm
            GC.SuppressFinalize(Me)
        End Sub

        ''' <summary>
        ''' Finalize�f�X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overrides Sub Finalize()
            m_OpenMethod = Nothing
            '' �t�@�C���N���[�Y
            Call Close()
        End Sub

        ''' <summary>
        ''' �O�����J �t�@�C���I�[�v��
        ''' </summary>
        ''' <remarks>�R���X�g���N�^�Ŏw�肵���A�N�Z�X���[�h�ɂ��N�����\�b�h�ύX</remarks>
        Public Sub Open()
            Try
                '' �R���X�g���N�^�Ŏw�肵���A�N�Z�X���[�h�ɂ��N�����\�b�h�ύX
                m_OpenMethod.Invoke()
            Catch ex As Exception
                m_Writer = Nothing
                m_Reader = Nothing
                m_Stream = Nothing
                Throw New System.Exception("Stream �R���X�g���N�^�ُ�", ex)
            End Try
        End Sub

        ''' <summary>
        ''' �t�@�C���I�[�v���p�C���X�^���X����
        ''' </summary>
        ''' <remarks></remarks>
        Protected Sub OpenRead()
            '' �t�@�C���X�g���[���Z�b�g
            m_Stream = New System.IO.FileStream(m_FPath, m_Mode, m_Access, m_Share)
            '' �ǂݏ����p�N���X�Z�b�g
            m_Reader = New System.IO.StreamReader(m_Stream, m_Encoding)
        End Sub
        Protected Sub OpenWrite()
            '' �t�@�C���X�g���[���Z�b�g
            m_Stream = New System.IO.FileStream(m_FPath, m_Mode, m_Access, m_Share)
            '' �ǂݏ����p�N���X�Z�b�g
            m_Writer = New System.IO.StreamWriter(m_Stream, m_Encoding)
            m_Writer.AutoFlush = True		 '�o�b�t�@�ɒ~�������ɏo��
        End Sub
        Protected Sub OpenReadWrite()
            '' �t�@�C���X�g���[���Z�b�g
            m_Stream = New System.IO.FileStream(m_FPath, m_Mode, m_Access, m_Share)
            '' �ǂݏ����p�N���X�Z�b�g
            m_Reader = New System.IO.StreamReader(m_Stream, m_Encoding)
            m_Writer = New System.IO.StreamWriter(m_Stream, m_Encoding)
            m_Writer.AutoFlush = True		 '�o�b�t�@�ɒ~�������ɏo��
        End Sub

        ''' <summary>
        ''' �t�@�C���N���[�Y
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Close()
            Try
                '' �I�[�v���t�@�C���N���[�Y
                If Not Nothing Is m_Writer Then
                    m_Writer.Close()
                    m_Writer = Nothing
                End If
                If Not Nothing Is m_Reader Then
                    m_Reader.Close()
                    m_Reader = Nothing
                End If
                If Not Nothing Is m_Stream Then
                    m_Stream.Close()
                    m_Stream = Nothing
                End If
            Catch ex As Exception
                Throw New System.Exception("Stream Close �ُ�", ex)
            End Try
        End Sub

        ''' <summary>
        ''' 1�s�t�@�C���Ǎ���
        ''' </summary>
        ''' <returns>1�s���̕�����</returns>
        ''' <remarks> �擾�s�������A�܂��͓Ǎ��ُ݈�� Nothing���A��
        ''' 		  1�s�Ǎ��ނƁA���̍s�Ƀt�F�b�`���� ��While��Nothing���A��܂œǍ���ł�OK</remarks>
        Public Overridable Overloads Function ReadLine() As String
            Try
                '' 1�s�Ǎ��� (�Ǎ��ݍs������ Nothing)
                Return m_Reader.ReadLine()
            Catch
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' �S�t�@�C���Ǎ���
        ''' </summary>
        ''' <returns>�s�����̕�����</returns>
        ''' <remarks></remarks>
        Public Overridable Overloads Function ReadAll() As ArrayList
            Try
                '' �S�s�����X�g�ɒǉ�
                Dim ary As New ArrayList
                Do While m_Reader.Peek() >= 0
                    ary.Add(m_Reader.ReadLine())
                Loop
                Return ary
            Catch
                Return Nothing
            End Try
        End Function

        ''' <summary>
        ''' 1�s�t�@�C��������
        ''' </summary>
        ''' <param name="strVal">�t�@�C���t���p�X</param>
        ''' <returns>True:���� False:�ُ�</returns>
        ''' <remarks></remarks>
        Public Overridable Overloads Function WriteLine(ByVal strVal As String) As Boolean
            Try
                '' �t�@�C��������
                m_Writer.WriteLine(strVal)
                Return True
            Catch
                Return False
            End Try
        End Function
        Public Overridable Overloads Function WriteLine(ByVal strVal As String()) As Boolean
            Try
                '' �t�@�C��������
                For ii As Integer = 0 To strVal.Length - 1
                    m_Writer.WriteLine(strVal(ii))
                Next ii
                Return True
            Catch
                Return False
            End Try
        End Function
        Public Overridable Overloads Function WriteLine(ByVal strVal As ArrayList) As Boolean
            Try
                '' �t�@�C��������
                For ii As Integer = 0 To strVal.Count - 1
                    m_Writer.WriteLine(strVal(ii))
                Next ii
                Return True
            Catch
                Return False
            End Try
        End Function
    End Class
End Namespace

