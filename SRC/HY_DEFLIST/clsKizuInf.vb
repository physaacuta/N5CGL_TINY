
'*********************************************************************************
'�@�r�ꗗ���N���X
'	[Ver]
'		Ver.01    2017/11/01  ����
'
'	[����]
'
'*********************************************************************************
Option Strict Off

Imports System.Reflection

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class clsKizuInf
    Implements IDisposable

    '///////////////////////////////////////////////////////////////////////////
    '�萔��`
    '///////////////////////////////////////////////////////////////////////////
    Private Const DEFECT_ZOOM As Boolean = False        ' �r�����������Ɋg�傷�邩 false:�g�債�Ȃ� true:�g�傷��
    'Private m_ColorMemori As Color = Color.DarkGreen    ' �I�[�o�[���C�A�ڐ���\���F

    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    '����r���\����
    Public Structure TYP_DEFECT_DSP
        Dim nNo As Integer              '���No.
        Dim pnlImage As Panel           '�摜�p�l��

        Dim lblPos As Label             'F�`
        Dim lblWs As Label              'WS�`
        Dim lblDs As Label              'DS�`
        Dim lblType As Label            '�r�햼
        Dim lblGrade As Label           '�O���[�h
        Dim lblSuccess As Label         '����
        Dim lblSpeed As Label           '���x
        Dim lblPitch As Label           '�s�b�`
        Dim lblRoll As Label            '���[���a

        Dim lblPosTitle As Label        '�r�ʒu�^�C�g��
        Dim lblProductNo As Label       '���Y�ԍ�
        Dim lblDate As Label            '��������

        Dim lblImgName() As Label       '�摜�^�C�g��
        Dim lblTitleL() As Label        '�摜�E���x��
        Dim lblTitleR() As Label        '�摜�����x��

        Dim lblRec As Label             '���R�[�h�ԍ�(�f�o�b�O�p)
        Dim lblPnt As Label             '�|�C���^�ԍ�(�f�o�b�O�p)
        Dim lblKizuNo As Label          '�rNo(�f�o�b�O�p)

        Dim picImg() As PictureBox      '�摜
        '�C�j�V�����C�Y
        Public Sub Initialize()
            ReDim lblImgName(DSP_IMAGE_MAX - 1)
            ReDim lblTitleL(DSP_IMAGE_MAX - 1)
            ReDim lblTitleR(DSP_IMAGE_MAX - 1)
            ReDim picImg(DSP_IMAGE_MAX - 1)
        End Sub
    End Structure
    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                    '���\�[�X�J���t���O[True:�J����,False:���J��]

    Private mcls_Param As clsParamManager                       '�p�����[�^�Ǘ�
    Private mcls_Log As tClass_LogManager                       '���O�Ǘ�
    Private mcls_ImgDsp() As tClass_ImageDsp                    '�摜�\���N���X

    Private m_nMen As Integer = -1                              '��[0:�\�A1:��]
    Private m_nPnt As Integer = -1                              'TBL_HY_DEFLIST�̃��R�[�hNo(-1:�r�����A0�`�|�C���^�ʒu(�f�[�^�L��))
    Private m_typDefectDsp As TYP_DEFECT_DSP                    '�r���\��
    Private m_typDefData As DSP_LIST_DEFECT                     '�r���(�f�[�^)

    Private m_bDspData As Boolean                               '�r���\����� (True:�\���� False:��\��)
    Private m_bOverlay As Boolean                               '�I�[�o�[���C�̕\���L�� (True:�\�� False:��\��)
    Private m_bScale As Boolean                                 '�X�P�[���i�������j�\��(True:�L�� False:����)
    Private m_bImageZoom As Boolean                             '�g��\��(True:�L�� False:����)
    Private m_nZoomRate As Integer                              '�r�摜�̕\���{��
    Private m_nImageWidth As Integer = 25                       '�\���G���A�̉���[mm]
    Private m_emSide As EM_MAP_SIDE                             '�r�摜���x��DS�ʒu[NORMAL:����DS�AREVERSE:����WS]

    Private m_nImageExtend As Single = 1.5                      ' �������r���g��\������{�̔{���ݒ�
    Private m_dRate As Single = 1.0                             ' �������r���g��\������{�̔{���ݒ�
    Private m_extend As Boolean = False                         ' �\�����Ă����r�摜���g��\�����ǂ����̐ݒ�
    Private m_picbox1 As PictureBox
    Private m_colProduct As Color


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
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
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
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
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
    ''' �r���(�L�[���)���擾
    ''' </summary>
    ''' <returns>�r�\�����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property GetDefectKeyInf() As DSP_LIST_DEFECT
        Get
            Return m_typDefData
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
    ''' �\���r�摜�^�C�g�����擾
    ''' </summary>
    ''' <param name="emkind">0:�� 1:��</param>
    ''' <returns>�r�摜</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DspImgTitle(ByVal emkind As EM_IMAGE_POS) As String
        Get
            Return m_typDefectDsp.lblImgName(emkind).Text
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
    Public Property DspRate() As Single
        Get
            Return m_dRate
        End Get
        Set(ByVal value As Single)
            m_dRate = value
        End Set
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
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
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
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
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
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
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
            mcls_Log = log
            mcls_Param = param
            m_typDefectDsp = typDefectDsp
            m_bDspData = False

            ReDim mcls_ImgDsp(m_typDefectDsp.picImg.Length - 1)

            For nCnt As Integer = 0 To m_typDefectDsp.picImg.Length - 1
                mcls_ImgDsp(nCnt) = New tClass_ImageDsp()
                InitImageDsp(mcls_ImgDsp(nCnt), m_typDefectDsp.picImg(nCnt), strPath)
                m_typDefectDsp.lblImgName(nCnt).Text = String.Empty
                'AddHandler m_typDefectDsp.tblLayoutPnl.Resize, AddressOf Ctrl_ReSize
            Next
            ImageZoom = True

            m_colProduct = g_ColorControl
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
                For Each dsp As tClass_ImageDsp In mcls_ImgDsp               '�摜�\���̊J��
                    If Not dsp Is Nothing Then
                        dsp.Dispose()
                        dsp = Nothing
                    End If
                Next
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
    Private Sub InitImageDsp(ByVal clsImgDsp As tClass_ImageDsp, ByRef clsPic As PictureBox, ByVal strPath As String)
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
                .Color = g_ColLstOverlay
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
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
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
    ''' �摜�擾
    ''' </summary>
    ''' <param name="typDefInf">�r���</param>
    ''' <param name="emImgPos">�摜�ʒu</param>
    ''' <param name="strTitle">�r�^�C�g��</param>
    ''' <returns>�摜�C���[�W</returns>
    ''' <remarks></remarks>
    Private Function GetImage(ByVal typDefInf As DSP_TYP_DEFECTINF, ByVal emImgPos As EM_IMAGE_POS, ByRef strTitle As String) As Image
        Dim imgData As Image = Nothing

        Try
            '������
            strTitle = ""
            Dim strImgPath As String = Nothing

            Select Case CType(mcls_Param.GetTypeImage(typDefInf.data.nTid, emImgPos), EM_IMAGE_NO)
                Case EM_IMAGE_NO.S
                    strImgPath = typDefInf.image_path_s
                    strTitle = DEFECT_IMG_NAME(0)
                Case EM_IMAGE_NO.R1
                    strImgPath = typDefInf.image_path_r
                    strTitle = DEFECT_IMG_NAME(1)
                Case EM_IMAGE_NO.R2
                    strImgPath = typDefInf.image_path_r2
                    strTitle = DEFECT_IMG_NAME(2)
                Case EM_IMAGE_NO.R3
                    strImgPath = typDefInf.image_path_r3
                    strTitle = DEFECT_IMG_NAME(3)
                Case Else
                    'strTitle = String.Empty
                    If EM_IMAGE_POS.IMG2 = emImgPos Then
                        strImgPath = typDefInf.image_path_r
                        strTitle = DEFECT_IMG_NAME(1)
                    Else
                        strImgPath = typDefInf.image_path_s
                        strTitle = DEFECT_IMG_NAME(0)
                    End If
                    Return Nothing
            End Select

            If 0 = strImgPath.Length Then
                '�摜�ۑ��p�X�����w��(�摜�t�@�C���Ȃ�)
                Return Nothing
            End If

            Return tClass_BmpJpeg.BmpFileLoad(strImgPath)                   ' �摜�ǂݍ���

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�摜�擾�ُ�[{0}]", ex.Message))
        End Try

        Return imgData
    End Function


    ''' <summary>
    ''' �r���\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspDefInf(ByVal nType As modMain.EM_DSP_PATTERN)
        Try
            '-------------------------------------------------------------------
            '�r����\������
            '-------------------------------------------------------------------
            Dim typData As DETECT_BASE_DATA = m_typDefData.typ.data

            With m_typDefectDsp
                '�f�o�b�O�p
                .lblRec.Text = ""
                .lblPnt.Text = ""
                .lblKizuNo.Text = typData.nKizuNo

                '�r���\��
                .lblType.Text = mcls_Param.GetTypeName(typData.nTid)
                .lblGrade.Text = mcls_Param.GetGradeName(typData.nGid)
                .lblGrade.BackColor = mcls_Param.GetGradeColor(typData.nGid)
                .lblPos.Text = Format(Int(typData.y / 100) / 10, "####0.0")
                'If modMain.EM_DSP_PATTERN.nPatternNo02 = nType Then
                '    .lblPos.Text = Format(Int(typData.y / 100) / 10, "####0.0")
                'Else
                '    .lblPos.Text = Format(CInt(Fix(typData.y / 1000)), "####0")
                'End If

                If modMain.EM_DSP_PATTERN.nPatternNo02 = nType Then
                    '.lblWs.Text = CStr(Format(typData.ws, "###0.0"))
                    '.lblWs.Text = Format(CInt(Int(typData.ws)), "###0")
                    .lblWs.Text = typData.ws.ToString("####0")
                Else
                    If 0 = typData.dsws Then
                        .lblWs.Text = ""
                        '.lblDs.Text = Format(CInt(Int(typData.ds)), "###0")
                        .lblDs.Text = typData.ds.ToString("####0")
                    Else
                        '.lblWs.Text = Format(CInt(Int(typData.ws)), "###0")
                        .lblWs.Text = typData.ws.ToString("####0")
                        .lblDs.Text = ""
                    End If
                End If
                .lblSpeed.Text = typData.speed
                If 1 = typData.cycle_div Or 2 = typData.cycle_div Then                                    ' �������r�̏ꍇ
                    .lblSuccess.Text = "�L"
                    If 1 = typData.cycle_div Then
                        .lblSuccess.BackColor = g_ColorSimpleCycle
                        .lblSuccess.ForeColor = Color.Black
                    Else
                        .lblSuccess.BackColor = g_ColorKizuCycle
                        .lblSuccess.ForeColor = Color.White
                    End If
                    .lblPitch.Text = typData.pitch
                    '.lblRoll.Text = typData.roolkei.ToString("####0")
                    .lblRoll.Text = Format(CInt(Int(typData.roolkei)), "###0")
                Else
                    .lblSuccess.Text = "��"
                    .lblSuccess.BackColor = Color.White
                    .lblSuccess.ForeColor = Color.Black
                    .lblPitch.Text = ""
                    .lblRoll.Text = ""
                End If


                .lblProductNo.Text = tMod_Common.ByteToString(m_typDefData.cCoilNo)
                .lblProductNo.BackColor = m_colProduct
                .lblDate.Text = String.Format("{0:0000}/{1:00}/{2:00} {3:00}:{4:00}:{5:00}",
                                              typData.time(0), typData.time(1), typData.time(2),
                                              typData.time(3), typData.time(4), typData.time(5))

            End With

            Dim nPos As Integer = 0                         '�摜�ʒu(0:��,1:��))
            DisposeImage()                                      ' �Â��摜��j������
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth) * m_dRate
                dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth) * m_dRate
                Dim imgData As Image = GetImage(m_typDefData.typ, CType(nPos, EM_IMAGE_POS), m_typDefectDsp.lblImgName(nPos).Text)
                '---------------------------------------------------------------
                '�r�摜��ݒ肷��
                '---------------------------------------------------------------
                If Not imgData Is Nothing Then                                       '�����ˉ摜���擾�ł���ΐݒ�
                    dsp.SetImgInf(imgData.Width, imgData.Height, typData.w_res, typData.h_res, typData.center_x, typData.center_y)
                    dsp.SetImage(imgData, True)
                    dsp.tProp_Paint_IsDefImgDspn = False
                    dsp.tProp_AriaInf_IsCenter = True

                    '�g��\�����̔{����ݒ�
                    'dsp.tProp_AriaInf_ZoomW = dsp.tProp_AriaInf_ZoomW * m_nImageExtend
                    'dsp.tProp_AriaInf_ZoomH = dsp.tProp_AriaInf_ZoomH * m_nImageExtend


                    dsp.Calculation()

                    '' �g�債����Ԃ��r�̊O�ژg���\���͈͂Ɏ��܂�ꍇ�́A�g�債���摜��\������
                    'Dim sz As Point = dsp.GetSimplePoint_ImgToDsp(typData.box_x_max - typData.box_x_min, typData.box_y_max - typData.box_y_min)
                    'If sz.X < m_picbox1.Width And sz.Y < m_picbox1.Height And DEFECT_ZOOM = True Then
                    '    '�g��\��ON
                    '    m_extend = True
                    'Else
                    '    '�g��\��OFF
                    '    m_extend = False
                    '    '�l��߂�
                    '    dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth)
                    '    dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth)
                    '    dsp.Calculation()
                    'End If


                    If 0 < dsp.tProp_OverLayCnt Then        '���ɃI�[�o�[���C���ݒ肳��Ă����ꍇ
                        dsp.ResetOverLayInf()
                    End If

                    '-----------------------------------------------------------
                    '�I�[�o�[���C��ݒ肷��
                    '-----------------------------------------------------------
                    Dim oly As tClass_ImageDsp.OVERLAY_INF = Nothing
                    oly.bIsDsp = m_bOverlay
                    oly.Color = g_ColLstOverlay
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
    ''' �r���\�������ŉ摜���擾�ł��Ă��Ȃ��ꍇ�̃��J�o���[����(�T�����)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspDefectRetry(ByVal nType As modMain.EM_DSP_PATTERN)

        Try
            '' �������R�[�h�̃`�F�b�N �r���I������Ă��Ȃ��ꍇ�Ȃǂ� -1���Z�b�g���Ă���
            If 0 > m_nMen Then
                '    'mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("[{0}]�������R�[�h[{1}]", TBL_HY_DEFECTINF_NAME, m_nNowRecord))
                Exit Try
            End If

            If 0 > m_nPnt Then
                '    'mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("[{0}]�������R�[�h[{1}]", TBL_HY_DEFECTINF_NAME, m_nNowRecord))
                Exit Try
            End If

            ' �\������摜��ނ��r�킩�画�肵�A��̉摜�Ɖ��ɕ\������摜�p�X�����ꂼ��擾
            Dim strPath(1) As String
            For ii As Integer = 0 To strPath.Length - 1
                Select Case CType(mcls_Param.GetTypeImage(m_typDefData.typ.data.nTid, CType(ii, EM_IMAGE_POS)), EM_IMAGE_NO)
                    Case EM_IMAGE_NO.S
                        strPath(ii) = m_typDefData.typ.image_path_s
                    Case EM_IMAGE_NO.R1
                        strPath(ii) = m_typDefData.typ.image_path_r
                    Case EM_IMAGE_NO.R2
                        strPath(ii) = m_typDefData.typ.image_path_r2
                    Case EM_IMAGE_NO.R3
                        strPath(ii) = m_typDefData.typ.image_path_r3
                    Case Else
                        ' �s��H
                        'strPath(ii) = String.Empty
                        If EM_IMAGE_POS.IMG2 = CType(ii, EM_IMAGE_POS) Then
                            strPath(ii) = m_typDefData.typ.image_path_r
                        Else
                            strPath(ii) = m_typDefData.typ.image_path_s
                        End If
                End Select
            Next

            Dim nReadAddr As Integer
            Dim nRetc As ValueType = 0


            '�r���|�C���^�ǂݍ���[���L������]
            nReadAddr = SIZE_DSP_LIST_HEADER + SIZE_DSP_LIST_DATA * m_nMen + SIZE_DSP_LIST_DEFECT * m_nPnt
            m_typDefData = CType(ksMemRead(TBL_HY_DEFLIST_NAME, nReadAddr, GetType(DSP_LIST_DEFECT), nRetc), DSP_LIST_DEFECT)
            If Not CBool(nRetc) Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]�r���ĕ\�� �r���|�C���^�ǂݍ��ݎ��s[{1}]", TBL_HY_DEFLIST_NAME, nReadAddr))
                Exit Try
            End If

            If strPath(0) = "" Or strPath(1) = "" Then
                ' �㉺�Q�摜�̂����ǂ��炩�̃p�X���擾�o���Ă��Ȃ��ꍇ�́A
                ' �ēx���L����������擾�����݂�
                ' ���L����������f�[�^�擾��A�\������摜��ނ��r�킩�画�肵�A��̉摜�Ɖ��ɕ\������摜�p�X�����ꂼ��擾
                Dim strPathNew(1) As String
                For ii As Integer = 0 To strPathNew.Length - 1
                    Select Case CType(mcls_Param.GetTypeImage(m_typDefData.typ.data.nTid, CType(ii, EM_IMAGE_POS)), EM_IMAGE_NO)
                        Case EM_IMAGE_NO.S
                            strPathNew(ii) = m_typDefData.typ.image_path_s
                        Case EM_IMAGE_NO.R1
                            strPathNew(ii) = m_typDefData.typ.image_path_r
                        Case EM_IMAGE_NO.R2
                            strPathNew(ii) = m_typDefData.typ.image_path_r2
                        Case EM_IMAGE_NO.R3
                            strPathNew(ii) = m_typDefData.typ.image_path_r3
                        Case Else
                            ' �s��H
                            If EM_IMAGE_POS.IMG2 = CType(ii, EM_IMAGE_POS) Then
                                strPathNew(ii) = m_typDefData.typ.image_path_r
                            Else
                                strPathNew(ii) = m_typDefData.typ.image_path_s
                            End If
                    End Select
                Next

                If strPath(0) <> strPathNew(0) Or strPath(1) <> strPathNew(1) Then
                    '����̃��J�o���[�����ŉ摜�p�X�̎擾���o�����̂ŁA�r���\��
                    mcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("�r�摜�\�����J�o���[[{0}]��[{1}] [{2}]��[{3}]", strPath(0), strPathNew(0), strPath(1), strPathNew(1)))
                    DspDefInf(nType)
                End If

            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���ĕ\���ُ�[{0}]", ex.Message))
        End Try


    End Sub

    ''' <summary>
    ''' �r���\��
    ''' </summary>
    ''' <param name="nMen">��[0:�\�A1:��]</param>
    ''' <param name="nPnt"></param>
    ''' <param name="typDefData">�r���</param>
    ''' <param name="col"></param>
    Public Sub DspDefect(ByVal nMen As Integer, ByVal nPnt As Integer, ByVal typDefData As DSP_LIST_DEFECT, ByVal col As Color, ByVal nType As modMain.EM_DSP_PATTERN)

        Try
            m_nMen = nMen
            m_nPnt = nPnt
            m_typDefData = typDefData
            m_colProduct = col
            '�r���\��
            DspDefInf(nType)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\��(�f�[�^�Z�b�g)�ُ�[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �\��������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit(Optional ByVal bDebug As Boolean = False)
        Try
            With m_typDefectDsp
                .pnlImage.BackColor = g_ColorControl        '�摜�p�l��
                .lblType.Text = ""

                .lblGrade.Text = ""
                .lblGrade.BackColor = g_ColorControl
                .lblPos.Text = ""
                .lblWs.Text = ""
                .lblDs.Text = ""
                .lblSpeed.Text = ""
                .lblSuccess.Text = "��"
                .lblSuccess.BackColor = Color.White
                .lblSuccess.ForeColor = Color.Black
                .lblPitch.Text = ""
                .lblRoll.Text = ""

                .lblPosTitle.Text = ""        '�r�ʒu�^�C�g��
                .lblPosTitle.BackColor = g_ColorControl
                .lblProductNo.Text = ""       '���Y�ԍ�
                .lblProductNo.BackColor = g_ColorControl
                .lblDate.Text = ""            '��������

                .lblRec.Text = ""
                .lblPnt.Text = ""
                .lblKizuNo.Text = ""
                If bDebug Then
                    .lblRec.Visible = True
                    .lblPnt.Visible = True
                    .lblKizuNo.Visible = True
                Else
                    .lblRec.Visible = False
                    .lblPnt.Visible = False
                    .lblKizuNo.Visible = False
                End If
                For ii As Integer = 0 To .lblImgName.Length - 1
                    .lblImgName(ii).Text = ""
                Next
            End With

            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                dsp.tProp_Paint_IsPaint = True
                dsp.tProp_Paint_IsDefImgDspn = True
            Next

            ' �����o�ϐ��N���A
            m_bDspData = False
            m_nMen = -1
            m_nPnt = -1
            m_typDefData.cKanriNo = ""
            m_typDefData.cCoilNo = Nothing
            m_typDefData.cMeiNo = Nothing
            m_typDefData.typ = Nothing
            m_typDefData.initialize()



            Refresh()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\���������\���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �\���X�V
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Refresh()
        Try

            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                dsp.Refresh()
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���\���X�V�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�g��摜�\��
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Function ShowPopup(ByVal clsPopup As frmDefectImage) As Boolean
        Dim bRet As Boolean = False
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

            '�摜�͑��݂��Ȃ��ꍇ������̂ŕ\�����̉摜���g�p
            For ii As Integer = 0 To clsImg.Length - 1
                If IsDspDefault(ii) Then
                    clsImg(ii) = Nothing
                Else
                    clsImg(ii) = DspImage(ii)
                End If
                tmpTitle(ii) = DspImgTitle(ii)
            Next

            '---------------------------
            '�r�摜�g��\��
            '---------------------------
            With clsPopup
                .DispImgNo = m_typDefectDsp.nNo                                 '�摜No.
                .SetVisibleMemory(MemoriVisible)                                '�������̕\���L���ݒ�
                .SetVisibleOverlay(OverlayVisible)                              '�I�[�o�[���C�̕\���L���ݒ�
                .SetColor = g_ColLstOverlay                                     '�������A�I�[�o���C�F
                .IsTurnLR = IsTurnLR                                            '���E���]�ݒ�
                .IsTurnTB = IsTurnTB                                            '�㉺���]�ݒ�
                .IsRotate90 = IsTurn90                                          '90�x��]�ݒ�()
                .Side = Side                                                    '�r�摜���x��DS�ʒu
                ''�r���
                .SetDefectData(m_typDefData, m_typDefData.typ.data, clsImg, tmpTitle)
                '�\���{��
                .ZoomRate = CInt((100.0 * m_typDefData.typ.data.w_res / m_nImageWidth) * 100.0)
                '�ڐ���`��ڍ�
                .SetMemoryConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X * 2, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y * 2)
                .DspUpDate(True)                                                '�\��

                bRet = True
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�g��摜�\���ُ�[{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �r�摜�`��I���C�x���g
    ''' </summary>
    ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
    ''' <remarks></remarks>
    Private Sub ImgDsp_tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim ii As Integer
        Dim nPosLineTop As Integer
        Dim nLineWidth As Integer

        Dim typAriaInf As tClass_ImageDsp.ARIA_INF                     ' �\���̈���
        Dim typCalcAns As tClass_ImageDsp.CALC_ANS                     ' ���W�ŏI����
        Dim typMemori As tClass_ImageDsp.MEMORI_INF                    ' ���������
        Dim clsPen As Pen

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

            If (MemoriVisible = True) Then
                clsPen = New System.Drawing.Pen(typMemori.Color, nLineWidth)
                '�w���\��
                For ii = 1 To typMemori.nLoopCntX * typMemori.nIntervalX
                    If ii Mod (typMemori.nIntervalX \ 5) = 0 Then
                        Dim nPos As Integer = typMemori.nPosLineLeft + CInt(ii * typCalcAns.RateRes.dW)
                        '��؂��
                        e.Graphics.DrawLine(clsPen, nPos, nPosLineTop, nPos, nPosLineTop - CInt(typMemori.HojoLineLen / 2))
                    End If
                Next ii

                '�x���\��
                For ii = 1 To typMemori.nLoopCntY * typMemori.nIntervalY
                    If ii Mod (typMemori.nIntervalY \ 5) = 0 Then
                        Dim nPos As Integer = nPosLineTop - CInt(ii * typCalcAns.RateRes.dH)
                        '��؂��
                        e.Graphics.DrawLine(clsPen, typMemori.nPosLineLeft, nPos, typMemori.nPosLineLeft + CInt(typMemori.HojoLineLen / 2), nPos)
                    End If
                Next ii
                clsPen.Dispose()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�ڍזڐ��`��ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �ŐV�摜�g�\��
    ''' </summary>
    ''' <param name="colValue">�F</param>
    ''' <remarks></remarks>
    Public Sub ChangePnlImage(ByVal colValue As Color)

        '�I�u�W�F�N�g���Ȃ��ꍇ�́A�I��
        If m_typDefectDsp.pnlImage Is Nothing Then Exit Sub

        '�w�i�F�\��
        m_typDefectDsp.pnlImage.BackColor = colValue
    End Sub


    ''' <summary>
    ''' ������(�Oor��),���b�N�����x���\��
    ''' </summary>
    ''' <param name="strValue"></param>
    ''' <remarks></remarks>
    Public Sub SetKensaLabel(ByVal strValue As String, ByVal colValue As Color)

        '�I�u�W�F�N�g���Ȃ��ꍇ�́A�I��
        If m_typDefectDsp.lblPosTitle Is Nothing Then Exit Sub

        '�f�[�^�\��
        m_typDefectDsp.lblPosTitle.Text = strValue
        m_typDefectDsp.lblPosTitle.BackColor = colValue

    End Sub

End Class
