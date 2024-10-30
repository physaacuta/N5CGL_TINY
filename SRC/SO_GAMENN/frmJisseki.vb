'*********************************************************************************
' �������щ��
'	[Ver]
'		Ver.01    2012/09/01  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tClass.tClass_LogManager

Public Class frmJisseki
    '---------------------------------------------------------------------------
    ' �񋓑̒�`
    '---------------------------------------------------------------------------
    Public Enum EM_DIV_FUNC_TYPE
        DIV_SELECT_0 = 0            '�t�@���N�V�����{�^��TYPE_0�i�ʏ�j
        DIV_SELECT_1 = 1            '�t�@���N�V�����{�^��TYPE_1�i�T�u���j���[�j
    End Enum


    '---------------------------------------------------------------------------
    ' �\���̒�`
    '---------------------------------------------------------------------------
    '�t�@���N�V�����{�^���؂�ւ��O���ۑ��p�\����
    Private Structure FUNC_INF
        Dim strText As String       '�t�@���N�V�����{�^������
        Dim bEnabled As Boolean     '�t�@���N�V�����{�^���L���^����
        Public Sub initialize()
            strText = ""
            bEnabled = False
        End Sub
    End Structure

    Private Structure MENU_INF
        Dim func() As FUNC_INF
        Public Sub initialize(ByVal n As Integer)
            ReDim func(n)
            For ii As Integer = 0 To func.Length - 1
                func(ii).initialize()
            Next
        End Sub
    End Structure


    '�摜�`��
    Public Enum EM_DSP_IMAGE_FORMAT
        nJpeg = 0       'JPEG
        nBmp            'BMP
    End Enum

    '---------------------------------------------------------------------------
    ' �萔��`
    '---------------------------------------------------------------------------
    Private Const RSC_IMG_DEFAULT As String = "SO_GAMENN.�r����.bmp"       ' �f�t�H���g�摜�̃��[�X�p�X
    Private Const KEY_OVERLAY_VISIBLE As String = "OVERLAY_VISIBLE"        ' KizuLocal.ini�L�[�� �I�[�o�[���C���X�P�[���\���L��
    'Private Const KEY_SCALE_VISIBLE As String = "SCALE_VISIBLE"            ' KizuLocal.ini�L�[�� �X�P�[���\���L��

    '---------------------------------------------------------------------------
    ' �����o�ϐ�
    '---------------------------------------------------------------------------
    Private m_nMapInterval As Integer = -1                                  ' �}�b�v�Ԋu(-1:�������,0:�ŏ��Ԋu,1:�ő�Ԋu)
    Private mcls_Param As clsParamManager                                   ' �p�����[�^�Ǘ�
    Private mcls_PreParam As clsPreParamManager                             ' �O�H���p�����[�^�Ǘ��N���X

    Private mcls_DispMap As clsDispMap                                      ' �r�}�b�v�\���N���X
    Private mcls_DataMap As clsDataMap                                      ' �r�}�b�v�f�[�^�N���X
    Private mcls_DispDefect() As clsDispDefect                              ' �r���\���N���X
    Private mcls_DispCoil As clsDispCoil                                    ' �R�C�����\���N���X

    ' �T�u���
    Private mfrm_Search As frmSearch                                        ' ���ь������
    Private mfrm_PopImage As frmDefectImage                                 ' �r�g��摜�\��
    Private mfrm_PopTcmImage As frmDefectImage2                             '�r�g��摜�\��(TCM�p)
    '    Private m_typMap As clsDispMap.TYP_MAP_INF                              ' �r�}�b�v
    Private m_nMenuMode As Integer                                          ' ���j���[��ʏ��
    Private m_typMenu() As MENU_INF

    'Private m_nSelectMen As Integer                                         ' ���߂��r�I���(0:�\�A1:��)

    Private m_emImageTop(NUM_MEN - 1) As EM_IMAGE_TOP                       ' �摜�̏㉺���]
    Private m_emImageSide(NUM_MEN - 1) As EM_IMAGE_SIDE                     ' �摜�̍��E���]

    Private m_nTcmImageWide As Integer = 25                                 'TCM�r�摜�\���]�[���̉���

#Region "�v���p�e�B"
    '---------------------------------------------------------------------------
    ' �v���p�e�B
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' �r�}�b�v�̃X�N���[������
    ''' </summary>
    ''' <returns>False:�� True:��</returns>
    ''' <remarks></remarks>
    Public Property ScrollDrection() As EM_MAP_SCROOL
        Get
            ' ���ʒP�ʂ̕������擾����Ȃ�Amcls_DispMap�ł͂Ȃ��A��p�ŕϐ������ׂ�
            ' �ŏI�I�ɂ͏C������
            Return mcls_DispMap.MapDir
        End Get
        Set(ByVal value As EM_MAP_SCROOL)
            mcls_DispMap.MapDir = value
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
    ''' �\���ʎ擾
    ''' </summary>
    ''' <returns>0:�\ 1:��</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Men() As Integer
        Get
            Return mcls_DispMap.Men
        End Get
    End Property

    ''' <summary>
    ''' �I�[�o�[���C���ڐ���\���ݒ�ON/OFF
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
                dsp.MemoriVisible = value
            Next

            ' �g��摜 �̃I�[�o�[���C���ڐ���\����ݒ�
            mfrm_PopImage.SetVisibleOverlay(value)
            mfrm_PopImage.SetVisibleMemory(value)
            ' �g��摜(TCM) �̃I�[�o�[���C�\����ݒ�
            mfrm_PopTcmImage.SetVisibleOverlay(value)
            mfrm_PopTcmImage.SetVisibleMemory(value)

            ' ���̑��ύX����Ώۂ�����΁A�����ɋL�q����

            ' �ؑփ{�^���̐F�ύX����
            If EM_DIV_FUNC_TYPE.DIV_SELECT_1 = m_nMenuMode Then
                If value Then
                    ' �\�����̃{�^���F�ݒ�
                    btnF06.BackColor = Color.Cyan
                Else
                    ' ��\�����̃{�^���F�ݒ�
                    btnF06.BackColor = Color.FromKnownColor(KnownColor.Control)
                End If
            End If

        End Set
    End Property

    '''' <summary>
    '''' �ڐ���\���ݒ�ON/OFF
    '''' </summary>
    '''' <value>True�F�\���AFalse�F��\��</value>
    '''' <returns>True�F�\���AFalse�F��\��</returns>
    '''' <remarks></remarks>
    'Public Property Memori() As Boolean
    '    Get
    '        Return mcls_DispDefect(0).MemoriVisible
    '    End Get
    '    Set(ByVal value As Boolean)

    '        ' mcls_DispDefect �̖ڐ���\����ݒ�
    '        For Each dsp As clsDispDefect In mcls_DispDefect
    '            dsp.MemoriVisible = value
    '        Next

    '        ' �g��摜 �̖ڐ���\����ݒ�
    '        mfrm_PopImage.SetVisibleMemory(value)

    '        ' ���̑��ύX����Ώۂ�����΁A�����ɋL�q����

    '        ' �ؑփ{�^���̐F�ύX����
    '        If EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode Then
    '            If value Then
    '                ' �\�����̃{�^���F�ݒ�
    '                btnF07.BackColor = Color.Cyan
    '            Else
    '                ' ��\�����̃{�^���F�ݒ�
    '                btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)
    '            End If
    '        End If

    '    End Set
    'End Property


#End Region

    '///////////////////////////////////////////////////////////////////////////
    '���\�b�h
    '///////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()
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

        Dim sw As New Stopwatch
        sw.Reset()
        sw.Start()

        Try
            '-------------------------------------------------------------------
            ' �����o������
            '-------------------------------------------------------------------
            mcls_Param = New clsParamManager(gcls_Log)                                  ' �p�����[�^�Ǘ�
            mcls_PreParam = New clsPreParamManager(gcls_Log)                            ' �O�H���p�����[�^�Ǘ�
            mfrm_Search = New frmSearch(mcls_Param, mcls_PreParam, gcls_Log)                           ' �������

            '-----------------------------------------------
            '�R�C�����
            '-----------------------------------------------
            mcls_DispCoil = InitCoilInf(UcCoil1)

            '-----------------------------------------------
            '�r�g��摜���
            '-----------------------------------------------
            Dim bPicDirect As Boolean = CBool(tMod.ReadIniInt(SO_GAMENN, "DEFIMAGE_DIRECT", TASKINI_NAME, 0))
            mfrm_PopImage = New frmDefectImage(gcls_Log, mcls_Param, RSC_IMG_DEFAULT, SO_GAMENN, JISSEKI_DSP_KIRI_IMAGE_MAX, bPicDirect)
            mfrm_PopImage.Owner = Me

            '-----------------------------------------------
            'TCM�r�摜���
            '-----------------------------------------------
            ' �^�X�N��(HY_TANSHO) ����ini���Z�N�V��������
            mfrm_PopTcmImage = New frmDefectImage2(gcls_Log, mcls_PreParam, RSC_IMG_DEFAULT, SO_GAMENN, 1, bPicDirect)
            mfrm_PopTcmImage.Owner = Me

            '-----------------------------------------------
            '�r�\��
            '-----------------------------------------------
            ReDim mcls_DispDefect(DSP_DEFECT_MAX - 1)
            mcls_DispDefect(0) = InitDefInf(UcDefect1, g_ColorNewKizu, 1)    ' �ŐV�r
            mcls_DispDefect(1) = InitDefInf(UcDefect2, g_ColorOldKizu, 2)    ' 1�O���r

            '-----------------------------------------------
            '�}�b�v�\��
            '-----------------------------------------------
            Dim typMapInf As clsDispMap.TYP_MAP_INF = Nothing
            With typMapInf
                .dgvMap = dgvMap                                            '�\���r�}�b�v
                .dgvDeep = dgvDeep                                          '�r�[�����X�g
                .dgvTcm = dgvOth1                                           'TCM���X�g
                .vsbMap = vsbMap                                            '�X�N���[���o�[
            End With
            mcls_DispMap = New clsDispMap(typMapInf, mcls_Param, mcls_PreParam, gcls_Log)
            mcls_DispMap.Men = 0

            vsbMap.Enabled = False
            vsbMap.Value = 0

            ''�r�}�b�v�̃��C���F�ݒ�
            'lblSharTitle.ForeColor = g_ColorShar                            '�V���[�J�b�g��
            'lblYousetuTitle.ForeColor = g_ColorYousetu                      '�n�ڐ�
            '�\�ʂ���\���J�n
            'm_nSelectMen = 0

            '-------------------------------------------------------------------
            ' �\���̏�����
            '-------------------------------------------------------------------
            mcls_DataMap = New clsDataMap(mcls_Param, mcls_PreParam, gcls_Log)               ' �r�}�b�v�f�[�^

            '�t�@���N�V�����{�^����񏉊��ݒ�
            ReDim m_typMenu(1)
            FunctionKey_Init()

            '-----------------------------------------------
            '�C�x���g�̐ݒ�
            '-----------------------------------------------
            '�}�b�v�����X�V
            AddHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmJisseki_SelectMapCellAuto
            '�}�b�v�����X�V(�f�[�^�������ꍇ������)
            AddHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmJisseki_SelectMapCellAutoDspInit
            '�}�b�v�I��
            AddHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmJisseki_SelectMapCellClick
            AddHandler mcls_DispMap.SelectMapTcmCell_Click, AddressOf frmJisseki_SelectTcmMapCellClick
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���щ�� �R���X�g���N�^���ɗ�O [{0}]", ex.Message))
        End Try

        sw.Stop()
        gcls_Log.Write(tClass_LogManager.LNO.INF, String.Format("frmJisseki new ����[{0}]", sw.Elapsed))

    End Sub

    '///////////////////////////////////////////////////////////////////////////
    '���C�����W���[���Ăяo���֐�(�O���ďo��)
    '///////////////////////////////////////////////////////////////////////////
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�]�[���̐ݒ�ُ� Width={0}[{1}]", nWidth, ex.Message))
        End Try
    End Sub


    '***************************************************************************
    ' �C�x���g�n���h���֐�
    '***************************************************************************
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Try

            mcls_DispCoil.Load()

            Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
            Me.Location = s.Bounds.Location
            Me.Height = Me.Owner.Height
            Me.Width = Me.Owner.Width

            If NOA_ONLINE <> g_NOA Then                                         ' �f�o�b�O��
                ' �t�H�[�����
                Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
                Me.tProp_EndButtonEnable = False
                Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' �t�H�[���T�C�Y�ύX�\
            End If

            ' ��ʃ^�C�g���Z�b�g
            SetJissekiTitle(Men)

            While (Not mcls_Param.ReadAllParam())                               ' �p�����[�^�ǂݍ��߂�܂ŁA�S��
                System.Threading.Thread.Sleep(1000)
            End While
            While (Not mcls_PreParam.ReadAll())                               ' �p�����[�^�ǂݍ��߂�܂ŁA�S��
                System.Threading.Thread.Sleep(1000)
            End While

            ' �e�\���Ԋu�ݒ�
            mcls_DataMap.Interval(EM_LONG_POS.SMALL) = mcls_Param.MapStageSmall
            mcls_DataMap.Interval(EM_LONG_POS.BIG) = mcls_Param.MapStageBig

            mcls_DispMap.SelectColor(0) = g_ColorStop1st
            mcls_DispMap.SelectColor(1) = g_ColorStop2nd

            ' �\���Ԋu���ɂċN��
            SetMapInterval(EM_LONG_POS.SMALL)

            '-------------------------------------------------------------------
            ' �I�[�o�[���C�̕\���ݒ�
            '-------------------------------------------------------------------
            Dim nOverLay As Integer = tMod.ReadIniInt(SO_GAMENN, KEY_OVERLAY_VISIBLE, LOCALINI_NAME, 1)
            Select Case nOverLay
                Case 0
                    Overlay = False
                Case 1
                    Overlay = True
                Case Else
                    Overlay = True
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�[�o�[���C�ؑֈُ�[{0}]", nOverLay))
            End Select

            ''-------------------------------------------------------------------
            '' �X�P�[���̕\���ݒ�
            ''-------------------------------------------------------------------
            'Dim nScale As Integer = tMod.ReadIniInt(SO_GAMENN, KEY_SCALE_VISIBLE, LOCALINI_NAME, 1)
            'Select Case nScale
            '    Case 0
            '        Memori = False
            '    Case 1
            '        Memori = True
            '    Case Else
            '        Memori = True
            '        gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�P�[���ؑֈُ�[{0}]", nScale))
            'End Select

            '-------------------------------------------------------------------
            ' �ݒ�t�@�C���Ǎ�(KizuTask.ini�ǂݍ���)
            '-------------------------------------------------------------------
            GetIniSetting(Men)

            '-------------------------------------------------------------------
            ' �\��������
            '-------------------------------------------------------------------
            DspInit()

            AddHandler TableLayoutPanel4.Resize, AddressOf MapReSize
            MapReSize(Nothing, Nothing)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���щ�� �t�H�[�����[�h���ɗ�O [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �I�u�W�F�N�g�̔j��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
            '-------------------------------------------------------------------
            ' KizuLocal.ini�ւ̕ۑ�
            '-------------------------------------------------------------------
            If Not mcls_DispDefect Is Nothing Then
                tMod.WriteIni(SO_GAMENN, KEY_OVERLAY_VISIBLE, LOCALINI_NAME, IIf(mcls_DispDefect(0).OverlayVisible, 1, 0).ToString)
                'tMod.WriteIni(SO_GAMENN, KEY_SCALE_VISIBLE, LOCALINI_NAME, IIf(mcls_DispDefect(0).MemoriVisible, 1, 0).ToString)
            End If

            '�C�x���g����
            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            RemoveHandler mcls_DispMap.SelectMapCell_Auto, AddressOf frmJisseki_SelectMapCellAuto
            RemoveHandler mcls_DispMap.SelectMapCell_AutoDspInit, AddressOf frmJisseki_SelectMapCellAutoDspInit
            RemoveHandler mcls_DispMap.SelectMapCell_Click, AddressOf frmJisseki_SelectMapCellClick
            RemoveHandler dgvMap.MouseWheel, AddressOf dgvMap_MouseWheel
            RemoveHandler UcDefect1.picImg1.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect1.picImg2.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect2.picImg1.DoubleClick, AddressOf pic_DoubleClick
            RemoveHandler UcDefect2.picImg2.DoubleClick, AddressOf pic_DoubleClick

            '-------------------------------------------------------------------
            ' �������
            '-------------------------------------------------------------------
            If Not mfrm_Search Is Nothing Then                            ' �t�@�C���ۑ������̂ŁA���߂ɊJ��
                mfrm_Search.SaveSetting()
                mfrm_Search.Dispose()
                mfrm_Search = Nothing
            End If
            If Not mcls_DispDefect Is Nothing Then
                For Each dsp As clsDispDefect In mcls_DispDefect
                    If Not dsp Is Nothing Then
                        dsp.Dispose()
                    End If
                Next
                mcls_DispDefect = Nothing
            End If
            If Not mcls_DataMap Is Nothing Then
                mcls_DataMap.Dispose()
                mcls_DataMap = Nothing
            End If
            If Not mfrm_PopImage Is Nothing Then
                mfrm_PopImage.Dispose()
                mfrm_PopImage = Nothing
            End If
            If Not mfrm_PopTcmImage Is Nothing Then
                mfrm_PopTcmImage.Dispose()
                mfrm_PopTcmImage = Nothing
            End If
            If Not mcls_Param Is Nothing Then                               ' �p�����[�^�Ǘ��N���X�͈�ԍŌ�ɂ�邱��
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���щ�� �I�u�W�F�N�g�j�����ɗ�O [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I���C�x���g(�I�[�g��)
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="pntKizu">�I���r���</param>
    ''' <param name="emInf">�r���ʒu</param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_SelectMapCellAuto(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE, ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspDefect(nMen, pntKizu, True)

            If emInf = clsDispMap.EM_DSP_KIZU_INF.DSP_JI1 Then
                SetDefectButton(nMen, pntKizu)
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I���C�x���g�ŗ�O���� �\����={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I�������C�x���g(�I�[�g��)
    ''' </summary>
    ''' <param name="emInf">�r���ʒu</param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_SelectMapCellAutoDspInit(ByVal emInf As clsDispMap.EM_DSP_KIZU_INF)
        Try
            mcls_DispDefect(CType(emInf, Integer)).DspInit()
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I�������C�x���g�ŗ�O���� �\����={0}[{1}]", MEN, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�}�b�v/�r�Z���I���C�x���g(�I����)
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <param name="pntKizu">�I���r���</param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_SelectMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Dim nLen As Integer = 0
        Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try

            For nCnt As Integer = mcls_DispDefect.Count - 1 To 1 Step -1
                'If 0 < mcls_DispMap.GetSelectKizu(nCnt) Then
                Dim strVal As TYP_MAP_LIST_VALUE = Nothing
                If mcls_DispMap.GetSelectKizu(nCnt, strVal) AndAlso 0 < strVal.nKizuNo Then
                    mcls_DispDefect(nCnt).DspDefect(nMen, mcls_DispDefect(nCnt - 1).CurrentDefect, False)
                Else
                    mcls_DispDefect(nCnt).DspInit()
                End If
            Next

            '�r����\��
            mcls_DispDefect(0).DspDefect(nMen, pntKizu, False)

            SetDefectButton(nMen, pntKizu)

            If mfrm_PopImage.Visible Then                               '�r�摜�g��\����
                '�_�u���N���b�N�ȊO�͍ŐV�r�̏���\��
                mfrm_PopImage.DispImgNo = 1
                DispDefectImg()
            End If

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I���C�x���g �\����={0}[{1}]", nMen, ex.Message))
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�摜�_�u���N���b�N�ُ�[{0}]", ex.Message))
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
                vsbMap.ValueProgramable = value
                SetScroll(value)
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[���C�x���g�ُ�[{0}]", ex.Message))
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�E�X�X�N���[���C�x���g�����ُ�[{0}]", ex.Message))
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
    Private Sub frmJisseki_SelectTcmMapCellClick(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Dim nLen As Integer = 0
        'Dim coilinf As COINFO_BASE = New COINFO_BASE

        Try
            'If EM_REFRESH_MODE.PAUSE <> mcls_DispMap.RefreshMode Then
            '    Exit Try
            'End If

            'If mcls_DispCoil.GetCoilInf(pntKizu.nRec, coilinf) Then    '�R�C�����e�[�u���Ǎ�
            '    mcls_DispCoil.DspCoil(pntKizu.nRec, coilinf, Men)      '�R�C�����\��
            'End If

            DispDefectTcmImg(nMen, pntKizu)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v/�r�Z���I���C�x���g(TCM) �\����={0}[{1}]", nMen, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �t�@���N�V�������j���[�ؑ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmJisseki_KeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        'shift�L�[�ȊO�͏I��
        If Not e.Shift Then Exit Sub
        '�t�@���N�V�����{�^���̌���ۑ��ƕ\��
        FuncInfoSaveDisp()
    End Sub

    ''' <summary>
    ''' �؂�ւ��{�^�������ishift�L�[�������Ɠ��l�̏����j
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnShift_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnShift.Click
        '�t�@���N�V�����{�^���̌���ۑ��ƕ\��
        FuncInfoSaveDisp()
    End Sub


    '***************************************************************************
    ' �����֐�
    '***************************************************************************

    ''' <summary>
    ''' �ݒ�l�擾
    ''' </summary>
    ''' <param name="nMen">��[0:�\�A1:��]</param>
    Private Sub GetIniSetting(ByVal nMen As Integer)
        Try
            Dim nVal As Integer = 0

            '-------------------------------------------------------------------
            ' �r�}�b�v�R�C������(�\������)
            '-------------------------------------------------------------------
            nVal = tMod.ReadIniInt(SO_GAMENN, "MAP_SCROLL_DIRECT", TASKINI_NAME, 0)
            Select Case nVal
                Case 0
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD            '�������
                Case 1
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL            '�ォ�牺
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini MAP_SCROLL_DIRECT�ُ� �l={0}", nVal))
                    mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD
            End Select

            '-------------------------------------------------------------------
            ' �r���x��DS�ʒu(�\/��)
            '-------------------------------------------------------------------
            Dim strMapViewDirect() As String = {"MAP_VIEW_DIRECT_O", "MAP_VIEW_DIRECT_U"}
            nVal = tMod.ReadIniInt(SO_GAMENN, strMapViewDirect(nMen), TASKINI_NAME, 0)
            Select Case nVal
                Case 0
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL                      '����DS
                Case 1
                    mcls_DispMap.Side = EM_MAP_SIDE.REVERSE                     '����WS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} �l={0}", nVal, strMapViewDirect(nMen)))
                    mcls_DispMap.Side = EM_MAP_SIDE.NORMAL
            End Select

            '-------------------------------------------------------------------
            ' �r�}�b�vDS�ʒu(�\/��)
            '-------------------------------------------------------------------
            Dim strMapCellDirect() As String = {"MAP_CELL_DIRECT_O", "MAP_CELL_DIRECT_U"}
            nVal = tMod.ReadIniInt(SO_GAMENN, strMapCellDirect(nMen), TASKINI_NAME, 0)
            Select Case nVal
                Case 0
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL                     '����DS
                Case 1
                    mcls_DispMap.DSPos = EM_MAP_CELL.REVERSE                    '����WS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} �l={0}", nVal, strMapCellDirect(nMen)))
                    mcls_DispMap.DSPos = EM_MAP_CELL.NORMAL
            End Select

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q -------------------->>>>>
			'-------------------------------------------------------------------
			' �r�}�b�v���ʒu�ԍ�(�\/��)
			'-------------------------------------------------------------------
			Dim strMapViewWidthNo() As String = {"MAP_VIEW_WIDTH_NO_O", "MAP_VIEW_WIDTH_NO_U"}
			Dim nMapViewWidthNo As Integer = tMod.ReadIniInt(SO_GAMENN, strMapViewWidthNo(nMen), TASKINI_NAME, 0)
			Select Case nMapViewWidthNo
				Case 0
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL                      '������1,2,�E�E�E
				Case 1
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.REVERSE                     '�E����1,2,�E�E�E
				Case Else
					gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} �l={0}", nMapViewWidthNo, strMapViewWidthNo(nMen)))
					mcls_DispMap.WidthNo = EM_MAP_WIDTH_NO.NORMAL
			End Select
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂Q <<<<<--------------------

			'-------------------------------------------------------------------
			' �r�摜�㉺���](�\/��)
			'-------------------------------------------------------------------
			Dim strImageTopBot() As String = {"IMAGE_TOP_BOTTOM_INF_O", "IMAGE_TOP_BOTTOM_INF_U"}
            m_emImageTop(nMen) = CType(tMod.ReadIniInt(SO_GAMENN, strImageTopBot(nMen), TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case m_emImageTop(nMen)
                Case EM_IMAGE_TOP.NORMAL                                        '���]����
                Case EM_IMAGE_TOP.REVERSE                                       '���]�L��
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} �l={0}", m_emImageTop(nMen), strImageTopBot(nMen)))
                    m_emImageTop(nMen) = EM_IMAGE_TOP.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnTB = m_emImageTop(nMen)
            Next

            '-------------------------------------------------------------------
            ' �r�摜���E���](�\/��)
            '-------------------------------------------------------------------
            Dim strImageSide() As String = {"IMAGE_SIDE_INF_O", "IMAGE_SIDE_INF_U"}
            m_emImageSide(nMen) = CType(tMod.ReadIniInt(SO_GAMENN, strImageSide(nMen), TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case m_emImageSide(nMen)
                Case EM_IMAGE_SIDE.NORMAL                                       '���]����
                Case EM_IMAGE_SIDE.REVERSE                                      '���]�L��
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} �l={0}", m_emImageSide(nMen), strImageSide(nMen)))
                    m_emImageSide(nMen) = EM_IMAGE_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.IsTurnLR = m_emImageSide(nMen)
            Next

            '-------------------------------------------------------------------
            ' �r�摜���x��DS�ʒu(�\/��)
            '-------------------------------------------------------------------
            Dim strImageView() As String = {"IMAGE_VIEW_DIRECT_O", "IMAGE_VIEW_DIRECT_U"}
            Dim nImgSide As EM_MAP_SIDE = CType(tMod.ReadIniInt(SO_GAMENN, strImageView(nMen), TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nImgSide
                Case EM_MAP_SIDE.NORMAL                                         '����DS
                Case EM_MAP_SIDE.REVERSE                                        '����WS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini {1} �l={0}", nImgSide, strImageView(nMen)))
                    nImgSide = EM_MAP_SIDE.NORMAL
            End Select
            For Each dsp As clsDispDefect In mcls_DispDefect
                dsp.Side = nImgSide
            Next

            '-------------------------------------------------------------------
            'TCM�r�摜�㉺���]
            '-------------------------------------------------------------------
            Dim nTcmImgVMode As EM_IMAGE_TOP = CType(tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_TOP_BOTTOM_INF", TASKINI_NAME, 0), EM_IMAGE_TOP)
            Select Case nTcmImgVMode
                Case EM_IMAGE_TOP.NORMAL                                        '���]����
                Case EM_IMAGE_TOP.REVERSE                                       '���]�L��
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM�r�摜�㉺���]�ُ�[{0}]", nTcmImgVMode))
                    nTcmImgVMode = EM_IMAGE_TOP.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnTB = nTcmImgVMode                                            '�㉺���]�ݒ�


            '-------------------------------------------------------------------
            'TCM�r�摜���E���]
            '-------------------------------------------------------------------
            Dim nTcmImgHMode As EM_IMAGE_SIDE = CType(tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_SIDE_INF", TASKINI_NAME, 0), EM_IMAGE_SIDE)
            Select Case nTcmImgHMode
                Case EM_IMAGE_SIDE.NORMAL                                       '���]����
                Case EM_IMAGE_SIDE.REVERSE                                      '���]�L��
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM�r�摜���E���]�ُ�[{0}]", nTcmImgHMode))
                    nTcmImgHMode = EM_IMAGE_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.IsTurnLR = nTcmImgHMode                                                               '���E���]�ݒ�

            '-------------------------------------------------------------------
            'TCM�r�摜���x��DS�ʒu
            '-------------------------------------------------------------------
            Dim nTcmImgSide As EM_MAP_SIDE = CType(tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_VIEW_DIRECT", TASKINI_NAME, 0), EM_MAP_SIDE)
            Select Case nTcmImgSide
                Case EM_MAP_SIDE.NORMAL                                         '����DS
                Case EM_MAP_SIDE.REVERSE                                        '����WS
                Case Else
                    gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM�r�摜���x��DS�ʒu�ُ�[{0}]", nTcmImgSide))
                    nTcmImgSide = EM_MAP_SIDE.NORMAL
            End Select
            mfrm_PopTcmImage.Side = nTcmImgSide                                                    '�r�摜���x��DS�ʒu

            '-------------------------------------------------------------------
            ' �摜100pixel�����ݒ�
            '-------------------------------------------------------------------
            nVal = tMod.ReadIniInt(SO_GAMENN, "IMAGE_WIDTH", TASKINI_NAME, 100)
            If nVal < 0 Then                                                    ' �ُ�l
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini IMAGE_WIDTH�ُ� �l={0}", nVal))
                nVal = 100
            End If
            For Each dsp As clsDispDefect In mcls_DispDefect                       ' �摜�{�����f
                dsp.ImageWidth = nVal
            Next

            '-------------------------------------------------------------------
            ' �摜100pixel�����ݒ�
            '-------------------------------------------------------------------
            nVal = tMod.ReadIniInt(SO_GAMENN, "TCM_IMAGE_WIDTH", TASKINI_NAME, 100)
            If nVal < 0 Then                                                    ' �ُ�l
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("KizuTask.ini TCM_IMAGE_WIDTH�ُ� �l={0}", nVal))
                nVal = 100
            End If
            m_nTcmImageWide = nVal

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���щ�� �ݒ�t�@�C���ǂݍ��ݒ��ɗ�O [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' �r�摜���̏�����
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

            clsDispDefect = New clsDispDefect(typDefInf, mcls_Param, gcls_Log, RSC_IMG_DEFAULT)
            clsDispDefect.SetScaleConfig(JISSEKI_SCALE_INTVAL_X, JISSEKI_SCALE_NUM_X, JISSEKI_SCALE_INTVAL_Y, JISSEKI_SCALE_NUM_Y)
            clsDispDefect.ImageZoom = True

        Catch ex As Exception
            gcls_Log.Write(LNO.ERR, String.Format("�r���\���G���A������[{0}],[{1}]", nImgNo, ex.Message))
        End Try

        Return clsDispDefect

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
                .lblKensa = ucCoil.lblKensa
                .lblKiki = ucCoil.lblKiki
                .tblLayoutPnl = ucCoil.TableLayoutPanel1
            End With

            clsDispCoil = New clsDispCoil(typCoilDsp, gcls_Log, mcls_Param)

        Catch ex As Exception
            gcls_Log.Write(LNO.ERR, String.Format("�R�C�����\���G���A�������ُ�[{0}]", ex.Message))
        End Try

        Return clsDispCoil

    End Function


    ''' <summary>
    ''' �r�}�b�v�s�Ԋu�̐ݒ�
    ''' </summary>
    ''' <param name="emStage">�ύX��̊Ԋu 1:�\��(��) 2:�\��(��)</param>
    ''' <remarks></remarks>
    Private Sub SetMapInterval(ByVal emStage As EM_LONG_POS)
        Try

            'MAP�\���Ԋu��ݒ�
            mcls_DispMap.Stage = emStage

            '-------------------------------------------------------------------
            '�{�^���\���ύX
            '-------------------------------------------------------------------
            Dim strBtn As String
            If EM_LONG_POS.BIG = mcls_DispMap.Stage Then
                strBtn = mcls_Param.MapStageBig.ToString & "m��" & mcls_Param.MapStageSmall.ToString & "m"
            Else
                strBtn = mcls_Param.MapStageSmall.ToString & "m��" & mcls_Param.MapStageBig.ToString & "m"
            End If
            If EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode Then
                btnF08.Text = strBtn
            Else
                m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1).func(8 - 1).strText = strBtn
            End If

            mcls_DispMap.DspData(Men)


        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�}�b�v�Ԋu�ݒ�ُ� value={0}[{1}]", emStage, ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' ���щ�ʏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()
        Try

            mcls_DispMap.ClearMap()
            mcls_DispMap.ClearDeep()
            mcls_DispMap.ClearTcm()

            mcls_DispCoil.DspInit()

            For Each dsp As clsDispDefect In mcls_DispDefect                    ' �r��񏉊���
                dsp.DspInit()
            Next

            ' �t�@���N�V�����L�[�����������
            btnF01.Enabled = False
            btnF02.Enabled = False
            btnF03.Enabled = False
            btnF04.Enabled = False
            btnF05.Enabled = True
            btnF06.Enabled = False
            btnF07.Enabled = False
            btnF08.Enabled = False
            btnF09.Enabled = False
            btnF10.Enabled = False
            btnF11.Enabled = False
            btnF12.Enabled = True

            vsbMap.ValueProgramable = 0                                         ' �X�N���[���o�[������

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("���щ�ʏ������ُ� [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' ���щ�ʂ̉�ʃ^�C�g����ݒ肷��
    ''' </summary>
    ''' <param name="nMen">�\����</param>
    ''' <remarks></remarks>
    Private Sub SetJissekiTitle(ByVal nMen As Integer)
        ' ��ʃ^�C�g���Z�b�g
        If 0 = nMen Then
            Me.Text = "�������щ��(�\��)"
        Else
            Me.Text = "�������щ��(����)"
        End If

        SetTitleName(Me)
    End Sub

    ''' <summary>
    ''' �r�}�b�v�X�N���[������
    ''' </summary>
    ''' <param name="nValue">�X�N���[����</param>
    ''' <remarks></remarks>
    Private Sub SetScroll(ByVal nValue As Integer)
        Try
            Dim value As Integer = nValue
            If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then value = vsbMap.Maximum - (vsbMap.LargeChange - 1) - nValue '�i�s����

            Dim nMaxRowNum As Integer
            mcls_DispMap.SetMapData(Men, value, nMaxRowNum)

            dgvMap.Update()
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[�������ُ�[{0}]", ex.Message))
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[���C�x���g�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �r�摜�g��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispDefectTcmImg(ByVal nMen As Integer, ByVal pntKizu As TYP_MAP_LIST_VALUE)
        Try
            Dim typDefInf As TYP_MAE_DEFECT_DATA = Nothing
            Dim key As clsDataMap.DEFECT_KEY
            key.nKizuNo = pntKizu.nKizuNo

            mcls_DataMap.TcmDefectData(nMen, key, typDefInf)


            ShowPopupTcm(typDefInf, mfrm_PopTcmImage)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�}�b�v�X�N���[���C�x���g�ُ�[{0}]", ex.Message))
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
    Private Sub frmJisseki_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' 0�I���W��

            Case 0                              'F1
                Call SelectF1()
            Case 1                              'F2
                Call SelectF2()
            Case 2                              'F3
                Call SelectF3()
            Case 3                              'F4
                Call SelectF4()
            Case 4                              'F5
                Call SelectF5()
            Case 5                              'F6
                Call SelectF6()
            Case 6                              'F7
                Call SelectF7()
            Case 7                              'F8
                Call SelectF8()
            Case 8                              'F9
                Call SelectF9()
            Case 9                              'F10
                Call SelectF10()
            Case 10                             'F11                
                Call SelectF11()
            Case 11
                MyBase.ExecFromClose()          ' �I��
        End Select
    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�P
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF1()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                '�O�R�C��
                All_Popup_Close(True)
                btnF01.Enabled = False
                ReadPreviousCoil()
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�Q
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF2()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                '���R�C��
                All_Popup_Close(True)
                btnF02.Enabled = False
                ReadNextCoil()

            Case Else
        End Select
    End Sub


    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�R
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF3()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                ' �O�r
                mcls_DispMap.SetSelection(Men, False)
            Case Else
        End Select


    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�S
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF4()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                ' ���r
                mcls_DispMap.SetSelection(Men, True)
            Case Else
        End Select
    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�T
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF5()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0, EM_DIV_FUNC_TYPE.DIV_SELECT_1
                frmSearchShowDialog()

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�U
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF6()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                Dim frm As New frmMixedRate(gcls_Log, mcls_DataMap.KizukenNo_In)

                '�f�[�^�`�F�b�N
                If frm.ChkData(mcls_DataMap.KizukenNo_In) Then

                    '�f�[�^����
                    frm.ShowDialog()
                    frm.Dispose()
                    frm = Nothing
                Else

                    '�f�[�^�Ȃ�
                    Dim frmMsg As New tClassForm.frmPop_MsgBox("�f�[�^�����݂��܂���", Me.Text)
                    frmMsg.ShowDialog()
                    frm.Dispose()
                    frm = Nothing

                End If
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                ' �ڐ���̕\���E��\����Ԃ𔽓]������
                Overlay = Not Overlay
                'Memori = Not Memori
                dgvMap.Focus()
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�V
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF7()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                All_Popup_Close(True)
                ''HACK�@2015/02/02/18 ���݂��r�̈ʒu���o���Ă���
                'Dim key As clsDataMap.LIST_KEY = mcls_DispMapDefLst.CurrentKey
                'gcls_Log.Write(tClass_LogManager.LNO.INF, String.Format("�ʐؑփ{�^�� key = [{0}]", key.ToString))

                ''HACK�@2015/01/06 �V���[�J�b�g��������
                'mcls_DispMap.DspLineInit()

                ''HACK�@2015/02/23  Resize�����vsbMap.Value���O�ɂȂ�A�\/���ʐؑւ����܂������Ȃ�
                ''ResizeScrollSize()      

                If mcls_DispMap.Men = 0 Then
                    mcls_DispMap.Men = 1
                    'm_nSelectMen = 1
                    SetJissekiTitle(mcls_DispMap.Men)
                Else
                    mcls_DispMap.Men = 0
                    'm_nSelectMen = 0
                    SetJissekiTitle(mcls_DispMap.Men)
                End If

                SetMen(mcls_DispMap.Men)

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                All_Popup_Close(True)
                ''�G�b�W���o�󋵉��
                Dim frm As New frmEdgeDetect(gcls_Log, mcls_DataMap.CoilInfo, mcls_DataMap.TurnTB, mcls_DataMap.KizukenNo_In)
                '�f�[�^�`�F�b�N
                If frm.ChkData_CoilInf(mcls_DataMap.KizukenNo_In) Then
                    '�f�[�^����
                    frm.ShowDialog()
                    frm.Dispose()
                    frm = Nothing
                Else

                    '�f�[�^�Ȃ�
                    Dim frmMsg As New tClassForm.frmPop_MsgBox("�f�[�^�����݂��܂���", Me.Text)
                    frmMsg.ShowDialog()
                    frm.Dispose()
                    frm = Nothing

                End If

            Case Else
        End Select
    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�W
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF8()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                All_Popup_Close(False)

                If EM_LONG_POS.SMALL = Stage Then
                    SetMapInterval(EM_LONG_POS.BIG)
                Else
                    SetMapInterval(EM_LONG_POS.SMALL)
                End If

                ' ���݃}�b�v�ɕ\�����Ă���ʒu��V����MAP�\���Ԋu�ł��\���ł���悤�ɂ���B
                Dim nValue As Integer = (vsbMap.Value)
                If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
                    '�i�s�������ォ�牺�̏ꍇ�́A���̈ʒu����ɂ���
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
                If mcls_DispMap.MapDir = EM_MAP_SCROOL.HEAD_TO_TAIL Then
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

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                All_Popup_Close(True)
                ' �R�C���g�����h���
                Dim frm As New frmCoilTrand(mcls_DataMap.CoilInfo, mcls_DataMap.TurnTB, gcls_Log)
                frm.Owner = Me
                frm.ShowDialog(Me)
                frm.Dispose()
                frm = Nothing
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�X
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF9()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                Dim frmDefMap As New frmDefMapJisseki(mcls_DataMap.KizukenNo_In, mcls_DataMap.CoilLength, mcls_DataMap.CoilWidth, mcls_DataMap.CoilInfo.bCorrect, mcls_Param, gcls_Log)

                '�R�C�����Z�b�g
                frmDefMap.SetCoilData(mcls_DataMap.CoilInfo)
                '�J�b�g���Z�b�g
                frmDefMap.SetCutData(mcls_DataMap.CutDataLst)
                '�r��񃊃X�g�Z�b�g
                frmDefMap.SetDefectData(0, mcls_DataMap.DefectDataLst(0).Values.ToList)
                frmDefMap.SetDefectData(1, mcls_DataMap.DefectDataLst(1).Values.ToList)
                '�R�C�����]�L���Z�b�g
                frmDefMap.SetTurnTB(mcls_DataMap.TurnTB)

                frmDefMap.DspData()
                frmDefMap.Owner = Me
                frmDefMap.ShowDialog()

                frmDefMap.Dispose()
                frmDefMap = Nothing
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                '�R�C�����\��
                Dim frm As New frmDspCoil(mcls_DataMap.KizukenNo_In)
                frm.ShowDialog()
                frm.Dispose()
                frm = Nothing
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�P�O
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF10()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                ' �n�[�h�R�s�[
                Call HardCopy(IMG_PATH, "���щ��_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^�� �e�P�P
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SelectF11()

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                ' ��ʈ��
                Call PrintOut()
			Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
				'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
				Dim frm As New frmLongPdList(gcls_Log, mcls_Param, False)

				Dim lstKanriNo As New List(Of String)
				Dim lstCoilNo As New List(Of String)

				lstKanriNo.Add(mcls_DataMap.CoilInfo().strKanriNo)
				lstCoilNo.Add(mcls_DataMap.CoilInfo().strCoilNo)
				frm.Owner = Me
				frm.tProp_Moveable = True
				frm.PreDispProc(lstKanriNo, lstCoilNo, IIf(mcls_DispMap.Side = EM_MAP_SIDE.REVERSE, True, False))
				frm.ShowDialog()
				frm.Dispose()
				frm = Nothing
				'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
			Case Else
		End Select

    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^����񏉊��ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub FunctionKey_Init()

        For ii As Integer = 0 To m_typMenu.Length - 1

            m_typMenu(ii).initialize(my_btnFunction.Length)

            With m_typMenu(ii)
                Select Case ii
                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                        '�{�^������
                        .func(0).strText = "�O��"
                        .func(1).strText = "����"
                        .func(2).strText = "�O�r"
                        .func(3).strText = "���r"
                        .func(4).strText = "����"
                        .func(5).strText = "�r������"
                        .func(6).strText = "���ʕ\��"
                        .func(7).strText = "5m->30m"
                        .func(8).strText = "�r�}�b�v"
                        .func(9).strText = "��ʕۑ�"
                        .func(10).strText = "��ʈ��"
                        .func(11).strText = "���j���["
                        '�����l�Z�b�g
                        .func(4).bEnabled = True
                        .func(11).bEnabled = True

                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                        '�{�^������
                        .func(0).strText = "�O��"
                        .func(1).strText = "����"
                        .func(2).strText = "�O�r"
                        .func(3).strText = "���r"
                        .func(4).strText = "����"
                        .func(5).strText = "���ްڲ"
                        .func(6).strText = "���ޏ�"
                        .func(7).strText = "��������"
                        .func(8).strText = "��ُ��"
						.func(9).strText = ""
						'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>						
						'.func(10).strText = ""
						.func(10).strText = "������"
						'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
						.func(11).strText = "���j���["
                        '�����l�Z�b�g
                        .func(4).bEnabled = True
						.func(5).bEnabled = True
						.func(11).bEnabled = True
                End Select
            End With
        Next

        For ii As Integer = 0 To my_btnFunction.Length - 1
            With m_typMenu(m_nMenuMode)
                my_btnFunction(ii).Enabled = .func(ii).bEnabled
                my_btnFunction(ii).Text = .func(ii).strText
            End With
        Next


    End Sub

    ''' <summary>
    ''' �t�@���N�V�����{�^���̌���ۑ��ƕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub FuncInfoSaveDisp()

        If mcls_Param Is Nothing Then Exit Sub

        For ii As Integer = 0 To my_btnFunction.Length - 1
            With m_typMenu(m_nMenuMode)
                .func(ii).bEnabled = my_btnFunction(ii).Enabled
                .func(ii).strText = my_btnFunction(ii).Text
            End With
        Next

        m_nMenuMode = CInt(IIf(EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode, EM_DIV_FUNC_TYPE.DIV_SELECT_1, EM_DIV_FUNC_TYPE.DIV_SELECT_0))

        For ii As Integer = 0 To my_btnFunction.Length - 1
            With m_typMenu(m_nMenuMode)
                my_btnFunction(ii).Enabled = .func(ii).bEnabled
                my_btnFunction(ii).Text = .func(ii).strText
            End With
        Next

        Select Case m_nMenuMode
            Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                btnF06.BackColor = Color.FromKnownColor(KnownColor.Control)
                btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)

            Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                '�{�^���F��(�I�[�o�[���C)
                If Overlay Then
                    btnF06.BackColor = Color.Cyan
                Else
                    btnF06.BackColor = Color.FromKnownColor(KnownColor.Control)
                End If
                ''�{�^���F��(�X�P�[��)
                'If Memori Then
                '    btnF07.BackColor = Color.Cyan
                'Else
                '    btnF07.BackColor = Color.FromKnownColor(KnownColor.Control)
                'End If
        End Select

    End Sub

    ''' <summary>
    ''' �O�R�C�����ǂݍ��ݏ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadPreviousCoil()

        mfrm_Search.ReadPreviousCoil()
        If mfrm_Search.ReturnCode = frmSearch.SEARCH_RETURN.emSELECT_COIL Then
            DspResult()
        End If

    End Sub


    ''' <summary>
    ''' ���R�C�����ǂݍ��ݏ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadNextCoil()

        mfrm_Search.ReadNextCoil()
        If mfrm_Search.ReturnCode = frmSearch.SEARCH_RETURN.emSELECT_COIL Then
            DspResult()
        End If

    End Sub


    ''' <summary>
    ''' ������ʕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmSearchShowDialog()

        All_Popup_Close(True)
        mfrm_Search.ShowDialog()      ' ���ь�����ʌďo

        If mfrm_Search.ReturnCode = frmSearch.SEARCH_RETURN.emSELECT_COIL Then

            'ini�t�@�C���̐ݒ�̔��f�͈�ԍŏ��ɍs���K�v������
            '�\���ݒ��ǂݍ���
            GetIniSetting(Men)

            '�R�C������������
            mcls_DispCoil.DspInit()

            DspResult()
        End If
    End Sub


    ''' <summary>
    ''' �O�r/���r�{�^���ݒ�
    ''' </summary>
    ''' <param name="nMen"></param>
    ''' <param name="val"></param>
    Private Sub SetDefectButton(ByVal nMen As Integer, ByVal val As TYP_MAP_LIST_VALUE)

        btnF03.Enabled = mcls_DispMap.IsExistEachDef(nMen, val, False)
        btnF04.Enabled = mcls_DispMap.IsExistEachDef(nMen, val, True)
        If EM_DIV_FUNC_TYPE.DIV_SELECT_0 = m_nMenuMode Then
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1).func(3 - 1).bEnabled = btnF03.Enabled
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1).func(4 - 1).bEnabled = btnF04.Enabled
        Else
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_0).func(3 - 1).bEnabled = btnF03.Enabled
            m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_0).func(4 - 1).bEnabled = btnF04.Enabled
        End If

    End Sub

    ''' <summary>
    ''' �\���ʐ؂�ւ�
    ''' </summary>
    ''' <param name="nMen">�\�������</param>
    ''' <remarks></remarks>
    Private Sub SetMen(ByVal nMen As Integer)
        Try
            '-------------------------------------------------------------------
            ' �\���ʂ̕ύX
            '-------------------------------------------------------------------
            If nMen = 0 Then                                                    ' �\�ʕ\��
                mcls_DispMap.Men = 0
                btnF07.Text = "���ʕ\��"
            Else                                                                ' ���ʕ\�����͕\�ʂɐݒ�
                mcls_DispMap.Men = 1
                btnF07.Text = "�\�ʕ\��"
            End If

            '�؂�ւ��O�\���ʒu�擾
            Dim nValue As Integer = vsbMap.Value

            '-------------------------------------------------------------------
            ' �p�����[�^������
            '-------------------------------------------------------------------
            GetIniSetting(nMen)
            '-------------------------------------------------------------------
            ' �I��������
            '-------------------------------------------------------------------
            mcls_DispMap.ClearMap()
            mcls_DispMap.ClearDeep()
            mcls_DispMap.ClearTcm()

            mcls_DispCoil.DspInit()

            For Each dsp As clsDispDefect In mcls_DispDefect                    ' �r��񏉊���
                dsp.DspInit()
            Next

            '�R�C�����\��
            mcls_DispCoil.DspCoil(mcls_DataMap.CoilInfo, nMen)

            mcls_DispMap.ClearSelect()                              ' �I���r������

            mcls_DispMap.DspData(nMen)

            RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
            vsbMap.ValueProgramable = nValue
            SetScroll(nValue)
            AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���ʐ؂�ւ����ɗ�O [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' ���ѕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspResult()
        Try
            With mfrm_Search

                DspInit()

                ' �t�@���N�V�����L�[��������ѕ\���ɍ��킹�čĐݒ�
                ' F1����F5�͑S���j���[����
                btnF01.Enabled = .ExistPrevious()           '�O�R�C��
                btnF02.Enabled = .ExistNext()               '���R�C��
                btnF03.Enabled = False                      '�O�r�͈�U����
                btnF04.Enabled = False                      '���r����U����
                btnF05.Enabled = True

                Select Case m_nMenuMode
                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_0
                        btnF06.Enabled = True
                        btnF07.Enabled = True
                        btnF08.Enabled = True
                        btnF09.Enabled = True
                        btnF10.Enabled = True
                        btnF11.Enabled = IIf(tClass.tClass_HeadCopy.IsCheckPrinter, True, False)
                        btnF12.Enabled = True
                        With m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1)
                            .func(0).bEnabled = btnF01.Enabled
                            .func(1).bEnabled = btnF02.Enabled
                            .func(2).bEnabled = btnF03.Enabled
                            .func(3).bEnabled = btnF04.Enabled
                            .func(4).bEnabled = btnF05.Enabled
                            .func(5).bEnabled = True
                            .func(6).bEnabled = True                '�G�b�W���o�L��
                            .func(7).bEnabled = True                '�R�C���g�����h�L��
                            .func(8).bEnabled = True                '�R�C�����L��
							.func(9).bEnabled = False
							'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>							
							'.func(10).bEnabled = False
							.func(10).bEnabled = True               ' �������ꗗ
							'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
							.func(11).bEnabled = True
                        End With

                    Case EM_DIV_FUNC_TYPE.DIV_SELECT_1
                        btnF06.Enabled = True
                        btnF07.Enabled = True                       '�G�b�W���o�L��
                        btnF08.Enabled = True                       '�R�C���g�����h�L��
                        btnF09.Enabled = True                       '�R�C�����L��
						btnF10.Enabled = False
						'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>							
						'btnF11.Enabled = False
						btnF11.Enabled = True                       '�������ꗗ
						'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
						btnF12.Enabled = True
                        With m_typMenu(EM_DIV_FUNC_TYPE.DIV_SELECT_1)
                            .func(0).bEnabled = btnF01.Enabled
                            .func(1).bEnabled = btnF02.Enabled
                            .func(2).bEnabled = btnF03.Enabled
                            .func(3).bEnabled = btnF04.Enabled
                            .func(4).bEnabled = btnF05.Enabled
                            .func(5).bEnabled = True
                            .func(6).bEnabled = True
                            .func(7).bEnabled = True
                            .func(8).bEnabled = True
                            .func(9).bEnabled = True
                            .func(10).bEnabled = IIf(tClass.tClass_HeadCopy.IsCheckPrinter, True, False)
                            .func(11).bEnabled = True
                        End With
                End Select

                ' ���܂ł̃}�b�v�f�[�^��j������
                mcls_DataMap.Dispose()
                mcls_DataMap = Nothing

                ' �V���ȃ}�b�v�f�[�^�擾
                mcls_DataMap = .DataMap
                mcls_DispMap.DataMap = mcls_DataMap
                For Each dsp As clsDispDefect In mcls_DispDefect
                    If Not dsp Is Nothing Then
                        dsp.DataMap = mcls_DataMap
                    End If
                Next

                Dim bTurnLR As Boolean = CBool(m_emImageSide(Men))
                Dim bTurnTop As Boolean = CBool(m_emImageTop(Men))
                For Each dsp As clsDispDefect In mcls_DispDefect
                    ' �摜�̍��E���]������ݒ肵�܂�
                    If bTurnLR Xor .IsTurnDS Then
                        dsp.IsTurnLR = EM_IMAGE_SIDE.REVERSE
                    Else
                        dsp.IsTurnLR = EM_IMAGE_SIDE.NORMAL
                    End If
                    ' �摜�̏㉺���]�L����ݒ肵�܂�
                    If bTurnTop Xor .IsTurnNagate Then
                        dsp.IsTurnTB = EM_IMAGE_TOP.REVERSE
                    Else
                        dsp.IsTurnTB = EM_IMAGE_TOP.NORMAL
                    End If
                Next

                '�R�C�����\��
                mcls_DispCoil.DspCoil(mcls_DataMap.CoilInfo, Men)

                mcls_DispMap.ClearSelect()                              ' �I���r������

                SetMapInterval(Stage)

                vsbMap.Enabled = True
                vsbMap.Minimum = 0
                RemoveHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
                vsbMap.Maximum = mcls_DispMap.GetScrollMax() + (vsbMap.LargeChange - 1)
                If mcls_DispMap.MapDir = EM_MAP_SCROOL.TAIL_TO_HEAD Then
                    vsbMap.ValueProgramable = mcls_DispMap.GetMapRowTop()
                Else
                    vsbMap.ValueProgramable = vsbMap.Maximum - mcls_DispMap.GetMapRowBot()
                End If

                AddHandler vsbMap.ValueChanged, AddressOf vsbMap_ValueChanged
                mcls_DispMap.MapResizeH()
            End With

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�R�C���������ɗ�O [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' �|�b�v�A�b�v�N���[�Y
    ''' </summary>
    ''' <param name="btyp">false:�}�b�v��̃p�l���̂݁Atrue:�p�l���{�t�H�[��</param>
    ''' <remarks></remarks>
    Private Sub All_Popup_Close(ByRef btyp As Boolean)
        If btyp Then
            If mfrm_PopImage.Visible Then                               '�r�摜�g��\����
                mfrm_PopImage.CloseFrom()                               '�g��摜OFF��
            End If
            If mfrm_PopTcmImage.Visible Then                            '�r�摜�g��\����
                mfrm_PopTcmImage.CloseFrom()                            '�g��摜OFF��
            End If
        End If
    End Sub

    ''' <summary>
    ''' TCM�r�g��摜�\��
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Sub ShowPopupTcm(ByVal typDefInf As TYP_MAE_DEFECT_DATA, ByVal clsPopup As frmDefectImage2)
        Dim clsImg(1 - 1) As Image
        Dim tmpTitle(1 - 1) As String
        Dim nRetc As ValueType = 0
        Dim nImgTyp As Integer = EM_DSP_IMAGE_FORMAT.nJpeg
        Try
            '�摜�`���擾(0:JPEG�A1:BMP)
            nImgTyp = mcls_PreParam.GetProcImgTyp(tMod.ByteToString(typDefInf.ff.cProc))
            '// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
            If KOUTEI_CODE_2TCM = tMod.ByteToString(typDefInf.ff.cProc) Then
                '�O�H���Ǘ�No�擾
                Dim strMaeKizukenNo As String = tMod.ByteToString(typDefInf.ff.cMaeKizukenNo)
                Dim strID As String = Nothing
                If strMaeKizukenNo.Length > 2 Then
                    '�Ǘ�No���Ӄ�ID�擾
                    strID = strMaeKizukenNo.Substring(0, 2)
                    '����2TCM�����̏ꍇ�́A��������BITMAP���Z�b�g�B
                    If strID = KIZUKEN_CODE_2TCM Then
                        nImgTyp = 1
                    End If
                End If
            End If
            '// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
            '�摜�擾
            For ii As Integer = 0 To clsImg.Length - 1
                clsImg(ii) = mcls_DataMap.GetTcmImage(typDefInf.strKanriNo, typDefInf.nKizuNo, typDefInf.data, nImgTyp)
            Next
            '---------------------------
            '�r�摜�g��\��
            '---------------------------
            With clsPopup
                .DispImgNo = 1                                      '�摜No.
                .SetVisibleMemory(Overlay)                           '�������̕\���L���ݒ�
                .SetVisibleOverlay(Overlay)                         '�I�[�o�[���C�̕\���L���ݒ�
                .SetColor = Color.Cyan                              '�������A�I�[�o���C�F

                ''�r���
                .SetDefectData(typDefInf.nKizuNo, typDefInf.ff, typDefInf.data, clsImg, tmpTitle)
                '�\���{��
                .ZoomRate = CInt((100.0 * typDefInf.data.w_res / m_nTcmImageWide) * 100.0)
                '�ڐ���`��ڍ�
                .SetMemoryConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X * 2, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y * 2)
                .DspUpDate(True)                                                          '�\��
            End With

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�r�g��摜�\���ُ�[{0}]", ex.Message))
        End Try
    End Sub


End Class
