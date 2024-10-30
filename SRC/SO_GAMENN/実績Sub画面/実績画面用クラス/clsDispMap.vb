'*******************************************************************************
'�@�r�}�b�v�N���X
'	[Ver]
'		Ver.01    2010/11/04  ����
'
'	[����]
'		�r�}�b�v�\���N���X
'*******************************************************************************
Option Strict On

Imports System.Threading

Imports KizuLibCLI.KizuMod
Imports KizuForm

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports tClassLibrary.tClass.tClass_LogManager
Imports System.Reflection

''' <summary>
''' �r�}�b�v�\��
''' </summary>
''' <remarks></remarks>
Public Class clsDispMap
	Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '�萔��`
    '///////////////////////////////////////////////////////////////////////////
    Private Const MAX_MAPDEFLST_ROW As Integer = 3         ' �r���X�g�ő�\����

    '///////////////////////////////////////////////////////////////////////////
    '�񋓑�
    '///////////////////////////////////////////////////////////////////////////
    Public Enum EM_DSP_KIZU_INF
        DSP_JI1 = 0
        DSP_JI2
    End Enum

    ''' <summary>
    ''' �r�}�b�v�r���X�g�\����
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_DEEP_LIST_COL
        emNo = 0                    ' No
        emGr                        ' Gr
        emType                      ' �r��
        emEnd
    End Enum


    ''' <summary>
    ''' TCM�r���X�g�\����
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_TCM_LIST_COL
        emNo = 0                    ' No
        emLine                      ' ���C����
        emGr                        ' Gr
        emType                      ' �r��
        emEnd
    End Enum

    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    '�r�}�b�v�\����
    Public Structure TYP_MAP_INF
        Dim dgvMap As tCnt_DataGridView_turn                ' �r�}�b�v
        Dim dgvDeep As tCnt_DataGridView_turn               ' �r�[�����X�g
        Dim dgvTcm As tCnt_DataGridView_turn                ' TCM���X�g
        Dim vsbMap As tCnt_VScrollBar                       ' �X�N���[���o�[
    End Structure


    ''' <summary>
    ''' �}�b�v����r���
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure MAP_DEFECT
        Dim strKizukenNo As String                                              ' �Ǘ�No
        Dim nKizuNo As Integer                                                  ' �rNo
        Dim nCol As EM_MAP_COL_INF                                              ' �r�}�b�v��̗�ԍ� 0�I���W��
        Dim nRow As Integer                                                     ' �r�}�b�v��̍s�ԍ� 0�I���W��
    End Structure

    ''' <summary>
    ''' �}�b�v�s���
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure MAP_ROW_DATA
        Dim MapHeader As MAP_HEADER                                             ' �}�b�v�w�b�_���
        Dim MapInf As G_MAP_ROW                                                 ' �}�b�v���
    End Structure

    ''' <summary>
    ''' �}�b�v�w�b�_���
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure MAP_HEADER
        Dim strKizukenNo As String                                              ' �Ǘ�No
        Dim bIsSharTop As Boolean                                               ' �V���[�J�b�g�擪�ʒu�L��
        Dim bIsSharBot As Boolean                                               ' �V���[�J�b�g���[�ʒu�L��
        Dim bIsIshuten As Boolean                                               ' �َ�_
        Dim bIsIshutenBot As Boolean                                            ' �َ�_����
        Dim bIsTanpanCut As Boolean                                             ' �[�J�b�g�L��
    End Structure

    ''' <summary>
    ''' �r�}�b�v�l
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure MAP_VALUE
        Dim strKizukenNo As String                                              ' �Ǘ�No
        Dim nKizuNo As Integer                                                  ' �rNo
        Dim nTypeID As Integer                                                  ' �r���EdasysID
        Dim nGradeID As Integer                                                 ' �O���[�h��EdasysID
        Dim strMapName As String                                                ' �}�b�v�\����

        ''' <summary>
        ''' �}�b�v�l�̕�����擾
        ''' </summary>
        ''' <returns>�r��}�b�v�����擾����</returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return strMapName                                                   ' DataGridView�̕\���p��ToStrign���I�[�o�[���C�h����
        End Function
    End Structure


    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    ' ���\�[�X�J���ς݃t���E�O True:�J���ς�
    Private m_emStage As EM_LONG_POS                                            ' ����Ԋu(1:���A2:��)
    Private m_emMapDir As EM_MAP_SCROOL                                         ' �}�b�v�̐i�s����
    Private m_emSide As EM_MAP_SIDE                                             ' ���x��DS�ʒu[NORMAL:����DS�AREVERSE:����WS]
    Private m_DSPos As EM_MAP_CELL                                              ' �r�}�b�vDS�ʒu[NORMAL:����DS�AREVERSE:����WS]
    Private m_nDspMen As Integer                                                ' �\����
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>	
	Private m_emWidthNo As EM_MAP_WIDTH_NO                          ' �}�b�v���ʒu�ԍ�[NORMAL:������1,2,�E�E�E�AREVERSE:�E����1,2,�E�E�E]
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q <<<<<--------------------

	Private m_typMapInf As TYP_MAP_INF                                          ' �r�}�b�v
    Private mcls_Log As tClass_LogManager                                       ' ���O�Ǘ�
    Private mcls_Param As clsParamManager                                       ' �p�����[�^�Ǘ�
    Private mcls_PreParam As clsPreParamManager                                 ' �O�H���p�����[�^�Ǘ�
    Private mcls_DataMap As clsDataMap                                          ' �r�}�b�v�f�[�^�N���X

    Private m_typSelectValue(DSP_DEFECT_MAX - 1) As TYP_MAP_LIST_VALUE          '�I���r
    Private m_typTcmSelectValue As TYP_MAP_LIST_VALUE                           'TCM�I���r
    Private m_nMaxRowCnt As Integer                                             '�ő�s��

    Private m_aryColor() As Color                                               ' �I��F�ꗗ

    Private Const DEFECT_DEGAWA As Boolean = False                              ' �o�������\�����邩 false:�\�����Ȃ� true:�\������

    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
#Region "�v���p�e�B"

    ''' <summary>
    ''' �\����
    ''' </summary>
    ''' <value>0:�\�ʁA1:����</value>
    ''' <returns>0:�\�ʁA1:����</returns>
    ''' <remarks></remarks>
    Public Property Men() As Integer
        Get
            Return m_nDspMen
        End Get
        Set(ByVal value As Integer)
            m_nDspMen = value
        End Set
    End Property

    ''' <summary>
    ''' �\���Ԋu
    ''' </summary>
    ''' <value>SMALL(1):����Ԋu�� BIG(2):����Ԋu��</value>
    ''' <returns>SMALL(1):����Ԋu�� BIG(2):����Ԋu��</returns>
    ''' <remarks></remarks>
    Public Property Stage() As EM_LONG_POS
        Get
            Return m_emStage
        End Get
        Set(ByVal value As EM_LONG_POS)
            m_emStage = value
        End Set
    End Property

    ''' <summary>
    ''' �}�b�v�i�s����
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property MapDir() As EM_MAP_SCROOL
        Get
            Return m_emMapDir
        End Get
        Set(ByVal value As EM_MAP_SCROOL)
            m_emMapDir = value
        End Set
    End Property

    ''' <summary>
    ''' �r�}�b�vDS�ʒu
    ''' </summary>
    ''' <value>NORMAL:����DS�AREVERSE:����WS</value>
    ''' <returns>NORMAL:����DS�AREVERSE:����WS</returns>
    ''' <remarks></remarks>
    Public Property DSPos() As EM_MAP_CELL
        Get
            Return m_DSPos
        End Get
        Set(ByVal value As EM_MAP_CELL)
			m_DSPos = value
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>		
			'If EM_MAP_CELL.NORMAL = m_DSPos Then
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "2"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "3"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "4"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "5"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "6"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "7"
			'         Else
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "7"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "6"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "5"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "4"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "3"
			'             m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "2"
			'         End If
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>		
		End Set
    End Property

    ''' <summary>
    ''' ���x��DS�ʒu
    ''' </summary>
    ''' <value>NORMAL:����DS�AREVERSE:����WS</value>
    ''' <returns>NORMAL:����DS�AREVERSE:����WS</returns>
    ''' <remarks></remarks>
    Public Property Side() As EM_MAP_SIDE
        Get
            Return m_emSide
        End Get
        Set(ByVal value As EM_MAP_SIDE)
            m_emSide = value
            If m_emSide = EM_MAP_SIDE.NORMAL Then
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_1).HeaderText = DIRECTION_NAME_DS
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_8).HeaderText = DIRECTION_NAME_WS
            Else
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_1).HeaderText = DIRECTION_NAME_WS
                m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_8).HeaderText = DIRECTION_NAME_DS
            End If
        End Set
    End Property

	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>	
	''' <summary>
	''' �}�b�v�ԍ��ʒu
	''' </summary>
	''' <value>[NORMAL:������1,2,�E�E�E�AREVERSE:�E����1,2,�E�E�E]</value>
	''' <returns>[NORMAL:������1,2,�E�E�E�AREVERSE:�E����1,2,�E�E�E]</returns>
	''' <remarks></remarks>
	Public Property WidthNo() As EM_MAP_WIDTH_NO
		Get
			Return m_emWidthNo
		End Get
		Set(ByVal value As EM_MAP_WIDTH_NO)
			m_emWidthNo = value
			If m_emWidthNo = EM_MAP_SIDE.NORMAL Then
				Dim nAddr As Integer = EM_MAP_COL_INF.COL_2
				For ii As Integer = EM_MAP_COL_INF.COL_2 To EM_MAP_COL_INF.COL_7
					m_typMapInf.dgvMap.Columns(ii).HeaderText = nAddr.ToString
					nAddr = nAddr + 1
				Next
			Else
				Dim nAddr As Integer = EM_MAP_COL_INF.COL_7
				For ii As Integer = EM_MAP_COL_INF.COL_2 To EM_MAP_COL_INF.COL_7
					m_typMapInf.dgvMap.Columns(ii).HeaderText = nAddr.ToString
					nAddr = nAddr - 1
				Next
			End If
		End Set
	End Property
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q <<<<<--------------------

	''' <summary>
	''' �I��F�̐ݒ�Ǝ擾
	''' </summary>
	''' <param name="index">�I���� 0:�ŐV 1:��O</param>
	''' <value>�I��F</value>
	''' <returns>�I��F</returns>
	''' <remarks></remarks>
	Public Property SelectColor(ByVal index As Integer) As Color
        Get
            Return m_aryColor(index)
        End Get
        Set(ByVal value As Color)
            m_aryColor(index) = value
        End Set
    End Property

    ''' <summary>
    ''' �r�}�b�v�f�[�^�N���X�ݒ�
    ''' </summary>
    Public WriteOnly Property DataMap() As clsDataMap
        Set(value As clsDataMap)
            mcls_DataMap = value
        End Set
    End Property


#End Region

    '///////////////////////////////////////////////////////////////////////////
    '�C�x���g
    '///////////////////////////////////////////////////////////////////////////
#Region "�C�x���g"
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    ''' <param name="emInf"></param>
    ''' <remarks></remarks>
    Public Event SelectMapCell_Auto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal emInf As EM_DSP_KIZU_INF)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="emInf"></param>
    ''' <remarks></remarks>
    Public Event SelectMapCell_AutoDspInit(ByVal emInf As EM_DSP_KIZU_INF)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    ''' <remarks></remarks>
    Public Event SelectMapCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    ''' <remarks></remarks>
    Public Event SelectTcmMapCell_Auto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <remarks></remarks>
    Public Event SelectTcmMapCell_AutoDspInit()
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    Public Event SelectMapTcmCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)

#End Region

    ''' <summary>
    ''' �r�}�b�v���쐬���܂�
    ''' </summary>
    ''' <param name="typMap"></param>
    ''' <param name="param"></param>
    ''' <param name="log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typMap As TYP_MAP_INF, ByRef param As clsParamManager, ByRef preparam As clsPreParamManager, ByRef log As tClass_LogManager)
        Try
            m_typMapInf = typMap       '�r�}�b�v���
            mcls_Param = param
            mcls_PreParam = preparam
            mcls_Log = log
            mcls_DataMap = Nothing


            For ii As Integer = 0 To m_typSelectValue.Length - 1
                m_typSelectValue(ii).Initialize()
            Next
            m_typTcmSelectValue.Initialize()

            ReDim m_aryColor(DSP_DEFECT_MAX - 1)

            '�r�}�b�v�O���b�h
            dgvInitialize(m_typMapInf.dgvMap)
            m_typMapInf.dgvMap.bTanspose = False
            InitdgvMap(m_typMapInf.dgvMap)
            Side = EM_MAP_SIDE.NORMAL
            DSPos = EM_MAP_CELL.NORMAL
            AddHandler m_typMapInf.dgvMap.Paint, AddressOf dgvMap_Paint
            AddHandler m_typMapInf.dgvMap.CellMouseDown, AddressOf dgvMap_CellMouseDown


            '�r���X�g�O���b�h
            InitdgvDeep(m_typMapInf.dgvDeep)
            m_typMapInf.dgvDeep.bTanspose = False
            AddHandler m_typMapInf.dgvDeep.Paint, AddressOf dgvDeep_Paint
            AddHandler m_typMapInf.dgvDeep.CellMouseDown, AddressOf dgvDeep_CellMouseDown


            'TCM���X�g�O���b�h
            InitdgvTcm(m_typMapInf.dgvTcm)
            m_typMapInf.dgvTcm.bTanspose = False
            'AddHandler m_typMapInf.dgvTcm.Paint, AddressOf dgvTcm_Paint
            AddHandler m_typMapInf.dgvTcm.CellMouseDown, AddressOf dgvTcm_CellMouseDown


            m_typMapInf.vsbMap.SmallChange = 1
            m_typMapInf.vsbMap.LargeChange = m_typMapInf.dgvMap.RowCount

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�\�������ُ� [{0}]", ex.Message))
        End Try
    End Sub

#Region " IDisposable Support "
    ''' <summary>
    ''' �I�u�W�F�N�g�̔j��
    ''' </summary>
    ''' <param name="disposing">True:�����I�ȌĂяo�� False:�Öق̌Ăяo��</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not m_bIsDesposed Then
            '�I�u�W�F�N�g�j��
        End If
        m_bIsDesposed = True                                                'Dispose�ς݂Ƃ���
    End Sub

    ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' �r�}�b�v�f�[�^�O���b�h������
    ''' </summary>
    ''' <param name="dgvCtrl">�r�}�b�v�f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub InitdgvMap(ByVal dgvCtrl As tCnt_DataGridView_turn)
        '��{�ݒ�
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl
            '�s��`
            .RowCount = JISSEKI_MAP_DSP_ROW_MAX
            .ColumnHeadersHeight = 20

            '�����`
            .ScrollBars = ScrollBars.None
            .ReadOnly = True

            '�Z����`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '���`
            .ColumnCount = MAP_COL_NUM + 3 + 1             '���������A�o�������A���ATCM

            .Columns(EM_MAP_COL_INF.DEGAWA).HeaderText = "�o��"
            .Columns(EM_MAP_COL_INF.DEGAWA).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.DEGAWA).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MAP_COL_INF.DEGAWA).Width = 43
            .Columns(EM_MAP_COL_INF.DEGAWA).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.DEGAWA).Frozen = False
            .Columns(EM_MAP_COL_INF.DEGAWA).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            For ii As Integer = EM_MAP_COL_INF.COL_1 To EM_MAP_COL_INF.COL_8
                .Columns(ii).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).Width = 52
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).Frozen = True
                .Columns(ii).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Next ii

            .Columns(EM_MAP_COL_INF.IRIGAWA).HeaderText = "����"
            .Columns(EM_MAP_COL_INF.IRIGAWA).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.IRIGAWA).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MAP_COL_INF.IRIGAWA).Width = 43
            .Columns(EM_MAP_COL_INF.IRIGAWA).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.IRIGAWA).Frozen = True
            .Columns(EM_MAP_COL_INF.IRIGAWA).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            .Columns(EM_MAP_COL_INF.KIZUNUM).HeaderText = "��"
            .Columns(EM_MAP_COL_INF.KIZUNUM).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.KIZUNUM).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MAP_COL_INF.KIZUNUM).Width = 30
            .Columns(EM_MAP_COL_INF.KIZUNUM).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.KIZUNUM).Frozen = False
            .Columns(EM_MAP_COL_INF.KIZUNUM).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            .Columns(EM_MAP_COL_INF.OTH1).HeaderText = "TCM"
            .Columns(EM_MAP_COL_INF.OTH1).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.OTH1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_MAP_COL_INF.OTH1).Width = 32
            .Columns(EM_MAP_COL_INF.OTH1).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MAP_COL_INF.OTH1).Frozen = True
            .Columns(EM_MAP_COL_INF.OTH1).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

        End With
    End Sub


    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tCnt_DataGridView_turn)

        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                         ' �I��F����
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            '�s��`
            .RowTemplate.Height = MAP_ROW_HIGHT
            .ColumnHeadersHeight = HED_ROW_HIGHT
            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 8.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .ColumnHeadersHeight = 20
            .ScrollBars = ScrollBars.None                                 ' �X�N���[���o�[�̕\��

            ''�w�i�F
            '.BackgroundColor = Color.White

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 7.5, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            .ColumnHeadersVisible = False                   ' ��w�b�_�\��
            .RowHeadersVisible = False                      ' �s�w�b�_�\��

        End With
    End Sub

    ''' <summary>
    ''' �[�����X�g����������
    ''' </summary>
    ''' <param name="dgvCtrl">�}�b�v�f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub InitdgvDeep(ByVal dgvCtrl As tCnt_DataGridView_turn)
        '��{�ݒ�
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl

            '�����`
            .ScrollBars = ScrollBars.Vertical
            .ReadOnly = True

            '�Z����`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '���`
            .ColumnCount = EM_DEEP_LIST_COL.emEnd
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For nCnt As Integer = 0 To .ColumnCount - 1
                .Columns(nCnt).ReadOnly = True
                If nCnt = EM_DEEP_LIST_COL.emNo Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                    .Columns(nCnt).Width = 30
                    .Columns(nCnt).HeaderText = "No"
                ElseIf nCnt = EM_DEEP_LIST_COL.emGr Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = 48
                    .Columns(nCnt).HeaderText = "Gr"
                ElseIf nCnt = EM_DEEP_LIST_COL.emType Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = .ClientSize.Width - .Columns(0).Width - .Columns(1).Width - 3
                    .Columns(nCnt).HeaderText = "�r���X�g(0)"
                End If
                .Columns(nCnt).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(nCnt).Frozen = False
                .Columns(nCnt).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                '.Width = .Width + .Columns(nCnt).Width + 1
            Next

            .RowCount = MAX_MAPDEFLST_ROW

        End With
    End Sub

    ''' <summary>
    ''' TCM�r���X�g����������
    ''' </summary>
    ''' <param name="dgvCtrl">�}�b�v�f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub InitdgvTcm(ByVal dgvCtrl As tCnt_DataGridView_turn)
        '��{�ݒ�
        dgvCtrl.dgvInitialize(dgvCtrl)
        dgvCtrl.DataGridViewSelectionCancel(dgvCtrl)

        With dgvCtrl

            '�����`
            .ScrollBars = ScrollBars.Vertical
            .ReadOnly = True

            '�Z����`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            '���`
            .ColumnCount = EM_TCM_LIST_COL.emEnd
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20
            For nCnt As Integer = 0 To .ColumnCount - 1
                .Columns(nCnt).ReadOnly = True
                If nCnt = EM_TCM_LIST_COL.emNo Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                    .Columns(nCnt).Width = 30
                    .Columns(nCnt).HeaderText = "No"
                ElseIf nCnt = EM_TCM_LIST_COL.emLine Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = 42
                    .Columns(nCnt).HeaderText = "ײ�"
                ElseIf nCnt = EM_TCM_LIST_COL.emGr Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = 56
                    .Columns(nCnt).HeaderText = "Gr"
                ElseIf nCnt = EM_TCM_LIST_COL.emType Then
                    .Columns(nCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    .Columns(nCnt).Width = .ClientSize.Width - .Columns(0).Width - .Columns(1).Width - .Columns(2).Width - 3
                    .Columns(nCnt).HeaderText = "TCM(0)"
                End If
                .Columns(nCnt).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(nCnt).Frozen = False
                .Columns(nCnt).HeaderCell.Style.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                '.Width = .Width + .Columns(nCnt).Width + 1
            Next

            .RowCount = MAX_MAPDEFLST_ROW

        End With
    End Sub

    ''' <summary>
    ''' �}�b�v������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ClearMap()
        Try

            '�s�f�[�^
            For nRow As Integer = 0 To m_typMapInf.dgvMap.RowCount - 1

                m_typMapInf.dgvMap.Rows(nRow).Height = 20

                '��f�[�^
                For nCol As Integer = 0 To m_typMapInf.dgvMap.ColumnCount - 1
                    With m_typMapInf.dgvMap
                        .Item(nCol, nRow).Tag = Nothing
                        .Item(nCol, nRow).Value = ""
                        .Item(nCol, nRow).Style.ForeColor = Color.Black
                        .Item(nCol, nRow).Style.BackColor = Color.White
                    End With
                Next nCol
            Next nRow

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�}�b�v�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r���X�g������
    ''' </summary>
    Public Sub ClearDeep()

        With m_typMapInf.dgvDeep
            .Rows.Clear()
            .RowCount = 0
            .Columns(EM_DEEP_LIST_COL.emType).HeaderText = "�r���X�g(0)"
        End With

    End Sub
    ''' <summary>
    ''' TCM���X�g������
    ''' </summary>
    Public Sub ClearTcm()

        With m_typMapInf.dgvTcm
            .Rows.Clear()
            .RowCount = 0
            .Columns(EM_TCM_LIST_COL.emType).HeaderText = "TCM(0)"
        End With
        m_typTcmSelectValue.Initialize()

    End Sub


    ''' <summary>
    ''' �r�}�b�v�����\�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapInit()
        Try
            ClearMap()
            m_typMapInf.dgvMap.Refresh()
            ClearDeep()
            m_typMapInf.dgvDeep.Refresh()
            ClearTcm()
            m_typMapInf.dgvTcm.Refresh()
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�\�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v�\������
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function DspData(ByVal nMen As Integer) As Boolean
        Dim bReturn As Boolean = False

        Try
            If mcls_DataMap Is Nothing Then
                Exit Try
            End If

            m_nMaxRowCnt = 0

            Dim nSelectValue(m_typSelectValue.Length - 1) As TYP_MAP_LIST_VALUE
            For ii As Integer = 0 To m_typSelectValue.Length - 1
                nSelectValue(ii) = m_typSelectValue(ii)
            Next
            Dim nTcmSelectValue As TYP_MAP_LIST_VALUE = m_typTcmSelectValue

            '�r�}�b�v�ݒ�
            If Not SetMapData(nMen, 0, m_nMaxRowCnt) Then
                Exit Try
            End If

            If 0 > m_typSelectValue(0).nPnt Then
                '�ŐV�r��������
                GetSearchKizuFirst(nMen)
            End If

            '�r�����I���C�x���g����
            If 0 < m_typSelectValue(0).nPnt And nSelectValue(0) <> m_typSelectValue(0) Then
                ' �ŐV�r�I�����ω������ꍇ�̓}�b�v�ʒu�̕ω����m�F
                Dim nRow As Integer
                Dim nCol As Integer

                Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)
                If bDeepDisp AndAlso Not IsVisibleOnMap(m_typSelectValue(0)) Then
                    '�I���r�����̈�ɂȂ�
                    bDeepDisp = False
                End If
                If Not bDeepDisp Then
                    Dim kizukey As clsDataMap.KIZU_KEY
                    kizukey.nKizuNo = m_typSelectValue(0).nKizuNo
                    kizukey.nStage = Stage
                    If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                        Exit Try
                    End If
                    Dim listkeyWk As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)
                    Dim nValue As Integer = listkeyWk.nRow
                    If (m_nMaxRowCnt - nValue) < m_typMapInf.dgvMap.DisplayedRowCount(False) Then
                        nValue = m_nMaxRowCnt - m_typMapInf.dgvMap.DisplayedRowCount(False)
                    End If

                    'If Not SetMapData(nMen, listkey.nRow, m_nMaxRowCnt) Then
                    Dim nMaxRowNum As Integer
                    If Not SetMapData(nMen, nValue, nMaxRowNum) Then
                        Exit Try
                    End If
                    GetMapPos(m_typSelectValue(0), nRow, nCol)
                End If
            End If

            Dim listKey As clsDataMap.LIST_KEY

            Dim bDeepFlg As Boolean = False
            listKey.initialize()
            For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
                '' �r�I�����ω������ꍇ�̓}�b�v�ʒu�̕ω����m�F
                If 0 < m_typSelectValue(ii).nPnt Then
                    Dim kizukey As clsDataMap.KIZU_KEY
                    kizukey.nKizuNo = m_typSelectValue(ii).nKizuNo
                    kizukey.nStage = Stage
                    If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                        Continue For
                    Else
                        bDeepFlg = True
                    End If
                    listKey = mcls_DataMap.ListGetKizuNoKey(kizukey)
                End If
            Next
            If bDeepFlg Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = Nothing
                Dim bLstRet As Boolean = mcls_DataMap.GetMapDefectList(listKey.nCol, listKey.nRow, listKey.nMen, listKey.emStage, lstVal)
                If Not bLstRet Then
                    lstVal = Nothing
                End If
                ' �[�����X�g�̕\��
                SetDeepList(m_typMapInf.dgvDeep, lstVal)
            End If

            '�I���O�ƂȂ����Ƃ����X�g��������
            If Not bDeepFlg Then
                ClearDeep()
            End If
            m_typMapInf.dgvDeep.Refresh()

            Dim nTcmRow As Integer = 0
            Dim nTcmCol As Integer = 0
            Dim bTcmFlg As Boolean = False
            '' �r�I�����ω������ꍇ�̓}�b�v�ʒu�̕ω����m�F
            If 0 < m_typTcmSelectValue.nPnt And nTcmSelectValue <> m_typTcmSelectValue Then
                Dim nRowWk As Integer
                Dim nColWk As Integer
                If GetMapTcmPos(m_typTcmSelectValue, nRowWk, nColWk) Then
                    nTcmRow = nRowWk
                    nTcmCol = nColWk
                    bTcmFlg = True
                End If
            End If
            If bTcmFlg Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nTcmCol, nTcmRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                ' TCM���X�g�̕\��
                SetTcmList(m_typMapInf.dgvTcm, lstVal)
            End If

            For ii As Integer = 0 To m_typSelectValue.Length - 1
                If 0 < m_typSelectValue(ii).nPnt Then
                    If nSelectValue(ii) <> m_typSelectValue(ii) Then
                        RaiseEvent SelectMapCell_Auto(nMen, m_typSelectValue(ii), CType(ii, EM_DSP_KIZU_INF))
                    End If
                Else
                    If nSelectValue(ii) <> m_typSelectValue(ii) Then
                        RaiseEvent SelectMapCell_AutoDspInit(CType(ii, EM_DSP_KIZU_INF))
                    End If
                End If
            Next
            If 0 < m_typTcmSelectValue.nPnt Then
                If nTcmSelectValue <> m_typTcmSelectValue Then
                    RaiseEvent SelectTcmMapCell_Auto(nMen, m_typTcmSelectValue)
                End If
            Else
                If nTcmSelectValue <> m_typTcmSelectValue Then
                    RaiseEvent SelectTcmMapCell_AutoDspInit()
                End If
            End If


            bReturn = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�\�������ُ�[{0}]", ex.Message))
        End Try

        Return bReturn
    End Function

    ''' <summary>
    ''' �I���O�ƂȂ����Ƃ����X�g��������
    ''' </summary>
    Private Sub LstSelectClear()
        Dim bDeepDelFlg As Boolean = False
        Dim bCycDelFlg As Boolean = False
        For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
            '' �r�I�����ω������ꍇ�̓}�b�v�ʒu�̕ω����m�F
            'If 0 < m_typSelectValue(ii).nPnt And 2 <> m_typSelectValue(ii).nCycDiv Then
            If 0 < m_typSelectValue(ii).nPnt Then
                Dim kizukey As clsDataMap.KIZU_KEY
                kizukey.nKizuNo = m_typSelectValue(ii).nKizuNo
                kizukey.nStage = Stage
                If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                    Continue For
                Else
                    bDeepDelFlg = True
                End If
            End If
        Next

        If Not bDeepDelFlg Then
            ClearDeep()
        End If
        m_typMapInf.dgvDeep.Refresh()

    End Sub

    ''' <summary>
    ''' �r�}�b�v���\��
    ''' </summary>
    ''' <param name="nMove">�ړ���</param>
    ''' <remarks></remarks>
    Public Function SetMapData(ByVal nMen As Integer, ByVal nMove As Integer, ByRef nMaxRowNum As Integer) As Boolean
        Dim nRowPos As Integer                              '�s�ʒu
        Dim nDgvPos As Integer = 0                          'DGV�s�ʒu
        Dim nCoilCnt As Integer = 0                         '�\���R�C����
        Dim bStart As Boolean = False                       '�����J�n�t���O
        Dim nKensaPos As Integer = 0                        '������ʒu
        Dim nRowCheck As Integer = 0
        Dim nRowCount As Integer = 0
        Dim emStage As EM_LONG_POS = Stage
        Dim nRowCnt As Integer = 0
        Dim bRet As Boolean = False


        Try
            If mcls_DataMap Is Nothing Then
                Exit Try
            End If

            '������
            ClearMap()

            nRowPos = nMove

            DspMap(nMen, emStage, nRowPos, nDgvPos)

            If 0 <= nDgvPos And m_typMapInf.dgvMap.RowCount > nDgvPos Then
                '�f�[�^���Z�b�g����Ȃ������s�𖳌��s�ɂ���
                While m_typMapInf.dgvMap.RowCount > nDgvPos
                    Dim nRow As Integer = nDgvPos
                    If EM_MAP_SCROOL.TAIL_TO_HEAD <> MapDir Then         '�ォ�牺
                        nRow = m_typMapInf.dgvMap.RowCount - 1 - nDgvPos
                    End If
                    DspRowDisable(nRow)
                    nDgvPos = nDgvPos + 1
                End While
            End If

            nMaxRowNum = mcls_DataMap.GetRowCount(emStage)
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v���Z�b�g�ُ�[{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �ŐV�r����
    ''' </summary>
    ''' <param name="nMen">�\���ʔԍ�</param>
    ''' <returns>True�F���݂���AFalse�F���݂��Ȃ�</returns>
    ''' <remarks></remarks>
    Private Function GetSearchKizuFirst(ByVal nMen As Integer) As Boolean

        '-------------------------------------------------------------------
        ' ����������̌���
        '-------------------------------------------------------------------
        Dim bDirCol As Boolean = CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False))
        Dim nColS As Integer = 0        '�J�n��͍��[(0)
        ' TODO ���[�̃f�[�^����ɑI��
        If Not bDirCol Then
            nColS = MAP_COL_NUM - 1     '������������u�E���獶�v�̏ꍇ�́A�J�n����E�[
        End If

        '-------------------------------------------------------------------
        ' �����s�����̌���
        '-------------------------------------------------------------------
        Dim bDirRow As Boolean = True   '�f�[�^��̏ォ�牺
        Dim nRowS As Integer = 0

        '-------------------------------------------------------------------
        ' �R�C����̍����r��I��
        '-------------------------------------------------------------------
        Dim lstKey As New List(Of clsDataMap.LIST_KEY)
        Dim lstVal As New List(Of TYP_MAP_LIST_VALUE)
        If Not mcls_DataMap.FindDefect(nColS, 0, bDirCol, bDirRow, m_typSelectValue.Count, lstKey, lstVal, nMen, Stage) Then
            Return False
        End If

        Dim nCnt As Integer = 0
        For ii As Integer = 0 To lstVal.Count - 1
            If 0 < lstVal(ii).nKizuNo Then
                m_typSelectValue(nCnt) = lstVal(ii)
                nCnt = nCnt + 1
                If m_typSelectValue.Count = nCnt Then
                    Exit For
                End If
            End If
        Next

        Return CBool(IIf(0 < nCnt, True, False))

    End Function

    ''' <summary>
    ''' �}�b�v���Z�b�g
    ''' </summary>
    ''' <param name="nMapPos">���ݕ\���s</param>
    ''' <param name="nDgvPos">DGV�\���ʒu</param>
    ''' <remarks></remarks>
    Private Sub DspMap(ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByVal nMapPos As Integer, ByRef nDgvPos As Integer)
        Dim nColStart As Integer                            '�J�n��C���f�b�N�X
        Dim nColEnd As Integer                              '�I����C���f�b�N�X
        Dim nStep As Integer                                '��X�e�b�v�Ԋu
        Dim nColCnt As Integer                              '�\��COL�J�E���^
        Dim nRowStart As Integer


        '�r�}�b�vDS�ʒu
        If m_DSPos = EM_MAP_CELL.NORMAL Then                '����DS
            nColStart = 0
            nColEnd = MAP_COL_NUM - 1
            nStep = 1
        Else                                                '����WS
            nColStart = MAP_COL_NUM - 1
            nColEnd = 0
            nStep = -1
        End If

        Try
            nRowStart = nMapPos

            For nMapRow As Integer = nRowStart To mcls_DataMap.GetRowCount(Stage) - 1 Step 1

                If nDgvPos >= m_typMapInf.dgvMap.RowCount Then
                    Exit For
                End If

                Dim nDgvRow As Integer = nDgvPos
                If EM_MAP_SCROOL.TAIL_TO_HEAD <> MapDir Then         '�������
                    nDgvRow = m_typMapInf.dgvMap.RowCount - 1 - nDgvPos
                End If

                '�J�n�s�̔ԍ��Z�b�g
                nColCnt = 0

                '�}�b�v���擾
                Dim typRow As G_MAP_ROW = Nothing
                Dim lstCell() As List(Of TYP_MAP_LIST_VALUE)
                Dim lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)

                typRow.initialize()
                ReDim lstCell(MAP_COL_NUM - 1)
                For jj As Integer = 0 To lstCell.Length - 1
                    lstCell(jj) = Nothing
                Next
                ReDim lstTcmCell(1 - 1)
                For jj As Integer = 0 To lstTcmCell.Length - 1
                    lstTcmCell(jj) = Nothing
                Next

                GetMapRowData(nMen, emStage, nMapRow, typRow, lstCell, lstTcmCell)

                For nSetColCnt As Integer = nColStart To nColEnd Step nStep

                    If 0 < typRow.col(nSetColCnt).nKizuNo Then                      '�\���Ώ��r�L��
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Value = mcls_Param.GetMapName(typRow.col(nSetColCnt).nTid)
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor = mcls_Param.GetGradeColor(typRow.col(nSetColCnt).nGid)
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Tag = lstCell(nSetColCnt)
                    Else
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Value = vbNullString
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor = g_ColorTanshoOk
                        m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Tag = Nothing
                        If 0 > typRow.col(nSetColCnt).nKizuNo Then
                            m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor = g_ColorTanshoGai
                        End If
                    End If

                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.SelectionBackColor = m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.COL_1 + nColCnt, nDgvRow).Style.BackColor
                    nColCnt = nColCnt + 1                   '���i�߂�
                Next

                '���������\��
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Value = typRow.nLen_i
                '�o�������\��
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Value = typRow.nLen_o

                '�����Z���J���[
                If EM_DIV_KENSA.DIV_KENSA_NON = typRow.emKensa Then        '�����O
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
                ElseIf EM_DIV_KENSA.DIV_KENSA_STOP = typRow.emKensa Then    '��~
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
                ElseIf EM_DIV_KENSA.DIV_KENSA_SKIP = typRow.emKensa Then   '������
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_SKIP)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_SKIP)
                ElseIf EM_DIV_KENSA.DIV_KENSA_DBNON = typRow.emKensa Then  '���ۑ�
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_DBNON)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_DBNON)
                ElseIf EM_DIV_KIKI.DIV_KIKI_JYU = typRow.emKiki Then       '�d�̏�
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKikiBackColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKikiForeColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU)
                ElseIf EM_DIV_KENSA.DIV_KENSA_EDGENG = typRow.emKensa Then  '�G�b�W�s��
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_EDGENG)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_EDGENG)
                ElseIf EM_DIV_KENSA.DIV_KENSA_NG = typRow.emKensa Then     '�k��
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NG)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NG)
                ElseIf EM_DIV_KIKI.DIV_KIKI_KEI = typRow.emKiki Then       '�y�̏�
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKikiBackColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKikiForeColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                Else                                                        '����
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorTanshoOk
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                End If
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.SelectionBackColor = m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Style.BackColor
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Tag = typRow

                With m_typMapInf

                    '�s�P�ʂ��r����\��
                    If typRow.nKizuCnt <= 0 Then
                        '�r���Ȃ��Ƃ��́A����\�����Ȃ�
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Value = vbNullString
                    ElseIf typRow.nKizuCnt <= KZCNT_CNT_MAX Then
                        '�r���\���ő�l�𒴂��Ȃ��Ƃ��́A����\��
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Value = typRow.nKizuCnt.ToString()
                    Else
                        '�r���\���ő�l�𒴂���Ƃ��́A�ő�l��\��
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Value = KZCNT_CNT_MAX.ToString()
                    End If

                    '�s�P�ʂ��r���ɑΉ������F�̕\��
                    If typRow.nKizuCnt >= mcls_Param.GetCntMapCount(EM_DETECT_GR.DETECT_GR_JU) Then
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = mcls_Param.GetCntMapColor(EM_DETECT_GR.DETECT_GR_JU)
                    ElseIf typRow.nKizuCnt >= mcls_Param.GetCntMapCount(EM_DETECT_GR.DETECT_GR_TYU) Then
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = mcls_Param.GetCntMapColor(EM_DETECT_GR.DETECT_GR_TYU)
                    ElseIf typRow.nKizuCnt >= mcls_Param.GetCntMapCount(EM_DETECT_GR.DETECT_GR_KEI) Then
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = mcls_Param.GetCntMapColor(EM_DETECT_GR.DETECT_GR_KEI)
                    Else
                        .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor = Color.White
                    End If

                    ''�I�����̔w�i�F���A�ʏ펞�Ɠ����ɂ��Ă���
                    .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.SelectionBackColor = .dgvMap.Item(EM_MAP_COL_INF.KIZUNUM, nDgvRow).Style.BackColor

                End With

                If 0 < typRow.tcm(0).nKizuNo Then                      '�\���Ώ��r�L��
                    Dim typTcmData As TYP_MAP_LIST_VALUE = Nothing
                    typTcmData.Initialize()

                    typTcmData = lstTcmCell(0)(0)
                    Dim strDsp As String = ""
                    Select Case typTcmData.strProc
                        Case KOUTEI_CODE_2TCM   '2TCM
                            strDsp = "2"
                        Case KOUTEI_CODE_3TCM   '3TCM
                            strDsp = "3"
                    End Select
                    strDsp = strDsp & mcls_PreParam.GetDspMapName(typTcmData.strProc, typTcmData.strFFType)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Value = strDsp
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Style.BackColor = tMod.GetIntToColor(mcls_PreParam.GetDspGradeColor(typTcmData.strProc, typTcmData.strFFGrade))
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Tag = lstTcmCell(0)
                Else
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Value = vbNullString
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Style.BackColor = g_ColorTanshoOk
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Tag = Nothing
                    'If 0 > typRow.cyc(0).nKizuNo Then
                    '    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.OTH1, nDgvRow).Style.BackColor = g_ColorTanshoGai
                    'End If
                End If

                nDgvPos = nDgvPos + 1

            Next

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�������s[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �}�b�v�ʒu�s���擾
    ''' </summary>
    ''' <param name="nMen">��[0:�\,1:��]</param>
    ''' <param name="nRow">�s[0�`]</param>
    ''' <returns>�s���</returns>
    ''' <remarks></remarks>
    Private Function GetMapRowData(ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByVal nRow As Integer,
                                   ByRef typRow As G_MAP_ROW, ByRef lstVal() As List(Of TYP_MAP_LIST_VALUE),
                                   ByRef lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)) As Boolean

        Dim bRetc As Boolean = False

        Try
            typRow.initialize()
            If Not mcls_DataMap.ReadRow(typRow, nMen, emStage, nRow) Then
                Exit Try
            End If
            typRow.nMemRowPos = nRow

            For nCol As Integer = 0 To lstVal.Length - 1
                Dim bLstRet As Boolean = mcls_DataMap.GetMapDefectList(nCol, nRow, nMen, emStage, lstVal(nCol))
                If Not bLstRet Then
                    lstVal(nCol) = Nothing
                End If
            Next

            For nCol As Integer = 0 To lstTcmCell.Length - 1
                Dim bLstRet As Boolean = mcls_DataMap.GetMapDefectTcmList(nCol, nRow, nMen, emStage, lstTcmCell(nCol))
                If Not bLstRet Then
                    lstTcmCell(nCol) = Nothing
                End If
            Next

            bRetc = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�}�b�v�ʒu�s���擾�ُ�[{0}]", ex.Message))
        End Try

        Return bRetc
    End Function


    ''' <summary>
    ''' �r�}�b�v�O���b�h�`��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMap_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim penYousetu As New System.Drawing.Pen(g_ColorYousetu, 2)         '�n�ڐ�
        Dim penCut As New System.Drawing.Pen(g_ColorShar, 2)                '�V���[�J�b�g��
        Dim penKensa As New System.Drawing.Pen(g_ColorKensa, 2)             '�������
        Dim penBound As New System.Drawing.Pen(Color.Black, 2)              '���E��
        Dim penSelect(DSP_DEFECT_MAX - 1) As System.Drawing.Pen             '�r�F

        Dim dgvMap As tCnt_DataGridView_turn = CType(sender, tCnt_DataGridView_turn)
        Dim penPos As Integer = 0                                           '�����d�Ȃ����ꍇ�̃Y���V�Ԋu�ێ�
        Dim penRectTop As Integer = 0                                       '�g�̍���
        Dim penRectLeft As Integer = 0                                      '�g�̍��[

        Dim nStart As Integer
        Dim nStep As Integer
        Dim nMax As Integer


        penSelect(0) = New System.Drawing.Pen(m_aryColor(0), 3)
        penSelect(1) = New System.Drawing.Pen(m_aryColor(1), 3)

        Try

            '�n�ڐ��A�V���[�J�b�g���̕\��
            Dim nRowCount As Integer = dgvMap.DisplayedRowCount(True)
            Dim nRowTop As Integer = dgvMap.FirstDisplayedScrollingRowIndex

            For ii As Integer = 0 To nRowCount - 1
                Dim nRowIdx As Integer = nRowTop + ii
                If dgvMap(0, nRowIdx).Value Is "" Then
                    'MAP��񂪂Ȃ��ꍇ�͔�΂�(��������ȂǁA�\������f�[�^���Ȃ��s�̏ꍇ)
                    Continue For
                End If

                Dim typRow As G_MAP_ROW
                typRow = CType(dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nRowIdx).Tag, G_MAP_ROW)

                '�n�ړ_�A�V���[�J�b�g�� �d�Ȃ蔻��
                If dgvMap.RowCount - 1 > nRowIdx Then
                    Dim typRowN As G_MAP_ROW
                    typRowN = CType(dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nRowIdx + 1).Tag, G_MAP_ROW)
                    If (typRow.bYousetu And (0 < typRowN.nCut)) Or
                        (typRowN.bYousetu And (0 < typRow.nCut)) Then
                        penPos = 1
                    End If
                End If

                '�n�ڐ��\��
                If typRow.bYousetu Then
                    If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                        e.Graphics.DrawRectangle(penYousetu, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 2) - penPos, dgvMap.Width, 1)
                    Else
                        e.Graphics.DrawRectangle(penYousetu, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 1) - penPos, dgvMap.Width, 1)
                    End If
                End If

                '�V���[�J�b�g���\��
                If 0 < typRow.nCut Then
                    If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                        e.Graphics.DrawRectangle(penCut, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 1) + penPos, dgvMap.Width, 1)
                    Else
                        e.Graphics.DrawRectangle(penCut, 0, (dgvMap.Item(0, nRowIdx).Size.Height) * (nRowIdx - nRowTop + 2) + penPos, dgvMap.Width, 1)
                    End If
                End If
            Next


            '�I���r�\������
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                nStart = nRowTop + nRowCount - 1
                nStep = -1
                nMax = nRowTop
            Else
                nStart = nRowTop
                nStep = 1
                nMax = nRowTop + nRowCount - 1
            End If

            For nRowCnt As Integer = nStart To nMax Step nStep
                For nCol As Integer = EM_MAP_COL_INF.COL_1 To EM_MAP_COL_INF.COL_8
                    Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvMap.Item(nCol, nRowCnt).Tag, List(Of TYP_MAP_LIST_VALUE))
                    If lstVal IsNot Nothing Then

                        Dim bExist(2) As Boolean
                        bExist(0) = False
                        bExist(1) = False
                        For ii As Integer = 0 To lstVal.Count - 1
                            If lstVal(ii) = m_typSelectValue(0) Then
                                bExist(0) = True
                            End If
                            If lstVal(ii) = m_typSelectValue(1) Then
                                bExist(1) = True
                            End If
                        Next

                        If penSelect(1) IsNot Nothing _
                        And bExist(1) Then
                            '�I���r�̘g�`��
                            penRectTop = dgvMap(nCol, nRowCnt).Size.Height * (dgvMap(nCol, nRowCnt).RowIndex - nRowTop + 1)
                            penRectLeft = dgvMap(0, nRowCnt).Size.Width + dgvMap(nCol, nRowCnt).Size.Width * (nCol - 1) + 10
                            e.Graphics.DrawRectangle(penSelect(1), penRectLeft - 8, penRectTop, dgvMap.Item(nCol, nRowCnt).Size.Width - 1, dgvMap.Item(nCol, nRowCnt).Size.Height - 1)
                        End If

                        If penSelect(0) IsNot Nothing _
                        And bExist(0) Then
                            '�I���r�̘g�`��
                            penRectTop = dgvMap(nCol, nRowCnt).Size.Height * (dgvMap(nCol, nRowCnt).RowIndex - nRowTop + 1)
                            penRectLeft = dgvMap(0, nRowCnt).Size.Width + dgvMap(nCol, nRowCnt).Size.Width * (nCol - 1) + 10
                            e.Graphics.DrawRectangle(penSelect(0), penRectLeft - 8, penRectTop, dgvMap.Item(nCol, nRowCnt).Size.Width - 1, dgvMap.Item(nCol, nRowCnt).Size.Height - 1)
                        End If

                    End If
                Next
            Next nRowCnt

            penRectLeft = 0
            For nCol As Integer = EM_MAP_COL_INF.DEGAWA To EM_MAP_COL_INF.OTH1
                Select Case nCol
                    Case EM_MAP_COL_INF.KIZUNUM
                        e.Graphics.DrawLine(penBound, penRectLeft + 1, dgvMap.Top, penRectLeft + 1, dgvMap.Height)
                    Case EM_MAP_COL_INF.OTH1
                        e.Graphics.DrawLine(penBound, penRectLeft + 1, dgvMap.Top, penRectLeft + 1, dgvMap.Height)
                End Select
                penRectLeft = penRectLeft + dgvMap(nCol, 0).Size.Width
            Next

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�O���b�h�`��ُ�[{0}]", ex.Message))
        Finally
            If Not penYousetu Is Nothing Then penYousetu.Dispose()
            If Not penCut Is Nothing Then penCut.Dispose()
            If Not penKensa Is Nothing Then penKensa.Dispose()
            If Not penSelect(0) Is Nothing Then penSelect(0).Dispose()
            If Not penSelect(1) Is Nothing Then penSelect(1).Dispose()
            If Not penBound Is Nothing Then penBound.Dispose()
        End Try
    End Sub


    ''' <summary>
    ''' �r�}�b�v�ʒu����
    ''' </summary>
    ''' <param name="typVal"></param>
    ''' <param name="nRowR"></param>
    ''' <param name="nColR"></param>
    ''' <returns></returns>
    Private Function GetMapPos(ByVal typVal As TYP_MAP_LIST_VALUE, ByRef nRowR As Integer, ByRef nColR As Integer) As Boolean

        Dim nStartRow As Integer = 0
        Dim nEndRow As Integer = m_typMapInf.dgvMap.RowCount - 1
        Dim nStep As Integer = 1

        If 0 > typVal.nPnt Then
            Return False
        End If

        If EM_MAP_SCROOL.TAIL_TO_HEAD = MapDir Then
            nStartRow = m_typMapInf.dgvMap.RowCount - 1
            nEndRow = 0
            nStep = -1
        End If

        For nRow As Integer = nStartRow To nEndRow Step nStep
            For nCol As Integer = EM_MAP_COL_INF.COL_1 To EM_MAP_COL_INF.COL_8
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                If lstVal Is Nothing Then
                    Continue For
                End If
                For ii As Integer = 0 To lstVal.Count - 1
                    If typVal = lstVal(ii) Then
                        nRowR = nRow
                        nColR = nCol
                        Return True
                    End If
                Next
            Next
        Next

        Return False

    End Function


    ''' <summary>
    ''' �r�}�b�v�ʒu(TCM)����
    ''' </summary>
    ''' <param name="typVal"></param>
    ''' <param name="nRowR"></param>
    ''' <param name="nColR"></param>
    ''' <returns></returns>
    Private Function GetMapTcmPos(ByVal typVal As TYP_MAP_LIST_VALUE, ByRef nRowR As Integer, ByRef nColR As Integer) As Boolean

        Dim nStartRow As Integer = 0
        Dim nEndRow As Integer = m_typMapInf.dgvMap.RowCount - 1
        Dim nStep As Integer = 1

        If 0 > typVal.nPnt Then
            Return False
        End If

        If EM_MAP_SCROOL.TAIL_TO_HEAD = MapDir Then
            nStartRow = m_typMapInf.dgvMap.RowCount - 1
            nEndRow = 0
            nStep = -1
        End If

        For nRow As Integer = nStartRow To nEndRow Step nStep
            For nCol As Integer = EM_MAP_COL_INF.OTH1 To EM_MAP_COL_INF.OTH1
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                If lstVal Is Nothing Then
                    Continue For
                End If
                For ii As Integer = 0 To lstVal.Count - 1
                    If typVal = lstVal(ii) Then
                        nRowR = nRow
                        nColR = nCol
                        Return True
                    End If
                Next
            Next
        Next

        Return False

    End Function

    ''' <summary>
    ''' �r�}�b�v�Z���I��(�}�E�X�N���b�N�C�x���g��)
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub dgvMap_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        '�����s
        If e.RowIndex < 0 Then
            Exit Sub
        End If

        If EM_MAP_COL_INF.OTH1 = e.ColumnIndex Then
            dgvMap_TcmCellMouseDown(sender, e)
            Exit Sub
        End If

        Dim dgvTmp As tCnt_DataGridView_turn = CType(sender, tCnt_DataGridView_turn)
        Dim dgvCell As DataGridViewCell = dgvTmp.Item(e.ColumnIndex, e.RowIndex)

        Try

            '�o���A�����Z���͖���
            If EM_MAP_COL_INF.COL_1 > dgvCell.ColumnIndex Then
                Exit Try
            End If
            If EM_MAP_COL_INF.COL_8 < dgvCell.ColumnIndex Then
                Exit Try
            End If

            '�E�N���b�N�͖���
            If e.Button = MouseButtons.Right Then
                Exit Try
            End If

            '�r���Ȃ��Ȃ疳��
            If dgvCell.Value Is Nothing Then
                Exit Try
            End If
            If dgvCell.Value.ToString = "" Then
                Exit Try
            End If

            '�I���Z�����r���擾
            Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvCell.Tag, List(Of TYP_MAP_LIST_VALUE))

            If lstVal Is Nothing Then
                Exit Try
            End If

            '�����r�̏ꍇ�͑I�����Ȃ�
            Dim bSelect As Boolean = True
            If m_typSelectValue(0) = lstVal(0) Then
                bSelect = False
            End If

            If e.Button = MouseButtons.Left Then
                If bSelect Then
                    '+++++++++++++++++++
                    '�I���r
                    '+++++++++++++++++++
                    For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1
                        m_typSelectValue(ii) = m_typSelectValue(ii - 1)
                    Next
                    '�I���r�������r��
                    m_typSelectValue(0) = lstVal(0)

                    '�I���O�ƂȂ����Ƃ����X�g��������
                    LstSelectClear()

                    '�N���b�N�C�x���g���s
                    RaiseEvent SelectMapCell_Click(Men, lstVal(0))

                End If

                '�r���X�g�\��
                SetDeepList(m_typMapInf.dgvDeep, lstVal)

            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�Z���I�������ُ�[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()

    End Sub

    ''' <summary>
    ''' �[�����X�g�ݒ�
    ''' </summary>
    ''' <param name="dgv">�[���O���b�h�I�u�W�F�N�g</param>
    ''' <param name="lstVal">�Z�����\����</param>
    ''' <param name="bTop">True:�擪�\���Afalse:�����\��</param>
    ''' <returns>�\������ True:�ݒ芮���AFalse:�ݒ薢����</returns>
    ''' <remarks></remarks>
    Private Function SetDeepList(ByVal dgv As tCnt_DataGridView_turn, ByVal lstVal As List(Of TYP_MAP_LIST_VALUE), Optional ByVal bTop As Boolean = True) As Boolean
        Dim bRet As Boolean = False

        Try
            dgv.Tag = lstVal
            dgv.Rows.Clear()                                            ' ��U�\���s���N���A����
            If lstVal Is Nothing Then
                Exit Try
            End If
            dgv.RowCount = lstVal.Count                        ' �r�[���Z�b�g
            For nCnt As Integer = 0 To dgv.RowCount - 1
                dgv(0, nCnt).Value = nCnt + 1
                dgv(1, nCnt).Value = mcls_Param.GetGradeName(lstVal(nCnt).nGid)
                dgv(1, nCnt).Style.BackColor = mcls_Param.GetGradeColor(lstVal(nCnt).nGid)
                dgv(2, nCnt).Value = mcls_Param.GetMapName(lstVal(nCnt).nTid)
            Next
            dgv.Columns(EM_DEEP_LIST_COL.emType).HeaderText = "�r���X�g(" & lstVal.Count.ToString("#,###") & ")"

            If Not bTop AndAlso dgv.RowCount > MAX_MAPDEFLST_ROW Then
                dgv.FirstDisplayedScrollingRowIndex = dgv.RowCount - MAX_MAPDEFLST_ROW
            End If
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�[�����X�g�ݒ�ُ�[{0}]", ex.Message))
        End Try

        dgv.Refresh()
        Return bRet
    End Function

    ''' <summary>
    ''' �[�����X�g�`��y�C���g
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks>�[�����X�g�\�����̑I���r�̘g�`�揈��</remarks>
    Public Sub dgvDeep_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim dgv As tCnt_DataGridView = CType(sender, tCnt_DataGridView)
        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgv.Tag, List(Of TYP_MAP_LIST_VALUE))
        Dim penSelect(2 - 1) As System.Drawing.Pen                          '�r�F

        penSelect(0) = New System.Drawing.Pen(g_ColorStop1st, 2)            '����1
        penSelect(1) = New System.Drawing.Pen(g_ColorStop2nd, 2)            '����2

        Try
            Dim nTopRow As Integer = dgv.FirstDisplayedScrollingRowIndex
            If nTopRow < 0 Then nTopRow = 0
            For ii As Integer = 0 To MAX_MAPDEFLST_ROW
                Dim nRowCnt As Integer = nTopRow + ii
                If nRowCnt > dgv.RowCount - 1 Then
                    Exit For
                End If
                If lstVal(nRowCnt) = m_typSelectValue(1) Then
                    dgv.Rows(nRowCnt).DefaultCellStyle.BackColor = g_ColorStop2nd
                    dgv.Rows(nRowCnt).DefaultCellStyle.SelectionBackColor = g_ColorStop2nd
                ElseIf lstVal(nRowCnt) = m_typSelectValue(0) Then
                    dgv.Rows(nRowCnt).DefaultCellStyle.BackColor = g_ColorStop1st
                    dgv.Rows(nRowCnt).DefaultCellStyle.SelectionBackColor = g_ColorStop1st
                Else
                    dgv.Rows(nRowCnt).DefaultCellStyle.BackColor = Color.Empty
                    dgv.Rows(nRowCnt).DefaultCellStyle.SelectionBackColor = Color.Empty
                End If

            Next ii

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�[�����X�g�y�C���g�\���G���[[{0}]", ex.Message))
        Finally
            If Not penSelect(0) Is Nothing Then penSelect(0).Dispose()
            If Not penSelect(1) Is Nothing Then penSelect(1).Dispose()
        End Try
    End Sub

    ''' <summary>
    ''' �[�����X�g�Z���I��
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub dgvDeep_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        Dim dgvTmp As DataGridView = CType(sender, DataGridView)
        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvTmp.Tag, List(Of TYP_MAP_LIST_VALUE))

        Try
            If lstVal Is Nothing Then
                Exit Try
            End If

            If 0 > e.RowIndex Or lstVal.Count <= e.RowIndex Then
                Exit Try
            End If

            If m_typSelectValue(0) = lstVal(e.RowIndex) Then
                '�����r�̏ꍇ�͑I�����Ȃ�
                Exit Try
            End If

            '���N���b�N��
            If e.Button = MouseButtons.Left Then
                For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1
                    m_typSelectValue(ii) = m_typSelectValue(ii - 1)
                Next
                m_typSelectValue(0) = lstVal(e.RowIndex)

                '�I���O�ƂȂ����Ƃ����X�g��������
                LstSelectClear()

                '�C�x���g���s
                RaiseEvent SelectMapCell_Click(Men, lstVal(e.RowIndex))
            End If

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�[�����X�g�Z���I���ُ�[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()
        m_typMapInf.dgvMap.Refresh()
    End Sub


    ''' <summary>
    ''' �r�}�b�v�Z���I��(�}�E�X�N���b�N�C�x���g��)
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub dgvMap_TcmCellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        '�����s
        If e.RowIndex < 0 Then
            Exit Sub
        End If

        Dim dgvTmp As tCnt_DataGridView_turn = CType(sender, tCnt_DataGridView_turn)
        Dim dgvCell As DataGridViewCell = dgvTmp.Item(e.ColumnIndex, e.RowIndex)


        Try

            'TCM�}�b�v�ȊO�͖���
            If EM_MAP_COL_INF.OTH1 <> dgvCell.ColumnIndex Then
                Exit Try
            End If

            ''�����X�V���͖���
            'If EM_REFRESH_MODE.AUTO = m_emViewMode Then
            '    Exit Try
            'End If

            '�E�N���b�N�͖���
            If e.Button = MouseButtons.Right Then
                Exit Try
            End If

            '�r���Ȃ��Ȃ疳��
            If dgvCell.Value Is Nothing Then
                Exit Try
            End If
            If dgvCell.Value.ToString = "" Then
                Exit Try
            End If

            '�I���Z�����r���擾
            Dim listVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvCell.Tag, List(Of TYP_MAP_LIST_VALUE))
            If listVal Is Nothing Then
                Exit Try
            End If

            '�����r�̏ꍇ�͑I�����Ȃ�
            Dim bSelect As Boolean = True
            'If m_typTcmSelectValue = listVal(0) Then
            '    bSelect = False
            'End If

            If e.Button = MouseButtons.Left Then
                If bSelect Then
                    '+++++++++++++++++++
                    '�I���r
                    '+++++++++++++++++++
                    '�I���r�������r��
                    m_typTcmSelectValue = listVal(0)

                    '�N���b�N�C�x���g���s
                    RaiseEvent SelectMapTcmCell_Click(Men, listVal(0))

                End If

                '�r���X�g�\��
                SetTcmList(m_typMapInf.dgvTcm, listVal)

            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�Z���I�������ُ�[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()

    End Sub

    ''' <summary>
    ''' TCM���X�g�Z���I��
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub dgvTcm_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs)
        Dim dgvTmp As DataGridView = CType(sender, DataGridView)
        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvTmp.Tag, List(Of TYP_MAP_LIST_VALUE))

        Try
            If lstVal Is Nothing Then
                Exit Try
            End If

            If 0 > e.RowIndex Or lstVal.Count <= e.RowIndex Then
                Exit Try
            End If

            If m_typTcmSelectValue = lstVal(e.RowIndex) Then
                '�����r�̏ꍇ�͑I�����Ȃ�
                Exit Try
            End If

            '���N���b�N��
            If e.Button = MouseButtons.Left Then
                m_typTcmSelectValue = lstVal(e.RowIndex)
                '�C�x���g���s
                RaiseEvent SelectMapTcmCell_Click(Men, lstVal(e.RowIndex))
            End If

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�[�����X�g�Z���I���ُ�[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()
        m_typMapInf.dgvMap.Refresh()
    End Sub

    ''' <summary>
    ''' TCM���X�g�ݒ�
    ''' </summary>
    ''' <param name="dgv">�[���O���b�h�I�u�W�F�N�g</param>
    ''' <param name="lstVal">�Z�����\����</param>
    ''' <returns>�\������ True:�ݒ芮���AFalse:�ݒ薢����</returns>
    ''' <remarks></remarks>
    Private Function SetTcmList(ByVal dgv As tCnt_DataGridView_turn, ByVal lstVal As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = False

        Try
            dgv.Tag = lstVal
            dgv.Rows.Clear()                                            ' ��U�\���s���N���A����
            If lstVal Is Nothing Then
                Exit Try
            End If
            dgv.RowCount = lstVal.Count                        ' �r�[���Z�b�g
            For nCnt As Integer = 0 To dgv.RowCount - 1
                dgv(EM_TCM_LIST_COL.emNo, nCnt).Value = nCnt + 1
                dgv(EM_TCM_LIST_COL.emLine, nCnt).Value = mcls_PreParam.GetProcName(lstVal(nCnt).strProc)
                dgv(EM_TCM_LIST_COL.emGr, nCnt).Value = mcls_PreParam.GetDspGradeName(lstVal(nCnt).strProc, lstVal(nCnt).strFFGrade)
                dgv(EM_TCM_LIST_COL.emGr, nCnt).Style.BackColor = tMod.GetIntToColor(mcls_PreParam.GetDspGradeColor(lstVal(nCnt).strProc, lstVal(nCnt).strFFGrade))
                dgv(EM_TCM_LIST_COL.emType, nCnt).Value = mcls_PreParam.GetDspTypeName(lstVal(nCnt).strProc, lstVal(nCnt).strFFType)
            Next
            dgv.Columns(EM_TCM_LIST_COL.emType).HeaderText = "TCM(" & lstVal.Count.ToString("#,###") & ")"
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�[�����X�g�ݒ�ُ�[{0}]", ex.Message))
        End Try

        dgv.Refresh()
        Return bRet
    End Function

    ''' <summary>
    ''' �X�N���[���ʂ��擾
    ''' </summary>
    ''' <returns>�X�N���[����</returns>
    ''' <remarks></remarks>
    Public Function GetScrollMax() As Integer
        Try
            If m_nMaxRowCnt - m_typMapInf.vsbMap.LargeChange > 0 Then
                Return m_nMaxRowCnt - m_typMapInf.vsbMap.LargeChange
            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�X�N���[���ʂ��擾���s[{0}]", ex.Message))
        End Try
        Return 0
    End Function


    ''' <summary>
    ''' 
    ''' </summary>
    ''' <returns></returns>
    Public Function GetMapRowTop() As Integer
        With m_typMapInf
            Dim nFirst As Integer = .dgvMap.FirstDisplayedScrollingRowIndex
            Dim nStart As Integer = nFirst
            Dim nEnd As Integer = nFirst + .dgvMap.DisplayedRowCount(False) - 1
            Dim nStep As Integer = 1
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                nStart = nFirst + .dgvMap.DisplayedRowCount(False) - 1
                nEnd = nFirst
                nStep = -1
            End If
            For ii As Integer = nStart To nEnd Step nStep
                If .dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag Is Nothing Then
                    Continue For
                End If
                Dim typRow As G_MAP_ROW = CType(.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag, G_MAP_ROW)
                Return typRow.nMemRowPos
            Next
        End With
        Return -1
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <returns></returns>
    Public Function GetMapRowBot() As Integer
        With m_typMapInf
            Dim nFirst As Integer = .dgvMap.FirstDisplayedScrollingRowIndex
            Dim nStart As Integer = nFirst + .dgvMap.DisplayedRowCount(False) - 1
            Dim nEnd As Integer = nFirst
            Dim nStep As Integer = -1
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                nStart = nFirst
                nEnd = nFirst + .dgvMap.DisplayedRowCount(False) - 1
                nStep = 1
            End If
            For ii As Integer = nStart To nEnd Step nStep
                If .dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag Is Nothing Then
                    Continue For
                End If
                Dim typRow As G_MAP_ROW = CType(.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, ii).Tag, G_MAP_ROW)
                Return typRow.nMemRowPos
            Next
        End With
        Return -1
    End Function

    ''' <summary>
    ''' �}�b�v�̏c�T�C�Y�ύX
    ''' </summary>
    Public Sub MapResizeH()
        Dim dgv As tCnt.tCnt_DataGridView = m_typMapInf.dgvMap
        Dim count As Integer = dgv.DisplayedRowCount(False)
        Dim nValue As Integer
        If (dgv.RowCount < count) Then Exit Sub
        ' �}�b�v�X�N���[���̈ړ��T�C�Y(��)�́A�\���s�̕�����
        Dim nLastLarge As Integer = m_typMapInf.vsbMap.LargeChange
        m_typMapInf.vsbMap.LargeChange = count
        Dim nDiff As Integer = nLastLarge - count
        If 0 = nDiff Then
            '�ω��Ȃ�
            Exit Sub
        End If

        ' �J�n�s�����ł���ʒu�ɕύX����
        If EM_MAP_SCROOL.HEAD_TO_TAIL <> MapDir Then
            ' �と���̏ꍇ�͐擪(0)
            dgv.FirstDisplayedScrollingRowIndex = 0
            'm_typMapInf.vsbMap.ValueProgramable = 0
            If 0 > nDiff Then
                m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + nDiff
            End If
            nValue = m_typMapInf.vsbMap.Value
        Else
            ' ������̏ꍇ�́A���I�[�s���Œ肵�Č�����悤�ɊJ�n�ʒu�𒲐�
            dgv.FirstDisplayedScrollingRowIndex = dgv.RowCount - count
            'If nLastLarge > m_typMapInf.vsbMap.LargeChange Then
            '    m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + (nLastLarge - m_typMapInf.vsbMap.LargeChange)
            'End If
            If 0 > nDiff Then
                m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + nDiff
            End If
            nValue = m_typMapInf.vsbMap.Maximum - (m_typMapInf.vsbMap.LargeChange - 1) - m_typMapInf.vsbMap.Value
        End If
        'DspData(Men)
        If 0 <= nValue Then
            Dim nMaxRowCnt As Integer
            SetMapData(Men, nValue, nMaxRowCnt)
        End If
    End Sub


    ''' <summary>
    ''' �}�b�v�I���N���A
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ClearSelect()
        Try
            For ii As Integer = 0 To m_typSelectValue.Length - 1
                m_typSelectValue(ii).Initialize()
            Next ii
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�I���N���A�ُ� [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �I���r���擾
    ''' </summary>
    ''' <param name="n">0:�ŐV�̑I���r�A1:1�O�̑I���r</param>
    ''' <returns></returns>
    Public Function GetSelectKizu(ByVal n As Integer, ByRef typVal As TYP_MAP_LIST_VALUE) As Boolean
        If 0 > n Or m_typSelectValue.Length <= n Then
            Return False
        End If
        typVal = m_typSelectValue(n)
        Return True
    End Function


    ''' <summary>
    ''' �r�̑I�����w������ɐݒ肵�܂�
    ''' </summary>
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <param name="bDir">False:�O�ATrue:��</param>
    ''' <returns>True:�I��OK�AFalse:�I��NG</returns>
    ''' <remarks></remarks>
    Public Function SetSelection(ByVal nMen As Integer, ByVal bDir As Boolean) As Boolean
        Dim bResult As Boolean = False
        Try

            ' �ŐV�̑I���r���擾
            Dim kizukey As clsDataMap.KIZU_KEY
            kizukey.nKizuNo = m_typSelectValue(0).nKizuNo
            kizukey.nStage = Stage
            If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                Exit Try
            End If
            Dim listkey As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)

            '-------------------------------------------------------------------
            ' ����������̌���
            '-------------------------------------------------------------------
            Dim bDirCol As Boolean = CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False))
            If Not bDir Then
                bDirCol = Not bDirCol
            End If
            '-------------------------------------------------------------------
            ' �ړ��������r���擾
            '-------------------------------------------------------------------
            Dim lstKey As New List(Of clsDataMap.LIST_KEY)
            Dim lstVal As New List(Of TYP_MAP_LIST_VALUE)

            ' ���������͎������܂߂�2���B
            Dim bRet As Boolean = mcls_DataMap.FindDefect(listkey, m_typSelectValue(0), bDirCol, bDir, 3, lstKey, lstVal, nMen, Stage)
            If Not bRet Or lstVal.Count < 2 Then
                ' �������s�A�܂��́A�������ʂ�2������
                Exit Try
            End If

            For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1             ' �r����O�̑I���r�ɂ���
                m_typSelectValue(ii) = m_typSelectValue(ii - 1)
            Next
            m_typSelectValue(0) = lstVal(1)

            If Not IsVisibleOnMap(m_typSelectValue(0)) Then
                ' �I���r���܂ރZ�����}�b�v�ɕ\������Ă��Ȃ�
                Dim nValue As Integer = lstKey(1).nRow
                If bDir AndAlso nValue > (m_typMapInf.dgvMap.DisplayedRowCount(False) - 1) Then
                    nValue = nValue - (m_typMapInf.dgvMap.DisplayedRowCount(False) - 1)
                End If
                If (m_nMaxRowCnt - nValue) < m_typMapInf.dgvMap.DisplayedRowCount(False) Then
                    nValue = m_nMaxRowCnt - m_typMapInf.dgvMap.DisplayedRowCount(False)
                End If
                Dim nMaxRowNum As Integer
                SetMapData(nMen, nValue, nMaxRowNum)
                If MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                    m_typMapInf.vsbMap.ValueProgramable = nValue
                Else
                    m_typMapInf.vsbMap.ValueProgramable = nMaxRowNum - (nValue + m_typMapInf.dgvMap.DisplayedRowCount(False))
                End If
            Else
                m_typMapInf.dgvMap.Refresh()
            End If

            '�N���b�N�C�x���g���s
            RaiseEvent SelectMapCell_Click(nMen, m_typSelectValue(0))

            LstSelectClear()

            If lstKey(0) <> lstKey(1) Then
                '���X�g�L�[���قȂ�ꍇ�́A�[�����X�g���X�V
                Dim nRow As Integer
                Dim nCol As Integer
                Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)
                If bDeepDisp Then
                    ' �[�����X�g�\��
                    SetDeepList(m_typMapInf.dgvDeep, CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE)), bDir)
                End If
            End If

            ' �I���r��[�����X�g��ɉ��ƂȂ�悤�ɂ���
            DeepDispVisible(m_typSelectValue(0))

            bResult = True

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�̑I��ύX���ɗ�O [{0}] Dir={1}", ex.Message, bDir))
        End Try

        Return bResult
    End Function


    ''' <summary>
    ''' �r�}�b�v�̉����Ɋ܂܂�Ă��邩���m�F
    ''' </summary>
    ''' <param name="val">�r���</param>
    ''' <returns></returns>
    Private Function IsVisibleOnMap(ByVal val As TYP_MAP_LIST_VALUE) As Boolean

        ' �ŐV�̑I���r���擾
        Dim kizukey As clsDataMap.KIZU_KEY
        kizukey.nKizuNo = val.nKizuNo
        kizukey.nStage = Stage
        If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
            Return False
        End If
        Dim listkey As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)

        Dim nTopRow As Integer = GetMapRowTop()
        Dim nBotRow As Integer = GetMapRowBot()
        If nTopRow <= listkey.nRow And nBotRow >= listkey.nRow Then
            Return True
        End If

        Return False

    End Function

    ''' <summary>
    ''' �w�肵���r�����X�g��ɑ��݂���ꍇ�͉��ł���悤�ɂ���
    ''' </summary>
    ''' <param name="val">�r���</param>
    Private Sub DeepDispVisible(ByVal val As TYP_MAP_LIST_VALUE)

        With m_typMapInf.dgvDeep
            Dim lst As List(Of TYP_MAP_LIST_VALUE) = CType(.Tag, List(Of TYP_MAP_LIST_VALUE))
            If lst Is Nothing Then
                Exit Sub
            End If
            Dim nDispCount As Integer = .DisplayedRowCount(False)
            If lst.Count <= nDispCount Then
                .Refresh()
                Exit Sub
            End If

            For ii As Integer = 0 To lst.Count - 1
                If lst(ii) <> val Then
                    Continue For
                End If
                If ii < .FirstDisplayedScrollingRowIndex Then
                    .FirstDisplayedScrollingRowIndex = ii
                ElseIf ii > .FirstDisplayedScrollingRowIndex + nDispCount - 1 Then
                    .FirstDisplayedScrollingRowIndex = ii - nDispCount + 1
                Else
                    .Refresh()
                End If
                Exit For
            Next
        End With
    End Sub

    ''' <summary>
    ''' ���r�A�O�r�̗L�����m�F
    ''' </summary>
    ''' <param name="nMen">0:�\�A1:��</param>
    ''' <param name="val">�r���</param>
    ''' <param name="bDir">True:���Afalse:�O</param>
    ''' <returns></returns>
    Public Function IsExistEachDef(ByVal nMen As Integer, ByVal val As TYP_MAP_LIST_VALUE, ByVal bDir As Boolean) As Boolean

        Dim bExist As Boolean = False

        Try

            ' �ŐV�̑I���r���擾
            Dim kizukey As clsDataMap.KIZU_KEY
            kizukey.nKizuNo = val.nKizuNo
            kizukey.nStage = Stage
            If Not mcls_DataMap.ListKizuNoKeyUmu(kizukey) Then
                Exit Try
            End If
            Dim listkey As clsDataMap.LIST_KEY = mcls_DataMap.ListGetKizuNoKey(kizukey)

            '-------------------------------------------------------------------
            ' ����������̌���
            '-------------------------------------------------------------------
            Dim bDirCol As Boolean = CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False))
            If Not bDir Then
                bDirCol = Not bDirCol
            End If

            '-------------------------------------------------------------------
            ' �����s�����̌���
            '-------------------------------------------------------------------
            Dim bDirRow As Boolean = True   '�f�[�^��̏ォ�牺
            If Not bDir Then
                bDirRow = Not bDirRow
            End If

            '-------------------------------------------------------------------
            ' �r������
            '-------------------------------------------------------------------
            Dim lstKey As New List(Of clsDataMap.LIST_KEY)
            Dim lstVal As New List(Of TYP_MAP_LIST_VALUE)
            ' ���������͎������܂߂�2���B
            Dim bRet As Boolean = mcls_DataMap.FindDefect(listkey, val, bDirCol, bDirRow, 2, lstKey, lstVal, nMen, Stage)
            If bRet AndAlso 1 < lstVal.Count Then
                ' �������ʂ�1���ȏ�
                bExist = True
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���r/�O�r�̗L�����m�F���ɗ�O [{0}] Dir={1}", ex.Message, bDir))
        End Try

        Return bExist
    End Function

    ''' <summary>
    ''' �r�}�b�v�s�𖳌��\�����܂�
    ''' </summary>
    ''' <param name="nRowNo">�s�ԍ� 0�I���W��</param>
    ''' <remarks></remarks>
    Private Sub DspRowDisable(ByVal nRowNo As Integer)
        Try

            For col As Integer = 0 To m_typMapInf.dgvMap.ColumnCount - 1                ' �R���g���[���F�ɐݒ肷��
                With m_typMapInf.dgvMap(col, nRowNo)
                    .Value = ""
                    .Style.BackColor = g_ColorControl                                   ' �w�i�F�̐ݒ�
                    .Style.SelectionBackColor = g_ColorControl                          ' �I�����w�i�F�̐ݒ�
                    If EM_MAP_COL_INF.IRIGAWA = col Then
                        .Tag = Nothing
                    End If
                End With
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�s�\���������ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �[�����X�g�ĕ`��
    ''' </summary>
    Public Sub ReDrawDeepList()

        If 0 < m_typSelectValue(0).nPnt Then
            Dim nRow As Integer
            Dim nCol As Integer
            Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)
            If bDeepDisp Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                ' �[�����X�g�̕\��
                SetDeepList(m_typMapInf.dgvDeep, lstVal)
                Exit Sub
            End If
        End If

    End Sub

End Class
