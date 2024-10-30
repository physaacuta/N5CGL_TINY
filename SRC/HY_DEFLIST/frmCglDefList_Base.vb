'*********************************************************************************
'�@�r�ꗗ��ʊ��N���X
'	[Ver]
'		Ver.01    2008/07/25  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports KizuLibCLI
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class frmCglDefList_Base
    '///////////////////////////////////////////////////////////////////////////
    '�萔
    '///////////////////////////////////////////////////////////////////////////
    Private Const PATH_DEFAULT_IMG As String = "HY_DEFLIST.�r����.bmp"         '�f�t�H���g�摜�̃p�X
    Private Const DEFAULT_GAMEN_SIZE_X As Integer = 1280
    Private Const DEFAULT_GAMEN_SIZE_Y As Integer = 1024

    Private Const DEFLIST_DISP_DEFECT_MAX As Integer = 25                      '�\���ő��r��

    Public Structure TYP_DSP_INDEX
        Dim strKanriNo As String    ' �Ǘ�No
        Dim nKizuSeq As Integer     ' �r�A��
        Dim nspmode As Integer      ' �����\���Ώۃt���O   (0:�����g����   1:�����g�\��)   �p�g���C�g�o�͑Ώ��r
        '�C�j�V�����C�Y
        Public Sub initialize()
            strKanriNo = ""
            nKizuSeq = 0
            nspmode = 0
        End Sub
    End Structure

    '�r�摜�\���ʒu���\����
    Private Structure IMAGE_DSP_POS_INF
        Dim nRealPoint As Integer   '�\�����Ă����r�ꗗ�̃|�C���^
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f���Q�[�g�֐�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Shadows Event DspDefList(ByVal nDspPattern As modMain.EM_DSP_PATTERN, ByVal nOuDiv As Integer)        '�T����ʕ\��
    Private Delegate Sub m_deleChangeParam(ByVal nMode As Integer)  '�ʃX���b�h����̃R���g���[���A�N�Z�X(�p�����[�^�ēǂݍ���)
    Private Delegate Sub m_deleDspInit()                            '�ʃX���b�h����̃R���g���[���A�N�Z�X(��ʏ���������)
    Private Delegate Sub m_deleExit()                               '�ʃX���b�h����̃R���g���[���A�N�Z�X(�^�X�N�I������)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass_LogManager                           '���O�Ǘ�
    Private mcls_ParamManager As clsParamManager                    '�p�����[�^�Ǘ�
    Private mcls_ProductNoBackColor As clsProductBackColor          '���Y�ԍ��w�i�F�ݒ�N���X
    Private mfrm_PopImage As frmDefectImage                         '�r�g��摜�\��
    Private mcls_KizuInf() As clsKizuInf                            '�r���N���X
    Private WithEvents mfrm_setting As frmSetting                   '�\���ݒ�|�b�v�A�b�v���
    '///////////////////////////////////////////////////////////////////////////
    '�����o�[�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_nDspMode As Integer                                   '�\���敪0:�\ 1:��
    Private m_nMode As Integer = -1                                 '�\�����[�h (-1:�������, 0:����,1:�ꎞ��~)
    Private m_bDebug As Boolean = False                             '�f�o�b�O���[�h
    Private m_bChangeParam As Boolean                               '�p�����[�^�Z�b�g���t���O
    Private m_nPCDiv As Integer                                     'PC�敪(0:�\�ʌ�����,1:���ʌ�����)
    Private m_nImagePattern As EM_DSP_PATTERN                       '�摜�\���p�^�[��
    Private m_typDefInf As TBL_HY_DEFLIST = Nothing                 '�r���ꗗ�e�[�u��
    Private m_nStop_newrec As Integer                               '�ꎞ��~�����Ƃ��̍ŐV���R�[�hNO
    Private m_nImageWidth As Integer = 25                           '�\���G���A�̉���[mm]
    Private m_nImageRate As Integer = 100                           '�\���{���i100�F1�{�j

    Private m_nMonitor As Integer                                   '��ʔԍ�

    Private m_lstKizuInf As List(Of clsKizuInf.TYP_DEFECT_DSP)      '�r��񃊃X�g
    Private m_typDspIndex() As TYP_DSP_INDEX

    Private m_typImageDspPosInf As IMAGE_DSP_POS_INF                '�ŐV���r�摜�\���ʒu���

    Private m_nPageNo As Integer                                    '�ꎞ��~�y�[�W��

    '///////////////////////////////////////////////////////////////////////////
    '�ǉ��C�x���g
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �ꎞ��~�ύX
    ''' </summary>
    ''' <param name="nOuDiv">��(0:�\�A1:��)</param>
    ''' <param name="nStop"></param>
    Public Event StopSelectedIndexChanged(ByVal nOuDiv As Integer, ByVal nStop As Integer)

    ''' <summary>
    ''' �|�b�v�A�b�v�ύX
    ''' </summary>
    ''' <param name="nOuDiv">��(0:�\�A1:��)</param>
    ''' <param name="nPopUp"></param>
    Public Event PopSelectedIndexChanged(ByVal nOuDiv As Integer, ByVal nPopUp As Integer)

    ''' <summary>
    ''' �\�����[�h�`�F���W
    ''' </summary>
    ''' <param name="nOuDiv">��(0:�\�A1:��)</param>
    ''' <param name="nPattern"></param>
    Public Event ModeClickEvent(ByVal nOuDiv As Integer, ByVal nPattern As modMain.EM_DSP_PATTERN)

    ''' <summary>
    ''' �I�[�o���C�\���`�F���W
    ''' <param name="bVisible">�ύX��̕\�� True:�\�� False:��\��</param>
    ''' </summary>
    Public Event ev_ChangeOverlyVisible(ByVal bVisible As Boolean)

    ''' <summary>
    ''' �ڐ���\���ύX
    ''' </summary>
    ''' <param name="bVisible">�ύX��̕\�� True:�\�� False:��\��</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeMemoriVisible(ByVal bVisible As Boolean)

    Public Event ev_ChangeFilter(ByVal nVal As Integer)

    '///////////////////////////////////////////////////////////////////////////
    '���\�b�h
    '///////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        '���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()
        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

    End Sub

    ''' <summary>
    ''' �I�u�W�F�N�g�̔j��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCglDefList_Base_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        SetOverlay()
        SetScale()
    End Sub

    ''' <summary>
    ''' �r���R���g���[�����Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetKizuInf(ByRef lstKizuInf As List(Of clsKizuInf.TYP_DEFECT_DSP))

        '�f�[�^�Z�b�g
        m_lstKizuInf = lstKizuInf

    End Sub

    ''' <summary>
    ''' �r���N���X����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateKizu()

        Dim ii As Integer

        ReDim mcls_KizuInf(m_lstKizuInf.Count - 1)

        For ii = 0 To m_lstKizuInf.Count - 1

            mcls_KizuInf(ii) = New clsKizuInf(m_lstKizuInf(ii), mcls_ParamManager, mcls_Log, PATH_DEFAULT_IMG)

            ''�s�N�`���[�̃C�x���g���Z�b�g
            AddHandler m_lstKizuInf(ii).picImg(0).DoubleClick, AddressOf picImg_DoubleClick
            AddHandler m_lstKizuInf(ii).picImg(1).DoubleClick, AddressOf picImg_DoubleClick

        Next ii

        For Each dsp As clsKizuInf In mcls_KizuInf
            dsp.ImageWidth = m_nImageWidth
            dsp.DspRate = CSng(m_nImageRate / 100)
        Next

        m_lstKizuInf.Clear()
        m_lstKizuInf = Nothing

    End Sub

    ''' <summary>
    ''' �t�H�[�������ݒ�
    ''' </summary>
    ''' <param name="nDspMode">�\���敪(0:�\,1:��)</param>
    ''' <param name="nMyID">���j�^�ԍ�(1�`)</param>
    ''' <param name="clsLog">���O�N���X</param>
    ''' <param name="clsParam">�p�����[�^�N���X</param>
    ''' <param name="clsPnoBGC">���Y�ԍ��F�ς��N���X</param>
    ''' <param name="nImageWidth">�\���r�摜��`(�摜100pixel�����ݒ�[mm])</param>
    ''' <param name="nImageRate">�\���{��(1�{�̎�100�A2�{��200)</param>
    ''' <param name="nPattern">�\���p�^�[��</param>
    Public Sub DspInitForm(ByVal nDspMode As Integer, ByVal nMyID As Integer, ByRef clsLog As tClass.tClass_LogManager, ByRef clsParam As clsParamManager,
                           ByVal clsPnoBGC As clsProductBackColor, ByVal nImageWidth As Integer, ByVal nImageRate As Integer, ByVal nPattern As modMain.EM_DSP_PATTERN)
        Try

            Dim ii As Integer = 0    '���[�v�J�E���^

            Dim monitor() As System.Windows.Forms.Screen
            monitor = System.Windows.Forms.Screen.AllScreens()
            Me.Bounds = monitor(nMyID - 1).Bounds

            Me.Width = DEFAULT_GAMEN_SIZE_X
            Me.Height = DEFAULT_GAMEN_SIZE_Y

            '�����o�[�ϐ��ɃZ�b�g
            m_nDspMode = nDspMode
            mcls_Log = clsLog
            mcls_ParamManager = clsParam
            mcls_ProductNoBackColor = clsPnoBGC
            m_nImagePattern = nPattern
            m_nImageWidth = nImageWidth
            m_nImageRate = nImageRate


            ReDim m_typDspIndex(m_lstKizuInf.Count - 1)
            For ii = 0 To m_lstKizuInf.Count - 1
                m_typDspIndex(ii).initialize()
            Next

            '-----------------------------------------------
            '�\���ݒ���
            '-----------------------------------------------
            mfrm_setting = New frmSetting()

            '-----------------------------------------------
            '�r�g��摜���
            '-----------------------------------------------
            ' �^�X�N��(HY_TANSHO) ����ini���Z�N�V��������
            Dim strSection As String = KizuMod.GetMyTaskName()
            Dim bPicDirect As Boolean = CBool(tMod.tMod_IniFile.ReadIniInt(strSection, "DEFIMAGE_DIRECT", TASKINI_NAME, 0))
            mfrm_PopImage = New frmDefectImage(clsLog, mcls_ParamManager, PATH_DEFAULT_IMG, strSection, DSP_KIRI_IMAGE_MAX, bPicDirect)
            mfrm_PopImage.Owner = Me

            '�r���N���X�o�^
            Call CreateKizu()

            '�����\���ݒ�
            Call DspInitProcess()

            If 0 = m_nDspMode Then
                mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�\�� �d���׈ꗗ��ʁ@�p�^�[��[" & m_nImagePattern & "]")
            Else
                mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "���� �d���׈ꗗ���  �p�^�[��[" & m_nImagePattern & "]")
            End If

            '�����X�V���[�h
            Call ChangeMode()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�t�H�[�������ݒ�[{0}][{1}][{2}][{3}]", nDspMode, nMyID, nPattern, ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInitProcess()

        Dim ii As Integer = 0    '���[�v�J�E���^

        '�\�����
        If 0 = m_nDspMode Then
            lblFace.Text = "�\��"
            lblFace.ForeColor = Color.Yellow
            lblFace.BackColor = Color.Blue
        Else
            lblFace.Text = "����"
            lblFace.ForeColor = Color.Black
            lblFace.BackColor = Color.Orange
        End If

        '�f�o�b�O���擾
        m_bDebug = CBool(IIf(0 = tMod.ReadIniInt(HY_DEFLIST, "DEBUG", TASKINI_NAME), False, True))

        If m_bDebug Then

            '�I�t���C��
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            'Me.TopMost = True
            Me.TopMost = False
        Else

            '�I�����C��
            Me.tProp_FromFormatMode = gem_FromFormatMode.iONLINE
        End If
        Me.ControlBox = False
        Me.DoubleBuffered = True

        '������r���擾�^�C�}�[
        Dim nTimer As Integer = tMod.ReadIniInt(HY_DEFLIST, "DSP_INTERVAL", TASKINI_NAME, 2000)
        If 0 >= nTimer Then             ' �ُ�l
            timReal.Interval = 2000     ' �f�t�H���g�l��ݒ�
        Else
            timReal.Interval = nTimer
        End If

        timReal.Enabled = True

        If 0 = m_nDspMode Then

            '�r�摜���x��DS�ʒu
            Dim nImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_DIRECT_TOP", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '����DS
                Case EM_MAP_SIDE.REVERSE                                        '����WS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜���x��DS�ʒu�ُ�[{0}],[{1}]", m_nDspMode, nImgSide))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.Side = nImgSide
            Next

            '�r�摜�㉺���]
            Dim nImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_UP_TOP", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '���]����
                Case EM_IMAGE_TOP.REVERSE                                       '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�㉺���]�ُ�[{0}][{1}]", m_nDspMode, nImgVMode))
                    nImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnTB = nImgVMode
            Next

            '�r�摜���E���]
            Dim nImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_SIDE_TOP", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '���]����
                Case EM_IMAGE_SIDE.REVERSE                                      '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜���E���]�ُ�[{0}[{1}]]", m_nDspMode, nImgHMode))
                    nImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnLR = nImgHMode
            Next

            '��ʂŐݒ肵���|�b�v�A�b�v�̃^�C�}�[�̒l
            mfrm_setting.PopUpCloseTime = tMod.ReadIniInt(HY_DEFLIST, "POP_TIMER_TOP", LOCALINI_NAME)

            '��ʂŐݒ肵���ꎞ��~�̃^�C�}�[�̒l
            mfrm_setting.StopCloseTime = tMod.ReadIniInt(HY_DEFLIST, "STOP_TIMER_TOP", LOCALINI_NAME)

            '��ʂŐݒ肵���I�[�o�[���C�̒l
            Dim nOverLay As Integer = tMod.ReadIniInt(HY_DEFLIST, "OVERLAY_VIEW_TOP", LOCALINI_NAME)
            If 1 = nOverLay Then
                Overlay = True
            Else
                Overlay = False
            End If

            '��ʂŐݒ肵���X�P�[���̒l
            Dim nScale As Integer = tMod.ReadIniInt(HY_DEFLIST, "SCALE_VIEW_TOP", LOCALINI_NAME)
            If 1 = nScale Then
                Memori = True
            Else
                Memori = False
            End If
        Else

            '�r�摜���x��DS�ʒu
            Dim nImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_DIRECT_BOT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '����DS
                Case EM_MAP_SIDE.REVERSE                                        '����WS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜���x��DS�ʒu�ُ�[{0}],[{1}]", m_nDspMode, nImgSide))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.Side = nImgSide
            Next

            '�r�摜�㉺���]
            Dim nImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_UP_BOT", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '���]����
                Case EM_IMAGE_TOP.REVERSE                                       '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�㉺���]�ُ�[{0}][{1}]", m_nDspMode, nImgVMode))
                    nImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnTB = nImgVMode
            Next

            '�r�摜���E���]
            Dim nImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_SIDE_BOT", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '���]����
                Case EM_IMAGE_SIDE.REVERSE                                      '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜���E���]�ُ�[{0}][{1}]", m_nDspMode, nImgHMode))
                    nImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.IsTurnLR = nImgHMode
            Next

            '��ʂŐݒ肵���|�b�v�A�b�v�̃^�C�}�[�̒l
            mfrm_setting.PopUpCloseTime = tMod.ReadIniInt(HY_DEFLIST, "POP_TIMER_BOT", LOCALINI_NAME)

            '��ʂŐݒ肵���ꎞ��~�̃^�C�}�[�̒l
            mfrm_setting.StopCloseTime = tMod.ReadIniInt(HY_DEFLIST, "STOP_TIMER_BOT", LOCALINI_NAME)

            '��ʂŐݒ肵���I�[�o�[���C�̒l
            Dim nOverLay As Integer = tMod.ReadIniInt(HY_DEFLIST, "OVERLAY_VIEW_BOT", LOCALINI_NAME)
            If 1 = nOverLay Then
                Overlay = True
            Else
                Overlay = False
            End If

            '��ʂŐݒ肵���X�P�[���̒l
            Dim nScale As Integer = tMod.ReadIniInt(HY_DEFLIST, "SCALE_VIEW_BOT", LOCALINI_NAME)
            If 1 = nScale Then
                Memori = True
            Else
                Memori = False
            End If
        End If

        ''PC�敪
        m_nPCDiv = ReadIniInt(HY_DEFLIST, "PCDIV", TASKINI_NAME)

        '��ʃ^�C�g��
        Me.Text = CStr(IIf(0 = m_nPCDiv, "�\������ �d���׈ꗗ���", "�������� �d���׈ꗗ���"))
        Call SetTitleName(Me)

        '�y�[�W�ݒ�
        m_nPageNo = 1
        btnF01.Enabled = False
        btnF02.Enabled = False
    End Sub

    ''' <summary>
    ''' ����������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()

        Dim ii As Integer

        '�r��񏉊���
        For ii = 0 To UBound(mcls_KizuInf)
            mcls_KizuInf(ii).DspInit(m_bDebug)
        Next ii

        '�r�摜�\���ʒu��񏉊���
        m_typImageDspPosInf.nRealPoint = -1

        m_nStop_newrec = 0
    End Sub

    ''' <summary>
    ''' �I�[�o�[���C�\���ݒ�ON/OFF
    ''' </summary>
    ''' <value>True�F�\���AFalse�F��\��</value>
    ''' <returns>True�F�\���AFalse�F��\��</returns>
    ''' <remarks></remarks>
    Public Property Overlay() As Boolean
        Get
            Return mcls_KizuInf(0).OverlayVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect �̃I�[�o�[���C�\����ݒ�
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.OverlayVisible = value
            Next

            ' �g��摜 �̃I�[�o�[���C�\����ݒ�
            mfrm_PopImage.SetVisibleOverlay(value)

            ' ���̑��ύX����Ώۂ�����΁A�����ɋL�q����

            ' �ؑփ{�^���̐F�ύX����
            If value Then
                ' �\�����̃{�^���F�ݒ�
                btnF07.BackColor = Color.Cyan
            Else
                ' ��\�����̃{�^���F�ݒ�
                btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)
            End If

        End Set
    End Property


    ''' <summary>
    ''' �ڐ���\���ݒ�ON/OFF
    ''' </summary>
    ''' <value>True�F�\���AFalse�F��\��</value>
    ''' <returns>True�F�\���AFalse�F��\��</returns>
    ''' <remarks></remarks>
    Public Property Memori() As Boolean
        Get
            Return mcls_KizuInf(0).MemoriVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect �̖ڐ���\����ݒ�
            For Each dsp As clsKizuInf In mcls_KizuInf
                dsp.MemoriVisible = value
            Next

            ' �g��摜 �̖ڐ���\����ݒ�
            mfrm_PopImage.SetVisibleMemory(value)

            ' ���̑��ύX����Ώۂ�����΁A�����ɋL�q����

            ' �ؑփ{�^���̐F�ύX����
            If value Then
                ' �\�����̃{�^���F�ݒ�
                btnF08.BackColor = Color.Cyan
            Else
                ' ��\�����̃{�^���F�ݒ�
                btnF08.BackColor = Color.FromKnownColor(KnownColor.Control)
            End If

        End Set
    End Property

    ''' <summary>
    ''' �`�F�b�N�{�b�N�X �w�i�F�ύX
    ''' </summary>
    ''' <param name="chk">�`�F�b�N�{�b�N�X</param>
    ''' <remarks></remarks>
    Private Sub SetChkBackColor_Change(ByVal chk As CheckBox)
        If chk.Checked Then                 ' ON
            chk.BackColor = Color.Cyan
        Else                                ' OFF
            chk.BackColor = Drawing.SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' �ꎞ��~�^�C�}�[�ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetStopTime()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "STOP_TIMER_TOP", "STOP_TIMER_BOT"))
        modMain.WriteIniData(strKey, CStr(mfrm_setting.StopCloseTime))

    End Sub

    ''' <summary>
    ''' �|�b�v�A�b�v�^�C�}�[�ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetPopUpTime()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "POP_TIMER_TOP", "POP_TIMER_BOT"))
        modMain.WriteIniData(strKey, CStr(mfrm_setting.PopUpCloseTime))

    End Sub

    ''' <summary>
    ''' �I�[�o�[���C�ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetOverlay()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "OVERLAY_VIEW_TOP", "OVERLAY_VIEW_BOT"))
        Dim strValue As String = IIf(Overlay, "1", "0")
        modMain.WriteIniData(strKey, strValue)

    End Sub

    ''' <summary>
    ''' �X�P�[���ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetScale()

        Dim strKey As String = CStr(IIf(0 = m_nDspMode, "SCALE_VIEW_TOP", "SCALE_VIEW_BOT"))
        Dim strValue As String = IIf(Memori, "1", "0")
        modMain.WriteIniData(strKey, strValue)

    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X�Ŏw�肵���l���Y������^�C�}�[�ɃZ�b�g���A�N������
    ''' </summary>
    ''' <param name="cmb">�l��ݒ肷��R���{�{�b�N�X</param>
    ''' <param name="tim">�l�𔽉f���������^�C�}�[�̃I�u�W�F�N�g</param>
    ''' <remarks></remarks>
    Private Sub TimerSet(ByVal cmb As ComboBox, ByVal tim As System.Windows.Forms.Timer)

        '�^�C�}�[�ݒ�
        If 0 < CType(cmb.Items(cmb.SelectedIndex), frmSetting.cls_CmbItem).time Then
            tim.Interval = CType(cmb.Items(cmb.SelectedIndex), frmSetting.cls_CmbItem).time * 1000

            If tim.Enabled = False Then
                tim.Enabled = True
            Else
                tim.Start()
            End If
        End If

    End Sub

    ''' <summary>
    ''' �r���擾�^�C�}�[
    ''' </summary>
    Private Sub tmrReal_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timReal.Tick

        '��ʍX�V����
        Call DspPointGet()
        If m_nMode = EM_REFRESH_MODE.AUTO Then
            Call DspData(False)
        End If

    End Sub

    ''' <summary>
    ''' �ꎞ��~�����^�C�}�[
    ''' </summary>
    Private Sub tmrStop_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timStop.Tick

        '�������[�h��
        Call ChangeMode()

    End Sub


    ''' <summary>
    ''' �ꎞ��~�����^�C�}�[
    ''' </summary>
    Private Sub tmrPopUp_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timPopUp.Tick

        '�r�g��摜��ʔ�\��
        Call ClosePopUp()

    End Sub

    Private Sub ChangeSetting()
        '�ꎞ��~�^�C�}�[�ݒ�
        Call SetStopTime()
        '�S�p�^�[��
        RaiseEvent StopSelectedIndexChanged(m_nDspMode, mfrm_setting.StopCloseTime)

        '�|�b�v�A�b�v�^�C�}�[�ݒ�
        Call SetPopUpTime()
        '�S�p�^�[��
        RaiseEvent PopSelectedIndexChanged(m_nDspMode, mfrm_setting.PopUpCloseTime)

    End Sub

    ''' <summary>
    ''' ���[�h�\���ؑ�
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ChangeMode()

        If EM_REFRESH_MODE.AUTO = m_nMode Then

            ''�����X�V���ꎞ��~
            btnF05.Text = "�ꎞ��~" & vbNewLine & "����"
            btnF05.BackColor = g_ColorControl
            btnF05.ForeColor = Color.Black

            lblMode.Text = "�ꎞ��~��"
            lblMode.BackColor = Color.Red
            lblMode.ForeColor = Color.White

            '�O�A���y�[�W�{�^�������ݒ�
            m_nPageNo = 1

            '���y�[�W�̕\�����\��
            Dim dPageMax As Decimal = CDec(DEFLIST_DISP_DEFECT_MAX / mcls_KizuInf.Length)
            Dim bNextPageFlg As Boolean = NextPageCheck(m_nPageNo + 1, CInt(dPageMax)) '���݂̃y�[�W����Q�y�[�W��ȍ~���r��񂪂��邩
            If HY_DEFLIST_REC_MAX > mcls_KizuInf.Length AndAlso bNextPageFlg Then
                btnF01.Enabled = False
                btnF02.Enabled = True
            Else
                btnF01.Enabled = False
                btnF02.Enabled = False
            End If

            '�ꎞ��~�����^�C�}�[�ݒ�
            Call TimerSet(mfrm_setting.cmbStop, timStop)
            '�ꎞ��~��
            m_nMode = EM_REFRESH_MODE.PAUSE
            '������r���擾�^�C�}�[��~
            timReal.Enabled = False
            '�\�������r���̃|�C���^���擾����
            Call DspPointGet()
            ''��ʍX�V����
            Call DspData()
            '�u�����N�^�C�}�[�J�n
            timMode.Enabled = True
            timMode.Start()

        Else

            '�ꎞ��~�������X�V
            timMode.Stop()
            btnF05.Text = "�ꎞ��~"
            btnF05.BackColor = g_ColorControl
            btnF05.ForeColor = Color.Black

            lblMode.Text = "�����X�V��"
            lblMode.BackColor = Color.Blue
            lblMode.ForeColor = Color.Yellow

            '�O�A���y�[�W�{�^�������ݒ�
            m_nPageNo = 1
            btnF01.Enabled = False
            btnF02.Enabled = False

            '�r��񏉊���
            DspInit()

            '�ꎞ��~�����^�C�}�[�I��
            timStop.Enabled = False
            '�����X�V��
            m_nMode = EM_REFRESH_MODE.AUTO
            '�\�������r���̃|�C���^���擾����
            Call DspPointGet()
            '��ʍX�V����
            Call DspData()
            '������r���擾�^�C�}�[�J�n
            timReal.Enabled = True
            '�u�����N�^�C�}�[��~
            timMode.Enabled = False

        End If
    End Sub


    ''' <summary>
    ''' �r�g��摜��ʔ�\��
    ''' </summary>
    Private Sub ClosePopUp()
        If mfrm_PopImage.Visible Then                               '�r�摜�g��\����
            mfrm_PopImage.CloseFrom()                               '�g��摜OFF��
        End If
    End Sub

    ''' <summary>
    ''' �\�������r���̃|�C���^���擾����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspPointGet()

        Try
            '�r���ꗗ�e�[�u���ǂݍ��݁B
            m_typDefInf = CType(KizuLibCLI.KizuMod.ksMemRead(TBL_HY_DEFLIST_NAME, GetType(TBL_HY_DEFLIST)), TBL_HY_DEFLIST)

            '���Y�ԍ��w�i�F�R�t��
            mcls_ProductNoBackColor.ReadPnoBGinit()

            '�����X�V���̓��R�[�hNO��ێ�����
            If EM_REFRESH_MODE.PAUSE <> m_nMode Then
                m_nStop_newrec = m_typDefInf.rec.head.nPos(m_nDspMode)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r��|�C���^�擾�G���[ [{0}]", ex.Message))
        End Try

    End Sub


    ''' <summary>
    ''' ��ʍX�V����
    ''' </summary>
    ''' <param name="bUpdateFlg">�����ĕ`��t���O</param>
    ''' <remarks></remarks>
    Private Sub DspData(Optional ByVal bUpdateFlg As Boolean = True)

        Dim ii As Integer                           '�J�E���^
        Dim nRecNo As Integer                       '���R�[�h�ʒu
        Dim nIndex As Integer = 0                   '�r�摜�\���C���f�b�N�X
        Dim nPnt As Integer = 0                     '�\���J�n�|�C���^
        Dim nRecNoChk As Integer = 0                '�ŐV���R�[�h�Z�b�g
        Dim nIndexChk As Integer = 0                '�摜�\���ŐV�C���f�b�N�X�Z�b�g
        Dim nDsipCnt As Integer = 0                 '��ʕ\���r���

        Try
            '���R�[�h���Ȃ���ΏI��
            If 0 = m_typDefInf.rec.head.nPos(m_nDspMode) Then

                '�f�[�^�����Ȃ珉����
                For ii = 0 To mcls_KizuInf.Length - 1
                    m_typDspIndex(ii).initialize()
                    mcls_KizuInf(ii).DspInit(m_bDebug)
                    mcls_KizuInf(ii).ChangePnlImage(g_ColorControl)
                Next ii

                '�r�摜�\���ʒu��񏉊���
                m_typImageDspPosInf.nRealPoint = -1

                Exit Sub
            End If

            '�ꎞ��~����
            If EM_REFRESH_MODE.PAUSE <> m_nMode Then
                '�����X�V���̓|�C���^���X�V���A�r�ꗗ���e�[�u���̍ŐV���R�[�h�Ńf�[�^�i�[���R�[�h���Z�o����
                nRecNo = (m_typDefInf.rec.head.nPos(m_nDspMode) - 1)
            Else
                ''�ꎞ��~���͈ꎞ��~�������̍ŐV���R�[�h�Ńf�[�^�i�[���R�[�h���Z�o����
                nRecNo = m_nStop_newrec - 1
            End If

            If nRecNo = -1 Then
                nRecNo = HY_DEFLIST_REC_MAX - 1
            End If

            ''�ŐV���R�[�h�X�V���� or �C�ӌďo��
            If m_typImageDspPosInf.nRealPoint <> nRecNo OrElse bUpdateFlg = True Then

                '�\���f�[�^�����擾
                nDsipCnt = mcls_KizuInf.Length * (m_nPageNo - 1)

                '�J�n�|�C���^��ݒ�
                nPnt = nRecNo - nDsipCnt
                If nPnt < 0 Then nPnt = HY_DEFLIST_REC_MAX - Math.Abs(nPnt)

                '��ʂɕ\�������r��񕪃��[�v�i��ʍ��[�r��񗓂��珇�ɍŐV���r�����i�[���Ă����j
                For nIndex = 0 To mcls_KizuInf.Length - 1

                    Dim typData As DSP_LIST_DEFECT = m_typDefInf.rec.data(m_nDspMode).DfData(nPnt)

                    If 0 < typData.typ.data.nKizuNo And nDsipCnt < HY_DEFLIST_REC_MAX Then

                        '�r���\��
                        Dim col As Color = mcls_ProductNoBackColor.SetPNoBackColor(typData.cKanriNo)
                        mcls_KizuInf(nIndex).DspDefect(m_nDspMode, nPnt, typData, col, m_nImagePattern)

                        '�\�����Z�b�g
                        m_typDspIndex(nIndex).strKanriNo = typData.cKanriNo
                        m_typDspIndex(nIndex).nKizuSeq = typData.typ.data.nKizuNo
                        m_typDspIndex(nIndex).nspmode = typData.typ.data.nModeDeflist
                        '�����g�\��
                        If 1 = m_typDspIndex(nIndex).nspmode Then
                            mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstSp)
                        Else
                            mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                        End If

                        '�ŐV�|�C���^�ɂȂ�����A�\���r�摜���X�V
                        If nPnt = nRecNo Then
                            m_typImageDspPosInf.nRealPoint = nRecNo
                            If 1 = m_typDspIndex(nIndex).nspmode Then
                                mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewSp)
                            Else
                                mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewKizu)
                            End If

                        End If

                    Else
                        '�f�[�^�Ȃ�(������)
                        mcls_KizuInf(nIndex).DspInit(m_bDebug)
                        mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                        m_typDspIndex(nIndex).initialize()
                    End If

                    '�V�����r��񂩂��ʂɐݒ肷��ׁA1�O���r�����擾
                    nPnt -= 1
                    If nPnt < 0 Then nPnt = HY_DEFLIST_REC_MAX - 1

                    '�\���f�[�^�����Z
                    nDsipCnt += 1
                Next nIndex
            End If

            '�r�摜�\���ɕω�����������r
            nRecNoChk = m_typImageDspPosInf.nRealPoint   '�ŐV���R�[�h�Z�b�g�i0�I���W���j
            If 0 <= nRecNoChk AndAlso EM_REFRESH_MODE.AUTO = m_nMode Then
                For nIndex = 0 To mcls_KizuInf.Length - 1
                    '�f�[�^�L��
                    Dim typData As DSP_LIST_DEFECT = m_typDefInf.rec.data(m_nDspMode).DfData(nRecNoChk)
                    If 0 < typData.typ.data.nKizuNo Then

                        '�\�����Ă����r�ƃe�[�u���̊Y�����Ԃ��r���������H
                        If m_typDspIndex(nIndex).strKanriNo = typData.cKanriNo And
                               m_typDspIndex(nIndex).nKizuSeq = typData.typ.data.nKizuNo And
                               m_typDspIndex(nIndex).nspmode = typData.typ.data.nModeDeflist Then
                        Else

                            '�r���\��
                            Dim col As Color = mcls_ProductNoBackColor.SetPNoBackColor(typData.cKanriNo)
                            mcls_KizuInf(nIndex).DspDefect(m_nDspMode, nRecNoChk, typData, col, m_nImagePattern)

                            '�\�����Z�b�g
                            m_typDspIndex(nIndex).strKanriNo = typData.cKanriNo
                            m_typDspIndex(nIndex).nKizuSeq = typData.typ.data.nKizuNo
                            m_typDspIndex(nIndex).nspmode = typData.typ.data.nModeDeflist

                            If m_typImageDspPosInf.nRealPoint = nRecNoChk Then
                                '�����g�\��
                                If 1 = typData.typ.data.nModeDeflist Then
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewSp)
                                Else
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstNewKizu)
                                End If
                            Else
                                '�����g�\��
                                If 1 = typData.typ.data.nModeDeflist Then
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColLstSp)
                                Else
                                    mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                                End If
                            End If
                        End If
                    Else
                        '�f�[�^�Ȃ�(������)
                        mcls_KizuInf(nIndex).DspInit()
                        mcls_KizuInf(nIndex).ChangePnlImage(g_ColorControl)
                        m_typDspIndex(nIndex).initialize()
                    End If

                    nRecNoChk -= 1
                    If 0 > nRecNoChk Then nRecNoChk = HY_DEFLIST_REC_MAX - 1

                Next nIndex

            End If

            '������ʒu���菈��
            Dim strBaseKanriNo As String = ""
            Dim nBaseLen As Integer = 0
            '������ʒu���擾
            If GetKensaPosCoil(strBaseKanriNo, nBaseLen) Then
                For ii = 0 To mcls_KizuInf.Length - 1

                    '�rNO���i�[����ĂȂ�������\�����Ȃ�
                    If 0 = m_typDspIndex(ii).nKizuSeq Then
                        mcls_KizuInf(ii).SetKensaLabel("", g_ColorControl)
                    Else
                        mcls_KizuInf(ii).SetKensaLabel("������O", g_ColLstKensadaiMae)

                        '�r���Ǘ�NO�̔�r
                        If m_typDspIndex(ii).strKanriNo = strBaseKanriNo Then
                            '�r���̗n�ڂ���̈ʒu�ƃR�C���擪���猟����܂ł̈ʒu�𔻒�
                            If mcls_KizuInf(ii).GetDefectKeyInf.typ.data.y / 1000 < nBaseLen Then
                                mcls_KizuInf(ii).SetKensaLabel("�������", g_ColorControl)
                            End If
                        ElseIf m_typDspIndex(ii).strKanriNo < strBaseKanriNo.Trim Then
                            mcls_KizuInf(ii).SetKensaLabel("�������", g_ColorControl)
                        End If

                    End If
                Next ii
            Else
                For ii = 0 To mcls_KizuInf.Length - 1

                    '������܂œ��B���Ă��Ȃ��B�܂��͏������Ńf�[�^���Ȃ�
                    mcls_KizuInf(ii).SetKensaLabel("", g_ColorControl)

                Next ii
            End If

            For ii = 0 To mcls_KizuInf.Length - 1

                '���R�[�h���i�[����ĂȂ�������\�����Ȃ�
                If 0 < m_typDspIndex(ii).nKizuSeq Then
                    If mcls_KizuInf(ii).IsDspDefault(EM_IMAGE_POS.IMG1) Or mcls_KizuInf(ii).IsDspDefault(EM_IMAGE_POS.IMG2) Then
                        mcls_KizuInf(ii).DspDefectRetry(m_nImagePattern)
                    End If
                End If
            Next ii
            System.GC.Collect()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��ʍX�V�G���[ [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' ������ʒu���擾
    ''' </summary>
    ''' <param name="strKanriNo">�Ǘ�No</param>
    ''' <param name="nLen">������ʒu[m]</param>
    ''' <returns></returns>
    Private Function GetKensaPosCoil(ByRef strKanriNo As String, ByRef nLen As Integer) As Boolean
        Dim bRet As Boolean = False
        Dim nKensa As Integer
        ' ������
        strKanriNo = ""
        nLen = 0

        '������ʒu���擾
        If 0 = m_nPCDiv Then
            nKensa = EM_DIV_DISP.DIV_LEN_POS_DSP_TOP
        Else
            nKensa = EM_DIV_DISP.DIV_LEN_POS_DSP_BOT
        End If

        Try
            '�|�C���^���ǂݍ���
            Dim typHyPoint As TBL_HY_POINT = CType(KizuMod.ksMemRead(TBL_HY_POINT_NAME, GetType(TBL_HY_POINT)), TBL_HY_POINT)

            '�\����_���R�[�h�擾
            Dim nBaseRec As Integer = typHyPoint.nRecDisp(nKensa)
            '�\����_�ɃR�C�����Ȃ���Ώ����I��
            If 0 = nBaseRec Then Exit Try
            '�\����_�R�C���ʔ����擾
            Dim nBaseLen As Integer = typHyPoint.pnt(nBaseRec - 1).typ.nLenDisp(nKensa).nLen
            '������ʒu�ʔ���
            Dim nLenWk As Integer = nBaseLen - mcls_ParamManager.PosKensa(m_nPCDiv)
            '�u�ʔ����v�Ɓu�\����_�`������ʒu�v���r
            If 0 < nLenWk Then
                '�\����_�ʉ߃R�C������������ʉߒ�
                nLen = nBaseLen - mcls_ParamManager.PosKensa(m_nPCDiv)
                strKanriNo = tMod_Common.ByteToString(typHyPoint.pnt(nBaseRec - 1).cKizukenNo).TrimEnd(CType(vbNullChar, Char))
                bRet = True
            Else
                '�O��\�����R�[�h�擾
                Dim nPrevRec As Integer = GetPointPrevRecNo(nBaseRec)

                '�\���Ώۃ��R�[�h���W
                For ii As Integer = 0 To TBL_HY_REC_MAX - 1     '���R�[�h����
                    If ii > DSP_COIL_MAX Then                   '�\������
                        Exit For
                    End If
                    If 0 >= nPrevRec Then                       '�O���R�[�h����
                        Exit For
                    End If

                    '�\���\�܂��͕\���\��
                    If typHyPoint.pnt(nPrevRec - 1).emDispStatus = EM_DIV_DSP_STATUS.DIV_DSP_OK _
                    Or typHyPoint.pnt(nPrevRec - 1).emDispStatus = EM_DIV_DSP_STATUS.DIV_DSP_STANDBY Then
                        nLenWk = nLenWk + typHyPoint.pnt(nPrevRec - 1).typ.nLenDisp(nKensa).nLen
                        If 0 < nLenWk Then
                            '���̃R�C�����������ʉߒ�
                            nLen = nLenWk
                            strKanriNo = tMod_Common.ByteToString(typHyPoint.pnt(nPrevRec - 1).cKizukenNo).TrimEnd(CType(vbNullChar, Char))
                            bRet = True
                            Exit For
                        End If
                    End If
                    '�O��\�����R�[�h�擾
                    nPrevRec = GetPointPrevRecNo(nPrevRec)
                Next
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("������ʒu���擾�G���[ [{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �O���R�[�h�擾
    ''' </summary>
    ''' <param name="nRecNo">���R�[�h</param>
    ''' <returns>�O���R�[�h</returns>
    ''' <remarks></remarks>
    Private Function GetPointPrevRecNo(ByVal nRecNo As Integer) As Integer
        nRecNo -= 1

        If nRecNo < 1 Then                                  '�ŏ��l�A���_�[
            nRecNo = TBL_HY_REC_MAX                         '����
        End If

        If TBL_HY_REC_MAX < nRecNo Then                     '�s���l
            nRecNo = TBL_HY_REC_MAX                         '����
        End If

        Return nRecNo
    End Function

    '***************************************************************************
    ' �t�@���N�V�����L�[�֘A
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton

        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' 0�I���W��
            Case 0                              'F1
                Call SelectF01()
            Case 1                              'F2
                Call SelectF02()
            Case 2                              'F3
                Call SelectF03()
            Case 3                              'F4
                Call SelectF04()
            Case 4                              'F5
                Call SelectF05()
            Case 5                              'F6
                Call SelectF06()
            Case 6                              'F7
                Call SelectF07()
            Case 7                              'F8
                Call SelectF08()
            Case 8                              'F9
                Call SelectF09()
            Case 9                              'F10
                Call SelectF10()
            Case 10                             'F11                
                Call SelectF11()
            Case 11
                If onKey = Keys.Shift Then                                                     '
                    '�V�t�g�L�[�������Ă���ꍇ�́A�^�X�N���I��
                    EndNotice()
                Else
                    SelectF12()
                End If
        End Select
    End Sub

    ''' <summary>
    ''' F1�F
    ''' </summary>
    Private Sub SelectF01()
        Try
            m_nPageNo -= 1

            '�O�y�[�W�̐ݒ�
            If 1 >= m_nPageNo Then     '�ŏ��̃y�[�W��
                btnF01.Enabled = False
            Else
                btnF01.Enabled = True
            End If

            '���y�[�W�{�^���ݒ�
            btnF02.Enabled = True

            DspData()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F1���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F2�F
    ''' </summary>
    Private Sub SelectF02()
        Try
            m_nPageNo += 1

            '�O�y�[�W�{�^���ݒ�
            btnF01.Enabled = True

            '���y�[�W�{�^���ݒ�
            Dim dPageMax As Decimal = CDec(DEFLIST_DISP_DEFECT_MAX / mcls_KizuInf.Length)
            Dim bNextPageFlg As Boolean = NextPageCheck(m_nPageNo + 1, CInt(dPageMax)) '���݂̃y�[�W����Q�y�[�W��ȍ~���r��񂪂��邩
            If dPageMax <= m_nPageNo Or bNextPageFlg = False Then
                btnF02.Enabled = False
            Else
                btnF02.Enabled = True
            End If

            DspData()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F2���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F3�F
    ''' </summary>
    Private Sub SelectF03()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F3���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F4�F
    ''' </summary>
    Private Sub SelectF04()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F4���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F5�F
    ''' </summary>
    Private Sub SelectF05()
        Try
            ''�ꎞ��~�C�x���g����
            RaiseEvent ModeClickEvent(m_nDspMode, m_nImagePattern)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F5���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F6�F
    ''' </summary>
    Private Sub SelectF06()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F6���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F7�F�I�[�o�[���C
    ''' </summary>
    Private Sub SelectF07()
        Try
            '�I�[�o�[���C�\���ؑւ��C�x���g����
            RaiseEvent ev_ChangeOverlyVisible(Not Overlay)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F7���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F8�F�ڐ���
    ''' </summary>
    Private Sub SelectF08()
        Try
            '�ڐ���\���ؑւ��C�x���g����
            RaiseEvent ev_ChangeMemoriVisible(Not Memori)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F8���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F9�F
    ''' </summary>
    Private Sub SelectF09()
        Try
            If mfrm_setting.ShowDialog(Me) = Windows.Forms.DialogResult.OK Then
                ChangeSetting()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F9���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F10�F
    ''' </summary>
    Private Sub SelectF10()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F10���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F11�F
    ''' </summary>
    Private Sub SelectF11()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F11���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' F12�F
    ''' </summary>
    Private Sub SelectF12()
        Try

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("F12���� �G���[ [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �I���ʒm(������)
    ''' </summary>
    Private Sub EndNotice()
        Dim que As COMMON_QUE_S = Nothing
        que.nEventNo = FACT_KS_END
        que.nLineNo = KizuMod.GetLineID()
        KizuMod.Send_Mail(KizuMod.GetMyTaskName(), ".", que)
    End Sub

    ''' <summary>
    ''' �ꎞ��~�����x���̃u�����N����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timMode_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timMode.Tick

        '���x���\���F�ύX
        If Color.Red = lblMode.BackColor Then
            lblMode.BackColor = g_ColorControl
            lblMode.ForeColor = Color.Black
        Else
            lblMode.BackColor = Color.Red
            lblMode.ForeColor = Color.White
        End If

    End Sub

    ''' <summary>
    ''' �ꎞ��~�^�C�}�[�ύX
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <remarks></remarks>
    Public Sub ChangeStopTimer(ByVal nVal As Integer)

        mfrm_setting.StopCloseTime = nVal

    End Sub

    ''' <summary>
    ''' �|�b�v�A�b�v�摜�^�C�}�[�ύX
    ''' </summary>
    ''' <param name="nVal">�l</param>
    ''' <remarks></remarks>
    Public Sub ChangePopTimer(ByVal nVal As Integer)

        mfrm_setting.PopUpCloseTime = nVal

    End Sub

    ''' <summary>
    ''' �r�摜�_�u���N���b�N
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub picImg_DoubleClick(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try

            'If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
            '    '�ꎞ��~���ł͂Ȃ�
            '    Exit Try
            'End If

            For Each dsp As clsKizuInf In mcls_KizuInf
                If Not dsp.ImageZoom Then Continue For
                For Each picbox As PictureBox In dsp.DefectViewInf.picImg
                    If sender Is picbox Then
                        ' �_�u���N���b�N����PictureBox�ƈ�v������A�r�摜�g��\��
                        dsp.ShowPopup(mfrm_PopImage)
                        '�|�b�v�A�b�v�\�������^�C�}�[�I��
                        timPopUp.Enabled = False
                        '�|�b�v�A�b�v�\�������^�C�}�[�ݒ�
                        Call TimerSet(mfrm_setting.cmbPopUp, timPopUp)

                        Exit Try
                    End If
                Next
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�_�u���N���b�N�ُ�[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �����Ŏw�肳�ꂽ�y�[�W����y�[�W�܂��r��񂪑��݂��邩�ǂ������肷��
    ''' </summary>
    ''' <param name="nStartPage">�`�F�b�N�J�n�y�[�W</param>
    ''' <param name="nEndPage">�`�F�b�N�I���y�[�W</param>
    ''' <remarks></remarks>
    Private Function NextPageCheck(ByVal nStartPage As Integer, ByVal nEndPage As Integer) As Boolean

        '�ŏI�y�[�W - 1�܂Ń��[�v
        For ii As Integer = nStartPage To nEndPage
            '���y�[�W�����擾
            Dim nDsipCnt As Integer = mcls_KizuInf.Length * (ii - 1)
            Dim nNextRec As Integer = (m_nStop_newrec - 1) - nDsipCnt
            If nNextRec < 0 Then nNextRec = HY_DEFLIST_REC_MAX - Math.Abs(nNextRec)

            '���y�[�W��\�������ꍇ�A�\���\���r��񂪂��邩����
            For jj As Integer = 0 To mcls_KizuInf.Length - 1

                '�\���\���r��񂪑���
                If 0 < m_typDefInf.rec.data(m_nDspMode).DfData(nNextRec).typ.data.nKizuNo Then
                    Return True
                End If

                nNextRec = nNextRec - 1
                If nNextRec < 0 Then nNextRec = HY_DEFLIST_REC_MAX - 1

            Next jj

        Next ii

        Return False

    End Function

    ''' <summary>
    ''' ��ʍX�V
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspUpdate()
        DspData()
    End Sub

End Class
