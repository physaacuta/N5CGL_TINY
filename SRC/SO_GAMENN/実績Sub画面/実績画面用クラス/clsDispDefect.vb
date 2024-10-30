'*******************************************************************************
'   �r���\���N���X
'
'  [Ver]
'       Ver.01  2013/12/25  ����
'
'	[����]
'       ksMemRead�֐����g�p���Ă��邽��
'       [Option Strict Off]�Ƃ���(�G���[���)
'*******************************************************************************
Option Strict On

Imports System.Reflection

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' �r���\���N���X
''' </summary>
''' <remarks></remarks>
Public Class clsDispDefect
	Implements IDisposable
	'///////////////////////////////////////////////////////////////////////////
	'�萔��`
	'///////////////////////////////////////////////////////////////////////////
    Private Const DEFECT_ZOOM As Boolean = False        ' �r�����������Ɋg�傷�邩 false:�g�債�Ȃ� true:�g�傷��
    Private Const DEFECT_DGV_WIDTH As Integer = 120     ' �r���DGV��

    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r�\�����
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_DEFECT_DSP
        Dim nNo As Integer                              '���No.
        Dim lblTitleL() As Label                        '���^�C�g��
        Dim lblTitleR() As Label                        '�E�^�C�g��
        Dim picImg() As PictureBox                      '�r�摜
        Dim lblImg() As Label                           '�r�摜�^�C�g��

        Dim lblType As Label                            ' �r�햼
        Dim lblGrade As Label                           ' �O���[�h��
        Dim lblY As Label                               ' ����ʒu
        Dim lblKizuNo As Label                          ' �rNo
        Dim lblYName As Label                           ' ����ʒu(HED)
        Dim lblKizuNoName As Label                      ' �rNo(HED)
        Dim lblCamNo As Label                           ' �J����No

        Dim clrLine As Color                            '�⏕���i�������y�уI�[�o�[���C�j�̐F
        Dim tblLayoutPnl As TableLayoutPanel
        Dim dgvInfo As tCnt.tCnt_DataGridView

        '�C�j�V�����C�Y
        Public Sub Initialize()
            ReDim lblTitleL(DSP_IMAGE_MAX - 1)
            ReDim lblTitleR(DSP_IMAGE_MAX - 1)
            ReDim picImg(DSP_IMAGE_MAX - 1)
            ReDim lblImg(DSP_IMAGE_MAX - 1)
        End Sub
    End Structure

    '---------------------------------------------------------------------------
    ' �萔��`
    ''---------------------------------------------------------------------------
    ''' <summary>
    ''' �r���\���t�H�[�}�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_DEF_FORMAT
        emName1 = 0                                     ' �w�b�_�[1
        emData1                                         ' �f�[�^1
        emName2                                         ' �w�b�_�[2
        emData2                                         ' �f�[�^2
        emEnd
    End Enum

    Private Const DGV_HED_WIDTH As Integer = 80
    Private Const DGV_DATA_WIDTH As Integer = 48

    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                    '���\�[�X�J���t���O[True:�J����,False:���J��]

    Private mcls_Param As clsParamManager                       '�p�����[�^�Ǘ�
    Private mcls_Log As tClass_LogManager                       '���O�Ǘ�
    Private mcls_ImgDsp() As tClass_ImageDsp2                    '�摜�\���N���X
    Private mcls_DataMap As clsDataMap                          ' �r�}�b�v�f�[�^�N���X
    Private mcls_DataDef As clsDataDefect                       ' �r�摜�N���X

    Private m_typMapValue As TYP_MAP_LIST_VALUE                 '�\���I���r

    Private m_typDefectDsp As TYP_DEFECT_DSP                    '�r���\��

    Private m_bDspData As Boolean                               '�r���\����� (True:�\���� False:��\��)
    Private m_bOverlay As Boolean                               '�I�[�o�[���C�̕\���L�� (True:�\�� False:��\��)
    Private m_bScale As Boolean                                 '�X�P�[���i�������j�\��(True:�L�� False:����)
    Private m_bImageZoom As Boolean                             '�g��\��(True:�L�� False:����)
    Private m_nZoomRate As Integer                              '�r�摜�̕\���{��
    Private m_nImageWidth As Integer                            '�\���G���A�̉���[mm]
    Private m_emSide As EM_MAP_SIDE                             '�r�摜���x��DS�ʒu[NORMAL:����DS�AREVERSE:����WS]

    Private m_nImageExtend As Single = 1.5                      ' �������r���g��\������{�̔{���ݒ�
    Private m_extend As Boolean = False                         ' �\�����Ă����r�摜���g��\�����ǂ����̐ݒ�
    Private m_picbox1 As PictureBox

    '///////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �I�[�o�[���C�\���L��
    ''' </summary>
    ''' <value>True:�\�� False:��\��</value>
    ''' <returns>True:�\�� False:��\��</returns>
    ''' <remarks></remarks>
    Public Property OverlayVisible() As Boolean
        Get
            Return m_bOverlay
        End Get
        Set(ByVal value As Boolean)
            m_bOverlay = value
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                If 0 < dsp.tProp_OverLayCnt Then
                    dsp.OverLayDspChange(m_bOverlay)
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' �ڐ���\���L��
    ''' </summary>
    ''' <value>True:�\�� False:��\��</value>
    ''' <returns>True:�\�� False:��\��</returns>
    ''' <remarks></remarks>
    Public Property MemoriVisible() As Boolean
        Get
            Return m_bScale
        End Get
        Set(ByVal value As Boolean)
            m_bScale = value
            Dim memTmp As tClass_ImageDspBase.MEMORI_INF
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                memTmp = dsp.tProp_MemoriInf
                memTmp.bIsDsp = value
                dsp.tProp_MemoriInf = memTmp
                dsp.Refresh()
            Next
        End Set
    End Property

    ''' <summary>
    ''' �r�\�������擾
    ''' </summary>
    ''' <returns>�r�\�����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectViewInf() As TYP_DEFECT_DSP
        Get
            Return m_typDefectDsp
        End Get
    End Property

    ''' <summary>
    ''' �\���r�摜���擾
    ''' </summary>
    ''' <param name="emkind">0:�� 1:��</param>
    ''' <returns>�r�摜</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DspImage(ByVal emkind As EM_IMAGE_POS) As Image
        Get
            Return mcls_ImgDsp(emkind).tProp_Paint_Img(0)
        End Get
    End Property

    ''' <summary>
    ''' �f�t�H���g�摜�\����Ԃ��擾���܂�
    ''' </summary>
    ''' <param name="emkind">0:�� 1:��</param>
    ''' <returns>True:�f�t�H���g�摜�\�� False:�r�摜��\��</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsDspDefault(ByVal emkind As EM_IMAGE_POS) As Boolean
        Get
            If mcls_ImgDsp(emkind) Is Nothing = True Then
                Return True
            End If

            Return mcls_ImgDsp(emkind).tProp_Paint_IsDefImgDspn
        End Get
    End Property

    ''' <summary>
    ''' �r�摜�\���G���A�̊����[mm]
    ''' </summary>
    ''' <value>�r�摜�\���G���A�̊����[mm]</value>
    ''' <returns>�r�摜�\���G���A�̊����[mm]</returns>
    ''' <remarks></remarks>
    Public Property ImageWidth() As Integer
        Get
            Return m_nImageWidth
        End Get
        Set(ByVal value As Integer)
            m_nImageWidth = value
        End Set
    End Property

    ''' <summary>
    ''' �㉺���]�L�����擾
    ''' </summary>
    ''' <value>NORMAL:���]�����AREVERSE:���]�L��</value>
    ''' <returns>NORMAL:���]�����AREVERSE:���]�L��</returns>
    ''' <remarks></remarks>
    Public Property IsTurnTB() As EM_IMAGE_TOP
        Get
            If mcls_ImgDsp(0).tProp_ImgKind_IsTurnTB Then
                Return EM_IMAGE_TOP.REVERSE
            Else
                Return EM_IMAGE_TOP.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_TOP)
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                If value = EM_IMAGE_TOP.REVERSE Then
                    dsp.tProp_ImgKind_IsTurnTB = True
                Else
                    dsp.tProp_ImgKind_IsTurnTB = False
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
            If mcls_ImgDsp(0).tProp_ImgKind_IsTurnLR Then
                Return EM_IMAGE_SIDE.REVERSE
            Else
                Return EM_IMAGE_SIDE.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_SIDE)
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                If value = EM_IMAGE_SIDE.REVERSE Then
                    dsp.tProp_ImgKind_IsTurnLR = True
                Else
                    dsp.tProp_ImgKind_IsTurnLR = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 90�x��]�L�����擾
    ''' </summary>
    ''' <value>NORMAL:���]�����ALOTATE:���]�L��</value>
    ''' <returns>NORMAL:���]�����ALOTATE:���]�L��</returns>
    ''' <remarks></remarks>
    Public Property IsTurn90() As EM_IMAGE_90
        Get
            If mcls_ImgDsp(0).tProp_ImgKind_Rotate90 Then
                Return EM_IMAGE_90.LOTATE
            Else
                Return EM_IMAGE_90.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_90)
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                If value = EM_IMAGE_90.LOTATE Then
                    dsp.tProp_ImgKind_Rotate90 = True
                Else
                    dsp.tProp_ImgKind_Rotate90 = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' �r���\����Ԃ̎擾
    ''' </summary>
    ''' <returns>True:�\���� False:��\����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsDspData() As Boolean
        Get
            Return m_bDspData
        End Get
    End Property

    ''' <summary>
    ''' �g��\���L��/����
    ''' </summary>
    ''' <value>True:�L���AFalse:����</value>
    ''' <returns>True:�L���AFalse:����</returns>
    ''' <remarks></remarks>
    Public Property ImageZoom() As Boolean
        Get
            Return m_bImageZoom
        End Get
        Set(ByVal value As Boolean)
            m_bImageZoom = value
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
            If m_emSide = EM_MAP_SIDE.NORMAL Then           '����DS
                m_typDefectDsp.lblTitleL(0).Text = DIRECTION_NAME_DS
                m_typDefectDsp.lblTitleR(0).Text = DIRECTION_NAME_WS
                m_typDefectDsp.lblTitleL(1).Text = DIRECTION_NAME_DS
                m_typDefectDsp.lblTitleR(1).Text = DIRECTION_NAME_WS
            Else                                            '����WS
                m_typDefectDsp.lblTitleL(0).Text = DIRECTION_NAME_WS
                m_typDefectDsp.lblTitleR(0).Text = DIRECTION_NAME_DS
                m_typDefectDsp.lblTitleL(1).Text = DIRECTION_NAME_WS
                m_typDefectDsp.lblTitleR(1).Text = DIRECTION_NAME_DS
            End If
        End Set
    End Property

    ''' <summary>
    ''' ���ݕ\�������r���
    ''' </summary>
    ''' <value>�r���\����</value>
    ''' <returns>�r���\����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CurrentDefect() As TYP_MAP_LIST_VALUE
        Get
            Return m_typMapValue
        End Get
    End Property

    ''' <summary>
    ''' �r�}�b�v�f�[�^�N���X�ݒ�
    ''' </summary>
    Public WriteOnly Property DataMap() As clsDataMap
        Set(value As clsDataMap)
            mcls_DataMap = value
        End Set
    End Property

    '---------------------------------------------------------------------------
    ' ���\�b�h
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="typDefectDsp">�r���\���R���g���[��</param>
    ''' <param name="param">�p�����[�^�Ǘ�</param>
    ''' <param name="log">���O�Ǘ�</param>
    ''' <param name="strPath">�f�t�H���g�摜�̃��\�[�X�p�X</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typDefectDsp As TYP_DEFECT_DSP, ByRef param As clsParamManager, ByRef log As tClass_LogManager, ByVal strPath As String)
        Try
            m_typMapValue.Initialize()

            mcls_Log = log
            mcls_Param = param
            m_typDefectDsp = typDefectDsp
            m_bDspData = False

            mcls_DataDef = New clsDataDefect(mcls_Param, gcls_Log)

            ReDim mcls_ImgDsp(m_typDefectDsp.picImg.Length - 1)

            For nCnt As Integer = 0 To m_typDefectDsp.picImg.Length - 1
                mcls_ImgDsp(nCnt) = New tClass_ImageDsp2()
                mcls_ImgDsp(nCnt).ScaleInterval = 5                               ' �ڐ���ڍו\�� 3����
                mcls_ImgDsp(nCnt).SetLogManager = mcls_Log                        ' ���O�Ǘ��Z�b�g
                InitImageDsp(mcls_ImgDsp(nCnt), m_typDefectDsp.picImg(nCnt), strPath)
                m_typDefectDsp.lblImg(nCnt).Text = String.Empty
                m_typDefectDsp.lblCamNo.Text = String.Empty
                dgvInitialize(typDefectDsp.dgvInfo)
                AddHandler m_typDefectDsp.tblLayoutPnl.Resize, AddressOf Ctrl_ReSize
            Next

            m_nZoomRate = 100                                                   '�W���͔{��100%�ŕ\��

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R���X�g���N�^�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �f�X�g���N�^
    ''' </summary>
    ''' <param name="disposing">True:�����I�ȌĂяo�� False:�Öق̌Ăяo��</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                '�I�u�W�F�N�g�j��
                For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp               '�摜�\���̊J��
                    If Not dsp Is Nothing Then
                        dsp.Dispose()
                        dsp = Nothing
                    End If
                Next
                If Not mcls_DataDef Is Nothing Then
                    mcls_DataDef.Dispose()
                    mcls_DataDef = Nothing
                End If
            End If
            m_bIsDesposed = True                                                'Dispose�ς݂Ƃ���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�u�W�F�N�g�j���ُ�[{0}]", ex.Message))
        End Try
    End Sub
#Region " IDisposable Support "
    '���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        '���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' �摜�\��������
    ''' </summary>
    ''' <param name="clsImgDsp">�摜�\���N���X</param>
    ''' <param name="clsPic">�s�N�`���[�{�b�N�X</param>
    ''' <param name="strPath">�\���摜�̃p�X</param>
    ''' <remarks></remarks>
    Private Sub InitImageDsp(ByVal clsImgDsp As tClass_ImageDsp2, ByRef clsPic As PictureBox, ByVal strPath As String)
        Try

            m_picbox1 = clsPic

            '---------------------------
            '�\���摜�擾
            '---------------------------
            Dim asmData As System.Reflection.Assembly = [Assembly].GetExecutingAssembly()
            Dim clsStream As System.IO.Stream = asmData.GetManifestResourceStream(strPath)
            Dim clsImg As Image = Image.FromStream(clsStream, False, True)
            clsStream.Close()

            '---------------------------
            '�摜�ݒ�
            '---------------------------
            With clsImgDsp
                .SetImgInf(clsImg.Width, clsImg.Height, 1, 1)
                .tProp_AriaInf_Aria = clsPic.ClientSize
                .tProp_AriaInf_IsCenter = True
                .tProp_Paint_IsPaint = True
                .tProp_Paint_IsDefImgDspn = True
                .tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRawWidth
                .Calculation()
                .AddHandler_PaintEvent(clsPic)
                '�f�t�H���g�摜�ݒ�
                .SetDefImage(clsImg)
            End With

            '---------------------------
            '�ڐ���ݒ�
            '---------------------------
            Dim typMemo As tClass_ImageDspBase.MEMORI_INF
            With typMemo
                .bIsDsp = False
                .LineWidth = 2
                .FontSize = 12
                .Color = m_typDefectDsp.clrLine
                .HojoLineLen = 5
                .strBaseString = "0[mm]"
                .nPosStringBot = 15
                .nPosStringLeft = 5
                .nPosLineBot = 20
                .nPosLineLeft = 30
                .nLoopCntX = 6
                .nIntervalX = 15
                .nLoopCntY = 12
                .nIntervalY = 15
            End With
            clsImgDsp.tProp_MemoriInf = typMemo
            AddHandler clsImgDsp.tEv_Paint_End, AddressOf ImgDsp_tEv_Paint_End
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�摜�\���������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �X�P�[����ݒ肵�܂�
    ''' </summary>
    ''' <param name="nIntvalX">X����1�������P��[mm]</param>
    ''' <param name="nNumX">X���̃�������</param>
    ''' <param name="nIntvalY">Y����1�������P��[mm]</param>
    ''' <param name="nNumY">Y���̃�������</param>
    ''' <remarks></remarks>
    Public Sub SetScaleConfig(ByVal nIntvalX As Integer, ByVal nNumX As Integer, ByVal nIntvalY As Integer, ByVal nNumY As Integer)
        Try
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                Dim memo As tClass_ImageDsp.MEMORI_INF = dsp.tProp_MemoriInf
                memo.nIntervalX = nIntvalX
                memo.nIntervalY = nIntvalY
                memo.nLoopCntX = nNumX
                memo.nLoopCntY = nNumY
                dsp.tProp_MemoriInf = memo
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�摜�\���N���X�������ݒ�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�摜���폜���܂�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DisposeImage()
        Try
            For ii As Integer = 0 To mcls_ImgDsp.Length - 1
                If mcls_ImgDsp(ii) Is Nothing = True Then
                    Continue For
                End If
                Dim old_img As Image = mcls_ImgDsp(ii).tProp_Paint_Img(0)
                If old_img Is Nothing = False Then                              '�摜���̂Ă�
                    old_img.Dispose()
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�摜�\���N���X�摜�j���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r���\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspDefInf()
        Try
            '-------------------------------------------------------------------
            '�r����\������
            '-------------------------------------------------------------------
            Dim typData As DETECT_BASE_DATA = mcls_DataDef.DefectData

            With m_typDefectDsp
                '�r���\��
                .lblType.Text = mcls_Param.GetTypeName(typData.nTid)
                .lblGrade.Text = mcls_Param.GetGradeName(typData.nGid)
                .lblGrade.BackColor = mcls_Param.GetGradeColor(typData.nGid)
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>				
				'.lblY.Text = Format(Int(typData.y / 100) / 10, "####0.0")
				.lblY.Text = Format(Int(typData.y_out / 100) / 10, "####0.0")
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
                .lblKizuNo.Text = CStr(typData.nKizuNo)
                If 0 = typData.dsws Then
                    'DS
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Value = typData.ds.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Value = typData.ws.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.BackColor = Color.White
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.BackColor = Color.LightGray
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                Else
                    'WS
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Value = typData.ds.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Value = typData.ws.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.BackColor = Color.LightGray
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.BackColor = Color.White
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                End If

                .dgvInfo.Item(EM_DEF_FORMAT.emData2, 1).Value = typData.speed


                If 1 = typData.cycle_div Or 2 = typData.cycle_div Then                                    ' �������r�̏ꍇ
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Value = "�L"
                    If 1 = typData.cycle_div Then
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = g_ColorSimpleCycle
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.Black
                    Else
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = g_ColorKizuCycle
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.White
                    End If
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 2).Value = typData.pitch
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 2).Value = typData.roolkei.ToString("####0")
                Else
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Value = "��"
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = Color.White
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.Black
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 2).Value = ""
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 2).Value = ""
                End If

            End With

            Dim nPos As Integer = 0                         '�摜�ʒu(0:��,1:��))
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth)
                dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth)

                Dim nNo As Integer = mcls_Param.GetTypeImage(typData.nTid, CType(nPos, EM_IMAGE_POS))
                If 0 >= nNo Then
                    If 1 = nPos Then
                        nNo = 2
                    Else
                        nNo = 1
                    End If
                End If
                '�\������摜��SET
                Dim imgData As Image = mcls_DataDef.DefectImage(nNo - 1)
                '�摜��ނɑΉ�����^�C�g�����Z�b�g����
                m_typDefectDsp.lblImg(nPos).Text = mcls_DataDef.ImageTitle(nNo - 1)
                m_typDefectDsp.lblCamNo.Text = mcls_DataDef.DefectData.camset.ToString
                '---------------------------------------------------------------
                '�r�摜��ݒ肷��
                '---------------------------------------------------------------
                If Not imgData Is Nothing Then                                       '�����ˉ摜���擾�ł���ΐݒ�
                    dsp.SetImgInf(imgData.Width, imgData.Height, typData.w_res, typData.h_res, typData.center_x, typData.center_y)
                    dsp.SetImage(imgData, True)
                    dsp.tProp_Paint_IsDefImgDspn = False
                    dsp.tProp_AriaInf_IsCenter = True

                    '�g��\�����̔{����ݒ�
                    dsp.tProp_AriaInf_ZoomW = dsp.tProp_AriaInf_ZoomW * m_nImageExtend
                    dsp.tProp_AriaInf_ZoomH = dsp.tProp_AriaInf_ZoomH * m_nImageExtend

                    dsp.Calculation()

                    ' �g�債����Ԃ��r�̊O�ژg���\���͈͂Ɏ��܂�ꍇ�́A�g�債���摜��\������
                    Dim sz As Point = dsp.GetSimplePoint_ImgToDsp(typData.box_x_max - typData.box_x_min, typData.box_y_max - typData.box_y_min)
                    If sz.X < m_picbox1.Width And sz.Y < m_picbox1.Height And DEFECT_ZOOM = True Then
                        '�g��\��ON
                        m_extend = True
                    Else
                        '�g��\��OFF
                        m_extend = False
                        '�l��߂�
                        dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth)
                        dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth)
                        dsp.Calculation()
                    End If


                    If 0 < dsp.tProp_OverLayCnt Then        '���ɃI�[�o�[���C���ݒ肳��Ă����ꍇ
                        dsp.ResetOverLayInf()
                    End If

                    '-----------------------------------------------------------
                    '�I�[�o�[���C��ݒ肷��
                    '-----------------------------------------------------------
                    Dim oly As tClass_ImageDsp.OVERLAY_INF = Nothing
                    oly.bIsDsp = m_bOverlay
                    oly.Color = m_typDefectDsp.clrLine
                    oly.LineWidth = 2
                    oly.HoseiX = 10
                    oly.HoseiY = 10
                    oly.ovlPos1 = New Point(typData.box_x_min, typData.box_y_min)
                    oly.ovlPos2 = New Point(typData.box_x_max, typData.box_y_max)
                    dsp.SetOverLayInf(oly)                      '�I�[�o�[���C�ݒ�
                Else                                            '���s���Ă���΃f�t�H���g�摜��\��
                    dsp.tProp_Paint_IsDefImgDspn = True
                End If
                nPos += 1
            Next

            m_bDspData = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\���ُ�[{0}]", ex.Message))
            m_bDspData = False

        Finally
            Refresh()
        End Try
    End Sub

    ''' <summary>
    ''' �r���\���i���щ�ʗp�j
    ''' </summary>
    Public Sub DspDefect(ByVal nMen As Integer, ByVal typMapValue As TYP_MAP_LIST_VALUE, Optional ByVal bReTry As Boolean = False)

        Try
            Dim data As New DETECT_BASE_DATA
            If Not mcls_DataMap.DefectData(nMen, New clsDataMap.DEFECT_KEY(typMapValue.nKizuNo), data) Then
                Exit Try
            End If
            mcls_DataDef.SetDefect(mcls_DataMap.KizukenNo_In, data)
            m_typMapValue = typMapValue

            '�r���\��
            DspDefInf()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\���ُ�[{0}]", ex.Message))
        End Try

    End Sub


    ''' <summary>
    ''' �\��������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        Try
            With m_typDefectDsp
                .lblType.Text = ""

                .lblGrade.Text = ""
                .lblGrade.BackColor = g_ColorControl

                .lblY.Text = ""
                .lblKizuNo.Text = ""

                Call DspDgvInit(.dgvInfo)

                For ii As Integer = 0 To .lblImg.Length - 1
                    .lblImg(ii).Text = ""
                Next

                .lblCamNo.Text = ""
            End With

            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                dsp.tProp_Paint_IsPaint = True
                dsp.tProp_Paint_IsDefImgDspn = True
            Next

            mcls_DataDef.DataInit()

            ' �����o�ϐ��N���A
            m_bDspData = False
            m_typMapValue.Initialize()

            Refresh()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\���������\���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r��񗓏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub DspDgvInit(ByVal dgv As DataGridView)
        With dgv
            .Item(EM_DEF_FORMAT.emData1, 0).Value = ""

            .Item(EM_DEF_FORMAT.emData2, 0).Value = ""

            .Item(EM_DEF_FORMAT.emData1, 0).Style.BackColor = Color.White
            .Item(EM_DEF_FORMAT.emData1, 0).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Item(EM_DEF_FORMAT.emData2, 0).Style.BackColor = Color.White
            .Item(EM_DEF_FORMAT.emData2, 0).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)


            .Item(EM_DEF_FORMAT.emData2, 1).Value = ""
            .Item(EM_DEF_FORMAT.emData1, 1).Value = "��"
            .Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = Color.White
            .Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.Black
            .Item(EM_DEF_FORMAT.emData1, 2).Value = ""
            .Item(EM_DEF_FORMAT.emData2, 2).Value = ""

        End With
    End Sub

    ''' <summary>
    ''' �\���X�V
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Refresh()
        Try

            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                dsp.Refresh()
            Next

            m_typDefectDsp.dgvInfo.Refresh()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\���X�V�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�g��摜�\��
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Sub ShowPopup(ByVal clsPopup As frmDefectImage)
        Dim clsImg(DSP_KIRI_IMAGE_MAX - 1) As Image
        Dim tmpTitle(DSP_KIRI_IMAGE_MAX - 1) As String
        Dim nRetc As ValueType = 0

        Try
            '�r�摜���\������Ă��Ȃ�
            If Not IsDspData Then
                Exit Try
            End If

            '�摜���ݒ肳��Ă��Ȃ��ꍇ�͏I��
            If mcls_ImgDsp(0) Is Nothing Then
                Exit Try
            End If

            '�摜���ǂݍ��܂�Ă��Ȃ��ꍇ�͏I��
            If Not mcls_DataDef.IsImgRead Then
                Exit Try
            End If

            '---------------------------
            '�r�摜�g��\��
            '---------------------------
            With clsPopup
                .DispImgNo = m_typDefectDsp.nNo                                 '�摜No.
                .SetVisibleMemory(MemoriVisible)                                '�������̕\���L���ݒ�
                .SetVisibleOverlay(OverlayVisible)                              '�I�[�o�[���C�̕\���L���ݒ�
                .SetColor = m_typDefectDsp.clrLine                              '�������A�I�[�o���C�F
                .IsTurnLR = IsTurnLR                                            '���E���]�ݒ�
                .IsTurnTB = IsTurnTB                                            '�㉺���]�ݒ�
                .IsRotate90 = IsTurn90                                          '90�x��]�ݒ�()
                .Side = Side                                                    '�r�摜���x��DS�ʒu
                ''�r���
                .SetDefectData(mcls_DataDef.DefectData, mcls_DataDef.DefectImage, mcls_DataDef.ImageTitle)
                '�\���{��
                .ZoomRate = CInt((100.0 * mcls_DataDef.DefectData.w_res / m_nImageWidth) * 100.0)
                '�ڐ���`��ڍ�
                .SetMemoryConfig(JISSEKI_SCALE_INTVAL_X, JISSEKI_SCALE_NUM_X, JISSEKI_SCALE_INTVAL_Y, JISSEKI_SCALE_NUM_Y)
                .DspUpDate(True)                                                          '�\��
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�g��摜�\���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�摜�`��I���C�x���g
    ''' </summary>
    ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
    ''' <remarks></remarks>
    Private Sub ImgDsp_tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim nPosLineTop As Integer
        Dim nLineWidth As Integer

        Dim typAriaInf As tClass_ImageDsp.ARIA_INF                     ' �\���̈���
        Dim typCalcAns As tClass_ImageDsp.CALC_ANS                     ' ���W�ŏI����
        Dim typMemori As tClass_ImageDsp.MEMORI_INF                    ' ���������

        Try
            typMemori = mcls_ImgDsp(0).tProp_MemoriInf
            typAriaInf = mcls_ImgDsp(0).tProp_AriaInf
            typCalcAns = mcls_ImgDsp(0).tProp_CalcAns

            If EM_IMAGE_90.NORMAL = IsTurn90 Then
                nPosLineTop = typAriaInf.nAriaHeight - typMemori.nPosLineBot
            Else
                nPosLineTop = typAriaInf.nAriaWidth - typMemori.nPosLineBot
            End If
            nLineWidth = CInt(typMemori.LineWidth / 2)
            If nLineWidth <= 0 Then
                nLineWidth = 1
            End If

            '�G�b�W�J�b�g�s�\��
            With m_typDefectDsp
                Dim g As Graphics = e.Graphics
                Dim sf As New StringFormat
                Dim f As New Font("MS P�S�V�b�N", 9)
                If m_extend Then
                    sf.Alignment = StringAlignment.Center
                    sf.LineAlignment = StringAlignment.Center
                    g.DrawString("�g��\��", f, Brushes.Yellow, m_picbox1.Width - 40, m_picbox1.Height - 9, sf)
                End If

            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�ڍזڐ��`��ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r���f�[�^�O���b�h��������
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv

            .RowCount = 0
            .RowTemplate.Height = 30

            '' �����`
            .ScrollBars = ScrollBars.Both
            .DoubleBuffered = True

            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������
            .ColumnHeadersVisible = False                                   ' ��w�b�_�\���Ȃ�

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '�I��F
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

        End With
        Dim nWid As Integer = CInt((dgv.Size.Width - DGV_HED_WIDTH * 2) / 2)
        dgvColSetText(dgv, "Name1", DGV_HED_WIDTH, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, , , True)
        dgvColSetText(dgv, "data1", nWid, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter, , , True)
        dgvColSetText(dgv, "Name2", DGV_HED_WIDTH, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, , , True)
        dgvColSetText(dgv, "data2", nWid, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter, , , True)

        dgv.RowCount = 3
        For ii As Integer = 0 To dgv.RowCount - 1
            dgv.Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
            dgv.Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
            dgv.Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            dgv.Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            dgv.Item(EM_DEF_FORMAT.emName1, ii).Style.WrapMode = DataGridViewTriState.True
            dgv.Item(EM_DEF_FORMAT.emName2, ii).Style.WrapMode = DataGridViewTriState.True
        Next
        dgv.Item(EM_DEF_FORMAT.emName1, 0).Value = DIRECTION_NAME_DS & "�`[mm]"
        dgv.Item(EM_DEF_FORMAT.emName1, 1).Value = "������"
        dgv.Item(EM_DEF_FORMAT.emData1, 1).Style.Alignment = DataGridViewContentAlignment.MiddleCenter
        dgv.Item(EM_DEF_FORMAT.emName1, 2).Value = "�s�b�`"

        dgv.Item(EM_DEF_FORMAT.emName2, 0).Value = DIRECTION_NAME_WS & "�`[mm]"
        dgv.Item(EM_DEF_FORMAT.emName2, 1).Value = "���x[mpm]"
        dgv.Item(EM_DEF_FORMAT.emName2, 2).Value = "�a[mm]"

    End Sub


    ''///////////////////////////////////////////////////////////////////////////
    ''�C�x���g�n���h���֐�
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���T�C�Y(pic)
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Public Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)

        '�摜
        For ii As Integer = 0 To mcls_ImgDsp.Length - 1
            If mcls_ImgDsp(ii) Is Nothing Then Continue For
            ' �T�C�Y�ύX
            mcls_ImgDsp(ii).tProp_AriaInf_Aria = m_typDefectDsp.picImg(ii).ClientSize
            mcls_ImgDsp(ii).Calculation()
        Next

        '�r���
        With m_typDefectDsp.dgvInfo
            Dim nWid As Integer = CInt((.Size.Width - DGV_HED_WIDTH * 2) / 2)
            If nWid < 0 Then
                nWid = CInt(.Size.Width / 2)
                .Columns(EM_DEF_FORMAT.emName1).Width = 0
                .Columns(EM_DEF_FORMAT.emData1).Width = nWid
                .Columns(EM_DEF_FORMAT.emName2).Width = 0
                .Columns(EM_DEF_FORMAT.emData2).Width = nWid
                For ii As Integer = 0 To .RowCount - 1
                    .Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
                    .Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
                    .Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    .Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Next
                m_typDefectDsp.lblYName.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                m_typDefectDsp.lblKizuNoName.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                If nWid < DGV_DATA_WIDTH Then
                    Dim nWidX As Integer = CInt((.Size.Width - DGV_DATA_WIDTH * 2) / 2)
                    .Columns(EM_DEF_FORMAT.emName1).Width = nWidX
                    .Columns(EM_DEF_FORMAT.emData1).Width = DGV_DATA_WIDTH
                    .Columns(EM_DEF_FORMAT.emName2).Width = nWidX
                    .Columns(EM_DEF_FORMAT.emData2).Width = DGV_DATA_WIDTH

                    For ii As Integer = 0 To .RowCount - 1
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("�l�r ����", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("�l�r ����", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    Next
                    m_typDefectDsp.lblYName.Font = New System.Drawing.Font("�l�r ����", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    m_typDefectDsp.lblKizuNoName.Font = New System.Drawing.Font("�l�r ����", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Else
                    .Columns(EM_DEF_FORMAT.emName1).Width = DGV_HED_WIDTH
                    .Columns(EM_DEF_FORMAT.emData1).Width = nWid
                    .Columns(EM_DEF_FORMAT.emName2).Width = DGV_HED_WIDTH
                    .Columns(EM_DEF_FORMAT.emData2).Width = nWid

                    For ii As Integer = 0 To .RowCount - 1
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    Next
                    m_typDefectDsp.lblYName.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    m_typDefectDsp.lblKizuNoName.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End If
            End If

        End With

        Refresh()

    End Sub

End Class
