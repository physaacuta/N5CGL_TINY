'*******************************************************************************
'�@�r���
'	[Ver]
'		Ver.01    2012/09/01  ����
'
'	[����]
'		DB�����r�摜���擾�ێ�����
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod
Imports System.Data.SqlClient

''' <summary>
''' �r���N���X
''' </summary>
''' <remarks></remarks>
Public Class clsDataDefect
    Implements IDisposable

    '///////////////////////////////////////////////////////////////////////////
    '�萔��`
    '///////////////////////////////////////////////////////////////////////////
    Private Const MAX_IMAGE As Integer = 4                                      ' �r�摜��

    '---------------------------------------------------------------------------
    ' �����o�ϐ�
    '---------------------------------------------------------------------------
    Private m_bIsDesposed As Boolean = False                                    ' ���\�[�X�J���ς݃t���E�O True:�J���ς�
    Private mcls_Log As tClass_LogManager                                       ' ���O�Ǘ�
    Private mcls_Param As clsParamManager                                       ' �p�����[�^�Ǘ�
    Private m_strKanriNo As String                                              ' �Ǘ�No
    Private m_typDefect As DETECT_BASE_DATA                                     ' �r���
    Private m_aryImage() As Bitmap                                              ' �r�摜
    Private m_aryTitle() As String                                              ' �摜��
    Private m_bImgRead As Boolean                                               ' �摜�ǂݍ���

    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �r�摜
    ''' </summary>
    ''' <returns>�r�摜</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectImage() As Bitmap()
        Get
            Return m_aryImage
        End Get
    End Property

    ''' <summary>
    ''' �r���擾
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectData() As DETECT_BASE_DATA
        Get
            Return m_typDefect
        End Get
    End Property

    ''' <summary>
    ''' �r���̊Ǘ�No
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KanriNo() As String
        Get
            Return m_strKanriNo
        End Get
    End Property

    Public ReadOnly Property ImageTitle() As String()
        Get
            Return m_aryTitle
        End Get
    End Property

    ''' <summary>
    ''' �摜�ǂݍ��݊m�F
    ''' </summary>
    ''' <returns></returns>
    Public ReadOnly Property IsImgRead() As Boolean
        Get
            Return m_bImgRead
        End Get
    End Property


    '---------------------------------------------------------------------------
    ' ���\�b�h
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �r���̍쐬
    ''' </summary>
    ''' <param name="param">�p�����[�^�Ǘ�</param>
    ''' <param name="log">���O�Ǘ�</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef param As clsParamManager, ByRef log As tClass_LogManager)
        Try
            mcls_Log = log
            mcls_Param = param

            ReDim m_aryImage(MAX_IMAGE - 1)
            ReDim m_aryTitle(m_aryImage.Length - 1)

            For ii As Integer = 0 To m_aryTitle.Length - 1
                Select Case CType(ii + 1, EM_IMAGE_NO)
                    Case EM_IMAGE_NO.S
                        m_aryTitle(ii) = DEFECT_IMG_NAME(0)
                    Case EM_IMAGE_NO.R1
                        m_aryTitle(ii) = DEFECT_IMG_NAME(1)
                    Case EM_IMAGE_NO.R2
                        m_aryTitle(ii) = DEFECT_IMG_NAME(2)
                    Case EM_IMAGE_NO.R3
                        m_aryTitle(ii) = DEFECT_IMG_NAME(3)
                    Case Else
                        m_aryTitle(ii) = String.Empty
                End Select
            Next

            DataInit()

        Catch ex As Exception
            log.Write(tClass_LogManager.LNO.ERR, String.Format("�������ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �I�u�W�F�N�g�̔j��
    ''' </summary>
    ''' <param name="disposing">True:�����I�ȌĂяo�� False:�Öق̌Ăяo��</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            ' TODO �I�u�W�F�N�g�j��
            If m_bIsDesposed = False Then
                For ii As Integer = 0 To m_aryImage.Length - 1
                    If Not m_aryImage(ii) Is Nothing Then
                        m_aryImage(ii).Dispose()
                        m_aryImage(ii) = Nothing
                    End If
                Next
            End If
            m_bIsDesposed = True                                            ' Dispose�ς݂Ƃ���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���I�u�W�F�N�g�j���ُ� [{0}]", ex.Message))
        End Try
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
    ''' �r�f�[�^�Z�b�g
    ''' </summary>
    ''' <param name="strKanriNo"></param>
    ''' <param name="typData"></param>
    Public Sub SetDefect(ByVal strKanriNo As String, ByVal typData As DETECT_BASE_DATA)

        If strKanriNo <> m_strKanriNo Then
            ' �����̊Ǘ�No.�ƈقȂ�f�[�^���Z�b�g����ꍇ�͉摜���ēǂݍ��݂���
            m_bImgRead = False
        End If
        If typData.nKizuNo <> m_typDefect.nKizuNo Then
            ' �������rNo.�ƈقȂ�f�[�^���Z�b�g����ꍇ�͉摜���ēǂݍ��݂���
            m_bImgRead = False
        End If

        m_strKanriNo = strKanriNo       ' �Ǘ�No
        m_typDefect = typData           ' �r���

        If Not m_bImgRead Then
            ' �r�摜��ǂݍ���
            Dim sw As New Stopwatch
            sw.Reset()
            sw.Start()
            Read(m_strKanriNo, m_typDefect.nKizuNo, m_typDefect.TorB)
            sw.Stop()
            'mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�Ǎ��ݎ��� [{0}]", sw.Elapsed))
        End If

    End Sub

    ''' <summary>
    ''' �r�摜�ǂݍ���
    ''' </summary>
    ''' <param name="strKizukenNo">�Ǘ�No</param>
    ''' <param name="nKizuNo">�rNo</param>
    ''' <param name="nMen">0:�\ 1:��</param>
    ''' <returns>True:�ǂݍ��ݐ��� False:�ǂݍ��ݎ��s</returns>
    ''' <remarks></remarks>
    Private Function Read(ByVal strKizukenNo As String, ByVal nKizuNo As Integer, ByVal nMen As Integer) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim readSQL As SqlDataReader = Nothing
        Try
            Dim strTblName() As String = {DB_DEFECT_IMG_T, DB_DEFECT_IMG_B}
            If 0 > nMen Or strTblName.Length - 1 < nMen Then Return False

            '-------------------------------------------------------------------
            ' �r�p�k�쐬 (�r�摜)
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL = ""
            strSQL &= "SELECT"

            strSQL &= " ���摜"
            strSQL &= " ,���摜"
            strSQL &= " ,���摜2"
            strSQL &= " ,���摜3"

            'strSQL &= " *"
            strSQL &= " FROM"
            strSQL &= " " & strTblName(nMen)
            strSQL &= " WHERE"
            strSQL &= " �Ǘ�No='" & strKizukenNo & "'"
            strSQL &= " AND �rNo=" & nKizuNo

            '-------------------------------------------------------------------
            ' ���s
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                              ' DB�ڑ����s
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜DB�ڑ����s"))
                Return False
            End If

            readSQL = clsDB.SelectExecute(strSQL)
            If readSQL Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�擾SQL���s���s SQL={0}", strSQL))
                Return False
            End If

            '-------------------------------------------------------------------
            ' �i�[
            '-------------------------------------------------------------------
            With readSQL
                For ii As Integer = 0 To m_aryImage.Length - 1
                    m_aryImage(ii) = Nothing
                Next
                If readSQL.Read() Then                                   ' �r�摜�͂���Ίi�[����
                    For ii As Integer = 0 To m_aryImage.Length - 1
                        If Not .IsDBNull(ii) Then
                            m_aryImage(ii) = tClass_BmpJpeg.DataToImage(CType(.GetValue(ii), Byte()))
                            m_bImgRead = True
                        End If
                    Next
                End If
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�ǂݍ��ُ݈� [{0}]", ex.Message))
            Return False
        Finally
            If clsDB Is Nothing = False Then
                clsDB.Cancel()
                If Not readSQL Is Nothing Then
                    readSQL.Close()
                    Read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return True
    End Function


    ''' <summary>
    ''' ����������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DataInit()
        Try

            m_typDefect = Nothing

            For ii As Integer = 0 To m_aryImage.Length - 1
                If Not m_aryImage(ii) Is Nothing Then
                    m_aryImage(ii).Dispose()
                    m_aryImage(ii) = Nothing
                End If
            Next

            m_bImgRead = False

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���I�u�W�F�N�g�������ُ� [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �f�[�^�R�s�[
    ''' </summary>
    ''' <param name="cls"></param>
    Public Sub DataCopy(ByVal cls As clsDataDefect)
        m_strKanriNo = cls.KanriNo                ' �Ǘ�No
        m_typDefect = cls.DefectData              ' �r���
        m_aryImage = cls.DefectImage              ' �r�摜
        m_bImgRead = cls.IsImgRead                ' �摜�ǂݍ���
    End Sub

End Class
