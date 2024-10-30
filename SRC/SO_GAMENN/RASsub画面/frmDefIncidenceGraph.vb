'*******************************************************************************
'�@�r���g�����h�O���t���
'	[Ver]
'		Ver.01    2019/03/20  ����
'
'	[����]
'		�r���g�����h�O���t��ʂ̕\��
'*******************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmDefIncidenceGraph
    '/ ////////////////////////////////////////////////////////////////////////////////
    '  �񋓑�
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R�C�����̃f�[�^�O���b�h��
    ''' </summary>
    Private Enum EM_COL_COLINF
        ORDER_NO = 0        ' ����No
        COIL_NO             ' ���Y�ԍ�
        TYPE_DIV            ' �i��
        MATERIAL            ' �ގ�
        NEXT_STEP           ' ���H��
        KENSA_REF           ' �����
        SIZE                ' �ޗ�����
        ORDER               ' ����
        MAX_COL             ' �ő��
    End Enum

    ''' <summary>
    ''' �r��f�[�^�O���b�h��
    ''' </summary>
    Private Enum EM_KIZU_KIND_DATA_COL
        COLOR = 0                      ' �\���F
        CHECK                          ' �I��
        NAME                           ' �r��
    End Enum

    ''' <summary>
    ''' �r���f�[�^�O���b�h��
    ''' </summary>
    Private Enum EM_KIZU_CNT_DATA_COL
        NAME = 0                       ' �r��
        CNT                            ' �r���i�\��/�����j
    End Enum

    ''' <summary>
    ''' �r���g�����h�O���t��ʁF�\���I��
    ''' </summary>
    Private Enum EM_KIZU_CNT_TRAND_DSP
        CNT = 0                         ' �r��
        RATE                            ' �r������
    End Enum

    ''' <summary>
    ''' �r���g�����h�O���t��ʁF�ʑI��
    ''' </summary>
    Private Enum EM_KIZU_CNT_TRAND_MEN
        FRONT = 0                       ' �\��
        BACK                            ' ����
        SUM                             ' �\���ʍ��Z
    End Enum

    '/ ////////////////////////////////////////////////////////////////////////////////
    '  �\����
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r���g�����h�F��������
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_SEARCH
        Dim nSelCoilIdx As Integer                  ' �I���R�C��Idx�i�R�C����񃊃X�g�j
        Dim nOKizuCntMax As Integer                 ' �r���ő�l�i�\�j
        Dim nUKizuCntMax As Integer                 ' �r���ő�l�i���j
        Dim nAKizuCntMax As Integer                 ' �r���ő�l�i���v�j
        Dim dOKizuRateMax As Single                 ' �r�������ő�l�i�\�j
        Dim dUKizuRateMax As Single                 ' �r�������ő�l�i���j
        Dim dAKizuRateMax As Single                 ' �r�������ő�l�i���v�j
        Dim strStartDate As String                  ' �����J�n���t
        Dim strStartTime As String                  ' �����J�n����
        Dim strEndDate As String                    ' �����I�����t
        Dim strEndTime As String                    ' �����I������
    End Structure

    ''' <summary>
    ''' �r���g�����h�F�r���
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_KIZUINF
        Dim nEdasysID As Integer                    ' �r��
        Dim nOKizuCnt As Integer                    ' �\�r����
        Dim nUKizuCnt As Integer                    ' ���r����
        Dim nAKizuCnt As Integer                    ' ���v�r����
        Dim dOKizuRate As Single                    ' �\�r��������
        Dim dUKizuRate As Single                    ' ���r��������
        Dim dAKizuRate As Single                    ' ���v�r������
    End Structure

    ''' <summary>
    ''' �r���g�����h�F�R�C�����
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_COILINF
        Dim strKizuKanNo As String                  ' �Ǘ�No
        Dim strOrderNo As String                    ' ����No
        Dim strCoilNo As String                     ' ���YNo
        Dim strHinCode As String                    ' �i��
        Dim strSyuZai As String                     ' ��ގ�
        Dim strNextCode As String                   ' �v�掟�H��
        Dim strKenKijun As String                   ' ������i�\�j
        Dim nCoilAtu As Integer                     ' �ޗ���
        Dim nCoilWidth As Integer                   ' �ޗ���
        Dim nCoilAtuSei As Integer                  ' ������
        Dim nCoilWidSei As Integer                  ' ������
        Dim nOKizuCnt As Integer                    ' �\�r����
        Dim nUKizuCnt As Integer                    ' ���r����
        Dim nAKizuCnt As Integer                    ' ���v�r����
        Dim lstKizuInf As List(Of TYP_KIZU_TRAND_KIZUINF) ' �r���

        Public Sub initialize()
            lstKizuInf = New List(Of TYP_KIZU_TRAND_KIZUINF)
        End Sub
    End Structure

    ''' <summary>
    ''' �r���g�����h���
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_INF
        Dim typSearch As TYP_KIZU_TRAND_SEARCH              ' ������ʂ̌�������
        Dim lstCoil As List(Of TYP_KIZU_TRAND_COILINF)      ' �R�C����񃊃X�g�i�O���t�\���Ώۂ̃R�C�����j
        Dim lstAllKizu As List(Of Integer)                  ' �r�탊�X�g�i�r��\���t�B���^�[�ɕ\�������r��j

        Public Sub initialize()
            lstCoil = New List(Of TYP_KIZU_TRAND_COILINF)
            lstAllKizu = New List(Of Integer)
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const INTERVAL_X_PIC_MAX As Double = 62                     ' �O���tX���̊Ԋu�ő�s�N�Z���i�O���t��Ő��Y�ԍ����d�Ȃ炸�ɕ\���ł�����E�l�j
    Private Const INTERVAL_X_INIT As Integer = 1                        ' �O���tX���̕\���Ԋu�����l
    Private Const INTERVAL_X_NEXT As Integer = 5                        ' �O���tX���̕\���Ԋu
    Private Const DEFAULT_Y_MAX As Integer = 100                        ' �O���tY���̃f�t�H���g�ő�l�i�f�t�H���g�ő�l�𒴂���ƕ\���Ԋu��ύX����j
    Private Const INTERVAL_Y_INIT As Integer = 1                        ' �O���tY���̕\���Ԋu�����l
    Private Const INTERVAL_Y_NEXT As Integer = 10                       ' �O���tY���̕\���Ԋu
    Private Const GRAPH_PADDING_LEFT As Integer = 40                    ' �O���t���p�f�B���O
    Private Const GRAPH_PADDING_TOP As Integer = 10                     ' �O���t��p�f�B���O
    Private Const GRAPH_PADDING_RIGHT As Integer = 33                   ' �O���t�E�p�f�B���O
    Private Const GRAPH_PADDING_BOT As Integer = 40                     ' �O���t���p�f�B���O

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f���Q�[�g
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Delegate Function GetKizuCntRate(ByVal nEdasysID As Integer, ByVal lstKizu As List(Of TYP_KIZU_TRAND_KIZUINF), ByVal nMen As EM_KIZU_CNT_TRAND_MEN) As Single
    Private m_KizuDspMethod As GetKizuCntRate                           ' �r�� or �r���������擾����֐�

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Param As clsParamManager                               ' �p�����[�^�Ǘ�
    Private mcls_Log As tClass.tClass_LogManager                        ' ���O�Ǘ�
    Private mcls_pMap As New tClass.tClass_MapBase                      ' �}�b�v�`����N���X 
    Private mfrm_DefTrand As New frmDefTrand                            ' �r���g�����h�O���t�������
    Private m_typProtData As TYP_KIZU_TRAND_INF                         ' �r���g�����h�O���t�v���b�g�f�[�^�i������ʂ���擾�j
    Private m_strDspFormat As String                                    ' �r��/�������\���t�H�[�}�b�g


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �����o�֐�
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "�R���X�g���N�^"
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="log">���O�Ǘ�</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal log As tClass.tClass_LogManager)
        Try
            ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
            InitializeComponent()

            ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

            Dim s As Size
            s.Width = WIN_WIDTH_MAX
            s.Height = WIN_HEIGHT_MAX
            Me.MaximumSize = s
            s.Width = WIN_WIDTH_MIN
            s.Height = WIN_HEIGHT_MIN
            Me.MinimumSize = s

            ' ������ʂ����t�H�[���̎q���ɒǉ�
            'Me.AddOwnedForm(mfrm_DefTrand)

            mfrm_DefTrand = New frmDefTrand                 ' �r���g�����h�O���t�������
            m_typProtData = mfrm_DefTrand.KizuTrandInf      ' �r���g�����h�O���t�v���b�g�f�[�^
            mcls_Log = log                                  ' ���O�Ǘ�  
            mcls_Param = New clsParamManager(mcls_Log)

        Catch ex As Exception
            log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �쐬�ُ� [{0}]", ex.Message))
        End Try
    End Sub

#End Region

#Region "�t�H�[���\��/�I���C�x���g�֘A"
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location

        ' �^�C�g���ݒ�
        SetTitleName(Me)

        ' �|�b�v�A�b�v��� ��𑜓x�Ή�
        SetPcHeight(Me)
        SetPcWidth(Me)

        If NOA_ONLINE <> g_NOA Then                                         ' �f�o�b�O��
            ' �t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
            Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' �t�H�[���T�C�Y�ύX�\
        End If

        While (Not mcls_Param.ReadAllParam())                               ' �p�����[�^�ǂݍ��߂�܂ŁA�S��
            System.Threading.Thread.Sleep(1000)
        End While

        ' ��ʍ��ڏ�����
        SetInit()

        ' �r��\���t�B���^�[�ɑS�r��\��
        m_typProtData.initialize()
        SetAllKizuKind()

    End Sub

    ''' <summary>
    ''' �t�H�[���\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Try
            '''' ��ʍ��ڒl�ݒ�
            SetDsp()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�t�H�[���\�� ��O[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �t�H�[���I������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing

        'ini�t�@�C����񏑍�
        PutKizuCntTrendDsp()

    End Sub

    ''' <summary>
    ''' ���̉�ʂ̎�������C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
            '-------------------------------------------------------------------
            ' �������
            '-------------------------------------------------------------------
            m_KizuDspMethod = Nothing

            If Not mfrm_DefTrand Is Nothing Then
                mfrm_DefTrand.Close()
                mfrm_DefTrand.Dispose()
                mfrm_DefTrand = Nothing
            End If

            If Not mcls_pMap Is Nothing Then
                mcls_pMap.Dispose()
                mcls_pMap = Nothing
            End If

            If Not mcls_Param Is Nothing Then
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�r���g�����h�O���t��� ��������ُ� [{0}]", ex.Message))
        End Try
    End Sub

#End Region

#Region "����������"
    ''' <summary>
    ''' ��ʏ����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()

        ' �R�C����񏉊���
        CoilInit()

        ' �r��\���t�B���^�[������
        KizuKindInit()

        ' �r��/������������
        KizuCntRateInit()

        ' �}�b�v������
        MapInit()

        ' ���t������
        lblDate.Text = ""

        ' �C�x���g�ݒ�
        SetItemEvent(True)
    End Sub

    ''' <summary>
    ''' �}�b�v��{��񏉊���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapInit()
        '''' �O���t�`��ɕK�v�ȏ���ݒ�
        With mcls_pMap
            '' �\���̈���
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph.Size
            .tProp_AriaInf_Margin = New Padding(GRAPH_PADDING_LEFT, GRAPH_PADDING_TOP, GRAPH_PADDING_RIGHT, GRAPH_PADDING_BOT)

            '' �}�b�v�̈�
            .tProp_MapInf_IsCenterX = False
            .tProp_MapInf_IsCenterY = False
            .tProp_MapInf_IsBaseX_L = True
            .tProp_MapInf_IsBaseY_T = False
            .tProp_MapInf_IsStrechX = True
            .tProp_MapInf_IsStrechY = True
            .tProp_MapInf_LimitXmin = 1
            .tProp_MapInf_LimitXmax = 1000
            .tProp_MapInf_LimitYmin = 0
            .tProp_MapInf_LimitYmax = 100
            .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

            '' �}�b�v�ΏۊO�̈�ݒ�
            .tProp_LimitInf_LimitDraw = False

            '' �������ݒ�
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            .tProp_MemoriInf_LinePen = p
            .tProp_MemoriInf_StringFont = New Font("�l�r �S�V�b�N", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
            .tProp_MemoriInf_PosStringX = 10
            .tProp_MemoriInf_PosStringY = 1
            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(INTERVAL_X_INIT, INTERVAL_Y_INIT)
            .tProp_MemoriInf_ScaleStringFormatX = "0�{�O"
            .tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1)
            .tProp_MemoriInf_StringDisp_T = False
            .tProp_MemoriInf_StringDisp_B = True
            .tProp_MemoriInf_StringDisp_L = True
            .tProp_MemoriInf_StringDisp_R = False
            .tProp_MemoriInf_LineDsp_X = True
            .tProp_MemoriInf_LineDsp_Y = True
        End With
    End Sub

    ''' <summary>
    ''' �R�C����񏉊���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CoilInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvCoil

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                                ' �I��F����

        With dgv
            .ScrollBars = ScrollBars.Horizontal                             ' �X�N���[���o�[(�����̂�)
            .DoubleBuffered = True

            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
            .ColumnHeadersVisible = True                                    ' ��w�b�_�\������

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True
            .ColumnHeadersHeight = 20                                       ' ��w�b�_��������

            For col As EM_COL_COLINF = EM_COL_COLINF.ORDER_NO To EM_COL_COLINF.MAX_COL
                Select Case col
                    Case EM_COL_COLINF.ORDER_NO
                        ' ����No
                        dgvColSetText(dgv, "����No", 70, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.COIL_NO
                        ' ���Y�ԍ�
                        dgvColSetText(dgv, "���Y�ԍ�", 167, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.TYPE_DIV
                        ' �i��
                        dgvColSetText(dgv, "�i��", 38, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.MATERIAL
                        ' �ގ�
                        dgvColSetText(dgv, "�ގ�", 38, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.NEXT_STEP
                        ' ���H��
                        dgvColSetText(dgv, "���H��", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.KENSA_REF
                        ' �����
                        dgvColSetText(dgv, "�����", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.SIZE
                        ' ����
                        dgvColSetText(dgv, "����", 186, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.ORDER
                        ' ����
                        dgvColSetText(dgv, "����", 186, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

                End Select
            Next

            .RowCount = 1
            .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight

        End With

        ' XX�{�ڂ��\��
        pnlCoilCnt.Visible = False

    End Sub

    ''' <summary>
    ''' �r��\���t�B���^�[������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub KizuKindInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvKizukind

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                                ' �I��F����

        With dgv
            .ColumnCount = 0
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Vertical                               ' �X�N���[���o�[�̕\���i�����̂݁j
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .ReadOnly = False                                               ' �ǂݎ���p�ł͂Ȃ�
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = False                                   ' ��w�b�_��\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            ' 1���
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "�\���F"
                clm.Width = 45
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns.Add(clm)
            End Using

            ' 2���
            Using clm As New DataGridViewCheckBoxColumn
                clm.HeaderText = "�L������" & vbNewLine & "(�����F�L��)"

                clm.Width = 25
                clm.Frozen = False
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter

                .Columns.Add(clm)
            End Using

            ' 3���
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "����"
                clm.Width = .Width - 70 - 3
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

        End With

    End Sub

    ''' <summary>
    ''' �r��/�r������������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub KizuCntRateInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvKizuCntRate

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)   ' �I��F����

        With dgv
            '' ��`
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �s�I��
            .ScrollBars = ScrollBars.Horizontal                             ' �X�N���[���o�[(�����̂�)
            .ReadOnly = True
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 14.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .DefaultCellStyle.WrapMode = DataGridViewTriState.False

            '' ���`
            .ColumnCount = dgvKizukind.Rows.Count
            .ColumnHeadersVisible = False
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 14.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' ��w�b�_�T�C�Y

            ' �񂪐ݒ肳��Ă���ꍇ
            If 0 < .ColumnCount Then
                '�s��`
                .RowCount = 2
                For ii As Integer = 0 To .Rows.Count - 1
                    .Rows(ii).Height = CInt(dgv.ClientSize.Height / dgv.Rows.Count)     '�s�̕����Z�b�g
                Next ii

                '' ���`
                For ii As Integer = 0 To .Columns.Count - 1
                    .Columns(ii).Width = 100
                    .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                Next ii
            End If

        End With

    End Sub

    ''' <summary>
    ''' ��ʍ��ڃC�x���g�ݒ�
    ''' </summary>
    ''' <param name="bSet">�C�x���g�ݒ�L�� true�F�C�x���g�ݒ� false�F�C�x���g����</param>
    ''' <remarks></remarks>
    Private Sub SetItemEvent(ByVal bSet As Boolean)
        If bSet Then
            ' �}�b�v
            AddHandler mcls_pMap.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

            ' �f�[�^�O���b�h�r���[�i�R�C�����j
            AddHandler dgvCoil.Resize, AddressOf Ctrl_ReSize

            ' �f�[�^�O���b�h�r���[�i�r��\���t�B���^�[�j
            AddHandler dgvKizukind.CellContentClick, AddressOf dgvGraph_CellContentClick
            AddHandler dgvKizukind.SelectionChanged, AddressOf dgvGraph_SelectionChanged

            ' �f�[�^�O���b�h�r���[�i�r���j
            AddHandler dgvKizuCntRate.Resize, AddressOf Ctrl_ReSize
            AddHandler dgvKizuCntRate.ColumnWidthChanged, AddressOf dgvColumnWidthChanged

            ' �s�N�`���{�b�N�X
            AddHandler picGraph.Paint, AddressOf PictureBox_Paint
            AddHandler picGraph.Resize, AddressOf PictureBox_Resize

            ' �`�F�b�N�{�b�N�X�i�v���b�g�ݒ�j
            AddHandler chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

            ' ���W�I�{�^���i�ʑI���j
            AddHandler radFront.CheckedChanged, AddressOf radbtn_CheckedChanged
            AddHandler radBack.CheckedChanged, AddressOf radbtn_CheckedChanged
            AddHandler radSum.CheckedChanged, AddressOf radbtn_CheckedChanged

            ' ���W�I�{�^���i�\���I���j
            AddHandler radKizuCnt.CheckedChanged, AddressOf radbtn_CheckedChanged
            AddHandler radKizuRate.CheckedChanged, AddressOf radbtn_CheckedChanged
        Else
            ' �}�b�v
            RemoveHandler mcls_pMap.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

            ' �f�[�^�O���b�h�r���[�i�R�C�����j
            RemoveHandler dgvCoil.Resize, AddressOf Ctrl_ReSize

            ' �f�[�^�O���b�h�r���[�i�r��\���t�B���^�[�j
            RemoveHandler dgvKizukind.CellContentClick, AddressOf dgvGraph_CellContentClick
            RemoveHandler dgvKizukind.SelectionChanged, AddressOf dgvGraph_SelectionChanged

            ' �f�[�^�O���b�h�r���[�i�r���j
            RemoveHandler dgvKizuCntRate.Resize, AddressOf Ctrl_ReSize

            ' �s�N�`���{�b�N�X
            RemoveHandler picGraph.Paint, AddressOf PictureBox_Paint
            RemoveHandler picGraph.Resize, AddressOf PictureBox_Resize

            ' �`�F�b�N�{�b�N�X�i�v���b�g�ݒ�j
            RemoveHandler chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

            ' ���W�I�{�^���i�ʑI���j
            RemoveHandler radFront.CheckedChanged, AddressOf radbtn_CheckedChanged
            RemoveHandler radBack.CheckedChanged, AddressOf radbtn_CheckedChanged
            RemoveHandler radSum.CheckedChanged, AddressOf radbtn_CheckedChanged

            ' ���W�I�{�^���i�\���I���j
            RemoveHandler radKizuCnt.CheckedChanged, AddressOf radbtn_CheckedChanged
            RemoveHandler radKizuRate.CheckedChanged, AddressOf radbtn_CheckedChanged
        End If

    End Sub

    ''' <summary>
    ''' �r��\���t�B���^�[�ݒ�
    ''' �S�r����r��\���t�B���^�[�ɐݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetAllKizuKind()

        If m_typProtData.lstAllKizu Is Nothing Then Return

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)    'DB�N���X
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""               'Sql���쐬�p

        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT EdasysID "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            While sqlRead.Read()
                m_typProtData.lstAllKizu.Add(sqlRead.GetInt32(0))
            End While

        Catch ex As Exception

        Finally
            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()

            If Not tcls_DB Is Nothing Then
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

    End Sub
#End Region

#Region "�v���b�g�f�[�^�ݒ�"
    ''' <summary>
    ''' �v���b�g�f�[�^����ʍ��ڂɐݒ�
    ''' </summary>
    Private Function SetProtData() As Boolean
        Dim bVal As Boolean = False

        Try
            '''' ��ʍ��ڂɒl��ݒ肷��׈ꎞ�I�ɃC�x���g����
            SetItemEvent(False)

            '''' ��ʍ��ڕ\��
            SetItem()

            '''' �\���I��ݒ�
            SetDspKind()

            '''' �X�N���[���o�[�\�����̃Z���E�t�H���g�T�C�Y�ݒ�
            dgvCoilWidthResize()
            dgvKizuCntWidthResize()

            bVal = True
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("��ʃf�[�^�ݒ� ��O[{0}]", ex.Message))
        Finally
            '''' �C�x���g�Đݒ�
            SetItemEvent(True)
        End Try

        Return bVal
    End Function

    ''' <summary>
    ''' ��ʍ��ڕ\��
    ''' �\���I���A�ʑI���A�v���b�g�\���ɑO��l�𔽉f��
    ''' �擾�����v���b�g�f�[�^����R�C�����A�r����A�r��/���������A���t���\����\������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetItem()

        ' �\���I��
        Dim nDsp As Integer = tMod.ReadIniInt(SO_GAMENN, "KIZUCNTTRANDDISP", LOCALINI_NAME, EM_KIZU_CNT_TRAND_DSP.CNT)
        If EM_KIZU_CNT_TRAND_DSP.RATE = nDsp Then
            radKizuRate.Select()
        Else
            radKizuCnt.Select()
        End If

        ' �ʑI��
        Dim nMen As Integer = tMod.ReadIniInt(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, EM_KIZU_CNT_TRAND_MEN.FRONT)
        If EM_KIZU_CNT_TRAND_MEN.SUM = nMen Then
            radSum.Select()
        ElseIf EM_KIZU_CNT_TRAND_MEN.BACK = nMen Then
            radBack.Select()
        Else
            radFront.Select()
        End If

        ' �v���b�g�\��
        Dim nProt As Integer = tMod.ReadIniInt(SO_GAMENN, "KIZUCNTTRANDPROT", LOCALINI_NAME, 0)
        If 1 = nProt Then
            chkGraphProt.Checked = True
        Else
            chkGraphProt.Checked = False
        End If

        ' ���t���\��
        Dim strStartDate As String = String.Format("{0} {1}",
                              m_typProtData.typSearch.strStartDate, m_typProtData.typSearch.strStartTime).Trim
        Dim strEndDate As String = String.Format("{0} {1}",
                                      m_typProtData.typSearch.strEndDate, m_typProtData.typSearch.strEndTime).Trim
        If String.Empty = strStartDate Or String.Empty = strEndDate Then
            lblDate.Text = ""
        Else
            lblDate.Text = String.Format("���ԁF{0} �` {1}", strStartDate, strEndDate)
        End If

        ' �S�I���A�S�����{�^���ݒ�
        If Not m_typProtData.lstCoil Is Nothing AndAlso 0 < m_typProtData.lstCoil.Count _
           AndAlso Not m_typProtData.lstAllKizu Is Nothing AndAlso 0 < m_typProtData.lstAllKizu.Count Then
            ' �\���R�C���E�r��񂠂�
            btnAllSelect.Enabled = True
            btnAllCancel.Enabled = True
        Else
            ' ��L�ȊO
            btnAllSelect.Enabled = False
            btnAllCancel.Enabled = False
        End If

        ' �R�C����񗓕\��
        DspCoil()

        ' �r����\��
        DspKizuKind()

        ' �r��/���������\��
        DspKizuCntRate()

    End Sub

    ''' <summary>
    ''' �\���I���f�[�^�̐ݒ�i�r�� or �r�������j
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDspKind()

        If radKizuRate.Checked Then
            m_KizuDspMethod = New GetKizuCntRate(AddressOf GetKizuRate)
            If radSum.Checked Then
                m_strDspFormat = "{0:#0.0}/{1:#0.0}"
            Else
                m_strDspFormat = "{0:#0.0}"
            End If
        Else
            m_KizuDspMethod = New GetKizuCntRate(AddressOf GetKizuCnt)
            If radSum.Checked Then
                m_strDspFormat = "{0}/{1}"
            Else
                m_strDspFormat = "{0}"
            End If
        End If
    End Sub

    ''' <summary>
    ''' �R�C�����\��
    ''' </summary>
    Private Sub DspCoil()
        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then
                ' XX�{�ڔ�\��
                pnlCoilCnt.Visible = False
                Return
            End If

            With dgvCoil

                Dim nSelect As Integer = m_typProtData.typSearch.nSelCoilIdx
                Dim typCoilInf As TYP_KIZU_TRAND_COILINF = m_typProtData.lstCoil(nSelect)

                ' XX�{�ڐݒ�
                pnlCoilCnt.Visible = True
                lblCoilCnt.Text = (nSelect + 1).ToString

                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF.ORDER_NO           ' ����No
                            .Item(ii, 0).Value = typCoilInf.strOrderNo
                        Case EM_COL_COLINF.COIL_NO            ' ���Y�ԍ�
                            .Item(ii, 0).Value = typCoilInf.strCoilNo
                        Case EM_COL_COLINF.TYPE_DIV           ' �i��
                            .Item(ii, 0).Value = typCoilInf.strHinCode
                        Case EM_COL_COLINF.MATERIAL           ' �ގ�
                            .Item(ii, 0).Value = typCoilInf.strSyuZai
                        Case EM_COL_COLINF.NEXT_STEP          ' ���H��
                            .Item(ii, 0).Value = typCoilInf.strNextCode
                        Case EM_COL_COLINF.KENSA_REF          ' �����
                            .Item(ii, 0).Value = typCoilInf.strKenKijun ' �����_�\
                        Case EM_COL_COLINF.SIZE
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThick As String = Format(typCoilInf.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = typCoilInf.nCoilWidth.ToString
                            .Item(ii, 0).Value = String.Format("{0}�~{1,4}", sThick, sWidth)
                        Case EM_COL_COLINF.ORDER
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThickSei As String = Format(typCoilInf.nCoilAtuSei / 1000, "0.000")
                            Dim sWidthSei As String = typCoilInf.nCoilWidSei.ToString
                            .Item(ii, 0).Value = String.Format("{0}�~{1,4}", sThickSei, sWidthSei)
                        Case Else
                            .Item(ii, 0).Value = ""
                            .Item(ii, 0).Style.BackColor = .DefaultCellStyle().BackColor
                    End Select
                Next
            End With
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�R�C�����\���ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r����\��
    ''' </summary>
    Private Sub DspKizuKind()
        Dim ii As Integer = 0

        Try
            If m_typProtData.lstAllKizu Is Nothing _
               OrElse m_typProtData.lstCoil Is Nothing Then Return

            dgvKizukind.RowCount = 0

            For Each nEdasysID As Integer In m_typProtData.lstAllKizu

                '''' �r��f�[�^�O���b�h
                dgvKizukind.RowCount = dgvKizukind.RowCount + 1

                ' �r��t�B���^�[
                dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value = True

                ' �\���F
                Dim strColor As String = tMod.ReadIniStr(SO_GAMENN, "KIZUCNTTRANDCOLOR_" & nEdasysID, LOCALINI_NAME, "")
                If "" <> strColor Then
                    dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32("FF" & strColor, 16))
                Else
                    dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor = mcls_Param.GetTypeSymbolColor(nEdasysID)
                End If

                ' ����
                dgvKizukind(EM_KIZU_KIND_DATA_COL.NAME, ii).Value = mcls_Param.GetTypeName(nEdasysID)

                ' EdasysID�ێ��i�`�F�b�N��ԕێ��p�j
                dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag = nEdasysID

                ii += 1
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�r����\���ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r��/���������\��
    ''' </summary>
    Private Sub DspKizuCntRate()
        Dim ii As Integer = 0

        Try
            If m_typProtData.lstAllKizu Is Nothing _
               OrElse m_typProtData.lstCoil Is Nothing _
               OrElse 1 > m_typProtData.lstCoil.Count Then Return

            ' �r��/��������񏉊���
            KizuCntRateInit()

            For Each nEdasysID As Integer In m_typProtData.lstAllKizu

                '''' �r���f�[�^�O���b�h
                Dim nSelect As Integer = m_typProtData.typSearch.nSelCoilIdx
                Dim typCoilInf As TYP_KIZU_TRAND_COILINF = m_typProtData.lstCoil(nSelect)

                ' �r��/�������擾
                Dim strValue As String = ""
                If radSum.Checked Then
                    strValue = String.Format(m_strDspFormat,
                                                 m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.FRONT),
                                                 m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.BACK))
                ElseIf radBack.Checked Then
                    strValue = String.Format(m_strDspFormat, m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.BACK))
                Else
                    strValue = String.Format(m_strDspFormat, m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.FRONT))
                End If

                dgvKizuCntRate(ii, EM_KIZU_CNT_DATA_COL.NAME).Value = dgvKizukind(EM_KIZU_KIND_DATA_COL.NAME, ii).Value.ToString
                dgvKizuCntRate(ii, EM_KIZU_CNT_DATA_COL.NAME).Style.ForeColor = dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor
                dgvKizuCntRate(ii, EM_KIZU_CNT_DATA_COL.CNT).Value = strValue

                ii += 1
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�r�����\���ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r���擾
    ''' �\���I�����r����I�����Ă���ꍇ�Am_KizuDspMethod�ɓ��֐���ݒ�
    ''' </summary>
    ''' <param name="nEdasysID">�r��</param>
    ''' <param name="lstKizu">�r���</param>
    ''' <param name="nMen">�Ώۖ�</param>
    ''' <remarks></remarks>
    Private Function GetKizuCnt(ByVal nEdasysID As Integer, ByVal lstKizu As List(Of TYP_KIZU_TRAND_KIZUINF), ByVal nMen As EM_KIZU_CNT_TRAND_MEN) As Integer

        For Each typKizu As TYP_KIZU_TRAND_KIZUINF In lstKizu
            If nEdasysID = typKizu.nEdasysID Then
                If EM_KIZU_CNT_TRAND_MEN.FRONT = nMen Then      ' �\��
                    Return typKizu.nOKizuCnt
                ElseIf EM_KIZU_CNT_TRAND_MEN.BACK = nMen Then   ' ����
                    Return typKizu.nUKizuCnt
                Else                                            ' �\�����Z
                    Return typKizu.nAKizuCnt
                End If
            End If
        Next

        Return 0
    End Function

    ''' <summary>
    ''' �r�������擾
    ''' �\���I�����r��������I�����Ă���ꍇ�Am_KizuDspMethod�ɓ��֐���ݒ�
    ''' </summary>
    ''' <param name="nEdasysID">�r��</param>
    ''' <param name="lstKizu">�r���</param>
    ''' <param name="nMen">�Ώۖ�</param>
    ''' <remarks></remarks>
    Private Function GetKizuRate(ByVal nEdasysID As Integer, ByVal lstKizu As List(Of TYP_KIZU_TRAND_KIZUINF), ByVal nMen As EM_KIZU_CNT_TRAND_MEN) As Single
        Dim result As Integer = 0

        For Each typKizu As TYP_KIZU_TRAND_KIZUINF In lstKizu
            If nEdasysID = typKizu.nEdasysID Then
                If EM_KIZU_CNT_TRAND_MEN.FRONT = nMen Then      ' �\��
                    Return typKizu.dOKizuRate
                ElseIf EM_KIZU_CNT_TRAND_MEN.BACK = nMen Then   ' ����
                    Return typKizu.dUKizuRate
                Else                                            ' �\�����Z
                    Return typKizu.dAKizuRate
                End If
            End If
        Next

        Return result
    End Function
#End Region

#Region "��ʕ`��"
    ''' <summary>
    ''' ��ʃf�[�^�`�� (�ݒ�ύX���Ɏ��s) (��ʕ\�����ɂ�)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDsp()
        Dim bVal As Boolean = False

        '''' �v���b�g�f�[�^�ݒ�
        If SetProtData() Then
            '''' �}�b�v�ĕ`��
            ReDrawMap()
        End If

    End Sub

    ''' <summary>
    ''' �}�b�v�ĕ`��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReDrawMap()

        '''' �}�b�v�̈�Ē�`
        MapReSetring()

        '''' �}�b�v�Đ����˗�
        mcls_pMap.DrawPaint()

        '''' �}�b�v�ĕ`��
        picGraph.Refresh()

    End Sub

    ''' <summary>
    ''' �}�b�v�Ē�`
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapReSetring()

        '''' �O���t�`��ɕK�v�ȏ���ݒ�
        With mcls_pMap

            Dim nXMax As Integer = 0
            Dim nYMax As Integer = DEFAULT_Y_MAX

            '''' X���ő�l�̐ݒ�
            If Not m_typProtData.lstCoil Is Nothing AndAlso 1 < m_typProtData.lstCoil.Count Then
                ' �R�C�����2���ȏ゠�����ꍇ��X���ő�l��ݒ�
                ' �R�C�����1���̏ꍇ�A�R�C�����͎n�_�݂̂ɂȂ�̂Ńf�t�H���g��0�̂܂܂ł悢
                nXMax = m_typProtData.lstCoil.Count
            End If

            '''' Y���ő�l�̐ݒ�
            If Not m_typProtData.lstCoil Is Nothing AndAlso 0 < m_typProtData.lstCoil.Count _
               AndAlso Not m_typProtData.lstAllKizu Is Nothing AndAlso 0 < m_typProtData.lstAllKizu.Count Then
                ' �R�C����񂪑��݂����r�����擾���Ă����ꍇ��Y���ő�l��ݒ�
                If radKizuRate.Checked Then
                    ' �r�������͎l�̌ܓ�����Y���ő�l��ݒ�
                    If radSum.Checked Then
                        nYMax = CInt(m_typProtData.typSearch.dAKizuRateMax + 0.5)
                    ElseIf radBack.Checked Then
                        nYMax = CInt(m_typProtData.typSearch.dUKizuRateMax + 0.5)
                    Else
                        nYMax = CInt(m_typProtData.typSearch.dOKizuRateMax + 0.5)
                    End If
                Else
                    If radSum.Checked Then
                        nYMax = m_typProtData.typSearch.nAKizuCntMax
                    ElseIf radBack.Checked Then
                        nYMax = m_typProtData.typSearch.nUKizuCntMax
                    Else
                        nYMax = m_typProtData.typSearch.nOKizuCntMax
                    End If
                End If

                ' Y���ő�l��0��ݒ肷��ƃO���t�̕`�悪���������Ȃ�itClass_MapBase�̎d�l�H�j�̂�
                ' Y���ő�l��0�Ȃ�f�t�H���g�l��ݒ�
                nYMax = IIf(0 = nYMax, DEFAULT_Y_MAX, nYMax)
            End If


            '''' X���̊Ԋu���擾�i�ϐݒ�j
            Dim nXLen As Integer = picGraph.Width - (GRAPH_PADDING_LEFT + GRAPH_PADDING_RIGHT) ' �O���tX���̒������擾
            Dim nXSplitCnt As Integer = CInt(Math.Floor(nXLen / INTERVAL_X_PIC_MAX))           ' X���̍ő啪�������擾
            Dim nXMaxSplitVal As Integer = CInt(Math.Ceiling(nXMax / nXSplitCnt))              ' X���ő�l�𕪊�����l���擾

            ' X���̊Ԋu�����߂�
            Dim nXInterval As Integer
            If 1 >= nXMaxSplitVal / INTERVAL_X_INIT Then         ' �����l���Ԋu�����l�ȉ�
                ' �Ԋu�������l�ɐݒ�
                nXInterval = INTERVAL_X_INIT
            Else
                ' �����l��INTERVAL_X_NEXT�̔{���ɏC�����ĊԊu��ݒ�
                nXInterval = INTERVAL_X_NEXT * CInt(Math.Ceiling(nXMaxSplitVal / INTERVAL_X_NEXT))
            End If


            '''' Y���̊Ԋu���擾�i�ϐݒ�j
            Dim nYSplitCnt As Integer = CInt(Math.Floor(DEFAULT_Y_MAX / INTERVAL_Y_NEXT))      ' Y���̍ő啪�������擾
            Dim nYMaxSplitVal As Integer = CInt(Math.Ceiling(nYMax / nYSplitCnt))              ' Y���ő�l�𕪊�����l���擾

            ' Y���̊Ԋu�����߂�
            Dim nYInterval As Integer
            If 1 >= nYMaxSplitVal / INTERVAL_Y_INIT Then         ' �����l���Ԋu�����l�ȉ�
                ' �Ԋu�������l�ɐݒ�
                nYInterval = INTERVAL_Y_INIT
            Else
                ' �����l��INTERVAL_Y_NEXT�̔{���ɏC�����ĊԊu��ݒ�
                nYInterval = INTERVAL_Y_NEXT * CInt(Math.Ceiling(nYMaxSplitVal / INTERVAL_Y_NEXT))
            End If

            ' X,Y���̊Ԋu��ݒ�
            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(nXInterval, nYInterval)


            '''' �}�b�v�̈�ݒ�
            ' Y���̊Ԋu�������l�̏ꍇ�AY���̍ő�l��INTERVAL_Y_NEXT�ɐݒ�
            nYInterval = IIf(INTERVAL_Y_INIT = nYInterval, INTERVAL_Y_NEXT, nYInterval)

            ' Y���̍ő�l��INTERVAL_Y_NEXT�Ŋ���؂��l�ɕ␳�iY���̈�ԏ�ɏ�ɐ��l��\������j
            If (0 <> nYMax Mod nYInterval) Then nYMax = nYMax + nYInterval - (nYMax Mod nYInterval)

            .tProp_MapInf_LimitXmax = nXMax
            .tProp_MapInf_LimitYmax = nYMax

        End With
    End Sub
#End Region

#Region "���̑�"
    ''' <summary>
    ''' �r���g�����h�O���t�ݒ�ۑ�(Ini�t�@�C��������)
    ''' </summary>
    Private Sub PutKizuCntTrendDsp()

        Try
            'ini�t�@�C����񏑍�
            For ii As Integer = 0 To dgvKizukind.RowCount - 1
                ' �t�B���^�[�ݒ�
                If True = CBool(dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value) Then
                    tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDFILTER_" & dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag.ToString, LOCALINI_NAME, "1")
                Else
                    tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDFILTER_" & dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag.ToString, LOCALINI_NAME, "0")
                End If

                ' �I��F
                Dim strColor As String = Microsoft.VisualBasic.Right(Hex(dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor.ToArgb), 6)
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDCOLOR_" & dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag.ToString, LOCALINI_NAME, strColor)
            Next

            ' �\���I��
            If radKizuRate.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDDISP", LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDDISP", LOCALINI_NAME, "0")
            End If

            ' �ʑI��
            If radSum.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, "2")
            ElseIf radBack.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, "0")
            End If

            ' �v���b�g�\��
            If chkGraphProt.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDPROT", LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDPROT", LOCALINI_NAME, "0")
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("INI�t�@�C���������ُ� [{0}]", ex.Message))
        End Try

    End Sub

#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '  �C�x���g�n���h���֐�
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "���T�C�Y�֘A�C�x���g"
    ''' <summary>
    ''' ���T�C�Y
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        If sender.Equals(dgvKizuCntRate) Then
            dgvKizuCntWidthResize()
        ElseIf sender.Equals(dgvCoil) Then
            dgvCoilWidthResize()
        End If
    End Sub

    ''' <summary>
    ''' ���T�C�Y�ɂ��\���ύX�i�R�C�����j
    ''' </summary>
    Private Sub dgvCoilWidthResize()
        Dim hsc As HScrollBar

        ' �s����ݒ�
        hsc = CType(dgvCoil.Controls(0), HScrollBar)
        If hsc.Visible Then
            ' �����X�N���[���o�[�\����
            dgvCoil.ColumnHeadersHeight = 16
            dgvCoil.Rows(0).Height = dgvCoil.ClientSize.Height - dgvCoil.ColumnHeadersHeight - hsc.Height
        Else
            ' �����X�N���[���o�[���\��
            dgvCoil.ColumnHeadersHeight = 20
            dgvCoil.Rows(0).Height = dgvCoil.ClientSize.Height - dgvCoil.ColumnHeadersHeight
        End If

        ' �񐔕��̕����T�C�Y��ݒ�
        For ii As Integer = 0 To dgvCoil.ColumnCount - 1
            If hsc.Visible Then
                ' �����X�N���[���o�[�\����
                dgvCoil.Item(ii, 0).Style.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                ' �����X�N���[���o�[���\��
                dgvCoil.Item(ii, 0).Style.Font = New System.Drawing.Font("�l�r ����", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End If
        Next

    End Sub

    ''' <summary>
    ''' ���T�C�Y�ɂ��\���ύX�i�r��/�r���������j
    ''' </summary>
    Private Sub dgvKizuCntWidthResize()
        Dim hsc As HScrollBar
        Dim nHeight As Integer

        If 1 > dgvKizuCntRate.Rows.Count Then Exit Sub

        ' �s�����擾
        hsc = CType(dgvKizuCntRate.Controls(0), HScrollBar)
        If hsc.Visible Then
            ' �����X�N���[���o�[�\����
            nHeight = CInt((dgvKizuCntRate.ClientSize.Height - hsc.Height) / dgvKizuCntRate.Rows.Count)
        Else
            ' �����X�N���[���o�[���\��
            nHeight = CInt(dgvKizuCntRate.ClientSize.Height / dgvKizuCntRate.Rows.Count)
        End If

        ' �s�������[�v
        For ii As Integer = 0 To dgvKizuCntRate.Rows.Count - 1
            dgvKizuCntRate.Rows(ii).Height = nHeight   '�s����ݒ�

            ' �񐔕��̕����T�C�Y��ݒ�
            For jj As Integer = 0 To dgvKizuCntRate.ColumnCount - 1
                If hsc.Visible Then
                    '�����X�N���[���o�[�\����
                    dgvKizuCntRate.Item(jj, ii).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Else
                    ' �����X�N���[���o�[���\��
                    Dim emSize As Single = 14.0
                    ' �\�Ɨ����r��/�r�������l���Z���̈���ɑS�ĕ\���ł��Ȃ��ꍇ�i9�����ȏ�j
                    If ii = 1 AndAlso Not dgvKizuCntRate(jj, ii).Value Is Nothing AndAlso dgvKizuCntRate(jj, ii).Value.ToString.Count > 8 Then
                        ' �Z���̈���ɕ�����S�ĕ\���ł���悤�ɕ����T�C�Y��ύX
                        emSize = 10.0
                    End If

                    dgvKizuCntRate(jj, ii).Style.Font = New System.Drawing.Font("�l�r ����", emSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End If
            Next
        Next
    End Sub

    ''' <summary>
    ''' �񕝂̃T�C�Y�ύX�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvColumnWidthChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewColumnEventArgs)

        Dim dgv As tCnt.tCnt_DataGridView = CType(sender, tCnt.tCnt_DataGridView)

        ' �r��/�r���������\���̈�Đݒ�
        dgvKizuCntWidthResize()

    End Sub
#End Region

#Region "�r���񗓊֘A�C�x���g"
    ''' <summary>
    ''' �S�I���{�^������
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(sender As Object, e As EventArgs) Handles btnAllSelect.Click

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            '�`�F�b�N�{�b�N�X��S��ON
            CheckGrid(dgvKizukind, True)

            ' �}�b�v�ĕ`��
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�S�I���{�^�������ُ� [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �S�����{�^������
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(sender As Object, e As EventArgs) Handles btnAllCancel.Click

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            '�`�F�b�N�{�b�N�X��S��OFF
            CheckGrid(dgvKizukind, False)

            ' �}�b�v�ĕ`��
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�S�����{�^�������ُ� [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �O���b�h�̃`�F�b�N�{�b�N�X�Z�b�g
    ''' </summary>
    ''' <param name="dgvData">�f�[�^�O���b�h</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Private Sub CheckGrid(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' �O���b�h�̑S�Ẵ`�F�b�N�{�b�N�X��ݒ�
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value = bValue
        Next ii

    End Sub


    ''' <summary>
    ''' �r��t�B���^�[�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)

        Try
            If m_typProtData.lstCoil Is Nothing Then Return

            Dim dgv As DataGridView = CType(sender, DataGridView)

            ' �s���N���b�N����Ă��Ȃ���ΏI��
            If 0 > e.RowIndex Then Exit Sub

            ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
            Select Case e.ColumnIndex
                Case EM_KIZU_KIND_DATA_COL.COLOR  '�\���F
                    GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)

                    ' �r��/��������񗓂ɕύX�F�𔽉f
                    dgvKizuCntRate(e.RowIndex, EM_KIZU_CNT_DATA_COL.NAME).Style.ForeColor = dgv(e.ColumnIndex, e.RowIndex).Style.BackColor

                Case EM_KIZU_KIND_DATA_COL.CHECK
                    dgv(e.ColumnIndex, e.RowIndex).Value = dgv(e.ColumnIndex, e.RowIndex).EditedFormattedValue

                Case EM_KIZU_KIND_DATA_COL.NAME
            End Select

            ' �}�b�v�ĕ`��
            ReDrawMap()
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�r��t�B���^�[�Z�������ُ� [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �\���F�{�^���������ꂽ��A�F�I���_�C�A���O��\�����A�{�^���̐F��ύX
    ''' </summary>
    ''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
    ''' <param name="nColIndex">�{�^���̗�C���f�b�N�X</param>
    ''' <param name="nRowIndex">�{�^���̍s�C���f�b�N�X</param>
    ''' <remarks></remarks>
    Private Sub GetDlgColor(ByVal dgv As DataGridView, ByVal nColIndex As Integer, ByVal nRowIndex As Integer)
        '�F�I���_�C�A���O�̐ݒ�
        '�F�̍쐬���g�p���Ȃ�
        dlgColor.AllowFullOpen = False

        '�F�I���_�C�A���O�̑I��F���A�{�^���̔w�i�F�ɂ���
        dlgColor.Color = dgv(nColIndex, nRowIndex).Style.BackColor

        ' �F�I���_�C�A���O��ok�{�^���������ꂽ��A�{�^���̐F��ύX���� 
        If Windows.Forms.DialogResult.OK = dlgColor.ShowDialog() Then
            '�I��F���{�^���̔w�i�F��
            dgv(nColIndex, nRowIndex).Style.BackColor = dlgColor.Color
        End If

        dlgColor.Dispose()
    End Sub


    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv
        dgv.CurrentCell.Selected = False
    End Sub

#End Region

#Region "��ʍ��ڃ`�F�b�N�C�x���g"
    ''' <summary>
    ''' �v���b�g�\���`�F�b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkGraphProt_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            ' �}�b�v�ĕ`��
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�v���b�g�\���`�F�b�N�ُ� [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' ���W�I�{�^���`�F�b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radbtn_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            ' �\���I��ݒ�
            SetDspKind()

            ' �r��/���������\��
            DspKizuCntRate()

            ' �r��/�r���������\���̈�Đݒ�
            dgvKizuCntWidthResize()

            ' �}�b�v�ĕ`��
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("���W�I�{�^���`�F�b�N�ُ� [{0}]", ex.Message))
        End Try

    End Sub

#End Region

#Region "�O���t�֘A�C�x���g"
    ''' <summary>
    ''' �O���t����
    ''' �O���t�������ꂽ�ꍇ�A�O���t��Y���ɑΉ�����R�C��������ʏ�ɕ\��
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub picGraph_Click(sender As Object, e As EventArgs) Handles picGraph.Click

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            Dim pos As Point = picGraph.PointToClient(Windows.Forms.Cursor.Position)         ' �N���b�N�ʒu (��ʍ��W����Pic�R���g���[���̍��ォ��̍��W�ɕϊ�)

            '''' X���͈͐ݒ�
            Dim nXHosei As Integer = 0
            ' �R�C�����Q�ȏ㑶��
            If 1 < m_typProtData.lstCoil.Count Then
                ' �R�C���Ԃ̕�/2��X���͈̔͂ɐݒ�
                nXHosei = CInt((mcls_pMap.GetVtoP_X(2) - mcls_pMap.GetVtoP_X(1)) / 2)
            End If

            '''' �N���b�N�ʒu�Ɉ�ԋ߂��R�C�������擾
            ' �R�C���������[�v
            For ii As Integer = 1 To m_typProtData.lstCoil.Count

                ' X���\���ő匏���𒴂����ꍇ
                If ii > mcls_pMap.tProp_MapInf_LimitXmax Then Exit For

                ' X���|�C���g�Z�o
                Dim x1 As Integer = mcls_pMap.GetVtoP_X(ii)

                ' �ΏۊO
                If x1 - nXHosei > pos.X OrElse x1 + nXHosei < pos.X Then Continue For

                ' �I���R�C���X�V
                m_typProtData.typSearch.nSelCoilIdx = ii - 1

                ' �R�C����񗓕\��
                DspCoil()

                ' �r��/���������\��
                DspKizuCntRate()

                ' �r��/�r���������\���̈�Đݒ�
                dgvKizuCntWidthResize()

                Exit For
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�O���t�����ُ� [{0}]", ex.Message))
        End Try

    End Sub

#End Region

#Region "�`��֘A�C�x���g"
    ''' <summary>
    ''' �`��
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' </summary>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        '''' �r�b�g�}�b�v�擾
        Dim bmp As Bitmap = mcls_pMap.tProp_Bmp
        If bmp Is Nothing Then Return

        '''' �w�i���Z�b�g
        e.Graphics.DrawImage(bmp, 0, 0)

    End Sub

    ''' <summary>
    ''' �s�N�`���{�b�N�X�̃T�C�Y�ύX
    ''' </summary>
    Private Sub PictureBox_Resize(ByVal sender As Object, ByVal e As System.EventArgs)
        '''' �s�N�`���{�b�N�X�T�C�Y���Đݒ�
        mcls_pMap.tProp_AriaInf_AriaSize = picGraph.Size

        '''' �ĕ`��
        ReDrawMap()
    End Sub

    ''' <summary>
    ''' ��ʃf�[�^�`��
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)

        ' �R�C����񖢎擾�̏ꍇ�͕`��Ȃ�
        If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

        Dim lstCoil As List(Of TYP_KIZU_TRAND_COILINF) = m_typProtData.lstCoil
        Dim nMen As EM_KIZU_CNT_TRAND_MEN

        If radFront.Checked Then
            nMen = EM_KIZU_CNT_TRAND_MEN.FRONT
        ElseIf radBack.Checked Then
            nMen = EM_KIZU_CNT_TRAND_MEN.BACK
        Else
            nMen = EM_KIZU_CNT_TRAND_MEN.SUM
        End If

        '''' �܂���O���t�`��
        Dim dVal As Single
        Dim x1, y1, x2, y2 As Integer
        Try
            For ii As Integer = 0 To dgvKizukind.RowCount - 1

                ' �r��\���L��
                If False = CType(dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value, Boolean) Then Continue For

                Dim nEdasysID As Integer = CInt(dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag)
                Dim colLine As Color = dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor

                ' �n�_�Z�o (�擪�̍��W�Z�o)
                x1 = cls.GetVtoP_X(1)
                dVal = m_KizuDspMethod(nEdasysID, lstCoil(0).lstKizuInf, nMen)

                If dVal > cls.tProp_MapInf_LimitYmax Then dVal = cls.tProp_MapInf_LimitYmax
                y1 = cls.GetVtoP_Y(dVal)

                Using p As New Pen(colLine, 1)
                    ' �n�_�̂݃v���b�g�̎w��
                    If chkGraphProt.Checked Then
                        g.FillEllipse(New SolidBrush(p.Color), x1 - 4, y1 - 4, 8, 8)
                    End If

                    ' �R�C�����1���̂�
                    If 1 = lstCoil.Count Then
                        ' �`��
                        g.DrawLine(p, x1, y1, x1, y1)
                        Continue For
                    End If

                    ' �R�C��2�{�ڈȍ~�̕`��
                    For jj As Integer = 1 To lstCoil.Count - 1

                        ' �\���ő匏���𒴂����ꍇ
                        If jj > cls.tProp_MapInf_LimitXmax - 1 Then Exit For

                        ' �I�_�Z�o
                        x2 = cls.GetVtoP_X(jj + 1)
                        dVal = m_KizuDspMethod(nEdasysID, lstCoil(jj).lstKizuInf, nMen)

                        If dVal > cls.tProp_MapInf_LimitYmax Then dVal = cls.tProp_MapInf_LimitYmax
                        y2 = cls.GetVtoP_Y(dVal)

                        ' �`��
                        g.DrawLine(p, x1, y1, x2, y2)

                        ' �v���b�g�̎w��
                        If chkGraphProt.Checked Then
                            g.FillEllipse(New SolidBrush(p.Color), x2 - 4, y2 - 4, 8, 8)
                        End If

                        ' ���̏���
                        x1 = x2
                        y1 = y2
                    Next
                End Using
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�܂���O���t�`�掞�ُ� [{0}]", ex.Message))
        End Try


        '''' ���������̃R�C��No��\��
        Dim b As SolidBrush = Nothing
        Dim f As Font = Nothing
        x1 = 0
        y1 = 0
        Try
            b = New SolidBrush(Color.Black)
            f = New Font("�l�r �S�V�b�N", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)

            ' ���_�\��
            y1 = picGraph.Height
            x1 = cls.GetVtoP_X(1)
            cls.DrawString(g, x1, y1, 0, -1, lstCoil(0).strCoilNo, ContentAlignment.TopCenter, f, b)

            ' ���_�ȍ~�\��
            Dim nInterval As Integer = CInt(cls.tProp_MemoriInf_ScaleInterval.X)
            For ii As Integer = nInterval To lstCoil.Count Step nInterval
                ' �\������f�[�^�́A��������100�̏ꍇ�́A�z���99�B���������z���0��\�����Ă����
                x1 = cls.GetVtoP_X(ii)
                cls.DrawString(g, x1, y1, 0, -1, lstCoil(ii - 1).strCoilNo, ContentAlignment.TopCenter, f, b)
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�R�C��No�`�掞�ُ� [{0}]", ex.Message))
        Finally
            b.Dispose()
            f.Dispose()
        End Try

    End Sub
#End Region

#Region "�L�[�C�x���g"
    ''' <summary>
    ''' �t�@���N�V�����L�[�C�x���g
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No 0�I���W��</param>
    ''' <param name="enable">�{�^���R���g���[���̗L������</param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo
                Case 4                                  ' ����
                    ' �r���g�����h�O���t������ʂ̕\��
                    ShowDefTrand()

                Case 9                                  ' ��ʈ��
                    PrintOut()

                Case 10                                 ' ��ʕۑ�
                    HardCopy(IMG_PATH, "�r���g�����h�O���t���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

                Case 11                                 ' RAS��ʕ\��
                    MyBase.ExecFromClose()
            End Select
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�t�@���N�V���������ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r���g�����h������ʕ\��
    ''' </summary>
    Private Sub ShowDefTrand()

        ' �r���g�����h������ʕ\��
        mfrm_DefTrand.ShowDialog()

        ' ������ʐݒ�ۑ�
        mfrm_DefTrand.SaveCheck()

        ' �O���t�o�̓{�^������
        If frmDefTrand.KIZU_TRAND_RETURN.nShowKizuTrand = mfrm_DefTrand.ReturnCode Then

            ' �r���g�����h��ʐݒ�ۑ�
            PutKizuCntTrendDsp()

            '''' �v���b�g�f�[�^���Z�b�g
            m_typProtData = mfrm_DefTrand.KizuTrandInf

            ' �f�[�^�Đݒ�
            SetDsp()
        End If

    End Sub

#End Region


End Class
