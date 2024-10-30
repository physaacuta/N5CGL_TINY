'*******************************************************************************
'   �r�}�b�v�\��
'
'   [Ver]
'       Ver.01  2013/12/25  ����
'
'   [����]
'       tMod_MemSet.MemSet�֐����g�p���Ă��邽��
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
''' �r�}�b�v�\���N���X
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
    ''' �����r�}�b�v�r���X�g�\����
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_CYC_LIST_COL
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

    ''' <summary>
    ''' �r�̑I���ʒu
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_SELECT_POS
        emKensaAto = 0              ' �������
        emKensaMae                  ' ������O
        emEnd
    End Enum

    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    '�}�b�v���
    Public Structure TYP_MAP_INF
        Dim dgvMap As tCnt_DataGridView_turn                '�r�}�b�v
        Dim dgvDeep As tCnt_DataGridView_turn               '�r�[�����X�g
        Dim dgvTcm As tCnt_DataGridView_turn                'TCM���X�g
        Dim vsbMap As tCnt_VScrollBar                       '�X�N���[���o�[
        Dim lblKensaPoint As Label                          '������
    End Structure


    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                        '���\�[�X�J���t���O[True:�J����,False:���J��]
    Private m_emStage As EM_LONG_POS                                '����Ԋu(1:���A2:��)
    Private m_emMapDir As EM_MAP_SCROOL                             '�}�b�v�̐i�s����
    Private m_emSide As EM_MAP_SIDE                                 '�r���x��DS�ʒu
    Private m_DSPos As EM_MAP_CELL                                  '�r�}�b�vDS�ʒu[NORMAL:����DS�AREVERSE:����WS]
    Private m_nDspMen As Integer                                    '�\����
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>	
	Private m_emWidthNo As EM_MAP_WIDTH_NO                          ' �}�b�v���ʒu�ԍ�[NORMAL:������1,2,�E�E�E�AREVERSE:�E����1,2,�E�E�E]
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q <<<<<--------------------

    Private m_emViewMode As EM_REFRESH_MODE                         '�r�}�b�v�X�V���[�h
    Private m_nKensaNo As Integer                                   '������ԍ�
    Private m_bKensaDsp As Boolean                                  '������\���L��
    Private m_emMapMode As EM_DEFECT_MODE                           '�r�}�b�v�\�����[�h
    ' 20201010 ������ʒu�\���ύX --->>>
    Private m_nKensaPos As Integer                                  '������\���ʒu
    ' 20201010 ������ʒu�\���ύX <<<---

    Private m_typMapInf As TYP_MAP_INF                              '�r�}�b�v
    Private mcls_Log As tClass_LogManager                           '���O�Ǘ�
    Private mcls_Param As clsParamManager                           '�p�����[�^�Ǘ�
    Private mcls_PreParam As clsPreParamManager                     '�O�H���p�����[�^�Ǘ�
    Private mcls_PointInf As clsPointInf                            '�|�C���^���
    Private mcls_CoilInf As clsDispCoil                             '�R�C�����

	Private m_nDispRec(TBL_HY_REC_MAX - 1) As Integer               '�\���Ώۃ��R�[�h

	Private m_typSelectValue(DSP_DEFECT_MAX - 1) As TYP_MAP_LIST_VALUE   '�I���r
    Private m_typTcmSelectValue As TYP_MAP_LIST_VALUE               'TCM�I���r

    Private m_nMaxRowCnt As Integer                                 '�ő�s��

    Private m_posSelectValue(DSP_DEFECT_MAX - 1) As EM_SELECT_POS   '�I���r�̌�����ʒu

    Private m_bcheck As Boolean = False
    '///////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '///////////////////////////////////////////////////////////////////////////
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
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "2"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "3"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "4"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "5"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "6"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "7"
			'Else
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_2).HeaderText = "7"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_3).HeaderText = "6"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_4).HeaderText = "5"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_5).HeaderText = "4"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_6).HeaderText = "3"
			'	m_typMapInf.dgvMap.Columns(EM_MAP_COL_INF.COL_7).HeaderText = "2"
			'End If
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q <<<<<--------------------
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
    ''' �ʔ|�C���g�ʒu
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property KensaNo() As Integer
        Get
            Return m_nKensaNo
        End Get
        Set(ByVal value As Integer)
            m_nKensaNo = value
        End Set
    End Property

    ''' <summary>
    ''' ������\���L��
    ''' </summary>
    ''' <value>������ʒu</value>
    ''' <returns>������ʒu</returns>
    ''' <remarks></remarks>
    Public Property KensaDsp() As Boolean
        Get
            Return m_bKensaDsp
        End Get
        Set(ByVal value As Boolean)
            m_bKensaDsp = value
        End Set
    End Property

    ' 20201010 ������ʒu�\���ύX --->>>
    ''' <summary>
    ''' ������\���s
    ''' </summary>
    ''' <value>1�`50:������ʒu</value>
    ''' <returns>1�`50:������ʒu</returns>
    ''' <remarks></remarks>
    Public Property KensaPos() As Integer
        Get
            Return m_nKensaPos
        End Get
        Set(ByVal value As Integer)
            m_nKensaPos = value
        End Set
    End Property
    ' 20201010 ������ʒu�\���ύX <<<---

    ''' <summary>
    ''' ��ʍX�V���[�h
    ''' </summary>
    ''' <value>AUTO(0):�����X�V���[�h PAUSE(1):�ꎞ��~���[�h</value>
    ''' <returns>AUTO(0):�����X�V���[�h PAUSE(1):�ꎞ��~���[�h</returns>
    ''' <remarks></remarks>
    Public Property RefreshMode() As EM_REFRESH_MODE
        Get
            Return m_emViewMode
        End Get
        Set(ByVal value As EM_REFRESH_MODE)
            m_emViewMode = value
            m_typMapInf.dgvMap.Refresh()
        End Set
    End Property

    ''' <summary>
    ''' �����pF01�{�^�������ꂽ�t���O
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property checkcheck() As Boolean
        Get
            Return m_bcheck
        End Get
        Set(ByVal value As Boolean)
            m_bcheck = value
        End Set
    End Property

    ''' <summary>
    ''' �r�}�b�v�X�V���[�h
    ''' </summary>
    ''' <value>TRACKING(0):�g���b�L���O�r�摜�\�� REALTIME(1):���A���^�C���r�摜�\��</value>
    ''' <returns>TRACKING(0):�g���b�L���O�r�摜�\�� REALTIME(1):���A���^�C���r�摜�\��</returns>
    ''' <remarks></remarks>
    Public Property MapUpdateMode() As EM_DEFECT_MODE
        Get
            Return m_emMapMode
        End Get
        Set(ByVal value As EM_DEFECT_MODE)
            m_emMapMode = value
        End Set
    End Property

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	''' <summary>
	''' �}�b�v�\���Ώۃ��R�[�h
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property MapDispRec() As Integer()
		Get
			Return m_nDispRec
		End Get
	End Property
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

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

    '''' <summary>
    '''' 
    '''' </summary>
    '''' <param name="nMen"></param>
    '''' <param name="pntKizu"></param>
    '''' <remarks></remarks>
    'Public Event SelectMapCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="pntKizu"></param>
    ''' <param name="bSelect"></param>
    ''' <remarks></remarks>
    Public Event SelectMapCell_Click(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal bSelect As Boolean)

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


    '///////////////////////////////////////////////////////////////////////////
    '�����o�֐�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="typMapInf"></param>
    ''' <param name="pointer"></param>
    ''' <param name="coil"></param>
    ''' <param name="param"></param>
    ''' <param name="preparam"></param>
    ''' <param name="log"></param>
    ''' <param name="nFormID"></param>
    Public Sub New(ByVal typMapInf As TYP_MAP_INF, ByRef param As clsParamManager, ByRef preparam As clsPreParamManager, ByRef log As tClass_LogManager, ByRef pointer As clsPointInf, ByRef coil As clsDispCoil, ByVal nFormID As Integer)
        Try
            m_typMapInf = typMapInf
            mcls_Param = param
            mcls_PreParam = preparam
            mcls_PointInf = pointer
            mcls_CoilInf = coil
            mcls_Log = log


            For ii As Integer = 0 To m_typSelectValue.Length - 1
                m_typSelectValue(ii).Initialize()
            Next
            m_typTcmSelectValue.Initialize()

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
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�\�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

#Region " IDisposable Support "
    ''' <summary>
    ''' �f�X�g���N�^
    ''' </summary>
    ''' <param name="disposing">True:�����I�ȌĂяo�� False:�Öق̌Ăяo��</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                '�I�u�W�F�N�g�j��
            End If
            m_bIsDesposed = True                                                'Dispose�ς݂Ƃ���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�u�W�F�N�g�j���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    '���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        '���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
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
            .RowCount = TANSHO_MAP_DSP_ROW_MAX
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

            '�w�i�F
            .BackgroundColor = Color.White

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
    ''' �ő�s��������
    ''' </summary>
    Public Sub ClearMaxRowCnt()
        m_nMaxRowCnt = 0
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
    ''' <param name="nMen">0:�\�� 1:����</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function DspData(ByVal nMen As Integer) As Boolean
        Dim bReturn As Boolean = False

        Try
            m_nMaxRowCnt = 0

            '��r�p�ɕύX�O�̏����R�s�[
            Dim nSelectValue(m_typSelectValue.Length - 1) As TYP_MAP_LIST_VALUE
            For ii As Integer = 0 To m_typSelectValue.Length - 1
                nSelectValue(ii) = m_typSelectValue(ii)
            Next
            Dim nTcmSelectValue As TYP_MAP_LIST_VALUE = m_typTcmSelectValue

            '�����X�V��
            If m_emViewMode = EM_REFRESH_MODE.AUTO Then
                For ii As Integer = 0 To m_typSelectValue.Length - 1
                    m_typSelectValue(ii).Initialize()
                Next
                m_typTcmSelectValue.Initialize()
            End If

            '�\���Ώۃ��R�[�h�擾
            If Not GetDispRecord(nMen) Then
                MapInit()
                Exit Try
            End If

			'�\���Ώۃ��R�[�h����
			If m_nDispRec(0) = 0 Then
                MapInit()
                Exit Try
            End If

			'�r�}�b�v�ݒ�
			If Not SetMapData(nMen, 0, m_nMaxRowCnt) Then
                Exit Try
            End If

            Dim nRecDisp As Integer = mcls_PointInf.RecDisp(m_nKensaNo)
            If Not mcls_PointInf.KizukenNo(nRecDisp) Is Nothing And m_emViewMode = EM_REFRESH_MODE.AUTO Then
                '�ŐV�r��������
                GetSearchKizuFirst(nMen)
                GetSearchTcmKizuFirst(nMen)
            End If

            ''�r�����I���C�x���g����
            'If 0 < m_typSelectValue(0).nPnt And nSelectValue(0) <> m_typSelectValue(0) Then
            '    ' �ŐV�r�I�����ω������ꍇ�̓}�b�v�ʒu�̕ω����m�F
            '    Dim nRow As Integer
            '    Dim nCol As Integer
            '    Dim bDeepDisp As Boolean = GetMapPos(m_typSelectValue(0), nRow, nCol)

            '    If bDeepDisp Then
            '        Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nCol, nRow).Tag, List(Of TYP_MAP_LIST_VALUE))
            '        ' �[�����X�g�̕\��
            '        SetDeepList(m_typMapInf.dgvDeep, lstVal)
            '    End If
            'End If

            Dim nDeepRow As Integer = 0
            Dim nDeepCol As Integer = 0
            Dim bDeepFlg As Boolean = False
            For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
                '' �r�I�����ω������ꍇ�̓}�b�v�ʒu�̕ω����m�F
                If (0 < m_typSelectValue(ii).nPnt And nSelectValue(ii) <> m_typSelectValue(ii)) Or bDeepFlg Then
                    Dim nRowWk As Integer
                    Dim nColWk As Integer
                    If GetMapPos(m_typSelectValue(ii), nRowWk, nColWk) Then
                        nDeepRow = nRowWk
                        nDeepCol = nColWk
                        bDeepFlg = True
                    End If
                End If
            Next
            If bDeepFlg Then
                Dim lstVal As List(Of TYP_MAP_LIST_VALUE) = CType(m_typMapInf.dgvMap.Item(nDeepCol, nDeepRow).Tag, List(Of TYP_MAP_LIST_VALUE))
                ' �[�����X�g�̕\��
                SetDeepList(m_typMapInf.dgvDeep, lstVal)
            End If

            '�I���O�ƂȂ����Ƃ����X�g��������
            LstSelectClear()

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
        For ii As Integer = m_typSelectValue.Length - 1 To 0 Step -1
            Dim nRowWk As Integer
            Dim nColWk As Integer
            If GetMapPos(m_typSelectValue(ii), nRowWk, nColWk) Then
                bDeepDelFlg = True
            End If
        Next ii
        If Not bDeepDelFlg Then
            ClearDeep()
        End If
        m_typMapInf.dgvDeep.Refresh()

    End Sub

    ''' <summary>
    ''' �\���Ώۃ��R�[�h�擾
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetDispRecord(ByVal nMen As Integer) As Boolean
        Dim bRet As Boolean = False
        Dim nDispRec As Integer                             '�\�����R�[�h
        Dim nCoilCnt As Integer = 0

        Try
            '������
            For ii As Integer = 0 To TBL_HY_REC_MAX - 1
                m_nDispRec(ii) = 0
            Next

            '������̌��ݕ\�����R�[�h�擾
            nDispRec = mcls_PointInf.RecDisp(m_nKensaNo)
            If nDispRec = 0 Then
                Exit Try
            End If

            ' 20201010 ������ʒu�\���ύX --->>>
            '�g���b�L���O�\���̏ꍇ�́A�\����_�`������ʒu �� ������\���s�̍������炷�B
            If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' �g���b�L���O�\�� 
                Dim nRec As Integer = 0         ' �\���J�n���R�[�h�擾�p
                Dim nRow As Integer = 0         ' �\���J�n�s�擾�p
                ' �g���b�L���O�p�\���J�n�ʒu�擾
                If GetDispStartRecAndRow(nMen, nRec, nRow) Then
                    nDispRec = nRec
                End If
            End If
            ' 20201010 ������ʒu�\���ύX <<<---

            '�\���Ώۃ��R�[�h���W
            For ii As Integer = 0 To TBL_HY_REC_MAX - 1     '���R�[�h����
                If ii > DSP_COIL_MAX Then                   '�\������
                    Exit For
                End If
                '�\���\�܂��͕\���\��
                If mcls_PointInf.DispStatus(nDispRec) = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                Or mcls_PointInf.DispStatus(nDispRec) = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then
                    m_nDispRec(nCoilCnt) = nDispRec         '���R�[�h�i�[
                    nCoilCnt = nCoilCnt + 1
                End If
                '�O��\�����R�[�h�擾
                nDispRec = clsPointInf.GetPrevRecNo(nDispRec)
            Next
            '��������
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r�}�b�v�\�������ُ�[{0}]", ex.Message))
        End Try

        Return bRet
    End Function


    ''' <summary>
    ''' �r�}�b�v���\��
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="nMove">�ړ���</param>
    ''' <param name="nMaxRowNum">�ő��</param>
    ''' <remarks></remarks>
    Public Function SetMapData(ByVal nMen As Integer, ByVal nMove As Integer, ByRef nMaxRowNum As Integer) As Boolean
        Dim nRowDisp As Integer                             '���ݕ\���s
        Dim nRowPos As Integer                              '�f�[�^�s�ʒu
        Dim nDgvPos As Integer = 0                          'DGV�s�ʒu
        Dim nRecDisp As Integer                             '�\�����R�[�h
        Dim nStartID As Integer = 0                         '�J�n���R�[�h�ʒu
        Dim nRecID As Integer = 0                           '�\�����R�[�h�ʒu
        Dim nCoilCnt As Integer = 0                         '�\���R�C����
        Dim bStart As Boolean = False                       '�����J�n�t���O
        Dim nKensaPos As Integer = 0                        '������ʒu
        Dim nRowCheck As Integer = 0
        Dim nRowCount As Integer = 0
        Dim nStage As Integer = CInt(Stage)
        Dim nRowCnt As Integer = 0
        Dim bRet As Boolean = False


        Try
            If True = mcls_PointInf.Refresh Then
                Exit Try
            End If

            '������
            ClearMap()

            '�\�����R�[�h�擾
            nRecDisp = mcls_PointInf.RecDisp(m_nKensaNo)

            '���ݕ\���s�擾
            nRowDisp = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, nStage)

            ' 20201010 ������ʒu�\���ύX --->>>
            '�g���b�L���O�\���̏ꍇ�́A�\����_�`������ʒu �� ������\���s�̍������炷�B
            If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' �g���b�L���O�\�� 
                Dim nRec As Integer = 0         ' �\���J�n���R�[�h�擾�p
                Dim nRow As Integer = 0         ' �\���J�n�s�擾�p
                ' �g���b�L���O�p�\���J�n�ʒu�擾
                If GetDispStartRecAndRow(nMen, nRec, nRow) Then
                    nRowDisp = nRow
                End If
            End If
            ' 20201010 ������ʒu�\���ύX <<<---

            '�}�b�v�\�����[�v
            For ii As Integer = 0 To TBL_HY_REC_MAX - 1
                Dim nRec As Integer = m_nDispRec(ii)
                If nRec <= 0 Then                           '�������R�[�h
                    Exit For
                End If
                If nCoilCnt >= DSP_COIL_MAX Then            '�\���R�C��������
                    Exit For
                End If
                If nRec = m_nDispRec(nStartID) Then         '�J�n���R�[�h
                    bStart = True                           '�����J�n
                End If
                If Not bStart Then                           '�����J�n
                    Continue For
                End If
                If nCoilCnt = 0 Then
                    nRowCnt = nRowCnt + nRowDisp
                    nRowPos = nRowDisp + nMove
                Else
                    Dim LPos As Integer = mcls_PointInf.RowLast(nRec, nStage)                 ' ���������s
                    Dim WPos As Integer = mcls_PointInf.RowWrite(nRec, nMen, nStage) + 1      ' ���ݏ������ݍs
                    Dim DPos As Integer = mcls_PointInf.RowDisp(nRec, m_nKensaNo, nStage)     ' ���ݕ\���s
                    Dim nPos As Integer = 0
                    If LPos > 0 Then
                        '���������s�Z�b�g
                        WPos = LPos
                    End If

                    If LPos <> 0 Then
                        nPos = LPos
                    Else
                        nPos = DPos
                    End If

                    nRowCnt = nRowCnt + nPos

                    If nRowPos > -1 Then
                        nRowPos = nPos
                    Else
                        nRowPos = nPos + nRowPos
                    End If
                End If
                If m_typMapInf.dgvMap.RowCount > nDgvPos Then
                    DspMap(nMen, nStage, nRowPos, nDgvPos, nRec)
                End If
                If checkcheck Then
                    mcls_Log.Write(LNO.MSG, String.Format("���� {0}�{�� �s{1}({2}) rec={3} �Ǘ�No=[{4}] men={5} nmove={6}" _
                                                          , nCoilCnt + 1, nRowPos, nRowCnt, nRec, mcls_PointInf.KizukenNo(nRec), nMen, nMove))
                End If
                nCoilCnt = nCoilCnt + 1                                     '�\���R�C����
            Next

            '�ړ��ʂ�0�̏ꍇ�ɁA�ő�s�����X�V����
            nMaxRowNum = nRowCnt

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
        Dim nColMost As Integer
        Dim nRowStart As Integer
        Dim nRowEnd As Integer
        Dim nRowStep As Integer
        Dim nChkCnt As Integer = 0

        '������̍s���擾(�P�I���W��)
        Dim nKensaRow As Integer = GetKensaRow() + 1

        Dim bDispRec As Boolean
        Dim nRowDisp As Integer                             '������ʒu�̌��ݕ\���s
        Dim nRowLast As Integer                             '���������s
        Dim nRecDisp As Integer                             '������ʒu�̌��ݕ\�����R�[�h
        Dim ii As Integer = 0                               '���R�[�h�J�E���g

        ' 20201010 ������ʒu�\���ύX --->>>
        If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' �g���b�L���O�\�� 
            nRecDisp = m_nDispRec(0)
            nRowDisp = mcls_PointInf.RowDisp(m_nDispRec(0), m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(m_nDispRec(0), m_emStage)
        Else
            nRecDisp = mcls_PointInf.RecDisp(m_nKensaNo)
            nRowDisp = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(nRecDisp, m_emStage)
        End If
        ' 20201010 ������ʒu�\���ύX <<<---

        Dim nRec As Integer = 0
        For ii = 0 To DSP_COIL_MAX - 1                      '�\���R�C�������[�v
            nRec = m_nDispRec(ii)                           '�Ώۃ��R�[�h

            If nRec <= 0 Then
                Exit For
            End If


            If nRowLast > 0 Then
                If nRowDisp - 1 > nRowLast Then
                    If nRec = m_nDispRec(0) Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                Else
                    If nRec = nRecDisp Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                End If
            Else
                If nRec = m_nDispRec(0) Then
                    bDispRec = True
                Else
                    bDispRec = False
                End If
            End If

            If bDispRec Then
                If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, 0).Tag, G_MAP_ROW).nMemRowPos
                Else
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, TANSHO_MAP_DSP_ROW_MAX - 1).Tag, G_MAP_ROW).nMemRowPos
                End If
            Else
                If mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) > mcls_PointInf.RowLast(nRec, m_emStage) Then
                    nRowStart = mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) - 1
                Else
                    nRowStart = mcls_PointInf.RowLast(nRec, m_emStage) - 1
                    ' 20201010 ������ʒu�\���ύX(�s�v���W�b�N�̂��ߍ폜) --->>>
                    'If m_emStage = EM_LONG_POS.BIG Then
                    '    nRowStart = nRowStart + 10
                    'End If
                    ' 20201010 ������ʒu�\���ύX <<<---
                End If
            End If

            nRowEnd = 0
            nRowStep = -1

            'Dim nMapColNum As Integer = GetMapColNum(nRec)

            For nCnt As Integer = nRowStart To nRowEnd Step nRowStep

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

                GetMapRowData(nRec, nMen, m_emStage, nCnt, typRow, lstCell, lstTcmCell)

                nColMost = -1

                If nChkCnt < nKensaRow Then             ' ������ȑO���r�H

                    ' �ŐV�r�擾
                    nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec)
                    If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                        If 0 < lstCell(nColMost)(0).nPnt Then
                            m_typSelectValue(1) = m_typSelectValue(0)
                            m_typSelectValue(0) = lstCell(nColMost)(0)
                            m_posSelectValue(1) = m_posSelectValue(0)
                            m_posSelectValue(0) = EM_SELECT_POS.emKensaMae
                        End If
                    End If

                    ' ���r�擾
                    nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec, nColMost)
                    If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                        If m_typSelectValue(0) <> lstCell(nColMost)(0) Then
                            If 0 < lstCell(nColMost)(0).nPnt Then
                                m_typSelectValue(1) = lstCell(nColMost)(0)
                                m_posSelectValue(1) = EM_SELECT_POS.emKensaMae
                            End If
                        End If
                    End If
                Else
                    If 0 >= m_typSelectValue(0).nPnt Then

                        ' �ŐV�r�����肳��Ă��Ȃ���
                        nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec)
                        If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                            If 0 < lstCell(nColMost)(0).nPnt Then
                                m_typSelectValue(0) = lstCell(nColMost)(0)
                                m_posSelectValue(0) = EM_SELECT_POS.emKensaAto
                            End If
                        End If
                    End If
                    If 0 < m_typSelectValue(0).nPnt And 0 >= m_typSelectValue(1).nPnt Then

                        ' �ŐV�r�����肵�Ă��āA1�O���r�����肵�Ă��Ȃ��ꍇ
                        nColMost = GetMostDeffectColumn(nMen, lstCell, typRow, nRec, nColMost)
                        If 0 <= nColMost And MAP_COL_NUM > nColMost Then
                            If m_typSelectValue(0) <> lstCell(nColMost)(0) Then
                                If 0 < lstCell(nColMost)(0).nPnt Then
                                    m_typSelectValue(1) = lstCell(nColMost)(0)
                                    m_posSelectValue(1) = EM_SELECT_POS.emKensaAto
                                    Return True ' �Ώۂ��r���Q���������̂Ō����I��
                                End If
                            End If
                        End If
                    End If
                End If

                nChkCnt = nChkCnt + 1
            Next
        Next
        Return True
    End Function

    ''' <summary>
    ''' �ŐV�r����
    ''' </summary>
    ''' <param name="nMen">�\���ʔԍ�</param>
    ''' <returns>True�F���݂���AFalse�F���݂��Ȃ�</returns>
    ''' <remarks></remarks>
    Private Function GetSearchTcmKizuFirst(ByVal nMen As Integer) As Boolean
        Dim nColMost As Integer
        Dim nRowStart As Integer
        Dim nRowEnd As Integer
        Dim nRowStep As Integer
        Dim nChkCnt As Integer = 0

        '������̍s���擾(�P�I���W��)
        Dim nKensaRow As Integer = GetKensaRow() + 1

        Dim bDispRec As Boolean
        Dim nRowDisp As Integer                             '������ʒu�̌��ݕ\���s
        Dim nRowLast As Integer                             '���������s
        Dim nRecDisp As Integer                             '������ʒu�̌��ݕ\�����R�[�h
        Dim ii As Integer = 0                               '���R�[�h�J�E���g

        ' 20201010 ������ʒu�\���ύX --->>>
        If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' �g���b�L���O�\�� 
            nRecDisp = m_nDispRec(0)
            nRowDisp = mcls_PointInf.RowDisp(m_nDispRec(0), m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(m_nDispRec(0), m_emStage)
        Else
            nRecDisp = mcls_PointInf.RecDisp(m_nKensaNo)
            nRowDisp = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, m_emStage)
            nRowLast = mcls_PointInf.RowLast(nRecDisp, m_emStage)
        End If
        ' 20201010 ������ʒu�\���ύX <<<---

        Dim nRec As Integer = 0
        For ii = 0 To DSP_COIL_MAX - 1                      '�\���R�C�������[�v
            nRec = m_nDispRec(ii)                           '�Ώۃ��R�[�h

            If nRec <= 0 Then
                Exit For
            End If

            If nRowLast > 0 Then
                If nRowDisp - 1 > nRowLast Then
                    If nRec = m_nDispRec(0) Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                Else
                    If nRec = nRecDisp Then
                        bDispRec = True
                    Else
                        bDispRec = False
                    End If
                End If
            Else
                If nRec = m_nDispRec(0) Then
                    bDispRec = True
                Else
                    bDispRec = False
                End If
            End If

            If bDispRec Then
                If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, 0).Tag, G_MAP_ROW).nMemRowPos
                Else
                    nRowStart = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, TANSHO_MAP_DSP_ROW_MAX - 1).Tag, G_MAP_ROW).nMemRowPos
                End If
            Else
                If mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) > mcls_PointInf.RowLast(nRec, m_emStage) Then
                    nRowStart = mcls_PointInf.RowDisp(nRec, m_nKensaNo, m_emStage) - 1
                Else
                    nRowStart = mcls_PointInf.RowLast(nRec, m_emStage) - 1
                    ' 20201010 ������ʒu�\���ύX(�s�v���W�b�N�̂��ߍ폜) --->>>
                    'If m_emStage = EM_LONG_POS.BIG Then
                    '    nRowStart = nRowStart + 10
                    'End If
                    ' 20201010 ������ʒu�\���ύX <<<---
                End If
            End If

            nRowEnd = 0
            nRowStep = -1

            For nCnt As Integer = nRowStart To nRowEnd Step nRowStep

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

                GetMapRowData(nRec, nMen, m_emStage, nCnt, typRow, lstCell, lstTcmCell)

                nColMost = -1
                If nChkCnt < nKensaRow Then            ' ������ȑO���r�H
                    ' �ŐVTCM�r�擾
                    If lstTcmCell(0) IsNot Nothing AndAlso 0 < lstTcmCell(0)(0).nPnt Then
                        m_typTcmSelectValue = lstTcmCell(0)(0)
                    End If
                Else                                    ' �����������r
                    If 0 >= m_typTcmSelectValue.nPnt Then
                        ' �ŐVTCM�r�����肳��Ă��Ȃ���
                        If lstTcmCell(0) IsNot Nothing AndAlso 0 < lstTcmCell(0)(0).nPnt Then
                            m_typTcmSelectValue = lstTcmCell(0)(0)
                            Return True ' �Ώ�TCM�r�����������̂Ō����I��
                        End If
                    Else
                        Return True ' ������O�ɑΏ�TCM�r���������Ă���̂Ō����I��
                    End If
                End If

                nChkCnt = nChkCnt + 1
                If nChkCnt > m_typMapInf.dgvMap.DisplayedRowCount(True) Then
                    '�\���͈͂𒴂����̂Ō����I��
                    Return True
                End If
            Next
        Next
        Return True
    End Function

    ''' <summary>
    ''' �}�b�v���Z�b�g
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="nStage">�X�e�[�W</param>
    ''' <param name="nMapPos">���ݕ\���s</param>
    ''' <param name="nDgvPos">DGV�\���ʒu</param>
    ''' <param name="nRec">���R�[�h�ԍ�</param>
    ''' <remarks></remarks>
    Private Sub DspMap(ByVal nMen As Integer, ByVal nStage As Integer, ByVal nMapPos As Integer, ByRef nDgvPos As Integer, ByVal nRec As Integer)
        Dim nColStart As Integer                            '�J�n��C���f�b�N�X
        Dim nColEnd As Integer                              '�I����C���f�b�N�X
        Dim nStep As Integer                                '�X�e�b�v�Ԋu
        Dim nColCnt As Integer                              '�\��COL�J�E���^
        Dim nRowStart As Integer
        Dim bCoilMax As Boolean = False


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


        ' 20201010 ������ʒu�\���ύX(�s�v���W�b�N�̂��ߍ폜) --->>>
        'Dim nRecDisp As Integer = mcls_PointInf.RecDisp(m_nKensaNo)
        'Dim nRowDisp As Integer = mcls_PointInf.RowDisp(nRecDisp, m_nKensaNo, nStage)
        ' 20201010 ������ʒu�\���ύX <<<---

        Dim nMapColNum As Integer = GetMapColNum(nRec)

        Try
            nRowStart = nMapPos - 1     '0�I���W��

            For nMapRow As Integer = nRowStart To 0 Step -1

                bCoilMax = False

                If nDgvPos >= m_typMapInf.dgvMap.RowCount Then
                    Exit For
                End If

                Dim nDgvRow As Integer = nDgvPos
                If EM_MAP_SCROOL.TAIL_TO_HEAD = MapDir Then         '�������
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

                GetMapRowData(nRec, nMen, nStage, nMapRow, typRow, lstCell, lstTcmCell)

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
                If EM_DIV_KENSA.DIV_KENSA_STOP = typRow.emKensa Then       '��~
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_STOP)
                ElseIf EM_DIV_KENSA.DIV_KENSA_NON = typRow.emKensa Then   '��~(�ΏۊO)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKensaBackColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKensaForeColor(KizuForm.EM_DIV_KENSA.DIV_KENSA_NON)
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
                ElseIf -1 = typRow.nEnable Then                            '��M�x��(�������\��)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorJyusinOkure
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                ElseIf EM_DIV_KIKI.DIV_KIKI_KEI = typRow.emKiki Then       '�y�̏�
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = ucStatusAria.MapKikiBackColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = ucStatusAria.MapKikiForeColor(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI)
                ElseIf 0 = typRow.nEnable Then                             '���T��
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorTanshoNon
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                Else                                                            '����
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.BackColor = g_ColorTanshoOk
                    m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.DEGAWA, nDgvRow).Style.ForeColor = Color.Black
                End If
                m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Style.SelectionBackColor = m_typMapInf.dgvMap.Item(EM_MAP_COL_INF.IRIGAWA, nDgvRow).Style.BackColor
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
                    Dim typTcmData As TYP_MAP_LIST_VALUE = lstTcmCell(0)(0)
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
    ''' <param name="nRec">���R�[�h[1�`]</param>
    ''' <param name="nMen">��[0:�\,1:��]</param>
    ''' <param name="nRow">�s[0�`]</param>
    ''' <returns>�s���</returns>
    ''' <remarks></remarks>
    Private Function GetMapRowData(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nStage As Integer, ByVal nRow As Integer,
                                   ByRef typRow As G_MAP_ROW, ByRef lstCell() As List(Of TYP_MAP_LIST_VALUE),
                                   ByRef lstTcmCell() As List(Of TYP_MAP_LIST_VALUE)) As Boolean

        Dim bRetc As Boolean = False

        Try

            Dim typMapRow_Row As New MAP_ROW_ROW
            If Not GetMapRow_Row(nRec, nMen, nRow, nStage, typMapRow_Row) Then
                Exit Try
            End If

            typRow.initialize()
            typRow.nRec = nRec
            typRow.nMemRowPos = nRow
            typRow.emKensa = typMapRow_Row.typ.emDspKensa
            typRow.emKiki = typMapRow_Row.typ.emDspKiki
            typRow.nLen_i = typMapRow_Row.typ.nLen_i
            typRow.nLen_o = typMapRow_Row.typ.nLen_o
            typRow.nEnable = typMapRow_Row.typ.nEnable
            typRow.nCut = typMapRow_Row.typ.nCut
            If 0 < typMapRow_Row.typ.nKizuCnt Then
                '�r����M�ς�
                typRow.nKizuCnt = typMapRow_Row.typ.nKizuCnt
            Else
                typRow.nKizuCnt = 0
            End If

            If 0 = nRow Or (typMapRow_Row.typ.nKind And ROW_KIND_YOSETU) <> 0 Then
                typRow.bYousetu = True
            End If

            Dim nMapColNum As Integer = GetMapColNum(nRec)
            For ii As Integer = 0 To CInt((MAP_COL_NUM - nMapColNum) / 2) - 1
                '�}�b�v��ΏۊO�i�G�b�W�O�j
                typRow.col(ii).nKizuNo = -2
                typRow.col(MAP_COL_NUM - ii - 1).nKizuNo = -2
            Next
            typRow.tcm(0).nKizuNo = 0

            Dim nMaxY As Integer = typRow.nLen_i * 1000
            Dim nMinY As Integer = 0
            Dim nMaxRow As Integer = MAP_ROW_NUM
            If EM_LONG_POS.BIG = nStage Then
                nMinY = nMaxY - mcls_Param.MapStageBig * 1000
                If 0 < mcls_Param.MapStageBig Then
                    nMaxRow = COIL_LEN_MAX \ mcls_Param.MapStageBig
                    If 0 <> (COIL_LEN_MAX Mod mcls_Param.MapStageBig) Then
                        nMaxRow = nMaxRow + 1
                    End If
                End If
            Else
                nMinY = nMaxY - mcls_Param.MapStageSmall * 1000
                If 0 < mcls_Param.MapStageSmall Then
                    nMaxRow = COIL_LEN_MAX \ mcls_Param.MapStageSmall + 1
                    If 0 <> (COIL_LEN_MAX Mod mcls_Param.MapStageSmall) Then
                        nMaxRow = nMaxRow + 1
                    End If
                End If
            End If
            If MAP_ROW_NUM < nMaxRow Then
                nMaxRow = MAP_ROW_NUM
            End If

            For nPnt As Integer = typMapRow_Row.typ.pnt(0) To typMapRow_Row.typ.pnt(1)
                If 0 >= nPnt Then
                    Continue For
                End If

                Dim typDefInf As DSP_TYP_DEFECTINF = Nothing
                If Not frmTansho.GetDefect(nRec, nPnt, typDefInf, mcls_Log) Then
                    Continue For
                End If

                If 0 = typDefInf.data.nKizuNo Then
                    ' �rNo�Ȃ�
                    Continue For
                End If

                If 1 <> typDefInf.data.nObjTansho Then
                    ' �T����ʑΏۊO(1: �Ώ�)
                    Continue For
                End If

                If nMinY > typDefInf.data.y Then
                    ' ���������ʒu���͈͊O(�������j
                    Continue For
                End If

                If nMaxY <= typDefInf.data.y Then
                    ' ���������ʒu���͈͊O�i�傫���j
                    ' �A���A���̍s�����������s�̏ꍇ�̂݁A���������ʒu���͈͊O�i�傫���j�r���\������B
                    Dim LPos As Integer = mcls_PointInf.RowLast(nRec, nStage)
                    If (LPos <> nRow + 1) Then
                        'MAX�s�ł͂Ȃ�
                        If nRow <> (nMaxRow - 1) Then
                            ' ���̍s�͌��������s�ł͂Ȃ�(���������s��1�I���W��)
                            Continue For
                        End If
                    End If
                End If


                If nMen <> typDefInf.data.TorB Then
                    ' �\���敪
                    Continue For
                End If

                Dim col As Integer = typDefInf.data.nWidDiv
                Dim data As TYP_MAP_LIST_VALUE = Nothing
                data.Initialize()
                data.nRec = nRec
                data.nPnt = nPnt
                data.nKizuNo = typDefInf.data.nKizuNo
                data.nTid = typDefInf.data.nTid
                data.nTYuu = typDefInf.data.nTyuu
                data.nGid = typDefInf.data.nGid
                data.nGYuu = typDefInf.data.nGyuu
                data.nY = CInt(typDefInf.data.y)
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
				data.nY_Out = CInt(typDefInf.data.y_out)
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------                
                data.nAria = CInt(typDefInf.data.aria)
                data.nCycDiv = CInt(typDefInf.data.cycle_div)
                ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
                data.nKouteiID = 0
                ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

                If lstCell(col) Is Nothing Then
                    lstCell(col) = New List(Of TYP_MAP_LIST_VALUE)
                    lstCell(col).Clear()
                End If

                Dim bAdd As Boolean = True
                For ii As Integer = 0 To lstCell(col).Count - 1
                    If nPnt = lstCell(col)(ii).nPnt Then
                        '���ɓo�^�ς�
                        bAdd = False
                        Exit For
                    End If
                Next

                If bAdd Then
                    ' ���X�g�ǉ�
                    lstCell(col).Add(data)
                    If 1 > typMapRow_Row.typ.nKizuCnt Then      ' �r��������M�̏ꍇ
                        typRow.nKizuCnt += 1
                    End If
                End If
            Next

            '��\�r�Z�o(�D�揇�ʂŃ\�[�g)
            For ii As Integer = 0 To lstCell.Length - 1
                If lstCell(ii) Is Nothing Then
                    Continue For
                End If
                lstCell(ii).Sort(AddressOf SortPriorty)
                If 0 < lstCell(ii).Count Then
                    typRow.col(ii).nKizuNo = lstCell(ii)(0).nKizuNo
                    typRow.col(ii).nTid = lstCell(ii)(0).nTid
                    typRow.col(ii).nGid = lstCell(ii)(0).nGid
                End If
            Next ii


            For nPnt As Integer = typMapRow_Row.typ.pntMae(0) To typMapRow_Row.typ.pntMae(1)
                If 0 >= nPnt Then
                    Continue For
                End If

                Dim typDefInf As MAE_INF = Nothing
                If Not frmTansho.GetMaeDefect(nRec, nMen, nPnt, typDefInf, mcls_Log) Then
                    Continue For
                End If

                If 0 = typDefInf.nKizuNo Then
                    ' �rNo�Ȃ�
                    Continue For
                End If

                If nMinY > typDefInf.ff.nY Then
                    ' ���������ʒu���͈͊O(�������j
                    Continue For
                End If

                If nMaxY <= typDefInf.ff.nY Then
                    ' ���������ʒu���͈͊O�i�傫���j
                    ' �A���A���̍s�����������s�̏ꍇ�̂݁A���������ʒu���͈͊O�i�傫���j�r���\������B
                    Dim LPos As Integer = mcls_PointInf.RowLast(nRec, nStage)
                    If (LPos <> nRow + 1) Then
                        'MAX�s�ł͂Ȃ�
                        If nRow <> (nMaxRow - 1) Then
                            ' ���̍s�͌��������s�ł͂Ȃ�(���������s��1�I���W��)
                            Continue For
                        End If
                    End If
                End If


                'If nMen <> typDefInf.data(nSpm).TorB Then
                '    ' �\���敪
                '    Continue For
                'End If

                Dim col As Integer = 0
                Dim data As TYP_MAP_LIST_VALUE = Nothing
                data.Initialize()
                data.nRec = nRec
                data.nPnt = nPnt
                data.nKizuNo = typDefInf.ff.nMaeKizuNo
                data.nTid = typDefInf.ff.nTid

                data.nGid = typDefInf.ff.nGid
                data.nY = CInt(typDefInf.ff.nY)
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
				data.nY_Out = CInt(typDefInf.ff.nY_Out)
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
                data.nAria = 0
                data.nCycDiv = 0
                data.strFFType = tMod.ByteToString(typDefInf.ff.cT)
                data.strFFGrade = tMod.ByteToString(typDefInf.ff.cG)
                data.strProc = tMod.ByteToString(typDefInf.ff.cProc)
                data.nTYuu = mcls_PreParam.GetTypePriority(data.strProc, data.strFFType)
                data.nGYuu = mcls_PreParam.GetGradePriority(data.strProc, data.strFFGrade)
                ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
                data.nKouteiID = typDefInf.ff.nKouteiID
                ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

                If lstTcmCell(col) Is Nothing Then
                    lstTcmCell(col) = New List(Of TYP_MAP_LIST_VALUE)
                    lstTcmCell(col).Clear()
                End If

                Dim bAdd As Boolean = True
                For ii As Integer = 0 To lstTcmCell(col).Count - 1
                    If nPnt = lstTcmCell(col)(ii).nPnt Then
                        '���ɓo�^�ς�
                        bAdd = False
                        Exit For
                    End If
                Next
                If bAdd Then
                    ' ���X�g�ǉ�
                    lstTcmCell(col).Add(data)
                End If
            Next

            '��\�r�Z�o(�D�揇�ʂŃ\�[�g)
            For ii As Integer = 0 To lstTcmCell.Length - 1
                If lstTcmCell(ii) Is Nothing Then
                    Continue For
                End If
                ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
                'lstTcmCell(ii).Sort(AddressOf SortPriorty)
                lstTcmCell(ii).Sort(AddressOf SortPriortyTcm)
                ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<
                If 0 < lstTcmCell(ii).Count Then
                    typRow.tcm(ii).nKizuNo = lstTcmCell(ii)(0).nKizuNo
                    typRow.tcm(ii).strFFType = lstTcmCell(ii)(0).strFFType
                    typRow.tcm(ii).strFFGrade = lstTcmCell(ii)(0).strFFGrade
                End If
            Next ii

            bRetc = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�}�b�v�ʒu�s���擾�ُ�[{0}]", ex.Message))
        End Try

        Return bRetc
    End Function

    ''' <summary>
    ''' �s���̗D��x�����r���擾
    ''' </summary>
    ''' <param name="nMen">��</param>
    ''' <param name="lstCell">�}�b�v���</param>
    ''' <param name="typRow">�}�b�v�ʒu</param>
    ''' <remarks>�D��x�����r�̗�ʒu</remarks>
    Private Function GetMostDeffectColumn(ByVal nMen As Integer, ByVal lstCell() As List(Of TYP_MAP_LIST_VALUE), ByVal typRow As G_MAP_ROW, ByVal nRec As Integer, Optional ByVal nExCol As Integer = -1) As Integer
        Dim nGrPos As Integer = 0
        Dim nKzPos As Integer = 0
        Dim nRetrun As Integer = -1
        Dim bPosChg As Boolean = False
        Dim nGrRank(lstCell.Length - 1) As Integer
        Dim nKzRank(lstCell.Length - 1) As Integer
        Dim nStart As Integer = 0
        Dim nEnd As Integer = lstCell.Length - 1
        Dim nStep As Integer = 1

        Try

            'DS/WS����r�}�b�v��������ŏ������ς��
            If m_DSPos = EM_MAP_CELL.NORMAL Then    '����DS
                nStart = 0
                nEnd = MAP_COL_NUM - 1
                nStep = 1
            Else                                    '����WS
                nStart = MAP_COL_NUM - 1
                nEnd = 0
                nStep = -1
            End If

            nGrPos = nStart

            '�D��x�擾
            For nCnt As Integer = nStart To nEnd Step nStep
                nGrRank(nCnt) = Integer.MaxValue
                nKzRank(nCnt) = Integer.MaxValue
                If nCnt = nExCol Then
                    Continue For
                End If
                If lstCell(nCnt) Is Nothing Then
                    Continue For
                End If
                If 0 = lstCell(nCnt).Count Then
                    Continue For
                End If
                nGrRank(nCnt) = mcls_Param.GetGradePriority(lstCell(nCnt)(0).nGid)
                nKzRank(nCnt) = mcls_Param.GetTypePriority(lstCell(nCnt)(0).nTid)
            Next

            For nCnt As Integer = nStart To nEnd Step nStep
                If nGrRank(nGrPos) >= nGrRank(nCnt) And nGrRank(nCnt) <> Integer.MaxValue Then
                    If nKzRank(nGrPos) >= nKzRank(nCnt) Then
                        If (nKzRank(nGrPos) = nKzRank(nCnt)) Then
                            nGrPos = nGrPos
                            bPosChg = True
                        Else
                            nGrPos = nCnt
                            bPosChg = True
                        End If
                    End If
                End If
            Next
            For nCnt As Integer = nStart To nEnd Step nStep
                If nGrRank(nGrPos) > nGrRank(nCnt) Then
                    nGrPos = nCnt
                    bPosChg = True
                End If
            Next

            '0�Ԗڂ̃`�F�b�N
            If bPosChg = False Then
                If nKzRank(nGrPos) <> 0 And nKzRank(nGrPos) < Integer.MaxValue Then
                    nGrPos = 0
                    bPosChg = True
                End If
            End If

            If bPosChg Then
                Return nGrPos
            End If

            '�r�팟��
            For nCnt As Integer = nStart To nEnd Step nStep
                If nKzRank(nGrPos) > nKzRank(nCnt) Then
                    nKzPos = nCnt
                    bPosChg = True
                End If
            Next
            If bPosChg Then
                Return nKzPos
            End If
            If Not bPosChg And nKzRank(nStart) < Integer.MaxValue Then
                Return nStart
            End If

            '�����ꍇ��WS�Ɋ���Ă���
            For nCnt As Integer = nStart To nEnd Step nStep
                If nGrRank(nCnt) < Integer.MaxValue Then
                    Return nCnt
                End If
            Next

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�s���̗D��x�����r���Z�b�g���s[{0}]", ex.Message))
        End Try

        Return -1

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
        Dim bolDeep As Boolean = False

        Dim nStart As Integer
        Dim nStep As Integer
        Dim nMax As Integer

        If EM_REFRESH_MODE.AUTO = m_emViewMode Then
            '�������̃y��
            penSelect(0) = New System.Drawing.Pen(g_ColorNewKizu, 3)
            penSelect(1) = New System.Drawing.Pen(g_ColorOldKizu, 3)
        Else
            '�ꎞ��~���̃y��
            penSelect(0) = New System.Drawing.Pen(g_ColorStop1st, 3)
            penSelect(1) = New System.Drawing.Pen(g_ColorStop2nd, 3)
        End If

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

            '������̕\��
            Dim bKensaLbl As Boolean = False
            Dim nMen As Integer = 0 ' ������ �\��
            If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = m_nKensaNo Then  ' ������ ����
                nMen = 1
            End If

            If EM_REFRESH_MODE.AUTO = m_emViewMode And m_bKensaDsp Then
                m_typMapInf.lblKensaPoint.BackColor = g_ColorKensa

                Dim nKensaRow As Integer = 0

                ' 20201010 ������ʒu�\���ύX --->>>
                'If m_emStage = EM_LONG_POS.BIG Then
                '    nKensaRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageBig)
                'Else
                '    nKensaRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageSmall)
                'End If
                nKensaRow = KensaPos - 1
                ' 20201010 ������ʒu�\���ύX <<<---
                penKensa.DashStyle = Drawing2D.DashStyle.Dash
                If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                    If nRowTop <= nKensaRow And (nRowTop + nRowCount) > nKensaRow Then
                        e.Graphics.DrawRectangle(penKensa, 0, (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop + 1), dgvMap.Width, 1)
                        m_typMapInf.lblKensaPoint.Top = (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop + 1)
                        bKensaLbl = True
                    End If
                Else
                    nKensaRow = dgvMap.RowCount - nKensaRow
                    If nRowTop <= nKensaRow And (nRowTop + nRowCount) >= nKensaRow Then
                        e.Graphics.DrawRectangle(penKensa, 0, (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop + 1), dgvMap.Width, 1)
                        If nRowTop < nKensaRow Then
                            m_typMapInf.lblKensaPoint.Top = (dgvMap.Item(0, 0).Size.Height) * (nKensaRow - nRowTop)
                            bKensaLbl = True
                        End If
                    End If
                End If
            End If
            m_typMapInf.lblKensaPoint.Visible = bKensaLbl
            m_typMapInf.lblKensaPoint.Refresh()


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
                    If EM_MAP_COL_INF.IRIGAWA = nCol Then Continue For
                    If EM_MAP_COL_INF.KIZUNUM = nCol Then Continue For

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
    ''' TCM�}�b�v�ʒu����
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

            '�����X�V���͖���
            If EM_REFRESH_MODE.AUTO = m_emViewMode Then
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
            Dim listVal As List(Of TYP_MAP_LIST_VALUE) = CType(dgvCell.Tag, List(Of TYP_MAP_LIST_VALUE))
            If listVal Is Nothing Then
                Exit Try
            End If

            '�����r�̏ꍇ�͑I�����Ȃ�
            Dim bSelect As Boolean = True
            If m_typSelectValue(0) = listVal(0) Then
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
                    m_typSelectValue(0) = listVal(0)

                End If

                '�N���b�N�C�x���g���s
                RaiseEvent SelectMapCell_Click(Men, listVal(0), bSelect)

                '�r���X�g�\��
                SetDeepList(m_typMapInf.dgvDeep, listVal)
                '�I���O�ƂȂ����Ƃ����X�g��������
                LstSelectClear()

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
                dgv(EM_DEEP_LIST_COL.emNo, nCnt).Value = nCnt + 1
                dgv(EM_DEEP_LIST_COL.emGr, nCnt).Value = mcls_Param.GetGradeName(lstVal(nCnt).nGid)
                dgv(EM_DEEP_LIST_COL.emGr, nCnt).Style.BackColor = mcls_Param.GetGradeColor(lstVal(nCnt).nGid)
                dgv(EM_DEEP_LIST_COL.emType, nCnt).Value = mcls_Param.GetMapName(lstVal(nCnt).nTid)
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
            For ii As Integer = 0 To MAX_MAPDEFLST_ROW
                Dim nRowCnt As Integer = nTopRow + ii
                If nRowCnt > dgv.RowCount - 1 Then
                    Exit For
                End If
                If 0 > nRowCnt Then
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

            Dim bSelect As Boolean = True
            If m_typSelectValue(0) = lstVal(e.RowIndex) Then
                '�����r�̏ꍇ�͑I�����Ȃ�
                bSelect = False
            End If

            '�ꎞ��~���A���N���b�N��
            If EM_REFRESH_MODE.PAUSE = m_emViewMode And e.Button = MouseButtons.Left Then
                If bSelect Then
                    For ii As Integer = m_typSelectValue.Length - 1 To 1 Step -1
                        m_typSelectValue(ii) = m_typSelectValue(ii - 1)
                    Next
                    m_typSelectValue(0) = lstVal(e.RowIndex)

                    '�I���O�ƂȂ����Ƃ����X�g��������
                    LstSelectClear()
                End If

                '�C�x���g���s
                RaiseEvent SelectMapCell_Click(Men, lstVal(e.RowIndex), bSelect)
            End If

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�[�����X�g�Z���I���ُ�[{0}]", ex.Message))
        End Try

        dgvTmp.Refresh()
        m_typMapInf.dgvMap.Refresh()
    End Sub

    ''' <summary>
    ''' TCM�}�b�v�Z���I��(�}�E�X�N���b�N�C�x���g��)
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

            '�����X�V���͖���
            If EM_REFRESH_MODE.AUTO = m_emViewMode Then
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

            '�ꎞ��~���A���N���b�N��
            If EM_REFRESH_MODE.PAUSE = m_emViewMode And e.Button = MouseButtons.Left Then
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
    ''' �D�揇�ʂɂ��\�[�g�֐�
    ''' </summary>
    ''' <param name="a"></param>
    ''' <param name="b"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SortPriorty(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Integer
        ' �D�揇��
        ' �@ �O���[�h�D��x
        ' �A �r��D��x
        ' �B �ʐϏ�
        ' �C ���菇
        ' �D �rNo
        Dim dNow(5) As Double
        Dim dPer(5) As Double
        Dim bMode(5) As Boolean


        dNow(0) = CDbl(a.nGYuu)
        dPer(0) = CDbl(b.nGYuu)
        bMode(0) = False

        dNow(1) = CDbl(a.nTYuu)
        dPer(1) = CDbl(b.nTYuu)
        bMode(1) = False

        dNow(2) = CDbl(a.nAria)
        dPer(2) = CDbl(b.nAria)
        bMode(2) = True

        dNow(3) = CDbl(a.nY)
        dPer(3) = CDbl(b.nY)
        bMode(3) = False

        dNow(4) = CDbl(a.nKizuNo)
        dPer(4) = CDbl(b.nKizuNo)
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

    ' 20221021 2TCM���@���ɔ������H������ ���� --->>>
    ''' <summary>
    ''' �D�揇�ʂɂ��\�[�g�֐�(TCM�p)
    ''' </summary>
    ''' <param name="a"></param>
    ''' <param name="b"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function SortPriortyTcm(ByVal a As TYP_MAP_LIST_VALUE, ByVal b As TYP_MAP_LIST_VALUE) As Integer
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

        dNow(0) = CDbl(a.nKouteiID)
        dPer(0) = CDbl(b.nKouteiID)
        bMode(0) = False

        dNow(1) = CDbl(a.nGYuu)
        dPer(1) = CDbl(b.nGYuu)
        bMode(1) = False

        dNow(2) = CDbl(a.nTYuu)
        dPer(2) = CDbl(b.nTYuu)
        bMode(2) = False

        dNow(3) = CDbl(a.nAria)
        dPer(3) = CDbl(b.nAria)
        bMode(3) = True

        dNow(4) = CDbl(a.nY)
        dPer(4) = CDbl(b.nY)
        bMode(4) = False

        dNow(5) = CDbl(a.nKizuNo)
        dPer(5) = CDbl(b.nKizuNo)
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
    ' 20221021 2TCM���@���ɔ������H������ ���� ---<<<

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
    ''' �}�b�v����w�肳�ꂽ�s�̓����������擾���܂�
    ''' </summary>
    ''' <returns>��������[m] ���s��:-1</returns>
    ''' <remarks></remarks>
    Public Function GetInCoilLen() As Integer
        Dim nLen As Integer = -1
        Dim row As Integer

        Try
            '������̍s���擾
            Dim nKensaRow As Integer = GetKensaRow()
            If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
                row = 0 + nKensaRow
            Else
                row = m_typMapInf.dgvMap.RowCount - 1 - nKensaRow
            End If

            If row < 0 Then
                Return row
            End If

            Dim obj As Object = m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, row).Value
            If obj Is Nothing = True Then
                Return -1
            End If

            Dim strLen As String = obj.ToString()
            If Integer.TryParse(strLen, nLen) = False Then
                nLen = -1
            End If
        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�}�b�v����̏o�������擾�ُ�[{0}]", ex.Message))
            nLen = -1
        End Try

        Return nLen
    End Function

    ''' <summary>
    ''' �}�b�v�񐔎擾
    ''' </summary>
    ''' <param name="nRec">���R�[�h</param>
    ''' <returns></returns>
    Private Function GetMapColNum(ByVal nRec As Integer) As Integer
        Dim coilinf As COINFO_BASE = Nothing
        coilinf.initialize()
        If mcls_CoilInf.GetCoilInf(nRec, coilinf) Then
            Return coilinf.nMapColNum
        End If
        Return MAP_COL_NUM
    End Function


    ''' <summary>
    ''' �}�b�v�̏c�T�C�Y�ύX
    ''' </summary>
    Public Sub MapResizeH()
        Dim dgv As tCnt.tCnt_DataGridView = m_typMapInf.dgvMap
        Dim count As Integer = dgv.DisplayedRowCount(False)
        If (dgv.RowCount < count) Then Exit Sub

        ' �}�b�v�X�N���[���̈ړ��T�C�Y(��)�́A�\���s�̕�����
        Dim nLastLarge As Integer = m_typMapInf.vsbMap.LargeChange
        m_typMapInf.vsbMap.LargeChange = count

        ' �J�n�s�����ł���ʒu�ɕύX����
        If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
            ' �と���̏ꍇ�͐擪(0)
            dgv.FirstDisplayedScrollingRowIndex = 0
        Else
            ' ������̏ꍇ�́A���I�[�s���Œ肵�Č�����悤�ɊJ�n�ʒu�𒲐�
            dgv.FirstDisplayedScrollingRowIndex = dgv.RowCount - count
            If nLastLarge > m_typMapInf.vsbMap.LargeChange Then
                m_typMapInf.vsbMap.ValueProgramable = m_typMapInf.vsbMap.Value + (nLastLarge - m_typMapInf.vsbMap.LargeChange)
            End If
        End If
    End Sub

    ''' <summary>
    ''' �I���r���擾
    ''' </summary>
    ''' <param name="n"></param>
    ''' <returns></returns>
    Public Function GetSelectKizu(ByVal n As Integer, ByRef strVal As TYP_MAP_LIST_VALUE) As Boolean
        If 0 > n Or m_typSelectValue.Length <= n Then
            Return False
        End If
        strVal = m_typSelectValue(n)
        Return True
    End Function


    ''' <summary>
    ''' TCM�I���r���擾
    ''' </summary>
    ''' <returns></returns>
    Public Function GetTcmSelectKizu(ByRef strVal As TYP_MAP_LIST_VALUE) As Boolean
        strVal = m_typTcmSelectValue
        Return True
    End Function

    ''' <summary>
    ''' �}�b�v�ʒu�s���擾
    ''' </summary>
    ''' <param name="nRec">���R�[�h(1�I���W��)</param>
    ''' <param name="nMen">��[0:�\,1:��]</param>
    ''' <param name="nRow">�s�ʒu(0�I���W��)</param>
    ''' <param name="nStage">�X�e�[�W[0:�ŏ�,1:��,2:��]</param>
    ''' <param name="typMapRow_Row">�s���</param>
    ''' <returns>TRUE:����,FALSE:���s</returns>
    ''' <remarks></remarks>
    Private Function GetMapRow_Row(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nRow As Integer, ByVal nStage As Integer, ByRef typMapRow_Row As MAP_ROW_ROW) As Boolean
        Dim nReadAddr As Integer = 0                           '�ǂݍ��݃A�h���X
        Dim nRetc As ValueType = Nothing                    '���^�[���R�[�h
        Dim bRet As Boolean = False

        Try
            '������
            typMapRow_Row.initialize()

            '�������R�[�h
            If nRec <= 0 Or nRec > TBL_HY_REC_MAX Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]�������R�[�h[{1}]", TBL_HY_MAP_ROW_NAME, nRec))
                Exit Try
            End If

            '������
            If nMen < 0 Or nMen >= NUM_MEN Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]������[{1}]", TBL_HY_MAP_ROW_NAME, nMen))
                Exit Try
            End If

            '�����X�e�[�W
            If nStage < 0 Or nStage >= MAP_STAGE_NUM Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]�����X�e�[�W[{1}]", TBL_HY_MAP_ROW_NAME, nStage))
                Exit Try
            End If

            '�����s
            If nRow < 0 Or nRow >= MAP_ROW_NUM Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]�����s[{1}]", TBL_HY_MAP_ROW_NAME, nRow))
                Exit Try
            End If

            '�ǂݍ��݃A�h���X
            nReadAddr = SIZE_DSP_MAP_ROW * (nRec - 1) + SIZE_MAP_ROW_STAGE * nStage _
            + SIZE_MAP_ROW_MEN * nMen + SIZE_MAP_ROW_ROW * nRow

            '���L����������s���ǂݍ���
            typMapRow_Row = CType(ksMemReadMap(TBL_HY_MAP_ROW_NAME, nReadAddr, tMod_MemSet.SizeOf(typMapRow_Row), GetType(MAP_ROW_ROW), nRetc), MAP_ROW_ROW)
            If Not CBool(nRetc) Then
                mcls_Log.Write(LNO.ERR, String.Format("[{0}]�s���ǂݍ��ݎ��s[{1}]", TBL_HY_MAP_ROW_NAME, nReadAddr))
                Exit Try
            End If
            '�擾����
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�}�b�v�ʒu�s���擾�ُ�[{0}] nRec[{1}] nRow[{2}] nReadAddr[{3}]", ex.Message, nRec, nRow, nReadAddr))
        End Try

        Return bRet
    End Function

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

    ''' <summary>
    ''' ������ʒu�R�C����񃌃R�[�h�擾(�g���b�L���O�p)
    ''' </summary>
    ''' <returns></returns>
    Public Function GetKensaPosCoilRec() As Integer
        Dim nRec As Integer = 0
        Dim nPos As Integer = 0
        Dim nKensaRow As Integer = GetKensaRow()

        Dim count As Integer = m_typMapInf.dgvMap.RowCount
        '������̍s���擾
        If EM_MAP_SCROOL.HEAD_TO_TAIL = MapDir Then
            nPos = 0 + nKensaRow
        Else
            nPos = count - 1 - nKensaRow
        End If
        If nPos < 0 Then nPos = 0
        If nPos > (count - 1) Then nPos = (count - 1)
        nRec = CType(m_typMapInf.dgvMap(EM_MAP_COL_INF.IRIGAWA, nPos).Tag, G_MAP_ROW).nRec

        Return nRec
    End Function

    ''' <summary>
    ''' ������̍s���擾
    ''' </summary>
    ''' <returns></returns>
    Private Function GetKensaRow() As Integer
        Dim nRow As Integer = 0
        Dim nMen As Integer = 0 ' ������ �\��

        If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = m_nKensaNo Then  ' ������ ����
            nMen = 1
        End If

        If EM_DEFECT_MODE.TRACKING = MapUpdateMode Then
            ' 20201010 ������ʒu�\���ύX --->>>
            ' �������Z�o����̂���߁Aini�t�@�C���ɒ�`���ꂽ�l���g�p����B
            'If m_emStage = EM_LONG_POS.BIG Then
            '    nRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageBig)
            'Else
            '    nRow = CInt(mcls_Param.PosKensa(nMen) \ mcls_Param.MapStageSmall)
            'End If
            nRow = KensaPos - 1
            ' 20201010 ������ʒu�\���ύX <<<---
        Else
            '���A���^�C���\�����͖���
            nRow = 0
        End If
        Return nRow
    End Function

    ''' <summary>
    ''' �I���r�̈ʒu
    ''' </summary>
    ''' <returns>0:������O�A1:�������</returns>
    ''' <remarks></remarks>
    Public Function GetSelectKizuPos(ByVal nSelect As Integer) As Integer

        If 0 >= m_typSelectValue(nSelect).nPnt Then
            ' �I�𖳂��̏ꍇ
            Return EM_SELECT_POS.emKensaAto
        End If

        Return m_posSelectValue(nSelect)
    End Function

    ' 20201010 ������ʒu�\���ύX --->>>
    ''' <summary>
    ''' �g���b�L���O�p�\���J�n�ʒu�擾
    ''' </summary>
    ''' <param name="nMen">�\����(0:�\�ʁA1:����)</param>
    ''' <param name="nRec">�\���J�n���R�[�h(1�I���W���A=0�͖���)</param>
    ''' <param name="nRow">�\���J�n�s�ʒu(1�I���W��)</param>
    ''' <returns></returns>
    Private Function GetDispStartRecAndRow(ByVal nMen As Integer, ByRef nRec As Integer, ByRef nRow As Integer) As Boolean
        Dim nStage As Integer = CInt(Stage)

        '������
        nRec = 0
        nRow = 0

        '������̌��ݕ\�����R�[�h�擾
        Dim nDispRec As Integer = mcls_PointInf.RecDisp(m_nKensaNo)
        If 0 = nDispRec Then
            Return False
        End If

        Dim nCheckRec As Integer = nDispRec                            '�\�����R�[�h
        Dim nSa As Integer
        Dim nRowPos As Integer = 0              ' ��ʕ\���J�n�s
        Dim LPosBase As Integer = mcls_PointInf.RowLast(nDispRec, nStage)                 ' ���������s
        Dim WPosBase As Integer = mcls_PointInf.RowWrite(nDispRec, nMen, nStage) + 1      ' ���ݏ������ݍs
        Dim DPosBase As Integer = mcls_PointInf.RowDisp(nDispRec, m_nKensaNo, nStage)     ' ���ݕ\���s

        '�g���b�L���O�\���̏ꍇ�́A�\����_�`������ʒu �� ������\���s�̍������炷�B
        If EM_DEFECT_MODE.TRACKING = m_emMapMode Then                       ' �g���b�L���O�\��
            '�\�ʊ�̗��ʕ\���͕\�̌�����ʒu���g�p����
            Dim nMenWK As Integer = 0 ' ������ �\��
            If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = m_nKensaNo Then  ' ������ ����
                nMenWK = 1
            End If
            ' �\����_����̂��炵�ʎZ�o
            If m_emStage = EM_LONG_POS.BIG Then
                nSa = KensaPos - CInt(mcls_Param.PosKensa(nMenWK) \ mcls_Param.MapStageBig) - 1
            Else
                nSa = KensaPos - CInt(mcls_Param.PosKensa(nMenWK) \ mcls_Param.MapStageSmall) - 1
            End If

            Dim nPosBase As Integer
            If 0 <> LPosBase Then
                nPosBase = LPosBase
            Else
                nPosBase = WPosBase
            End If

            nRec = nDispRec
            nRow = DPosBase + nSa

            Dim nRowZan As Integer = DPosBase + nSa - nPosBase      '�\������s�����܂��Ă��Ȃ��s��

            If 0 < nSa Then
                '�܂��\������s�����܂��Ă��Ȃ��ꍇ
                If 0 < nRowZan Then
                    nCheckRec = nDispRec
                    For jj As Integer = 0 To TBL_HY_REC_MAX - 1
                        '����\�����R�[�h�擾
                        nCheckRec = clsPointInf.GetNextRecNo(nCheckRec)
                        ' ��ʃf�[�^�����̏ꍇ�́A�����I��
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_INITIALIZE Then
                            Exit For
                        End If
                        '�\���\�܂��͕\���\��
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                            Or mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then

                            Dim LPos As Integer = mcls_PointInf.RowLast(nCheckRec, nStage)                 ' ���������s(1�I���W���A0�͖������Ɣ��f)
                            Dim WPos As Integer = mcls_PointInf.RowWrite(nCheckRec, nMen, nStage) + 1      ' ���ݏ������ݍs(0�I���W��) ���ƍ��킹�Ďg�p�Ɉׁ{�P
                            Dim DPos As Integer = mcls_PointInf.RowDisp(nCheckRec, m_nKensaNo, nStage)     ' ���ݕ\���s(1�I���W��)

                            ' �\���\�ł͂��邪���ʔ̏ꍇ�̓X�L�b�v
                            If 0 = LPos And 1 = WPos Then
                                Continue For
                            End If
                            ' �J�nREC�A�J�n�s
                            nRec = nCheckRec
                            nRow = nRowZan

                            If 0 < LPos Then
                                nRowZan = nRowZan - LPos
                            Else
                                nRowZan = nRowZan - WPos
                            End If

                            ' �c�肪�����ꍇ�́A�����I��
                            If 0 >= nRowZan Then
                                Exit For
                            End If
                        End If
                    Next jj
                End If
            ElseIf (0 > nSa) Then

                If (DPosBase + nSa) > 0 Then
                    nRec = nDispRec
                    nRow = DPosBase + nSa
                Else
                    ' �Ώی����ʒu�̌��ݕ\���s���A��ʊJ�n�ʒu�ɂƂǂ��Ă��Ȃ��B
                    nRec = nDispRec
                    nRow = DPosBase + nSa

                    nCheckRec = nDispRec
                    For jj As Integer = 0 To TBL_HY_REC_MAX - 1
                        '�O��\�����R�[�h�擾
                        nCheckRec = clsPointInf.GetPrevRecNo(nCheckRec)
                        ' ��ʃf�[�^�����̏ꍇ�́A�����I��
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_INITIALIZE Then
                            Exit For
                        End If
                        '�\���\�܂��͕\���\��
                        If mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                            Or mcls_PointInf.DispStatus(nCheckRec) = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then

                            Dim LPos As Integer = mcls_PointInf.RowLast(nCheckRec, nStage)                 ' ���������s(1�I���W���A0�͖������Ɣ��f)
                            Dim WPos As Integer = mcls_PointInf.RowWrite(nCheckRec, nMen, nStage) + 1      ' ���ݏ������ݍs(0�I���W��) ���ƍ��킹�Ďg�p�Ɉׁ{�P
                            Dim DPos As Integer = mcls_PointInf.RowDisp(nCheckRec, m_nKensaNo, nStage)     ' ���ݕ\���s(1�I���W��)

                            ' �\���\�ł͂��邪���ʔ̏ꍇ�̓X�L�b�v
                            If 0 = LPos And 1 = WPos Then
                                Continue For
                            End If
                            ' �J�nREC�A�J�n�s
                            nRec = nCheckRec
                            nRow = LPos + (DPosBase + nSa)
                            Exit For

                        End If
                    Next jj
                End If
            Else
                nRec = nDispRec
                nRow = DPosBase + nSa
            End If
        Else
            nRec = nDispRec
            nRow = DPosBase
        End If

        Return True

    End Function
    ' 20201010 ������ʒu�\���ύX <<<---

End Class
