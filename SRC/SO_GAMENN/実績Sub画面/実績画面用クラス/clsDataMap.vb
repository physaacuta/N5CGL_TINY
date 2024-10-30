'*******************************************************************************
'�@�}�b�v�f�[�^
'	[Ver]
'		Ver.01    2010/11/30  ����
'
'	[����]
'		DB�����r�}�b�v�f�[�^�쐬�ێ�����
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod
Imports System.Data.SqlClient

''' <summary>
''' �}�b�v�f�[�^�N���X
''' </summary>
''' <remarks></remarks>
Public Class clsDataMap
    Implements IDisposable
    '---------------------------------------------------------------------------
    ' �񋓑̒�`
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �ǂݍ��݃f�[�^���
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum DATA_KIND
        emCOIL_POS = 0
        emCOIL_INF
        emCUT_INF
    End Enum

    '---------------------------------------------------------------------------
    ' �^��`
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �����L�[(�r���X�g�f�[�^)
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure LIST_KEY
        Dim emStage As EM_LONG_POS                                              ' �}�b�v�Ԋu�敪
        Dim nMen As Integer                                                     ' �\���敪 0:�\ 1:��
        Dim nCol As Integer                                                     ' ��ԍ� 0�I���W��
        Dim nRow As Integer                                                     ' �s�ԍ� 0�I���W��

        Shared Operator =(ByVal a As LIST_KEY, ByVal b As LIST_KEY) As Boolean
            If a.emStage <> b.emStage Or a.nMen <> b.nMen Or a.nCol <> b.nCol Or a.nRow <> b.nRow Then
                Return False
            End If
            Return True
        End Operator

        Shared Operator <>(ByVal a As LIST_KEY, ByVal b As LIST_KEY) As Boolean
            If a.emStage <> b.emStage Or a.nMen <> b.nMen Or a.nCol <> b.nCol Or a.nRow <> b.nRow Then
                Return True
            End If
            Return False
        End Operator

        ''' <summary>
        ''' �L�[�̏�����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub initialize()
            emStage = EM_LONG_POS.MOSTSMALL
            nMen = -1
            nCol = -1
            nRow = -1
        End Sub

        ''' <summary>
        ''' �L�[�̔�r
        ''' </summary>
        ''' <param name="obj">��r�ΏۃL�[</param>
        ''' <returns>True:��v False:�s��v</returns>
        ''' <remarks></remarks>
        Public Overrides Function Equals(ByVal obj As Object) As Boolean
            If TypeOf obj Is LIST_KEY = False Then
                Return False
            End If

            Dim arg As LIST_KEY = CType(obj, LIST_KEY)

            If arg.emStage = emStage _
            AndAlso arg.nMen = nMen _
            AndAlso arg.nCol = nCol _
            AndAlso arg.nRow = nRow Then                                        ' �S���̗v�f�������ď��߂�OK
                Return True
            Else
                Return False
            End If
        End Function

        ''' <summary>
        ''' ������\��
        ''' </summary>
        ''' <returns>������\��</returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return String.Format("Stage={0} Men={1} Col={2} Row={3}", emStage, nMen, nCol, nRow)
        End Function
    End Structure

    ''' <summary>
    ''' �����L�[(�r���X�g�L�[)
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure KIZU_KEY
        Dim nKizuNo As Integer                                                  ' �rNo
        Dim nStage As Integer                                                   ' �}�b�v�Ԋu�敪
    End Structure


    ''' <summary>
    ''' �����L�[(�r���)
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DEFECT_KEY
        Dim nKizuNo As Integer

        Public Sub New(ByVal no As Integer)
            Me.nKizuNo = no
        End Sub

    End Structure


    ''' <summary>
    ''' �ǂݍ��݃p�����[�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure READ_PARAM
        Dim strKanriNo As String                                              ' �Ǘ�No
        Dim nTidList As List(Of Integer)                                        ' �ǂݍ����r��EdasysID
        Dim nGidList As List(Of Integer)                                        ' �ǂݍ��݃O���[�hEdasysID
        Dim bNagate As Boolean                                                  ' ����������] True:���] False:�Ȃ�
        Dim bDS As Boolean                                                      ' DS�������]
        Dim bTansho As Boolean                                                  ' �T����ʂƓ����r�`�F�b�N�L��(True:�`�F�b�N���� False:�Ȃ�)
        Dim bFFSnd As Boolean                                                   ' �e�e���M�ς݃`�F�b�N�L��(True:�`�F�b�N���� False:�Ȃ�)
        Dim bCorrect As Boolean                                                 ' �␳���g�p(True:���� False:�Ȃ�)
    End Structure


    ' �R�C�����\����
    Public Structure TYP_COIL_INF
        Dim strKanriNo As String                    ' �Ǘ�No
        Dim strOrderNo As String                    ' ����No
        Dim strCoilNo As String                     ' ���YNo
        Dim strHinCode As String                    ' �i��
        Dim strUse As String                        ' �p�r
        Dim strSyuZai As String                     ' ��ގ�
        Dim strNextCode As String                   ' �v�掟�H���R�[�h
        Dim strKenKijun() As String                 ' �����
        Dim nCoilAtu As Integer                     ' �ޗ���
        Dim nCoilWidth As Integer                   ' �ޗ���
        Dim nCoilAtuSei As Integer                  ' ������
        Dim nCoilWidSei As Integer                  ' ������
        Dim nTrimWidth As Integer                   ' �g������
        Dim strMuke As String                       ' ����
        Dim nCoilLen As Integer                     ' �������u������ [mm]
        Dim bCorrect As Boolean                     ' �␳���g�p(True:�g�p�AFalse:�s�g�p)
        Dim dCorrect As Double                      ' �␳��
        Dim nOutTopPos As Integer                   ' �o���P�ʐ�[�ʒu [mm]
        Dim nScode() As Integer                     ' �����\�ʏ��No
        Dim nPcode() As Integer                     ' �����p�^�[���R�[�h
        Dim nKensa() As Integer                     ' �������
        Dim nKiki() As Integer                      ' �@����
        Dim dtToroku As DateTime                    ' �o�^���t
        Public Sub initialize()
            strKanriNo = String.Empty
            ReDim strKenKijun(NUM_MEN - 1)
            ReDim nKensa(NUM_MEN - 1)
            ReDim nKiki(NUM_MEN - 1)
            ReDim nScode(NUM_MEN - 1)
            ReDim nPcode(NUM_MEN - 1)
        End Sub
    End Structure

    ''' <summary>
    ''' �t���[�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure FRAME_INFO
        Dim nKanriNo As String
        Dim nFNo As Integer
        Dim nCamSet As Integer
        Dim nSSize As Integer
    End Structure

    '---------------------------------------------------------------------------
    ' �����o�ϐ�
    '---------------------------------------------------------------------------
    Private m_bIsDesposed As Boolean = False                                    ' ���\�[�X�J���ς݃t���E�O True:�J���ς�
    Private mcls_Param As clsParamManager                                       ' �p�����[�^�}�l�[�W��
    Private mcls_PreParam As clsPreParamManager                                 ' �O�H���p�����[�^�Ǘ��N���X
    Private mcls_Log As tClass_LogManager                                       ' ���O�Ǘ�

    Private m_aryInterval() As Integer                                          ' ���݂̃}�b�v�\���敪���̋���
    Private m_bTurnLR As Boolean                                                ' �}�b�v�f�[�^�̍��E���](True:���]���� False:�Ȃ�)
    Private m_bTurnTB As Boolean                                                ' �}�b�v�f�[�^�̏㉺���](True:���]�L�� False:�Ȃ�)

    Private m_stMap As DSP_G_MAP                                                ' �}�b�v���
    Private m_listCutInf As List(Of CUT_INF_DATA)                               ' �J�b�g���

    Private m_bEnaRead As Boolean                                               ' �ǂݍ��݋��t���O

    Private m_nColNum As Integer                                                ' MAP������(MAP_COL_NUM���ő�)
    Private m_nColStart As Integer                                              ' �J�n��
    Private m_nColEnd As Integer                                                ' �I����

    Private m_dctDefLst As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)) ' �f�B�N�V���i��(�r���X�g�f�[�^)
    Private m_dctDefKey As Dictionary(Of KIZU_KEY, LIST_KEY)                    ' �f�B�N�V���i��(�r���X�g�L�[)

    Private m_dctDefTcmLst As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)) ' �f�B�N�V���i��(TCM�r���X�g�f�[�^)
    Private m_dctDefTcmKey As Dictionary(Of KIZU_KEY, LIST_KEY)                    ' �f�B�N�V���i��(TCM�r���X�g�L�[)

    Private m_dctDefInf() As Dictionary(Of DEFECT_KEY, DETECT_BASE_DATA)        ' �f�B�N�V���i��(�r���) �������r���܂�
    Private m_dctDefTcmInf() As Dictionary(Of DEFECT_KEY, TYP_MAE_DEFECT_DATA)  ' �f�B�N�V���i��(TCM�r���)
    Private m_lstFrameInf As New List(Of FRAME_INFO)                            ' �t���[����񃊃X�g

    Private m_typCoilInf As New TYP_COIL_INF                                    ' �R�C�����

    '---------------------------------------------------------------------------
    ' �C�x���g
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �f�[�^�̓ǂݍ��݊J�n��ʒm���܂�
    ''' </summary>
    ''' <param name="nKind">�f�[�^���</param>
    ''' <param name="nMax">�f�[�^����</param>
    ''' <remarks></remarks>
    Public Event StartDataNti(ByVal nKind As DATA_KIND, ByVal nMax As Integer)

    ''' <summary>
    ''' �f�[�^�̓ǂݍ��݂�ʒm���܂�
    ''' </summary>
    ''' <param name="nKind">�f�[�^���</param>
    ''' <param name="nCount">�f�[�^����</param>
    ''' <remarks></remarks>
    Public Event ReadDataNti(ByVal nKind As DATA_KIND, ByVal nCount As Integer)

    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �����Ǘ�No���擾����
    ''' </summary>
    ''' <value></value>
    ''' <returns>���݂��Ȃ��ꍇ��""��Ԃ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizukenNo_In() As String
        Get
            Return m_typCoilInf.strKanriNo
        End Get
    End Property


    ''' <summary>
    ''' �o���Ǘ�No���擾����
    ''' </summary>
    ''' <value></value>
    ''' <returns>���݂��Ȃ��ꍇ��""��Ԃ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizukenNo_Out() As String
        Get
            Return m_typCoilInf.strKanriNo
        End Get
    End Property

    ''' <summary>
    ''' �o�^�������擾����
    ''' </summary>
    ''' <value></value>
    ''' <returns>���݂��Ȃ��ꍇ��""��Ԃ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DateTime() As String
        Get
            Return m_typCoilInf.dtToroku.ToString("yyyy/MM/dd HH:mm:ss")
        End Get
    End Property

    ''' <summary>
    ''' �R�C�������擾���܂�
    ''' </summary>
    ''' <returns>�R�C����[mm]</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CoilLength() As Integer
        Get
            Return m_typCoilInf.nCoilLen
        End Get
    End Property

    ''' <summary>
    ''' �R�C�������擾���܂�
    ''' </summary>
    ''' <returns>�R�C����[mm]</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CoilWidth() As Integer
        Get
            Return m_typCoilInf.nCoilWidth
        End Get
    End Property


    ''' <summary>
    ''' �}�b�v�\���敪���̋�����ݒ�擾
    ''' </summary>
    ''' <param name="stage">�}�b�v�\���敪 0:�ŏ��l 1:�\��(��) 2:�\��(��)</param>
    ''' <value>����[m]</value>
    ''' <returns>����[m]</returns>
    ''' <remarks></remarks>
    Public Property Interval(ByVal stage As EM_LONG_POS) As Integer
        Get
            Return m_aryInterval(stage)
        End Get
        Set(ByVal value As Integer)
            m_aryInterval(stage) = value
        End Set
    End Property

    ''' <summary>
    ''' �L�����Z���L�����擾���܂�
    ''' </summary>
    ''' <returns>True:�L�����Z�����ꂽ False:�ǂݍ��݌p��</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsReadCancel() As Boolean
        Get
            Return Not m_bEnaRead
        End Get
    End Property

    ''' <summary>
    ''' �}�b�v�l���擾���܂�
    ''' </summary>
    ''' <param name="key">���X�g�L�[</param>
    ''' <param name="nIndex">���X�g�C���f�b�N�X</param>
    ''' <returns>�o�����[</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListValue(ByVal key As LIST_KEY, ByVal nIndex As Integer) As TYP_MAP_LIST_VALUE
        Get
            If m_dctDefLst.ContainsKey(key) = False Then
                Return Nothing
            ElseIf m_dctDefLst(key).Count <= nIndex Then
                Return Nothing
            Else
                Return m_dctDefLst(key)(nIndex)
            End If
        End Get
    End Property

    ''' <summary>
    ''' ���X�g���̃C���f�b�N�X��Ԃ��܂�(0�I���W��)
    ''' </summary>
    ''' <param name="key">���X�g�L�[</param>
    ''' <param name="val">�o�����[</param>
    ''' <returns>���X�g�C���f�b�N�X</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListIndex(ByVal key As LIST_KEY, ByVal val As TYP_MAP_LIST_VALUE) As Integer
        Get
            If m_dctDefLst.ContainsKey(key) = False Then
                Return 0
            Else
                Return m_dctDefLst(key).IndexOf(val)
            End If
        End Get
    End Property

    ''' <summary>
    ''' �}�b�v���r��񃊃X�g���擾���܂�
    ''' </summary>
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <returns>�}�b�v���r���ꗗ</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectDataLst(ByVal nMen As Integer) As Dictionary(Of DEFECT_KEY, DETECT_BASE_DATA)
        Get
            Return m_dctDefInf(nMen)
        End Get
    End Property

    ''' <summary>
    ''' �J�b�g��񃊃X�g���擾���܂�
    ''' </summary>
    ''' <returns>�}�b�v���r���ꗗ</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CutDataLst() As List(Of CUT_INF_DATA)
        Get
            Return m_listCutInf
        End Get
    End Property

    ''' <summary>
    ''' �r���̗L����Ԃ��܂�
    ''' </summary>
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <param name="nKizuNo">�rNo</param>
    ''' <returns>True:�L��A1:����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectDataUmu(ByVal nMen As Integer, ByVal nKizuNo As clsDataMap.DEFECT_KEY) As Boolean
        Get
            If m_dctDefInf(nMen).ContainsKey(nKizuNo) = False Then
                Return False
            End If
            Return True
        End Get
    End Property

    ''' <summary>
    ''' �}�b�v���r������Ԃ��܂�
    ''' </summary>
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <returns>�r����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectDataCnt(ByVal nMen As Integer) As Integer
        Get
            If m_dctDefInf Is Nothing = True Then
                Return 0
            End If
            Return m_dctDefInf(nMen).Count
        End Get
    End Property

    ''' <summary>
    ''' �rNo�̗L����Ԃ��܂�
    ''' </summary>
    ''' <param name="key">���X�g�L�[</param>
    ''' <returns>True:�L��A1:����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListKizuNoKeyUmu(ByVal key As KIZU_KEY) As Boolean
        Get
            If m_dctDefKey.ContainsKey(key) = False Then
                Return False
            End If
            Return True
        End Get
    End Property

    ''' <summary>
    ''' �L�[�ɑ΂����r���X�g��Ԃ��܂�
    ''' </summary>
    ''' <param name="key">���X�g�L�[</param>
    ''' <returns>�o�����[</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListGetKizuNoKey(ByVal key As KIZU_KEY) As LIST_KEY
        Get
            If m_dctDefKey.ContainsKey(key) = False Then
                Return Nothing
            End If
            Return m_dctDefKey(key)
        End Get
    End Property

    ''' <summary>
    ''' �R�C�����
    ''' </summary>
    ''' <returns></returns>
    Public ReadOnly Property CoilInfo() As TYP_COIL_INF
        Get
            Return m_typCoilInf
        End Get
    End Property

    ''' <summary>
    ''' ����������]
    ''' </summary>
    ''' <returns>True:�L��AFalse:����</returns>
    Public ReadOnly Property TurnTB() As Boolean
        Get
            Return m_bTurnTB
        End Get
    End Property

    ''' <summary>
    ''' ���������]
    ''' </summary>
    ''' <returns>True:�L��AFalse:����</returns>
    Public ReadOnly Property TurnLR() As Boolean
        Get
            Return m_bTurnLR
        End Get
    End Property


    '---------------------------------------------------------------------------
    ' ���\�b�h
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �}�b�v�f�[�^�̍쐬
    ''' </summary>
    ''' <param name="param">�p�����[�^�Ǘ�</param>
    ''' <param name="log">���O�Ǘ�</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef param As clsParamManager, ByRef preparam As clsPreParamManager, ByRef log As tClass_LogManager)
        Try
            mcls_Param = param
            mcls_PreParam = preparam
            mcls_Log = log

            ReDim m_aryInterval(MAP_STAGE_NUM - 1)
            For stage As Integer = 0 To m_aryInterval.Length - 1
                m_aryInterval(stage) = MAP_REPRES_MIN
            Next

            m_nColNum = 0
            m_nColStart = 0
            m_nColEnd = MAP_COL_NUM - 1
            m_bTurnLR = False

            Dim sw As New Stopwatch

            ' MAP�s���𓮓I�Ɋm�ۂ���悤�ɕύX ������100�s������Ώ\��
            modMain.Log(tClass_LogManager.LNO.INF, "�̈�̊m�ۑO ")
            sw.Start()
            ' �rMAP ������
            m_stMap.initialize(100)
            sw.Stop()
            modMain.Log(tClass_LogManager.LNO.INF, "�̈�̊m�ێ��� [{0}]", sw.Elapsed)

            m_listCutInf = New List(Of CUT_INF_DATA)
            m_listCutInf.Clear()

            m_dctDefLst = New Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE))
            m_dctDefKey = New Dictionary(Of KIZU_KEY, LIST_KEY)
            m_dctDefTcmLst = New Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE))
            m_dctDefTcmKey = New Dictionary(Of KIZU_KEY, LIST_KEY)
            ReDim m_dctDefInf(NUM_MEN - 1)
            For ii As Integer = 0 To m_dctDefInf.Length - 1
                m_dctDefInf(ii) = New Dictionary(Of DEFECT_KEY, DETECT_BASE_DATA)
            Next

            ReDim m_dctDefTcmInf(NUM_MEN - 1)
            For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                m_dctDefTcmInf(ii) = New Dictionary(Of DEFECT_KEY, TYP_MAE_DEFECT_DATA)
            Next

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
                m_bIsDesposed = True                                            ' Dispose�ς݂Ƃ���

                If m_listCutInf IsNot Nothing Then
                    m_listCutInf.Clear()
                    m_listCutInf = Nothing
                End If
                If m_dctDefLst IsNot Nothing Then
                    m_dctDefLst.Clear()
                    m_dctDefLst = Nothing
                End If
                If m_dctDefKey IsNot Nothing Then
                    m_dctDefKey.Clear()
                    m_dctDefKey = Nothing
                End If
                If m_dctDefTcmLst IsNot Nothing Then
                    m_dctDefTcmLst.Clear()
                    m_dctDefTcmLst = Nothing
                End If
                If m_dctDefTcmKey IsNot Nothing Then
                    m_dctDefTcmKey.Clear()
                    m_dctDefTcmKey = Nothing
                End If
                If m_dctDefInf IsNot Nothing Then
                    For ii As Integer = 0 To m_dctDefInf.Length - 1
                        If m_dctDefInf(ii) IsNot Nothing Then
                            m_dctDefInf(ii).Clear()
                            m_dctDefInf(ii) = Nothing
                        End If
                    Next
                    m_dctDefInf = Nothing
                End If
                If m_dctDefTcmInf IsNot Nothing Then
                    For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                        If m_dctDefTcmInf(ii) IsNot Nothing Then
                            m_dctDefTcmInf(ii).Clear()
                            m_dctDefTcmInf(ii) = Nothing
                        End If
                    Next
                    m_dctDefInf = Nothing
                End If
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�f�[�^�I�u�W�F�N�g�j���ُ� [{0}]", ex.Message))
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
    ''' �}�b�v�f�[�^�̍쐬���L�����Z�����܂�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub CancelRead()
        Try
            m_bEnaRead = False
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�f�[�^�ǂݍ��݃L�����Z���ُ� [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �}�b�v�f�[�^��ǂݍ��݂܂�
    ''' </summary>
    ''' <param name="param"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function Read(ByVal param As READ_PARAM) As Boolean
        Try
            Dim nMax As Integer = 0
            Dim bRet As Boolean = False

            With param

                '-------------------------------------------------------------------
                ' �ǂݍ��݊J�n����
                '-------------------------------------------------------------------
                ' �f�[�^���N���A
                m_typCoilInf.initialize()
                m_dctDefKey.Clear()
                m_dctDefLst.Clear()
                m_dctDefTcmKey.Clear()
                m_dctDefTcmLst.Clear()
                For ii As Integer = 0 To m_dctDefInf.Length - 1
                    m_dctDefInf(ii).Clear()
                Next
                For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                    m_dctDefTcmInf(ii).Clear()
                Next

                ' �R�C���f�[�^�𐶐�����
                bRet = ReadCoilInfo(.strKanriNo, .bCorrect)
                If Not bRet Then
                    Return False
                End If

                'If .strKanriNoIn <> "" Then
                '    ' �R�C���������O�ɏo��
                '    Log(tClass_LogManager.LNO.MSG, "�����R�C��[{0}] �R�C����[{1:###,###,##0} mm]", m_lstCoilInf(0).strOutKizuKen, m_lstCoilInf(0).nCoilLen)
                'End If

                ' �}�b�v�f�[�^�C�j�V����
                bRet = ReadInitialize(.bNagate)                             ' �ǂݍ��݊J�n����
                If Not bRet Then
                    Return False
                End If

                '-------------------------------------------------------------------
                ' �}�b�v�ʒu���ǂݍ���(����)
                '-------------------------------------------------------------------
                ' �f�[�^�����擾
                bRet = GetMapPosCount(m_typCoilInf.strKanriNo, DB_COIL_POSITION, nMax)
                If Not bRet Then
                    Return False
                End If

                RaiseEvent StartDataNti(DATA_KIND.emCOIL_POS, nMax)         ' �}�b�v�ʒu���ő吔��ʒm����
                Application.DoEvents()

                ' �f�[�^�ǂݍ���
                bRet = ReadMapPos(m_typCoilInf.strKanriNo, DB_COIL_POSITION, .bNagate, m_typCoilInf.bCorrect)            ' �}�b�v�ʒu���ǂݍ���
                If Not bRet Then
                    Return False
                End If

				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>> ���ꏊ�ړ�
				'-------------------------------------------------------------------
				' �J�b�g���ǂݍ���
				'-------------------------------------------------------------------
				' ��񐔂̎擾
				bRet = GetCutInfCount(m_typCoilInf.strKanriNo, nMax)
				If Not bRet Then
					Return False
				End If

				RaiseEvent StartDataNti(DATA_KIND.emCUT_INF, nMax)
				Application.DoEvents()

				' �J�b�g���ǂݍ���
				bRet = ReadCutInf(m_typCoilInf.strKanriNo, .bNagate, m_typCoilInf.bCorrect)                  ' �r�}�b�v���ǂݍ���
				If Not bRet Then
					Return False
				Else
					Application.DoEvents()
				End If
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

				'-------------------------------------------------------------------
				' �}�b�v���ǂݍ���
				'-------------------------------------------------------------------
				' ��񐔂̎擾
				bRet = GetMapInfCount(m_typCoilInf.strKanriNo, .nTidList, .nGidList, .bTansho, .bFFSnd, nMax)
                If Not bRet Then
                    Return False
                End If

                RaiseEvent StartDataNti(DATA_KIND.emCOIL_INF, nMax)
                Application.DoEvents()

                ' �}�b�v���ǂݍ���
                bRet = ReadMapInf(m_typCoilInf.strKanriNo, .nTidList, .nGidList, .bTansho, .bNagate, .bDS, .bFFSnd, m_typCoilInf.bCorrect)   ' �r�}�b�v���ǂݍ���
                If Not bRet Then
                    Return False
                Else
                    Application.DoEvents()
                End If

                ' �}�b�v���ǂݍ���
                bRet = ReadPreMapInf(m_typCoilInf.strKanriNo, .bNagate, .bDS)   ' �r�}�b�v���ǂݍ���
                'If Not bRet Then
                '    Return False
                'Else
                '    Application.DoEvents()
                'End If


                Sort_Map_Kizulist(m_dctDefLst, .bNagate)
                ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
                'Sort_Map_Kizulist(m_dctDefTcmLst, .bNagate)
                Sort_Map_KizulistTcm(m_dctDefTcmLst, .bNagate)
				' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>> ���ꏊ�ړ�
				'-------------------------------------------------------------------
				' �J�b�g���ǂݍ���
				'-------------------------------------------------------------------
				'' ��񐔂̎擾
				'bRet = GetCutInfCount(m_typCoilInf.strKanriNo, nMax)
				'If Not bRet Then
				'	Return False
				'End If
				'
				'RaiseEvent StartDataNti(DATA_KIND.emCUT_INF, nMax)
				'Application.DoEvents()
				'
				'' �J�b�g���ǂݍ���
				'bRet = ReadCutInf(m_typCoilInf.strKanriNo, .bNagate, m_typCoilInf.bCorrect)                  ' �r�}�b�v���ǂݍ���
				'If Not bRet Then
				'    Return False
				'Else
				'	Application.DoEvents()
				'End If
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

				ReadFinalize(.bNagate)                                           ' �ǂݍ��ݏI������

                m_bTurnLR = .bDS
                m_bTurnTB = .bNagate

            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�f�[�^�ǂݍ��ُ݈� [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function


    ''' <summary>
    ''' �R�C������ǂݍ��݂܂�
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="bCorrect">�␳���g�p(True:����AFalse:�Ȃ�)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ReadCoilInfo(ByVal strKanriNo As String, ByVal bCorrect As Boolean) As Boolean
        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlDataReader = Nothing
        Dim bRet As Boolean = False

        Try

            '-------------------------------------------------------------------
            ' �r�p�k�쐬
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " RES.�o�^���t as �o�^���t"
            strSQL &= " ,RES.�Ǘ�No "
            strSQL &= " ,ISNULL(RES.����������,0) as ���������� "
            strSQL &= " ,ISNULL(RES.�������_�\, 0) as �������_�\ "
            strSQL &= " ,ISNULL(RES.�������_��, 0) as �������_�� "
            strSQL &= " ,ISNULL(RES.�@����_�\, 0) as �@����_�\ "
            strSQL &= " ,ISNULL(RES.�@����_��, 0) as �@����_�� "
            strSQL &= " ,ISNULL(INF.����No, '') as ����No"
            strSQL &= " ,ISNULL(INF.���YNo,'') as ���YNo "
            strSQL &= " ,ISNULL(INF.�i��, '') as �i��"
            strSQL &= " ,ISNULL(INF.�p�r, '') as �p�r"
            strSQL &= " ,ISNULL(INF.��ގ�, '') as ��ގ�"
            strSQL &= " ,ISNULL(INF.�v�掟�H���R�[�h, '') as ���H���R�[�h "
            strSQL &= " ,ISNULL(INF.�����_�\, '') as �����_�\ "
            strSQL &= " ,ISNULL(INF.�����_��, '') as �����_�� "
            strSQL &= " ,ISNULL(INF.�ޗ���, 0) as �ޗ��� "
            strSQL &= " ,ISNULL(INF.�ޗ���, 0) as �ޗ��� "
            strSQL &= " ,ISNULL(INF.������, 0) as ������ "
            strSQL &= " ,ISNULL(INF.������, 0) as ������ "
            strSQL &= " ,ISNULL(INF.���H���g������, 0) as �g������ "
            strSQL &= " ,ISNULL(INF.����, '') as ���� "
            strSQL &= " ,ISNULL(RES.�����\�ʏ��No_�\, 0) as �����\�ʏ��_�\ "
            strSQL &= " ,ISNULL(RES.�����\�ʏ��No_��, 0) as �����\�ʏ��_�� "
            strSQL &= " ,ISNULL(RES.�����p�^�[���R�[�h_�\, 0) as �����p�^�[���R�[�h_�\ "
            strSQL &= " ,ISNULL(RES.�����p�^�[���R�[�h_��, 0) as �����p�^�[���R�[�h_�� "
            strSQL &= " ,ISNULL(RES.�␳��, 1) as �␳�� "
            strSQL &= " ,ISNULL(RES.�o���P�ʐ�[�ʒu, 0) as �o���P�ʐ�[�ʒu "

            ' SQL FROM��
            strSQL &= " FROM " & DB_COIL_RESULT & " RES "
            ' SQL �O��������
            strSQL &= " LEFT OUTER JOIN " & DB_COIL_INF & " INF "
            strSQL &= " ON RES.�Ǘ�No = INF.�Ǘ�No "
            ' SQL WHERE��
            strSQL &= " WHERE RES.�Ǘ�No = '" & strKanriNo & "' "

            '-------------------------------------------------------------------
            ' ���s
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = tcls_DB.Open()
            If Not bDbRet Then                                              ' DB�ڑ����s
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����擾DB�ڑ����s"))
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSQL)
            If sqlRead Is Nothing Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����擾SQL���s���s SQL={0}", strSQL))
                Exit Try
            End If

            If Not sqlRead.Read() Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����f�[�^�Ȃ�={0}", strKanriNo))
                Exit Try
            End If

            '-------------------------------------------------------------------
            ' �i�[
            '-------------------------------------------------------------------
            m_typCoilInf.initialize()
            With sqlRead
                m_typCoilInf.strKanriNo = CStr(.GetValue(.GetOrdinal("�Ǘ�No")))              ' COIL_RESULT �Ǘ�No
                m_typCoilInf.strOrderNo = CStr(.GetValue(.GetOrdinal("����No")))              ' COIL_INF ����No
                m_typCoilInf.strCoilNo = CStr(.GetValue(.GetOrdinal("���YNo")))               ' COIL_INF ���YNo
                m_typCoilInf.strHinCode = CStr(.GetValue(.GetOrdinal("�i��")))                ' COIL_INF �i��
                m_typCoilInf.strUse = CStr(.GetValue(.GetOrdinal("�p�r")))                    ' COIL_INF �p�r
                m_typCoilInf.strSyuZai = CStr(.GetValue(.GetOrdinal("��ގ�")))               ' COIL_INF ��ގ�
                m_typCoilInf.strNextCode = CStr(.GetValue(.GetOrdinal("���H���R�[�h")))       ' COIL_INF �v�掟�H���R�[�h
                m_typCoilInf.strKenKijun(0) = CStr(.GetValue(.GetOrdinal("�����_�\")))     ' COIL_INF �����_�\
                m_typCoilInf.strKenKijun(1) = CStr(.GetValue(.GetOrdinal("�����_��")))     ' COIL_INF �����_�\
                m_typCoilInf.nCoilAtu = .GetInt32(.GetOrdinal("�ޗ���"))                    ' COIL_INF �ޗ���
                m_typCoilInf.nTrimWidth = .GetInt32(.GetOrdinal("�g������"))                  ' COIL_INF ���H���g������
                m_typCoilInf.nCoilWidth = .GetInt32(.GetOrdinal("�ޗ���"))                  ' COIL_INF �ޗ���
                'If 0 = m_typCoilInf.nTrimWidth Then
                '    ' �g��������0�̏ꍇ�́A�ޗ������g�p����
                '    m_typCoilInf.nCoilWidth = .GetInt32(.GetOrdinal("�ޗ���"))            �@' COIL_INF �ޗ���
                'Else
                '    m_typCoilInf.nCoilWidth = m_typCoilInf.nTrimWidth
                'End If
                m_typCoilInf.nCoilAtuSei = .GetInt32(.GetOrdinal("������"))                 ' COIL_INF ������
                m_typCoilInf.nCoilWidSei = .GetInt32(.GetOrdinal("������"))            �@   ' COIL_INF ������
                m_typCoilInf.strMuke = CStr(.GetValue(.GetOrdinal("����")))                   ' COIL_INF ����
                m_typCoilInf.nCoilLen = .GetInt32(.GetOrdinal("����������"))                  ' COIL_RESULT ����������
                m_typCoilInf.nScode(0) = .GetInt32(.GetOrdinal("�����\�ʏ��_�\"))            ' COIL_RESULT �����\�ʏ��No_�\
                m_typCoilInf.nScode(1) = .GetInt32(.GetOrdinal("�����\�ʏ��_��"))            ' COIL_RESULT �����\�ʏ��No_�\
                m_typCoilInf.nPcode(0) = .GetInt32(.GetOrdinal("�����p�^�[���R�[�h_�\"))      ' COIL_RESULT �����p�^�[���R�[�h_�\
                m_typCoilInf.nPcode(1) = .GetInt32(.GetOrdinal("�����p�^�[���R�[�h_��"))      ' COIL_RESULT �����p�^�[���R�[�h_�\
                m_typCoilInf.nKensa(0) = .GetInt32(.GetOrdinal("�������_�\"))                ' COIL_RESULT �������_�\
                m_typCoilInf.nKensa(1) = .GetInt32(.GetOrdinal("�������_��"))                ' COIL_RESULT �������_�\
                m_typCoilInf.nKiki(0) = .GetInt32(.GetOrdinal("�@����_�\"))                 ' COIL_RESULT �@����_�\
                m_typCoilInf.nKiki(1) = .GetInt32(.GetOrdinal("�@����_��"))                 ' COIL_RESULT �@����_�\

                m_typCoilInf.dtToroku = .GetDateTime(.GetOrdinal("�o�^���t"))                 ' COIL_RESULT �o�^���t
                m_typCoilInf.dCorrect = .GetDouble(.GetOrdinal("�␳��"))                     ' COIL_RESULT �␳��
                If 0 >= m_typCoilInf.dCorrect Then
                    m_typCoilInf.dCorrect = 1
                End If

                m_typCoilInf.bCorrect = bCorrect
                If m_typCoilInf.bCorrect Then
                    '�R�C�����ɕ␳����ݒ�
                    m_typCoilInf.nCoilLen = CInt(m_typCoilInf.nCoilLen * m_typCoilInf.dCorrect)
                End If
                m_typCoilInf.nOutTopPos = .GetInt32(.GetOrdinal("�o���P�ʐ�[�ʒu"))        ' COIL_RESULT �o���P�ʐ�[�ʒu

            End With

            ' �ǂݍ��ݐ����Ȃ̂ŁA�߂�l��True���Z�b�g
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����擾�ǂݍ��� ��O���� [{0}]", ex.Message))
        Finally
            If tcls_DB IsNot Nothing Then
                tcls_DB.Cancel()
                If sqlRead IsNot Nothing Then
                    sqlRead.Close()
                End If
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet

    End Function


    ''' <summary>
    ''' �}�b�v�f�[�^������
    ''' </summary>
    ''' <param name="bNagate">����W�J���](True:����AFalse:�Ȃ�)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ReadInitialize(ByVal bNagate As Boolean) As Boolean
        Try
            m_bEnaRead = True

            '-------------------------------------------------------------------
            ' �}�b�v���̌v�Z
            '-------------------------------------------------------------------
            Dim nColNum As Integer = SelectGridCol2(CoilWidth, MAP_COL_INTERVAL, MAP_COL_NUM)
            m_nColNum = nColNum
            m_nColStart = (MAP_COL_NUM - nColNum) \ 2
            m_nColEnd = MAP_COL_NUM - m_nColStart - 1

            '-------------------------------------------------------------------
            ' �}�b�v�s����K�v�ȍs���ɍ��킹�ē��I�Ɋm�ۂ���
            '-------------------------------------------------------------------
            Dim sw As New Stopwatch
            Dim nRow As Integer = CoilLength \ 1000 \ Interval(0) + 1       ' �m�ې��s��
            modMain.Log(tClass_LogManager.LNO.INF, "�̈�̊m�ۑO �m�ۂ���s��[{0}]", nRow)
            sw.Start()
            m_stMap.initialize(nRow)
            sw.Stop()
            modMain.Log(tClass_LogManager.LNO.INF, "�̈�̊m�ێ��� [{0}] �s��[{1}]", sw.Elapsed, nRow)

            ' �s���̏��������s��
            For nStage As Integer = 0 To m_stMap.stage.Length - 1
                For nMen As Integer = 0 To m_stMap.stage(nStage).men.Length - 1
                    For nRow = 0 To CoilLength \ 1000 \ Interval(CType(nStage, EM_LONG_POS))
                        If nRow >= m_stMap.stage(nStage).men(nMen).row.Length Then
                            ' �z������s�������H �s���̗̈�m�ێ��s
                            mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("�}�b�v�ʒu���ǂݍ��ُ݈� ���������Z�o���ُ� nRow[{0}} �}�b�v�s��[{1}]", nRow, m_stMap.stage(0).men(nMen).row.Length))
                            Continue For
                        End If
                        ' �s��񏉊��l�Z�b�g
                        With m_stMap.stage(nStage).men(nMen).row(nRow)
                            .nLen_i = (nRow + 1) * Interval(CType(nStage, EM_LONG_POS)) ' ���������ݒ�
                            .nLen_o = .nLen_i                                           ' �o�������ݒ�
                            .emKiki = 0
                            .emKensa = EM_DIV_KENSA.DIV_KENSA_OK
                        End With
                        ' �e��̏���������
                        For col As Integer = 0 To m_stMap.stage(nStage).men(nMen).row(nRow).col.Length - 1                 ' �}�b�v��񏉊���
                            If col < m_nColStart OrElse m_nColEnd < col Then
                                ' �R�C�����͈̔͊O�̏ꍇ
                                m_stMap.stage(nStage).men(nMen).row(nRow).col(col).nKizuNo = -2
                            Else
                                ' �R�C�����͈͓̔��̏ꍇ
                                m_stMap.stage(nStage).men(nMen).row(nRow).col(col).nKizuNo = 0
                            End If
                        Next
                        'TCM������
                        For col As Integer = 0 To m_stMap.stage(nStage).men(nMen).row(nRow).tcm.Length - 1
                            m_stMap.stage(nStage).men(nMen).row(nRow).tcm(col).nKizuNo = 0
                        Next

                    Next
                Next
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�f�[�^�ǂݍ��ݏ������ُ� [{0}]", ex.Message))
            Return False
        End Try

        Return m_bEnaRead
    End Function


    ''' <summary>
    ''' �}�b�v�ʒu��񐔎擾
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="nCount">�擾������(�߂�l)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetMapPosCount(ByVal strKanriNo As String, ByVal strTable As String, ByRef nCount As Integer) As Boolean

        Dim bRet As Boolean = False

        Try
            nCount = 0
            '-------------------------------------------------------------------
            ' �r�p�k�쐬
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " COUNT(*)"
            strSQL &= " FROM " & strTable
            strSQL &= " WHERE"
            strSQL &= " �Ǘ�No ='" & strKanriNo & "'"

            '-------------------------------------------------------------------
            ' ���s
            '-------------------------------------------------------------------
            If ExecuteSelectCount(strSQL, nCount) Then
                ' �ǂݍ��ݐ����Ȃ̂ŁA�߂�l��True���Z�b�g
                bRet = True
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ʒu��񐔎擾�ُ� [{0}][{1}]", strTable, ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �}�b�v�ʒu���ǂݍ���
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="strTable">�e�[�u������</param>
    ''' <param name="bNagate"></param>
    ''' <param name="bCorrect">�␳���g�p(True:����AFalse:�Ȃ�)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ReadMapPos(ByVal strKanriNo As String, ByVal strTable As String, ByVal bNagate As Boolean, ByVal bCorrect As Boolean) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim bRet As Boolean = False
        Try
            '-------------------------------------------------------------------
            ' �r�p�k�쐬
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " *"
            strSQL &= " FROM"
            strSQL &= " " & strTable
            strSQL &= " WHERE"
            strSQL &= " �Ǘ�No ='" & strKanriNo & "'"
            strSQL &= " ORDER BY ID ASC"

            '-------------------------------------------------------------------
            ' ���s
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If Not bDbRet Then                                              ' DB�ڑ����s
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ʒu���DB�ڑ����s"))
                Exit Try
            End If

            read = clsDB.SelectExecute(strSQL)
            If read Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ʒu���擾SQL���s���s SQL={0}", strSQL))
                Exit Try
            End If

            '-------------------------------------------------------------------
            ' �i�[
            '-------------------------------------------------------------------
            Dim nCount As Integer = 0
            Dim nMaxRow As Integer = m_stMap.stage(0).men(0).row.Length - 1 ' �ő�s�ԍ�
            Dim nCurRow As Integer = 0                                      ' �ݒ�ς݂̍s�ԍ�
            If bNagate Then
                '��������ɔ��]�̏ꍇ�́A��������J�n����
                nCurRow = nMaxRow
            End If

            ' �J�n�s�̌�����Ԃ�ΏۊO�ɂ��Ă���
            For nMen As Integer = 0 To m_stMap.stage(0).men.Length - 1
                m_stMap.stage(0).men(nMen).row(nCurRow).emKensa = EM_DIV_KENSA.DIV_KENSA_NON
            Next

            While read.Read()
                If Not m_bEnaRead Then                                      ' �ǂݍ��ݕs�̏ꍇ
                    '�ǂݍ��݃L�����Z��
                    Exit Try
                End If

                Dim nRow As Integer = CInt(read.GetValue(read.GetOrdinal("ID")))
                Dim nPos As Integer = nRow * Interval(0)

                If bCorrect Then
                    '�␳������������
                    Dim dHosei As Double = m_typCoilInf.dCorrect
                    nPos = CInt(Int(nPos * dHosei))
                End If
                nRow = nPos \ Interval(0)

                If nRow > nMaxRow Then                                     ' �ő�l�𒴂��Ă���ꍇ
                    'mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ʒu���̍s���I�[�o�[ Row={0} �Ǘ�No={1}", nRow, strKizukenNo))
                    nRow = nMaxRow
                End If
                If nRow < 0 Then                                                ' �t�W�J�����Ƃ��Ƃ��}�C�i�X�ɂȂ�\������
                    'mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ʒu���̍s���I�[�o�[ Row={0} �Ǘ�No={1}", nRow, strKizukenNo))
                    nRow = 0
                End If

                If bNagate Then
                    '��������ɔ��]�̏ꍇ�́A�s�ʒu��ϊ�
                    nRow = nMaxRow - nRow
                End If

                With m_stMap.stage(0)
                    ' �s���Ɏ�����������ꍇ�́A�ݒ�ςݍs�̏����������s�ɃZ�b�g����B
                    If bNagate AndAlso nCurRow - nRow > 1 Then
                        ' ���蔽�]����
                        For ii As Integer = nCurRow - 1 To nRow + 1 Step -1
                            If 0 > ii Or nMaxRow < ii Then
                                Continue For
                            End If
                            For nMen As Integer = 0 To .men.Length - 1
                                .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                                .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                            Next
                        Next
                    ElseIf Not bNagate AndAlso nRow - nCurRow > 1 Then
                        ' ���蔽�]�Ȃ�
                        For ii As Integer = nCurRow + 1 To nRow - 1
                            If 0 > ii Or nMaxRow < ii Then
                                Continue For
                            End If
                            For nMen As Integer = 0 To .men.Length - 1
                                .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                                .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                            Next
                        Next
                    End If

                    Dim emKensaO As EM_DIV_KENSA = CType(read.GetValue(read.GetOrdinal("�������_�\")), EM_DIV_KENSA)
                    Dim emKensaU As EM_DIV_KENSA = CType(read.GetValue(read.GetOrdinal("�������_��")), EM_DIV_KENSA)
                    If .men(0).row(nRow).emKensa < emKensaO Then
                        .men(0).row(nRow).emKensa = emKensaO
                    End If
                    If .men(1).row(nRow).emKensa < emKensaU Then
                        .men(1).row(nRow).emKensa = emKensaU
                    End If
                    Dim emKikiO As EM_DIV_KIKI = CType(read.GetValue(read.GetOrdinal("�@����_�\")), EM_DIV_KIKI)
                    Dim emKikiU As EM_DIV_KIKI = CType(read.GetValue(read.GetOrdinal("�@����_��")), EM_DIV_KIKI)
                    If .men(0).row(nRow).emKiki < emKikiO Then
                        .men(0).row(nRow).emKiki = emKikiO
                    End If
                    If .men(1).row(nRow).emKiki < emKikiU Then
                        .men(1).row(nRow).emKiki = emKikiU
                    End If

                    nCurRow = nRow
                End With

                nCount += 1                                                     ' ��͂���܂�����[
                If nCount Mod 100 = 0 Then
                    RaiseEvent ReadDataNti(DATA_KIND.emCOIL_POS, nCount)            ' �ǂݍ��݌����ʒm
                    Application.DoEvents()                                          ' �������Ă�C�x���g�����s����
                End If
            End While

            'COIL_POSITION�̍ŏI�s�����A���уR�C�����������ꍇ�̎������Ή�
            If bNagate AndAlso 0 < nCurRow Then
                ' ���蔽�]����
                For ii As Integer = nCurRow - 1 To 0 Step -1
                    If 0 > ii Or nMaxRow < ii Then
                        Continue For
                    End If
                    With m_stMap.stage(0)
                        For nMen As Integer = 0 To .men.Length - 1
                            .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                            .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                        Next
                    End With
                Next ii
            ElseIf Not bNagate AndAlso nMaxRow > nCurRow Then
                ' ���蔽�]�Ȃ�
                For ii As Integer = nCurRow + 1 To nMaxRow
                    If 0 > ii Or nMaxRow < ii Then
                        Continue For
                    End If
                    With m_stMap.stage(0)
                        For nMen As Integer = 0 To .men.Length - 1
                            .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                            .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                        Next
                    End With
                Next ii
            End If

            ' �ǂݍ��ݐ����Ȃ̂ŁA�߂�l��True���Z�b�g
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ʒu���ǂݍ��ُ݈� [{0}]", ex.Message))

        Finally
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �}�b�v��񐔂��擾���܂�
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="nTid">�\���ΏۊO�r��EdasysID���X�g</param>
    ''' <param name="nGid">�\���ΏۊO�O���[�hEdasysID���X�g</param>
    ''' <param name="bTansho">�T����ʂƓ����r�`�F�b�N�L�� True:�`�F�b�N���� False:�Ȃ�</param>
    ''' <param name="bFFSnd">���z�r���p�r�̂݃`�F�b�N�L�� True:�`�F�b�N���� False:�Ȃ�</param>
    ''' <param name="nCount">���</param>
    ''' <returns>True:�擾������ False:�擾���s</returns>
    ''' <remarks></remarks>
    Private Function GetMapInfCount(
     ByVal strKanriNo As String,
     ByVal nTid As List(Of Integer),
     ByVal nGid As List(Of Integer),
     ByVal bTansho As Boolean,
     ByVal bFFSnd As Boolean,
     ByRef nCount As Integer) As Boolean

        Dim bRet As Boolean = False

        Try
            Dim strTable() As String = {DB_DEFECT_INF_T, DB_DEFECT_INF_B}
            nCount = 0
            For ii As Integer = 0 To strTable.Length - 1
                '---------------------------------------------------------------
                ' �r�p�k�쐬
                '---------------------------------------------------------------
                Dim strSQL As String = ""
                strSQL &= "SELECT"
                strSQL &= " COUNT(*)"
                strSQL &= " FROM"
                strSQL &= " " & strTable(ii)
                strSQL &= " d "

                If bFFSnd Then
                    'strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " adf "
                End If

                strSQL &= " WHERE"
                strSQL &= " d.�Ǘ�No = '" & strKanriNo & "'"
                strSQL &= " AND d.�d������ <> 1 "

                'FF�T�[�o���M�r�I���@�\�ǉ�
                If bFFSnd Then
                    'strSQL &= "AND ((adf.�Ǘ�No = d.�Ǘ�No) AND (adf.�rNo=d.�rNo)) "
                ElseIf bTansho Then
                    strSQL &= " AND d.�\���Ώۋ敪_�T����� = 1 "
                Else
                    For jj As Integer = 0 To nTid.Count - 1                         ' �r��ݒ�
                        strSQL &= " AND d.�����r�� = " & CStr(nTid(jj))
                    Next

                    For jj As Integer = 0 To nGid.Count - 1                         ' �O���[�h�ݒ�
                        strSQL &= " AND d.����O���[�h = " & CStr(nGid(jj))
                    Next
                End If

                '---------------------------------------------------------------
                ' ���s
                '---------------------------------------------------------------
                Dim nCnt As Integer = 0
                If ExecuteSelectCount(strSQL, nCnt) Then
                    nCount += nCnt
                Else
                    Exit Try
                End If
            Next

            ' �ǂݍ��ݐ����Ȃ̂ŁA�߂�l��True���Z�b�g
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v��񐔎擾�ُ� [{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �}�b�v���ǂݍ���
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="nLstGid">�\���ΏۊO�O���[�hEdasysID���X�g</param>
    ''' <param name="bTansho">�T����ʂƓ����r�`�F�b�N�L�� True:�`�F�b�N���� False:�Ȃ�</param>
    ''' <param name="bNagate">���蔽�] True:���]���� False:�Ȃ�</param>
    ''' <param name="bDS">DS�ʒu���] True:���](�E��DS) False:�Ȃ�(����DS)</param>
    ''' <param name="bFFSnd">���z�r���p�r�̂݃`�F�b�N�L�� True:�`�F�b�N���� False:�Ȃ�</param>
    ''' <param name="bCorrect">�␳���g�p(True:����AFalse:�Ȃ�)</param>
    ''' <returns>True:�ǂݍ��ݐ��� False:�ǂݍ��ݎ��s</returns>
    ''' <remarks></remarks>
    Private Function ReadMapInf(
      ByVal strKanriNo As String,
      ByVal nLstTid As List(Of Integer),
      ByVal nLstGid As List(Of Integer),
      ByVal bTansho As Boolean,
      ByVal bNagate As Boolean,
      ByVal bDS As Boolean,
      ByVal bFFSnd As Boolean,
      ByVal bCorrect As Boolean) As Boolean

        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim bRet As Boolean = False

        Try
            '' �t���[������ǂݍ���
            'ReadFrameInf(strKanriNo)

            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If Not bDbRet Then                                          ' DB�ڑ����s
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v���DB�ڑ����s"))
                Exit Try
            End If

            Dim key As LIST_KEY = Nothing
            Dim val As TYP_MAP_LIST_VALUE = Nothing

            Dim strTable() As String = {DB_DEFECT_INF_T, DB_DEFECT_INF_B}           ' �r�e�[�u���ꗗ
            Dim nCount As Integer = 0                                               ' �擾���
            For ii As Integer = 0 To strTable.Length - 1
                '---------------------------------------------------------------
                ' �r�p�k�쐬
                '---------------------------------------------------------------
                Dim strSQL As String = ""
                strSQL &= "SELECT"
                strSQL &= " d.* "
                strSQL &= " ,INF.�ޗ��� as �R�C����"
                strSQL &= " FROM"
                strSQL &= " " & strTable(ii)
                strSQL &= " d "

                '' �����R�C��
                strSQL &= "INNER JOIN " & DB_COIL_INF & " INF on INF.�Ǘ�No=d.�Ǘ�No "

                If bFFSnd Then
                    strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " adf "
                End If

                strSQL &= " WHERE"
                strSQL &= " d.�Ǘ�No ='" & strKanriNo & "'"
                strSQL &= " AND d.�d������ <> 1 "

                If bFFSnd Then
                    strSQL &= "AND ((adf.�Ǘ�No = d.�Ǘ�No) AND (adf.�rNo=d.�rNo)) "
                ElseIf bTansho Then
                    strSQL &= " AND d.�\���Ώۋ敪_�T����� = 1 "
                Else
                    For jj As Integer = 0 To nLstTid.Count - 1                      ' �r��ݒ�
                        strSQL &= " AND NOT d.�����r�� = " & CStr(nLstTid(jj))
                    Next

                    For jj As Integer = 0 To nLstGid.Count - 1                      ' �O���[�h�ݒ�
                        strSQL &= " AND NOT d.����O���[�h = " & CStr(nLstGid(jj))
                    Next
                End If

                '---------------------------------------------------------------
                ' ORDER �� (�r�ʐρA����ʒu���㏟���ɂ���)
                '---------------------------------------------------------------
                strSQL &= " ORDER BY d.�r�ʐ� ASC,"                             ' ���������̂��珇�Ɋi�[���Ă���
                If bNagate = False Then                                         ' ����������]����
                    strSQL &= " d.����ʒu DESC"                                ' ���[�ʒu���珇�Ɋi�[���Ă���

                Else                                                            ' ����������]�L��
                    strSQL &= " d.����ʒu ASC"                                 ' ��[�ʒu���珇�Ɋi�[���Ă���
                End If
                strSQL &= " ,d.�rNo DESC"                                       ' ��[�ʒu���珇�Ɋi�[���Ă���

                '---------------------------------------------------------------
                ' ���s
                '---------------------------------------------------------------
                read = clsDB.SelectExecute(strSQL)
                If read Is Nothing Then
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v���擾SQL���s���s SQL={0}", strSQL))
                    Exit Try
                End If

                While read.Read() = True
                    If Not m_bEnaRead Then
                        ' �ǂݍ��݃L�����Z��
                        Return False
                    End If

                    Dim nMen As Integer = 0
                    Dim nKizuNo As Integer = 0
                    Dim nTid As Integer = 0
                    Dim nGid As Integer = 0
                    Dim nLen As Integer = 0
                    Dim nAria As Integer = 0
                    Dim nDSWS As Integer = 0
                    Dim nDS As Single = 0
                    Dim nWS As Single = 0
                    Dim typDefDt As DETECT_BASE_DATA = Nothing
                    typDefDt.initialize()

                    '-----------------------------------------------------------
                    ' �ǂݍ��݃f�[�^�擾
                    '-----------------------------------------------------------
                    With read
                        typDefDt.TorB = CUShort(.GetInt32(.GetOrdinal("�\���敪")))
                        typDefDt.nTid = CUShort(.GetInt32(.GetOrdinal("�����r��")))
                        typDefDt.nGid = CUShort(.GetInt32(.GetOrdinal("����O���[�h")))
                        typDefDt.cycle_div = CUShort(.GetInt32(.GetOrdinal("�����t���O")))
                        typDefDt.dsws = CUShort(.GetInt32(.GetOrdinal("DSWS�敪")))
                        typDefDt.camset = CUShort(.GetInt32(.GetOrdinal("�J�����Z�b�g")))
                        typDefDt.nKizuNo = .GetInt32(.GetOrdinal("�rNo"))
                        typDefDt.y = CSng(.GetInt32(.GetOrdinal("����ʒu")))
                        typDefDt.nWidDiv = CShort(.GetInt32(.GetOrdinal("�������敪")))
                        typDefDt.nWidAbsPos = .GetInt32(.GetOrdinal("��������Έʒu"))

                        If bCorrect Then
                            typDefDt.y = CInt(typDefDt.y * m_typCoilInf.dCorrect)
                        End If


                        If bNagate Then
                            ' ����������]�̂��߁AY�ʒu���R�C��������̍����Ƃ���
                            typDefDt.y = CoilLength - typDefDt.y
                            If typDefDt.y < 0 Then
                                typDefDt.y = 0
                            End If
                        End If

						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
						typDefDt.y_out = GetOutLen(typDefDt.y, bNagate, CoilLength)
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

						If bDS Then
                            typDefDt.ds = .GetFloat(.GetOrdinal("WS�ʒu"))
                            typDefDt.ws = .GetFloat(.GetOrdinal("DS�ʒu"))
                            If 0 = typDefDt.dsws Then
                                typDefDt.dsws = 1
                            Else
                                typDefDt.dsws = 0
                            End If
                            typDefDt.nWidDiv = CShort((MAP_COL_NUM - 1) - typDefDt.nWidDiv)
                        Else
                            typDefDt.ds = .GetFloat(.GetOrdinal("DS�ʒu"))
                            typDefDt.ws = .GetFloat(.GetOrdinal("WS�ʒu"))
                        End If

                        ' �Z���^�[����̋������Z�o
                        Dim nWid As Integer = .GetInt32(.GetOrdinal("�R�C����"))
                        'Dim nXmax As Integer = CInt(nWid / 2)
                        'Dim nXmin As Integer = nXmax * -1

                        typDefDt.dsc = CSng(nWid) / CSng(2.0) - typDefDt.ds
                        typDefDt.wsc = CSng(nWid) / CSng(2.0) - typDefDt.ws
                        'If 0 = typDefDt.dsws Then
                        '    ' DS�
                        '    typDefDt.dsc = nXmax - typDefDt.ds
                        '    typDefDt.wsc = nXmin + typDefDt.ds
                        'Else
                        '    ' WS�
                        '    typDefDt.wsc = nXmax - typDefDt.ws
                        '    typDefDt.dsc = nXmin + typDefDt.ws
                        'End If

                        typDefDt.speed = .GetInt32(.GetOrdinal("���C�����x"))
                        typDefDt.pitch = .GetInt32(.GetOrdinal("�s�b�`"))
                        typDefDt.roolkei = .GetFloat(.GetOrdinal("�a"))

                        typDefDt.aria = .GetFloat(.GetOrdinal("�r�ʐ�"))
                        typDefDt.kizu_wid = .GetInt32(.GetOrdinal("�r��"))
                        typDefDt.kizu_len = .GetInt32(.GetOrdinal("�r����"))

                        typDefDt.h_res = .GetFloat(.GetOrdinal("�c����\"))
                        typDefDt.w_res = .GetFloat(.GetOrdinal("������\"))
                        If 0.0 >= typDefDt.h_res Then typDefDt.h_res = 0.1
                        If 0.0 >= typDefDt.w_res Then typDefDt.w_res = 0.1

                        '' �t���[��No�̌���́A���O�ɓǂݍ��񂾃t���[�������g�p����
                        'typDefDt.nFrameNo = GetFrameImage(.GetString(.GetOrdinal("�Ǘ�No")), .GetInt32(.GetOrdinal("�t���[��No")), .GetInt32(.GetOrdinal("�J�����Z�b�g")))

                        typDefDt.nFrameNo = .GetInt32(.GetOrdinal("�t���[��No"))

                        typDefDt.frm_x_min = CShort(.GetInt32(.GetOrdinal("�t���[���O��Xmin")))
                        typDefDt.frm_x_max = CShort(.GetInt32(.GetOrdinal("�t���[���O��Xmax")))
                        typDefDt.frm_y_min = CShort(.GetInt32(.GetOrdinal("�t���[���O��Ymin")))
                        typDefDt.frm_y_max = CShort(.GetInt32(.GetOrdinal("�t���[���O��Ymax")))

                        typDefDt.cull_frm_x_min = CShort(.GetInt32(.GetOrdinal("�Ԉ����t���[���O��Xmin")))
                        typDefDt.cull_frm_x_max = CShort(.GetInt32(.GetOrdinal("�Ԉ����t���[���O��Xmax")))
                        typDefDt.cull_frm_y_min = CShort(.GetInt32(.GetOrdinal("�Ԉ����t���[���O��Ymin")))
                        typDefDt.cull_frm_y_max = CShort(.GetInt32(.GetOrdinal("�Ԉ����t���[���O��Ymax")))

                        typDefDt.box_x_min = CShort(.GetInt32(.GetOrdinal("�O��Xmin")))
                        typDefDt.box_x_max = CShort(.GetInt32(.GetOrdinal("�O��Xmax")))
                        typDefDt.box_y_min = CShort(.GetInt32(.GetOrdinal("�O��Ymin")))
                        typDefDt.box_y_max = CShort(.GetInt32(.GetOrdinal("�O��Ymax")))

                        typDefDt.center_x = CShort(.GetInt32(.GetOrdinal("�r���S�ʒuX")))
                        typDefDt.center_y = CShort(.GetInt32(.GetOrdinal("�r���S�ʒuY")))
                        typDefDt.h_size = CUShort(.GetInt32(.GetOrdinal("�c��f��")))
                        typDefDt.w_size = CUShort(.GetInt32(.GetOrdinal("����f��")))

                        typDefDt.nTyuu = CUShort(mcls_Param.GetTypePriority(typDefDt.nTid))
                        typDefDt.nGyuu = CUShort(mcls_Param.GetGradePriority(typDefDt.nGid))


                        typDefDt.base_box_x_min = CShort(.GetInt32(.GetOrdinal("���摜�O��Xmin")))
                        typDefDt.base_box_x_max = CShort(.GetInt32(.GetOrdinal("���摜�O��Xmax")))
                        typDefDt.base_box_y_min = CShort(.GetInt32(.GetOrdinal("���摜�O��Ymin")))
                        typDefDt.base_box_y_max = CShort(.GetInt32(.GetOrdinal("���摜�O��Ymax")))

                        typDefDt.base_center_x = CShort(.GetInt32(.GetOrdinal("���摜�r���S�ʒuX")))
                        typDefDt.base_center_y = CShort(.GetInt32(.GetOrdinal("���摜�r���S�ʒuY")))
                        typDefDt.base_h_size = CUShort(.GetInt32(.GetOrdinal("���摜�c��f��")))
                        typDefDt.base_w_size = CUShort(.GetInt32(.GetOrdinal("���摜����f��")))

                        typDefDt.base_h_res = .GetFloat(.GetOrdinal("���c����\"))
                        typDefDt.base_w_res = .GetFloat(.GetOrdinal("��������\"))
                        If 0.0 >= typDefDt.base_h_res Then typDefDt.base_h_res = 0.1
                        If 0.0 >= typDefDt.base_w_res Then typDefDt.base_w_res = 0.1
                        typDefDt.nShrinkRatio = CUShort(.GetInt32(.GetOrdinal("�k����")))

                        typDefDt.nObjTansho = CShort(.GetInt32(.GetOrdinal("�\���Ώۋ敪_�T�����")))

                        nKizuNo = .GetInt32(.GetOrdinal("�rNo"))

                        nMen = typDefDt.TorB
                        nLen = CInt(typDefDt.y)
                        nAria = CInt(typDefDt.aria)
                        nTid = typDefDt.nTid
                        nGid = typDefDt.nGid
                        nDSWS = typDefDt.dsws
                        nDS = typDefDt.ds
                        nWS = typDefDt.ws
                    End With

                    If nKizuNo < 0 Then ' �rNo�s���Ȃ�Ύ���
                        Continue While
                    End If

                    ' �ڍ��r���z��Z�b�g
                    If m_dctDefInf(ii).ContainsKey(New DEFECT_KEY(nKizuNo)) = False Then
                        m_dctDefInf(ii).Add(New DEFECT_KEY(nKizuNo), typDefDt)
                    Else
                        mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("�ڍ��r���z��Z�b�g�ُ� �rNo�d�� [�rNo={0} ��={1}]", nKizuNo, ii))
                    End If

                    '-----------------------------------------------------------
                    ' ��ԍ��̌���
                    '-----------------------------------------------------------
                    Dim nCol As Integer = 0
                    nCol = typDefDt.nWidDiv

                    If m_nColStart > nCol Then                                  ' �R�C�����𒴂��邱�Ƃ�����
                        nCol = m_nColStart
                    ElseIf nCol > m_nColEnd Then                                ' �R�C�����𒴂��邱�Ƃ�����
                        nCol = m_nColEnd
                    End If

                    '-----------------------------------------------------------
                    ' �\���Ԋu���Ƀf�[�^���i�[����
                    '-----------------------------------------------------------
                    For stage As Integer = 0 To m_stMap.stage.Length - 1       ' �e�X�e�[�W�ɐݒ�
                        Dim nRow As Integer = 0
                        nRow = (nLen \ 1000) \ Interval(CType(stage, EM_LONG_POS))

                        If nRow < 0 Then                                        ' �s���z�񒷂𒴂����ꍇ
                            nRow = 0
                        ElseIf nRow > m_stMap.stage(stage).men(nMen).row.Length - 1 Then
                            nRow = m_stMap.stage(stage).men(nMen).row.Length - 1
                        End If


                        '�ʏ��r
                        With m_stMap.stage(stage).men(nMen).row(nRow).col(nCol)
                            If .nKizuNo > 0 Then                                    ' �����r���ݒ肳��Ă���ꍇ�͑�\�r���������
                                ' �擾���ɖʐςƒ���ʒu�̔�r�ς̂��߁A�r��A�O���[�h�̗D��x�̂�
                                Dim cmp As Integer = mcls_Param.CompDefPriority(.nTid, nTid, .nGid, nGid)
                                If cmp >= 0 Then                                ' �㏟���ɐݒ肷��
                                    .strKanriNo = strKanriNo
                                    .nKizuNo = nKizuNo
                                    .nTid = CShort(nTid)
                                    .nGid = CShort(nGid)
                                End If
                            Else                                                ' ����o�^���͂��̂܂ܐݒ�
                                .strKanriNo = strKanriNo
                                .nKizuNo = nKizuNo
                                .nTid = CShort(nTid)
                                .nGid = CShort(nGid)
                            End If
                        End With

                        With key
                            .nCol = nCol
                            .nRow = nRow
                            .nMen = nMen
                            .emStage = CType(stage, EM_LONG_POS)
                        End With
                        With val
                            .nPnt = .nKizuNo
                            .nKizuNo = nKizuNo
                            .nGid = nGid
                            .nTid = nTid
                            .nTYuu = typDefDt.nTyuu
                            .nGYuu = typDefDt.nGyuu
                            .nAria = nAria
                            .nY = nLen
                            .nCycDiv = typDefDt.cycle_div
                        End With
                        AddDefLst(key, val)                                     ' �r���X�g�f�[�^�ɐݒ肷��

                        '-----------------------------------
                        Dim kizuNokey As KIZU_KEY = Nothing
                        With kizuNokey
                            .nKizuNo = nKizuNo
                            .nStage = stage
                        End With
                        If m_dctDefKey.ContainsKey(kizuNokey) = False Then
                            m_dctDefKey.Add(kizuNokey, key)
                        Else
                            mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("�r���e�[�u���ǂݍ��ُ݈�(�ʏ�) �rNo�d�� [�rNo={0} MAP�Ԋu�敪={1}]", kizuNokey.nKizuNo, kizuNokey.nStage))
                        End If

                        '-----------------------------------
                        m_stMap.stage(stage).men(ii).row(nRow).nKizuCnt += 1

                    Next

                    nCount += 1
                    If nCount Mod 100 = 0 Then
                        RaiseEvent ReadDataNti(DATA_KIND.emCOIL_INF, nCount)            ' �ǂݍ��݌����ʒm
                        Application.DoEvents()
                    End If

                End While

                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If

            Next

            ' �ǂݍ��ݐ����Ȃ̂ŁA�߂�l��True���Z�b�g
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���e�[�u���ǂݍ��ُ݈� [{0}]", ex.Message))
        Finally
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �}�b�v���ǂݍ���
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="bNagate">���蔽�] True:���]���� False:�Ȃ�</param>
    ''' <param name="bDS">DS�ʒu���] True:���](�E��DS) False:�Ȃ�(����DS)</param>
    ''' <returns>True:�ǂݍ��ݐ��� False:�ǂݍ��ݎ��s</returns>
    ''' <remarks></remarks>
    Private Function ReadPreMapInf(
      ByVal strKanriNo As String,
      ByVal bNagate As Boolean,
      ByVal bDS As Boolean) As Boolean

        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim bRet As Boolean = False

        Try

            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If Not bDbRet Then                                          ' DB�ڑ����s
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�}�b�v���DB�ڑ����s"))
                Exit Try
            End If

            Dim key As LIST_KEY = Nothing
            Dim val As TYP_MAP_LIST_VALUE = Nothing

            Dim nCount As Integer = 0                                               ' �擾���
            For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                '---------------------------------------------------------------
                ' �r�p�k�쐬
                '---------------------------------------------------------------
                Dim strSQL As String = ""
                strSQL &= "SELECT"
                strSQL &= " ff.�Ǘ�No"
                strSQL &= ", ff.�rNo"
                strSQL &= ", ff.�H�����"
                strSQL &= ", ff.�\���敪"
                strSQL &= ", ff.���O�ҏW�t���O"
                strSQL &= ", ff.�O�H���Ǘ�No"
                strSQL &= ", ff.�O�H���rNo"
                strSQL &= ", ff.FF�r��"
                strSQL &= ", ff.FF�O���[�h"
                strSQL &= ", ff.FF�r���o�H��"
                strSQL &= ", ff.FF�r�敪"
                strSQL &= ", ff.FF�r���������ʒu"
                strSQL &= ", ff.FF�r�Е����ʒu"
                strSQL &= ", ff.FF�r����"
                strSQL &= ", ff.�����r��"
                strSQL &= ", ff.����O���[�h"
                strSQL &= ", ff.�ŏI����ʒu"

                'strSQL &= ", def.�Ǘ�No"
                'strSQL &= ", def.�rNo"
                'strSQL &= ", def.�H�����"
                'strSQL &= ", def.�\���敪"
                strSQL &= ", def.�����t���O"
                strSQL &= ", def.����ʒu"
                strSQL &= ", def.DSWS�敪"
                strSQL &= ", def.DS�ʒu"
                strSQL &= ", def.WS�ʒu"
                strSQL &= ", def.���C�����x"
                strSQL &= ", def.�s�b�`"
                strSQL &= ", def.�a"
                strSQL &= ", def.�r��"
                strSQL &= ", def.�r����"
                strSQL &= ", def.�c����\"
                strSQL &= ", def.������\"
                strSQL &= ", def.�O��Xmin"
                strSQL &= ", def.�O��Xmax"
                strSQL &= ", def.�O��Ymin"
                strSQL &= ", def.�O��Ymax"
                strSQL &= ", def.�r���S�ʒuX"
                strSQL &= ", def.�r���S�ʒuY"
                strSQL &= ", def.�c��f��"
                strSQL &= ", def.����f��"
                strSQL &= ", def.�摜�T�C�Y1"
                strSQL &= ", def.�摜�T�C�Y2"
                strSQL &= ", def.�摜�T�C�Y3"
                strSQL &= ", def.�摜�T�C�Y4"

                'strSQL &= ", lin.�Ǘ�No"
                'strSQL &= ", lin.�H�����"
                'strSQL &= ", lin.�\���敪"
                strSQL &= ", lin.�����X�e�[�^�X"

                'strSQL &= ", mst.�O�H���R�[�h"
                strSQL &= ", mst.�L������"
                strSQL &= ", mst.�O�H���\�����[�h"
                strSQL &= ", mst.�摜�`��"


                strSQL &= " FROM "
                strSQL &= DB_MAE_FF_INF '// �O�H���e�e�r���[ff]
                strSQL &= " ff"

                strSQL &= " INNER JOIN "
                strSQL &= DB_MAE_DEFECT_INF '// �O�H���r���[def]
                strSQL &= " def"
                strSQL &= " ON"
                strSQL &= " ff.�Ǘ�No=def.�Ǘ�No"
                strSQL &= " AND ff.�rNo=def.�rNo"
                strSQL &= " AND ff.�H�����=def.�H�����"
                strSQL &= " AND ff.�\���敪=def.�\���敪"

                strSQL &= " INNER JOIN "
                strSQL &= DB_MAE_COIL_LINE '// �O�H�����C���ʃR�C�����[lin]
                strSQL &= " lin"
                strSQL &= " ON"
                strSQL &= " ff.�Ǘ�No=lin.�Ǘ�No"
                strSQL &= " AND ff.�H�����=lin.�H�����"
                strSQL &= " AND ff.�\���敪=lin.�\���敪"

                strSQL &= " INNER JOIN "
                strSQL &= DB_MAE_MASTER '// �O�H���}�X�^�[[mst]
                strSQL &= " mst"
                strSQL &= " ON"
                strSQL &= " ff.FF�r���o�H��=mst.�O�H���R�[�h"
                strSQL &= " WHERE"
                strSQL &= " ff.�Ǘ�No="
                strSQL &= "'" & strKanriNo & "'"
                        strSQL &= " AND"
                strSQL &= " ff.�\���敪="
                strSQL &= ii
                strSQL &= " ORDER BY"
                strSQL &= " ff.�H����� ASC, ff.�rNo ASC"

                '---------------------------------------------------------------
                ' ���s
                '---------------------------------------------------------------
                read = clsDB.SelectExecute(strSQL)
                If read Is Nothing Then
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v���擾SQL���s���s SQL={0}", strSQL))
                    Exit Try
                End If

                While read.Read() = True
                    If Not m_bEnaRead Then
                        ' �ǂݍ��݃L�����Z��
                        Return False
                    End If


                    Dim nMen As Integer = 0
                    Dim nKizuNo As Integer = 0
                    Dim nTid As Integer = 0
                    Dim nGid As Integer = 0
                    Dim nLen As Integer = 0
                    Dim nAria As Integer = 0
                    Dim strProc As String = ""
                    Dim strTid As String = ""
                    Dim strGid As String = ""


                    Dim typDefDt As TYP_MAE_DEFECT_DATA = Nothing
                    typDefDt.initialize()

                    '-----------------------------------------------------------
                    ' �ǂݍ��݃f�[�^�擾
                    '-----------------------------------------------------------
                    With read


                        Dim nCheck As Integer = .GetInt32(.GetOrdinal("�L������"))
                        Dim nMaeDsp As Integer = .GetInt32(.GetOrdinal("�O�H���\�����[�h"))
                        Dim nStatus As Integer = .GetInt32(.GetOrdinal("�����X�e�[�^�X"))

                        If 1 <> nCheck Then Continue While
                        If 1 <> nMaeDsp And 2 <> nMaeDsp Then Continue While
                        If 0 <> nStatus Then Continue While

                        typDefDt.ff.cMaeKizukenNo = tMod_Common.StringToByte(.GetString(.GetOrdinal("�O�H���Ǘ�No")))
                        typDefDt.ff.nMaeKizuNo = .GetInt32(.GetOrdinal("�O�H���rNo"))
                        typDefDt.ff.cT = tMod_Common.StringToByte(.GetString(.GetOrdinal("FF�r��")))
                        strTid = .GetString(.GetOrdinal("FF�r��"))
                        typDefDt.ff.cG = tMod_Common.StringToByte(.GetString(.GetOrdinal("FF�O���[�h")))
                        strGid = .GetString(.GetOrdinal("FF�O���[�h"))
                        typDefDt.ff.cProc = tMod_Common.StringToByte(.GetString(.GetOrdinal("FF�r���o�H��")))
                        strProc = .GetString(.GetOrdinal("FF�r���o�H��"))
                        typDefDt.ff.nDiv = .GetInt32(.GetOrdinal("FF�r�敪"))
                        typDefDt.ff.nPos = .GetInt32(.GetOrdinal("FF�r���������ʒu"))
                        typDefDt.ff.nWidth = .GetInt32(.GetOrdinal("FF�r�Е����ʒu"))
                        typDefDt.ff.nLen = .GetInt32(.GetOrdinal("FF�r����"))
                        typDefDt.ff.nY = .GetInt32(.GetOrdinal("�ŏI����ʒu"))
                        typDefDt.ff.nTid = CShort(.GetInt32(.GetOrdinal("�����r��")))
                        typDefDt.ff.nGid = CShort(.GetInt32(.GetOrdinal("����O���[�h")))

                        typDefDt.data.nImageFlg = CShort(.GetInt32(.GetOrdinal("�摜�`��")))
                        typDefDt.data.nDspMode = CShort(.GetInt32(.GetOrdinal("���O�ҏW�t���O")))
                        typDefDt.data.men = CUShort(.GetInt32(.GetOrdinal("�\���敪")))
                        typDefDt.data.cycle_div = CUShort(.GetInt32(.GetOrdinal("�����t���O")))
                        typDefDt.data.dsws = CUShort(.GetInt32(.GetOrdinal("DSWS�敪")))

                        typDefDt.data.speed = CUShort(.GetInt32(.GetOrdinal("���C�����x")))
                        typDefDt.data.pitch = CUShort(.GetInt32(.GetOrdinal("�s�b�`")))
                        typDefDt.data.roolkei = .GetFloat(.GetOrdinal("�a"))

                        typDefDt.data.box_wid = CShort(.GetInt32(.GetOrdinal("�r��")))
                        typDefDt.data.box_len = CShort(.GetInt32(.GetOrdinal("�r����")))


                        typDefDt.data.h_res = .GetFloat(.GetOrdinal("�c����\"))
                        typDefDt.data.w_res = .GetFloat(.GetOrdinal("������\"))

                        typDefDt.data.box_x_min = CShort(.GetInt32(.GetOrdinal("�O��Xmin")))
                        typDefDt.data.box_x_max = CShort(.GetInt32(.GetOrdinal("�O��Xmax")))
                        typDefDt.data.box_y_min = CShort(.GetInt32(.GetOrdinal("�O��Ymin")))
                        typDefDt.data.box_y_max = CShort(.GetInt32(.GetOrdinal("�O��Ymax")))

                        typDefDt.data.center_x = CShort(.GetInt32(.GetOrdinal("�r���S�ʒuX")))
                        typDefDt.data.center_y = CShort(.GetInt32(.GetOrdinal("�r���S�ʒuY")))

                        typDefDt.data.h_size = CUShort(.GetInt32(.GetOrdinal("�c��f��")))
                        typDefDt.data.w_size = CUShort(.GetInt32(.GetOrdinal("����f��")))

                        typDefDt.strKanriNo = strKanriNo
                        typDefDt.nKizuNo = .GetInt32(.GetOrdinal("�rNo"))
                        nKizuNo = typDefDt.nKizuNo

                        ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
                        typDefDt.ff.nKouteiID = .GetInt32(.GetOrdinal("�H�����"))
                        ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

                        If bNagate Then
                            ' ����������]�̂��߁AY�ʒu���R�C��������̍����Ƃ���
                            typDefDt.ff.nY = CoilLength - typDefDt.ff.nY
                            If typDefDt.ff.nY < 0 Then
                                typDefDt.ff.nY = 0
                            End If
                        End If

						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
						typDefDt.ff.nY_Out = CInt(GetOutLen(CInt(typDefDt.ff.nY), bNagate, CoilLength))
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

						nMen = typDefDt.data.men
                        nLen = typDefDt.ff.nY
                        nAria = 0
                        nTid = typDefDt.ff.nTid
                        nGid = typDefDt.ff.nGid


                        ' �Z���^�[����̋������Z�o
                    End With

                    If nKizuNo < 0 Then ' �rNo�s���Ȃ�Ύ���
                        Continue While
                    End If

                    ' �ڍ��r���z��Z�b�g
                    If m_dctDefTcmInf(ii).ContainsKey(New DEFECT_KEY(nKizuNo)) = False Then
                        m_dctDefTcmInf(ii).Add(New DEFECT_KEY(nKizuNo), typDefDt)
                    Else
                        mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("�ڍ��r���z��Z�b�g�ُ�(TCM) �rNo�d�� [�rNo={0} ��={1}]", nKizuNo, ii))
                    End If

                    '-----------------------------------------------------------
                    ' ��ԍ��̌���
                    '-----------------------------------------------------------
                    Dim nCol As Integer = 0

                    '-----------------------------------------------------------
                    ' �\���Ԋu���Ƀf�[�^���i�[����
                    '-----------------------------------------------------------
                    For stage As Integer = 0 To m_stMap.stage.Length - 1       ' �e�X�e�[�W�ɐݒ�
                        Dim nRow As Integer = 0
                        nRow = (nLen \ 1000) \ Interval(CType(stage, EM_LONG_POS))

                        If nRow < 0 Then                                        ' �s���z�񒷂𒴂����ꍇ
                            nRow = 0
                        ElseIf nRow > m_stMap.stage(stage).men(nMen).row.Length - 1 Then
                            nRow = m_stMap.stage(stage).men(nMen).row.Length - 1
                        End If

                        '�����r
                        With m_stMap.stage(stage).men(nMen).row(nRow).tcm(0)
                            If .nKizuNo > 0 Then                                    ' �����r���ݒ肳��Ă���ꍇ�͑�\�r���������
                                ' �擾���ɖʐςƒ���ʒu�̔�r�ς̂��߁A�r��A�O���[�h�̗D��x�̂�
                                Dim bcmp As Boolean = mcls_PreParam.CheckPriority(.strProc, strProc, .strFFType, strTid, .strFFGrade, strGid)
                                If bcmp Then                                ' �㏟���ɐݒ肷��
                                    .strKanriNo = strKanriNo
                                    .nKizuNo = nKizuNo
                                    .strProc = strProc
                                    .strFFType = strTid
                                    .strFFGrade = strGid
                                End If
                            Else                                                ' ����o�^���͂��̂܂ܐݒ�
                                .strKanriNo = strKanriNo
                                .nKizuNo = nKizuNo
                                .strProc = strProc
                                .strFFType = strTid
                                .strFFGrade = strGid
                            End If
                        End With

                        With key
                            .nCol = 0
                            .nRow = nRow
                            .nMen = nMen
                            .emStage = CType(stage, EM_LONG_POS)
                        End With
                        With val
                            .nPnt = .nKizuNo
                            .nKizuNo = nKizuNo
                            .nGid = nGid
                            .nTid = nTid
                            .nAria = nAria
                            .nY = nLen
                            .strFFType = strTid
                            .strFFGrade = strGid
                            .strProc = strProc
                            .nTYuu = mcls_PreParam.GetTypePriority(strProc, strTid)
                            .nGYuu = mcls_PreParam.GetGradePriority(strProc, strGid)
                            .nCycDiv = 0
                            ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
                            .nKouteiID = typDefDt.ff.nKouteiID
                            ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

                        End With
                        AddDefTcmLst(key, val)                                     ' �r���X�g�f�[�^�ɐݒ肷��

                        '-----------------------------------
                        Dim kizuNokey As KIZU_KEY = Nothing
                        With kizuNokey
                            .nKizuNo = nKizuNo
                            .nStage = stage
                        End With
                        If m_dctDefTcmKey.ContainsKey(kizuNokey) = False Then
                            m_dctDefTcmKey.Add(kizuNokey, key)
                        Else
                            mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("�r���e�[�u���ǂݍ��ُ݈�(TCM) �rNo�d�� [�rNo={0} MAP�Ԋu�敪={1}]", kizuNokey.nKizuNo, kizuNokey.nStage))
                        End If
                        '�����r���r���ɃJ�E���g���Ȃ��B
                        '-----------------------------------
                    Next

                    nCount += 1
                    RaiseEvent ReadDataNti(DATA_KIND.emCOIL_INF, nCount)            ' �ǂݍ��݌����ʒm
                    Application.DoEvents()

                End While

                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If

            Next

            ' �ǂݍ��ݐ����Ȃ̂ŁA�߂�l��True���Z�b�g
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���e�[�u���ǂݍ��ُ݈� [{0}]", ex.Message))
        Finally
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �J�b�g��񐔂��擾���܂�
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="nCount">�J�b�g��񌏐�</param>
    ''' <returns>True:�擾���� False:�擾���s</returns>
    ''' <remarks></remarks>
    Private Function GetCutInfCount(ByVal strKanriNo As String, ByRef nCount As Integer) As Boolean
        Try
            '-------------------------------------------------------------------
            ' �r�p�k�쐬
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " COUNT(*)"
            strSQL &= " FROM"
            strSQL &= " " & DB_COIL_CUT_POS
            strSQL &= " WHERE"
            strSQL &= " �Ǘ�No='" & strKanriNo & "'"

            '-------------------------------------------------------------------
            ' ���s
            '-------------------------------------------------------------------
            Dim bRet As Boolean = ExecuteSelectCount(strSQL, nCount)
            If bRet = False Then
                Return False
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�J�b�g��񐔎擾�ُ� [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function


    ''' <summary>
    ''' �J�b�g����ǂݍ��݂܂�
    ''' </summary>
    ''' <param name="strKizukenNo">�Ǘ�No</param>
    ''' <param name="bNagate">True:��������̔��]���� False:���]�Ȃ�</param>
    ''' <param name="bCorrect">�␳���g�p(True:����AFalse:�Ȃ�)</param>
    ''' <returns>True:�擾���� False:�擾���s</returns>
    ''' <remarks></remarks>
    Private Function ReadCutInf(ByVal strKizukenNo As String, ByVal bNagate As Boolean, ByVal bCorrect As Boolean) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing

        Try
            '-------------------------------------------------------------------
            ' �r�p�k�쐬
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " *"
            strSQL &= " FROM"
            strSQL &= " " & DB_COIL_CUT_POS
            strSQL &= " WHERE"
            strSQL &= " �Ǘ�No='" & strKizukenNo & "'"
            strSQL &= " AND �擪�ʒu > 0"
            strSQL &= " ORDER BY �����q��"

            '-------------------------------------------------------------------
            ' ���s
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                              ' DB�ڑ����s
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�J�b�g���DB�ڑ����s"))
                Return False
            End If

            read = clsDB.SelectExecute(strSQL)
            If read Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�J�b�g���擾SQL���s���s SQL={0}", strSQL))
                Return False
            End If

            '-------------------------------------------------------------------
            ' �i�[
            '-------------------------------------------------------------------
            m_listCutInf.Clear()
            Dim inf As CUT_INF_DATA = Nothing
            Dim nCount As Integer = 0
            While read.Read() = True
                If m_bEnaRead = False Then                                      ' �ǂݍ��ݕs�̏ꍇ
                    Return False
                End If

                With read
                    inf.nKubun = .GetInt32(.GetOrdinal("�敪"))
                    Dim dPosS As Integer = .GetInt32(.GetOrdinal("�擪�ʒu"))
                    Dim nPosS As Integer

                    If bCorrect Then
                        '����ʒu�ɕ␳������������
                        dPosS = CInt(dPosS * m_typCoilInf.dCorrect)
                    End If

                    If bNagate Then
						' ���蔽�]�̏ꍇ
						inf.dPosS = CSng((CoilLength - dPosS) / 1000)
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
						inf.dPosS_MM = CoilLength - dPosS
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
						nPosS = (CoilLength - dPosS) \ 1000
                    Else
						inf.dPosS = CSng(dPosS / 1000)
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
						inf.dPosS_MM = dPosS
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
						nPosS = dPosS \ 1000
                    End If
                    m_listCutInf.Add(inf)

                    Dim nRow As Integer = nPosS \ Interval(0)
                    Dim nMaxRow As Integer = m_stMap.stage(0).men(0).row.Length - 1

                    If 0 <= nRow And nMaxRow >= nRow Then
                        For ii As Integer = 0 To m_stMap.stage(0).men.Length - 1
                            If 1 = inf.nKubun Then
                                m_stMap.stage(0).men(ii).row(nRow).nCut = 1
                            End If
                            If 4 = inf.nKubun Then
                                m_stMap.stage(0).men(ii).row(nRow).bYousetu = True
                            End If
                        Next
                    End If
                End With

                nCount += 1
                RaiseEvent ReadDataNti(DATA_KIND.emCUT_INF, nCount)             ' �ǂݍ��݌����ʒm
                Application.DoEvents()
            End While

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�J�b�g���擾�ُ� [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function


    ''' <summary>
    ''' �ǂݍ��ݏI������
    ''' </summary>
    ''' <param name="bNagate">True:��������̔��]���� False:���]�Ȃ�</param>
    ''' <remarks></remarks>
    Private Sub ReadFinalize(ByVal bNagate As Boolean)
        Try
            Dim nOutTopPos As Integer = m_typCoilInf.nOutTopPos \ 1000                        ' �o���P�ʐ�[�ʒu[m]

            For nMen As Integer = 0 To m_stMap.stage(0).men.Length - 1

                Dim nRowMax As Integer = CoilLength \ 1000 \ Interval(0)
                If nRowMax >= m_stMap.stage(0).men(nMen).row.Length Then
                    nRowMax = m_stMap.stage(0).men(nMen).row.Length - 1
                End If

                For nRow As Integer = 0 To nRowMax
                    For nStage As Integer = 1 To MAP_STAGE_NUM - 1

                        Dim nSetRow As Integer = (nRow * Interval(0)) \ Interval(CType(nStage, EM_LONG_POS))

                        '-------------------------------------------------------
                        ' �}�b�v�ʒu���̑S�X�e�[�W���f
                        '-------------------------------------------------------
                        Dim nKensa As EM_DIV_KENSA = m_stMap.stage(0).men(nMen).row(nRow).emKensa
                        Dim nKiki As EM_DIV_KIKI = m_stMap.stage(0).men(nMen).row(nRow).emKiki
                        Dim bYosetsu As Boolean = m_stMap.stage(0).men(nMen).row(nRow).bYousetu
                        Dim nCut As Integer = m_stMap.stage(0).men(nMen).row(nRow).nCut

                        With m_stMap.stage(nStage).men(nMen).row(nSetRow)
                            If nKensa = EM_DIV_KENSA.DIV_KENSA_NON Then         ' ��~(-9)�̏ꍇ�͖ⓚ���p
                                .emKensa = nKensa
                            ElseIf .emKensa < nKensa Then                       ' ��͐��l�ɏ]��
                                If .emKensa <> EM_DIV_KENSA.DIV_KENSA_NON Then  ' ��~�͏��O
                                    .emKensa = nKensa
                                End If
                            End If
                            If .emKiki < nKiki Then                             ' �@���Ԃ̍X�V
                                .emKiki = nKiki
                            End If
                            If Not .bYousetu And bYosetsu Then
                                .bYousetu = bYosetsu
                            End If
                            If 0 = .nCut And 0 <> nCut Then
                                .nCut = nCut
                            End If
                        End With

                    Next
                Next
                '-------------------------------------------------------
                ' �o�������̐U��Ȃ���
                '-------------------------------------------------------
                For nStage As Integer = 1 To MAP_STAGE_NUM - 1
                    Dim nEndRow As Integer = CoilLength \ 1000 \ Interval(CType(nStage, EM_LONG_POS))
                    Dim nOffset As Integer = (nOutTopPos \ Interval(CType(nStage, EM_LONG_POS)) + 1) * Interval(CType(nStage, EM_LONG_POS))
                    Dim nCnt As Integer = 0
                    For nRow As Integer = 0 To nEndRow
                        With m_stMap.stage(nStage).men(nMen).row(nRow)
                            .nLen_o = nOffset + nCnt * Interval(CType(nStage, EM_LONG_POS))
                            nCnt = nCnt + 1
                            If 0 <> .nCut Then
                                nOffset = Interval(CType(nStage, EM_LONG_POS))
                                nCnt = 0
                            End If
                        End With
                    Next
                Next nStage


            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�f�[�^�ǂݍ��ݏI���ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ��񐔎擾SQL���s
    ''' </summary>
    ''' <param name="strSQL">SQL</param>
    ''' <param name="nCount">�i�[��</param>
    ''' <returns>True:����I�� False:�ُ�I��</returns>
    ''' <remarks></remarks>
    Private Function ExecuteSelectCount(ByVal strSQL As String, ByRef nCount As Integer) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Try
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                          ' DB�ڑ����s
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��񐔎擾DB�ڑ����s  SQL={0}", strSQL))
                Return False
            End If

            read = clsDB.SelectExecute(strSQL)
            If read Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��񐔎擾SQL���s���s SQL={0}", strSQL))
                Return False
            End If

            bDbRet = read.Read()
            If bDbRet = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��񐔎擾���s SQL={0}", strSQL))
                Return False
            End If

            nCount += CInt(read.GetSqlInt32(0))                             ' ����Ǝ擾

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��񐔎擾�ُ� [{0}]  SQL={0}", ex.Message, strSQL))
            Return False
        Finally
            If clsDB Is Nothing = False Then
                clsDB.Cancel()
                If read Is Nothing = False Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return True
    End Function

    ''' <summary>
    ''' �r���X�g���r��ǉ����܂�
    ''' </summary>
    ''' <remarks>�r��A�r�O���[�h�������ꍇ�́A�㏟�����ɑ}�����܂�</remarks>
    Private Sub AddDefLst(ByVal key As LIST_KEY, ByVal val As TYP_MAP_LIST_VALUE)
        Try
            Dim list As List(Of TYP_MAP_LIST_VALUE)

            If m_dctDefLst.ContainsKey(key) = True Then
                '---------------------------------------------------------------
                ' ���Ƀ}�b�v���r������ꍇ(�\�[�g�͌�Ŏ��{���邽�ߒǉ��o�^)
                '---------------------------------------------------------------
                list = m_dctDefLst(key)
                list.Add(val)

            Else
                '---------------------------------------------------------------
                ' �ŏ��̏��̏ꍇ
                '---------------------------------------------------------------
                list = New List(Of TYP_MAP_LIST_VALUE)
                list.Add(val)
                m_dctDefLst.Add(key, list)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r���X�g�r�ǉ��ُ� [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �r�}�b�v�r���X�g���擾���܂�
    ''' </summary>
    ''' <param name="nCol">��ԍ� 0�I���W��</param>
    ''' <param name="nRow">�s�ԍ� 1�I���W��</param>
    ''' <param name="nMen">0:�\�A1:��</param>
    ''' <param name="emStage">1:�\���Ԋu���A2:�\���Ԋu��</param>
    ''' <param name="list">�r�}�b�v�r���X�g</param>
    ''' <returns>True:�r���X�g�L�� False:�r���X�g�Ȃ�</returns>
    ''' <remarks></remarks>
    Public Function GetMapDefectList(ByVal nCol As Integer, ByVal nRow As Integer, ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' �����L�[�쐬
            '-------------------------------------------------------------------
            Dim key As LIST_KEY = Nothing                                       ' �����L�[
            key.nCol = nCol
            key.nRow = nRow
            key.emStage = emStage
            key.nMen = nMen

            bRet = GetMapDefectList(key, list)
            If m_dctDefLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefLst(key)                                             ' ���X�g���n��
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r���X�g�擾1�ُ� [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="key"></param>
    ''' <param name="list"></param>
    ''' <returns></returns>
    Public Function GetMapDefectList(ByVal key As LIST_KEY, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' �����L�[�쐬
            '-------------------------------------------------------------------
            If m_dctDefLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefLst(key)                                             ' ���X�g���n��
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r���X�g�擾2�ُ� [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �r���X�g(TCM)���r��ǉ����܂�
    ''' </summary>
    ''' <remarks>�r��A�r�O���[�h�������ꍇ�́A�㏟�����ɑ}�����܂�</remarks>
    Private Sub AddDefTcmLst(ByVal key As LIST_KEY, ByVal val As TYP_MAP_LIST_VALUE)
        Try
            Dim list As List(Of TYP_MAP_LIST_VALUE)

            If m_dctDefTcmLst.ContainsKey(key) = True Then
                '---------------------------------------------------------------
                ' ���Ƀ}�b�v���r������ꍇ(�\�[�g�͌�Ŏ��{���邽�ߒǉ��o�^)
                '---------------------------------------------------------------
                list = m_dctDefTcmLst(key)
                list.Add(val)

            Else
                '---------------------------------------------------------------
                ' �ŏ��̏��̏ꍇ
                '---------------------------------------------------------------
                list = New List(Of TYP_MAP_LIST_VALUE)
                list.Add(val)
                m_dctDefTcmLst.Add(key, list)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r���X�g(TCM)�r�ǉ��ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v�r���X�g(TCM)���擾���܂�
    ''' </summary>
    ''' <param name="nCol">��ԍ� 0�I���W��</param>
    ''' <param name="nRow">�s�ԍ� 1�I���W��</param>
    ''' <param name="nMen">0:�\�A1:��</param>
    ''' <param name="emStage">1:�\���Ԋu���A2:�\���Ԋu��</param>
    ''' <param name="list">�r�}�b�v�r���X�g</param>
    ''' <returns>True:�r���X�g�L�� False:�r���X�g�Ȃ�</returns>
    ''' <remarks></remarks>
    Public Function GetMapDefectTcmList(ByVal nCol As Integer, ByVal nRow As Integer, ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' �����L�[�쐬
            '-------------------------------------------------------------------
            Dim key As LIST_KEY = Nothing                                       ' �����L�[
            key.nCol = nCol
            key.nRow = nRow
            key.emStage = emStage
            key.nMen = nMen

            bRet = GetMapDefectTcmList(key, list)
            If m_dctDefTcmLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefTcmLst(key)                                             ' ���X�g���n��
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r���X�g(TCM)�擾�ُ�1 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="key"></param>
    ''' <param name="list"></param>
    ''' <returns></returns>
    Public Function GetMapDefectTcmList(ByVal key As LIST_KEY, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' �����L�[�쐬
            '-------------------------------------------------------------------
            If m_dctDefTcmLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefTcmLst(key)                                             ' ���X�g���n��
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r���X�g(TCM)�擾�ُ�2 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �r�����擾���܂�
    ''' </summary>
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <returns>�r���</returns>
    ''' <remarks></remarks>
    Public Function DefectData(ByVal nMen As Integer, ByVal key As clsDataMap.DEFECT_KEY, ByRef typ As DETECT_BASE_DATA) As Boolean
        If Not m_dctDefInf(nMen).ContainsKey(key) Then
            Return False
        End If
        typ = m_dctDefInf(nMen)(key)
        Return True
    End Function

    ''' <summary>
    ''' TCM�r�����擾���܂�
    ''' </summary>
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <returns>�r���</returns>
    ''' <remarks></remarks>
    Public Function TcmDefectData(ByVal nMen As Integer, ByVal key As clsDataMap.DEFECT_KEY, ByRef typ As TYP_MAE_DEFECT_DATA) As Boolean
        If Not m_dctDefTcmInf(nMen).ContainsKey(key) Then
            Return False
        End If
        typ = m_dctDefTcmInf(nMen)(key)
        Return True
    End Function

    ''' <summary>
    ''' �擪���珇�Ƀ}�b�v�f�[�^���擾���܂�
    ''' </summary>
    ''' <param name="row_data">�}�b�v�s���</param>
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <returns>True:�擾���� False:�擾���s</returns>
    ''' <remarks></remarks>
    Public Function ReadRow(ByRef row_data As G_MAP_ROW, ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByVal nRow As Integer) As Boolean
        If GetRowCount(emStage) <= nRow Then
            Return False
        End If

        row_data = m_stMap.stage(emStage).men(nMen).row(nRow)

        Return True
    End Function


    ''' <summary>
    ''' ���݂̕\���Ԋu�ɂ�����R�C���̑��s�����擾���܂�
    ''' </summary>
    ''' <param name="emStage">1:�\���Ԋu���A2:�\���Ԋu��</param>
    ''' <returns></returns>
    Public Function GetRowCount(ByVal emStage As EM_LONG_POS) As Integer
        Dim nCount As Integer = CInt(CoilLength \ 1000 \ Interval(emStage)) + 1
        If nCount >= m_stMap.stage(emStage).men(0).row.Length Then
            nCount = m_stMap.stage(emStage).men(0).row.Length
        End If
        Return nCount
    End Function

    ''' <summary>
    ''' �r����(�J�n�s��w��)
    ''' </summary>
    ''' <param name="nColS">�����J�n�r�f�[�^��̗�ԍ� 0�I���W��</param>
    ''' <param name="nRowS">�����J�n�r�f�[�^��̍s�ԍ� 0�I���W��</param>
    ''' <param name="bDirCol">�������� �� True:�R�C���f�[�^�̍�����E�� False:�R�C���f�[�^�̉E���獶��</param>
    ''' <param name="bDirRow">�������� �s True:�R�C���f�[�^�̏ォ�牺�� False:�R�C���f�[�^�̉�������</param>
    ''' <param name="nNum">������</param>
    ''' <param name="lstKeyR">�������ʊi�[���r���X�g�L�[ �ꗗ</param>
    ''' <param name="lstValR">�������ʊi�[���r���X�g�l �ꗗ</param>
    ''' <param name="nMen">0:�\�A1:��</param>
    ''' <param name="emStage">1:�\���Ԋu���A2:�\���Ԋu��</param>
    ''' <returns>True:�������� False:�������s</returns>
    ''' <remarks>�f�[�^��̃R�C�����ォ��E���֌������Č�������</remarks>
    Public Function FindDefect(
      ByVal nColS As Integer,
      ByVal nRowS As Integer,
      ByVal bDirCol As Boolean,
      ByVal bDirRow As Boolean,
      ByVal nNum As Integer,
      ByVal lstKeyR As List(Of LIST_KEY),
      ByVal lstValR As List(Of TYP_MAP_LIST_VALUE),
      ByVal nMen As Integer,
      ByVal emStage As EM_LONG_POS) As Boolean

        Dim bRet As Boolean = False

        Try
            Dim key As LIST_KEY = Nothing

            '-------------------------------------------------------------------
            ' ���������ݒ�
            '-------------------------------------------------------------------
            Dim nRowE As Integer = GetRowCount(emStage) - 1
            Dim nStepRow As Integer = 1
            If Not bDirRow Then
                nRowE = 0
                nStepRow = -1
                nRowS = GetRowCount(emStage) - 1
            End If

            Dim nColE As Integer = MAP_COL_NUM - 1
            Dim nStepCol As Integer = 1
            If Not bDirCol Then
                nColE = 0
                nStepCol = -1
            End If

            '-------------------------------------------------------------------
            ' �r������L�[���������������A�r����������
            '-------------------------------------------------------------------
            Dim bFindKey As Boolean = False
            For nRow As Integer = nRowS To nRowE Step nStepRow
                For nCol As Integer = nColS To nColE Step nStepCol
                    key.emStage = emStage
                    key.nMen = nMen
                    key.nCol = nCol
                    key.nRow = nRow
                    If m_dctDefLst.ContainsKey(key) = False Then
                        Continue For
                    End If
                    If 1 > m_dctDefLst(key).Count Then
                        Continue For
                    End If

                    Dim val As TYP_MAP_LIST_VALUE = m_dctDefLst(key)(0)
                    bRet = FindDefect(key, val, bDirCol, bDirRow, nNum, lstKeyR, lstValR, nMen, emStage)
                    Exit Try
                Next
                If bDirCol Then                                          ' ���������ɍ��킹�āA��̌����J�n�ʒu�����肷��
                    nColS = 0
                Else
                    nColS = MAP_COL_NUM - 1
                End If
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r�����i���W�w��j�ُ� [{0}]", ex.Message))
        End Try

        Return bRet
    End Function


    ''' <summary>
    ''' �r����(�J�n�r�w��)
    ''' </summary>
    ''' <param name="stKeyS">�����J�n�r�̃��X�g�L�[</param>
    ''' <param name="stValS">�����J�n�r�̒l</param>
    ''' <param name="bDirCol">�������� �� True:�f�[�^��R�C��������E�� False:�f�[�^��R�C���E���獶��</param>
    ''' <param name="bDirRow">�������� �s True:�f�[�^��R�C���ォ�牺�� False:�f�[�^��R�C����������</param>
    ''' <param name="nNum">������</param>
    ''' <param name="lstKeyR">�������ʊi�[���r���X�g�L�[ �ꗗ</param>
    ''' <param name="lstValR">�������ʊi�[���r���X�g�l �ꗗ</param>
    ''' <param name="nMen">0:�\�A1:��</param>
    ''' <param name="emStage">1:�\���Ԋu���A2:�\���Ԋu��</param>
    ''' <returns>True:�������� False:�������s</returns>
    ''' <remarks>�f�[�^��̃R�C�����ォ��E���֌������Č�������</remarks>
    Public Function FindDefect(
     ByVal stKeyS As LIST_KEY,
     ByVal stValS As TYP_MAP_LIST_VALUE,
     ByVal bDirCol As Boolean,
     ByVal bDirRow As Boolean,
     ByVal nNum As Integer,
     ByVal lstKeyR As List(Of LIST_KEY),
     ByVal lstValR As List(Of TYP_MAP_LIST_VALUE),
     ByVal nMen As Integer,
     ByVal emStage As EM_LONG_POS) As Boolean
        Try
            Dim bRet As Boolean = False

            '�i�[�惊�X�g���N���A
            lstKeyR.Clear()
            lstValR.Clear()

            Dim nCnt As Integer = 0                                             ' ���������r�J�E���g
            '-------------------------------------------------------------------
            ' �����J�n�O�����m�F
            '-------------------------------------------------------------------
            If m_dctDefLst.ContainsKey(stKeyS) = False Then
                Exit Try
            End If
            If m_dctDefLst(stKeyS).Contains(stValS) = False Then
                Exit Try
            End If

            '-------------------------------------------------------------------
            ' �����r���X�g��������
            '-------------------------------------------------------------------
            Dim nIndexS As Integer = m_dctDefLst(stKeyS).IndexOf(stValS)
            Dim nIndexE As Integer = m_dctDefLst(stKeyS).Count - 1
            Dim nStepIdx As Integer = 1
            If bDirRow = False Then
                nIndexE = 0
                nStepIdx = -1
            End If

            For idx As Integer = nIndexS To nIndexE Step nStepIdx               ' ���I������Ă����r���猟������
                lstKeyR.Add(stKeyS)
                lstValR.Add(m_dctDefLst(stKeyS)(idx))
                nCnt = nCnt + 1
                If lstKeyR.Count >= nNum Then
                    bRet = True
                    Exit Try
                End If
            Next

            '-------------------------------------------------------------------
            ' ���������̐ݒ�
            '-------------------------------------------------------------------
            Dim nRowS As Integer = stKeyS.nRow
            Dim nRowE As Integer = GetRowCount(emStage) - 1
            Dim nStepRow As Integer = 1
            If bDirRow = False Then
                nRowE = 0
                nStepRow = -1
            End If

            Dim nColS As Integer = stKeyS.nCol + 1
            Dim nColE As Integer = MAP_COL_NUM - 1
            Dim nStepCol As Integer = 1

            If bDirCol = False Then
                nColS = stKeyS.nCol - 1
                nColE = 0
                nStepCol = -1
            End If

            '-------------------------------------------------------------------
            ' �����J�n
            '-------------------------------------------------------------------
            Dim key As LIST_KEY = Nothing
            key.emStage = emStage
            key.nMen = nMen

            For row As Integer = nRowS To nRowE Step nStepRow
                For col As Integer = nColS To nColE Step nStepCol
                    key.nCol = col
                    key.nRow = row

                    If m_dctDefLst.ContainsKey(key) = False Then                ' �L�[��������Ύ���
                        Continue For
                    End If

                    If m_dctDefLst(key).Count <= 0 Then                         ' �r��������Ύ���
                        Continue For
                    End If

                    nIndexS = 0                                                 ' ��������C���f�b�N�X������ݒ肷��
                    nIndexE = m_dctDefLst(key).Count - 1
                    If bDirRow = False Then
                        nIndexS = nIndexE
                        nIndexE = 0
                    End If

                    For idx As Integer = nIndexS To nIndexE Step nStepIdx       ' �r���X�g���珇���i�[����
                        lstKeyR.Add(key)
                        lstValR.Add(m_dctDefLst(key)(idx))
                        nCnt += 1

                        If nCnt >= nNum Then                                    ' �����������������I������
                            Return True
                        End If
                    Next
                Next

                If bDirCol = True Then                                          ' �J�n��ʒu�̍Đݒ�
                    nColS = 0
                Else
                    nColS = MAP_COL_NUM - 1
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�r�����i�r�w��j�ُ� [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function

    ''' <summary>
    ''' �O���b�h�̗񐔂��Z�o���� (��}�b�v�ɑΉ�)
    ''' </summary>
    ''' <param name="width">�Z�o��������</param>
    ''' <param name="interval">��̊Ԋu</param>
    ''' <param name="maxCol">�ő�̗�</param>
    ''' <returns>�O���b�h��</returns>
    ''' <remarks></remarks>
    Private Function SelectGridCol2(ByRef width As Integer, ByRef interval As Integer, ByRef maxCol As Integer) As Integer
        '// ���s���ɂ��A�ő�ɂ���
        If 0 >= width Then
            Return maxCol
        End If

        '// ���ɂ��Z�o
        Dim nCol As Integer = 0
        Dim env As Integer = maxCol Mod 2          '// �}�b�v��ʂ̗� (0:������  1:���)
        Dim nJustIndex As Integer = 0
        '// ����̋��E�ʒu����
        If 0 = env Then
            '// ����
            nJustIndex = CInt(Int((width - 1) / (interval * 2)))
            nCol = (nJustIndex + 1) * 2
        Else
            '// �
            width -= interval
            If 0 >= width Then
                Return 1                        '// 1�}�X�ȓ��̏ꍇ�̓��ꏈ��
            End If

            nJustIndex = CInt(Int((width - 1) / (interval * 2)))
            nCol = (nJustIndex + 1) * 2 + 1
        End If

        '// �������`�F�b�N
        If maxCol < nCol Or 0 > nCol Then
            nCol = maxCol
        End If

        Return nCol

    End Function


    ''' <summary>
    ''' �r���X�g���\�[�g����
    ''' </summary>
    ''' <param name="dicMapDefect"></param>
    ''' <param name="bNagate">True:��������̔��]���� False:���]�Ȃ�</param>
    ''' <remarks></remarks>
    Private Sub Sort_Map_Kizulist(ByRef dicMapDefect As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)), ByVal bNagate As Boolean)
        Dim lstData As List(Of TYP_MAP_LIST_VALUE)             '�r�탊�X�g
        Try
            For Each key As LIST_KEY In dicMapDefect.Keys
                lstData = dicMapDefect(key)
                Dim Comparer As New List_Comparer()
                lstData.Sort(Comparer)
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�r���X�g�\�[�g�G���[ [" & ex.Message & "]")
        End Try
    End Sub


    ''' <summary>
    ''' �����{�� (�����l����������)
    ''' </summary>
    ''' <remarks></remarks>
    Public Class List_Comparer
        Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE) 'IComparer�C���^�[�t�F�C�X����������

        ''' <summary>
        ''' ��r���@ (sort�ɂ�����)
        ''' </summary>
        ''' <param name="x">��r�Ώ�</param>
        ''' <param name="y">��r�Ώ�</param>
        ''' <returns>0�F���� 0��菬����:X�̕��������� 0���傫��:X�̕����傫��</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As TYP_MAP_LIST_VALUE, ByVal y As TYP_MAP_LIST_VALUE) As Integer _
         Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE).Compare

            ' �D�揇��
            ' �@ �O���[�h�D��x
            ' �A �r��D��x
            ' �B �ʐϏ�
            ' �C ���菇
            ' �D �rNo
            Dim dNow(5) As Double
            Dim dPer(5) As Double
            Dim bMode(5) As Boolean


            dNow(0) = CDbl(x.nGYuu)
            dPer(0) = CDbl(y.nGYuu)
            bMode(0) = False

            dNow(1) = CDbl(x.nTYuu)
            dPer(1) = CDbl(y.nTYuu)
            bMode(1) = False

            dNow(2) = CDbl(x.nAria)
            dPer(2) = CDbl(y.nAria)
            bMode(2) = True

            dNow(3) = CDbl(x.nY)
            dPer(3) = CDbl(y.nY)
            bMode(3) = False

            dNow(4) = CDbl(x.nKizuNo)
            dPer(4) = CDbl(y.nKizuNo)
            bMode(4) = False

            If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) Then
                Return 0
            End If
            ' �D��x��r
            If Not SelectPriorityDetect(dNow, dPer, bMode) Then
                Return 1
            Else
                Return -1
            End If
        End Function
    End Class

    ''' <summary>
    ''' �����{�� (�����l����������) ����������]�p
    ''' </summary>
    ''' <remarks></remarks>
    Public Class List_Comparer_Reverse
        Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE) 'IComparer�C���^�[�t�F�C�X����������

        ''' <summary>
        ''' ��r���@ (sort�ɂ�����)
        ''' </summary>
        ''' <param name="x">��r�Ώ�</param>
        ''' <param name="y">��r�Ώ�</param>
        ''' <returns>0�F���� 0��菬����:X�̕��������� 0���傫��:X�̕����傫��</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As TYP_MAP_LIST_VALUE, ByVal y As TYP_MAP_LIST_VALUE) As Integer _
         Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE).Compare

            ' �D�揇��
            ' �@ �O���[�h�D��x
            ' �A �r��D��x
            ' �B �ʐϏ�
            ' �C ���菇
            ' �D �rNo
            Dim dNow(5) As Double
            Dim dPer(5) As Double
            Dim bMode(5) As Boolean


            dNow(0) = CDbl(x.nGYuu)
            dPer(0) = CDbl(y.nGYuu)
            bMode(0) = False

            dNow(1) = CDbl(x.nTYuu)
            dPer(1) = CDbl(y.nTYuu)
            bMode(1) = False

            dNow(2) = CDbl(x.nAria)
            dPer(2) = CDbl(y.nAria)
            bMode(2) = True

            dNow(3) = CDbl(x.nY)
            dPer(3) = CDbl(y.nY)
            bMode(3) = True

            dNow(4) = CDbl(x.nKizuNo)
            dPer(4) = CDbl(y.nKizuNo)
            bMode(4) = False

            If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) Then
                Return 0
            End If
            ' �D��x��r
            If Not SelectPriorityDetect(dNow, dPer, bMode) Then
                Return 1
            Else
                Return -1
            End If
        End Function
    End Class

    ''' <summary>
    ''' �r����ǂݍ��݂܂�
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="nKizuNo">�rNo</param>
    ''' <returns>True:�ǂݍ��ݐ��� False:�ǂݍ��ݎ��s</returns>
    ''' <remarks></remarks>
    Public Function GetTcmImage(ByVal strKanriNo As String, ByVal nKizuNo As Integer, ByVal typData As DSP_TYP_MAE_DEFECTINF, ByVal nImgType As Integer) As Image
        Dim clsDB As tClass_SQLServer = Nothing
        Dim readSQL As SqlDataReader = Nothing
        Dim imgData As Image = Nothing
        Try


            Dim strTableImg() As String = {DB_MAE_DEFECT_IMG}

            '-------------------------------------------------------------------
            ' �r�p�k�쐬 (�r�摜)
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL = ""
            strSQL &= "SELECT"
            strSQL &= " �摜1"

            strSQL &= " FROM"
            strSQL &= " " & strTableImg(0)
            strSQL &= " WHERE"
            strSQL &= " �Ǘ�No='" & strKanriNo & "'"
            strSQL &= " AND �rNo=" & nKizuNo

            '-------------------------------------------------------------------
            ' ���s
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                              ' DB�ڑ����s
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�f�[�^�Ǘ� TCM�r�摜DB�ڑ����s [{0}][{1}]", strKanriNo, nKizuNo))
                Return Nothing
            End If

            readSQL = clsDB.SelectExecute(strSQL)
            If readSQL Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�f�[�^�Ǘ� TCM�r�摜�擾SQL���s���s SQL={0}", strSQL))
                Return Nothing
            End If

            '-------------------------------------------------------------------
            ' �i�[
            '-------------------------------------------------------------------
            With readSQL
                imgData = Nothing
                If readSQL.Read() = True Then                                   ' �r�摜�͂���Ίi�[����
                    If .IsDBNull(0) = False Then
                        If frmJisseki.EM_DSP_IMAGE_FORMAT.nJpeg = nImgType Then

                            'JPEG
                            imgData = tClass_BmpJpeg.DataToImage(CType(.GetValue(0), Byte()))
                        Else
                            'BMP
                            imgData = tClass_BmpJpeg.DataToBmp(typData.w_size, typData.h_size, CType(.GetValue(0), Byte()))
                        End If
                    End If
                End If
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�f�[�^�Ǘ� TCM�r�g��摜�\�� �r�摜�ǂݍ��ُ݈� [{0}]", ex.Message))
            imgData = Nothing
            Return Nothing
        Finally
            If clsDB Is Nothing = False Then
                clsDB.Cancel()
                If readSQL Is Nothing = False Then
                    readSQL.Close()
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try
        Return imgData
    End Function

    ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
    ''' <summary>
    ''' �r���X�g���\�[�g����
    ''' </summary>
    ''' <param name="dicMapDefect"></param>
    ''' <param name="bNagate">True:��������̔��]���� False:���]�Ȃ�</param>
    ''' <remarks></remarks>
    Private Sub Sort_Map_KizulistTcm(ByRef dicMapDefect As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)), ByVal bNagate As Boolean)
        Dim lstData As List(Of TYP_MAP_LIST_VALUE)             '�r�탊�X�g
        Try
            For Each key As LIST_KEY In dicMapDefect.Keys
                lstData = dicMapDefect(key)
                Dim Comparer As New List_ComparerTcm()
                lstData.Sort(Comparer)
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�r���X�g�\�[�g�G���[ [" & ex.Message & "]")
        End Try
    End Sub


    ''' <summary>
    ''' �����{�� (�����l����������) TCM�p
    ''' </summary>
    ''' <remarks></remarks>
    Public Class List_ComparerTcm
        Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE) 'IComparer�C���^�[�t�F�C�X����������

        ''' <summary>
        ''' ��r���@ (sort�ɂ�����)
        ''' </summary>
        ''' <param name="x">��r�Ώ�</param>
        ''' <param name="y">��r�Ώ�</param>
        ''' <returns>0�F���� 0��菬����:X�̕��������� 0���傫��:X�̕����傫��</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As TYP_MAP_LIST_VALUE, ByVal y As TYP_MAP_LIST_VALUE) As Integer _
         Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE).Compare

            ' �D�揇��
            ' �@ �H�����(TCM��(4)��TCM�r(6))
            ' �A �O���[�h�D��x
            ' �B �r��D��x
            ' �C �ʐϏ�
            ' �D ���菇
            ' �E �rNo
            Dim dNow(6) As Double
            Dim dPer(6) As Double
            Dim bMode(6) As Boolean

            dNow(0) = CDbl(x.nKouteiID)
            dPer(0) = CDbl(y.nKouteiID)
            bMode(0) = False

            dNow(1) = CDbl(x.nGYuu)
            dPer(1) = CDbl(y.nGYuu)
            bMode(1) = False

            dNow(2) = CDbl(x.nTYuu)
            dPer(2) = CDbl(y.nTYuu)
            bMode(2) = False

            dNow(3) = CDbl(x.nAria)
            dPer(3) = CDbl(y.nAria)
            bMode(3) = True

            dNow(4) = CDbl(x.nY)
            dPer(4) = CDbl(y.nY)
            bMode(4) = False

            dNow(5) = CDbl(x.nKizuNo)
            dPer(5) = CDbl(y.nKizuNo)
            bMode(5) = False

            If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) And dNow(5) = dPer(5) Then
                Return 0
            End If
            ' �D��x��r
            If Not SelectPriorityDetect(dNow, dPer, bMode) Then
                Return 1
            Else
                Return -1
            End If
        End Function
    End Class
	' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	''' <summary>
	''' �o������ʒu�擾
	''' </summary>
	''' <param name="dInLen">��������ʒu(mm)</param>
	''' <returns>�o������ʒu(mm)</returns>
	''' <remarks>�����蔽�]���͓�������ʒu�A�J�b�g�ʒu�͎��O�ɔ��]����Ă�����̂Ƃ���B</remarks>
	Private Function GetOutLen(ByVal dInLen As Single, ByVal bUpTrun As Boolean, ByVal nResultLen As Integer) As Single
		Dim dOutLen As Single = dInLen

		If m_listCutInf Is Nothing OrElse 0 = m_listCutInf.Count Then
			' �J�b�g�Ȃ�
			Return dInLen
		End If

		' �J�b�g�L�Ȃ璼�߂̃J�b�g�ʒu����̋�����Ԃ�
		Dim bExistCut As Boolean = False

		For ii As Integer = 0 To m_listCutInf.Count - 1
			Dim nCutIndex As Integer = 0
			If True = bUpTrun Then
				' ���蔽�]���� -> �J�b�g�ʒu�͒��蔽�]����č~���ɃZ�b�g����Ă���B
				nCutIndex = ii
			Else
				' ���蔽�]�Ȃ� -> �J�b�g�ʒu�͏����ɃZ�b�g����Ă���B
				nCutIndex = m_listCutInf.Count - ii - 1
			End If

			If dInLen >= m_listCutInf(nCutIndex).dPosS_MM Then
				' �J�b�g�ʒu����̋���
				bExistCut = True
				dOutLen = dInLen - m_listCutInf(nCutIndex).dPosS_MM
				Exit For
			End If
		Next

		If 0 > dOutLen Then
			dOutLen = 0
		End If

		Return dOutLen
	End Function
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

End Class
