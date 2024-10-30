'*******************************************************************************
'   �T�����
'
'  [Ver]
'       Ver.01  2013/12/18  ����
'
'  [����]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI
Imports KizuLibCLI.KizuMod
Imports KizuForm

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Reflection
Imports tClassLibrary.tClass.tClass_LogManager



''' <summary>
''' �T����ʃN���X
''' </summary>
''' <remarks></remarks>
Public Class frmTansho
    '///////////////////////////////////////////////////////////////////////////
    '�萔
    '///////////////////////////////////////////////////////////////////////////
    Private Const MAX_FUNC_TAB As Integer = 3
    Private Const MAX_COIL_TAB As Integer = 1
    Private Const PATH_DEFAULT_IMG As String = "HY_TANSHO.�r����.bmp"           '�f�t�H���g�摜�̃p�X

    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////

    '///////////////////////////////////////////////////////////////////////////
    '�����o�[�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private WithEvents m_timMap As Timer                            '�r�}�b�v�X�V�^�C�}�[
    Private WithEvents m_timStatus As Timer                         '�X�e�[�^�X���X�V�^�C�}�[
    Private WithEvents m_timCoil As Timer                           '�R�C�����X�V�^�C�}�[
    Private WithEvents m_timBrink As Timer                          '�u�����N�\���^�C�}�[

    Private mcls_Log As tClass_LogManager                           '���O�Ǘ�

    Private mcls_Param As clsParamManager                           '�p�����[�^�Ǘ�
    Private mcls_PreParam As clsPreParamManager                     '�O�H���p�����[�^�Ǘ��N���X

    Private mcls_PointInf As clsPointInf                            '�|�C���^���
    Private mcls_DispMap As clsDispMap                              '�r�}�b�v�\��
    Private mcls_DispCoil As clsDispCoil                            '�R�C�����\��
    Private mcls_DispStatus As clsDispStatus                        '�X�e�[�^�X���\��
    Private mcls_DispDefect() As clsDispDefect                      '�r���\��
    Private mcls_Alarm As clsDispAlarm                              '�x���\��

    Private mfrm_PopImage As frmDefectImage                         '�r�g��摜�\��
    Private mfrm_PopTcmImage As frmDefectImage2                     '�r�g��摜�\��(TCM�p)
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	Private mfrm_LongPdList As frmLongPdList                        '���������׈ꗗ
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	Private m_nFormID As Integer = 0                                '�t�H�[�����ʔԍ�
    Private m_nMapValue As Integer                                  '�X�N���[���l(�����p)

    Private m_bScrollSync As Boolean = False                        '�X�N���[�������L�� (True:���� False:�񓯊�)
    Private m_bEnaDspCoilInf As Boolean = True                      '�R�C�����\������t���O (True:���� False:�ُ�)
    Private m_nPauseRec As Integer                                  '�ꎞ��~�����݃��R�[�h
    Private m_nMonitor As Integer                                   '��ʔԍ�

    Private m_nTcmImageWide As Integer = 25                         'TCM�r�摜�\���]�[���̉���
    Private m_bDspAlarm As Boolean = False                          '�r�ڋ߃����v�\���L��(True:�\���AFalse:��\��)
    '///////////////////////////////////////////////////////////////////////////
    '�ǉ��C�x���g
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �X�V���[�h�ύX
    ''' </summary>
    ''' <param name="sender">�t�H�[��</param>
    ''' <param name="emRefreshMode">�ύX����X�V���[�h</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeRefreshMode(ByVal sender As frmTansho, ByVal emRefreshMode As EM_REFRESH_MODE)

    ''' <summary>
    ''' �r�}�b�v�Ԋu�ύX
    ''' </summary>
    ''' <param name="sender">�t�H�[��</param>
    ''' <param name="stage">����Ԋu</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeMapInterval(ByVal sender As frmTansho, ByVal stage As EM_LONG_POS)

    '''' <summary>
    '''' �r�摜�\�����[�h�ύX
    '''' </summary>
    '''' <param name="sender">�ύX�̂������T�����</param>
    '''' 	<param name="emMode">�\�����[�h</param>
    '''' <remarks></remarks>
    'Public Event ev_ChangeDefectMode(ByVal sender As frmTansho, ByVal emMode As EM_DEFECT_MODE)

    ''' <summary>
    ''' �I�[�o�[���C�\���ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bVisible">�ύX��̕\�� True:�\�� False:��\��</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeOverlyVisible(ByVal sender As frmTansho, ByVal bVisible As Boolean)

    ''' <summary>
    ''' �ڐ���\���ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bVisible">�ύX��̕\�� True:�\�� False:��\��</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeMemoryVisible(ByVal sender As frmTansho, ByVal bVisible As Boolean)

    ''' <summary>
    ''' �g��\���ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bEnable">�ύX��̕\�� True:�L�� False:����</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeImageZoom(ByVal sender As frmTansho, ByVal bEnable As Boolean)

    ''' <summary>
    ''' �g��\���{���ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="nRate">�\���{��</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeZoomRate(ByVal sender As frmTansho, ByVal nRate As Integer)

    ''' <summary>
    ''' �������[�h�ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bSync">�ύX��̃��[�h True:���� False:�񓯊�</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeSyncMode(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' �{�^���^�u�����ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bSync">�ύX��̃��[�h True:���� False:�񓯊�</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeFuncSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' ��ʍX�V���[�h�����ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bSync">�ύX��̃��[�h True:���� False:�񓯊�</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeModeSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' �c�Ԋu�ؑ֓����ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bSync">�ύX��̃��[�h True:���� False:�񓯊�</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeScaleSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' �X�N���[�������ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bSync">�ύX��̃��[�h True:���� False:�񓯊�</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeScrollSync(ByVal sender As frmTansho, ByVal bSync As Boolean)

    ''' <summary>
    ''' �X�N���[���ʕύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="nValue">�X�N���[����</param>
    ''' <param name="emScrool">�X�N���[������</param>
    ''' <remarks></remarks>
    Public Event ev_ChangeScrollValue(ByVal sender As frmTansho, ByVal nValue As Integer, ByVal emScrool As EM_MAP_SCROOL)

    ''' <summary>
    '''  �t�@���N�V��������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Public Event ev_PressFunctionKey(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs)


#Region "�v���p�e�B"
    '///////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���̒T����ʂ�ID���擾���܂�
    ''' </summary>
    ''' <returns>���ʔԍ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property FormID() As Integer
        Get
            Return m_nFormID
        End Get
    End Property

    ''' <summary>
    ''' ������ԍ����擾���܂�
    ''' </summary>
    ''' <returns>������ԍ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KensaNo() As Integer
        Get
            If Not mcls_DispMap Is Nothing Then
                Return mcls_DispMap.KensaNo
            End If
            Return 0
        End Get
    End Property

    ''' <summary>
    ''' �r�}�b�v�X�V�Ԋu�ݒ�Ǝ擾
    ''' </summary>
    ''' <value>�X�V�Ԋu[ms]</value>
    ''' <returns>�X�V�Ԋu[ms]</returns>
    ''' <remarks></remarks>
    Public Property MapInterval() As Integer
        Get
            Return m_timMap.Interval
        End Get
        Set(ByVal value As Integer)
            If Not m_timMap Is Nothing Then
                m_timMap.Interval = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' �X�e�[�^�X���X�V�Ԋu�ݒ�
    ''' </summary>
    ''' <value>�X�V�Ԋu[ms]</value>
    ''' <returns>�X�V�Ԋu[ms]</returns>
    ''' <remarks></remarks>
    Public Property StatusInterval() As Integer
        Get
            Return m_timStatus.Interval
        End Get
        Set(ByVal value As Integer)
            If Not m_timStatus Is Nothing Then
                m_timStatus.Interval = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' �R�C�����X�V�Ԋu�ݒ�
    ''' </summary>
    ''' <value>�X�V�Ԋu[ms]</value>
    ''' <returns>�X�V�Ԋu[ms]</returns>
    ''' <remarks></remarks>
    Public Property CoilInterval() As Integer
        Get
            Return m_timCoil.Interval
        End Get
        Set(ByVal value As Integer)
            If Not m_timCoil Is Nothing Then
                m_timCoil.Interval = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' �r�}�b�v�̃X�N���[������
    ''' </summary>
    ''' <value>False:�� True:��</value>
    ''' <returns>False:�� True:��</returns>
    ''' <remarks></remarks>
    Public Property ScrollDrection() As EM_MAP_SCROOL
        Get
            Return mcls_DispMap.MapDir
        End Get
        Set(ByVal value As EM_MAP_SCROOL)
            If Not mcls_DispMap Is Nothing Then
                mcls_DispMap.MapDir = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' �\���Ԋu�敪���擾���܂�
    ''' </summary>
    ''' <returns>�\���Ԋu</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Stage() As EM_LONG_POS
        Get
            Return mcls_DispMap.Stage
        End Get
    End Property

    ''' <summary>
    ''' �\���\���ʔԍ����擾���܂�
    ''' </summary>
    ''' <returns>�\����</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Men() As Integer
        Get
            Return mcls_DispMap.Men
        End Get
    End Property

    ''' <summary>
    ''' �r�}�b�vDS�ʒu
    ''' </summary>
    ''' <value>NORMAL:����DS�AREVERSE:����WS</value>
    ''' <returns>NORMAL:����DS�AREVERSE:����WS</returns>
    ''' <remarks></remarks>
    Public Property DSPos() As EM_MAP_CELL
        Get
            Return mcls_DispMap.DSPos
        End Get
        Set(ByVal value As EM_MAP_CELL)
            If Not mcls_DispMap Is Nothing Then
                mcls_DispMap.DSPos() = value
            End If
        End Set
    End Property


    ''' <summary>
    ''' DB�ύX�ʒm
    ''' </summary>
    ''' <value>True�F�ڑ��AFalse�F���ڑ�</value>
    ''' <returns>True�F�ڑ��AFalse�F���ڑ�</returns>
    ''' <remarks></remarks>
    Public Property DBStatus() As Boolean
        Get
            Return mcls_DispStatus.DbStatus
        End Get
        Set(ByVal value As Boolean)
            If Not mcls_DispStatus Is Nothing Then
                mcls_DispStatus.DbStatus = value
            End If
        End Set
    End Property

    ''' <summary>
    ''' �r�}�b�v�X�N���[������/�񓯊�
    ''' </summary>
    ''' <value>True�F�����AFalse�F�񓯊�</value>
    ''' <returns>True�F�����AFalse�F�񓯊�</returns>
    ''' <remarks></remarks>
    Public Property ScrollSync() As Boolean
        Get
            Return m_bScrollSync
        End Get
        Set(ByVal value As Boolean)
            m_bScrollSync = value
            If m_bScrollSync Then
                btnF02.BackColor = Color.Aqua
            Else
                btnF02.BackColor = Color.FromKnownColor(KnownColor.Control)
            End If
        End Set
    End Property

    ''' <summary>
    ''' �I�[�o�[���C�\���ݒ�ON/OFF
    ''' </summary>
    ''' <value>True�F�\���AFalse�F��\��</value>
    ''' <returns>True�F�\���AFalse�F��\��</returns>
    ''' <remarks></remarks>
    Public Property Overlay() As Boolean
        Get
            Return mcls_DispDefect(0).OverlayVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect �̃I�[�o�[���C�\����ݒ�
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.OverlayVisible = value
            Next

            ' �g��摜 �̃I�[�o�[���C�\����ݒ�
            mfrm_PopImage.SetVisibleOverlay(value)
            ' �g��摜(TCM) �̃I�[�o�[���C�\����ݒ�
            mfrm_PopTcmImage.SetVisibleOverlay(value)

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
            Return mcls_DispDefect(0).MemoriVisible
        End Get
        Set(ByVal value As Boolean)

            ' mcls_DispDefect �̖ڐ���\����ݒ�
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.MemoriVisible = value
            Next

            ' �g��摜 �̖ڐ���\����ݒ�
            mfrm_PopImage.SetVisibleMemory(value)
            ' �g��摜(TCM) �̖ڐ���\����ݒ�
            mfrm_PopTcmImage.SetVisibleMemory(value)

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
    ''' ��ʔԍ��擾
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MonitorNo() As Integer
        Get
            Return m_nMonitor
        End Get
    End Property

#End Region

    '///////////////////////////////////////////////////////////////////////////
    '���\�b�h
    '///////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="nFormID">���ID</param>
    ''' <param name="clsLog">���O�Ǘ�</param>
    ''' <param name="clsParam">�p�����[�^�Ǘ�</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal nFormID As Integer, ByRef clsLog As tClass_LogManager, ByRef clsParam As clsParamManager, ByRef cls_PreParam As clsPreParamManager)

        '���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()
        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

        Try
            '������
            m_nFormID = nFormID
            mcls_Log = clsLog
            mcls_Param = clsParam
            mcls_PreParam = cls_PreParam
            m_nMonitor = 1

            '-----------------------------------------------
            '�^�C�}�[
            '-----------------------------------------------
            m_timMap = New Timer()                          '�\���r�}�b�v�X�V�^�C�}�[
            m_timStatus = New Timer()                       '�X�e�[�^�X���X�V�^�C�}�[
            m_timCoil = New Timer()                         '�R�C�����X�V�^�C�}�[

            '-----------------------------------------------
            '�|�C���^���
            '-----------------------------------------------
            mcls_PointInf = New clsPointInf(clsLog)

			'-----------------------------------------------
			'�X�e�[�^�X���
			'-----------------------------------------------
			mcls_DispStatus = InitStatusInf(ucStatus, UcCoil1)
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			mcls_DispStatus.LongPdGuidanceMessage = mcls_Param.Pro_LongPd_GuidanceMessage
			mcls_DispStatus.LongPdGuidanceForeColor = mcls_Param.Pro_LongPd_GuidanceForeColor
			mcls_DispStatus.LongPdGuidanceBackColor = mcls_Param.Pro_LongPd_GuidanceBackColor
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

			'-----------------------------------------------
			'�R�C�����
			'-----------------------------------------------
			mcls_DispCoil = InitCoilInf(UcCoil1)

            '-----------------------------------------------
            '�r�g��摜���
            '-----------------------------------------------
            ' �^�X�N��(HY_TANSHO) ����ini���Z�N�V��������
            Dim strSection As String = GetMyTaskName() & "_FORM" & nFormID
            Dim bPicDirect As Boolean = CBool(tMod_IniFile.ReadIniInt(strSection, "DEFIMAGE_DIRECT", TASKINI_NAME, 0))
            mfrm_PopImage = New frmDefectImage(clsLog, mcls_Param, PATH_DEFAULT_IMG, strSection, DSP_KIRI_IMAGE_MAX, bPicDirect)
            mfrm_PopImage.Owner = Me

            '-----------------------------------------------
            'TCM�r�摜���
            '-----------------------------------------------
            ' �^�X�N��(HY_TANSHO) ����ini���Z�N�V��������
            mfrm_PopTcmImage = New frmDefectImage2(clsLog, mcls_PreParam, PATH_DEFAULT_IMG, strSection, 1, bPicDirect)
            mfrm_PopTcmImage.Owner = Me

			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			'-----------------------------------------------
			'���������׈ꗗ���
			'-----------------------------------------------
			mfrm_LongPdList = New frmLongPdList(clsLog, clsParam)
			mfrm_LongPdList.Owner = Me
			mfrm_LongPdList.Visible = False
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

			'-----------------------------------------------
			'�r�\��
			'-----------------------------------------------
			ReDim mcls_DispDefect(DSP_DEFECT_MAX - 1)
            mcls_DispDefect(0) = InitDefInf(UcDefect1, g_ColorNewKizu, 1)      '�ŐV���r
            mcls_DispDefect(1) = InitDefInf(UcDefect2, g_ColorOldKizu, 2)      '�P�O���r

            '-----------------------------------------------
            '�}�b�v�\��
            '-----------------------------------------------
            Dim typMapInf As clsDispMap.TYP_MAP_INF = Nothing
            With typMapInf
                .dgvMap = dgvMap                                            '�\���r�}�b�v
                .dgvDeep = dgvDeep                                          '�r�[�����X�g
                .dgvTcm = dgvOth1                                           'TCM���X�g
                .vsbMap = vsbMap                                            '�X�N���[���o�[
                .lblKensaPoint = lblKensaPoint
            End With

            mcls_DispMap = New clsDispMap(typMapInf, clsParam, cls_PreParam, clsLog, mcls_PointInf, mcls_DispCoil, nFormID)

            vsbMap.Enabled = False
            vsbMap.Value = 0

            '��ʐݒ�
            GetIniSetting(nFormID)                                              '�ݒ�l�擾

            '�r�}�b�v�̃��C���F�ݒ�
            lblShearTitle.ForeColor = g_ColorShar                               '�V���[�J�b�g��
            lblYousetuTitle.ForeColor = g_ColorYousetu                          '�n�ڐ�


            '-----------------------------------------------
            '�r�ڋ߃����v
            '-----------------------------------------------
            Dim lblLamp() As Label = {lblAlarm1, lblAlarm2, lblAlarm3, lblAlarm4, lblAlarm5, lblAlarm6, lblAlarm7, lblAlarm8}
            Dim typAlarmDsp As clsDispAlarm.TYP_ALARM_DSP = Nothing
            With typAlarmDsp
                .lblLamp = lblLamp
            End With
            mcls_Alarm = New clsDispAlarm(typAlarmDsp, clsLog)
            If m_bDspAlarm Then
                pnlAlarm.Visible = True
            Else
                pnlAlarm.Visible = False
            End If

            '-----------------------------------------------
            '�C�x���g�̐ݒ�
            '-----------------------------------------------
            '�}�b�v�����X�V
            AddHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmTansho_SelectMapCellAuto
            AddHandler mcls_DispMap.SelectTcmMapCell_Auto, AddressOf frmTansho_SelectTcmMapCellAuto
            '�}�b�v�����X�V(�f�[�^�������ꍇ������)
            AddHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmTansho_SelectMapCellAutoDspInit
            AddHandler mcls_DispMap.SelectTcmMapCell_AutoDspInit, AddressOf frmTansho_SelectTcmMapCellAutoDspInit
            '�}�b�v�I��
            AddHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmTansho_SelectMapCellClick
            AddHandler mcls_DispMap.SelectMapTcmCell_Click, AddressOf frmTansho_SelectTcmMapCellClick
            '�X�N���[���o�[
            AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            '�}�E�X�z�C�[���X�N���[��
            AddHandler dgvMap.MouseWheel, AddressOf dgvMap_MouseWheel
            '�r�摜�_�u���N���b�N
            AddHandler UcDefect1.picImg1.DoubleClick, AddressOf pic_DoubleClick
            AddHandler UcDefect1.picImg2.DoubleClick, AddressOf pic_DoubleClick
            AddHandler UcDefect2.picImg1.DoubleClick, AddressOf pic_DoubleClick
            AddHandler UcDefect2.picImg2.DoubleClick, AddressOf pic_DoubleClick

        Catch ex As Exception
            If Not mcls_Log Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�T����ʐ����ُ�[{0}],[{1}]", nFormID, ex.Message))
            End If
        End Try
    End Sub

    '***************************************************************************
    ' ���C�����W���[���Ăяo���֐�(�O���ďo��)
    '***************************************************************************
    ''' <summary>
    ''' ���\�[�X�j��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub frmTansho_Disposed()
        Try
            Dim strSection As String = HY_TANSHO & "_FORM" & m_nFormID          '�Z�N�V�������쐬

            '///////////////////////////////////////////////
            'KizuLocal.ini�̐ݒ�
            '///////////////////////////////////////////////
            If Not mcls_DispMap Is Nothing Then
                tMod_IniFile.WriteIni(strSection, "LONG_POS_INF", LOCALINI_NAME, CType(mcls_DispMap.Stage, Integer).ToString)
            End If

            If Not mcls_DispDefect Is Nothing Then
                tMod_IniFile.WriteIni(strSection, "OVERLAY_VIEW_INF", LOCALINI_NAME, IIf(mcls_DispDefect(0).OverlayVisible, 1, 0).ToString)
                tMod_IniFile.WriteIni(strSection, "SCALE_VIEW_INF", LOCALINI_NAME, IIf(mcls_DispDefect(0).MemoriVisible, 1, 0).ToString)
                'tMod_IniFile.WriteIni(strSection, "SCROLL_VIEW_INF", LOCALINI_NAME, IIf(ScrollSync = True, 1, 0).ToString)
            End If

            '�C�x���g����
            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            RemoveHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmTansho_SelectMapCellAuto
            RemoveHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmTansho_SelectMapCellAutoDspInit
            RemoveHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmTansho_SelectMapCellClick
            RemoveHandler dgvMap.MouseWheel, AddressOf dgvMap_MouseWheel
            RemoveHandler UcDefect1.picImg1.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect2.picImg1.DoubleClick, AddressOf pic_DoubleClick

            '�^�C�}�[�j��
            If Not m_timMap Is Nothing Then
                m_timMap.Stop()
                m_timMap.Dispose()
                m_timMap = Nothing
            End If
            If Not m_timStatus Is Nothing Then
                m_timStatus.Stop()
                m_timStatus.Dispose()
                m_timStatus = Nothing
            End If
            If Not m_timCoil Is Nothing Then
                m_timCoil.Stop()
                m_timCoil.Dispose()
                m_timCoil = Nothing
            End If

            '�T�u�N���X���
            If Not mcls_PointInf Is Nothing Then
                mcls_PointInf.Dispose()
                mcls_PointInf = Nothing
            End If
            If Not mcls_DispMap Is Nothing Then
                mcls_DispMap.Dispose()
                mcls_DispMap = Nothing
            End If
            If Not mcls_DispDefect Is Nothing Then
                For Each dsp As clsDispDefect In mcls_DispDefect
                    If Not dsp Is Nothing Then
                        dsp.DisposeImage()
                        dsp.Dispose()
                    End If
                Next
                mcls_DispDefect = Nothing
            End If
            If Not mcls_DispCoil Is Nothing Then
                mcls_DispCoil.Dispose()
                mcls_DispCoil = Nothing
            End If
            If Not mcls_DispStatus Is Nothing Then
                mcls_DispStatus.Dispose()
                mcls_DispStatus = Nothing
            End If
            If Not mfrm_PopImage Is Nothing Then
                mfrm_PopImage.Dispose()
                mfrm_PopImage = Nothing
            End If
			If Not mfrm_PopTcmImage Is Nothing Then
				mfrm_PopTcmImage.Dispose()
				mfrm_PopTcmImage = Nothing
			End If
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			If Not mfrm_LongPdList Is Nothing Then
				mfrm_LongPdList.Dispose()
				mfrm_LongPdList = Nothing
			End If
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

			Me.Dispose()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�J���C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �\��������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        Try

            mcls_DispMap.ClearMap()
            mcls_DispMap.ClearDeep()
            mcls_DispMap.ClearTcm()
            mcls_DispMap.ClearMaxRowCnt()

            '-------------------------------------------------------------------
            '�e�\���̏�����
            '-------------------------------------------------------------------
            InitLabel()
            'DspMapInfInit()                                                    '�}�b�v��񏉊���
            mcls_DispStatus.DspInit()                                           '�X�e�[�^�X��񏉊���
            mcls_DispCoil.DspInit()                                             '�R�C����񏉊���
            mcls_DispCoil.DispCoilLen("")
            mcls_DispCoil.DispMen(Men)                                          '�ʏ��\��
            dgvMap.Refresh()                                                    '�r�}�b�v�ĕ`��
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.DspInit()
            Next

            ChangeRefreshMode(EM_REFRESH_MODE.AUTO)

            SetMapInterval(mcls_DispMap.Stage)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�T����ʑS�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�摜�\���]�[���̉�����ݒ�
    ''' </summary>
    ''' <param name="nWidth">�摜�\���]�[���̉���[mm]</param>
    ''' <remarks></remarks>
    Public Sub SetImageWidth(ByVal nWidth As Integer)
        Try
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.ImageWidth = nWidth
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�]�[���̐ݒ�ُ� Width={0}[{1}]", nWidth, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' TCM�r�摜�\���]�[���̉�����ݒ�
    ''' </summary>
    ''' <param name="nWidth">�摜�\���]�[���̉���[mm]</param>
    ''' <remarks></remarks>
    Public Sub SetTcmImageWidth(ByVal nWidth As Integer)
        Try
            m_nTcmImageWide = nWidth
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("Tcm�r�摜�]�[���̐ݒ�ُ� Width={0}[{1}]", nWidth, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �}�b�v�X�N���[��
    ''' </summary>
    ''' <param name="nValue">�X�N���[���l</param>
    ''' <remarks></remarks>
    Public Sub SetScrollValue(ByVal nValue As Integer, ByVal emMapScrool As EM_MAP_SCROOL)
        Dim nData As Integer = nValue
        Try
            vsbMap.ValueProgramable = nValue
            m_nMapValue = nValue
            If m_bScrollSync Then                                               '�X�N���[�������̏ꍇ�̂ݎ��s
                SetScroll(nData)
            End If
            'End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�N���[���ʐݒ�ُ� value={0}[{1}]", nValue, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �u�����N�\��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub m_timBrink_Tick()
        Try
            If mcls_DispMap.RefreshMode = EM_REFRESH_MODE.PAUSE Then
                If lblMode.BackColor = Color.Red And lblMode.ForeColor = Color.White Then
                    lblMode.BackColor = g_ColorControl
                    lblMode.ForeColor = Color.Black
                ElseIf lblMode.BackColor = g_ColorControl And lblMode.ForeColor = Color.Black Then
                    lblMode.BackColor = Color.Red
                    lblMode.ForeColor = Color.White
                End If
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�u�����N�\���ŗ�O����[{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' �C�x���g�n���h���֐�
    '***************************************************************************
    ''' <summary>
    ''' �T����ʃ��[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTansho_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Try

            DspInit()                                                           '��ʑS���������s

            mcls_DispCoil.Load()

            vsbMap.Enabled = False
            vsbMap.Value = 0

            m_timStatus.Start()

            mcls_DispStatus.DspStatus(Men)                                '�X�e�[�^�X�\��

            AddHandler TableLayoutPanel8.Resize, AddressOf MapReSize
            MapReSize(Nothing, Nothing)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�T����ʃ��[�h�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �L�[����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub frmTansho_KeyDown_(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        RaiseEvent ev_PressFunctionKey(sender, e)
    End Sub

    ''' <summary>
    ''' �X�e�[�^�X���X�V
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub m_timStatus_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timStatus.Tick
		Try
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			mcls_DispStatus.LongPdGuidanceMessage = mcls_Param.Pro_LongPd_GuidanceMessage
			mcls_DispStatus.LongPdGuidanceForeColor = mcls_Param.Pro_LongPd_GuidanceForeColor
			mcls_DispStatus.LongPdGuidanceBackColor = mcls_Param.Pro_LongPd_GuidanceBackColor
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

			mcls_DispStatus.DspStatus(Men)                                    '�X�e�[�^�X�\��
		Catch ex As Exception
			mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�e�[�^�X�\���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �R�C�����X�V
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub m_timCoil_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timCoil.Tick
        Try
            Dim nRecNo As Integer
            Dim coilinf As COINFO_BASE = Nothing

            '������
            coilinf.initialize()

            If EM_DEFECT_MODE.TRACKING = mcls_DispMap.MapUpdateMode Then
                nRecNo = mcls_DispMap.GetKensaPosCoilRec()
            Else
                nRecNo = mcls_PointInf.RecDisp(KensaNo)
            End If

            If 0 >= nRecNo Then
                Exit Try
            End If

            If mcls_DispCoil.GetCoilInf(nRecNo, coilinf) Then             '�R�C�����e�[�u���Ǎ�
                mcls_DispCoil.DspCoil(nRecNo, coilinf, Men)               '�R�C�����\��
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C�����X�V�^�C���A�E�g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v�X�V
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub m_timMap_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timMap.Tick

        Try
            If m_bDspAlarm And EM_REFRESH_MODE.AUTO = mcls_DispMap.RefreshMode Then
                '�x��K�C�_���X�\���L�������X�V
                pnlAlarm.Visible = True
                TableLayoutPanel7.Visible = False

                Dim nDspPos As Integer = 0 ' ������ �\�ʕ\��
                If EM_DIV_DISP.DIV_LEN_POS_DSP_BOT = KensaNo Then  ' ������ ���ʕ\��
                    nDspPos = 1
                End If

                mcls_Alarm.DspDefLamp(nDspPos, Men, CBool(IIf(DSPos = EM_MAP_CELL.NORMAL, True, False)))
            Else
                pnlAlarm.Visible = False
                TableLayoutPanel7.Visible = True
            End If

            '�\�����[�h
            Select Case mcls_DispMap.RefreshMode
                Case EM_REFRESH_MODE.AUTO
                    ' �|�C���^���ǂݍ���
                    mcls_PointInf.ReadHyPoint()
                    ' �rMAP�X�V
                    mcls_DispMap.DspData(Men)

                    '' �r�摜�^�C�g�����x���ݒ�
                    If EM_DEFECT_MODE.TRACKING = mcls_DispMap.MapUpdateMode Then
                        Dim nKizu1 As Integer = mcls_DispMap.GetSelectKizuPos(EM_IMAGE_POS.IMG1)
                        Dim nKizu2 As Integer = mcls_DispMap.GetSelectKizuPos(EM_IMAGE_POS.IMG2)

                        If 0 = nKizu1 And 0 = nKizu2 Then
                            UcDefect1.lblDefTitle.Text = "�O�r"
                            UcDefect1.lblDefTitle.Tag = False
                            UcDefect2.lblDefTitle.Text = "�O�X�r"
                            UcDefect2.lblDefTitle.Tag = False
                        ElseIf 0 < nKizu1 And 0 = nKizu2 Then
                            UcDefect1.lblDefTitle.Text = "���r"
                            UcDefect1.lblDefTitle.Tag = True
                            UcDefect2.lblDefTitle.Text = "�O�r"
                            UcDefect2.lblDefTitle.Tag = False
                        Else
                            UcDefect1.lblDefTitle.Text = "���r"
                            UcDefect1.lblDefTitle.Tag = True
                            UcDefect2.lblDefTitle.Text = "���X�r"
                            UcDefect2.lblDefTitle.Tag = True
                        End If
                    Else
                        ' �r�摜�^�C�g�����x���ݒ�
                        UcDefect1.lblDefTitle.Text = "�ŐV���r"
                        UcDefect1.lblDefTitle.Tag = True
                        UcDefect2.lblDefTitle.Text = "��O���r"
                        UcDefect2.lblDefTitle.Tag = True
                    End If

                    '����
                    Dim nLen As Integer = mcls_DispMap.GetInCoilLen()
                    If nLen >= 0 Then
                        mcls_DispCoil.DispCoilLen(nLen.ToString)
                    End If

                    ' �摜�̕\�����g���C���s��(�摜�\�����ɉ摜�t�@�C�����܂���������Ă��Ȃ��ꍇ�����邽�߁j
                    For Each def As clsDispDefect In mcls_DispDefect
                        def.DspDefectRetry()
                    Next

                Case EM_REFRESH_MODE.PAUSE
                    ' �ꎞ��~���ɓ���̏����ƂȂ����ꍇ�́A�ꎞ��~�������I�ɉ������鏈��
                    ' ������ʒu�̃��R�[�h�ԍ����擾���A�ꎞ��~�������̃��R�[�h�ԍ��Ɣ�r
                    Using clsPointTmp As New clsPointInf(mcls_Log)
                        clsPointTmp.ReadHyPoint()
                        Dim nNowRec As Integer = clsPointTmp.RecDisp(KensaNo)       '���݃��R�[�h

                        If nNowRec <> m_nPauseRec Then      '���݃��R�[�h�ƈꎞ��~���̃��R�[�h���قȂ�
                            SelectF01()                     '�ꎞ��~����
                        End If

                    End Using

            End Select
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�V�^�C���A�E�g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �t�H�[���N���[�Y(�V�X�e���{�^��)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub frmTansho_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        '�N���[�Y�{�^���ɂ��t�H�[���̓L�����Z���B
        e.Cancel = True
        '�^�X�N�I��
        EndNotice()
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I���C�x���g(�I�[�g��)
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="pntKizu">�I���r���</param>
    ''' <param name="emInf">�r���ʒu</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectMapCellAuto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspDefect(nMen, pntKizu, True)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I���C�x���g�ŗ�O���� �\����={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I�������C�x���g(�I�[�g��)
    ''' </summary>
    ''' <param name="emInf">�r���ʒu</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectMapCellAutoDspInit(ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspInit()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I�������C�x���g�ŗ�O���� �\����={0}[{1}]", Men, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I��(TCM)�C�x���g(�I�[�g��)
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="pntKizu">�I���r���</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectTcmMapCellAuto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Try
            DispDefectTcmImg(nMen, pntKizu)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I��(TCM)�C�x���g�ŗ�O���� �\����={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I��(TCM)�����C�x���g(�I�[�g��)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectTcmMapCellAutoDspInit()
        Try
            If mfrm_PopTcmImage.Visible Then                               'TCM�r�摜�g��\����
                mfrm_PopTcmImage.CloseFrom()                               'TCM�g��摜OFF��
            End If
            mcls_DispMap.ClearTcm()
            dgvOth1.Refresh()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I��(TCM)�����C�x���g�ŗ�O���� �\����={0}[{1}]", Men, ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �r�}�b�v/�r�Z���I���C�x���g(�I����)
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="pntKizu">�I���r���</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal bSelect As Boolean)
        Dim nLen As Integer = 0
        Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try
            If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
                Exit Try
            End If


            If bSelect Then
                For nCnt As Integer = mcls_DispDefect.Count - 1 To 1 Step -1
                    Dim strVal As TYP_MAP_LIST_VALUE = Nothing
                    If mcls_DispMap.GetSelectKizu(nCnt, strVal) AndAlso 0 < strVal.nKizuNo Then
                        mcls_DispDefect(nCnt).DspDefect(nMen, mcls_DispDefect(nCnt - 1).CurrentDefect, False)
                    Else
                        mcls_DispDefect(nCnt).DspInit()
                    End If
                Next

                '�r����\��
                mcls_DispDefect(0).DspDefect(nMen, pntKizu, False)
            End If

            If mcls_DispCoil.GetCoilInf(pntKizu.nRec, coilinf) Then    '�R�C�����e�[�u���Ǎ�
                mcls_DispCoil.DspCoil(pntKizu.nRec, coilinf, Men)      '�R�C�����\��
            End If

            If mfrm_PopImage.Visible Then                              '�r�摜�g��\����
                '�_�u���N���b�N�ȊO�͍ŐV�r�̏���\��
                mfrm_PopImage.DispImgNo = 1
                DispDefectImg()
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I���C�x���g �\����={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�摜�_�u���N���b�N
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub pic_DoubleClick(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Try

            If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
                '�ꎞ��~���ł͂Ȃ�
                Exit Try
            End If

            For Each dsp As clsDispDefect In mcls_DispDefect
                If Not dsp.ImageZoom Then Continue For
                For Each picbox As PictureBox In dsp.DefectViewInf.picImg
                    If sender Is picbox Then
                        ' �_�u���N���b�N����PictureBox�ƈ�v������A�r�摜�g��\��
                        dsp.ShowPopup(mfrm_PopImage)
                        Exit Try
                    End If
                Next
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�_�u���N���b�N�ُ�[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �r�}�b�v�X�N���[��
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub vsbMap_ValueChanged(ByVal sender As System.Object, ByVal e As System.Boolean)
        Dim value As Integer = vsbMap.Value
        Try
            If Not e Then
                SetScroll(value)
                RaiseEvent ev_ChangeScrollValue(Me, value, mcls_DispMap.MapDir) '�r�}�b�v�X�N���[���l�ύX
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[���C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v�}�E�X�X�N���[��
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub dgvMap_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Dim nValue As Integer = vsbMap.Value
        Try

            If e.Delta > 0 Then
                nValue = nValue - 3
                If nValue < vsbMap.Minimum Then nValue = vsbMap.Minimum
            Else
                nValue = nValue + 3
                If nValue > vsbMap.Maximum - vsbMap.LargeChange + 1 Then nValue = vsbMap.Maximum - vsbMap.LargeChange + 1
            End If

            vsbMap.Value = nValue

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�E�X�X�N���[���C�x���g�����ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v���T�C�Y
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub MapReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        mcls_DispMap.MapResizeH()
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I���C�x���g(�I����)
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="pntKizu">�I���r���</param>
    ''' <remarks></remarks>
    Private Sub frmTansho_SelectTcmMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Dim nLen As Integer = 0
        'Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try
            If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
                Exit Try
            End If

            'If mcls_DispCoil.GetCoilInf(pntKizu.nRec, coilinf) Then    '�R�C�����e�[�u���Ǎ�
            '    mcls_DispCoil.DspCoil(pntKizu.nRec, coilinf, Men)      '�R�C�����\��
            'End If

            DispDefectTcmImg(nMen, pntKizu)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I���C�x���g �\����={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub


    '***************************************************************************
    ' �����֐�
    '***************************************************************************
    ''' <summary>
    ''' �ݒ�l�擾
    ''' </summary>
    ''' <param name="nFormID">��ʎ��ʔԍ�</param>
    ''' <remarks></remarks>
    Private Sub GetIniSetting(ByVal nFormID As Integer)
        Try
            '�Z�N�V����������
            Dim strSection As String = HY_TANSHO & "_FORM" & nFormID

            '  ----------------------- KizuTask.ini�t�@�C������̓ǂݍ���  -----------------------

            '�\���ʔԍ�
            Dim nMenNo As Integer = CInt(IIf(tMod_IniFile.ReadIniInt(strSection, "FROM_MEN_NUM", TASKINI_NAME, 0) = 0, 0, 1))
            Select Case nMenNo
                Case 0                                                          '�\��
                    'lblFace.Text = "TOP"
                    'lblFace.BackColor = Color.GreenYellow
                Case 1                                                          '����
                    'lblFace.Text = "BOT"
                    'lblFace.BackColor = Color.HotPink
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���ʔԍ��ُ�[{0}],[{1}]", nFormID, nMenNo))
                    nMenNo = 1
                    'lblFace.Text = "TOP"
                    'lblFace.BackColor = Color.GreenYellow
            End Select
            mcls_DispMap.Men = nMenNo

            '���j�^�[�ʒu
            Dim nFormPos As Integer = tMod_IniFile.ReadIniInt(strSection, "FROM_POS_NUM", TASKINI_NAME, 0)
            Dim clsMonitor() As Screen = Screen.AllScreens()                    '���j�^�[�ꗗ
            If 0 >= nFormPos Or clsMonitor.Length < nFormPos Then                '�\�����������j�^�[���Ȃ��ꍇ
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���j�^�[�ʒu�ُ�[{0}],[{1}]", nFormID, nFormPos))
                nFormPos = 1                                                    '�v���C�}���f�B�X�v���C�ɕ\��
            End If
            m_nMonitor = nFormPos
            Me.Bounds = clsMonitor(nFormPos - 1).Bounds

            '�ʔ|�C���g�ʒu
            Dim nKensaNo As Integer = tMod_IniFile.ReadIniInt(strSection, "KENSA_NUM", TASKINI_NAME, 0)
            If nKensaNo < 0 OrElse nKensaNo > MAX_DISP_POS Then
                nKensaNo = 0
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�ʔ|�C���g�ʒu�ُ�[{0}],[{1}]", nFormID, nKensaNo))
            End If
            mcls_DispMap.KensaNo = nKensaNo

            '�����䖼��
            Dim strKensaName As String = tMod_IniFile.ReadIniStr(strSection, "KENSA_NAME", TASKINI_NAME)
            Me.Text = String.Format("[{0}] �T�����", SYSTEM_TITLE)

            '�r�}�b�v�i�s����
            Dim nMapScrollDir As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_SCROLL_DIRECT", TASKINI_NAME, 0)
            Select Case nMapScrollDir
                Case 0
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD            '�������
                Case 1
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL            '�ォ�牺
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�i�s�����ُ�[{0}],[{1}]", nFormID, nMapScrollDir))
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD
            End Select


            '�r���x��DS�ʒu
            Dim nMapSide As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_VIEW_DIRECT", TASKINI_NAME, 0)
            Select Case nMapSide
                Case 0
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL                      '����DS
                Case 1
                    mcls_DispMap.Side = EM_MAP_SIDE.REVERSE                     '����WS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r���x�����]�ُ�[{0}],[{1}]", nFormID, nMapSide))
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL
            End Select

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>
			' �r�}�b�v���ʒu�ԍ�
			Dim nMapViewWidthNo As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_VIEW_WIDTH_NO", TASKINI_NAME, 0)
			Select Case nMapViewWidthNo
				Case 0
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL                      '������1,2,�E�E�E
				Case 1
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.REVERSE                     '�E����1,2,�E�E�E
				Case Else
					mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v���ʒu�ԍ�[{0}],[{1}]", nFormID, nMapViewWidthNo))
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL
			End Select
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q <<<<<--------------------

            '�r�}�b�vDS�ʒu
            Dim nMapCell As Integer = tMod_IniFile.ReadIniInt(strSection, "MAP_CELL_DIRECT", TASKINI_NAME, 0)
            Select Case nMapCell
                Case 0
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL                     '����DS
                Case 1
                    mcls_DispMap.DSPos = EM_MAP_CELL.REVERSE                    '����WS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v���]�ُ�[{0}],[{1}]", nFormID, nMapCell))
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL
            End Select

            '�r�摜�㉺���]
            Dim nImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_TOP_BOTTOM_INF", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '���]����
                Case EM_IMAGE_TOP.REVERSE                                       '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�㉺���]�ُ�[{0}],[{1}]", nFormID, nImgVMode))
                    nImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnTB = nImgVMode
            Next

            '�r�摜���E���]
            Dim nImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_SIDE_INF", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '���]����
                Case EM_IMAGE_SIDE.REVERSE                                      '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜���E���]�ُ�[{0}],[{1}]", nFormID, nImgHMode))
                    nImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnLR = nImgHMode
            Next

            ''�r�摜90�x��]�ݒ�
            'Dim nImg90 As EM_IMAGE_90 = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_90_INF", TASKINI_NAME, 0), EM_IMAGE_90)
            'Select Case nImg90
            '    Case EM_IMAGE_90.NORMAL                                         '���]����
            '    Case EM_IMAGE_90.LOTATE                                         '���]�L��
            '    Case Else
            '        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜���E���]�ُ�[{0}],[{1}]", nFormID, nImg90))
            '        nImg90 = EM_IMAGE_90.NORMAL
            'End Select
            'For Each dsp As clsDispDefect In mcls_DispDefect
            '    dsp.IsTurn90 = nImg90
            'Next

            '�r�摜���x��DS�ʒu
            Dim nImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "IMAGE_VIEW_DIRECT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '����DS
                Case EM_MAP_SIDE.REVERSE                                        '����WS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜���x��DS�ʒu�ُ�[{0}],[{1}]", nFormID, nImgSide))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.Side = nImgSide
            Next

            'TCM�r�摜�㉺���]
            Dim nTcmImgVMode As EM_IMAGE_TOP = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_TOP_BOTTOM_INF", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nTcmImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '���]����
                Case EM_IMAGE_TOP.REVERSE                                       '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�r�摜�㉺���]�ُ�[{0}],[{1}]", nFormID, nTcmImgVMode))
                    nTcmImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnTB = nTcmImgVMode                                            '�㉺���]�ݒ�


            'TCM�r�摜���E���]
            Dim nTcmImgHMode As EM_IMAGE_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_SIDE_INF", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nTcmImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '���]����
                Case EM_IMAGE_SIDE.REVERSE                                      '���]�L��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�r�摜���E���]�ُ�[{0}],[{1}]", nFormID, nTcmImgHMode))
                    nTcmImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnLR = nTcmImgHMode                                                               '���E���]�ݒ�

            ''TCM�r�摜90�x��]�ݒ�
            'Dim nTcmImg90 As EM_IMAGE_90 = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_90_INF", TASKINI_NAME, 0), EM_IMAGE_90)
            'Select Case nTcmImg90
            '    Case EM_IMAGE_90.NORMAL                                         '���]����
            '    Case EM_IMAGE_90.LOTATE                                         '���]�L��
            '    Case Else
            '        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�r�摜���E���]�ُ�[{0}],[{1}]", nFormID, nTcmImg90))
            '        nTcmImg90 = EM_IMAGE_90.NORMAL
            'End Select
            'mfrm_PopTcmImage.IsRotate90 = nTcmImg90                                          '90�x��]�ݒ�()

            'TCM�r�摜���x��DS�ʒu
            Dim nTcmImgSide As EM_MAP_SIDE = CType(tMod_IniFile.ReadIniInt(strSection, "TCM_IMAGE_VIEW_DIRECT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nTcmImgSide
                Case EM_MAP_SIDE.NORMAL                                         '����DS
                Case EM_MAP_SIDE.REVERSE                                        '����WS
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�r�摜���x��DS�ʒu�ُ�[{0}],[{1}]", nFormID, nTcmImgSide))
                    nTcmImgSide = EM_MAP_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.Side = nTcmImgSide                                                    '�r�摜���x��DS�ʒu

            '�r�}�b�v��̌�����\��
            mcls_DispMap.KensaDsp = CBool(tMod_IniFile.ReadIniInt(strSection, "MAP_KENSA_DISP", TASKINI_NAME, 0))

            '�T������r�\���X�V���[�h
            Dim nMapUpdateMode As EM_DEFECT_MODE = CType(tMod_IniFile.ReadIniInt(strSection, "MAP_DISP_MODE", TASKINI_NAME, 0), EM_DEFECT_MODE)
            Select Case nMapUpdateMode
                Case EM_DEFECT_MODE.TRACKING                                    '�g���b�L���O�\��
                Case EM_DEFECT_MODE.REALTIME                                    '���A���^�C���\��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�V���[�h�ُ�[{0}],[{1}]", nFormID, nMapUpdateMode))
                    nMapUpdateMode = EM_DEFECT_MODE.TRACKING
            End Select
            mcls_DispMap.MapUpdateMode = nMapUpdateMode

            '�x��K�C�_���X���\��
            m_bDspAlarm = CBool(tMod_IniFile.ReadIniInt(strSection, "APPROACH_ALARM_DISP", TASKINI_NAME, 0))

            ' 20201010 ������ʒu�\���ύX --->>>
            '-------------------------------------------------------------------
            ' �r�}�b�v�̌�����ʒu
            '-------------------------------------------------------------------
            Dim nKensaRow As Integer = tMod.ReadIniInt(strSection, "MAP_KENSA_POS", TASKINI_NAME, 1)
            If nKensaRow < 1 Then                                               ' �A���_�[�t���[
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini MAP_KENSA_POS�ُ�[{0}] [{1}]", nFormID, nKensaRow))
                nKensaRow = 1
            ElseIf TANSHO_MAP_DSP_ROW_MAX < nKensaRow Then                      ' �I�[�o�[�t���[ (�d�l)
                nKensaRow = TANSHO_MAP_DSP_ROW_MAX
            End If
            mcls_DispMap.KensaPos = nKensaRow
            ' 20201010 ������ʒu�\���ύX <<<---


            '  ----------------------- KizuLocal.ini�t�@�C������̓ǂݍ��� -----------------------

            '�}�b�v�ؑ�
            Dim nLongPosInf As EM_LONG_POS = CType(tMod_IniFile.ReadIniInt(strSection, "LONG_POS_INF", LOCALINI_NAME, 0), EM_LONG_POS)
            Select Case nLongPosInf
                Case EM_LONG_POS.SMALL                                          '�\���Ԋu��
                Case EM_LONG_POS.BIG                                            '�\���Ԋu��
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ؑֈُ�[{0}],[{1}]", nFormID, nLongPosInf))
                    nLongPosInf = EM_LONG_POS.SMALL
            End Select
            mcls_DispMap.Stage = nLongPosInf

            '�I�[�o�[���C�\���ؑ�
            Dim nOverLay As Integer = tMod_IniFile.ReadIniInt(strSection, "OVERLAY_VIEW_INF", LOCALINI_NAME, 1)
            Select Case nOverLay
                Case 0                                                          '�\������
                    Overlay = False
                Case 1                                                          '�\���L��
                    Overlay = True
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�[�o�[���C�ؑֈُ�[{0}],[{1}]", nFormID, nOverLay))
                    Overlay = True
            End Select

            '�ڐ���\���ؑ�
            Dim nScale As Integer = tMod_IniFile.ReadIniInt(strSection, "SCALE_VIEW_INF", LOCALINI_NAME, 1)
            Select Case nScale
                Case 0                                                          '�\������
                    Memori = False
                Case 1                                                          '�\���L��
                    Memori = True
                Case Else
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�P�[���ؑֈُ�[{0}],[{1}]", nFormID, nScale))
                    Memori = True
            End Select

            ''�X�N���[�������ݒ�Ǎ�
            'Dim nScroll As Integer = tMod_IniFile.ReadIniInt(strSection, "SCROLL_VIEW_INF", LOCALINI_NAME, 1)
            'Select Case nScroll
            '    Case 0
            '        ScrollSync = False                                          '�X�N���[�������Ȃ�
            '    Case 1
            '        ScrollSync = True                                           '�X�N���[����������
            '    Case Else
            '        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�N���[���ؑֈُ�[{0}],[{1}]", nFormID, nScroll))
            '        ScrollSync = True                                           '�X�N���[����������
            'End Select

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�ݒ�l�擾�ُ�[{0}],[{1}]", nFormID, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r���\���G���A������
    ''' </summary>
    ''' <param name="ucDef"></param>
    ''' <param name="clr"></param>
    ''' <param name="nImgNo"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InitDefInf(ByVal ucDef As ucDefect, ByVal clr As Color, ByVal nImgNo As Integer) As clsDispDefect
        Dim typDefInf As clsDispDefect.TYP_DEFECT_DSP = Nothing
        Dim clsDispDefect As clsDispDefect = Nothing

        Try
            With typDefInf
                .Initialize()
                .nNo = nImgNo

                .tblLayoutPnl = ucDef.tblLayoutPnl

                .lblTitleL(0) = ucDef.lblImgLeft01
                .lblTitleL(1) = ucDef.lblImgLeft02

                .lblTitleR(0) = ucDef.lblImgRight01
                .lblTitleR(1) = ucDef.lblImgRight02

                .lblType = ucDef.lblType
                .lblGrade = ucDef.lblGrade

                .lblY = ucDef.lblY
                .lblKizuNo = ucDef.lblKizuNo
                .lblYName = ucDef.lblYName
                .lblKizuNoName = ucDef.lblKizuNoName

                .dgvInfo = ucDef.dgvDefInfo

                .picImg(0) = ucDef.picImg1
                .picImg(1) = ucDef.picImg2

                .lblImg(0) = ucDef.lblImg01
                .lblImg(1) = ucDef.lblImg02
                .lblCamNo = ucDef.lblCamNo

                .clrLine = clr
            End With

            clsDispDefect = New clsDispDefect(typDefInf, mcls_Param, mcls_Log, mcls_PointInf, PATH_DEFAULT_IMG)
            clsDispDefect.SetScaleConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y)
            clsDispDefect.ImageZoom = True

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�r���\���G���A������[{0}],[{1}]", m_nFormID, ex.Message))
        End Try

        Return clsDispDefect

    End Function

    ''' <summary>
    ''' �X�e�[�^�X�\���G���A������
    ''' </summary>
    ''' <param name="usStat"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InitStatusInf(ByVal usStat As ucStatus, ByVal ucCoil As ucCoil) As clsDispStatus
        Dim clsDispStatus As clsDispStatus = Nothing
        Dim typStatusDsp As clsDispStatus.TYP_STATUS_DSP = Nothing

        Try
            With typStatusDsp
                .lblGaidans = usStat.lblMsgGuaid        '�K�C�_���X
                .lblKadou = usStat.lblKadou             '�ғ����
                .lblKensa = usStat.lblKensa             '�������
                .lblKiki = usStat.lblKiki               '�@����
            End With
            clsDispStatus = New clsDispStatus(typStatusDsp, mcls_Log)

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�X�e�[�^�X�\���G���A�������ُ�[{0}],[{1}]", m_nFormID, ex.Message))
        End Try

        Return clsDispStatus

    End Function

    ''' <summary>
    ''' �R�C�����\���G���A������
    ''' </summary>
    ''' <param name="ucCoil"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InitCoilInf(ByVal ucCoil As ucCoil) As clsDispCoil
        Dim clsDispCoil As clsDispCoil = Nothing
        Dim typCoilDsp As clsDispCoil.TYP_COIL_DSP = Nothing

        Try
            With typCoilDsp
                .dgvInf1 = ucCoil.TCnt_DataGridView1
                .dgvInf2 = ucCoil.TCnt_DataGridView2
                .tblLayoutPnl = ucCoil.TableLayoutPanel1
            End With

            clsDispCoil = New clsDispCoil(typCoilDsp, mcls_Log, mcls_Param)

        Catch ex As Exception
            mcls_Log.Write(LNO.ERR, String.Format("�R�C�����\���G���A�������ُ�[{0}],[{1}]", m_nFormID, ex.Message))
        End Try

        Return clsDispCoil

    End Function

    ''' <summary>
    ''' �X�VӰ�ޕύX����(�����X�V/�ꎞ��~)
    ''' </summary>
    ''' <param name="mode">�X�VӰ��</param>
    ''' <remarks></remarks>
    Private Sub ChangeRefreshMode(ByVal mode As EM_REFRESH_MODE)
        Try
            mcls_DispMap.RefreshMode = mode                                     '���[�h�Z�b�g
            mcls_Log.Write(LNO.MSG, String.Format("����[�J�n] Minimum[{0}] Maximum[{1}] LargeChange[{2}] GetScrollMax[{3}] Value[{4}]", vsbMap.Minimum, vsbMap.Maximum, vsbMap.LargeChange, mcls_DispMap.GetScrollMax(), vsbMap.Value))
            vsbMap.Minimum = 0
            vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
                vsbMap.ValueProgramable = vsbMap.Minimum
            Else
                vsbMap.ValueProgramable = vsbMap.Maximum - vsbMap.LargeChange + 1
            End If
            mcls_Log.Write(LNO.MSG, String.Format("����[�ύX] Minimum[{0}] Maximum[{1}] LargeChange[{2}] GetScrollMax[{3}] Value[{4}]", vsbMap.Minimum, vsbMap.Maximum, vsbMap.LargeChange, mcls_DispMap.GetScrollMax(), vsbMap.Value))
            If mcls_DispMap.RefreshMode = EM_REFRESH_MODE.AUTO Then
                'TCM�I���r�͍X�V�O�ɏ���������B�����X�V���A���ɕ\�������邽��
                mcls_DispMap.ClearTcm()
                If mfrm_PopTcmImage.Visible Then                                'TCM�r�摜�g��\����
                    mfrm_PopTcmImage.CloseFrom()                                'TCM�g��摜OFF��
                End If

                m_timMap.Start()                                                '�r�}�b�v�X�V�J�n
                m_timMap_Tick(m_timMap, New System.EventArgs())                 '�r�}�b�v�\��
                m_timCoil.Start()                                               '�R�C�����X�V�J�n
                m_timCoil_Tick(m_timCoil, New System.EventArgs())               '�R�C�����\��

                '�ꎞ��~�����Ŋg��OFF
                If mfrm_PopImage.Visible Then                                   '�r�摜�g��\����
                    mfrm_PopImage.CloseFrom()                                   '�g��摜OFF��
                End If

            Else
                m_timCoil.Stop()                                                '�R�C�����X�V�I��
                m_nPauseRec = mcls_PointInf.RecDisp(mcls_DispMap.KensaNo)       '�ꎞ��~�����݃��R�[�h

            End If
            mcls_Log.Write(LNO.MSG, String.Format("����[�ؑ�] Minimum[{0}] Maximum[{1}] LargeChange[{2}] GetScrollMax[{3}] Value[{4}]", vsbMap.Minimum, vsbMap.Maximum, vsbMap.LargeChange, mcls_DispMap.GetScrollMax(), vsbMap.Value))
            SetRefreshModeControl(mcls_DispMap.RefreshMode)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�V���[�h�{�^�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v�s�Ԋu�̐ݒ�
    ''' </summary>
    ''' <param name="emStage">�ύX��̊Ԋu 1:�\��(��) 2:�\��(��)</param>
    ''' <remarks></remarks>
    Private Sub SetMapInterval(ByVal emStage As EM_LONG_POS)
        Try
            'MAP�\���Ԋu��ݒ�
            mcls_DispMap.Stage = emStage
            'mcls_PointInf.ReadHyPoint()

            '-------------------------------------------------------------------
            '�{�^���\���ύX
            '-------------------------------------------------------------------
            If EM_LONG_POS.BIG = Stage Then
                btnF03.Text = mcls_Param.MapStageBig.ToString & "m��" & mcls_Param.MapStageSmall.ToString & "m"
            Else
                btnF03.Text = mcls_Param.MapStageSmall.ToString & "m��" & mcls_Param.MapStageBig.ToString & "m"
            End If

            mcls_DispMap.DspData(Men)

            'vsbMap.Minimum = 0
            'RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            'vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
            'AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged

            'If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
            '    vsbMap.ValueProgramable = 0
            'Else
            '    vsbMap.ValueProgramable = mcls_DispMap.GetScrollMax()
            '    'vsbMap_ValueChanged(vsbMap, True)
            'End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�Ԋu�ݒ�ُ� value={0}[{1}]", emStage, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v�X�V���[�h�ɍ��킹�ăR���g���[���\����ݒ肵�܂�
    ''' </summary>
    ''' <param name="mode">�X�V���[�h</param>
    ''' <remarks></remarks>
    Private Sub SetRefreshModeControl(ByVal mode As EM_REFRESH_MODE)
        Try
            Select Case mode
                Case EM_REFRESH_MODE.AUTO                                       '�����X�V�̏ꍇ
                    vsbMap.Enabled = False
                    'If mcls_DispMap.MapUpdateMode = EM_DEFECT_MODE.TRACKING Then
                    '    lblKensaPoint.Visible = True
                    'End If
                    btnF01.Text = "�ꎞ��~"
                    btnF01.BackColor = Color.FromKnownColor(KnownColor.Control)
                    btnF01.ForeColor = Color.Black
                    lblMode.Text = "�����X�V��"
                    lblMode.BackColor = Color.Blue
                    lblMode.ForeColor = Color.Yellow
                    UcDefect1.lblDefTitle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    UcDefect2.lblDefTitle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                    UcDefect1.TitleColor = g_ColorNewKizu
                    UcDefect2.TitleColor = g_ColorOldKizu
                    UcDefect1.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect2.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect1.lblDefTitle.Tag = False
                    UcDefect2.lblDefTitle.Tag = False


                Case EM_REFRESH_MODE.PAUSE                                      '�ꎞ��~�̏ꍇ
                    vsbMap.Enabled = True
                    'If mcls_DispMap.MapUpdateMode = EM_DEFECT_MODE.TRACKING Then
                    '    lblKensaPoint.Visible = False
                    'End If
                    btnF01.Text = "��~����"
                    btnF01.BackColor = Color.Red
                    btnF01.ForeColor = Color.Yellow
                    lblMode.Text = "�ꎞ��~��"
                    lblMode.BackColor = Color.Red
                    lblMode.ForeColor = Color.White

                    UcDefect1.Title = "���݂̑I���r"
                    UcDefect2.Title = "��O�̑I���r"
                    UcDefect1.lblDefTitle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    UcDefect2.lblDefTitle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    UcDefect1.TitleColor = g_ColorStop1st
                    UcDefect2.TitleColor = g_ColorStop2nd
                    UcDefect1.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect2.lblDefTitle.ForeColor = Color.FromArgb(0, 0, 192)
                    UcDefect1.lblDefTitle.Tag = False
                    UcDefect2.lblDefTitle.Tag = False

            End Select

            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then            '������ɃR�C�����i�ޏꍇ
                vsbMap.ValueProgramable = vsbMap.Maximum - vsbMap.LargeChange + 1
            Else                                                                '�������ɃR�C�����i�ޏꍇ
                vsbMap.ValueProgramable = 0
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�V���[�h���R���g���[���ݒ�ُ� mode={0}[{1}]", mode, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �\�����x��������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitLabel()
        Try
            UcDefect1.lblDefTitle.Tag = False
            UcDefect2.lblDefTitle.Tag = False
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\�����x��������[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �r�}�b�v�X�N���[������
    ''' </summary>
    ''' <param name="nValue">�X�N���[����</param>
    ''' <remarks></remarks>
    Private Sub SetScroll(ByVal nValue As Integer)
        Try

            If mcls_DispMap.RefreshMode = EM_REFRESH_MODE.AUTO Then Exit Sub '�����X�V���̓X�N���[�������Ȃ�

            Dim value As Integer = nValue
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then value = vsbMap.Maximum - (vsbMap.LargeChange - 1) - nValue '�i�s����

            Dim nMaxRowNum As Integer
            mcls_DispMap.SetMapData(Men, -value, nMaxRowNum)

            'dgvMap.Update()
            dgvMap.Refresh()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�摜�g��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispDefectImg()
        Try

            For Each dsp As clsDispDefect In mcls_DispDefect
                If dsp.DefectViewInf.nNo = mfrm_PopImage.DispImgNo Then
                    dsp.ShowPopup(mfrm_PopImage)
                    Exit Try
                End If
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[���C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�摜�g��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispDefectTcmImg(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Try
            ShowPopupTcm(pntKizu.nRec, nMen, pntKizu.nPnt, mfrm_PopTcmImage)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[���C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

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
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton

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
				'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
				'Call SelectF09()
				Call SelectF09()
				'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
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
    ''' F1:�X�V���[�h�{�^��[�ꎞ��~/�����X�V]
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF01()
        Try
            mcls_DispMap.checkcheck = True
            SetMapInterval(mcls_DispMap.Stage)                               '����Ԋu�ݒ�

            '�X�V���[�h�ύX
            If EM_REFRESH_MODE.AUTO = mcls_DispMap.RefreshMode Then             '�����X�V
                '�ꎞ��~�ɂ���
                ChangeRefreshMode(EM_REFRESH_MODE.PAUSE)
            Else                                                                '�ꎞ��~
                '�����X�V�ɂ���
                ChangeRefreshMode(EM_REFRESH_MODE.AUTO)
                If mfrm_PopImage.Visible Then                               '�r�摜�g��\����
                    mfrm_PopImage.CloseFrom()                               '�g��摜OFF��
                End If
            End If
            mcls_DispMap.checkcheck = False
            dgvMap.Refresh()
            dgvMap.Focus()
            RaiseEvent ev_ChangeRefreshMode(Me, mcls_DispMap.RefreshMode)       '�X�V���[�h�ύX
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�V���[�h�{�^�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �X�N���[�������{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF02()
        Try
            dgvMap.Focus()
            Me.ScrollSync = Not m_bScrollSync
            RaiseEvent ev_ChangeScrollSync(Me, Me.ScrollSync)                   '�X�N���[�������ύX
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�N���[�������{�^�������ُ�[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' F3:�}�b�v�ؑփ{�^��[5m��30m]/[30m��5m]
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF03()
        Try
            If EM_LONG_POS.SMALL = Stage Then
                SetMapInterval(EM_LONG_POS.BIG)
            Else
                SetMapInterval(EM_LONG_POS.SMALL)
            End If

            ' ���݃}�b�v�ɕ\�����Ă���ʒu��V����MAP�\���Ԋu�ł��\���ł���悤�ɂ���B
            Dim nValue As Integer = (vsbMap.Value)
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                '�i�s�������������̏ꍇ�́A���̈ʒu����ɂ���
                nValue = vsbMap.Maximum - vsbMap.LargeChange - nValue + 1
            End If
            'MAP�\���Ԋu�̔䗦
            Dim nMag As Integer = CInt(mcls_Param.MapStageBig / mcls_Param.MapStageSmall)
            If EM_LONG_POS.BIG = Stage Then
                ' MAP�Ԋu��������
                nValue = nValue \ nMag
            Else
                ' MAP�Ԋu���偨��
                nValue = nValue * nMag
            End If
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                nValue = mcls_DispMap.GetScrollMax() - nValue
            End If
            If mcls_DispMap.GetScrollMax() < nValue Then
                nValue = mcls_DispMap.GetScrollMax()
            ElseIf 0 > nValue Then
                nValue = 0
            End If
            vsbMap.Minimum = 0
            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
            vsbMap.ValueProgramable = nValue
            SetScroll(nValue)
            AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged

            mcls_DispMap.ReDrawDeepList()

            dgvMap.Focus()
            RaiseEvent ev_ChangeMapInterval(Me, Stage)          '�}�b�v�Ԋu�ύX
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�ؑփ{�^�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F4:�r�������{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF04()
        Try
            '�r�������\��
            Dim frm As New frmMixedRate(mcls_Log)

            '�r�������\��
            frm.ShowDialog()
            frm.Dispose()
            frm = Nothing
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�������{�^�������ُ�[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' F5:���׌x������{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF05()
        Try
            '���׌x��o�͉����v���𑗐M
            Dim bRet As DialogResult
            Call DspMsg("���׌��o�x���OFF���܂����H", bRet)
            If System.Windows.Forms.DialogResult.OK = bRet Then
                Call SendAlarmMail(1)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���׌x������{�^�������ُ�[{0}]", ex.Message))
        End Try

    End Sub
    ''' <summary>
    ''' �m�F���b�Z�[�W�\��
    ''' </summary>
    ''' <param name="strMsg">�\�����b�Z�[�W</param>
    ''' <param name="bRet">�߂�l</param>
    ''' <remarks></remarks>
    Public Overridable Sub DspMsg(ByVal strMsg As String, ByRef bRet As DialogResult)

        Dim frmMsg As tClassForm.frmPop_MsgBox
        frmMsg = New tClassForm.frmPop_MsgBox(strMsg, "�m�F", , True, True, System.Windows.Forms.DialogResult.OK, System.Windows.Forms.DialogResult.Cancel)

        '���b�Z�[�W�\��
        bRet = frmMsg.ShowDialog()
        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' F6:���u�x������{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF06()
        Try
            '�x��o�͉����v���𑗐M
            SendAlarmMail(0)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�x������{�^�������ُ�[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' F7:�I�[�o�[���C�\���E��\���ؑփ{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF07()
        Try
            ' �I�[�o�[���C�̕\���E��\����Ԃ𔽓]������
            Overlay = Not Overlay

            dgvMap.Focus()
            RaiseEvent ev_ChangeOverlyVisible(Me, Overlay)   '�I�[�o�[���C�\���ύX
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�[�o�[���C�\���E��\���ؑփ{�^�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F8:�ڐ���\���E��\���ؑփ{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF08()
        Try
            ' �ڐ���̕\���E��\����Ԃ𔽓]������
            Memori = Not Memori
            dgvMap.Focus()
            RaiseEvent ev_ChangeMemoryVisible(Me, Memori)   '�X�P�[���\���ύX
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�ڐ���\���E��\���ؑփ{�^�������ُ�[{0}]", ex.Message))

        End Try

    End Sub


	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	''' <summary>
	''' F9:�������{�^��
	''' </summary>
	''' <remarks></remarks>
	Private Sub SelectF09()
		Try
			If mfrm_LongPdList.Visible = True Then
				' �\�����͉������Ȃ�
				Exit Sub
			End If

			' �\�������R�[�h
			Dim nRecs() As Integer = mcls_DispMap.MapDispRec()

			' �\���ΏۃR�C���̊Ǘ�No�ƃR�C��No���擾
			Dim lstManagementNo As New List(Of String)
			Dim lstCoilNo As New List(Of String)

			For ii As Integer = 0 To nRecs.Length() - 1
				If 0 = nRecs(ii) Then
					Exit For
				End If

				lstManagementNo.Add(mcls_PointInf.KizukenNo(nRecs(ii)).TrimEnd(CType(vbNullChar, Char)).TrimEnd)

				Dim typCoil As COINFO_BASE = Nothing
				If False = mcls_DispCoil.GetCoilInf(nRecs(ii), typCoil) Then
					Exit Try
				End If

				lstCoilNo.Add(tMod_Common.ByteToString(typCoil.base.cCoilNoIn).TrimEnd(CType(vbNullChar, Char)).TrimEnd)
			Next

			'���������׈ꗗ


			'���������׈ꗗ�\��
			If True = mfrm_LongPdList.PreDispProc(lstManagementNo, lstCoilNo, IIf(mcls_DispMap.Side = EM_MAP_SIDE.REVERSE, True, False)) Then
				mfrm_LongPdList.Visible = True
			End If

		Catch ex As Exception
			mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���������׈ꗗ�{�^�������ُ�[{0}]", ex.Message))
		End Try

	End Sub
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	''' <summary>
	''' F10:��ʈ���{�^��
	''' </summary>
	''' <remarks></remarks>
	Private Sub SelectF10()
        Try
            Call PrintOut()
            dgvMap.Focus()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��ʈ���{�^�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F11:��ʕۑ��{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF11()
        Try
            Call HardCopy(IMG_PATH, "�T�����_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            dgvMap.Focus()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("��ʕۑ��{�^�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' F12:�I�y���[�^��ʃ{�^��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF12()
        Try
            '�Ƃ肠�����I�y���[�^��ʂ��I������B
            Dim que As COMMON_QUE_S = Nothing
            que.inisialize()
            que.nLineNo = KizuMod.GetLineID()
            ' �I�y���[�^��ʏI�� 
            que.nEventNo = FACT_KS_END
            ''���[�����M 
            Call KizuLibCLI.KizuMod.Send_Mail(SO_GAMENN, ".", que)

            System.Threading.Thread.Sleep(100)      '�X���[�v[100ms]

            '�I�y���[�^��ʃ{�^��
            Dim sCmd As String      ' �N���R�}���h
            sCmd = String.Format("{0}{1}", START_FORM_POS, m_nMonitor)
            Dim newProcess As System.Diagnostics.Process                        ' RpiViewer �v���Z�X
            Try
                newProcess = Process.Start(SO_GAMENN, sCmd)

            Catch ex As Exception
                Log(LNO.ERR, String.Format("�T�� �I�y���[�^��ʃ{�^�������ُ�[{0}]", ex.Message))
                Exit Sub
            Finally

            End Try

            '�^�X�N�I��
            EndNotice()

            'If Not CBool(tClass_TaskCheck.StartExe("SO_GAMENN")) Then
            '    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�y���[�^��ʋN���ُ�[{0}]", "SO_GAMENN"))
            'Else
            '    '�^�X�N�I��
            '    EndNotice()
            'End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�y���[�^��ʃ{�^�������ُ�[{0}]", ex.Message))
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
    ''' �x������v�����M
    ''' </summary>
    ''' <param name="nVal"></param>
    ''' <remarks></remarks>
    Private Sub SendAlarmMail(ByVal nVal As Integer)
        '�x�����
        Dim que As COMMON_QUE_L = Nothing

        '�x������ʒm
        que.inisialize()
        que.nEventNo = FACT_TO_DEFECT_20
        que.nLineNo = KizuMod.GetLineID()
        que.data(0) = nVal
        que.data(1) = KensaNo

        If Not LOCAL Then
            '�{��
            If 0 <> KizuMod.Send_Mail(TO_DEFECT, KizuMod.Tokatu, que) Then
                KizuMod.SysLog(SYSNO_MAIL_SNED_ERR, String.Format("[{0}]", HY_TANSHO))
            End If
        Else
            '�e�X�g
            If 0 <> KizuMod.Send_Mail(TO_DEFECT, ".", que) Then
                KizuMod.SysLog(SYSNO_MAIL_SNED_ERR, String.Format("[{0}]", HY_TANSHO))
            End If
        End If

    End Sub


    '***************************************************************************
    ' ���̑�
    '***************************************************************************
    ''' <summary>
    ''' �r���擾
    ''' </summary>
    ''' <param name="nRec">���R�[�h(1�I���W��)</param>
    ''' <param name="nPnt">�r���e�[�u���ւ̃|�C���^�ʒu(1�I���W��)</param>
    ''' <param name="typDefInf">�r���</param>
    ''' <param name="clslog">���O</param>
    ''' <returns>TRUE:����,FALSE:���s</returns>
    ''' <remarks></remarks>
    Public Shared Function GetDefect(ByVal nRec As Integer, ByVal nPnt As Integer, ByRef typDefInf As DSP_TYP_DEFECTINF, ByVal clslog As tClass_LogManager) As Boolean
        Dim nReadAddr As Integer = 0
        Dim nRetc As ValueType = 0
        Dim bRet As Boolean = False

        Try
            '�������R�[�h
            If nRec <= 0 Or nRec > TBL_HY_REC_MAX Then
                clslog.Write(LNO.ERR, String.Format("[{0}]�������R�[�h[{1}]", TBL_HY_DEFECTINF_NAME, nRec))
                Exit Try
            End If

            '�����|�C���^
            If nPnt <= 0 Or nPnt > MAX_MAP_DEFECT_NUM Then
                clslog.Write(LNO.ERR, String.Format("[{0}]�����|�C���^[{1}]", TBL_HY_DEFECTINF_NAME, nPnt))
                Exit Try
            End If

            '�r���|�C���^�ǂݍ���[���L������]
            nReadAddr = SIZE_DSP_MAP_DEFECTINF * (nRec - 1) + SIZE_DEFECTINF_HEADER + SIZE_DSP_TYP_DEFECTINF * (nPnt - 1)
            typDefInf = CType(ksMemReadMap(TBL_HY_DEFECTINF_NAME, nReadAddr, tMod_MemSet.SizeOf(typDefInf), GetType(DSP_TYP_DEFECTINF), nRetc), DSP_TYP_DEFECTINF)
            If Not CBool(nRetc) Then
                clslog.Write(LNO.ERR, String.Format("[{0}]�r���|�C���^�ǂݍ��ݎ��s[{1}]", TBL_HY_DEFECTINF_NAME, nReadAddr))
                Exit Try
            End If

            '�擾����
            bRet = True
        Catch ex As Exception
            clslog.Write(LNO.ERR, String.Format("�r���\���ُ�[{0}] nRec[{1}] pnt[{2}] nReadAddr[{3}]", ex.Message, nRec, nPnt, nReadAddr))
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �O�H���r���擾
    ''' </summary>
    ''' <param name="nRec">���R�[�h(1�I���W��)</param>
    ''' <param name="nPnt">�r���e�[�u���ւ̃|�C���^�ʒu(1�I���W��)</param>
    ''' <param name="typDefInf">�r���</param>
    ''' <param name="clslog">���O</param>
    ''' <returns>TRUE:����,FALSE:���s</returns>
    ''' <remarks></remarks>
    Public Shared Function GetMaeDefect(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nPnt As Integer, ByRef typDefInf As MAE_INF, ByVal clslog As tClass_LogManager) As Boolean
        Dim nReadAddr As Integer = 0
        Dim nRetc As ValueType = 0
        Dim bRet As Boolean = False

        Try
            '�������R�[�h
            If nRec <= 0 Or nRec > TBL_HY_REC_MAX Then
                clslog.Write(LNO.ERR, String.Format("[{0}]�������R�[�h[{1}]", TBL_HY_MAE_INF_NAME, nRec))
                Exit Try
            End If

            '�����|�C���^
            If nPnt <= 0 Or nPnt > CInt(COIL_LEN_MAX / MAP_REPRES_MIN) Then
                clslog.Write(LNO.ERR, String.Format("[{0}]�����|�C���^[{1}]", TBL_HY_MAE_INF_NAME, nPnt))
                Exit Try
            End If

            '�r���|�C���^�ǂݍ���[���L������]
            nReadAddr = SIZE_REC * (nRec - 1) + SIZE_MEN * nMen + 4 + SIZE_MAE_INF * (nPnt - 1)
            typDefInf = CType(ksMemReadMap(TBL_HY_MAE_INF_NAME, nReadAddr, tMod_MemSet.SizeOf(typDefInf), GetType(MAE_INF), nRetc), MAE_INF)
            If Not CBool(nRetc) Then
                clslog.Write(LNO.ERR, String.Format("[{0}]�r���|�C���^�ǂݍ��ݎ��s[{1}]", TBL_HY_MAE_INF_NAME, nReadAddr))
                Exit Try
            End If

            '�擾����
            bRet = True
        Catch ex As Exception
            clslog.Write(LNO.ERR, String.Format("�O�H���r���\���ُ�[{0}] nRec[{1}] pnt[{2}] nReadAddr[{3}]", ex.Message, nRec, nPnt, nReadAddr))
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' TCM�r�g��摜�\��
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Sub ShowPopupTcm(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nPnt As Integer, ByVal clsPopup As frmDefectImage2)
        Dim clsImg(1 - 1) As Image
        Dim tmpTitle(1 - 1) As String
        Dim nRetc As ValueType = 0
        Try
            Dim typDefInf As MAE_INF = Nothing
            If Not GetMaeDefect(nRec, nMen, nPnt, typDefInf, mcls_Log) Then
                Exit Sub
            End If
            For ii As Integer = 0 To clsImg.Length - 1
                clsImg(ii) = GetTcmImage(typDefInf, CType(ii + 1, EM_IMAGE_NO), tmpTitle(ii))
            Next
            '---------------------------
            '�r�摜�g��\��
            '---------------------------
            With clsPopup
                .DispImgNo = 1                                      '�摜No.
                .SetVisibleMemory(Memori)                           '�������̕\���L���ݒ�
                .SetVisibleOverlay(Overlay)                         '�I�[�o�[���C�̕\���L���ݒ�
                .SetColor = Color.Cyan                              '�������A�I�[�o���C�F

                ''�r���
                .SetDefectData(typDefInf.nKizuNo, typDefInf.ff, typDefInf.inf.data, clsImg, tmpTitle)
                '�\���{��
                .ZoomRate = CInt((100.0 * typDefInf.inf.data.w_res / m_nTcmImageWide) * 100.0)
                '�ڐ���`��ڍ�
                .SetMemoryConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X * 2, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y * 2)
                .DspUpDate(True)                                                          '�\��
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�r�g��摜�\���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �摜�擾(�T�����)
    ''' </summary>
    ''' <param name="typDefInf">�r���</param>
    ''' <param name="emImgNo">�摜�ʒu</param>
    ''' <param name="strTitle">�r�^�C�g��</param>
    ''' <returns>�摜�C���[�W</returns>
    ''' <remarks></remarks>
    Private Function GetTcmImage(ByVal typDefInf As MAE_INF, ByVal emImgNo As EM_IMAGE_NO, ByRef strTitle As String) As Image
        Dim imgData As Image = Nothing

        Try
            '������
            strTitle = ""
            Dim strImgPath As String = Nothing

            Select Case emImgNo
                Case EM_IMAGE_NO.S
                    strImgPath = typDefInf.inf.image_path_1
                    strTitle = ""
                Case EM_IMAGE_NO.R1
                    strImgPath = typDefInf.inf.image_path_2
                    strTitle = ""
                Case EM_IMAGE_NO.R2
                    strImgPath = typDefInf.inf.image_path_3
                    strTitle = ""
                Case EM_IMAGE_NO.R3
                    strImgPath = typDefInf.inf.image_path_4
                    strTitle = ""
                Case Else
                    strTitle = String.Empty
                    Return Nothing
            End Select

            If 0 = strImgPath.Length Then
                '�摜�ۑ��p�X�����w��(�摜�t�@�C���Ȃ�)
                Return Nothing
            End If

            Return tClass_BmpJpeg.BmpFileLoad(strImgPath)                   ' �摜�ǂݍ���

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�摜�擾�ُ�[{0}]", ex.Message))
        End Try

        Return imgData
    End Function

End Class
