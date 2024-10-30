'*******************************************************************************
'   �r�摜�g��\��
'
'  [Ver]
'       Ver.01  2017/09/08  ����
'
'	[����]
'*******************************************************************************
Option Strict On

Imports System.Reflection

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tMod
Public Class frmDefectImage
    '///////////////////////////////////////////////////////////////////////////
    '�񋓑�
    '///////////////////////////////////////////////////////////////////////////
    Public Enum EM_RATE_LIST
        emRate_XY                       '�S��
        emRate_X                        '�S��
        emRate_Y                        '�S��
        emRate_025                      '1/4�{
        emRate_050                      '1/2�{
        emRate_100                      '1�{
        emRate_150                      '1.5�{
        emRate_200                      '2�{
        emRate_300                      '3�{
        emRate_400                      '4�{
        RATE_END
    End Enum

    ''' <summary>
    ''' 
    ''' </summary>
    Private Enum EM_ROW_DEFECTINF
        TYPE = 0                        ' �r�햼
        GRADE                           ' �O���[�h��
        FPOS                            ' �R�C����[����̋���
        DS                              ' DS����̋���
        WS                              ' WS����̋���
        LENGTH                          ' �r����
        WIDTH                           ' �r��
        NO                              ' �rNo
        TORB                            ' �\���敪
        CYCLE_DIV                       ' �����r�敪
        DSWS                            ' DSWS�敪
        CAMSET                          ' �J�����Z�b�g
        CDS                             ' DS���Z���^�[����̋���[mm]
        CWS                             ' WS���Z���^�[����̋���[mm]
        SPEED                           ' ���x[mpm]
        PITCH                           ' �s�b�`[mm]
        ROOLKEI                         ' ���[���a[mm]
        ARIA                            ' �ʐ�[mm^2]
        H_RES                           ' �c����\[mm/pixel]
        W_RES                           ' ������\[mm/pixel]
        NFRAMENO                        ' �t���[��No
        FRM_X_MIN                       ' �t���[���O��Xmin[pixel]
        FRM_X_MAX                       ' �t���[���O��Xmax[pixel]
        FRM_Y_MIN                       ' �t���[���O��Ymin[pixel]
        FRM_Y_MAX                       ' �t���[���O��Ymax[pixel]
        CULL_FRM_X_MIN                  ' �Ԉ����t���[���O��Xmin[pixel]
        CULL_FRM_X_MAX                  ' �Ԉ����t���[���O��Xmax[pixel]
        CULL_FRM_Y_MIN                  ' �Ԉ����t���[���O��Ymin[pixel]
        CULL_FRM_Y_MAX                  ' �Ԉ����t���[���O��Ymax[pixel]
        BOX_X_MIN                       ' �O��Xmin[pixel]
        BOX_X_MAX                       ' �O��Xmax[pixel]
        BOX_Y_MIN                       ' �O��Ymin[pixel]
        BOX_Y_MAX                       ' �O��Ymax[pixel]
        BASE_BOX_X_MIN                  ' ���摜_�O��Xmin[pixel]
        BASE_BOX_X_MAX                  ' ���摜_�O��Xmax[pixel]
        BASE_BOX_Y_MIN                  ' ���摜_�O��Ymin[pixel]
        BASE_BOX_Y_MAX                  ' ���摜_�O��Ymax[pixel]
        BASE_CENTER_X                   ' ���摜_�r���S�ʒuX[pixel]
        BASE_CENTER_Y                   ' ���摜_�r���S�ʒuY[pixel]
        BASE_H_SIZE                     ' ���摜_�c��f��[pixel]
        BASE_W_SIZE                     ' ���摜_����f��[pixel]
        BASE_H_RES                      ' ���摜_�c����\[mm/pixel]
        BASE_W_RES                      ' ���摜_������\[mm/pixel]
        SHRINK_RATIO                    ' �k���� (1, 2, 4)
        CENTER_X                        ' �r���S�ʒuX[pixel]
        CENTER_Y                        ' �r���S�ʒuY[pixel]
        H_SIZE                          ' �c��f��[pixel]
        W_SIZE                          ' ����f��[pixel]
        TYUU                            ' �r��D��x
        GYUU                            ' �O���[�h�D��x
        WIDABSPOS                       ' ��������Έʒu
        WIDDIV                          ' �������敪
        NOBJTANSHO                      ' �T����ʑΏ��r
        OBJDEFLIST                      ' �d���׈ꗗ��ʑΏ��r
        MODEDEFLIST                     ' �d���׈ꗗ�����\���Ώ��r
        HASSEIDATE                      ' �r�������t
        MAX_ROW
    End Enum

    Private Enum EM_COL_DEFECTINF
        TYPE = 0                        ' �r�햼
        GRADE                           ' �O���[�h��
        FPOS                            ' �R�C����[����̋���
        MAX_COL
    End Enum

    Private Structure CMB_RATE_VALUE
        Dim emId As EM_RATE_LIST        ' ID
        Dim dValue As Double            ' �{���W��
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass_LogManager                           ' ���O�Ǘ�
    Private mcls_Param As clsParamManager                           ' �p�����[�^�Ǘ�

    Private m_strSection As String                                  ' ini�t�@�C���Z�N�V������

    Private m_bOverlay As Boolean                                   ' �I�[�o�[���C�̕\���L�� (True:�\�� False:��\��)
    Private m_nClickPos As Point                                    ' ���N���b�N�I���ʒu
    Private m_nDispTop As Integer                                   ' �\���ʒu(��[)
    Private m_nDispLeft As Integer                                  ' �\���ʒu(���[)
    Private m_nDispHeight As Integer                                ' �\���c��
    Private m_nDispWidth As Integer                                 ' �\������
    Private m_nZoomRate As Integer                                  ' �摜�g��{��

    Private m_emSide As EM_MAP_SIDE                                 ' �r�摜���x��DS�ʒu[NORMAL:����DS�AREVERSE:����WS]
    Private m_colLine As Color
    Private m_nDispImgNo As Integer = 0                             ' �\���摜��[1=���r,2=���X�r]
    Private m_DefectData As DETECT_BASE_DATA                        ' �r���

    Private m_ucDefectImg() As ucDefectImage

    '///////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���摜��
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property DispImgNo() As Integer
        Get
            Return m_nDispImgNo
        End Get
        Set(ByVal value As Integer)
            m_nDispImgNo = value
        End Set
    End Property

    ''' <summary>
    ''' �摜�g��{��[%]
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property ZoomRate() As Integer
        Get
            Return m_nZoomRate
        End Get
        Set(ByVal value As Integer)
            m_nZoomRate = value
        End Set
    End Property

    ''' <summary>
    ''' �I�[�o�[���C�\���L���ݒ�
    ''' </summary>
    ''' <param name="bVisible">True:�\�� False:��\��</param>
    ''' <remarks></remarks>
    Public Sub SetVisibleOverlay(ByVal bVisible As Boolean)
        Try
            m_bOverlay = bVisible
            For Each uc As ucDefectImage In m_ucDefectImg
                uc.ImgDsp.OverLayDspChange(bVisible)
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �I�[�o�[���C�\���ݒ�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �������\���L���ݒ�
    ''' </summary>
    ''' <param name="bVisible">True:�\�� False:��\��</param>
    ''' <remarks></remarks>
    Public Sub SetVisibleMemory(ByVal bVisible As Boolean)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                Dim memory As tClass_ImageDsp.MEMORI_INF = uc.ImgDsp.tProp_MemoriInf
                memory.bIsDsp = bVisible
                uc.ImgDsp.tProp_MemoriInf = memory
                uc.Refresh()
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �������\���ݒ�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �㉺���]�L�����擾
    ''' </summary>
    ''' <value>NORMAL:���]�����AREVERSE:���]�L��</value>
    ''' <returns>NORMAL:���]�����AREVERSE:���]�L��</returns>
    ''' <remarks></remarks>
    Public Property IsTurnTB() As EM_IMAGE_TOP
        Get
            If m_ucDefectImg(0).ImgDsp.tProp_ImgKind_IsTurnTB Then
                Return EM_IMAGE_TOP.REVERSE
            Else
                Return EM_IMAGE_TOP.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_TOP)
            For Each uc As ucDefectImage In m_ucDefectImg
                If value = EM_IMAGE_TOP.REVERSE Then
                    uc.ImgDsp.tProp_ImgKind_IsTurnTB = True
                Else
                    uc.ImgDsp.tProp_ImgKind_IsTurnTB = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' ���E���]�L�����擾
    ''' </summary>
    ''' <value>NORMAL:���]�����AREVERSE:���]�L��</value>
    ''' <returns>NORMAL:���]�����AREVERSE:���]�L��</returns>
    ''' <remarks></remarks>
    Public Property IsTurnLR() As EM_IMAGE_SIDE
        Get
            If m_ucDefectImg(0).ImgDsp.tProp_ImgKind_IsTurnLR Then
                Return EM_IMAGE_SIDE.REVERSE
            Else
                Return EM_IMAGE_SIDE.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_SIDE)
            For Each uc As ucDefectImage In m_ucDefectImg
                If value = EM_IMAGE_SIDE.REVERSE Then
                    uc.ImgDsp.tProp_ImgKind_IsTurnLR = True
                Else
                    uc.ImgDsp.tProp_ImgKind_IsTurnLR = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' �摜��90�x��]���擾
    ''' </summary>
    ''' <value>NORMAL:���]�����AREVERSE:���]�L��</value>
    ''' <returns>NORMAL:���]�����AREVERSE:���]�L��</returns>
    ''' <remarks></remarks>
    Public Property IsRotate90() As EM_IMAGE_90
        Get
            If m_ucDefectImg(0).ImgDsp.tProp_ImgKind_Rotate90 Then
                Return EM_IMAGE_90.LOTATE
            Else
                Return EM_IMAGE_90.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_90)
            For Each uc As ucDefectImage In m_ucDefectImg
                If value = EM_IMAGE_90.LOTATE Then
                    uc.ImgDsp.tProp_ImgKind_Rotate90 = True
                Else
                    uc.ImgDsp.tProp_ImgKind_Rotate90 = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' �r�摜���x��DS�ʒu
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
            For Each uc As ucDefectImage In m_ucDefectImg
                If m_emSide = EM_MAP_SIDE.REVERSE Then           '����DS
                    uc.SideR = DIRECTION_NAME_DS
                    uc.SideL = DIRECTION_NAME_WS
                Else                                            '����WS
                    uc.SideL = DIRECTION_NAME_DS
                    uc.SideR = DIRECTION_NAME_WS
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' �\���ʒu(��[)
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispTop() As Integer
        Set(ByVal value As Integer)
            m_nDispTop = value
        End Set
    End Property

    ''' <summary>
    ''' �\���ʒu(���[)
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispLeft() As Integer
        Set(ByVal value As Integer)
            m_nDispLeft = value
        End Set
    End Property

    ''' <summary>
    ''' �\���c��
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispHeight() As Integer
        Set(ByVal value As Integer)
            m_nDispHeight = value
        End Set
    End Property

    ''' <summary>
    ''' �\������
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispWidth() As Integer
        Set(ByVal value As Integer)
            m_nDispWidth = value
        End Set
    End Property

    ''' <summary>
    ''' �I�[�o�[���C�A�������F
    ''' </summary>
    Public WriteOnly Property SetColor As Color
        Set(value As Color)
            m_colLine = value
        End Set
    End Property
    '---------------------------------------------------------------------------
    ' ���\�b�h
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="clsLog">���O�N���X</param>
    ''' <param name="param">�p�����[�^�N���X</param>
    ''' <param name="strDefPath">�f�t�H���g�摜�p�X</param>
    ''' <param name="strSection">�Z�N�V������</param>
    ''' <param name="nPicNum">�\���摜��</param>
    ''' <param name="bPicDirect">�摜���я�(false:�ʏ�Atrue�F�t�j</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClass_LogManager, ByRef param As clsParamManager, ByVal strDefPath As String, ByVal strSection As String, ByVal nPicNum As UShort, ByVal bPicDirect As Boolean)
        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Me.Text = String.Format("[{0}] �r�g��摜���", SYSTEM_TITLE)

        mcls_Log = clsLog
        mcls_Param = param

        m_strSection = strSection
        m_nClickPos = Point.Empty

        m_nZoomRate = 100

        ' �g���ʃ��T�C�Y�ŏ��ݒ�
        Dim s As Size
        s.Width = 400
        s.Height = 400
        Me.MinimumSize = s

        Dim nNum As Integer = 1
        ' �e�[�u�����C�A�E�g�̗��ǉ�        
        For ii As Integer = 1 To nPicNum - 1
            tlpImage.ColumnStyles.Add(New ColumnStyle())
            tlpImage.ColumnCount = tlpImage.ColumnCount + 1
        Next
        ' �񕝂𓙕���
        Dim c As ColumnStyle
        For Each c In tlpImage.ColumnStyles
            c.SizeType = SizeType.Percent
            c.Width = CSng(100 / tlpImage.ColumnCount)
        Next
        nNum = tlpImage.ColumnCount

        ReDim m_ucDefectImg(nNum - 1)

        For ii As Integer = 0 To m_ucDefectImg.Length - 1
            m_ucDefectImg(ii) = New ucDefectImage(clsLog, strDefPath, True)
            Dim pos As Integer = ii
            If bPicDirect Then
                '�t�����ɉ摜��\������
                pos = m_ucDefectImg.Length - (ii + 1)
            End If

            tlpImage.Controls.Add(m_ucDefectImg(ii), pos, 0)

            With m_ucDefectImg(ii)
                Dim location As Point
                Dim margin As Padding
                location.X = 0
                location.Y = 0
                margin.All = 0
                .Location = location
                .Margin = margin
                .Anchor = AnchorStyles.Top Or AnchorStyles.Bottom Or AnchorStyles.Left Or AnchorStyles.Right
                AddHandler .picImg.MouseDoubleClick, AddressOf picImage_MouseDoubleClick
                AddHandler .picImg.MouseWheel, AddressOf picImage_MouseWheel
                AddHandler .picImg.MouseMove, AddressOf picImage_MouseMove
                AddHandler .picImg.MouseDown, AddressOf picImage_MouseDown
            End With
        Next

        AddHandler vsbImg.ValueChanged, AddressOf vsbImg_ValueChanged
        AddHandler hsbImg.ValueChanged, AddressOf hsbImg_ValueChanged

        Me.StartPosition = FormStartPosition.Manual

        cmbPicRate_Init()
        AddHandler cmbPicRate.SelectedIndexChanged, AddressOf cmbPicRate_SelectedIndexChanged

        cbDetail.Checked = False
        AddHandler cbDetail.CheckedChanged, AddressOf cbDetail_CheckedChanged

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)

        InitdgvDefInf(dgvKizuInfo)
        InitdgvCoilInf2(dgvKizuInfo2)
        AddHandler tlpTitle.Resize, AddressOf title_ReSize

    End Sub

    ''' <summary>
    ''' ���[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDetectImage_Load(sender As System.Object, e As System.EventArgs) Handles MyBase.Load
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                .tProp_AriaInf_Aria = uc.picImg.ClientSize
                .Calculation()
            End With
        Next

        ' �r���\��
        Dim hsc As HScrollBar
        Dim dgv As tCnt.tCnt_DataGridView

        dgv = dgvKizuInfo2
        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            '�����X�N���[���o�[�\��
            dgv.ColumnHeadersVisible = False
        Else
            dgv.ColumnHeadersVisible = True
        End If

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �ŐV�̏����r����\������
    ''' </summary>
    ''' <param name="bVisible">�t�H�[���\��</param>
    ''' <remarks></remarks>
    Public Sub DspUpDate(ByVal bVisible As Boolean)

        If 0 >= m_DefectData.nKizuNo Then
            Exit Sub
        End If

        If Not Visible Then
            ReadPosition()
            SetLocationAndSize()
        End If

        ' �X�N���[���o�[������
        vsbImg.Value = 0
        hsbImg.Value = 0

        '''' �摜�\��
        '' �C���[�W�\���N���X�����Z�b�g
        For ii As Integer = 0 To m_ucDefectImg.Length - 1
            If m_ucDefectImg(ii).ImgDsp Is Nothing Then
                Continue For
            End If
            With m_ucDefectImg(ii)

                If bVisible Then
                    '' ��{���Z�b�g
                    If .ImgData IsNot Nothing Then
                        .ImgDsp.SetImgInf(.ImgData.Width, .ImgData.Height,
                                                  m_DefectData.w_res, m_DefectData.h_res,
                                                  m_DefectData.center_x, m_DefectData.center_y, 1, 1)

                        If .ImgData.Width <> m_DefectData.w_size Or .ImgData.Height <> m_DefectData.h_size Then
                            Log(LNO.WAR, String.Format("�摜�T�C�Y�s��v img[w={0} h={1}] Data[w={2} h={3}]",
                                                                                .ImgData.Width, .ImgData.Height,
                                                                                m_DefectData.w_size, m_DefectData.h_size))
                        End If

                        .ImgDsp.tProp_AriaInf_Aria = .picImg.ClientSize             ' �S�������T�C�Y������ �����OK
                        '' �I�[�o�[���C
                        Dim oly As tClass.tClass_ImageDsp.OVERLAY_INF = Nothing
                        oly.bIsDsp = m_bOverlay
                        oly.Tag = Nothing
                        oly.Color = m_colLine
                        oly.LineWidth = 2
                        oly.HoseiX = 10
                        oly.HoseiY = 10
                        oly.ovlPos1 = New Point(m_DefectData.box_x_min, m_DefectData.box_y_min)
                        oly.ovlPos2 = New Point(m_DefectData.box_x_max, m_DefectData.box_y_max)
                        .ImgDsp.ResetOverLayInf()
                        .ImgDsp.SetOverLayInf(oly)
                        '-----------------------------------------------------------
                        '�X�P�[���i�ڐ���j�ݒ�
                        '-----------------------------------------------------------
                        Dim memory As tClass_ImageDsp.MEMORI_INF = .ImgDsp.tProp_MemoriInf
                        memory.Color = m_colLine
                        .ImgDsp.tProp_MemoriInf = memory
                        ' �؂�o���摜�\��
                        .ImgDsp.SetImage(CType(.ImgData, Image))
                        .ImgDsp.tProp_Paint_IsDefImgDspn = False
                    Else
                        ' �~�摜
                        .ImgDsp.tProp_Paint_IsDefImgDspn = True
                    End If

                    .ImgDsp.tProp_Paint_IsPaint = True                                                 ' �摜�\����ON
                End If

                Rate_Change(.ImgDsp)
                .ImgDsp.tProp_AriaInf_IsCenter = True                                  ' �r���S�ʒu�ړ�
                .ImgDsp.Calculation()                                                  ' �\�������m��
                .ImgDsp.tProp_AriaInf_IsCenter = False                                 ' �r���S�ʒu�ړ�����
                .ImgDsp.Refresh()

            End With

        Next

        ' �X�N���[���o�[���f
        SetCommonScrollBar()
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                If vsbImg.Enabled Then
                    vsbImg.ValueProgramable = -1 * .tProp_AriaInf_Offset.Y   ' ���ݒl�ύX
                End If
                If hsbImg.Enabled Then
                    hsbImg.ValueProgramable = -1 * .tProp_AriaInf_Offset.X   ' ���ݒl�ύX
                End If
                Exit For
            End With
        Next

        If bVisible = True Then
            '��������߁A������Visible=True�ɂ���
            Me.Visible = True
            Me.Focus()
        End If

        'Me.Owner.Focus()

    End Sub

    ''' <summary>
    ''' �X�P�[���ݒ�
    ''' </summary>
    ''' <param name="nIntvalX">X����1�������P��[mm]</param>
    ''' <param name="nNumX">X���̃�������</param>
    ''' <param name="nIntvalY">Y����1�������P��[mm]</param>
    ''' <param name="nNumY">Y���̃�������</param>
    ''' <remarks></remarks>
    Public Sub SetMemoryConfig(ByVal nIntvalX As Integer, ByVal nNumX As Integer, ByVal nIntvalY As Integer, ByVal nNumY As Integer)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    Dim memo As tClass_ImageDsp.MEMORI_INF = .tProp_MemoriInf
                    memo.nIntervalX = nIntvalX
                    memo.nIntervalY = nIntvalY
                    memo.nLoopCntX = nNumX
                    memo.nLoopCntY = nNumY
                    .tProp_MemoriInf = memo
                End With
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�摜�\�� �������ݒ�ُ�[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �؂�o���摜��� �Z�b�g
    ''' </summary>
    ''' <param name="img"></param>
    ''' <remarks></remarks>
    Public Sub SetDefectData(ByVal data As DETECT_BASE_DATA, ByVal img() As Image, ByVal title() As String)

        Try

            m_DefectData = data
            SetDgvData()

            For ii As Integer = 0 To m_ucDefectImg.Length - 1
                If img.Length > ii Then
                    m_ucDefectImg(ii).ImgData = img(ii)
                End If
                If title.Length > ii Then
                    m_ucDefectImg(ii).Title = title(ii)
                End If
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�摜�\�� �������ݒ�ُ�[{0}]", ex.Message))
        End Try

    End Sub

    '***************************************************************************
    ' ���T�C�Y�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' ���T�C�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picImg_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlpImage.Resize
        If m_ucDefectImg Is Nothing Then Exit Sub

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                .tProp_AriaInf_Aria = uc.picImg.ClientSize
                .Calculation()
            End With
        Next

        DspUpDate(False)

    End Sub

    ''' <summary>
    ''' ���T�C�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Public Sub title_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)

        ' �r���\��
        Dim hsc As HScrollBar
        Dim dgv As tCnt.tCnt_DataGridView

        dgv = dgvKizuInfo2
        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            '�����X�N���[���o�[�\��
            dgv.ColumnHeadersVisible = False
        Else
            dgv.ColumnHeadersVisible = True
        End If

    End Sub

    '***************************************************************************
    ' �X�N���[���C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �c�X�N���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub vsbImg_ValueChanged(ByVal sender As Object, ByVal e As Boolean)
        If e = True Then Return ' �v���O�������ŕύX�����ꍇ�́A������

        Dim val As Integer = -1 * vsbImg.Value

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                If .tProp_AriaInf_OffsetY = val Then Exit Sub
                .tProp_AriaInf_OffsetY = val
                .Calculation()
                .Refresh()
            End With
        Next

    End Sub

    ''' <summary>
    ''' ���X�N���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub hsbImg_ValueChanged(ByVal sender As Object, ByVal e As Boolean)
        If e = True Then Return ' �v���O�������ŕύX�����ꍇ�́A������

        Dim val As Integer = -1 * hsbImg.Value

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                If .tProp_AriaInf_OffsetX = val Then Exit Sub
                .tProp_AriaInf_OffsetX = val
                .Calculation()
                .Refresh()
            End With
        Next

    End Sub

    ''' <summary>
    ''' �X�N���[���o�[ �ő�T�C�Y�ύX
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetCommonScrollBar()

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                ' �c�X�N���[���o�[ �L����
                If 0 >= .tProp_CalcAns.ImgNonAriaSize.Height Then
                    vsbImg.Enabled = False
                    vsbImg.Maximum = 0
                Else
                    vsbImg.Value = 0
                    vsbImg.Maximum = .tProp_CalcAns.ImgDspRect.Height - 1
                    vsbImg.Minimum = 0
                    vsbImg.LargeChange = .tProp_CalcAns.ImgDspRect.Height - .tProp_CalcAns.ImgNonAriaSize.Height
                    vsbImg.Enabled = True
                End If

                ' ���X�N���[���o�[ �L����
                If 0 >= .tProp_CalcAns.ImgNonAriaSize.Width Then
                    hsbImg.Enabled = False
                    hsbImg.Maximum = 0
                Else
                    hsbImg.Value = 0
                    hsbImg.Maximum = .tProp_CalcAns.ImgDspRect.Width - 1
                    hsbImg.Minimum = 0
                    hsbImg.LargeChange = .tProp_CalcAns.ImgDspRect.Width - .tProp_CalcAns.ImgNonAriaSize.Width
                    hsbImg.Enabled = True
                End If
                Exit For
            End With
        Next

    End Sub

    ''' <summary>
    ''' �c�X�N���[���l�ݒ�
    ''' </summary>
    ''' <param name="value">�X�N���[���l</param>
    ''' <remarks></remarks>
    Private Sub SetVerticalScroll(ByVal value As Integer)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    .tProp_AriaInf_OffsetY = -1 * value
                End With
            Next
            vsbImg.ValueProgramable = value

        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �c�X�N���[���l�ݒ�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ���X�N���[���l�ݒ�
    ''' </summary>
    ''' <param name="value">�X�N���[���l</param>
    ''' <remarks></remarks>
    Private Sub SetHorizontalScroll(ByVal value As Integer)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    .tProp_AriaInf_OffsetX = -1 * value
                End With
            Next
            hsbImg.ValueProgramable = value
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 ���X�N���[���l�ݒ�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' �{���R���{�{�b�N�X�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �{���R���{�{�b�N�X�C�j�V����
    ''' </summary>
    Private Sub cmbPicRate_Init()
        Dim nSelIndex As Integer = 0

        Dim picTable As New DataTable()
        picTable.Columns.Add("NAME", GetType(String))
        picTable.Columns.Add("VALUE", GetType(CMB_RATE_VALUE))

        For emId As EM_RATE_LIST = EM_RATE_LIST.emRate_XY To EM_RATE_LIST.RATE_END
            Dim strName As String = ""
            Dim dValue As Double = 0
            Select Case emId
                'Case EM_RATE_LIST.emRate_XY
                '    strName = "�S��"
                '    dValue = -1
                Case EM_RATE_LIST.emRate_X
                    strName = "�S��"
                    dValue = -1
                Case EM_RATE_LIST.emRate_Y
                    strName = "�S��"
                    dValue = -1
                Case EM_RATE_LIST.emRate_025
                    strName = "1/4�{"
                    dValue = 0.25
                Case EM_RATE_LIST.emRate_050
                    strName = "1/2�{"
                    dValue = 0.5
                Case EM_RATE_LIST.emRate_100
                    strName = "�P�{"
                    dValue = 1
                    nSelIndex = picTable.Rows.Count     ' �f�t�H���g�Ŏw�肷��{��
                'Case EM_RATE_LIST.emRate_150
                '    strName = "1.5�{"
                '    dValue = 1.5
                Case EM_RATE_LIST.emRate_200
                    strName = "�Q�{"
                    dValue = 2
                'Case EM_RATE_LIST.emRate_300
                '    strName = "�R�{"
                '    dValue = 3
                Case EM_RATE_LIST.emRate_400
                    strName = "�S�{"
                    dValue = 4
                Case Else
                    Continue For
            End Select
            Dim typ As New CMB_RATE_VALUE
            typ.emId = emId
            typ.dValue = dValue
            Dim row As DataRow = picTable.NewRow()
            row("NAME") = strName
            row("VALUE") = typ
            picTable.Rows.Add(row)
        Next

        picTable.AcceptChanges()
        cmbPicRate.Items.Clear()
        cmbPicRate.DataSource = picTable
        cmbPicRate.DisplayMember = "NAME"
        cmbPicRate.ValueMember = "VALUE"
        cmbPicRate.SelectedIndex = nSelIndex
        AddHandler cmbPicRate.SelectedIndexChanged, AddressOf cmbPicRate_SelectedIndexChanged
    End Sub


    ''' <summary>
    ''' �\���{���R���{�{�b�N�X
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbPicRate_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' ���W�f�[�^�v�Z
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            Rate_Change(uc.ImgDsp)
            uc.ImgDsp.Calculation()
        Next

        ' �X�N���[���o�[���f
        SetCommonScrollBar()

        ' �ĕ`��
        DspUpDate(False)

    End Sub

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' �I��
                    cmdEnd.PerformClick()
            End Select
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �t�@���N�V�����C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �t�H�[�����B��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub CloseFrom()
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                .tProp_Paint_IsPaint = False
            End With
        Next
        SavePosition()
        m_nDispImgNo = 0
        cbDetail.Checked = False        '�O��J���Ƃ��͏ڍׂ͕������
        ' �I��
        Me.Visible = False
    End Sub

    ''' <summary>
    ''' ����{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdEnd.Click
        CloseFrom()
    End Sub

    ''' <summary>
    ''' �r�摜���폜���܂�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DisposeImage()
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    Dim old_img As Image = .tProp_Paint_Img(0)
                    If old_img Is Nothing = False Then              ' �摜���̂Ă�
                        old_img.Dispose()
                    End If
                End With
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �摜�j���ُ� [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' �}�E�X�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �s�N�`���[�{�b�N�X�̃}�E�X�z�C�[���C�x���g
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Public Sub picImage_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            If vsbImg.Enabled Then
                Dim nValue As Integer = CInt(IIf(e.Delta > 0, 20, -20))             '�ω��ʂ��v�Z
                nValue = vsbImg.Value - nValue
                If nValue < 0 Then
                    nValue = 0
                End If
                If nValue > vsbImg.Maximum - vsbImg.LargeChange Then
                    nValue = vsbImg.Maximum - vsbImg.LargeChange
                End If

                SetVerticalScroll(nValue)
                For Each uc As ucDefectImage In m_ucDefectImg
                    uc.ImgDsp.Refresh()
                Next
            End If
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �}�E�X�z�C�[���C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �s�N�`���[�{�b�N�X�̃}�E�X�ړ��C�x���g
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Public Sub picImage_MouseMove(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            Dim nValue As Integer
            If e.Button = Windows.Forms.MouseButtons.Left Then                  '���{�^���h���b�O��
                If hsbImg.Enabled Then
                    Dim nX As Integer = e.X - m_nClickPos.X                          'X�������̃X�N���[���ʂ��v�Z
                    nValue = hsbImg.Value - nX
                    If 0 > nValue Then                                     '�X�N���[������ƍ��[�ɂԂ���
                        nValue = 0
                    ElseIf nValue > hsbImg.Maximum - hsbImg.LargeChange Then                '�X�N���[������ƉE�[�ɂԂ���
                        nValue = hsbImg.Maximum - hsbImg.LargeChange
                    End If
                    SetHorizontalScroll(nValue)
                End If
                If vsbImg.Enabled Then
                    Dim nY As Integer = e.Y - m_nClickPos.Y                          'Y�������̃X�N���[���ʂ��v�Z
                    nValue = vsbImg.Value - nY
                    If 0 > nValue Then                                     '�X�N���[������ƍ��[�ɂԂ���
                        nValue = 0
                    ElseIf nValue > vsbImg.Maximum - vsbImg.LargeChange Then                '�X�N���[������ƉE�[�ɂԂ���
                        nValue = vsbImg.Maximum - vsbImg.LargeChange
                    End If
                    SetVerticalScroll(nValue)
                End If

                m_nClickPos.X = e.X
                m_nClickPos.Y = e.Y

                For Each uc As ucDefectImage In m_ucDefectImg
                    uc.ImgDsp.Refresh()
                Next
            End If

        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �}�E�X�ړ��C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �s�N�`���[�{�b�N�X�̃}�E�X�N���b�N�C�x���g
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub picImage_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            If e.Button = Windows.Forms.MouseButtons.Left Then                  '���{�^�������ꏊ���L������
                m_nClickPos.X = e.X
                m_nClickPos.Y = e.Y
            End If
            CType(sender, PictureBox).Focus()
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �}�E�X�N���b�N�C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �s�N�`���[�{�b�N�X�̃_�u���N���b�N�C�x���g
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Public Sub picImage_MouseDoubleClick(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            If e.Button = Windows.Forms.MouseButtons.Left Then
                CloseFrom()
            End If
        Catch ex As Exception
            Log(LNO.ERR, String.Format("�g��摜 �_�u���N���b�N�C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub


    '***************************************************************************
    ' �\���ʒu �� �\���T�C�Y
    '***************************************************************************
    ''' <summary>
    ''' �\���ʒu �� �\���T�C�Y�ۑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SavePosition()
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_POS_TOP", LOCALINI_NAME, CStr(Me.Top))
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_POS_LEFT", LOCALINI_NAME, CStr(Me.Left))
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_WINDOW_HEIGHT", LOCALINI_NAME, CStr(Me.Height))
        Dim nWidth As Integer = Me.Width
        If cbDetail.Checked Then
            nWidth = nWidth - dgvKizuInfo.Width
        End If
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_WINDOW_WIDTH", LOCALINI_NAME, CStr(nWidth))
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_DISP_RATE", LOCALINI_NAME, CStr(cmbPicRate.SelectedIndex))

    End Sub

    ''' <summary>
    ''' �\���ʒu �� �\���T�C�Y�ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadPosition()
        Dim nDispTop As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_POS_TOP", LOCALINI_NAME, -9999)
        If nDispTop >= Owner.Top And nDispTop <= Owner.Bottom Then
            Me.DispTop = nDispTop
        Else
            Me.DispTop = -9999
        End If
        Dim nDispLeft As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_POS_LEFT", LOCALINI_NAME, -9999)
        If nDispLeft >= Owner.Left And nDispLeft <= Owner.Right Then
            Me.DispLeft = nDispLeft
        Else
            Me.DispLeft = -9999
        End If
        Dim nSelIndex As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_DISP_RATE", LOCALINI_NAME, -1)
        If 0 <= nSelIndex Then
            cmbPicRate.SelectedIndex = nSelIndex
        End If

        Me.DispHeight = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_WINDOW_HEIGHT", LOCALINI_NAME, 640)
        Dim nWidth As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_WINDOW_WIDTH", LOCALINI_NAME, 720)

        If cbDetail.Checked Then
            Me.DispWidth = nWidth + dgvKizuInfo.Width
        Else
            Me.DispWidth = nWidth
        End If


    End Sub

    ''' <summary>
    ''' �\���ʒu���T�C�Y�ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetLocationAndSize()
        '�\���T�C�Y
        If m_nDispWidth = -9999 Or m_nDispHeight = -9999 Then
            '���T�C�Y
        Else
            Me.Size = New Size(m_nDispWidth, m_nDispHeight)
        End If
        '�\���ʒu
        If m_nDispTop = -9999 Or m_nDispLeft = -9999 Then
            Me.Location = New Point(
                Me.Owner.Location.X + (Me.Owner.Width - Me.Width) \ 2,
                Me.Owner.Location.Y + (Me.Owner.Height - Me.Height) \ 2)
        Else
            Me.Location = New Point(m_nDispLeft, m_nDispTop)
        End If

    End Sub

    Private Sub Rate_Change(ByRef dsp As tClass_ImageDsp2)

        Dim emStrechMode As tClass_ImageDsp_Calc.EM_IMG_STRECH
        Dim dRate As Double
        Dim typ As CMB_RATE_VALUE = CType(cmbPicRate.SelectedValue, CMB_RATE_VALUE)

        With dsp
            Select Case typ.emId
                Case EM_RATE_LIST.emRate_XY
                    Dim d_sz_h As Double = m_DefectData.h_size * m_DefectData.h_res
                    Dim d_sz_w As Double = m_DefectData.w_size * m_DefectData.w_res
                    Dim n_area_height As Integer = .tProp_AriaInf.nAriaHeight
                    Dim n_area_width As Integer = .tProp_AriaInf.nAriaWidth
                    ' �摜�T�C�Y�̏c���䗦�ƃN���C�A���g�T�C�Y�̏c���䗦�̔�r
                    If d_sz_h / d_sz_w > n_area_height / n_area_width Then
                        emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechHeight    ' �c����ێ��̂�
                    Else
                        emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth     ' �c����ێ��̂�
                    End If
                    dRate = 1.0
                Case EM_RATE_LIST.emRate_X
                    dRate = 1.0
                    emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth         ' �c����ێ��̂�
                Case EM_RATE_LIST.emRate_Y
                    dRate = 1.0
                    emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechHeight        ' �c����ێ��̂�
                Case Else
                    dRate = typ.dValue * (m_nZoomRate / 100.0)  '�{��
                    emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRawWidth
            End Select

            '' �I�t�Z�b�g�l������
            .tProp_ImgKind_StrechType = emStrechMode
            .tProp_AriaInf_OffsetX = 0
            .tProp_AriaInf_OffsetY = 0
            .tProp_AriaInf_Zoom = New tClass.tClass_ImageDsp_Calc.RATE(dRate, dRate)
        End With


    End Sub

    ''' <summary>
    ''' �r��񗓕\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�r���f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Private Sub InitdgvDefInf(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .RowCount = EM_ROW_DEFECTINF.MAX_ROW

            '' �����`
            .ScrollBars = ScrollBars.Vertical
            .DoubleBuffered = True

            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
            .ColumnHeadersVisible = False                                   ' ��w�b�_�\���Ȃ�

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            .ColumnCount = 2                ' �}�b�v��
            '''' ���`
            '1���:���ږ�
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(0).Width = 58
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            '2���:�l
            .Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(1).Width = .ClientSize.Width - .Columns(0).Width - 3
            .Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable

            '�s��`�@����
            For ii As Integer = 0 To .Rows.Count - 1
                .Rows(ii).Height = 20   '�s�̍����Z�b�g
                .Item(0, ii).Style.BackColor = Color.LightGray
                Select Case ii
                    Case EM_ROW_DEFECTINF.TYPE
                        .Item(0, ii).Value = "�r�햼"
                    Case EM_ROW_DEFECTINF.GRADE
                        .Item(0, ii).Value = "�O���[�h"
					Case EM_ROW_DEFECTINF.FPOS
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
						'.Item(0, ii).Value = "F�`"
						.Item(0, ii).Value = "�o��"
						'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
					Case EM_ROW_DEFECTINF.DS
                        .Item(0, ii).Value = "DS�`"
                    Case EM_ROW_DEFECTINF.WS
                        .Item(0, ii).Value = "WS�`"
                    Case EM_ROW_DEFECTINF.LENGTH
                        .Item(0, ii).Value = "���ג�"
                    Case EM_ROW_DEFECTINF.WIDTH
                        .Item(0, ii).Value = "���ו�"
                    Case EM_ROW_DEFECTINF.NO
                        .Item(0, ii).Value = "�r�A��"
                    Case EM_ROW_DEFECTINF.TORB
                        .Item(0, ii).Value = "�\���敪"
                    Case EM_ROW_DEFECTINF.CYCLE_DIV
                        .Item(0, ii).Value = "�����r"
                    Case EM_ROW_DEFECTINF.DSWS
                        .Item(0, ii).Value = "DSWS"
                    Case EM_ROW_DEFECTINF.CAMSET
                        .Item(0, ii).Value = "��׾��"
                    Case EM_ROW_DEFECTINF.CDS
                        .Item(0, ii).Value = "C�`DS"
                    Case EM_ROW_DEFECTINF.CWS
                        .Item(0, ii).Value = "C�`WS"
                    Case EM_ROW_DEFECTINF.SPEED
                        .Item(0, ii).Value = "���x"
                    Case EM_ROW_DEFECTINF.PITCH
                        .Item(0, ii).Value = "�s�b�`"
                    Case EM_ROW_DEFECTINF.ROOLKEI
                        .Item(0, ii).Value = "���[���a"
                    Case EM_ROW_DEFECTINF.ARIA
                        .Item(0, ii).Value = "�ʐ�"
                    Case EM_ROW_DEFECTINF.H_RES
                        .Item(0, ii).Value = "�c����\"
                    Case EM_ROW_DEFECTINF.W_RES
                        .Item(0, ii).Value = "������\"
                    Case EM_ROW_DEFECTINF.NFRAMENO
                        .Item(0, ii).Value = "FNo"
                    Case EM_ROW_DEFECTINF.FRM_X_MIN
                        .Item(0, ii).Value = "FXmin"
                    Case EM_ROW_DEFECTINF.FRM_X_MAX
                        .Item(0, ii).Value = "FXmax"
                    Case EM_ROW_DEFECTINF.FRM_Y_MIN
                        .Item(0, ii).Value = "FYmin"
                    Case EM_ROW_DEFECTINF.FRM_Y_MAX
                        .Item(0, ii).Value = "FYmax"
                    Case EM_ROW_DEFECTINF.CULL_FRM_X_MIN
                        .Item(0, ii).Value = "CFXmin"
                    Case EM_ROW_DEFECTINF.CULL_FRM_X_MAX
                        .Item(0, ii).Value = "CFXmax"
                    Case EM_ROW_DEFECTINF.CULL_FRM_Y_MIN
                        .Item(0, ii).Value = "CFYmin"
                    Case EM_ROW_DEFECTINF.CULL_FRM_Y_MAX
                        .Item(0, ii).Value = "CFYmax"
                    Case EM_ROW_DEFECTINF.BOX_X_MIN
                        .Item(0, ii).Value = "Xmin"
                    Case EM_ROW_DEFECTINF.BOX_X_MAX
                        .Item(0, ii).Value = "Xmax"
                    Case EM_ROW_DEFECTINF.BOX_Y_MIN
                        .Item(0, ii).Value = "Ymin"
                    Case EM_ROW_DEFECTINF.BOX_Y_MAX
                        .Item(0, ii).Value = "Ymax"
                    Case EM_ROW_DEFECTINF.BASE_BOX_X_MIN
                        .Item(0, ii).Value = "��Xmin"
                    Case EM_ROW_DEFECTINF.BASE_BOX_X_MAX
                        .Item(0, ii).Value = "��Xmax"
                    Case EM_ROW_DEFECTINF.BASE_BOX_Y_MIN
                        .Item(0, ii).Value = "��Ymin"
                    Case EM_ROW_DEFECTINF.BASE_BOX_Y_MAX
                        .Item(0, ii).Value = "��Ymax"
                    Case EM_ROW_DEFECTINF.BASE_CENTER_X
                        .Item(0, ii).Value = "�����SX"
                    Case EM_ROW_DEFECTINF.BASE_CENTER_Y
                        .Item(0, ii).Value = "�����SY"
                    Case EM_ROW_DEFECTINF.BASE_H_SIZE
                        .Item(0, ii).Value = "���c�搔"
                    Case EM_ROW_DEFECTINF.BASE_W_SIZE
                        .Item(0, ii).Value = "�����搔"
                    Case EM_ROW_DEFECTINF.BASE_H_RES
                        .Item(0, ii).Value = "���c����"
                    Case EM_ROW_DEFECTINF.BASE_W_RES
                        .Item(0, ii).Value = "��������"
                    Case EM_ROW_DEFECTINF.SHRINK_RATIO
                        .Item(0, ii).Value = "�k����"
                    Case EM_ROW_DEFECTINF.CENTER_X
                        .Item(0, ii).Value = "�r���SX"
                    Case EM_ROW_DEFECTINF.CENTER_Y
                        .Item(0, ii).Value = "�r���SY"
                    Case EM_ROW_DEFECTINF.H_SIZE
                        .Item(0, ii).Value = "�c��f��"
                    Case EM_ROW_DEFECTINF.W_SIZE
                        .Item(0, ii).Value = "����f��"
                    Case EM_ROW_DEFECTINF.TYUU
                        .Item(0, ii).Value = "�r��D��"
                    Case EM_ROW_DEFECTINF.GYUU
                        .Item(0, ii).Value = "Gr�D��"
                    Case EM_ROW_DEFECTINF.WIDABSPOS
                        .Item(0, ii).Value = "�����"
                    Case EM_ROW_DEFECTINF.WIDDIV
                        .Item(0, ii).Value = "������"
                    Case EM_ROW_DEFECTINF.NOBJTANSHO
                        .Item(0, ii).Value = "�T���Ώ�"
                    Case EM_ROW_DEFECTINF.OBJDEFLIST
                        .Item(0, ii).Value = "�d���Ώ�"
                    Case EM_ROW_DEFECTINF.MODEDEFLIST
                        .Item(0, ii).Value = "�����Ώ�"
                    Case EM_ROW_DEFECTINF.HASSEIDATE
                        .Item(0, ii).Value = "����"
                End Select
            Next ii
        End With


    End Sub

    ''' <summary>
    ''' �����ޏ��\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�r�ꗗ�f�[�^�O���b�h</param>
    ''' <remarks></remarks>
    Public Sub InitdgvCoilInf2(ByVal dgv As tCnt.tCnt_DataGridView)
        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        Try
            With dgv
                .ScrollBars = ScrollBars.Horizontal                             ' �X�N���[���o�[(�����̂�)
                .DoubleBuffered = True

                .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
                .ReadOnly = True                                                ' �ǂݎ���p
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
                .ColumnHeadersVisible = True                                    ' ��w�b�_�\������
                .AllowUserToResizeColumns = True                                ' ��̃T�C�Y�ύX�L

                '' �I���Z���F��`
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .ColumnHeadersHeight = 16                                       ' ��w�b�_��������

                For col As EM_COL_DEFECTINF = EM_COL_DEFECTINF.TYPE To EM_COL_DEFECTINF.MAX_COL
                    Select Case col
                        Case EM_COL_DEFECTINF.TYPE
                            ' �r�햼
                            dgvColSetText(dgv, "�r�햼", 120, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_DEFECTINF.GRADE
                            ' �O���[�h
                            dgvColSetText(dgv, "�O���[�h", 70, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_DEFECTINF.FPOS
                            ' �R�C����[����̈ʒu
                            '// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
                            'dgvColSetText(dgv, "F�`", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
							dgvColSetText(dgv, "�o��", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
							'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight

            End With
        Catch ex As Exception
        End Try


    End Sub

    ''' <summary>
    ''' �r���Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetDgvData()
        Try
            '-------------------------------------------------------------------
            '�r����\��DGV�ɃZ�b�g����B
            '-------------------------------------------------------------------
            With dgvKizuInfo
                For ii As Integer = 0 To .RowCount - 1
                    Select Case ii
                        Case EM_ROW_DEFECTINF.NO
                            .Item(1, ii).Value = m_DefectData.nKizuNo.ToString
                        Case EM_ROW_DEFECTINF.TYPE
                            .Item(1, ii).Value = mcls_Param.GetTypeName(m_DefectData.nTid)
                        Case EM_ROW_DEFECTINF.GRADE
                            .Item(1, ii).Value = mcls_Param.GetGradeName(m_DefectData.nGid)
                            .Item(1, ii).Style.BackColor = mcls_Param.GetGradeColor(m_DefectData.nGid)
                            .Item(1, ii).Style.SelectionBackColor = .Item(1, ii).Style.BackColor
						Case EM_ROW_DEFECTINF.FPOS
							'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>							
							'.Item(1, ii).Value = Format(Int(m_DefectData.y / 100) / 10, "####0.0") 'F�\��
							.Item(1, ii).Value = Format(Int(m_DefectData.y_out / 100) / 10, "####0.0") 'F�\��
							'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
						Case EM_ROW_DEFECTINF.DS
                            .Item(1, ii).Value = m_DefectData.ds.ToString("0.0")
                        Case EM_ROW_DEFECTINF.WS
                            .Item(1, ii).Value = m_DefectData.ws.ToString("0.0")
                        Case EM_ROW_DEFECTINF.WIDTH
                            .Item(1, ii).Value = m_DefectData.kizu_wid.ToString
                        Case EM_ROW_DEFECTINF.LENGTH
                            .Item(1, ii).Value = m_DefectData.kizu_len.ToString
                        Case EM_ROW_DEFECTINF.TORB
                            .Item(1, ii).Value = m_DefectData.TorB.ToString
                        Case EM_ROW_DEFECTINF.CYCLE_DIV
                            .Item(1, ii).Value = m_DefectData.cycle_div.ToString
                        Case EM_ROW_DEFECTINF.DSWS
                            .Item(1, ii).Value = m_DefectData.dsws.ToString
                        Case EM_ROW_DEFECTINF.CAMSET
                            .Item(1, ii).Value = m_DefectData.camset.ToString
                        Case EM_ROW_DEFECTINF.CDS
                            .Item(1, ii).Value = m_DefectData.dsc.ToString("0.0")
                        Case EM_ROW_DEFECTINF.CWS
                            .Item(1, ii).Value = m_DefectData.wsc.ToString("0.0")
                        Case EM_ROW_DEFECTINF.SPEED
                            .Item(1, ii).Value = m_DefectData.speed.ToString
                        Case EM_ROW_DEFECTINF.PITCH
                            If 1 = m_DefectData.cycle_div Or 2 = m_DefectData.cycle_div Then    ' �������r�̏ꍇ
                                .Item(1, ii).Value = m_DefectData.pitch.ToString
                            Else
                                .Item(1, ii).Value = ""
                            End If
                        Case EM_ROW_DEFECTINF.ROOLKEI
                            If 1 = m_DefectData.cycle_div Or 2 = m_DefectData.cycle_div Then    ' �������r�̏ꍇ
                                .Item(1, ii).Value = m_DefectData.roolkei.ToString("####0")
                            Else
                                .Item(1, ii).Value = ""
                            End If
                        Case EM_ROW_DEFECTINF.ARIA
                            .Item(1, ii).Value = m_DefectData.aria.ToString("0.0")
                        Case EM_ROW_DEFECTINF.H_RES
                            .Item(1, ii).Value = m_DefectData.h_res.ToString
                        Case EM_ROW_DEFECTINF.W_RES
                            .Item(1, ii).Value = m_DefectData.w_res.ToString
                        Case EM_ROW_DEFECTINF.NFRAMENO
                            .Item(1, ii).Value = m_DefectData.nFrameNo.ToString
                        Case EM_ROW_DEFECTINF.FRM_X_MIN
                            .Item(1, ii).Value = m_DefectData.frm_x_min.ToString
                        Case EM_ROW_DEFECTINF.FRM_X_MAX
                            .Item(1, ii).Value = m_DefectData.frm_x_max.ToString
                        Case EM_ROW_DEFECTINF.FRM_Y_MIN
                            .Item(1, ii).Value = m_DefectData.frm_y_min.ToString
                        Case EM_ROW_DEFECTINF.FRM_Y_MAX
                            .Item(1, ii).Value = m_DefectData.frm_y_max.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_X_MIN
                            .Item(1, ii).Value = m_DefectData.cull_frm_x_min.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_X_MAX
                            .Item(1, ii).Value = m_DefectData.cull_frm_x_max.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_Y_MIN
                            .Item(1, ii).Value = m_DefectData.cull_frm_y_min.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_Y_MAX
                            .Item(1, ii).Value = m_DefectData.cull_frm_y_max.ToString
                        Case EM_ROW_DEFECTINF.BOX_X_MIN
                            .Item(1, ii).Value = m_DefectData.box_x_min.ToString
                        Case EM_ROW_DEFECTINF.BOX_X_MAX
                            .Item(1, ii).Value = m_DefectData.box_x_max.ToString
                        Case EM_ROW_DEFECTINF.BOX_Y_MIN
                            .Item(1, ii).Value = m_DefectData.box_y_min.ToString
                        Case EM_ROW_DEFECTINF.BOX_Y_MAX
                            .Item(1, ii).Value = m_DefectData.box_y_max.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_X_MIN
                            .Item(1, ii).Value = m_DefectData.base_box_x_min.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_X_MAX
                            .Item(1, ii).Value = m_DefectData.base_box_x_max.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_Y_MIN
                            .Item(1, ii).Value = m_DefectData.base_box_y_min.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_Y_MAX
                            .Item(1, ii).Value = m_DefectData.base_box_y_max.ToString
                        Case EM_ROW_DEFECTINF.BASE_CENTER_X
                            .Item(1, ii).Value = m_DefectData.base_center_x.ToString
                        Case EM_ROW_DEFECTINF.BASE_CENTER_Y
                            .Item(1, ii).Value = m_DefectData.base_center_y.ToString
                        Case EM_ROW_DEFECTINF.BASE_H_SIZE
                            .Item(1, ii).Value = m_DefectData.base_h_size.ToString
                        Case EM_ROW_DEFECTINF.BASE_W_SIZE
                            .Item(1, ii).Value = m_DefectData.base_w_size.ToString
                        Case EM_ROW_DEFECTINF.BASE_H_RES
                            .Item(1, ii).Value = m_DefectData.base_h_res.ToString
                        Case EM_ROW_DEFECTINF.BASE_W_RES
                            .Item(1, ii).Value = m_DefectData.base_w_res.ToString
                        Case EM_ROW_DEFECTINF.SHRINK_RATIO
                            .Item(1, ii).Value = m_DefectData.nShrinkRatio.ToString
                        Case EM_ROW_DEFECTINF.CENTER_X
                            .Item(1, ii).Value = m_DefectData.center_x.ToString
                        Case EM_ROW_DEFECTINF.CENTER_Y
                            .Item(1, ii).Value = m_DefectData.center_y.ToString
                        Case EM_ROW_DEFECTINF.H_SIZE
                            .Item(1, ii).Value = m_DefectData.h_size.ToString
                        Case EM_ROW_DEFECTINF.W_SIZE
                            .Item(1, ii).Value = m_DefectData.w_size.ToString
                        Case EM_ROW_DEFECTINF.TYUU
                            .Item(1, ii).Value = m_DefectData.nTyuu.ToString
                        Case EM_ROW_DEFECTINF.GYUU
                            .Item(1, ii).Value = m_DefectData.nGyuu.ToString
                        Case EM_ROW_DEFECTINF.WIDABSPOS
                            .Item(1, ii).Value = m_DefectData.nWidAbsPos.ToString
                        Case EM_ROW_DEFECTINF.WIDDIV
                            .Item(1, ii).Value = m_DefectData.nWidDiv.ToString
                        Case EM_ROW_DEFECTINF.NOBJTANSHO
                            .Item(1, ii).Value = m_DefectData.nObjTansho.ToString
                        Case EM_ROW_DEFECTINF.OBJDEFLIST
                            .Item(1, ii).Value = m_DefectData.nObjDeflist.ToString
                        Case EM_ROW_DEFECTINF.MODEDEFLIST
                            .Item(1, ii).Value = m_DefectData.nModeDeflist.ToString
                        Case EM_ROW_DEFECTINF.HASSEIDATE
                            .Item(1, ii).Value = String.Format("{0:0000}/{1:00}/{2:00} {3:00}:{4:00}:{5:00}",
                                                               m_DefectData.time(0), m_DefectData.time(1), m_DefectData.time(2),
                                                               m_DefectData.time(3), m_DefectData.time(4), m_DefectData.time(5))
                            .Item(1, ii).Style.Font = New System.Drawing.Font("�l�r ����", 7.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                    End Select
                Next
            End With

            With dgvKizuInfo2
                For col As EM_COL_DEFECTINF = EM_COL_DEFECTINF.TYPE To EM_COL_DEFECTINF.MAX_COL
                    Select Case col
                        Case EM_COL_DEFECTINF.TYPE
							.Item(col, 0).Value = mcls_Param.GetTypeName(m_DefectData.nTid)
						Case EM_COL_DEFECTINF.GRADE
                            .Item(col, 0).Value = mcls_Param.GetGradeName(m_DefectData.nGid)
                            .Item(col, 0).Style.BackColor = mcls_Param.GetGradeColor(m_DefectData.nGid)
                            .Item(col, 0).Style.SelectionBackColor = .Item(col, 0).Style.BackColor
                        Case EM_COL_DEFECTINF.FPOS
                            ' �i��敪
                            '// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
                            '.Item(col, 0).Value = Format(Int(m_DefectData.y / 100) / 10, "####0.0") 'F�\��
							.Item(col, 0).Value = Format(Int(m_DefectData.y_out / 100) / 10, "####0.0") 'F�\��
							'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
                    End Select
                Next
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �ڍ׏��\���ؑ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub cbDetail_CheckedChanged(sender As Object, e As EventArgs)
        Dim cb As CheckBox = CType(sender, CheckBox)
        Dim minSize As Size = Me.MinimumSize


        ' ��ʃT�C�Y��ς���O�Ƀp�l���̉E�A���J�[���O��
        tlpMain.Anchor = tlpMain.Anchor And (AnchorStyles.Top Or AnchorStyles.Bottom Or AnchorStyles.Left)

        If cb.Checked Then
            cb.BackColor = Color.Cyan
            Me.Width = Me.Width + dgvKizuInfo.Width
            minSize.Width = minSize.Width + dgvKizuInfo.Width
            Me.MinimumSize = minSize
        Else
            minSize.Width = minSize.Width - dgvKizuInfo.Width
            Me.MinimumSize = minSize
            cb.BackColor = SystemColors.Control
            Me.Width = Me.Width - dgvKizuInfo.Width
            tlpMain.Width = Me.ClientSize.Width
        End If


        ' �E�A���J�[���ăZ�b�g
        tlpMain.Anchor = tlpMain.Anchor Or AnchorStyles.Right

    End Sub
End Class
