'*******************************************************************************
'�@�R�C���g�����h���
'	[Ver]
'		Ver.01    2012/07/12  ����
'
'	[����]
'		�R�C���g�����h����ʂ̕\��
'*******************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmCoilTrand
    '---------------------------------------------------------------------------
    ' �񋓑�
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �R�C�����̃f�[�^�O���b�h��
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_COLINF
        ORDER_NO = 0        ' ����No
        COIL_LEN            ' �R�C����
        KANRI_NO            ' �Ǘ�No
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
    ''' �g�����h�f�[�^�̃f�[�^�O���b�h��
    ''' </summary>
    Private Enum EM_COL_CALC
        MAX_NAME = 0
        MAX_DATA
        MIN_NAME
        MIN_DATA
        AVG_NAME
        AVG_DATA
        MAX_COL             ' �ő��
    End Enum

    ''' <summary>
    ''' �\���R�C���g�����h
    ''' </summary>
    Private Enum EM_COIL_TREND
        SPEED = 0           ' ���C�����x
        WIDTH               ' ��
        CAM_KIDO_O          ' �J�����P�x(�\)
        CAM_KIDO_U          ' �J�����P�x(��)
        CAM_GAIN_O          ' �J�����Q�C��(�\)
        CAM_GAIN_U          ' �J�����Q�C��(��)
        CAM_TIME_O          ' �J�����I������(�\)
        CAM_TIME_U          ' �J�����I������(��)
        WALK                ' �E�H�[�N��
        LED_O               ' �Ɩ����u�����l(�\)
        LED_U               ' �Ɩ����u�����l(��)
        MAX
    End Enum

    ' �}��̃f�[�^�O���b�h��
    Private Enum em_DataColumnType
        iColor = 0                      ' Color
        iCheck                          ' Check
        iName                           ' Name
    End Enum

    '/////////////////////////////////////////////////////////////////////////////////
    '�\����
    '/////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R�C���g�����h���
    ''' </summary>
    Private Structure TYP_TREND_INF
        Dim strGroupName As String                                      ' �O���[�v��
        Dim nMin As Integer                                             ' �����l
        Dim nMax As Integer                                             ' ����l
        Dim nDecPlaces As Integer                                       ' �����_����
        Dim lstItem As ArrayList
        Public Sub initialize()
            lstItem = New ArrayList
        End Sub
    End Structure

    ''' <summary>
    ''' �J�������
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure CAM_INFO
        Dim nKidoR As Integer               ' �P�xR_C
        Dim nKidoG As Integer               ' �P�xG_C
        Dim nKidoB As Integer               ' �P�xB_C
        Dim dGainR As Single                ' �Q�C��R
        Dim dGainG As Single                ' �Q�C��G
        Dim dGainB As Single                ' �Q�C��B
        Dim nTimeR As Integer               ' �I������R
        Dim nTimeG As Integer               ' �I������G
        Dim nTimeB As Integer               ' �I������B
    End Structure

    ''' <summary>
    ''' �R�C���g�����h�\���f�[�^
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TREND_DATA
        Dim TorB As Integer             ' �\���敪 (0:�\ 1:��)
        Dim nPos As Integer             ' �擾�ʒu
        Dim nSpeed As Integer           ' ���C�����x
        Dim dItahaba() As Single          ' ��
        Dim typCamInf() As CAM_INFO     ' �J�������
        Dim dWalk As Single             ' ������
        Dim nDimming() As Integer         ' �����l
        Public Sub initialize()
            ReDim typCamInf(mc_CameraN - 1)
            ReDim dItahaba(2 - 1)
            ReDim nDimming(5 - 1)
        End Sub
    End Structure

    ''' <summary>
    ''' �R�C���g�����h�\���v�Z�l
    ''' </summary>
    Private Structure TYP_TREND_CALC
        Dim min As String               '�ŏ��l
        Dim max As String               '�ő�l
        Dim avg As String               '���ϒl
        Public Sub initialize()
            min = String.Empty
            max = String.Empty
            avg = String.Empty
        End Sub
    End Structure

    ''' <summary>
    ''' �g�����h�O���t�\���e�[�u��
    ''' </summary>
    Private Structure TYP_TREND_DSP
        Dim clsGraph As tClass.tClass_MapBase                           ' �O���t�G���A
        Dim dgvHanrei As DataGridView                                   ' �}��z��
        Dim nudGraphMin As NumericUpDown                                ' �����l�z��
        Dim nudGraphMax As NumericUpDown                                ' ����l�z��
        Dim picGraph As PictureBox                                      ' �O���t�\���G���A
        Dim chkGraphProt As CheckBox                                    ' �O���t�v���b�g�\���E��\��          
        Dim cmbName As ComboBox                                         ' �g�����h��
        Dim dgvCalc As tCnt.tCnt_DataGridView                           ' �ő�/�ŏ�/���ϕ\��
        Dim emType As EM_COIL_TREND
        Dim nCnt As Integer                                             '����
        Dim nCoilPos() As Integer                                       '�擾�ʒu
        Dim nData(,) As Single                                          '�f�[�^
        Dim nMin As Integer                                             '�����l
        Dim nMax As Integer                                             '����l
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const mc_NUM As Integer = 5                                 ' �O���t�O���[�v��
    Private Const mc_CameraN As Integer = 10                            ' �J�����䐔
    Private Const mc_XScale As Integer = 10                             ' X���X�P�[���Ԋu(����)
    Private Const mc_YScale As Integer = 10                             ' Y���X�P�[���Ԋu(����)

    Private Const NON_TRAND_DATA As Integer = -999999

    ' �����J���[�l
    Private ReadOnly sdcDefault() As System.Drawing.Color = {
        Color.Red, Color.Orange, Color.Yellow, Color.Green, Color.Lime,
        Color.GreenYellow, Color.Blue, Color.RoyalBlue, Color.Cyan, Color.DarkCyan,
        Color.Violet, Color.DeepPink, Color.DarkRed, Color.PaleVioletRed, Color.DarkGreen,
        Color.DarkSeaGreen, Color.PaleGreen, Color.DarkBlue, Color.Purple, Color.Magenta,
        Color.IndianRed, Color.Indigo, Color.Khaki, Color.LawnGreen, Color.MediumOrchid,
        Color.Tomato, Color.Brown, Color.Aquamarine, Color.DeepSkyBlue, Color.BurlyWood
    }

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                        ' ���O�Ǘ�
    Private m_typCoilInf As clsDataMap.TYP_COIL_INF                     ' �R�C�����
    Private m_bNagate As Boolean                                        ' ���щ�ʒ���������](True:���]����Afalse:���]�Ȃ��j

    '' �ێ����[�N�f�[�^
    Private m_typTrendDsp(mc_NUM - 1) As TYP_TREND_DSP
    Private m_lstData As List(Of TYP_TREND_DATA)                        ' �g�����h�f�[�^
    Private m_dctCalc As Dictionary(Of EM_COIL_TREND, TYP_TREND_CALC)   ' �v�Z�l�f�[�^�i�ŏ��A�ő�A����)
    Private m_dctTrendInf As Dictionary(Of EM_COIL_TREND, TYP_TREND_INF)
    Private m_bSetEnd As Boolean                                        ' �������Z�b�g�����t���O(True:�Z�b�g�����AFalse:������)

    '---------------------------------------------------------------------------
    ' �����o�֐�
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �R�C���g�����h��ʂ��쐬���܂�
    ''' </summary>
    ''' <param name="typCoilInf">�R�C�����</param>
    ''' <param name="bNagate">���щ�ʒ���������](True:���]����Afalse:���]�Ȃ�)</param>
    ''' <param name="log">���O�Ǘ�</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typCoilInf As clsDataMap.TYP_COIL_INF, ByVal bNagate As Boolean, ByRef log As tClass.tClass_LogManager)
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

            mcls_Log = log                  ' ���O�Ǘ�  

            m_typCoilInf = typCoilInf       ' �R�C�����
            m_bNagate = bNagate             ' ���щ�ʒ���������]

            m_dctTrendInf = New Dictionary(Of EM_COIL_TREND, TYP_TREND_INF)
            m_dctCalc = New Dictionary(Of EM_COIL_TREND, TYP_TREND_CALC)
            m_bSetEnd = False

        Catch ex As Exception
            log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v��� �쐬�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �t�H�[���I������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing

        Dim ii As Integer = 0
        'ini�t�@�C����񏑍�

        For ii = 0 To m_typTrendDsp.Count - 1
            PutTrendDsp(m_typTrendDsp(ii))
        Next ii

        For ii = 0 To m_typTrendDsp.Count - 1
            '�O���t���񏑍�
            tMod.WriteIni(SO_GAMENN, "COILTRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, m_typTrendDsp(ii).cmbName.SelectedValue.ToString)
        Next ii

    End Sub

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location

        If NOA_ONLINE <> g_NOA Then                                         ' �f�o�b�O��
            ' �t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
            Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' �t�H�[���T�C�Y�ύX�\
        End If

        '�g�����h��񏉊��ݒ�
        TrandInfInit()

        '�^�C�g���ݒ�
        SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)
        Call SetPcWidth(Me)

        '''' ��ʏ����ݒ�
        SetInit()

        ''''' �R�C�����\��
        dgvCoilInit()
        DspCoil()

        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' �R���{�{�b�N�X�I���C�x���g��ݒ�
            AddHandler m_typTrendDsp(ii).cmbName.SelectedIndexChanged, AddressOf cmbName_SelectedIndexChanged
        Next ii

        ' DB�ǂݍ���
        m_lstData = New List(Of TYP_TREND_DATA)
        GetCoilTrandData()

        m_dctCalc.Clear()
        GetCalc()

        AddHandler dgvCoil.Resize, AddressOf Ctrl_ReSize

        '�|�b�v�A�b�v��� ��𑜓x�Ή�(�X�N���[���o�[������ƃR�C����񂪌��؂��j
        dgvWidthResize(dgvCoil)
    End Sub


    ''' <summary>
    ''' ���̉�ʂ̎�������C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�R�C���g�����h��� ��������ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �����`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As DataGridView)

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                                         ' �Z���G���[�̕\��
            .ShowCellToolTips = False                                       ' �c�[���q���g�̕\��
            .ShowEditingIcon = False                                        ' �ҏW���̃Z���̖���
            .ShowRowErrors = False                                          ' �G���[�s�̖���
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                                ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = False                                   ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

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

        End With

    End Sub

    ''' <summary>
    ''' ����dgv������
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvHanreiInit(ByVal dgv As DataGridView)


        ' �����`
        DgvInit(dgv)

        With dgv
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
                clm.Width = dgv.Width - 70 - 3
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

        End With

    End Sub

    Private Sub TrandInfInit()

        Dim typ As TYP_TREND_INF = Nothing
        m_dctTrendInf.Clear()

        For em As EM_COIL_TREND = EM_COIL_TREND.SPEED To EM_COIL_TREND.MAX
            With typ
                .initialize()
                Select Case em
                    Case EM_COIL_TREND.SPEED
                        .strGroupName = "���C�����x[mpm]"
                        .nMin = 0
                        .nMax = 1000
                        .nDecPlaces = 0
                        .lstItem.Add("�\")
                        .lstItem.Add("��")
                    Case EM_COIL_TREND.WIDTH
                        .strGroupName = "��[mm]"
                        .nMin = 0
                        .nMax = COIL_WID_MAX
                        .nDecPlaces = 0
                        .lstItem.Add("�\ ��Min")
                        .lstItem.Add("�\ ��Max")
                        .lstItem.Add("�� ��Min")
                        .lstItem.Add("�� ��Max")
                    Case EM_COIL_TREND.CAM_KIDO_O
                        .strGroupName = "�J�����P�x[�P�x] (�\)"
                        .nMin = 0
                        .nMax = 255
                        .nDecPlaces = 0
                        .lstItem.Add("�J����1_R")
                        .lstItem.Add("�J����1_G")
                        .lstItem.Add("�J����1_B")
                        .lstItem.Add("�J����2_R")
                        .lstItem.Add("�J����2_G")
                        .lstItem.Add("�J����2_B")
                        .lstItem.Add("�J����3_R")
                        .lstItem.Add("�J����3_G")
                        .lstItem.Add("�J����3_B")
                        .lstItem.Add("�J����4_R")
                        .lstItem.Add("�J����4_G")
                        .lstItem.Add("�J����4_B")
                        .lstItem.Add("�J����5_R")
                        .lstItem.Add("�J����5_G")
                        .lstItem.Add("�J����5_B")
                        .lstItem.Add("�J����6_R")
                        .lstItem.Add("�J����6_G")
                        .lstItem.Add("�J����6_B")
                        .lstItem.Add("�J����7_R")
                        .lstItem.Add("�J����7_G")
                        .lstItem.Add("�J����7_B")
                        .lstItem.Add("�J����8_R")
                        .lstItem.Add("�J����8_G")
                        .lstItem.Add("�J����8_B")
                        .lstItem.Add("�J����9_R")
                        .lstItem.Add("�J����9_G")
                        .lstItem.Add("�J����9_B")
                        .lstItem.Add("�J����10_R")
                        .lstItem.Add("�J����10_G")
                        .lstItem.Add("�J����10_B")
                    Case EM_COIL_TREND.CAM_KIDO_U
                        .strGroupName = "�J�����P�x[�P�x] (��)"
                        .nMin = 0
                        .nMax = 255
                        .nDecPlaces = 0
                        .lstItem.Add("�J����1_R")
                        .lstItem.Add("�J����1_G")
                        .lstItem.Add("�J����1_B")
                        .lstItem.Add("�J����2_R")
                        .lstItem.Add("�J����2_G")
                        .lstItem.Add("�J����2_B")
                        .lstItem.Add("�J����3_R")
                        .lstItem.Add("�J����3_G")
                        .lstItem.Add("�J����3_B")
                        .lstItem.Add("�J����4_R")
                        .lstItem.Add("�J����4_G")
                        .lstItem.Add("�J����4_B")
                        .lstItem.Add("�J����5_R")
                        .lstItem.Add("�J����5_G")
                        .lstItem.Add("�J����5_B")
                        .lstItem.Add("�J����6_R")
                        .lstItem.Add("�J����6_G")
                        .lstItem.Add("�J����6_B")
                        .lstItem.Add("�J����7_R")
                        .lstItem.Add("�J����7_G")
                        .lstItem.Add("�J����7_B")
                        .lstItem.Add("�J����8_R")
                        .lstItem.Add("�J����8_G")
                        .lstItem.Add("�J����8_B")
                        .lstItem.Add("�J����9_R")
                        .lstItem.Add("�J����9_G")
                        .lstItem.Add("�J����9_B")
                        .lstItem.Add("�J����10_R")
                        .lstItem.Add("�J����10_G")
                        .lstItem.Add("�J����10_B")
                    Case EM_COIL_TREND.CAM_GAIN_O
                        .strGroupName = "�J�����Q�C��[�{] (�\)"
                        .nMin = 0
                        .nMax = 10
                        .nDecPlaces = 2
                        .lstItem.Add("�J����1_R")
                        .lstItem.Add("�J����1_G")
                        .lstItem.Add("�J����1_B")
                        .lstItem.Add("�J����2_R")
                        .lstItem.Add("�J����2_G")
                        .lstItem.Add("�J����2_B")
                        .lstItem.Add("�J����3_R")
                        .lstItem.Add("�J����3_G")
                        .lstItem.Add("�J����3_B")
                        .lstItem.Add("�J����4_R")
                        .lstItem.Add("�J����4_G")
                        .lstItem.Add("�J����4_B")
                        .lstItem.Add("�J����5_R")
                        .lstItem.Add("�J����5_G")
                        .lstItem.Add("�J����5_B")
                        .lstItem.Add("�J����6_R")
                        .lstItem.Add("�J����6_G")
                        .lstItem.Add("�J����6_B")
                        .lstItem.Add("�J����7_R")
                        .lstItem.Add("�J����7_G")
                        .lstItem.Add("�J����7_B")
                        .lstItem.Add("�J����8_R")
                        .lstItem.Add("�J����8_G")
                        .lstItem.Add("�J����8_B")
                        .lstItem.Add("�J����9_R")
                        .lstItem.Add("�J����9_G")
                        .lstItem.Add("�J����9_B")
                        .lstItem.Add("�J����10_R")
                        .lstItem.Add("�J����10_G")
                        .lstItem.Add("�J����10_B")
                    Case EM_COIL_TREND.CAM_GAIN_U
                        .strGroupName = "�J�����Q�C��[�{] (��)"
                        .nMin = 0
                        .nMax = 10
                        .nDecPlaces = 2
                        .lstItem.Add("�J����1_R")
                        .lstItem.Add("�J����1_G")
                        .lstItem.Add("�J����1_B")
                        .lstItem.Add("�J����2_R")
                        .lstItem.Add("�J����2_G")
                        .lstItem.Add("�J����2_B")
                        .lstItem.Add("�J����3_R")
                        .lstItem.Add("�J����3_G")
                        .lstItem.Add("�J����3_B")
                        .lstItem.Add("�J����4_R")
                        .lstItem.Add("�J����4_G")
                        .lstItem.Add("�J����4_B")
                        .lstItem.Add("�J����5_R")
                        .lstItem.Add("�J����5_G")
                        .lstItem.Add("�J����5_B")
                        .lstItem.Add("�J����6_R")
                        .lstItem.Add("�J����6_G")
                        .lstItem.Add("�J����6_B")
                        .lstItem.Add("�J����7_R")
                        .lstItem.Add("�J����7_G")
                        .lstItem.Add("�J����7_B")
                        .lstItem.Add("�J����8_R")
                        .lstItem.Add("�J����8_G")
                        .lstItem.Add("�J����8_B")
                        .lstItem.Add("�J����9_R")
                        .lstItem.Add("�J����9_G")
                        .lstItem.Add("�J����9_B")
                        .lstItem.Add("�J����10_R")
                        .lstItem.Add("�J����10_G")
                        .lstItem.Add("�J����10_B")
                    Case EM_COIL_TREND.CAM_TIME_O
                        .strGroupName = "�I������[25ns] (�\)"
                        .nMin = 320
                        .nMax = 120000
                        .nDecPlaces = 0
                        .lstItem.Add("�J����1_R")
                        .lstItem.Add("�J����1_G")
                        .lstItem.Add("�J����1_B")
                        .lstItem.Add("�J����2_R")
                        .lstItem.Add("�J����2_G")
                        .lstItem.Add("�J����2_B")
                        .lstItem.Add("�J����3_R")
                        .lstItem.Add("�J����3_G")
                        .lstItem.Add("�J����3_B")
                        .lstItem.Add("�J����4_R")
                        .lstItem.Add("�J����4_G")
                        .lstItem.Add("�J����4_B")
                        .lstItem.Add("�J����5_R")
                        .lstItem.Add("�J����5_G")
                        .lstItem.Add("�J����5_B")
                        .lstItem.Add("�J����6_R")
                        .lstItem.Add("�J����6_G")
                        .lstItem.Add("�J����6_B")
                        .lstItem.Add("�J����7_R")
                        .lstItem.Add("�J����7_G")
                        .lstItem.Add("�J����7_B")
                        .lstItem.Add("�J����8_R")
                        .lstItem.Add("�J����8_G")
                        .lstItem.Add("�J����8_B")
                        .lstItem.Add("�J����9_R")
                        .lstItem.Add("�J����9_G")
                        .lstItem.Add("�J����9_B")
                        .lstItem.Add("�J����10_R")
                        .lstItem.Add("�J����10_G")
                        .lstItem.Add("�J����10_B")
                    Case EM_COIL_TREND.CAM_TIME_U
                        .strGroupName = "�I������[25ns] (��)"
                        .nMin = 320
                        .nMax = 120000
                        .nDecPlaces = 0
                        .lstItem.Add("�J����1_R")
                        .lstItem.Add("�J����1_G")
                        .lstItem.Add("�J����1_B")
                        .lstItem.Add("�J����2_R")
                        .lstItem.Add("�J����2_G")
                        .lstItem.Add("�J����2_B")
                        .lstItem.Add("�J����3_R")
                        .lstItem.Add("�J����3_G")
                        .lstItem.Add("�J����3_B")
                        .lstItem.Add("�J����4_R")
                        .lstItem.Add("�J����4_G")
                        .lstItem.Add("�J����4_B")
                        .lstItem.Add("�J����5_R")
                        .lstItem.Add("�J����5_G")
                        .lstItem.Add("�J����5_B")
                        .lstItem.Add("�J����6_R")
                        .lstItem.Add("�J����6_G")
                        .lstItem.Add("�J����6_B")
                        .lstItem.Add("�J����7_R")
                        .lstItem.Add("�J����7_G")
                        .lstItem.Add("�J����7_B")
                        .lstItem.Add("�J����8_R")
                        .lstItem.Add("�J����8_G")
                        .lstItem.Add("�J����8_B")
                        .lstItem.Add("�J����9_R")
                        .lstItem.Add("�J����9_G")
                        .lstItem.Add("�J����9_B")
                        .lstItem.Add("�J����10_R")
                        .lstItem.Add("�J����10_G")
                        .lstItem.Add("�J����10_B")
                    Case EM_COIL_TREND.WALK
                        .strGroupName = "�E�H�[�N��[m]"
                        .nMin = -100
                        .nMax = 100
                        .nDecPlaces = 0
                        .lstItem.Add("�\")
                        .lstItem.Add("��")
                    Case EM_COIL_TREND.LED_O
                        .strGroupName = "�Ɩ����u_�����l[%] (�\)"
                        .nMin = 0
                        .nMax = 100
                        .nDecPlaces = 0
                        .lstItem.Add("�Ɩ����u_�����l_1")
                        .lstItem.Add("�Ɩ����u_�����l_2")
                        .lstItem.Add("�Ɩ����u_�����l_3")
                        .lstItem.Add("�Ɩ����u_�����l_4")
                        .lstItem.Add("�Ɩ����u_�����l_5")
                    Case EM_COIL_TREND.LED_U
                        .strGroupName = "�Ɩ����u_�����l[%] (��)"
                        .nMin = 0
                        .nMax = 100
                        .nDecPlaces = 0
                        .lstItem.Add("�Ɩ����u_�����l_1")
                        .lstItem.Add("�Ɩ����u_�����l_2")
                        .lstItem.Add("�Ɩ����u_�����l_3")
                        .lstItem.Add("�Ɩ����u_�����l_4")
                        .lstItem.Add("�Ɩ����u_�����l_5")
                    Case Else
                        Continue For
                End Select
            End With
            m_dctTrendInf.Add(em, typ)
        Next
    End Sub

    ''' <summary>
    ''' ��ʏ����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()

        '�O���b�h�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).dgvHanrei = dgvHanrei1
        m_typTrendDsp(1).dgvHanrei = dgvHanrei2
        m_typTrendDsp(2).dgvHanrei = dgvHanrei3
        m_typTrendDsp(3).dgvHanrei = dgvHanrei4
        m_typTrendDsp(4).dgvHanrei = dgvHanrei5

        '�����l�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).nudGraphMin = nudGraphMin1
        m_typTrendDsp(1).nudGraphMin = nudGraphMin2
        m_typTrendDsp(2).nudGraphMin = nudGraphMin3
        m_typTrendDsp(3).nudGraphMin = nudGraphMin4
        m_typTrendDsp(4).nudGraphMin = nudGraphMin5

        '����l�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).nudGraphMax = nudGraphMax1
        m_typTrendDsp(1).nudGraphMax = nudGraphMax2
        m_typTrendDsp(2).nudGraphMax = nudGraphMax3
        m_typTrendDsp(3).nudGraphMax = nudGraphMax4
        m_typTrendDsp(4).nudGraphMax = nudGraphMax5

        '�s�N�`���[�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).picGraph = picGraph1
        m_typTrendDsp(1).picGraph = picGraph2
        m_typTrendDsp(2).picGraph = picGraph3
        m_typTrendDsp(3).picGraph = picGraph4
        m_typTrendDsp(4).picGraph = picGraph5

        '�O���t�v���b�g�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).chkGraphProt = chkGraphProt1
        m_typTrendDsp(1).chkGraphProt = chkGraphProt2
        m_typTrendDsp(2).chkGraphProt = chkGraphProt3
        m_typTrendDsp(3).chkGraphProt = chkGraphProt4
        m_typTrendDsp(4).chkGraphProt = chkGraphProt5

        '�O���t���R���{BOX�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).cmbName = cmbName1
        m_typTrendDsp(1).cmbName = cmbName2
        m_typTrendDsp(2).cmbName = cmbName3
        m_typTrendDsp(3).cmbName = cmbName4
        m_typTrendDsp(4).cmbName = cmbName5

        ''�ő�l/�ŏ��l/���ϒl�\��DGV�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).dgvCalc = dgvCalc1
        m_typTrendDsp(1).dgvCalc = dgvCalc2
        m_typTrendDsp(2).dgvCalc = dgvCalc3
        m_typTrendDsp(3).dgvCalc = dgvCalc4
        m_typTrendDsp(4).dgvCalc = dgvCalc5

        '�}�b�v�̃����o�[�ϐ��ݒ�
        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            With m_typTrendDsp(ii)

                '�}�b�v������`
                .clsGraph = New tClass.tClass_MapBase
                MapInitialize(.clsGraph)

                '�R���{�{�b�N�X�ݒ�
                .emType = CType(tMod.ReadIniInt(SO_GAMENN, "COILTRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, ii), EM_COIL_TREND)
                cmbName_Init(.cmbName, .emType)

                '�O���b�h�̏����ݒ�
                dgvHanreiInit(.dgvHanrei)
                dgvCalcInit(.dgvCalc)

                '�C�x���g�n���h���֘A�t��
                ' �}�b�v
                AddHandler .clsGraph.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

                ' �f�[�^�O���b�h�r���[
                AddHandler .dgvHanrei.CellContentClick, AddressOf dgvGraph_CellContentClick
                AddHandler .dgvHanrei.SelectionChanged, AddressOf dgvGraph_SelectionChanged

                ' �s�N�`���[�{�b�N�X
                AddHandler .picGraph.Paint, AddressOf PictureBox_Paint

                ' �`�F�b�N�{�b�N�X
                AddHandler .chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

                ' �X�s�� �{�b�N�X (�A�b�v�_�E�� �R���g���[��)
                AddHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
                AddHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            End With
        Next

    End Sub

    ''' <summary>
    ''' �}�b�v��{����`
    ''' </summary>
    ''' <param name="MapBase">�O���t�}�b�v</param>
    ''' <remarks></remarks>
    Private Sub MapInitialize(ByVal MapBase As tClass.tClass_MapBase)
        '''' �O���t�`��ɕK�v�ȏ���ݒ�
        With MapBase
            '' �\���̈���
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph1.Size
            .tProp_AriaInf_Margin = New Padding(50, 10, 15, 25)

            '' �}�b�v�̈�
            .tProp_MapInf_IsCenterX = False
            .tProp_MapInf_IsCenterY = False
            .tProp_MapInf_IsBaseX_L = True
            .tProp_MapInf_IsBaseY_T = False
            .tProp_MapInf_IsStrechX = True
            .tProp_MapInf_IsStrechY = True
            .tProp_MapInf_LimitXmin = 0
            .tProp_MapInf_LimitXmax = COIL_LEN_MAX * 1000
            .tProp_MapInf_LimitYmin = 0
            .tProp_MapInf_LimitYmax = 1000
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

            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(50 * 1000, 200)
            .tProp_MemoriInf_ScaleStringFormatX = "###0"
            .tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1000, 1)
            .tProp_MemoriInf_StringDisp_T = False
            .tProp_MemoriInf_StringDisp_B = False
            .tProp_MemoriInf_StringDisp_L = False
            .tProp_MemoriInf_StringDisp_R = False
            .tProp_MemoriInf_LineDsp_X = False
            .tProp_MemoriInf_LineDsp_Y = False
        End With
    End Sub
    ''' <summary>
    ''' ��ʃf�[�^�`�� (�ݒ�ύX���ɃC�x���g���s) (��ʕ\�����ɂ�)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDsp(ByRef typDsp As TYP_TREND_DSP)

        Try

            ' ���̓f�[�^�`�F�b�N
            If False = InputDataCheck(typDsp) Then
                Exit Try
            End If

            ' �㉺���ێ�
            typDsp.nMax = CInt(typDsp.nudGraphMax.Value)
            typDsp.nMin = CInt(typDsp.nudGraphMin.Value)

            '''' �v���b�g�f�[�^���Z�b�g
            GetProtData(typDsp)

            '''' �ŏ��A�ő�A���σf�[�^���Z�b�g
            DspCalcData(typDsp)

            '''' �}�b�v�̈�Ē�`
            MapReSetring(typDsp)

            '''' �}�b�v�Đ����˗�
            typDsp.clsGraph.DrawPaint()

            '''' �}�b�v�ĕ`��
            typDsp.picGraph.Refresh()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("��ʃf�[�^�`�� ��O[{0}]", ex.Message))
        End Try


    End Sub

    ''' <summary>
    ''' ���̓f�[�^�`�F�b�N
    ''' </summary>
    ''' <param name="typDsp">�O���t�O���[�v�ԍ�(�O�I���W��)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InputDataCheck(ByRef typDsp As TYP_TREND_DSP) As Boolean
        Dim strMin As String        ' dgv�̃f�[�^�i�[�p
        Dim strMax As String        ' dgv�̃f�[�^�i�[�p


        '�㉺�����͒l�`�F�b�N
        strMin = CStr(typDsp.nudGraphMin.Value)
        strMax = CStr(typDsp.nudGraphMax.Value)

        'NumericUpDown�ňُ펞�Ƀ��b�Z�[�W���o���Ɨ����邽�߁A�F�ւ������݂̂Ƃ���B
        If False = tClass.tClass_ImputCheck.CheckMinMax(strMin, strMax, 0, False, typDsp.cmbName.Text, False) Then
            typDsp.nudGraphMin.BackColor = Color.Red
            typDsp.nudGraphMax.BackColor = Color.Red
            Return False
        Else
            typDsp.nudGraphMin.BackColor = Color.White
            typDsp.nudGraphMax.BackColor = Color.White
        End If


        Return True
    End Function

    ''' <summary>
    ''' �}�b�v�Ē�`
    ''' </summary>
    ''' <param name="typDsp">�O���t�\���e�[�u��</param>
    ''' <remarks></remarks>
    Private Sub MapReSetring(ByVal typDsp As TYP_TREND_DSP)

        '''' �O���t�`��ɕK�v�ȏ���ݒ�
        With typDsp.clsGraph
            '' �}�b�v�̈�
            .tProp_MapInf_LimitXmax = m_typCoilInf.nCoilLen
            .tProp_MapInf_LimitYmin = CInt(typDsp.nMin)
            .tProp_MapInf_LimitYmax = CInt(typDsp.nMax)

        End With
    End Sub

    ''' <summary>
    ''' dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If 0 > e.RowIndex Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case em_DataColumnType.iColor  '�\���F
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
            Case em_DataColumnType.iCheck
                dgv(e.ColumnIndex, e.RowIndex).Value = dgv(e.ColumnIndex, e.RowIndex).EditedFormattedValue
            Case em_DataColumnType.iName
        End Select

        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' �O���t�O���[�v�̌���
            If sender Is m_typTrendDsp(ii).dgvHanrei Then
                If 0 < m_typTrendDsp(ii).nCnt Then
                    ''''' �}�b�v�Đ����˗�
                    m_typTrendDsp(ii).clsGraph.DrawPaint()

                    ''''' �}�b�v�ĕ`��
                    m_typTrendDsp(ii).picGraph.Refresh()

                    Exit For
                End If
            End If

        Next ii

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

    ''' <summary>
    ''' �}�b�v�\���F�{�^���������ꂽ��A�F�I���_�C�A���O��\�����A�{�^���̐F��ύX
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
    ''' �`��
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' </summary>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' �O���t�O���[�v�̌���
            If sender Is m_typTrendDsp(ii).picGraph Then
                '''' �r�b�g�}�b�v�擾
                Dim bmp As Bitmap = m_typTrendDsp(ii).clsGraph.tProp_Bmp
                If bmp Is Nothing Then Return

                '''' �w�i���Z�b�g
                e.Graphics.DrawImage(bmp, 0, 0)
            End If
        Next ii

    End Sub

    ''' <summary>
    ''' ��ʃf�[�^�`��
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)
        Dim ii As Integer                   ' INDEX
        Dim jj As Integer                   ' INDEX
        Dim nPixel As Integer               ' �}��X���ʒu
        Dim nGraphXInterval As Integer      ' X���\������
        Dim nGraphXPoint As Integer         ' X���\���|�C���^
        Dim nGraphYInterval As Integer      ' Y���\������
        Dim nGraphYPoint As Integer         ' Y���\���|�C���^
        Dim strWork As String               ' ���[�N�G���A
        Try

            ' �O���t�O���[�v�̌���
            For Each typ As TYP_TREND_DSP In m_typTrendDsp
                If cls IsNot typ.clsGraph Then
                    Continue For
                End If

                'If Not m_dctTrendInf.ContainsKey(typ.emType) Then
                '    Exit Try
                'End If
                'Dim inf As TYP_TREND_INF = m_dctTrendInf(typ.emType)

                ' '' X���}��\��
                '' X���X�P�[���Ԋu�Z�o
                Dim dLen As Double = m_typCoilInf.nCoilLen / 1000
                If dLen <= 200 Then                                         ' �R�C���� 200m�ȉ��Ȃ�20m�Ԋu
                    nGraphXInterval = 20
                ElseIf dLen <= 1000 Then                                    ' �R�C���� 1000m�ȉ��Ȃ�100m�Ԋu
                    nGraphXInterval = 100
                ElseIf dLen <= 3000 Then                                    ' �R�C���� 3000m�ȉ��Ȃ�200m�Ԋu
                    nGraphXInterval = 200
                ElseIf dLen <= 6000 Then                                    ' �R�C���� 6000m�ȉ��Ȃ�500m�Ԋu
                    nGraphXInterval = 500
                ElseIf dLen <= 10000 Then                                   ' �R�C���� 10000m�ȉ��Ȃ�1000m�Ԋu
                    nGraphXInterval = 1000
                Else                                                        ' �R�C���� 10000m�z���Ȃ�2000m�Ԋu
                    nGraphXInterval = 2000
                End If
                nGraphXInterval *= 1000                                     ' ���[�g�� -> �~�����[�g�ϊ�

                nGraphXPoint = 1
                For jj = 1 To m_typCoilInf.nCoilLen

                    If jj = nGraphXPoint Or jj = m_typCoilInf.nCoilLen Then

                        ' X�|�W�V�����擾
                        nPixel = cls.GetVtoP_X(jj)

                        ' �ŏ��ƍŌ�͏c����\�����Ȃ�
                        If 1 <> jj AndAlso m_typCoilInf.nCoilLen <> jj Then
                            '��
                            g.DrawLine(cls.tProp_MemoriInf.LinePen, nPixel, cls.tProp_CalcAns.MapRect.Top + 1, nPixel, cls.tProp_CalcAns.MapRect.Bottom)

                        End If

                        ' ����
                        strWork = CStr(jj \ 1000) & "m"
                        cls.DrawString(g, nPixel, cls.tProp_CalcAns.MapRect.Bottom, 0, +cls.tProp_MemoriInf.nPosStringX, strWork, ContentAlignment.BottomCenter, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                        nGraphXPoint += nGraphXInterval
                    End If

                Next jj

                '' Y���}��\��
                ' Y���X�P�[���Ԋu�Z�o
                nGraphYInterval = ((typ.nMax - typ.nMin) - 1) \ mc_YScale
                If 0 <> ((typ.nMax - typ.nMin) - 1) Mod mc_YScale Then
                    nGraphYInterval += 1
                End If
                nGraphYPoint = typ.nMin
                For jj = typ.nMin To typ.nMax
                    If jj = nGraphYPoint Or jj = typ.nMax Then
                        ' Y�|�W�V�����擾
                        nPixel = cls.GetVtoP_Y(jj)

                        ' �ŏ��ƍŌ�͉�����\�����Ȃ�
                        If typ.nMin <> jj AndAlso typ.nMax <> jj Then
                            '��
                            g.DrawLine(cls.tProp_MemoriInf.LinePen, cls.tProp_CalcAns.MapRect.Left + 1, nPixel, cls.tProp_CalcAns.MapRect.Right, nPixel)

                        End If

                        ' ����
                        strWork = Format(jj, cls.tProp_MemoriInf.sScaleStringFormatY)
                        cls.DrawString(g, cls.tProp_CalcAns.MapRect.Left, nPixel, -cls.tProp_MemoriInf.nPosStringY, 0, strWork, ContentAlignment.MiddleLeft, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                        nGraphYPoint += nGraphYInterval
                    End If

                Next jj

                ' �O���t�\���f�[�^=0�ŏI��
                If 0 = typ.nCnt Then Return

                '''' �`��
                Dim dVal As Single
                Dim nAddr As Integer = -1
                Dim x1, y1, x2, y2 As Integer
                For ii = 0 To typ.dgvHanrei.RowCount - 1
                    ' �O���t�\���`�F�b�N ��\���Ŏ��̃O���t�i���j
                    If False = CBool(typ.dgvHanrei(em_DataColumnType.iCheck, ii).Value) Then Continue For

                    For jj = 0 To typ.nCnt - 1
                        dVal = typ.nData(ii, jj)
                        If NON_TRAND_DATA = dVal Then
                            Continue For
                        Else
                            nAddr = jj
                            Exit For
                        End If
                    Next
                    If -1 = nAddr Then
                        Continue For
                    End If
                    ' �n�_�Z�o (�擪�̍��W�Z�o) X��
                    x1 = cls.GetVtoP_X(typ.nCoilPos(nAddr) + 1)

                    ' �n�_�Z�o (�擪�̍��W�Z�o) Y��
                    dVal = typ.nData(ii, nAddr)
                    If dVal > typ.nMax Then dVal = typ.nMax
                    If dVal < typ.nMin Then dVal = typ.nMin
                    y1 = cls.GetVtoP_Y(dVal)

                    Using p As New Pen(typ.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor, 1)

                        ' �v���b�g�̎w��
                        If typ.chkGraphProt.Checked Then
                            g.FillEllipse(New SolidBrush(p.Color), x1 - 3, y1 - 3, 6, 6)       ' ��
                        End If

                        For jj = 0 To typ.nCnt - 1

                            ' �I�_�Z�o X��
                            x2 = cls.GetVtoP_X(typ.nCoilPos(jj) + 1)

                            ' �I�_�Z�o Y��
                            dVal = typ.nData(ii, jj)
                            If dVal = NON_TRAND_DATA Then Continue For
                            If dVal > typ.nMax Then dVal = typ.nMax
                            If dVal < typ.nMin Then dVal = typ.nMin
                            y2 = cls.GetVtoP_Y(dVal)

                            ' �`��
                            g.DrawLine(p, x1, y1, x2, y2)

                            ' �v���b�g�̎w��
                            If typ.chkGraphProt.Checked Then
                                g.FillEllipse(New SolidBrush(p.Color), x2 - 3, y2 - 3, 6, 6)       ' ��
                            End If

                            ' ���̏���
                            x1 = x2
                            y1 = y2

                        Next jj

                    End Using
                Next
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("��ʃf�[�^�`��1 ��O[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �v���b�g�\���`�F�b�N��ԕύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkGraphProt_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' �O���t�O���[�v�̌���
            If sender Is m_typTrendDsp(ii).chkGraphProt Then
                If 0 < m_typTrendDsp(ii).nCnt Then
                    '''' �}�b�v�Đ����˗�
                    m_typTrendDsp(ii).clsGraph.DrawPaint()

                    '''' �}�b�v�ĕ`��
                    m_typTrendDsp(ii).picGraph.Refresh()

                    Exit For
                End If
            End If
        Next ii
    End Sub



    ''' <summary>
    ''' �t�H�[���\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Try

            '''' ��ʏ�����
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                DspInit(m_typTrendDsp(ii))
            Next ii

            m_bSetEnd = True

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                SetDsp(m_typTrendDsp(ii))
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�t�H�[���\�� ��O[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' ��ʏ����\��
    ''' </summary>
    ''' <param name="typDsp">�O���t�\���e�[�u��</param>
    ''' <remarks></remarks>
    Private Sub DspInit(ByRef typDsp As TYP_TREND_DSP)
        Dim ii As Integer                       ' INDEX

        With typDsp

            If Not m_dctTrendInf.ContainsKey(.emType) Then
                '���݂��Ȃ��H
                Exit Sub
            End If

            Dim inf As TYP_TREND_INF = m_dctTrendInf(.emType)

            ' �}����e�\��
            .dgvHanrei.RowCount = 0

            For ii = 0 To inf.lstItem.Count - 1
                .dgvHanrei.RowCount = .dgvHanrei.RowCount + 1
                ' �\���E��\��
                Dim nDisp As Integer = tMod.ReadIniInt(SO_GAMENN, "COILTRANDDISP_" & CStr(.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, 1)
                If 1 = nDisp Or 0 = nDisp Then
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = nDisp
                Else
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = 1
                End If

                ' �\���F
                Dim strColor As String = tMod.ReadIniStr(SO_GAMENN, "COILTRANDCOLOR_" & CStr(.emType) & "_" & CStr(ii + 1), LOCALINI_NAME)
                If "" <> strColor Then
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32("FF" & strColor, 16))
                Else
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32(Hex(sdcDefault(ii).ToArgb), 16))
                End If

                ' ����
                .dgvHanrei(em_DataColumnType.iName, ii).Value = inf.lstItem.Item(ii)
                'End If
            Next ii

            '�n���h������U��������
            RemoveHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            RemoveHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

            .nudGraphMin.Maximum = inf.nMax
            .nudGraphMin.Minimum = inf.nMin
            .nudGraphMin.DecimalPlaces = inf.nDecPlaces
            If 0 < inf.nDecPlaces Then
                .nudGraphMin.Increment = CDec(1 / (10 ^ inf.nDecPlaces))
            Else
                .nudGraphMin.Increment = 1
            End If

            .nudGraphMax.Maximum = .nudGraphMin.Maximum
            .nudGraphMax.Minimum = .nudGraphMin.Minimum
            .nudGraphMax.DecimalPlaces = .nudGraphMin.DecimalPlaces
            .nudGraphMax.Increment = .nudGraphMin.Increment

            ' ����l
            .nudGraphMax.Text = tMod.ReadIniStr(SO_GAMENN, "COILTRANDMAX_" & CStr(.emType), LOCALINI_NAME)
            If False = IsNumeric(.nudGraphMax.Text) Then
                .nudGraphMax.Text = inf.nMax.ToString
            End If

            ' �����l
            .nudGraphMin.Text = tMod.ReadIniStr(SO_GAMENN, "COILTRANDMIN_" & CStr(.emType), LOCALINI_NAME)
            If False = IsNumeric(.nudGraphMin.Text) Then
                .nudGraphMin.Text = inf.nMin.ToString
            End If

            If False = tClass.tClass_ImputCheck.CheckMinMax(.nudGraphMin.Text, .nudGraphMax.Text, 0, False, .cmbName.Text, False) Then
                'NumericUpDown�ňُ�̏ꍇ�́A�f�t�H���g�ݒ�ɂ���
                .nudGraphMax.Text = inf.nMax.ToString
                .nudGraphMin.Text = inf.nMin.ToString
            End If

            '�n���h���Đݒ�
            AddHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            AddHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

        End With

    End Sub

    ''' <summary>
    ''' �g�����h�ݒ�ۑ�(Ini�t�@�C��������)
    ''' </summary>
    ''' <param name="typDsp"></param>
    Private Sub PutTrendDsp(ByVal typDsp As TYP_TREND_DSP)
        Dim strWk As String     ' �����񃏁[�N

        'ini�t�@�C����񏑍�
        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            '�\���E��\����񏑍�
            If True = CBool(typDsp.dgvHanrei(em_DataColumnType.iCheck, ii).Value) Then
                tMod.WriteIni(SO_GAMENN, "COILTRANDDISP_" & CStr(typDsp.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "COILTRANDDISP_" & CStr(typDsp.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, "0")
            End If
        Next

        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            '�\���F��񏑍�
            strWk = Microsoft.VisualBasic.Right(Hex(typDsp.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor.ToArgb), 6)
            tMod.WriteIni(SO_GAMENN, "COILTRANDCOLOR_" & CStr(typDsp.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, strWk)
        Next

        '�㉺�����͒l�`�F�b�N
        Dim strMin As String = CStr(typDsp.nudGraphMin.Value)
        Dim strMax As String = CStr(typDsp.nudGraphMax.Value)

        If False = tClass.tClass_ImputCheck.CheckMinMax(strMin, strMax, 0, False, typDsp.cmbName.Text, False) Then
            'NumericUpDown�ňُ펞�̏ꍇ�́A�����݂��s��Ȃ�
        Else
            '�O���t����l����
            tMod.WriteIni(SO_GAMENN, "COILTRANDMAX_" & CStr(typDsp.emType), LOCALINI_NAME, typDsp.nudGraphMax.Text)

            '�O���t�����l����
            tMod.WriteIni(SO_GAMENN, "COILTRANDMIN_" & CStr(typDsp.emType), LOCALINI_NAME, typDsp.nudGraphMin.Text)
        End If
    End Sub

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����L�[�C�x���g
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No 0�I���W��</param>
    ''' <param name="enable">�{�^���R���g���[���̗L������</param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Select Case FncNo
            Case 8                                  ' CSV�o��
                CsvOutTrand(CSV_PATH, "�R�C���g�����h���.csv")

            Case 9                                  ' ��ʈ��
                PrintOut()

            Case 10                                 ' ��ʕۑ�
                HardCopy(IMG_PATH, "�R�C���g�����h���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

            Case 11                                 ' ���j���[��ʕ\��
                MyBase.ExecFromClose()
        End Select
    End Sub

    ''' <summary>
    ''' �w��Ǘ�No�̃R�C���g�����h���(COIL_TRAND)��ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetCoilTrandData()
        Dim strSQL As String                    ' SQL��
        Dim typData As TYP_TREND_DATA = Nothing

        '''' �O���t�f�[�^������
        m_lstData.Clear()

        '''' SQL�ڑ�����
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        '''' SQL������
        strSQL = ""
        strSQL += " SELECT * "
        strSQL += " FROM " & DB_COIL_TRAND
        strSQL += " WHERE �Ǘ�No = '" & m_typCoilInf.strKanriNo & "'"
        strSQL += " ORDER BY �擾�ʒu ASC"

        '''' �f�[�^�擾
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        Try
            While sqlRead.Read()
                With sqlRead
                    typData.initialize()

                    typData.TorB = .GetByte(.GetOrdinal("�\���敪"))
                    typData.nPos = .GetInt32(.GetOrdinal("�擾�ʒu"))

                    If m_typCoilInf.bCorrect Then
                        '�擾�ʒu�ɕ␳������Z
                        typData.nPos = CInt(typData.nPos * m_typCoilInf.dCorrect)
                    End If

                    If m_bNagate = True Then
                        ' �f�[�^�̏㉺���]������΃g�b�v����̋������]
                        typData.nPos = m_typCoilInf.nCoilLen - typData.nPos
                        If typData.nPos < 0 Then
                            typData.nPos = 0
                        End If
                    End If

                    '���C�����x
                    typData.nSpeed = .GetInt32(.GetOrdinal("���C�����x"))

                    '��
                    typData.dItahaba(0) = .GetFloat(.GetOrdinal("��Min"))
                    typData.dItahaba(1) = .GetFloat(.GetOrdinal("��Max"))

                    '�J�����P�x
                    typData.typCamInf(0).nKidoR = .GetByte(.GetOrdinal("�J����1_�P�xR_C"))
                    typData.typCamInf(0).nKidoG = .GetByte(.GetOrdinal("�J����1_�P�xG_C"))
                    typData.typCamInf(0).nKidoB = .GetByte(.GetOrdinal("�J����1_�P�xB_C"))
                    typData.typCamInf(1).nKidoR = .GetByte(.GetOrdinal("�J����2_�P�xR_C"))
                    typData.typCamInf(1).nKidoG = .GetByte(.GetOrdinal("�J����2_�P�xG_C"))
                    typData.typCamInf(1).nKidoB = .GetByte(.GetOrdinal("�J����2_�P�xB_C"))
                    typData.typCamInf(2).nKidoR = .GetByte(.GetOrdinal("�J����3_�P�xR_C"))
                    typData.typCamInf(2).nKidoG = .GetByte(.GetOrdinal("�J����3_�P�xG_C"))
                    typData.typCamInf(2).nKidoB = .GetByte(.GetOrdinal("�J����3_�P�xB_C"))
                    typData.typCamInf(3).nKidoR = .GetByte(.GetOrdinal("�J����4_�P�xR_C"))
                    typData.typCamInf(3).nKidoG = .GetByte(.GetOrdinal("�J����4_�P�xG_C"))
                    typData.typCamInf(3).nKidoB = .GetByte(.GetOrdinal("�J����4_�P�xB_C"))
                    typData.typCamInf(4).nKidoR = .GetByte(.GetOrdinal("�J����5_�P�xR_C"))
                    typData.typCamInf(4).nKidoG = .GetByte(.GetOrdinal("�J����5_�P�xG_C"))
                    typData.typCamInf(4).nKidoB = .GetByte(.GetOrdinal("�J����5_�P�xB_C"))
                    typData.typCamInf(5).nKidoR = .GetByte(.GetOrdinal("�J����6_�P�xR_C"))
                    typData.typCamInf(5).nKidoG = .GetByte(.GetOrdinal("�J����6_�P�xG_C"))
                    typData.typCamInf(5).nKidoB = .GetByte(.GetOrdinal("�J����6_�P�xB_C"))
                    typData.typCamInf(6).nKidoR = .GetByte(.GetOrdinal("�J����7_�P�xR_C"))
                    typData.typCamInf(6).nKidoG = .GetByte(.GetOrdinal("�J����7_�P�xG_C"))
                    typData.typCamInf(6).nKidoB = .GetByte(.GetOrdinal("�J����7_�P�xB_C"))
                    typData.typCamInf(7).nKidoR = .GetByte(.GetOrdinal("�J����8_�P�xR_C"))
                    typData.typCamInf(7).nKidoG = .GetByte(.GetOrdinal("�J����8_�P�xG_C"))
                    typData.typCamInf(7).nKidoB = .GetByte(.GetOrdinal("�J����8_�P�xB_C"))
                    typData.typCamInf(8).nKidoR = .GetByte(.GetOrdinal("�J����9_�P�xR_C"))
                    typData.typCamInf(8).nKidoG = .GetByte(.GetOrdinal("�J����9_�P�xG_C"))
                    typData.typCamInf(8).nKidoB = .GetByte(.GetOrdinal("�J����9_�P�xB_C"))
                    typData.typCamInf(9).nKidoR = .GetByte(.GetOrdinal("�J����10_�P�xR_C"))
                    typData.typCamInf(9).nKidoG = .GetByte(.GetOrdinal("�J����10_�P�xG_C"))
                    typData.typCamInf(9).nKidoB = .GetByte(.GetOrdinal("�J����10_�P�xB_C"))

                    '�J�����I������
                    typData.typCamInf(0).nTimeR = .GetInt16(.GetOrdinal("�J����1_�I������R"))
                    typData.typCamInf(0).nTimeG = .GetInt16(.GetOrdinal("�J����1_�I������G"))
                    typData.typCamInf(0).nTimeB = .GetInt16(.GetOrdinal("�J����1_�I������B"))
                    typData.typCamInf(1).nTimeR = .GetInt16(.GetOrdinal("�J����2_�I������R"))
                    typData.typCamInf(1).nTimeG = .GetInt16(.GetOrdinal("�J����2_�I������G"))
                    typData.typCamInf(1).nTimeB = .GetInt16(.GetOrdinal("�J����2_�I������B"))
                    typData.typCamInf(2).nTimeR = .GetInt16(.GetOrdinal("�J����3_�I������R"))
                    typData.typCamInf(2).nTimeG = .GetInt16(.GetOrdinal("�J����3_�I������G"))
                    typData.typCamInf(2).nTimeB = .GetInt16(.GetOrdinal("�J����3_�I������B"))
                    typData.typCamInf(3).nTimeR = .GetInt16(.GetOrdinal("�J����4_�I������R"))
                    typData.typCamInf(3).nTimeG = .GetInt16(.GetOrdinal("�J����4_�I������G"))
                    typData.typCamInf(3).nTimeB = .GetInt16(.GetOrdinal("�J����4_�I������B"))
                    typData.typCamInf(4).nTimeR = .GetInt16(.GetOrdinal("�J����5_�I������R"))
                    typData.typCamInf(4).nTimeG = .GetInt16(.GetOrdinal("�J����5_�I������G"))
                    typData.typCamInf(4).nTimeB = .GetInt16(.GetOrdinal("�J����5_�I������B"))
                    typData.typCamInf(5).nTimeR = .GetInt16(.GetOrdinal("�J����6_�I������R"))
                    typData.typCamInf(5).nTimeG = .GetInt16(.GetOrdinal("�J����6_�I������G"))
                    typData.typCamInf(5).nTimeB = .GetInt16(.GetOrdinal("�J����6_�I������B"))
                    typData.typCamInf(6).nTimeR = .GetInt16(.GetOrdinal("�J����7_�I������R"))
                    typData.typCamInf(6).nTimeG = .GetInt16(.GetOrdinal("�J����7_�I������G"))
                    typData.typCamInf(6).nTimeB = .GetInt16(.GetOrdinal("�J����7_�I������B"))
                    typData.typCamInf(7).nTimeR = .GetInt16(.GetOrdinal("�J����8_�I������R"))
                    typData.typCamInf(7).nTimeG = .GetInt16(.GetOrdinal("�J����8_�I������G"))
                    typData.typCamInf(7).nTimeB = .GetInt16(.GetOrdinal("�J����8_�I������B"))
                    typData.typCamInf(8).nTimeR = .GetInt16(.GetOrdinal("�J����9_�I������R"))
                    typData.typCamInf(8).nTimeG = .GetInt16(.GetOrdinal("�J����9_�I������G"))
                    typData.typCamInf(8).nTimeB = .GetInt16(.GetOrdinal("�J����9_�I������B"))
                    typData.typCamInf(9).nTimeR = .GetInt16(.GetOrdinal("�J����10_�I������R"))
                    typData.typCamInf(9).nTimeG = .GetInt16(.GetOrdinal("�J����10_�I������G"))
                    typData.typCamInf(9).nTimeB = .GetInt16(.GetOrdinal("�J����10_�I������B"))

                    '�J�����Q�C��
                    typData.typCamInf(0).dGainR = CSng(.GetDouble(.GetOrdinal("�J����1_�Q�C��_R")))
                    typData.typCamInf(0).dGainG = CSng(.GetDouble(.GetOrdinal("�J����1_�Q�C��_G")))
                    typData.typCamInf(0).dGainB = CSng(.GetDouble(.GetOrdinal("�J����1_�Q�C��_B")))
                    typData.typCamInf(1).dGainR = CSng(.GetDouble(.GetOrdinal("�J����2_�Q�C��_R")))
                    typData.typCamInf(1).dGainG = CSng(.GetDouble(.GetOrdinal("�J����2_�Q�C��_G")))
                    typData.typCamInf(1).dGainB = CSng(.GetDouble(.GetOrdinal("�J����2_�Q�C��_B")))
                    typData.typCamInf(2).dGainR = CSng(.GetDouble(.GetOrdinal("�J����3_�Q�C��_R")))
                    typData.typCamInf(2).dGainG = CSng(.GetDouble(.GetOrdinal("�J����3_�Q�C��_G")))
                    typData.typCamInf(2).dGainB = CSng(.GetDouble(.GetOrdinal("�J����3_�Q�C��_B")))
                    typData.typCamInf(3).dGainR = CSng(.GetDouble(.GetOrdinal("�J����4_�Q�C��_R")))
                    typData.typCamInf(3).dGainG = CSng(.GetDouble(.GetOrdinal("�J����4_�Q�C��_G")))
                    typData.typCamInf(3).dGainB = CSng(.GetDouble(.GetOrdinal("�J����4_�Q�C��_B")))
                    typData.typCamInf(4).dGainR = CSng(.GetDouble(.GetOrdinal("�J����5_�Q�C��_R")))
                    typData.typCamInf(4).dGainG = CSng(.GetDouble(.GetOrdinal("�J����5_�Q�C��_G")))
                    typData.typCamInf(4).dGainB = CSng(.GetDouble(.GetOrdinal("�J����5_�Q�C��_B")))
                    typData.typCamInf(5).dGainR = CSng(.GetDouble(.GetOrdinal("�J����6_�Q�C��_R")))
                    typData.typCamInf(5).dGainG = CSng(.GetDouble(.GetOrdinal("�J����6_�Q�C��_G")))
                    typData.typCamInf(5).dGainB = CSng(.GetDouble(.GetOrdinal("�J����6_�Q�C��_B")))
                    typData.typCamInf(6).dGainR = CSng(.GetDouble(.GetOrdinal("�J����7_�Q�C��_R")))
                    typData.typCamInf(6).dGainG = CSng(.GetDouble(.GetOrdinal("�J����7_�Q�C��_G")))
                    typData.typCamInf(6).dGainB = CSng(.GetDouble(.GetOrdinal("�J����7_�Q�C��_B")))
                    typData.typCamInf(7).dGainR = CSng(.GetDouble(.GetOrdinal("�J����8_�Q�C��_R")))
                    typData.typCamInf(7).dGainG = CSng(.GetDouble(.GetOrdinal("�J����8_�Q�C��_G")))
                    typData.typCamInf(7).dGainB = CSng(.GetDouble(.GetOrdinal("�J����8_�Q�C��_B")))
                    typData.typCamInf(8).dGainR = CSng(.GetDouble(.GetOrdinal("�J����9_�Q�C��_R")))
                    typData.typCamInf(8).dGainG = CSng(.GetDouble(.GetOrdinal("�J����9_�Q�C��_G")))
                    typData.typCamInf(8).dGainB = CSng(.GetDouble(.GetOrdinal("�J����9_�Q�C��_B")))
                    typData.typCamInf(9).dGainR = CSng(.GetDouble(.GetOrdinal("�J����10_�Q�C��_R")))
                    typData.typCamInf(9).dGainG = CSng(.GetDouble(.GetOrdinal("�J����10_�Q�C��_G")))
                    typData.typCamInf(9).dGainB = CSng(.GetDouble(.GetOrdinal("�J����10_�Q�C��_B")))

                    '�E�H�[�N��
                    typData.dWalk = .GetFloat(.GetOrdinal("�E�H�[�N��"))

                    '�Ɩ����u_�����l
                    typData.nDimming(0) = .GetInt16(.GetOrdinal("�Ɩ����u_�����l_1"))
                    typData.nDimming(1) = .GetInt16(.GetOrdinal("�Ɩ����u_�����l_2"))
                    typData.nDimming(2) = .GetInt16(.GetOrdinal("�Ɩ����u_�����l_3"))
                    typData.nDimming(3) = .GetInt16(.GetOrdinal("�Ɩ����u_�����l_4"))
                    typData.nDimming(4) = .GetInt16(.GetOrdinal("�Ɩ����u_�����l_5"))

                End With

                m_lstData.Add(typData)

            End While

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("COIL_TRAND �擾�ُ� [{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub


    ''' <summary>
    ''' �R�C���g�����h���(COIL_TRAND)��Ǎ��݁@MAX,MIN,AVE�𒊏o
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetCalc()
        Dim strSQL As String                    ' SQL��
        Dim typ As TYP_TREND_CALC = Nothing

        '''' SQL�ڑ�����
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        '''' SQL������
        strSQL = ""
        strSQL &= " SELECT ISNULL(MAX(���C�����x),0) AS ���xMAX"
        strSQL &= ", ISNULL(MIN(���C�����x),0) AS ���xMIN"
        strSQL &= ", ISNULL(AVG(���C�����x),0) AS ���xAVG"
        'strSQL &= ", ISNULL(MAX(��Min),0) AS ��MAX1"
        'strSQL &= ", ISNULL(MIN(��Min),0) AS ��MIN1"
        'strSQL &= ", ISNULL(AVG(��Min),0) AS ��AVG1"
        'strSQL &= ", ISNULL(MAX(��Max),0) AS ��MAX2"
        'strSQL &= ", ISNULL(MIN(��Max),0) AS ��MIN2"
        'strSQL &= ", ISNULL(AVG(��Max),0) AS ��AVG2"
        strSQL &= ", ISNULL(MAX(ABS(�E�H�[�N��)),0) AS �E�H�[�NMAX"
        strSQL &= ", ISNULL(MIN(ABS(�E�H�[�N��)),0) AS �E�H�[�NMIN"
        strSQL &= ", ISNULL(AVG(ABS(�E�H�[�N��)),0) AS �E�H�[�NAVG"
        'strSQL &= ", ISNULL(MAX(�Ɩ����u_�����l),0) AS �����lMAX"
        'strSQL &= ", ISNULL(MIN(�Ɩ����u_�����l),0) AS �����lMIN"
        'strSQL &= ", ISNULL(AVG(�Ɩ����u_�����l),0) AS �����lAVG"
        strSQL += " FROM " & DB_COIL_TRAND
        strSQL += " WHERE �Ǘ�No = '" & m_typCoilInf.strKanriNo & "'"

        '''' �f�[�^�擾
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        Try
            'While sqlRead.Read()
            With sqlRead
                .Read()

                For em As EM_COIL_TREND = EM_COIL_TREND.SPEED To EM_COIL_TREND.MAX
                    typ.initialize()
                    Select Case em
                        Case EM_COIL_TREND.SPEED
                            typ.min = .GetInt32(sqlRead.GetOrdinal("���xMIN")).ToString("#0")
                            typ.max = .GetInt32(sqlRead.GetOrdinal("���xMAX")).ToString("#0")
                            typ.avg = .GetInt32(sqlRead.GetOrdinal("���xAVG")).ToString("#0")
                            'Case EM_COIL_TREND.WIDTH
                            '    typ.min = .GetFloat(sqlRead.GetOrdinal("��MIN")).ToString("#0")
                            '    typ.max = .GetFloat(sqlRead.GetOrdinal("��MAX")).ToString("#0")
                            '    typ.avg = .GetDouble(sqlRead.GetOrdinal("��AVG")).ToString("#0")
                        Case EM_COIL_TREND.WALK
                            typ.min = .GetDouble(sqlRead.GetOrdinal("�E�H�[�NMIN")).ToString("#0")
                            typ.max = .GetDouble(sqlRead.GetOrdinal("�E�H�[�NMAX")).ToString("#0")
                            typ.avg = .GetDouble(sqlRead.GetOrdinal("�E�H�[�NAVG")).ToString("#0")
                            'Case EM_COIL_TREND.LED
                            '    typ.min = .GetByte(sqlRead.GetOrdinal("�����lMIN")).ToString("#0")
                            '    typ.max = .GetByte(sqlRead.GetOrdinal("�����lMAX")).ToString("#0")
                            '    typ.avg = .GetInt32(sqlRead.GetOrdinal("�����lAVG")).ToString("#0")
                        Case Else
                            Continue For
                    End Select

                    m_dctCalc.Add(em, typ)

                Next

            End With
            'End While
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("COIL_TRAND �擾�ُ� [{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try
    End Sub

    ''' <summary>
    ''' �\���̃v���b�g�f�[�^���擾����
    ''' </summary>
    ''' <param name="typDsp">�O���t�\���e�[�u��</param>
    ''' <remarks></remarks>
    Private Sub GetProtData(ByRef typDsp As TYP_TREND_DSP)
        Dim nPos() As Integer                   ' DB�擾�p �擾�ʒu
        Dim nData(,) As Single                  ' DB�擾�p �f�[�^
        Dim nCnt As Integer                     ' DB�擾����

        nCnt = m_lstData.Count
        ReDim Preserve nPos(nCnt - 1)
        ReDim Preserve nData(typDsp.dgvHanrei.RowCount - 1, nCnt - 1)

        '�������i�f�[�^�����̒l���Z�b�g����j
        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            For jj As Integer = 0 To nCnt - 1
                nData(ii, jj) = NON_TRAND_DATA
            Next
        Next
        Select Case typDsp.emType
            Case EM_COIL_TREND.SPEED
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    Dim nCk As Integer = -1
                    '���ɓ���擾�ʒu�ŃZ�b�g�L�肩�H
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' ����擾�ʒu�ŃZ�b�g�Ȃ�
                        nPos(nCnt) = m_lstData(ii).nPos
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCnt) = m_lstData(ii).nSpeed
                        Else
                            nData(1, nCnt) = m_lstData(ii).nSpeed
                        End If
                        nCnt += 1
                    Else
                        ' ���ɓ���擾�ʒu�ŃZ�b�g�L��
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCk) = m_lstData(ii).nSpeed
                        Else
                            nData(1, nCk) = m_lstData(ii).nSpeed
                        End If
                    End If
                Next

            Case EM_COIL_TREND.WIDTH
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    nPos(nCnt) = m_lstData(ii).nPos
                    If 0 = m_lstData(ii).TorB Then
                        nData(0, nCnt) = m_lstData(ii).dItahaba(0)
                        nData(1, nCnt) = m_lstData(ii).dItahaba(1)
                    Else
                        nData(2, nCnt) = m_lstData(ii).dItahaba(0)
                        nData(3, nCnt) = m_lstData(ii).dItahaba(1)
                    End If
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_KIDO_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '�\�f�[�^�̂�
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).nKidoR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).nKidoG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).nKidoB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).nKidoR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).nKidoG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).nKidoB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).nKidoR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).nKidoG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).nKidoB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).nKidoR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).nKidoG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).nKidoB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).nKidoR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).nKidoG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).nKidoB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).nKidoR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).nKidoG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).nKidoB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).nKidoR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).nKidoG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).nKidoB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).nKidoR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).nKidoG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).nKidoB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).nKidoR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).nKidoG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).nKidoB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).nKidoR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).nKidoG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).nKidoB
                    ' �擾�����C���N�������g
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_KIDO_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '���f�[�^�̂�
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).nKidoR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).nKidoG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).nKidoB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).nKidoR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).nKidoG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).nKidoB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).nKidoR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).nKidoG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).nKidoB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).nKidoR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).nKidoG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).nKidoB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).nKidoR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).nKidoG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).nKidoB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).nKidoR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).nKidoG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).nKidoB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).nKidoR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).nKidoG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).nKidoB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).nKidoR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).nKidoG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).nKidoB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).nKidoR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).nKidoG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).nKidoB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).nKidoR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).nKidoG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).nKidoB
                    ' �擾�����C���N�������g
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_GAIN_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '�\�f�[�^�̂�
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).dGainR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).dGainG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).dGainB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).dGainR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).dGainG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).dGainB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).dGainR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).dGainG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).dGainB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).dGainR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).dGainG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).dGainB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).dGainR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).dGainG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).dGainB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).dGainR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).dGainG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).dGainB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).dGainR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).dGainG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).dGainB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).dGainR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).dGainG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).dGainB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).dGainR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).dGainG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).dGainB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).dGainR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).dGainG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).dGainB
                    ' �擾�����C���N�������g
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_GAIN_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '���f�[�^�̂�
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).dGainR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).dGainG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).dGainB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).dGainR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).dGainG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).dGainB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).dGainR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).dGainG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).dGainB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).dGainR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).dGainG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).dGainB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).dGainR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).dGainG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).dGainB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).dGainR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).dGainG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).dGainB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).dGainR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).dGainG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).dGainB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).dGainR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).dGainG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).dGainB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).dGainR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).dGainG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).dGainB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).dGainR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).dGainG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).dGainB
                    ' �擾�����C���N�������g
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_TIME_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '�\�f�[�^�̂�
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeR)
                    nData(1, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeG)
                    nData(2, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeB)
                    nData(3, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeR)
                    nData(4, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeG)
                    nData(5, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeB)
                    nData(6, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeR)
                    nData(7, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeG)
                    nData(8, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeB)
                    nData(9, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeR)
                    nData(10, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeG)
                    nData(11, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeB)
                    nData(12, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeR)
                    nData(13, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeG)
                    nData(14, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeB)
                    nData(15, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeR)
                    nData(16, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeG)
                    nData(17, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeB)
                    nData(18, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeR)
                    nData(19, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeG)
                    nData(20, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeB)
                    nData(21, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeR)
                    nData(22, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeG)
                    nData(23, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeB)
                    nData(24, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeR)
                    nData(25, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeG)
                    nData(26, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeB)
                    nData(27, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeR)
                    nData(28, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeG)
                    nData(29, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeB)
                    nCnt += 1

                Next

            Case EM_COIL_TREND.CAM_TIME_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '���f�[�^�̂�
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeR)
                    nData(1, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeG)
                    nData(2, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeB)
                    nData(3, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeR)
                    nData(4, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeG)
                    nData(5, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeB)
                    nData(6, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeR)
                    nData(7, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeG)
                    nData(8, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeB)
                    nData(9, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeR)
                    nData(10, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeG)
                    nData(11, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeB)
                    nData(12, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeR)
                    nData(13, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeG)
                    nData(14, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeB)
                    nData(15, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeR)
                    nData(16, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeG)
                    nData(17, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeB)
                    nData(18, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeR)
                    nData(19, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeG)
                    nData(20, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeB)
                    nData(21, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeR)
                    nData(22, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeG)
                    nData(23, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeB)
                    nData(24, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeR)
                    nData(25, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeG)
                    nData(26, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeB)
                    nData(27, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeR)
                    nData(28, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeG)
                    nData(29, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeB)
                    nCnt += 1

                Next

            Case EM_COIL_TREND.WALK
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    Dim nCk As Integer = -1
                    '���ɓ���擾�ʒu�ŃZ�b�g�L�肩�H
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' ����擾�ʒu�ŃZ�b�g�Ȃ�
                        nPos(nCnt) = m_lstData(ii).nPos
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCnt) = CInt(m_lstData(ii).dWalk)
                        Else
                            nData(1, nCnt) = CInt(m_lstData(ii).dWalk)
                        End If
                        nCnt += 1
                    Else
                        ' ���ɓ���擾�ʒu�ŃZ�b�g�L��
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCnt) = CInt(m_lstData(ii).dWalk)
                        Else
                            nData(1, nCnt) = CInt(m_lstData(ii).dWalk)
                        End If
                    End If
                Next

            Case EM_COIL_TREND.LED_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '�\�f�[�^�̂�
                        Continue For
                    End If
                    Dim nCk As Integer = -1
                    '���ɓ���擾�ʒu�ŃZ�b�g�L�肩�H
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' ����擾�ʒu�ŃZ�b�g�Ȃ�
                        nPos(nCnt) = m_lstData(ii).nPos
                        nData(0, nCnt) = m_lstData(ii).nDimming(0)
                        nData(1, nCnt) = m_lstData(ii).nDimming(1)
                        nData(2, nCnt) = m_lstData(ii).nDimming(2)
                        nData(3, nCnt) = m_lstData(ii).nDimming(3)
                        nData(4, nCnt) = m_lstData(ii).nDimming(4)
                        nCnt += 1
                    Else
                        ' ���ɓ���擾�ʒu�ŃZ�b�g�L��
                        nData(0, nCk) = m_lstData(ii).nDimming(0)
                        nData(1, nCk) = m_lstData(ii).nDimming(1)
                        nData(2, nCk) = m_lstData(ii).nDimming(2)
                        nData(3, nCk) = m_lstData(ii).nDimming(3)
                        nData(4, nCk) = m_lstData(ii).nDimming(4)
                    End If
                Next
            Case EM_COIL_TREND.LED_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '���f�[�^�̂�
                        Continue For
                    End If
                    Dim nCk As Integer = -1
                    '���ɓ���擾�ʒu�ŃZ�b�g�L�肩�H
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' ����擾�ʒu�ŃZ�b�g�Ȃ�
                        nPos(nCnt) = m_lstData(ii).nPos
                        nData(0, nCnt) = m_lstData(ii).nDimming(0)
                        nData(1, nCnt) = m_lstData(ii).nDimming(1)
                        nData(2, nCnt) = m_lstData(ii).nDimming(2)
                        nData(3, nCnt) = m_lstData(ii).nDimming(3)
                        nData(4, nCnt) = m_lstData(ii).nDimming(4)
                        nCnt += 1
                    Else
                        ' ���ɓ���擾�ʒu�ŃZ�b�g�L��
                        nData(0, nCk) = m_lstData(ii).nDimming(0)
                        nData(1, nCk) = m_lstData(ii).nDimming(1)
                        nData(2, nCk) = m_lstData(ii).nDimming(2)
                        nData(3, nCk) = m_lstData(ii).nDimming(3)
                        nData(4, nCk) = m_lstData(ii).nDimming(4)
                    End If
                Next

        End Select

        typDsp.nCnt = nCnt
        typDsp.nCoilPos = nPos
        typDsp.nData = nData

    End Sub

    ''' <summary>
    ''' �ŏ��A�ő�A���σf�[�^�\��
    ''' </summary>
    ''' <param name="typDsp"></param>
    Private Sub DspCalcData(ByVal typDsp As TYP_TREND_DSP)

        With typDsp.dgvCalc

            If Not m_dctCalc.ContainsKey(typDsp.emType) Then
                .Visible = False
                Exit Sub
            End If

            Dim calc As TYP_TREND_CALC = m_dctCalc(typDsp.emType)

            .Item(EM_COL_CALC.MIN_DATA, 0).Value = calc.min
            .Item(EM_COL_CALC.MAX_DATA, 0).Value = calc.max
            .Item(EM_COL_CALC.AVG_DATA, 0).Value = calc.avg
            .Visible = True

        End With

    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbName_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim cmbData As ComboBox = CType(sender, ComboBox)

        If Not m_bSetEnd Then
            Exit Sub
        End If

        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                If cmbData IsNot m_typTrendDsp(ii).cmbName Then
                    Continue For
                End If

                PutTrendDsp(m_typTrendDsp(ii))

                m_typTrendDsp(ii).emType = CType(cmbData.SelectedValue, EM_COIL_TREND)

                DspInit(m_typTrendDsp(ii))

                SetDsp(m_typTrendDsp(ii))

            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�R���{�{�b�N�X ��O[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' CSV�o��
    ''' </summary>
    ''' <param name="strFolder">�ۑ��t�@�C���p�X</param>
    ''' <param name="strName">�ۑ��t�@�C����</param>
    ''' <remarks></remarks>
    Private Sub CsvOutTrand(ByVal strFolder As String, ByVal strName As String)

        Dim strFPath As String                          ' �t���p�X
        Dim strMsg As String = ""                       ' �ʒm���b�Z�[�W
        Dim strData As String = ""                      ' 1�s�̃f�[�^
        Dim nn As Integer                               ' INDEX
        Dim nCntMax As Integer = 0                      ' �f�[�^����MAX

        '' �t���p�X
        strFPath = IO.Path.Combine(strFolder, strName)

        '' �t�H���_�̗L���`�F�b�N(�����������낤)
        tMod.CheckFolder(strFolder)

        '' CSV�o��
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' �I�[�v��
                cls_Stram.Open()

                strData = ""
                '�w�b�_�[�o��(�g�����h���j
                For ii As Integer = 0 To m_typTrendDsp.Count - 1
                    strData += m_typTrendDsp(ii).cmbName.Text & ","
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                ' �Ō��","��r��
                strData = strData.Remove(strData.Length - 2, 2)

                ' �b�r�u�֏�������
                cls_Stram.WriteLine(strData)

                strData = ""
                '�w�b�_�[�o��(�}�ᖼ�j
                For ii As Integer = 0 To m_typTrendDsp.Count - 1
                    strData += "�擾�ʒu,"
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += m_typTrendDsp(ii).dgvHanrei(em_DataColumnType.iName, jj).Value.ToString
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                ' �Ō��","��r��
                strData = strData.Remove(strData.Length - 2, 2)

                ' �b�r�u�֏�������
                cls_Stram.WriteLine(strData)

                ' �f�[�^�����̍ő�l�擾
                For ii As Integer = 0 To m_typTrendDsp.Count - 1
                    If nCntMax < m_typTrendDsp(ii).nCnt Then
                        nCntMax = m_typTrendDsp(ii).nCnt
                    End If
                Next ii

                '�f�[�^�o��
                nn = 0
                While nCntMax > nn
                    strData = ""
                    For ii As Integer = 0 To m_typTrendDsp.Count - 1
                        ' �f�[�^�o�͌������f�[�^�ő吔�ȉ��H
                        If m_typTrendDsp(ii).nCnt > nn Then
                            ' �f�[�^�̐ݒ�
                            strData += CStr(m_typTrendDsp(ii).nCoilPos(nn))
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += CStr(m_typTrendDsp(ii).nData(jj, nn))
                                strData += ","
                            Next jj
                            strData += ","
                        Else
                            ' �󔒂̐ݒ�
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += ","
                            Next jj
                            strData += ","
                        End If

                    Next ii
                    '�Ō��","��r��
                    strData = strData.Remove(strData.Length - 2, 2)

                    ' �b�r�u�֏�������
                    cls_Stram.WriteLine(strData)

                    nn += 1
                    ' 
                End While

                ' �N���[�Y
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("CSV�o�� ��O[{0}]", ex.Message))
                strMsg = "CSV�o�͂Ɏ��s���܂����B"
            End Try
        End Using

        '''' �K�C�_���X�\��
        Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
            frmMsg.ShowDialog()
        End Using

    End Sub


    ''' <summary>
    ''' �A�b�v�_�E�� �R���g���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudGraphMaxMin_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim nud As NumericUpDown = CType(sender, NumericUpDown)

        If Not m_bSetEnd Then
            Exit Sub
        End If

        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                If nud IsNot m_typTrendDsp(ii).nudGraphMax AndAlso nud IsNot m_typTrendDsp(ii).nudGraphMin Then
                    Continue For
                End If

                ' ���̓f�[�^�`�F�b�N
                If False = InputDataCheck(m_typTrendDsp(ii)) Then
                    Exit Try
                End If

                SetDsp(m_typTrendDsp(ii))
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�A�b�v�_�E�� �R���g���[�� ��O[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �R���{�{�b�N�X�����ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="val"></param>
    ''' <remarks></remarks>
    Private Sub cmbName_Init(ByVal sender As ComboBox, ByVal val As EM_COIL_TREND)

        Dim ds As New DataTable()
        ds.Columns.Add("NAME", GetType(String))
        ds.Columns.Add("VALUE", GetType(EM_COIL_TREND))

        '�g�����h���ɓo�^����Ă���O���[�v��
        Dim keys As EM_COIL_TREND() = New EM_COIL_TREND(m_dctTrendInf.Keys.Count - 1) {}
        m_dctTrendInf.Keys.CopyTo(keys, 0)
        For ii As Integer = 0 To keys.Count - 1
            Dim typ As TYP_TREND_INF = m_dctTrendInf(keys(ii))
            Dim row As DataRow = ds.NewRow()
            row("NAME") = typ.strGroupName
            row("VALUE") = keys(ii)
            ds.Rows.Add(row)
        Next

        ds.AcceptChanges()
        sender.Items.Clear()
        sender.DataSource = ds
        sender.DisplayMember = "NAME"
        sender.ValueMember = "VALUE"
        sender.DropDownStyle = ComboBoxStyle.DropDownList
        sender.SelectedValue = val

    End Sub


    ''///////////////////////////////////////////////////////////////////////////
    ''�f�[�^�O���b�h�r���[
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �O���b�h������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvCoilInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvCoil

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                                    ' �I��F����

        Try
            With dgv
                .ScrollBars = ScrollBars.Horizontal                             ' �X�N���[���o�[(�����̂�)
                .DoubleBuffered = True

                .ReadOnly = True                                                ' �ǂݎ���p
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
                .ColumnHeadersVisible = True                                   ' ��w�b�_�\������

                '' �I���Z���F��`
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .ColumnHeadersHeight = 20                                       ' ��w�b�_��������

                For col As EM_COL_COLINF = EM_COL_COLINF.ORDER_NO To EM_COL_COLINF.MAX_COL
                    Select Case col
                        Case EM_COL_COLINF.ORDER_NO
                            ' ����No
                            dgvColSetText(dgv, "����No", 78, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.COIL_LEN
                            ' �R�C����
                            dgvColSetText(dgv, "�R�C����", 78, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.KANRI_NO
                            ' �Ǘ�No
                            dgvColSetText(dgv, "�Ǘ�No", 240, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)



                        Case EM_COL_COLINF.COIL_NO
                            ' ���Y�ԍ�
                            dgvColSetText(dgv, "���Y�ԍ�", 206, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.TYPE_DIV
                            ' �i��
                            dgvColSetText(dgv, "�i��", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.MATERIAL
                            ' �ގ�
                            dgvColSetText(dgv, "�ގ�", 38, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.NEXT_STEP
                            ' ���H��
                            dgvColSetText(dgv, "���H��", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.KENSA_REF
                            ' �����
                            dgvColSetText(dgv, "�����", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
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
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' �R�C�����\��
    ''' </summary>
    Private Sub DspCoil()
        Try
            With dgvCoil
                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF.ORDER_NO         ' ����No
                            .Item(ii, 0).Value = m_typCoilInf.strOrderNo
                        Case EM_COL_COLINF.COIL_LEN         ' �R�C����
                            .Item(ii, 0).Value = Format(m_typCoilInf.nCoilLen / 1000, "0")
                        Case EM_COL_COLINF.KANRI_NO         ' �Ǘ�No
                            .Item(ii, 0).Value = m_typCoilInf.strKanriNo
                        Case EM_COL_COLINF.COIL_NO            ' ���Y�ԍ�
                            .Item(ii, 0).Value = m_typCoilInf.strCoilNo
                        Case EM_COL_COLINF.TYPE_DIV           ' �i��
                            .Item(ii, 0).Value = m_typCoilInf.strHinCode
                        Case EM_COL_COLINF.MATERIAL           ' �ގ�
                            .Item(ii, 0).Value = m_typCoilInf.strSyuZai
                        Case EM_COL_COLINF.NEXT_STEP          ' ���H��
                            .Item(ii, 0).Value = m_typCoilInf.strNextCode
                        Case EM_COL_COLINF.KENSA_REF          ' �����
                            .Item(ii, 0).Value = m_typCoilInf.strKenKijun(0) '�����_�\
                        Case EM_COL_COLINF.SIZE
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThick As String = Format(m_typCoilInf.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = m_typCoilInf.nCoilWidth.ToString
                            .Item(ii, 0).Value = String.Format("{0}�~{1,4}", sThick, sWidth)
                        Case EM_COL_COLINF.ORDER
                            ''�T�C�Y(��[��m��mm]�~��[mm])
                            Dim sThickSei As String = Format(m_typCoilInf.nCoilAtuSei / 1000, "0.000")
                            Dim sWidthSei As String = m_typCoilInf.nCoilWidSei.ToString
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
    ''' �ő�A�ŏ��A���σf�[�^�\��DGV�C�j�V����
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvCalcInit(ByVal dgv As tCnt.tCnt_DataGridView)


        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        Try
            With dgv
                .ScrollBars = ScrollBars.None                                   ' �X�N���[���o�[�Ȃ�
                .DoubleBuffered = True

                .ReadOnly = True                                                ' �ǂݎ���p
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
                .ColumnHeadersVisible = False                                   ' ��w�b�_�\���Ȃ�

                '' �I���Z���F��`
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
                .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True

                For col As EM_COL_CALC = EM_COL_CALC.MAX_NAME To EM_COL_CALC.MAX_COL
                    Select Case col
                        Case EM_COL_CALC.MAX_NAME, EM_COL_CALC.MIN_NAME, EM_COL_CALC.AVG_NAME
                            dgvColSetText(dgv, "", 58, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_CALC.MAX_DATA, EM_COL_CALC.MIN_DATA, EM_COL_CALC.AVG_DATA
                            dgvColSetText(dgv, "", 88, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height
                .Item(EM_COL_CALC.MAX_NAME, 0).Value = "�ő�l"
                .Item(EM_COL_CALC.MAX_NAME, 0).Style.BackColor = g_ColorControl
                .Item(EM_COL_CALC.MIN_NAME, 0).Value = "�ŏ��l"
                .Item(EM_COL_CALC.MIN_NAME, 0).Style.BackColor = g_ColorControl
                .Item(EM_COL_CALC.AVG_NAME, 0).Value = "���ϒl"
                .Item(EM_COL_CALC.AVG_NAME, 0).Style.BackColor = g_ColorControl

            End With
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' ���T�C�Y�ɂ��\���ύX
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvWidthResize(ByVal dgv As tCnt.tCnt_DataGridView)
        Dim hsc As HScrollBar

        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            dgv.ColumnHeadersHeight = 16
            '�����X�N���[���o�[�\��
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight - hsc.Height
        Else
            dgv.ColumnHeadersHeight = 20
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight
        End If

        For ii As Integer = 0 To dgv.ColumnCount - 1
            If hsc.Visible Then
                '�����X�N���[���o�[�\��
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End If
        Next

    End Sub

    ''///////////////////////////////////////////////////////////////////////////
    ''�C�x���g�n���h���֐�
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���T�C�Y
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        dgvWidthResize(dgvCoil)
    End Sub
End Class
