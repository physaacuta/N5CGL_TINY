'Option Strict On	'���L�������A�N�Z�X�����邽�߁AOff��
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Reflection
Imports System.Collections.Generic

Imports LabelInfo = tClassLibrary.tClass.tClass_ImageDspAxisDrawner.LabelInfo


Public Class frmMain
    Implements RpiPlugin.IPropertiesProvider

    Private m_plugins As New RpiPlugin.RpiPluginManager
    Private m_active_mouse_handler As RpiPlugin.IMouseToolStrip

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_RpiView As New tClass_ViewRpiThread                     ' RPI�t�@�C���\���N���X
    Private WithEvents mcls_ImgCutDsp As tClass_ImageRawCutDsp                      ' �摜�����g��N���X
    Private WithEvents mcls_CheckKizu As clsCheckKizuWT = Nothing                   ' �����A��~�Ǘ��N���X (�{��)
    Private mcls_Param As New clsParamManager                                       ' �p�����[�^�Ǘ��N���X (�{��)

    Private m_bDebug As Boolean = False                                             ' �J�����t���O (�J����true)


    '' �c���\��
    Public scrNagate As ScrollBar                                                   ' ���葤�̃X�N���[���o�[�i�c���ρI�j
    Public scrHaba As ScrollBar                                                     ' �������̃X�N���[���o�[�i�c���ρI�j

    ' �摜�����p�t���O
    Private mcls_StopWatch As New tClass_PerformanceCounter                         ' �t���[�����[�g�Z�o�p
    Private m_bDrawKizuOvl As Boolean = True                                        ' �r�I�[�o�[���C�`��L��
    Private m_bDrawMemori As Boolean = True                                         ' �������`��L��
    Private m_bDrawEdge As Boolean = False                                          ' �G�b�W���`��L��
    Private m_sDrawStrLR(2) As String                                               ' ���E����

    '' (�r�ڋ߃����v)
    Private mcnt_Ranp(MC_RANP_CNT - 1) As Control                                   ' �r�ڋ߃����v �R���g���[���z��


    '' ���ъ֌W�@�\ 
    Private m_bDb As Boolean = False                                                ' DB�A�N�Z�X�L��
    Private m_bResutl As Boolean = False                                            ' DB���їL�� (���ꂪ�Ȃ���΁A���ъ֌W�͕\�����Ȃ�)
    Private m_typCoil As DATA_COIL                                                  ' �R�C�����
    Private m_lstDetect As New List(Of DATA_DETECT_INF)                             ' �r��񃊃X�g
    Private m_lstCut As New List(Of DATA_CUT_INF)                                   ' �J�b�g��񃊃X�g

    '' �g���@�\ 
    'Private WithEvents mcls_RecvMail As clsRecvMail = Nothing						' ���[���X���b�g��M
    Private WithEvents mcls_MailSlotServer As MailSlotServer(Of CommonQueMix)

    Private mfrm_Kiri As New frmPopKiriImg                                          ' �؂�o���摜�\�����
    Private WithEvents mfrm_DefMap As New frmPopDefMap                              ' �U�z�}�`���̑S�̃}�b�v
    Private WithEvents mfrm_KizuMap As New frmPopKizuMap                            ' �r�}�b�v�`���̑S�̃}�b�v
    Private WithEvents mfrm_KizuList As New frmPopKizuList                          ' �r�ꗗ

    'Private WithEvents mfrm_ImgCont As New frmPopImgControl							' �P�x�R���g���[�����
    Private WithEvents mfrm_Kai As New frmPopKaihatu                                ' �J���p���
    Private WithEvents mfrm_Search As New frmPopCoilSearch                          ' �R�C���������
    Private WithEvents mfrm_GridLine As New frmPopGridLine                          ' �O���b�h��
    Private WithEvents mfrm_FrmCont As New frmPopControl                            ' �W�����v���


    Private WithEvents mfrm_DspSet As New frmPopDspSet                              ' �\���ݒ���
    Private WithEvents mfrm_ReSetOvl As New frmPopResetOvl                          ' �I�[�o�[���C�蓮�␳���

    Private mfrm_CoilItem As frmPopCoilItem                                         ' �R�C�����ꗗ
    Private mfrm_Print As New frmPopPrint                                           ' �o�͐ݒ�
    Private mfrm_KiriInput As New frmPopKiriInput                                   ' �؂�o���摜����
    Private WithEvents mfrm_Bookmark As New frmPopBookmark                          ' �}�[�L���O�@�\
    Private WithEvents mfrm_InputList As New frmPopInputList                        ' ���͈ꗗ�@�\
    Private WithEvents mfrm_Setting As New frmPopSetting                            ' �ݒ���

    '' �����A��
    Private m_memTrkPtr As IntPtr = IntPtr.Zero                         ' ���L�������|�C���^(���H���ԘA�g)
    Private m_memSyncPtr As IntPtr = IntPtr.Zero                        ' ���L�������|�C���^(�Ȉ՘A�g)
    Private m_nSyncOffsetY As Integer                                   ' �Ȉ՘A�g�{�^���������̍���ʒu[mm]
    Private m_nSyncOffsetX As Integer                                   ' �Ȉ՘A�g�{�^���������̍���ʒu[mm]

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        '�^�C�g����RpiViewer.exe Assembly�̃o�[�W�����𑫂��B�R�Bminorrevision�͖����B
        Dim ver As Version = Assembly.GetExecutingAssembly().GetName().Version
        Me.Text &= String.Format(" {0}.{1}.{2}", ver.Major, ver.Minor, ver.Build)


        ' ���d�N�����ɂ́A�ŏ��̃��c�����_��
        Try
            gcls_Log = New tClass.tClass_LogManager("RPI_VIEWER", , , 0, 0)
            gcls_Log.Write(tClass_LogManager.LNO.MSG, "�N��")
        Catch ex As Exception
        End Try

        '===============================================
        ' ��{
        m_bDebug = IIf(0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DEBUG", TASKINI_NAME), False, True)

        '���߂͉�]�Ȃ�
        scrHaba = Me.hsBar
        scrNagate = Me.vsBar
        scrNagate.Maximum = 500         ' LargeChange�ȏ�̒l�����Ă����Ȃ��ƁA�����LargeChange�����������Ă��܂�
        scrNagate.SmallChange = 100
        scrNagate.LargeChange = 500

        'AP�o�[�W����
        If IntPtr.Size = 8 Then         '64bit��
            Me.Text &= " (64bit�����[�h)"
        Else                            '32bit��
            Me.Text &= " (32bit�����[�h)"
        End If

        If g_bOutTrk Then
            Me.Text &= " (�O���A�g���[�h)"
        End If


        '===============================================
        ' �g���@�\
        '   �g���@�\�́A�f�t�H���g�\���֎~

        '''' �������\��
        g_nLockMode = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "LOCK", TASKINI_NAME)
        If 0 <> g_nLockMode AndAlso 1 <> g_nLockMode AndAlso 9 <> g_nLockMode Then g_nLockMode = 0
        If 0 = g_nLockMode Then     ' ��\���ݒ�
            tsbRot.Visible = False
        End If


        '''' �I�v�V�����@�\�\����\��
        ' �Ȉ՘A��
        tsbSync.Visible = False     ' �Ȉ՘A��
        ' �r�ڋ߃����v
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_RANP", TASKINI_NAME) Then        ' �r�ڋ߃����v
            tlpRanp.Visible = False
        Else
            tlpRanp.Visible = True
        End If
        ' �W�����v��ʂ̂݁B�t�@���N�V�����͗L��
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_JUMP", TASKINI_NAME) Then
            tsbJump.Visible = False
        Else
            tsbJump.Visible = True
        End If
        ' �C���r�I��
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_KIRIINPUT", TASKINI_NAME) Then
            tsbKiriInput.Visible = False
        Else
            tsbKiriInput.Visible = True
        End If
        ' �e��ݒ���
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_SETTING", TASKINI_NAME) Then
            tsbSetting.Visible = False
        Else
            tsbSetting.Visible = True
        End If
        ' �ʒu�I��(�����)
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_INTPU_LIST", TASKINI_NAME) Then
            tsbInputList.Visible = False
        Else
            tsbInputList.Visible = True
        End If
        mfrm_Search.tProp_InputLine = tsbInputList.Visible
        mfrm_KiriInput.tProp_InputLine = tsbInputList.Visible

        '===============================================
        ' ���[���X���b�g�@�\
        If g_bOutTrk Then
            mcls_MailSlotServer = New MailSlotServer(Of CommonQueMix)(g_typStartArgs.sTaskName)
            Me.Text &= " (" & g_typStartArgs.sTaskName & ")"
        Else
            If 1 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "MAIL", TASKINI_NAME) Then
                g_typStartArgs.sTaskName = TASK_NAME    ' �f�t�H���g���̂�ݒ�

                mcls_MailSlotServer = New MailSlotServer(Of CommonQueMix)(g_typStartArgs.sTaskName)
                Me.Text &= " (" & g_typStartArgs.sTaskName & ")"
            End If
        End If

        If mcls_MailSlotServer IsNot Nothing Then
            mcls_MailSlotServer.ReadStart(Me)
        End If

        ' '' ��
        ''g_bOutTrk = True
        'g_typStartArgs.sTaskName = "RPIV0"
        'mcls_RecvMail = New clsRecvMail(Me)




        '===============================================
        ' ���L�������A��
        If g_bOutTrk Then
            '''' ���L�������m�� (�������Ȃ玩���Ő����B�����ς݂Ȃ�ڑ�����)
            m_memTrkPtr = tClass_GlovalMemory.AllocMapped(SIZE_TYPE_TRK, TBL_RPI_TRK_NAME)

            '''' �O���A�����̖�������
            tsbSync.Enabled = False
            tsbSearch.Enabled = False
            tsbSearchUp.Enabled = False
            tsbSearchDown.Enabled = False
            tsbSearchOU.Enabled = False
            tsbFolder.Enabled = False

        End If




        '===============================================
        '''' RPI�\���N���X��`
        ' �r�����摜���Z�b�g
        Try
            Dim asmData As System.Reflection.Assembly = [Assembly].GetExecutingAssembly()           ' �A�Z���u���ɃA�N�Z�X
            Dim semImage As IO.Stream = asmData.GetManifestResourceStream("RPI_VIEWER.�r����.bmp") ' ���ߍ��܂ꂽ���\�[�X�ɃA�N�Z�X

            Call mcls_RpiView.SetItem_DefImg(New Bitmap(semImage))                                         ' �f�t�H���g�摜�Z�b�g

        Catch ex As Exception
            'If Not mcls_Log Is Nothing Then mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "[tClass_ViewRPI] Alloc " & ex.ToString)
        End Try


        picFrameImg.BackColor = Color.Black     ' ���Ȃ�L��


        ' ���C�u�������ŃC�x���g�n���h���ǉ���� �Ǝ��ɒǉ�������̂P (�R�[�����Ԃ̊֌W���؂�) ���C�u�����O
        'AddHandler picFrameImg.Resize, AddressOf picImg_ReSize
        'AddHandler Application.Idle, AddressOf idle


        '===============================================
        ' �󂯓n���p�R���g���[��
        '' �R���g���[���̐ݒ肪��ԍŌ�
        Dim cntl As tClass_ViewRpiThread.CNTL_ITEM
        cntl.picDspImg = picFrameImg
        cntl.vsBar = vsBar
        cntl.hsBar = hsBar

        '''''''''''''''''''''''''''''''''
        ' RPI�t�@�C���Ǎ��݃N���X����
        '' �X���b�h�̐�����
        Dim nCpuCnt As Integer = Environment.ProcessorCount - 1 ' CPU�̐�(���Œ�1�X���b�h�́A��ʕ`��悤�Ɋm�ۂ��Ă���)
        Dim nThreadNum As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "EXEC_THREAD_NUM", TASKINI_NAME)
        If nThreadNum > nCpuCnt Then nThreadNum = nCpuCnt
        If 1 > nThreadNum Then nThreadNum = 1

        ' �i������
        Dim bIsUseImgLibCLI As Boolean = CBool(tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_MODE", TASKINI_NAME))
        Dim nQuarity As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DRAW_QUARITY", TASKINI_NAME)
        'If 0 = g_nLockMode Then
        '	bIsUseImgLibCLI = False
        '	nQuarity = 0
        'End If

        '�J�X�^���f�R�[�_�ǉ�
        Call mcls_RpiView.AddDecoders(m_plugins.Decoders)
        ' �N���X����
        Call mcls_RpiView.Alloc(cntl, nThreadNum, nQuarity, bIsUseImgLibCLI)

        ' ��ǂ݃o�b�t�@��`
        mcls_RpiView.tProp_ResvNum(0) = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "RESV_NUM_PER", TASKINI_NAME)
        mcls_RpiView.tProp_ResvNum(1) = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "RESV_NUM_NEXT", TASKINI_NAME)



        '''''''''''''''''''''''''''''''''
        ' �r�ڋ߃����v�R���g���[���Z�b�g
        mcnt_Ranp(0) = pnlLed1
        mcnt_Ranp(1) = pnlLed2
        mcnt_Ranp(2) = pnlLed3
        mcnt_Ranp(3) = pnlLed4
        mcnt_Ranp(4) = pnlLed5
        mcnt_Ranp(5) = pnlLed6
        mcnt_Ranp(6) = pnlLed7
        mcnt_Ranp(7) = pnlLed8
        mcnt_Ranp(8) = pnlLed9


        '' ���C�u�������ŃC�x���g�n���h���ǉ���� �Ǝ��ɒǉ�������� (�R�[�����Ԃ̊֌W���؂�)
        'AddHandler picFrameImg.Resize, AddressOf picImg_ReSize
        'AddHandler Application.Idle, AddressOf idle


        '===============================================
        ' ��ʃN���X (�C�x���g�������Ԃɉe�����邽�߁A�L�q���Ԃ��l�����邱��)
        ' �ŗD��C�x���g
        AddHandler picFrameImg.Click, AddressOf picFrameImg_Click

        '''' �����A��~�Ǘ��N���X
        mcls_CheckKizu = New clsCheckKizuWT
        Call mcls_CheckKizu.InitCls_Param(mcls_Param)
        Call mcls_CheckKizu.InitCls_RpiView(mcls_RpiView)
        Call mcls_CheckKizu.InitFrm_Control(mfrm_FrmCont)
        Call mcls_CheckKizu.InitData_LstDetect(m_lstDetect)

        '''' �摜�����g��N���X�̏���
        mcls_ImgCutDsp = New tClass_ImageRawCutDsp(Me, picFrameImg)
        mcls_ImgCutDsp.DefRitu = 3.0
        Dim memoriinf As New tClass_ImageRawCutDsp.MEMORI_INF
        memoriinf.initialize()
        mcls_ImgCutDsp.MemoriInf = memoriinf

        '''' �؂�o���摜�\�����
        Call mfrm_Kiri.SetCls_Param(mcls_Param)

        '''' �U�z�} �S���}�b�v
        Call mfrm_DefMap.SetCls_Param(mcls_Param)
        Call mfrm_DefMap.SetCls_RpiView(mcls_RpiView)
        Call mfrm_DefMap.SetData_LstDetect(m_lstDetect)
        Call mfrm_DefMap.SetData_LstCut(m_lstCut)
        AddHandler mfrm_DefMap.tEv_MapClick, AddressOf SubMapClick
        AddHandler mfrm_DefMap.tEv_KizuClick, AddressOf SubKizuClick

        '''' �r�}�b�v �S���}�b�v
        Call mfrm_KizuMap.SetCls_Param(mcls_Param)
        Call mfrm_KizuMap.SetCls_RpiView(mcls_RpiView)
        Call mfrm_KizuMap.SetData_LstDetect(m_lstDetect)
        Call mfrm_KizuMap.SetData_LstCut(m_lstCut)
        AddHandler mfrm_KizuMap.tEv_MapClick, AddressOf SubMapClick
        AddHandler mfrm_KizuMap.tEv_KizuClick, AddressOf SubKizuClick

        '''' �r�ꗗ
        Call mfrm_KizuList.SetCls_Param(mcls_Param)
        Call mfrm_KizuList.SetCls_RpiView(mcls_RpiView)
        Call mfrm_KizuList.SetData_LstDetect(m_lstDetect)
        AddHandler mfrm_KizuList.tEv_KizuClick, AddressOf SubKizuClick
        AddHandler mfrm_KizuList.tEv_ReSetKizuDsp, AddressOf ReSetKizuDsp




        '''' �R�C���I�����
        AddHandler mfrm_Search.tEv_RpiSelect, AddressOf SelectRpiFile

        '''' �O���b�h��
        AddHandler mfrm_GridLine.tEv_RePaint, AddressOf RePaint

        '''' �R���g���[�����
        If 0 = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "OPT_AUTODOWN", TASKINI_NAME) Then
            Call mfrm_FrmCont.Disable_AutoDown()    ' �����@�\������
        End If

        Call mfrm_FrmCont.SetCls_Param(mcls_Param)
        AddHandler mfrm_FrmCont.tEv_JumpNagate, AddressOf JumpNagate
        AddHandler mfrm_FrmCont.tEv_MapJumpNagate, AddressOf Map_JumpNagate
        AddHandler mfrm_FrmCont.tEv_AutoDownUmu, AddressOf AutoDownUmu
        AddHandler mfrm_FrmCont.tEv_AutoDownCancel, AddressOf AutoDownCancel

        '''' �\���ݒ���
        mfrm_DspSet.SetMemoriProp(m_doc.drawmemori)
        AddHandler mfrm_DspSet.tEv_ReSetKizuOvl, AddressOf ReSetKizuOvl

        '''' �I�[�o�[���C�蓮�␳���
        Call mfrm_ReSetOvl.SetData_LstDetect(m_lstDetect)
        AddHandler mfrm_ReSetOvl.tEv_ReSetKizuOvl, AddressOf ReSetKizuOvl


        '''' �J���p���
        Call mfrm_Kai.SetCls_RpiView(mcls_RpiView)
        AddHandler mfrm_Kai.tEv_ReCreate, AddressOf ReCreateImg

        '''' �o�͐ݒ�
        Call mfrm_Print.SetFrm_Main(Me)
        Call mfrm_Print.SetCls_RpiView(mcls_RpiView)


        '''' �}�[�L���O�@�\
        Call mfrm_Bookmark.SetCls_RpiView(mcls_RpiView)
        Call mfrm_Bookmark.SetCls_Calc(mcls_RpiView.GetCls_ImgDsp())
        Call mfrm_Bookmark.SetCnt_Pic(picFrameImg)
        AddHandler mfrm_Bookmark.tEv_JumpNagate, AddressOf JumpNagate
        AddHandler mfrm_Bookmark.tEv_JumpHaba, AddressOf JumpHaba


        '''' ���͈ꗗ�@�\
        Call mfrm_InputList.SetCls_RpiView(mcls_RpiView)
        Call mfrm_InputList.SetCls_Calc(mcls_RpiView.GetCls_ImgDsp())
        Call mfrm_InputList.SetCnt_Pic(picFrameImg)
        AddHandler mfrm_InputList.tEv_JumpNagate, AddressOf JumpNagate
        AddHandler mfrm_InputList.tEv_JumpHaba, AddressOf JumpHaba
        AddHandler mfrm_InputList.tEv_RePaint, AddressOf RePaint
        Call mfrm_InputList.InitData_LstDetect(m_lstDetect)

        '''' �؂�o���摜����
        Call mfrm_KiriInput.SetCls_Pic(picFrameImg)
        Call mfrm_KiriInput.SetFrm_InputList(mfrm_InputList)



        '''' �e��ݒ�
        Call mfrm_Setting.SetDocument(m_doc.displayshrinker)
        Call mfrm_Setting.SetCls_RpiView(mcls_RpiView)
        AddHandler mfrm_Setting.tEv_ReCreate, AddressOf ReCreateImg

        ' ������
        Call Init(True)
        Call InitPlugins()


        '===============================================
        ' �`�����
        mcls_StopWatch.initCycle()
        Dim nWk As Integer = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "DISP_INTERVAL", TASKINI_NAME)
        Dim nInterval As Integer
        If 0 = nWk Then
            nInterval = 20
        ElseIf 1 = nWk Then
            nInterval = 60
        ElseIf 2 = nWk Then
            nInterval = 200
        End If
        timDsp.Interval = nInterval
        timDsp.Start()

        ''������������������������������������������������������������������������
#If DEBUG Then
        'Call SelectRpiFile("C:\���f���p�\�t�g\1CGL�S���\��\RPI�t�@�C��\1420130306 124037@men=0@Angle=1@No=0@5-4465      .rpi", True)
        'Call SelectRpiFile("E:\���f�[�^�L��RPI\3220131107 192311@men=0@Color=1@No=0@N029530     .rpi", True)
#End If
    End Sub

    Private Function newPluginDspLen() As RpiPlugin.IPluginActivatable
        Dim dsplen As New frmPlDspLen(Me)
        dsplen.InitFrm_Control(mfrm_FrmCont)
        Return dsplen
    End Function

    Private Sub InitPlugins()
        'TODO : �悭����X�v���b�V���X�N���[���ł��\�����邩�H

        '�����v���O�C���̏����ǉ�
        m_plugins.AddPlugin(New DrawMemori())    '�ڐ���̕`��
        m_plugins.AddPlugin(New frmPopImgControl(Me))       '�摜�␳
        m_plugins.AddPlugin(newPluginDspLen)    '�����v��

        '�O���v���O�C���̓ǂݍ��݁B
        'Using reg As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("SOFTWARE\TEX\RPI_VIEWER", False)
        '    If reg IsNot Nothing Then
        For Each dll As String In System.IO.Directory.GetFiles(System.IO.Directory.GetCurrentDirectory(), "*plugin*.dll", IO.SearchOption.AllDirectories)
            'If reg.GetValue(dll.ToString()) IsNot Nothing Then
            m_plugins.LoadDll(dll)
            '            End If
        Next
        '    End If
        'End Using

        m_plugins.Init(mcls_RpiView, Me)

        '�c�[���o�[�̎��̂���`�ς�(=�����v���O�C���p)�́A�c�[���X�g���b�v�ǉ�
        For Each ev As RpiPlugin.IEvToolStrip In m_plugins.EvToolStrips
            Dim name As String = ev.ToolStripName
            If name = "tsbMemori" Then
                ev.InitToolbar(tsbMemori, Nothing)
            End If
            If name = "tsbBrite" Then
                ev.InitToolbar(tsbBrite, Nothing)
            End If
        Next


        '�{�^�����͂��ق����v���O�C���A���́A�c�[���X�g���b�v�ǉ�
        '�g��{�^���̉E���ɑ}�����Ă����B
        Dim mevIndex As Integer = Me.tlsMainMenu.Items.IndexOf(tsbDspBig)
        mev_selectors.Add(New KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip)(tsbDspBig, Nothing))
        For Each mev As RpiPlugin.IMouseToolStrip In m_plugins.MouseInputs
            Dim tsv As ToolStripButton = mev.InitToolbar()
            If tsv Is Nothing Then Continue For

            AddHandler tsv.Click, AddressOf tsb_MouseR_Click

            mevIndex += 1
            mev_selectors.Add(New KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip)(tsv, mev))
            Me.tlsMainMenu.Items.Insert(mevIndex, tsv)
        Next


        '�ǉ��������ToolStrip
        Dim toolstrips() As ToolStrip = New ToolStrip() {tlsMainMenu, tlsMainMenu2}
        For Each plts As RpiPlugin.IToolStrip In m_plugins.ToolStrips
            plts.InitToolbar(toolstrips)
        Next

    End Sub

    Dim mev_selectors As New List(Of KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip))

    ''' <summary>
    ''' �N���[�Y
    ''' </summary>
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        timDsp.Stop()
        'Call mfrm_ImgCont.Save_last_Control_Ini()
        Call mfrm_KizuMap.Save_last_Control_Ini()

        ' ���[���X���b�g�I��
        If mcls_MailSlotServer IsNot Nothing Then
            mcls_MailSlotServer.Dispose()
            mcls_MailSlotServer = Nothing
        End If

        ' ���L�������J��
        If m_memTrkPtr <> IntPtr.Zero Then
            tClass_GlovalMemory.FreeMapped(m_memTrkPtr, TBL_RPI_TRK_NAME)
        End If

        m_doc.drawmemori.Save()
        m_doc.imagecontrol.Save()

    End Sub
    '''' <summary>
    '''' �T�[�o�[�^�C�}�[ 
    '''' </summary>
    '''' <param name="source"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub OnTimedEvent(ByVal source As Object, ByVal e As Timers.ElapsedEventArgs)
    '	timDsp_Tick(source, Nothing)
    'End Sub


    ''' <summary>
    ''' ������
    ''' </summary>
    ''' <param name="bInit">�z���g�ɏ�������true</param>
    ''' <remarks></remarks>
    Private Sub Init(ByVal bInit As Boolean)
        ' �摜�\���G���A
        Call mcls_RpiView.Init(bInit)


        ' ���\���G���A
        If bInit Then spcMain.Panel1Collapsed = True
        lsvRpiHeader.Items.Clear()
        tsbInfMode.Visible = m_bDebug
        tsbImgQuarty.Visible = m_bDebug

        ' �X�e�[�^�X�\���G���A
        If bInit Then tscRate.SelectedIndex = 0
        tscSpeed.SelectedItem = "0"

        ' �`�����l���؂�ւ�
        tscChannel.Items.Clear()
        tspbReadRpi.Value = 0
        tspbReadRpi.ForeColor = Color.Red

        ' �����g��
        m_bIsImgDrag = False
        mcls_ImgCutDsp.DspStop = True       ' �����g�傪�\�������ςȂ��ƂȂ�ׁA��x�����I�ɕ\���|�b�v�A�b�v�����
        mcls_ImgCutDsp.DspStop = False

        ' �}�E�X�����v���O�C�����A�N�e�B�u�Ȃ�A�f�t�H���g�ɂ���
        If m_active_mouse_handler IsNot Nothing Then
            m_active_mouse_handler.Activate(False)
            m_active_mouse_handler = Nothing
        End If

        ' ����
        m_typCoil = Nothing
        m_lstDetect.Clear()
        m_lstCut.Clear()

        ' �r�ڋ߃����v
        For ii As Integer = 0 To mcnt_Ranp.Length - 1
            mcnt_Ranp(ii).BackColor = SystemColors.ControlDark
        Next

    End Sub



    ''/ /////////////////////////////////////////////////////////////////////////////////
    ''/ RPI �����\���֌W

    ''' <summary>
    ''' RPI�t�@�C�����h���b�O�Ŏw�肷�� (�h���b�O)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragEnter
        ' �h���b�O�ŗL���ɂ��Ă����Ȃ��ƃh���b�v�C�x���g������Ȃ�
        ' �t�H�[���v���p�e�B�� AllowDrop ��L���ɂ��Ă����Ȃ��ƃ_��
        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            e.Effect = DragDropEffects.Copy
        Else
            e.Effect = DragDropEffects.None
        End If
    End Sub

    ''' <summary>
    ''' RPI�t�@�C�����h���b�O�Ŏw�肷�� (�h���b�v)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_DragDrop(ByVal sender As Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles Me.DragDrop
        If Not e.Data.GetDataPresent(DataFormats.FileDrop) Then Return

        '' �h���b�O���ꂽ�f�[�^�`���𒲂ׁA�t�@�C���̂Ƃ��̓R�s�[�Ƃ���
        Dim fileName As String() = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
        Dim strFPath As String = fileName(0) ' �����w�莞���l�����āA�ŏ��̂P�R���g��


        ' RPI�t�@�C�����s
        Call SelectRpiFile(strFPath, True, "")
    End Sub


    ''' <summary>
    ''' �e�X�g�p�X�N���[���[�{�^�� (�Ƃ肠�����A�摜�ĕ`��ɂ��Ă���)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRituNon_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRituNon.Click
        ' 32bit���Ɛ�ǂ݂ŃG���[�ɂȂ��āA�`��ł��Ȃ��ꍇ������΍�
        Call ReCreateImg()
    End Sub



    ''' <summary>
    ''' RPI�t�@�C�������{�I��
    ''' </summary>
    ''' <param name="sIniDbName">�Ώۂ�INI�t�@�C���Z�N�V����</param>
    ''' <param name="sKanriNo">�Ǘ�No</param>
    ''' <param name="nMen">��(0,1)</param>
    ''' <param name="nNum">�p�x�A�F (0�I���W��)</param>
    ''' <remarks></remarks>
    Private Sub SelectRpiFile(ByVal sIniDbName As String, ByVal sKanriNo As String, ByVal nMen As Integer, ByVal nNum As Integer)

        ' ����
        Const MAXNUM As Integer = 10
        Dim sRpiForder(MAXNUM - 1) As String    ' �����t�H���_
        For jj As Integer = 0 To MAXNUM - 1
            sRpiForder(jj) = tMod.tMod_IniFile.ReadIniStr(sIniDbName, String.Format("S_{0}_{1}", nMen, jj + 1), TASKINI_NAME)
        Next

        ' �t�H�[�}�b�g����
        Dim sRpiFormat As String = tMod.tMod_IniFile.ReadIniStr(sIniDbName, "RPI_FORMAT", TASKINI_NAME)
        ' �t�@�C������������
        Dim sSearchName As String = sRpiFormat.Replace("{S_FOLDER_NAME}", GetFolderName(sKanriNo))
        sSearchName = m_reParen.Replace(sSearchName, "*")
        sSearchName = String.Format(sSearchName, sKanriNo, nMen, nNum + 1)
        ' ������.rpi�ŏI����Ă��Ȃ����"*.rpi"�𖖔��ɂ���
        If Not sSearchName.EndsWith(".rpi", StringComparison.CurrentCultureIgnoreCase) Then sSearchName &= "*.rpi"


        ' �S�t�H���_�[�T�[�`
        Dim sFullPath As String = ""
        For jj As Integer = 0 To MAXNUM - 1
            If "" = sRpiForder(jj) Then Continue For

            Dim sFpath() As String = IO.Directory.GetFiles(sRpiForder(jj), sSearchName, IO.SearchOption.TopDirectoryOnly)
            If 1 = sFpath.Length Then       ' ��������ꍇ�ǂ����悤�B���肦�Ȃ��͂�������

                sFullPath = sFpath(0)
                Exit For
            End If
        Next

        ' ���s
        If "" <> sFullPath Then
            Call SelectRpiFile(sFullPath, True, sKanriNo)
        End If
    End Sub

    ''' <summary>
    ''' RPI�t�@�C���I��
    ''' </summary>
    ''' <param name="strFPath">�t���p�X</param>
    ''' <param name="bInit">true:�V�R�C��  false:�\���ύX�̂�</param>
    ''' <param name="sKanriNo">�Ǘ�No</param>
    ''' <remarks></remarks>
    Private Sub SelectRpiFile(ByVal strFPath As String, Optional ByVal bInit As Boolean = True, Optional ByVal sKanriNo As String = "")

        Dim bChangeLine As Boolean = True   ' �O��ƈقȂ郉�C����true
        Dim sOldLine As String = ""         ' �O�񃉃C��

        If Not gcls_Log Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("RPI�t�@�C���I��[{0}] init:{1} KanriNo:{2}", strFPath, bInit, sKanriNo))

        End If

        If "" = strFPath Then
            Call Init(True)     ' ������
            '�I���ʒu�ꗗ�N���A
            Call mfrm_InputList.NewCoil(String.Empty, 0, String.Empty)
            Call RePaint()      ' ��ʕ`��
            Return
        End If
        '===============================================
        ' ���O����



        '' ���݈ʒu�擾
        Dim dDspPos As Single = 0      ' ���ݕ\���ʒu
        If Not bInit Then
            dDspPos = mcls_RpiView.GetPos_DpToLen_mm(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)
        End If

        '' �O��ƈقȂ郉�C���H �P�i
        If mcls_RpiView.GetRpi_Header.IsTHeader Then
            sOldLine = mcls_RpiView.GetRpi_THeader.cSysName
        End If

        '' �����܂ŗ�����RPI�t�@�C������ 
        ' ��x�O��̂����邩���m��Ȃ��̂ŁA�N���[�Y
        For Each pRpiOpen As RpiPlugin.IRpiOpen In m_plugins.RpiOpens
            pRpiOpen.OnRpiClose()
        Next

        mcls_RpiView.RpiClose()
        Call mcls_CheckKizu.ThreadStop()
        '�X�V�^�C�}���~�߂�B
        timThreadRpiRead.Stop()



        '' �f�t�H���g�Ǘ�No�K�p(�悤��RPI�t�@�C�����ɖ��ߍ��݂̂��)
        If "" = sKanriNo Then
            'RPI�t�@�C���Ǘ�No�̕t���ւ�
            sKanriNo = tClass_ReadRPIFile.READ_FROM_RPI_FILE
        End If


        '' �I�[�v��
        If Not mcls_RpiView.RpiOpen(strFPath, , , , sKanriNo) Then
            '' Ver4.4.2 MsgBox(String.Format("RPI�t�@�C���ǂݍ��ݎ��s2 {0}", strFPath))
            Call Init(True)
            Return
        End If

        '''' ���̑� ������������
        Me.Text = ""
        Call DspRpiFile(bInit)
        Me.Text &= String.Format(" [{0}]", strFPath)


        'mcls_RpiView.tProp_LenOff(0) = -1000
        'mcls_RpiView.tProp_LenOff(1) = 100000


        ''����\�ォ��ύX�Ή�(����)
        'Dim t As tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Cls().PropRpiHeader()
        't.res_y = 0.01
        'mcls_RpiView.GetRpi_Cls().PropRpiHeader() = t
        'mcls_RpiView.PropCls_ImgDsp.SetImgInf(t.width, t.height, t.res_x, t.res_y, 0, 0, 1, 1)




        '' �O��ƈقȂ郉�C���H �Q�i
        If mcls_RpiView.GetRpi_THeader.cSysName = sOldLine Then bChangeLine = False


        '===============================================
        '''' DB������擾
        Dim sSession As String = "DB_" & mcls_RpiView.GetRpi_THeader.cSysName
        Dim nWk As Integer = tMod.tMod_IniFile.ReadIniInt(sSession, "ENABLE_DB", TASKINI_NAME)
        m_bDb = IIf(nWk = 0, False, True)



        '===============================================
        '' DB�A�N�Z�X
        'If m_bDb And m_bDrawKizuOvl And mcls_RpiView.GetRpi_Header.IsTHeader Then
        If m_bDb And mcls_RpiView.GetRpi_Header.IsTHeader Then

            '' DB������擾
            mcls_Param.Set_DbSession(sSession)
            m_bResutl = mcls_Param.ReadAll()

            '' �R�C�����֌W
            m_typCoil = Nothing
            mcls_Param.ReadZi_CoilInf(mcls_RpiView, m_typCoil)

            '' �r���֌W
            m_lstDetect.Clear()
            mcls_RpiView.ResetOverLayInf()
            mcls_Param.ReadZi_DefectInf(mcls_RpiView, m_lstDetect)

            '' �J�b�g���
            m_lstCut.Clear()
            mcls_Param.ReadZi_CutInf(mcls_RpiView, m_lstCut)

            '' ��񔽓]�ݒ�
            mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect, m_lstCut)

            '' ���̑����
            Dim nVal As Integer = mcls_Param.ReadZi_DefPitch(mcls_RpiView)      ' ���w��̏ꍇ��0�B�g�����ŗ{�����邱��
            Call mfrm_FrmCont.ChangeDefVal(nVal)
            Call mfrm_GridLine.ChangeDefVal(nVal)

        Else
            m_bResutl = False
            ' �ΏۊO�Ȃ珉�����̂�
            m_lstDetect.Clear()
            mcls_RpiView.ResetOverLayInf()
        End If



        '===============================================
        '''' �T�u�t�H�[��������
        If Not bChangeLine Then
            ' �r��t�B���^���f
            Call mfrm_KizuList.FilterList()
        End If

        If bInit Then
            Call mfrm_Bookmark.MarkDel(-1) '�S��
        End If

        ' �R�C��No�̒��o
        Dim sRpiFileName As String = System.IO.Path.GetFileNameWithoutExtension(strFPath)
        Dim arrRpiFileName As String() = sRpiFileName.Split("@"c)
        Dim sCoilNo As String = String.Empty
        If 1 < arrRpiFileName.Length Then
            sCoilNo = arrRpiFileName(arrRpiFileName.Length - 1)
        End If
        Call mfrm_InputList.NewCoil(mcls_RpiView.GetRpi_THeader.cKizukenNo, mcls_RpiView.GetRpi_THeader.nMen, sCoilNo)

        '' �R�C�����A�r���N���X�֌W�ăZ�b�g
        Call ReverceDsp(bChangeLine)

        '' �I�[�o�[���C�蓮�␳�֌W
        If mcls_RpiView.GetRpi_Header.IsTHeader Then
            Call mfrm_ReSetOvl.ChangeCoil(mcls_RpiView.GetRpi_THeader.nStartCamset, mcls_RpiView.GetRpi_THeader.nCamsetNum)
        End If


        '===============================================
        '''' ���C���ŗL�ݒ�Ǎ���
        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "OPT_SYNC", TASKINI_NAME) Then        ' ��\���ݒ�
            tsbSync.Visible = False
        Else
            tsbSync.Visible = True
        End If


        '===============================================
        '''' �`��X�V
        ' �\���ԋ����␳�Ή�
        Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)     ' ��x�������l���Z�o

        mcls_RpiView.tProp_ImgOffY = 0
        mcls_RpiView.tProp_LabelOffsetYmm = 0
        If mcls_RpiView.GetRpi_Header.IsTHeader Then
            Dim tHeader As tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader

            Dim tpos_px As Integer = tHeader.nFTposCfNo(0) * mcls_RpiView.GetRpi_Header.height + tHeader.nFTposPixel(0)
            If tpos_px <> 0 Then
                Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)     ' �X�N���[���o�[�ʍČv�Z

                If dDspPos = 0 Then
                    dDspPos = -250
                End If

                mcls_RpiView.tProp_LabelOffsetYmm = -mcls_RpiView.GetPos_RpToLen_Y(tpos_px)

                bInit = False

            Else
                'If tHeader.nStartLen < 3 * mcls_RpiView.GetRpi_Header.res_y * mcls_RpiView.GetRpi_Header.height Then
                If tHeader.nStartLen < (mcls_RpiView.GetDspAriaInf.nDspFrameCnt - 1) * mcls_RpiView.GetRpi_Header.res_y * mcls_RpiView.GetRpi_Header.height Then
                    mcls_RpiView.tProp_ImgOffY = mcls_RpiView.GetPos_LenToRp_Y(tHeader.nStartLen)   ' �\���ԋ����œK���Ή�
                    Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)     ' �X�N���[���o�[�ʍČv�Z
                End If
            End If
        End If

        ' �\���ʒu����
        Dim nDspPos As Integer
        If bInit Then
            nDspPos = vsBar.Minimum
        Else
            nDspPos = mcls_RpiView.GetPos_LenToDp_mm(dDspPos)
        End If
        Call mcls_RpiView.ScrollH(nDspPos, True)
        If Not bInit Then Call mcls_RpiView.ReCreateImg() ' ��蒼��


        mcls_CheckKizu.Enable = mfrm_FrmCont.AutoDownUmu
        Call mcls_CheckKizu.ThreadStart(Threading.ThreadPriority.Highest)


        ' ���������� ���Ή��B���b��I�B�g�債���Ƃ��Ƃ����ǂ����邩 ����l������     ��
        Dim ImgInf As tClass.tClass_ImageDsp.IMG_INF = mcls_RpiView.GetImg_ImgInf()
        If 3000.0 <= ImgInf.dSizeW Then
            m_doc.drawmemori.g_nScalStringX = 5
        Else
            m_doc.drawmemori.g_nScalStringX = 2
        End If


        For Each pRpiOpen As RpiPlugin.IRpiOpen In m_plugins.RpiOpens
            pRpiOpen.OnRpiOpen(mcls_RpiView.GetRpi_Cls)
        Next


        ' �X�N���[���l��0���AvsBar_ValueChanged�Ń��y���g������Ȃ��ׂ̑΍� (�O�ׂ̈ɗ����ĕ`��)
        Call RePaint()


        Me.Activate()
    End Sub



    ''' <summary>
    ''' RPI�t�@�C�� �����ǂݍ��ݏ���
    ''' </summary>
    ''' <param name="bInit">true:�V�R�C��  false:�\���ύX�̂�</param>
    ''' <remarks></remarks>
    Private Sub DspRpiFile(ByVal bInit As Boolean)
        Dim nMen As Integer = -1    ' �� -1:�s��    0:�\  1:��

        '===============================================
        '' RPI�N���X�ݒ�
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header

        '' TEX�w�b�_�[
        If typ.IsTHeader Then
            Dim tH As tClass.tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader
            nMen = tH.nMen
        Else

        End If


        '===============================================
        '''' �����X�V
        nudFrameNo.Value = 1
        nudFrameNo.Maximum = typ.nframe

        '''' �w�b�_�[��񕔂�\��
        Call DspHeader()


        '''' ���C���ʐݒ�
        Dim sSession As String = "DB_" & mcls_RpiView.GetRpi_THeader.cSysName
        If Not bInit Then Return ' �\���ύX�݂̂Ȃ炱���܂�


        '===============================================
        '' �\������
        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "DorW", TASKINI_NAME) Then
            mcls_RpiView.SetProp_DSWS(0)
            tsbDSWS.Checked = False
        Else
            mcls_RpiView.SetProp_DSWS(1)
            tsbDSWS.Checked = True
        End If
        If -1 <> nMen Then
            If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, String.Format("DorW_{0}", nMen), TASKINI_NAME) Then
                mcls_RpiView.SetProp_DSWS(0)
                tsbDSWS.Checked = False
            Else
                mcls_RpiView.SetProp_DSWS(1)
                tsbDSWS.Checked = True
            End If
        End If

        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "TorB", TASKINI_NAME) Then
            mcls_RpiView.SetProp_TB(0)
            tsbTB.Checked = False
        Else
            mcls_RpiView.SetProp_TB(1)
            tsbTB.Checked = True
        End If
        If -1 <> nMen Then
            If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, String.Format("TorB_{0}", nMen), TASKINI_NAME) Then
                mcls_RpiView.SetProp_TB(0)
                tsbTB.Checked = False
            Else
                mcls_RpiView.SetProp_TB(1)
                tsbTB.Checked = True
            End If
        End If


        If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, "Rotate", TASKINI_NAME) Then
            SetRotate(False)
            tsbRot.Checked = False
        Else
            SetRotate(True)
            tsbRot.Checked = True
        End If
        If -1 <> nMen Then
            If 0 = tMod.tMod_IniFile.ReadIniInt(sSession, String.Format("Rotate_{0}", nMen), TASKINI_NAME) Then
                SetRotate(False)
                tsbRot.Checked = False
            Else
                SetRotate(True)
                tsbRot.Checked = True
            End If
        End If

        '' ���E����
        m_sDrawStrLR(0) = tMod.tMod_IniFile.ReadIniStr(sSession, "STRING_L", TASKINI_NAME)
        m_sDrawStrLR(1) = tMod.tMod_IniFile.ReadIniStr(sSession, "STRING_R", TASKINI_NAME)



        '''' �X�e�[�^�X�o�[�����X�V
        tslFrameAllCnt.Text = typ.nframe.ToString
        tscChannel.Items.Clear()
        For ii As Integer = 0 To mcls_RpiView.GetRpi_Header.nchannel - 1
            tscChannel.Items.Add(String.Format("{0}ch", ii))
        Next
        tscChannel.SelectedIndex = 0
        tspbReadRpi.Value = 0
        tspbReadRpi.ForeColor = Color.Red
        tspbReadRpi.Maximum = typ.nframe
        timThreadRpiRead.Start()            ' RPI�t�@�C����荞�ݏ󋵊J�n
    End Sub

    ''' <summary>
    ''' �񓯊�RPI�t�@�C���Ǎ��ݏ�
    ''' </summary>
    Private Sub timThreadRpiRead_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timThreadRpiRead.Tick
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        Dim nCnt As Integer = mcls_RpiView.GetRpi_Cls.GetRpiFrameCount()
        If nCnt < typ.nframe Then
            tspbReadRpi.Value = nCnt
        Else
            timThreadRpiRead.Stop()
            tspbReadRpi.Value = typ.nframe
            tspbReadRpi.ForeColor = SystemColors.Highlight
        End If
    End Sub


    ''' <summary>
    ''' �\�������ύX���Ȃǂ̃T�u��ʈꊇ���f
    ''' </summary>
    ''' <param name="bCangeLine">true:�V�R�C���Ń��C�����قȂ�</param>
    ''' <remarks></remarks>
    Private Sub ReverceDsp(Optional ByVal bCangeLine As Boolean = False)
        mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect, m_lstCut)     ' ���]�ݒ�

        Dim bCoilItemDsp As Boolean = False     ' �R�C���ꗗ��� �\�����H
        If Not mfrm_CoilItem Is Nothing AndAlso mfrm_CoilItem.Visible Then
            bCoilItemDsp = True
        End If


        '' �R�C�����A�r���N���X�֌W�ăZ�b�g
        Call mfrm_KizuMap.SetData_Coil(m_typCoil)
        Call mfrm_KizuMap.MapDisp()
        Call mfrm_DefMap.MapDisp()

        If bCangeLine Then
            Call mfrm_KizuList.NewCoil()
            Call mfrm_FrmCont.ChangeLine()

            ' �ʃ��C���Ȃ̂ōĐ���
            If bCoilItemDsp Then
                mfrm_CoilItem.Close()
                mfrm_CoilItem = New frmPopCoilItem
                Call mfrm_CoilItem.SetCls_RpiView(mcls_RpiView)
                mfrm_CoilItem.Show(Me)
            End If
        Else
            If bCoilItemDsp Then
                mfrm_CoilItem.ReDsp()
            End If
        End If
        Call mfrm_KizuList.MapDisp()

        'Call mfrm_Setting.ReChange(bCangeLine)
        m_doc.displayshrinker.Reset(mcls_RpiView.GetRpi_Header.res_x, mcls_RpiView.GetRpi_Header.res_y)
        m_doc.displayshrinker.Emit()

        Call mcls_CheckKizu.CoilChange()
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        If typ.IsTHeader Then
            Call mcls_CheckKizu.MenChange(mcls_RpiView.GetRpi_THeader.nMen)
        End If

    End Sub

    Private m_doc As New GlobalProperties
    Public ReadOnly Property Properties As RpiPlugin.PeropertiesBase Implements RpiPlugin.IPropertiesProvider.Document
        Get
            Return m_doc
        End Get
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[���X���b�g��M

    ''' <summary>
    ''' ���[���X���b�g��M
    ''' </summary>
    ''' <param name="que"></param>
    ''' <remarks></remarks>
    Private Sub mcls_RecvMail_tEv_RecvMail(ByVal que As CommonQueMix) Handles mcls_MailSlotServer.tEv_RecvMail

        'MsgBox(String.Format("{0}/{1},{2}", que.nEventNo, que.idata(0), que.idata(1)))
        Select Case que.nEventNo
            ' ��{�n -------------------------------------------------
            Case FACT_RPIV_END      ' �I��
                Me.Close()

            Case FACT_RPIV_DSPING       ' RPI�t�@�C���\��
                ' MIX_S[0] : RPI�t�H���_�p�X (�������Ƀt���p�X�����Ă�OK)
                ' MIX_S[1] : RPI�t�@�C����1
                ' MIX_S[2] : RPI�t�@�C����2	(��S[2]�Ƀf�[�^���Z�b�g����Ă���ꍇ�́A�����I�ɂ͒P����S[0]�`[2]��A�����邾���Ƃ���)

                ' MIX_I[0] : �����`�����l���� (0�I���W��)
                ' MIX_I[1] : Y���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
                ' MIX_I[2] : Y����[mm]
                ' MIX_I[3] : X���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
                ' MIX_I[4] : X����[mm] (-1:�ړ������Ȃ�)
                ' MIX_I[5] : X����[mm]���ǂ����̎w�肩��H(0:���[  1:�Z���^�[����)

                ' �ȉ��R���ڂ̈��� (0:���� -9:���\���ɑ΂��Ĕ��] -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 1:���摜�ɑ΂��Ĕ��] 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��])
                ' MIX_I[6] : ���E���]
                ' MIX_I[7] : �㉺���]
                ' MIX_I[8] : 90�x���]
                Call SelectRpiFile(que)

            Case FACT_RPIV_ACTIVE       ' RPI�r���[�A�[ �A�N�e�B�u
                Me.Activate()

                ' MIX_S[0] : �Ώۂ�INI�t�@�C���Z�N�V����
                ' MIX_S[1] : �Ǘ�No

                ' MIX_I[0] : ��(0,1)
                ' MIX_I[1] : �p�x�A�F (0�I���W��)
            Case FACT_RPIV_SEARCH_DSPING    ' RPI�t�@�C�������{�\��
                Call SelectRpiFile(que.cdata1, que.cdata2, que.idata(0), que.idata(1))


                ' ��ʑ���n -------------------------------------------------
            Case FACT_RPIV_DSP_MOVE     ' �w��ʒu
                ' MIX_I[0] : Y���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
                ' MIX_I[1] : Y����[mm]
                ' MIX_I[2] : X���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
                ' MIX_I[3] : X����[mm] (-1:�ړ������Ȃ�)
                ' MIX_I[4] : X����[mm]���ǂ����̎w�肩��H(0:���[  1:�Z���^�[����)
                If 0 <> que.idata(0) Then JumpNagate(que.idata(0), que.idata(1))
                If 0 <> que.idata(2) Then JumpHaba(que.idata(2), que.idata(3), que.idata(4))


                '----------------------------------------------------
                ' ���]
                ' �ȉ��R���ڂ̈��� (0:���� -9:���\���ɑ΂��Ĕ��] -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 1:���摜�ɑ΂��Ĕ��] 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��])
                ' MIX_I[0] : ���E���]
                ' MIX_I[1] : �㉺���]
                ' MIX_I[2] : 90�x���]
            Case FACT_RPIV_DSP_TURN     ' �\�������ؑ�
                If 0 <> que.idata(0) Then Call TurnDSWS(que.idata(0))
                If 0 <> que.idata(1) Then Call TurnTB(que.idata(1))
                If 0 <> que.idata(2) Then Call TurnRotate(que.idata(2))
        End Select

    End Sub



    ''' <summary>
    ''' RPI�t�@�C���I��
    ''' </summary>
    ''' <param name="que">���[���X���b�g</param>
    ''' <remarks></remarks>
    Private Sub SelectRpiFile(ByVal que As CommonQueMix)

        ' MIX_S[0] : RPI�t�H���_�p�X (�������Ƀt���p�X�����Ă�OK)
        ' MIX_S[1] : RPI�t�@�C����1
        ' MIX_S[2] : RPI�t�@�C����2	(��S[2]�Ƀf�[�^���Z�b�g����Ă���ꍇ�́A�����I�ɂ͒P����S[0]�`[2]��A�����邾���Ƃ���)

        ' MIX_I[0] : �����`�����l���� (0�I���W��)
        ' MIX_I[1] : Y���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
        ' MIX_I[2] : Y����[mm]
        ' MIX_I[3] : X���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)
        ' MIX_I[4] : X����[mm] (-1:�ړ������Ȃ�)
        ' MIX_I[5] : X����[mm]���ǂ����̎w�肩��H(0:���[  1:�Z���^�[����)

        ' �ȉ��R���ڂ̈��� (0:���� -9:���\���ɑ΂��Ĕ��] -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 1:���摜�ɑ΂��Ĕ��] 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��])
        ' MIX_I[6] : ���E���]
        ' MIX_I[7] : �㉺���]
        ' MIX_I[8] : 90�x���]
        'que.cdata1 = "c:\2420130828 134704@men=0@Angle=1@No=0@846382  .rpi"

        ' �`�����l���w��
        mcls_RpiView.GetRpi_Cls.PropRpiActiveCh = que.idata(0)

        ' RPI�t�@�C���ǂݍ���
        Dim sFPath As String
        If 0 = que.cdata3.Length Then
            sFPath = IO.Path.Combine(que.cdata1, que.cdata2)
        Else
            sFPath = que.cdata1 & que.cdata2 & que.cdata3
        End If
        Call SelectRpiFile(sFPath)


        ' ���]����	�����̎��_�ł́Aini�t�@�C���ݒ�Ŕ��]��������
        If 0 <> que.idata(6) Then Call TurnDSWS(que.idata(6))
        If 0 <> que.idata(7) Then Call TurnTB(que.idata(7))
        If 0 <> que.idata(8) Then Call TurnRotate(que.idata(8))

        ' �����\���ʒu
        If 0 <> que.idata(1) Then JumpNagate(que.idata(1), que.idata(2))
        If 0 <> que.idata(3) Then JumpHaba(que.idata(3), que.idata(4), que.idata(5))


        ' �ĕ`��
        Call ReverceDsp()
        Call ReCreateImg()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��������
#Region "��������"
    ''' <summary>
    ''' RPI�w�b�_�[�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspHeader()
        '' ���X�g������
        lsvRpiHeader.Items.Clear()
        '' �f�[�^�\��
        Dim typ As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        Dim itm As ListViewItem


        itm = lsvRpiHeader.Items.Add("�摜�`��")
        If typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_BMP Then
            itm.SubItems.Add("�񈳏k bmp")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_RAW Then
            itm.SubItems.Add("�񈳏k raw")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_JPEG Then
            itm.SubItems.Add("���k jpg")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC Then
            itm.SubItems.Add("���k LAC")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_NUC Then
            itm.SubItems.Add("���k NUC")
        ElseIf typ.emCompType = tClass_ReadRPI.EM_COMP_TYPE.COMP_TYPE_LAC5 Then
            itm.SubItems.Add("���k LAC5")
        Else
            itm.SubItems.Add(tClass.tClass_ReadRPI.GetString(typ.emCompType))
        End If

        itm = lsvRpiHeader.Items.Add("����\�L��")
        If typ.IsRes Then
            itm.SubItems.Add("�L��")
        Else
            itm.SubItems.Add("����")
        End If

        itm = lsvRpiHeader.Items.Add("�ڰя��")
        If typ.IsFrm Then
            itm.SubItems.Add("�L��")
        Else
            itm.SubItems.Add("����")
        End If

        itm = lsvRpiHeader.Items.Add("�^�C�v")
        itm.SubItems.Add(typ.type)

        itm = lsvRpiHeader.Items.Add("header_size")
        itm.SubItems.Add(String.Format("{0}", typ.header_size))

        itm = lsvRpiHeader.Items.Add("width")
        itm.SubItems.Add(String.Format("{0}", typ.width))

        itm = lsvRpiHeader.Items.Add("height")
        itm.SubItems.Add(String.Format("{0}", typ.height))

        itm = lsvRpiHeader.Items.Add("nchannel")
        itm.SubItems.Add(String.Format("{0}", typ.nchannel))

        itm = lsvRpiHeader.Items.Add("pixel_nbit")
        itm.SubItems.Add(String.Format("{0}", typ.pixel_nbit))

        itm = lsvRpiHeader.Items.Add("pitch_nbyte")
        itm.SubItems.Add(String.Format("{0}", typ.pitch_nbyte))

        itm = lsvRpiHeader.Items.Add("nframe")
        itm.SubItems.Add(String.Format("{0}", typ.nframe))

        itm = lsvRpiHeader.Items.Add("frame_nbyte")
        itm.SubItems.Add(String.Format("{0}", typ.frame_nbyte))

        itm = lsvRpiHeader.Items.Add("frame_start_pos")
        itm.SubItems.Add(String.Format("{0}", typ.frame_start_pos))

        itm = lsvRpiHeader.Items.Add("frame_pitch_nbyte")
        itm.SubItems.Add(String.Format("{0}", typ.frame_pitch_nbyte))

        itm = lsvRpiHeader.Items.Add("�ڰя����")
        itm.SubItems.Add(String.Format("{0}", typ.frame_info_type))

        itm = lsvRpiHeader.Items.Add("reso_x")
        itm.SubItems.Add(String.Format("{0}", typ.res_x))

        itm = lsvRpiHeader.Items.Add("reso_y")
        itm.SubItems.Add(String.Format("{0}", typ.res_y))


        'TEX�w�b�_�[�L��
        If typ.IsTHeader Then
            Dim tH As tClass.tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader
            Dim nNumCam As Integer          ' �J�������

            If tH.nTHeaderVer = 1 Then
                nNumCam = 8 - 1     ' Ver1 : 8��
            Else
                nNumCam = 16 - 1    ' Ver2 : 16��
            End If

            Me.Text = String.Format("�ΏۃV�X�e��={0}, �r���Ǘ�No={1}, ��={2}", tH.cSysName, tH.cKizukenNo, tH.nMen)

            lsvRpiHeader.Items.Add("�yTEXͯ�ށz-------------------------")

            itm = lsvRpiHeader.Items.Add("�w�b�_�[�T�C�Y") : itm.SubItems.Add(String.Format("{0}", tH.nTHeaderSize))
            itm = lsvRpiHeader.Items.Add("�o�[�W����") : itm.SubItems.Add(String.Format("{0}", tH.nTHeaderVer))
            itm = lsvRpiHeader.Items.Add("�V�X�e������") : itm.SubItems.Add(String.Format("{0}", tH.cSysName))
            itm = lsvRpiHeader.Items.Add("�r���Ǘ�No") : itm.SubItems.Add(String.Format("{0}", tH.cKizukenNo))
            itm = lsvRpiHeader.Items.Add("�ڰя������") : itm.SubItems.Add(String.Format("{0}", tH.frame_info_type))
            itm = lsvRpiHeader.Items.Add("�ڰя�񻲽�") : itm.SubItems.Add(String.Format("{0}", tH.frame_info_size))
            itm = lsvRpiHeader.Items.Add("��ِ擪�ڰ�No") : itm.SubItems.Add(String.Format("{0}", tH.nFrameNumInit))
            itm = lsvRpiHeader.Items.Add("���̺�ِ擪FNo") : itm.SubItems.Add(String.Format("{0}", tH.nFrameNumStart))
            itm = lsvRpiHeader.Items.Add("��ْP�ʉ�̧�ٖ�") : itm.SubItems.Add(String.Format("{0}", tH.nFileNum))
            itm = lsvRpiHeader.Items.Add("ײݾ�����f") : itm.SubItems.Add(String.Format("{0}", tH.nLineCenter))
            itm = lsvRpiHeader.Items.Add("���[��׾�Ĕԍ�") : itm.SubItems.Add(String.Format("{0}", tH.nStartCamset))
            itm = lsvRpiHeader.Items.Add("�������ڰь�����") : itm.SubItems.Add(String.Format("{0}", tH.nCamsetNum))
            itm = lsvRpiHeader.Items.Add("�J�����p�x") : itm.SubItems.Add(String.Format("{0}", tH.nCamAngle))
            itm = lsvRpiHeader.Items.Add("�\���敪") : itm.SubItems.Add(String.Format("{0}", tH.nMen))
            For ii As Integer = 0 To nNumCam
                itm = lsvRpiHeader.Items.Add(String.Format(" {0} ��ʒu", ii + 1)) : itm.SubItems.Add(String.Format("{0}", tH.caminfo(ii).dBaseXoffset))
                itm = lsvRpiHeader.Items.Add(String.Format(" {0} ���f", ii + 1)) : itm.SubItems.Add(String.Format("{0}", tH.caminfo(ii).nBaseLeft))
                itm = lsvRpiHeader.Items.Add(String.Format(" {0} �EOLV��", ii + 1)) : itm.SubItems.Add(String.Format("{0}", tH.caminfo(ii).nOverLap))
            Next
            itm = lsvRpiHeader.Items.Add("�擪�ڰђ���ʒu") : itm.SubItems.Add(String.Format("{0}", tH.nStartLen))
            itm = lsvRpiHeader.Items.Add("�摜�`��") : itm.SubItems.Add(String.Format("{0}", tH.nColorType))
            itm = lsvRpiHeader.Items.Add("�r���^�C�v") : itm.SubItems.Add(String.Format("{0}", tH.nSysType))
            itm = lsvRpiHeader.Items.Add("��[�ڰ�No") : itm.SubItems.Add(String.Format("{0}", tH.nFTposCfNo(0)))
            itm = lsvRpiHeader.Items.Add("��[�ڰѓ�pixel") : itm.SubItems.Add(String.Format("{0}", tH.nFTposPixel(0)))
            itm = lsvRpiHeader.Items.Add("��[�ڰ�No") : itm.SubItems.Add(String.Format("{0}", tH.nFTposCfNo(1)))
            itm = lsvRpiHeader.Items.Add("��[�ڰѓ�pixel") : itm.SubItems.Add(String.Format("{0}", tH.nFTposPixel(1)))

        End If

    End Sub

    ''' <summary>
    ''' �摜�ۑ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSave_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSave.Click
        'Dim nNow As Integer = CInt(mcls_RpiView.GetDspAriaInf.nDspFno)
        Dim nNow As Integer = CInt(nudFrameNo.Value)
        Dim sSaveName As String = String.Format("{0}.bmp", nNow)

        Try
            Dim Buf As tClass_ViewRpiThread.CLS_IMG_INF = mcls_RpiView.GetCls_Img_inf(nNow)
            Dim Img As Bitmap = Buf.pImgRaw
            Dim bRet As Boolean = tClass.tClass_BmpJpeg.BmpFileSave(sSaveName, Img)

        Catch ex As Exception
            'Img.Save(sSaveName, Drawing.Imaging.ImageFormat.Bmp)
        End Try


        ' �����
        sSaveName = String.Format("{0}.bin", nNow)
        Dim bwk() As Byte = mcls_RpiView.GetRpi_Cls().ReadFile_ImgByte(nNow)
        IO.File.WriteAllBytes(sSaveName, bwk)

    End Sub
#End Region



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���ʊ�{����

    ''' <summary>
    ''' �ĕ`��
    ''' </summary>
    Private Sub RePaint()
        Call mcls_RpiView.RePaint()
    End Sub

    ''' <summary>
    ''' �摜�Đ���
    ''' </summary>
    Private Sub ReCreateImg()
        ' �摜��ύX���邽�� �摜����蒼��
        Call mcls_RpiView.ReCreateImg()
        Call mcls_RpiView.RePaint()
    End Sub

    ''' <summary>
    ''' �摜�T�C�Y�ύX
    ''' </summary>
    Private Sub ReSizeImg(ByVal s As Size) Handles mcls_RpiView.tEv_ReSize
        'm_bResizeSkip = True

        If s.Width <> picFrameImg.ClientSize.Width Then
            ' ���T�C�Y�������ɂ� �摜����蒼��
            Call mcls_RpiView.ReCreateImg()
        End If

        ' �ύX���ʂ𑼃N���X�ɒʒm
        Call RePaint()  ' �ĕ`��
    End Sub

    ''' <summary>
    ''' �I�[�o�[���C�ăZ�b�g
    ''' </summary>
    Private Sub ReSetKizuOvl()
        If Not m_bDb Or Not m_bDrawKizuOvl Then Return
        Call SetKizuOvl(mcls_RpiView.GetDspAriaInf.nDspFno + 1, mcls_RpiView.GetDspAriaInf.nDspFnoEnd + 1)
        Call RePaint()
    End Sub

    ''' <summary>
    ''' �r�\����ԕύX
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReSetKizuDsp()
        If Not m_bDb Or Not m_bDrawKizuOvl Then Return

        Call mfrm_KizuMap.SetData_Coil(m_typCoil)
        Call mfrm_DefMap.MapDisp()
        Call mfrm_KizuMap.MapDisp()
        Call mfrm_KizuList.MapDisp()

        Call SetKizuOvl(mcls_RpiView.GetDspAriaInf.nDspFno + 1, mcls_RpiView.GetDspAriaInf.nDspFnoEnd + 1)
        Call RePaint()

    End Sub



    ''' <summary>
    ''' ��ʕ\���ʒu���擾 [mm]
    ''' </summary>
    ''' <param name="nXY">0:X 1:Y</param>
    ''' <param name="dRate">0:��[  1:���[</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetPos(ByVal nXY As Integer, ByVal dRate As Double) As Integer
        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf
        Dim nWkDp As Integer
        Dim nAriaDp As Integer  ' �\���G���A
        If 0 = nXY Then
            nWkDp = mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X
            nAriaDp = mcls_RpiView.GetImg_AriaInf.nAriaWidth

        Else
            nWkDp = mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y
            nAriaDp = mcls_RpiView.GetImg_AriaInf.nAriaHeight

        End If
        Return CInt(mcls_RpiView.GetPos_DpToLen_mm(CInt(nWkDp + nAriaDp * dRate)))
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �`��

    ''' <summary>
    ''' �s�N�`���[�{�b�N�X �N���b�N
    ''' </summary>
    Private Sub picFrameImg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        If Not mcls_RpiView.IsRpi() Then Return

        If m_active_mouse_handler IsNot Nothing Then
            If m_active_mouse_handler.OnMouseClick(e) Then
                Return
            End If
        End If


        ' �ĕ`��
        Call mcls_RpiView.RePaint()
    End Sub

    ''' <summary>
    ''' �摜�\���N���X�`�抮���C�x���g
    ''' </summary>
    Private Sub mcls_RpiView_tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs) Handles mcls_RpiView.tEv_Paint_End
        ' �x�[�X���擾
        Dim ImgInf As tClass.tClass_ImageDsp.IMG_INF = mcls_RpiView.GetImg_ImgInf()
        Dim ImgKind As tClass.tClass_ImageDsp.IMG_KIND = mcls_RpiView.GetImg_ImgKind()
        Dim AriaInf As tClass.tClass_ImageDsp.ARIA_INF = mcls_RpiView.GetImg_AriaInf()
        Dim CalcAns As tClass.tClass_ImageDsp.CALC_ANS = mcls_RpiView.GetImg_CalcAns()

        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf()

        For Each d As RpiPlugin.IDrawer In m_plugins.Drawers
            d.OnDraw(e.Graphics)
        Next


        ' ��{
        Dim cBsh As SolidBrush = New SolidBrush(Color.Cyan)
        Dim cFnt As Font = New System.Drawing.Font("�l�r �S�V�b�N", 16, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim cPen As Pen = New System.Drawing.Pen(Color.Cyan, 2)


        ' ================================================
        ' �T���v����p
        If 1 = g_nLockMode Then
            If mcls_RpiView.PropRotate90 Then

                If 20000 < mcls_RpiView.GetPos_DpToLen_Y(DspInf.DspPixelTotalOffset.Y) Then
                    e.Graphics.FillRectangle(Brushes.Black, 0, 0, picFrameImg.ClientSize.Width, picFrameImg.ClientSize.Height)
                End If

                cBsh.Color = Color.Red
                Using wkFnt As Font = New System.Drawing.Font("�l�r �S�V�b�N", 50, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                    e.Graphics.DrawString("�T���v���� (��[20m�̂�)", wkFnt, cBsh, 10, 10)
                End Using

            End If
        End If





        ' ================================================
        ' ���E�����`��
        Dim nIndex As Integer
        cBsh.Color = Color.Cyan
        '���`��
        nIndex = IIf(mcls_RpiView.GetCls_ImgDsp.tProp_ImgKind_IsTurnLR, 1, 0)
        If Not mcls_RpiView.PropRotate90 Then
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, 5, 5)
        Else
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, 5, picFrameImg.ClientSize.Height - 20)
        End If
        '�E�`��
        nIndex = IIf(mcls_RpiView.GetCls_ImgDsp.tProp_ImgKind_IsTurnLR, 0, 1)
        If Not mcls_RpiView.PropRotate90 Then
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, picFrameImg.ClientSize.Width - 20, 5)
        Else
            e.Graphics.DrawString(m_sDrawStrLR(nIndex), cFnt, cBsh, 5, 5)
        End If



        ' ================================================
        ' �G�b�W�ʒu
        Try
            ' �`����
            Dim nAddDsp As Integer = 0                                          ' 1:�㉺�ɏ����� 2:�c�S�� 0:���� 
            If mcls_RpiView.GetRpi_Header().IsTHeader AndAlso 1 = mcls_RpiView.GetRpi_THeader().frame_info_type Then
                If m_bDrawEdge Then
                    nAddDsp = 2
                Else
                    nAddDsp = 1
                End If
            End If


            '' �ǉ��`��
            If 0 <> nAddDsp Then
                Dim nWk As Integer
                Dim typFinfo As tClass_ReadRPI.T_RPI_FRAME_INFO_T1
                If 1 = nAddDsp Then
                    Using wkPen As Pen = New System.Drawing.Pen(Color.Magenta, 2)

                        Dim nSFno As Integer = DspInf.nDspFno + 1
                        Dim nEndFno As Integer = mcls_RpiView.GetOffset_TotalToOne_FrameNo(DspInf.DspPixelTotalOffset.Y + picFrameImg.ClientSize.Height) + 1
                        If nSFno <= 0 Then nSFno = 1
                        If nSFno >= mcls_RpiView.GetRpi_Header().nframe Then nSFno = mcls_RpiView.GetRpi_Header().nframe
                        If nEndFno <= 0 Then nEndFno = 1
                        If nEndFno >= mcls_RpiView.GetRpi_Header().nframe Then nEndFno = mcls_RpiView.GetRpi_Header().nframe


                        ' �G�b�W�ʒu�\�� ��
                        If nSFno > 0 Then
                            typFinfo = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(nSFno) ' �t�����
                            For ii As Integer = 0 To 1  ' ���E
                                nWk = mcls_RpiView.GetPosition(typFinfo.nEdgeIdx(ii), 0).X  ' ��
                                If Not mcls_RpiView.PropRotate90 Then
                                    e.Graphics.DrawLine(wkPen, nWk, 0, nWk, 5)
                                Else
                                    e.Graphics.DrawLine(wkPen, 0, nWk, 5, nWk)
                                End If

                            Next
                        End If

                        ' �G�b�W�ʒu�\�� ��
                        typFinfo = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(nEndFno)  ' �t�����
                        For ii As Integer = 0 To 1  ' ���E
                            nWk = mcls_RpiView.GetPosition(typFinfo.nEdgeIdx(ii), 0).X  ' ��
                            If Not mcls_RpiView.PropRotate90 Then
                                e.Graphics.DrawLine(wkPen, nWk, picFrameImg.ClientSize.Height - 5, nWk, picFrameImg.ClientSize.Height)
                            Else
                                e.Graphics.DrawLine(wkPen, picFrameImg.ClientSize.Width - 5, nWk, picFrameImg.ClientSize.Width, nWk)
                            End If

                        Next
                    End Using
                ElseIf 2 = nAddDsp Then
                    Using wkPen As Pen = New System.Drawing.Pen(Color.Magenta, 1)
                        For ii As Integer = DspInf.nDspFno To DspInf.nDspFnoEnd
                            If ii < 0 Then Continue For
                            If ii >= mcls_RpiView.GetRpi_Header().nframe Then Continue For

                            typFinfo = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(ii + 1)    ' �t�����
                            If typFinfo.nEdgeIdx Is Nothing Then Continue For

                            Dim ys As Integer = mcls_RpiView.GetOffsetY_OneToTotal(ii, 0)
                            Dim ye As Integer = mcls_RpiView.GetOffsetY_OneToTotal(ii + 1, 0)
                            ys -= DspInf.DspPixelTotalOffset.Y
                            ye -= DspInf.DspPixelTotalOffset.Y

                            For jj As Integer = 0 To 1  ' ���E
                                nWk = mcls_RpiView.GetPosition(typFinfo.nEdgeIdx(jj), 0).X  ' ��
                                If Not mcls_RpiView.PropRotate90 Then
                                    e.Graphics.DrawLine(wkPen, nWk, ys, nWk, ye)
                                Else
                                    e.Graphics.DrawLine(wkPen, ys, nWk, ye, nWk)
                                End If
                            Next
                        Next
                    End Using
                End If

            End If
        Catch ex As Exception
        End Try

        ' ================================================
        ' �O���b�h��

        If mfrm_GridLine.Visible Then
            Dim nWk As Integer

            ' ����A�����c���ɕϊ�
            Dim nIntvalX As Integer
            Dim nIntvalY As Integer
            Dim nFrmX As Integer
            Dim nFrmY As Integer
            If Not mcls_RpiView.PropRotate90 Then
                nIntvalX = mfrm_GridLine.g_nX
                nIntvalY = mfrm_GridLine.g_nY
                nFrmX = picFrameImg.Size.Width
                nFrmY = picFrameImg.Size.Height
            Else
                nIntvalX = mfrm_GridLine.g_nY
                nIntvalY = mfrm_GridLine.g_nX
                nFrmX = picFrameImg.Size.Height
                nFrmY = picFrameImg.Size.Width
            End If

            ' �t�H�[���̏c�A���̍l�����̂܂�
            Dim nDspTotalOffY As Integer = DspInf.DspPixelTotalOffset.Y + mcls_RpiView.GetPos_LenToDp_Y(mfrm_GridLine.g_nOffsetY)
            Dim nDspTotalOffX As Integer = DspInf.DspPixelTotalOffset.X + mcls_RpiView.GetPos_LenToDp_X(mfrm_GridLine.g_nOffsetX)

            Dim nY(2 - 1) As Integer  ' �������`��擪/���[��mm
            nY(0) = CInt(mcls_RpiView.GetPos_DpToLen_mm(nDspTotalOffY) / mfrm_GridLine.g_nY) * mfrm_GridLine.g_nY
            nY(1) = CInt(mcls_RpiView.GetPos_DpToLen_mm(nDspTotalOffY + nFrmY))

            Dim nX(2 - 1) As Integer  ' �������`��擪/���[��mm
            nX(0) = CInt(mcls_RpiView.GetPos_DpToLen_X(nDspTotalOffX) / mfrm_GridLine.g_nX) * mfrm_GridLine.g_nX
            nX(1) = CInt(mcls_RpiView.GetPos_DpToLen_X(nDspTotalOffX + nFrmX))

            ' �`��
            Using wkPen As Pen = New System.Drawing.Pen(Color.Red, 1)
                wkPen.DashStyle = Drawing2D.DashStyle.Dash

                For ii As Integer = nY(0) To nY(1) Step mfrm_GridLine.g_nY
                    nWk = mcls_RpiView.GetPos_LenToDp_Y(ii) - nDspTotalOffY    ' mm����pixel�ɕϊ�

                    If Not mcls_RpiView.PropRotate90 Then
                        e.Graphics.DrawLine(wkPen, 0, nWk, nFrmX, nWk)
                    Else
                        e.Graphics.DrawLine(wkPen, nWk, 0, nWk, nFrmX)
                    End If
                Next

                For ii As Integer = nX(0) To nX(1) Step mfrm_GridLine.g_nX
                    nWk = mcls_RpiView.GetPos_LenToDp_X(ii) - nDspTotalOffX    ' mm����pixel�ɕϊ�
                    If Not mcls_RpiView.PropRotate90 Then
                        e.Graphics.DrawLine(wkPen, nWk, 0, nWk, nFrmY)
                    Else
                        e.Graphics.DrawLine(wkPen, 0, nWk, nFrmY, nWk)
                    End If
                Next
            End Using
        End If

        ' =================================
        ' �J�b�g��  ��90�x���Ή�
        If 0 <> m_lstCut.Count Then
            Dim nMmS As Single = mcls_RpiView.GetPos_DpToLen_mm(DspInf.DspPixelTotalOffset.Y)
            Dim nMmE As Single = mcls_RpiView.GetPos_DpToLen_mm(DspInf.DspPixelTotalOffset.Y + picFrameImg.ClientSize.Height) + 1
            Dim nDspTotalOffY As Integer = DspInf.DspPixelTotalOffset.Y + mcls_RpiView.GetPos_LenToDp_Y(mfrm_GridLine.g_nOffsetY)

            For ii As Integer = 0 To m_lstCut.Count - 1
                Dim wk As DATA_CUT_INF = m_lstCut(ii)
                If wk.nY >= nMmS And wk.nY <= nMmE Then
                    Using wkPen As Pen = New System.Drawing.Pen(Color.Green, 2)
                        wkPen.DashStyle = Drawing2D.DashStyle.Dash

                        Dim nWk As Integer = mcls_RpiView.GetPos_LenToDp_mm(wk.nY) - nDspTotalOffY  ' mm����pixel�ɕϊ�

                        e.Graphics.DrawLine(wkPen, 0, nWk, picFrameImg.ClientSize.Width, nWk)
                    End Using
                End If
            Next
        End If




        ' =================================
        ' �u�b�N�}�[�N
        Call mfrm_Bookmark.PaintPos(e.Graphics, DspInf.nDspFno, DspInf.nDspFrameCnt)


        ' =================================
        ' �I���ʒu
        Call mfrm_InputList.PaintPos(e.Graphics, DspInf.nDspFno, DspInf.nDspFrameCnt)


        ' ================================================
        ' TEX�w�b�_�[�L��̎��̂� �t�����g���ĉ�ʒǉ��`��
        Using wkFnt As Font = New System.Drawing.Font("�l�r �S�V�b�N", 10, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            '' ����
            Dim nDspFno As Integer = DspInf.nDspFno     ' �\���擪�t���[��No (0�I���W��)
            Dim bAddDsp As Boolean = m_bDebug
            If Not mcls_RpiView.GetRpi_Header().IsTHeader Then bAddDsp = False
            If 1 <> mcls_RpiView.GetRpi_THeader().frame_info_type Then bAddDsp = False
            If nDspFno < 0 Then bAddDsp = False
            If nDspFno >= mcls_RpiView.GetRpi_Header().nframe Then bAddDsp = False
            '' �ǉ��`��
            If bAddDsp Then
                Dim sWk As String = ""
                Dim typFinfo As tClass_ReadRPI.T_RPI_FRAME_INFO_T1 = mcls_RpiView.GetRpi_Cls().GetRpiFrameAdd1(nDspFno + 1) ' �t�����


                ' �t���[�����
                Dim nX As Integer = AriaInf.nAriaWidth - 100
                Dim nY As Integer = 5

                sWk = String.Format("Fno={0}", typFinfo.nFNo)
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY)
                nY += 10
                sWk = String.Format("len={0}", typFinfo.nFromLen / 1000)
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY)
                nY += 10

                sWk = String.Format("mpm={0}", CInt(typFinfo.nSpeed * 60.0 / 1000))
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY) '
                nY += 10
                sWk = String.Format("vRes={0:0.00000}", typFinfo.vRes)
                e.Graphics.DrawString(sWk, wkFnt, cBsh, nX, nY)
            End If
        End Using
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ������� RPI ����

    ''' <summary>
    ''' �摜�����C�x���g (�ʃX���b�h)
    ''' </summary>
    Private Sub mcls_RpiView_tEv_CreatingImg(ByVal p As tClass_ViewRpiThread.CLS_IMG_INF, ByRef img As Bitmap) Handles mcls_RpiView.tEv_CreatingImg
        'img.Save(String.Format(".\r{0}.bmp", p.nIndex), Drawing.Imaging.ImageFormat.Bmp)
        For Each f As RpiPlugin.IFilterBitmap In m_plugins.FilterBitmaps
            f.OnFilterImage(img, p)
        Next

        '=====================================

        'img.Save(String.Format(".\d{0}.bmp", p.nIndex), Drawing.Imaging.ImageFormat.Bmp)

    End Sub


    ''' <summary>
    ''' ���������p�ɁA���S����AInterval�Ԋu�i�������j�Ń��x�����쐬����
    ''' </summary>
    ''' <param name="pxCcenter">���S</param>
    ''' <param name="mmInterval">�������Ԋu</param>
    ''' <param name="nCycle">�������Ԋu</param>
    ''' <param name="rateDw">mm->px�䗦</param>
    ''' <returns>���������W�E���x���z��</returns>
    ''' <remarks>���x���̓������̂Q�_���ō쐬</remarks>
    Function GenerateHabaLabels(ByVal dspxTotalOffset As Integer, ByVal pxCcenter As Integer, ByVal mmInterval As Integer, ByVal nCycle As Integer, ByVal rateDw As Double) _
     As List(Of LabelInfo)

        Dim rlist As New List(Of LabelInfo)

        '���S
        rlist.Add(New LabelInfo(pxCcenter - dspxTotalOffset, "0mm"))

        '�v���X�}�C�i�X15�{
        For ii As Integer = 1 To 30
            For jj As Integer = -1 To 1 Step 2 '�v���X�}�C�i�X
                Dim pp As Integer = CInt(pxCcenter + jj * ii * mmInterval * rateDw) - dspxTotalOffset
                '���x���� �P��΂��ŕ\��
                'If 0 = (ii Mod 2) Then
                If 0 = (ii Mod nCycle) Then
                    rlist.Add(New LabelInfo(pp, CInt(ii * mmInterval).ToString()))
                Else
                    '�������̂�
                    rlist.Add(New LabelInfo(pp, ""))
                End If
            Next
        Next
        Return rlist
    End Function


    ''' <summary>
    ''' �\���擪�t���[��No�ύX
    ''' </summary>
    ''' <param name="nFrameNoStart">����̕\���擪�t���[��No(1�I���W��)</param>
    ''' <param name="nFrameNoEnd">����̕\����[�t���[��No(1�I���W��)</param> 
    Private Sub mcls_RpiView_tEv_ChangeDspFrameNo(ByVal nFrameNoStart As Integer, ByVal nFrameNoEnd As Integer) Handles mcls_RpiView.tEv_ChangeDspFrameNo
        ' �r�I�[�o�[���C
        If m_bDb And m_bDrawKizuOvl Then
            SetKizuOvl(nFrameNoStart, nFrameNoEnd)
        End If

        ' �X�e�[�^�X���G���A�X�V
        tslFrameNowCnt.Text = CStr(mcls_RpiView.GetDspAriaInf.nDspFno + 1)
        tlsMainMenu.Refresh()
    End Sub

    ''' <summary>
    ''' �}�E�X�z�C�[��
    ''' </summary>
    Private Sub frmMain_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles Me.MouseWheel ', spcMain.MouseWheel
        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseWheel(e) Then
                Return
            End If
        End If

        If mcls_ImgCutDsp.IsDsp Then Return ' �E�N���b�N���́A�g��\�����ׁ̈A�X�N���[�����Ȃ�
        Dim nVal As Integer = scrNagate.Value
        If e.Delta > 0 Then
            nVal -= scrNagate.SmallChange
        Else
            nVal += scrNagate.SmallChange
        End If

        nVal = LimitByScrollMinMax(scrNagate, nVal)

        Call scrNagate_ValueChanged(nVal, True)
    End Sub
    ''' <summary>
    ''' �c�X�N���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub vsBar_ValueChanged(ByVal sender As Object, ByVal e As Boolean) Handles vsBar.ValueChanged
        If e = True Then Return ' �v���O�������ŕύX�����ꍇ�́A������

        ' �X�N���[���ړ�
        Dim nVal As Integer = CType(sender, ScrollBar).Value
        If vsBar Is scrNagate Then
            Call scrNagate_ValueChanged(nVal, e)


            ''''' �A�g�p �Ƃ��ċ��L�������Ɉʒu������
            'Call SetMemValLen()

        Else
            Call scrHaba_ValueChanged(nVal, e)
        End If
    End Sub
    ''' <summary>
    ''' ����X�N���[��
    ''' </summary>
    ''' <param name="nVal">�ύX�l</param>
    ''' <param name="bIsProgramable">�v���O�����ŏ���ɕς����ꍇtrue</param>
    ''' <remarks></remarks>
    Private Sub scrNagate_ValueChanged(ByVal nVal As Integer, ByVal bIsProgramable As Boolean)
        'If m_skip Then
        '	Threading.Thread.Sleep(100)
        '	'Return
        'End If
        'm_skip = True

        If nVal <> mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y Then
            Call mcls_RpiView.ScrollH(nVal, bIsProgramable)     ' �X�N���[�����v���O�������́A�����ɂ��ύX
            Call mcls_RpiView.RePaint()
        End If
    End Sub

    ''' <summary>
    ''' ���X�N���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub hsBar_ValueChanged(ByVal sender As Object, ByVal e As Boolean) Handles hsBar.ValueChanged
        If e = True Then Return ' �v���O�������ŕύX�����ꍇ�́A������

        ' �X�N���[���ړ�
        Dim nVal As Integer = CType(sender, ScrollBar).Value
        If hsBar Is scrHaba Then
            Call scrHaba_ValueChanged(nVal, e)
        Else
            Call scrNagate_ValueChanged(nVal, e)
        End If
    End Sub
    ''' <summary>
    ''' ���X�N���[��
    ''' </summary>
    ''' <param name="nVal">�ύX�l</param>
    ''' <param name="bIsProgramable">�v���O�����ŏ���ɕς����ꍇtrue</param>
    ''' <remarks></remarks>
    Private Sub scrHaba_ValueChanged(ByVal nVal As Integer, ByVal bIsProgramable As Boolean)
        If nVal <> mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X Then
            Call mcls_RpiView.ScrollW(nVal, bIsProgramable)     ' �X�N���[�����v���O�������́A�����ɂ��ύX
            Call mcls_RpiView.RePaint()
        End If
    End Sub


    Enum JumpMode
        None
        AbsMM
        AbsMM_Center
        Relative
    End Enum
    ''' <summary>
    ''' �w��ʒu�ړ� �������
    ''' </summary>
    ''' <param name="mode">Y���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)</param>
    ''' <param name="len">[mm]</param>
    Private Sub JumpNagate(ByVal mode As Integer, ByVal len As Single)
        If 0 = mode Then Exit Sub

        Dim nPixel As Integer   ' pixel
        If 1 = mode Then
            '��Έړ��ł́A�I�t�Z�b�g�̉e����������̂ŁAmm�������B
            nPixel = mcls_RpiView.GetPos_LenToDp_mm(len)

        ElseIf 2 = mode Then
            '��Έړ��ł́A�I�t�Z�b�g�̉e����������̂ŁAmm�������B
            Dim nWkPix As Integer = mcls_RpiView.GetPos_LenToDp_mm(len)
            nPixel = nWkPix - CInt(picFrameImg.ClientSize.Height / 2)

        ElseIf 3 = mode Then
            '�����͏����ȋ����Ȃ̂ŁA�I�t�Z�b�g�������Ă͂����Ȃ��B
            Dim nWkPix As Integer = mcls_RpiView.GetPos_LenToDp_Y(len)
            nPixel = nWkPix + mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y
        End If


        'Dim nPixelY As Integer = mcls_RpiView.GetPos_LenToDp_Y(nPosY)
        nPixel = LimitByScrollMinMax(scrNagate, nPixel)
        scrNagate_ValueChanged(nPixel, True)
    End Sub
    ''' <summary>
    ''' �w��ʒu�ړ� ������
    ''' </summary>
    ''' <param name="mode">X���[�h (0:���� 1:�w��l(�擪) 2:�w��l(����) 3:���݂��獷��)</param> 
    ''' <param name="len">[mm]</param>
    ''' <param name="lenMode">len���ǂ����̎w�肩��H(0:���[  1:�Z���^�[����)</param>
    Private Sub JumpHaba(ByVal mode As Integer, ByVal len As Integer, ByVal lenMode As Integer)
        If 0 = mode Then Exit Sub

        If 1 = lenMode Then
            ' �Z���^�[��ŗ��Ă��邽�߁A���[��ɒ���
            len += CInt(mcls_RpiView.GetPos_RpToLen_X(mcls_RpiView.GetImg_ImgInf().nOneImgW) / 2)
        End If


        Dim nPixel As Integer   ' pixel
        Dim nWkPix As Integer = mcls_RpiView.GetPos_LenToDp_X(len)
        If 1 = mode Then
            nPixel = nWkPix

        ElseIf 2 = mode Then
            nPixel = nWkPix - CInt(picFrameImg.ClientSize.Width / 2)

        ElseIf 3 = mode Then
            nPixel = nWkPix + mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X
        End If


        nPixel = LimitByScrollMinMax(scrHaba, nPixel)
        scrHaba_ValueChanged(nPixel, True)
    End Sub

    ''' <summary>
    ''' �w��ʒu�ɃW�����v
    ''' </summary>
    ''' <param name="nMode">-1:�擪 1:���[ 0:�C�ӈʒu</param>
    ''' <param name="nVal">nMode0�̂Ƃ��̂݁B��������</param>
    ''' <remarks></remarks>
    Private Sub Map_JumpNagate(ByVal nMode As Integer, ByVal nVal As Integer)
        Dim mmJumpTo As Single = 0
        If 0 = nMode Then
            Call JumpNagate(3, nVal)
        ElseIf -1 = nMode Then
            mmJumpTo = mcls_RpiView.GetPos_DpToLen_mm(vsBar.Minimum)
            If mmJumpTo < 0 Then mmJumpTo = 0
            Call JumpNagate(1, mmJumpTo)
        ElseIf 1 = nMode Then
            Dim tHeader As tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader
            Dim pxTailpos As Integer = tHeader.nFTposCfNo(1) * mcls_RpiView.GetRpi_Header.height + tHeader.nFTposPixel(1)
            If pxTailpos <> 0 Then
                mmJumpTo = mcls_RpiView.GetPos_RpToLen_mm(pxTailpos - mcls_RpiView.GetPos_DpToRp_Y(mcls_RpiView.GetImg_AriaInf.nAriaHeight))
            Else
                mmJumpTo = mcls_RpiView.GetPos_DpToLen_Y(vsBar.MaximumVirtual)
            End If
            Call JumpNagate(1, mmJumpTo)
        End If
    End Sub



    ''' <summary>
    ''' �}�b�v�N���b�N (�\�����W�n) ���S���摜�̍��W = �Z���^�[�͉摜�̃Z���^�[
    ''' </summary>
    ''' <param name="pos">[mm]</param>
    ''' <remarks></remarks>
    Private Sub SubMapClick(ByVal pos As System.Drawing.Point) Handles mfrm_KizuMap.tEv_MapClick
        Call JumpNagate(2, pos.Y)       ' �擪����
        Call JumpHaba(2, pos.X, 1)      ' �Z���^�[�����
    End Sub

    ''' <summary>
    ''' �}�b�v�N���b�N (�r���W�n) ���Ӄւ̍��W = �Z���^�[�͔Z���^�[
    ''' </summary>
    ''' <param name="pos">[mm]</param>
    ''' <remarks></remarks>
    Private Sub SubKizuClick(ByVal pos As System.Drawing.Point)
        pos.Y += CInt((mcls_RpiView.GetRpi_THeader.nFTposCfNo(0) * mcls_RpiView.GetRpi_Header.height + mcls_RpiView.GetRpi_THeader.nFTposPixel(0)) * mcls_RpiView.GetRpi_Header.res_y)

        Call JumpNagate(2, pos.Y)       ' �擪����
        Call JumpHaba(2, pos.X, 1)      ' �Z���^�[�����
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ������Ƃ����C�x���g


    ''' <summary>
    ''' �t���[�����Ǎ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudFrameNo_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudFrameNo.ValueChanged
        If Not mcls_RpiView.IsRpi Then Return

        ' ����
        Dim fno As Integer = CInt(nudFrameNo.Value)
        Dim head As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header
        Dim inf As tClass.tClass_ReadRPI.RPI_FRAME_INF = mcls_RpiView.GetRpi_FrameInf(fno)

        lblImgPos.Text = inf.img_pos.ToString
        lblImgSize.Text = inf.img_size.ToString
        lblInfPos.Text = inf.inf_pos.ToString

        ' �G�b�W���
        If head.frame_info_type = tClass.tClass_ReadRPI.EM_FRM_TYPE.EDGE_PER_FRAME Then
            Dim typ As tClass.tClass_ReadRPI.EDGE_PER_FRAME = mcls_RpiView.ReadFile_FrameInfo_EdgePerFrame(fno)
            lblEdge.Text = String.Format("{0}, {1}", typ.edge_left, typ.edge_right)
        Else
            lblEdge.Text = ""
        End If
    End Sub


    ''' <summary>
    ''' RPI���G���A �\����\���ؑ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tsbInfMode_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbInfMode.Click
        spcMain.Panel1Collapsed = Not spcMain.Panel1Collapsed
    End Sub



    Dim m_frmPopRpiPath As New frmPopRpiPath

    ''' <summary>
    ''' �t�H���_�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub tsbFolder_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbFolder.Click
        m_frmPopRpiPath.ShowToggle(Me)
    End Sub


    ''' <summary>
    ''' �I�[�o�[���C
    ''' </summary>
    Private Sub tsbOVL_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbOVL.Click
        m_bDrawKizuOvl = Not m_bDrawKizuOvl

        If m_bDb And m_bDrawKizuOvl Then
            SetKizuOvl(mcls_RpiView.GetDspAriaInf.nDspFno + 1, mcls_RpiView.GetDspAriaInf.nDspFnoEnd + 1)
        Else
            mcls_RpiView.ResetOverLayInf()
        End If
        Call RePaint()
    End Sub
    ''' <summary>
    ''' �G�b�W�ʒu�`��
    ''' </summary>
    Private Sub tsbEdge_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbEdge.Click
        m_bDrawEdge = Not m_bDrawEdge
        Call RePaint()
    End Sub






    ''' <summary>
    ''' �{���ύX
    ''' </summary>
    Private Sub tscRate_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles tscRate.SelectedIndexChanged
        Dim dRate As Double = 1.0

        '''' ���ݒ��S�ʒu���o���Ă���
        Dim nX As Integer = GetPos(0, 0.5)  ' [mm]
        Dim nY As Integer = GetPos(1, 0.5)  ' [mm]


        '''' �\���{�� ����
        Select Case tscRate.SelectedIndex
            Case 0          ' ���S��
                dRate = 1
            Case 1          ' ���摜1/4
                dRate = 0.25
            Case 2          ' ���摜1/2
                dRate = 0.5
            Case 3          ' ���摜1�{
                dRate = 1
            Case 4          ' 2�{
                dRate = 2
            Case 5          ' 3�{
                dRate = 3
            Case 6          ' 4�{
                dRate = 4
            Case 7          ' 5�{
                dRate = 5
            Case Else
        End Select

        ' �摜�\�����[�h
        If tscRate.SelectedIndex = 0 Then       ' �\���G���A�
            Call mcls_RpiView.SetProp_ImgKind_StrechType(tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth)
        Else                                    ' �摜�T�C�Y�
            Call mcls_RpiView.SetProp_ImgKind_StrechType(tClass.tClass_ImageDsp_Calc.EM_IMG_STRECH.emNormal)
        End If

        ' �Y�[���ݒ�
        Call mcls_RpiView.SetProp_Zoom(dRate)

        ' �C���[�W�𑦎���蒼��
        Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)
        Call ReCreateImg()


        '''' �����g��\���N���X�ɂ��\���{�����ύX���ꂽ����ʒm
        'Call mcls_ImgCutDsp.SetItemRes(mcls_RpiView.GetImg_CalcAns.FinalRes.dW, mcls_RpiView.GetImg_CalcAns.FinalRes.dH)		' ���݂̔{�����Z�b�g


        '''' ���̒��S�ʒu����Ɉړ�
        Call JumpNagate(2, nY)
        Call JumpHaba(2, nX, 0)

    End Sub

    ''' <summary>
    ''' �`�����l���؂�ւ�
    ''' </summary>
    Private Sub tscChannel_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles tscChannel.SelectedIndexChanged
        If mcls_RpiView.GetRpi_Cls.PropRpiActiveCh = tscChannel.SelectedIndex Then Return

        mcls_RpiView.GetRpi_Cls.PropRpiActiveCh = tscChannel.SelectedIndex  '�`�����l���؂�ւ�

        ' �C���[�W�𑦎���蒼��
        Call ReCreateImg()
    End Sub


    ''' <summary>
    ''' �J���p
    ''' </summary>
    Private Sub tsbImgQuarty_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbImgQuarty.Click
        mfrm_Kai.ShowToggle(Me)
    End Sub

    ''' <summary>
    ''' FT ���]
    ''' </summary>
    Private Sub tsbTB_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbTB.Click
        mcls_RpiView.SetProp_TB(IIf(tsbTB.Checked, 1, 0))

        'Call mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect)	' ���]�ݒ�
        Call ReverceDsp()
        Call ReCreateImg()
    End Sub
    ''' <summary>
    ''' DS WS���]
    ''' </summary>
    Private Sub tsbDSWS_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDSWS.Click
        mcls_RpiView.SetProp_DSWS(IIf(tsbDSWS.Checked, 1, 0))

        'Call mcls_Param.ReadZi_DefectInfReverce(mcls_RpiView, m_lstDetect)	' ���]�ݒ�
        Call ReverceDsp()
        Call ReCreateImg()
    End Sub
    ''' <summary>
    ''' ��]�̕ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tsbRot_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbRot.Click
        Call SetRotate(Not mcls_RpiView.PropRotate90)
    End Sub
    Private Sub SetRotate(ByVal bUmu As Boolean)

        '�`�摤�ɂ��`�B
        mcls_RpiView.PropRotate90 = bUmu

        '��]�����̂ŁA�o����E���p���Ɓo�c�E���p�̃X�N���[���o�[�̑Ή����Ƃ�Ȃ���
        If mcls_RpiView.PropRotate90 Then
            scrHaba = Me.vsBar
            scrNagate = Me.hsBar
        Else
            scrHaba = Me.hsBar
            scrNagate = Me.vsBar
        End If

        '�X�N���[���ʂ��ω�����B
        scrNagate.Maximum = 500         ' LargeChange�ȏ�̒l�����Ă����Ȃ��ƁA�����LargeChange�����������Ă��܂�
        scrNagate.SmallChange = 100
        scrNagate.LargeChange = 500

        scrHaba.Maximum = 500         ' LargeChange�ȏ�̒l�����Ă����Ȃ��ƁA�����LargeChange�����������Ă��܂�
        scrHaba.SmallChange = 1
        scrHaba.LargeChange = 1

        '�X�N���[���o�[����蒼��
        Call mcls_RpiView.ResizePaintAria(Me, Nothing)

        '�`��p�C���[�W�̍�蒼�����ĕ`��B
        Call ReCreateImg()
    End Sub


    ''' <summary>
    ''' ���E���]
    ''' </summary>
    ''' <param name="nMode">
    ''' -9:���\���ɑ΂��Ĕ��] 
    ''' -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 
    ''' 1:���摜�ɑ΂��Ĕ��] 
    ''' 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��]
    ''' </param>
    ''' <remarks></remarks>
    Private Sub TurnDSWS(ByVal nMode As Integer)
        If 0 = nMode Then Return
        Dim bTurn As Boolean = False        ' �ϊ�����

        '''' �ϊ�����
        ' �����I�ɔ��]
        If -9 = nMode Then
            bTurn = Not tsbDSWS.Checked
        End If
        ' �����I�ɔ��]���� = ��Δ��]����
        If -1 = nMode Then
            bTurn = False
        End If
        ' ���Ŕ��� = ��Δ��]
        If 1 = nMode Then
            bTurn = True
        End If
        ' Ini�ݒ�
        If 2 = nMode Then
            Dim nIni As Integer = tMod.tMod_IniFile.ReadIniInt("DB_" & mcls_RpiView.GetRpi_THeader.cSysName, "DorW", TASKINI_NAME)
            bTurn = Not CBool(nIni)
        End If


        '''' ���ʔ��f
        If tsbDSWS.Checked = bTurn Then Return
        tsbDSWS.Checked = bTurn
        tsbDSWS_Click(Nothing, Nothing)
    End Sub

    ''' <summary>
    ''' �㉺���]
    ''' </summary>
    ''' <param name="nMode">
    ''' -9:���\���ɑ΂��Ĕ��] 
    ''' -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 
    ''' 1:���摜�ɑ΂��Ĕ��] 
    ''' 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��]
    ''' </param>
    ''' <remarks></remarks>
    Private Sub TurnTB(ByVal nMode As Integer)
        If 0 = nMode Then Return
        Dim bTurn As Boolean = False        ' �ϊ�����

        '''' �ϊ�����
        ' �����I�ɔ��]
        If -9 = nMode Then
            bTurn = Not tsbTB.Checked
        End If
        ' �����I�ɔ��]���� = ��Δ��]����
        If -1 = nMode Then
            bTurn = False
        End If
        ' ���Ŕ��� = ��Δ��]
        If 1 = nMode Then
            bTurn = True
        End If
        ' Ini�ݒ�
        If 2 = nMode Then
            Dim nIni As Integer = tMod.tMod_IniFile.ReadIniInt("DB_" & mcls_RpiView.GetRpi_THeader.cSysName, "TorB", TASKINI_NAME)
            bTurn = Not CBool(nIni)
        End If


        '''' ���ʔ��f
        If tsbTB.Checked = bTurn Then Return
        tsbTB.Checked = bTurn
        tsbTB_Click(Nothing, Nothing)
    End Sub

    ''' <summary>
    ''' 90�x���]
    ''' </summary>
    ''' <param name="nMode">
    ''' -9:���\���ɑ΂��Ĕ��] 
    ''' -1:���摜�ɑ΂��Ĕ��]����(�v��ini�t�@�C���Ŕ��]����������߂�) 
    ''' 1:���摜�ɑ΂��Ĕ��] 
    ''' 2:ini�t�@�C���ݒ��ɑ΂��Ĕ��]
    ''' </param>
    ''' <remarks></remarks>
    Private Sub TurnRotate(ByVal nMode As Integer)
        If 0 = nMode Then Return
        Dim bTurn As Boolean = False        ' �ϊ�����

        '''' �ϊ�����
        ' �����I�ɔ��]
        If -9 = nMode Then
            bTurn = Not tsbRot.Checked
        End If
        ' �����I�ɔ��]���� = ��Δ��]����
        If -1 = nMode Then
            bTurn = False
        End If
        ' ���Ŕ��� = ��Δ��]
        If 1 = nMode Then
            bTurn = True
        End If
        ' Ini�ݒ�
        If 2 = nMode Then
            Dim nIni As Integer = tMod.tMod_IniFile.ReadIniInt("DB_" & mcls_RpiView.GetRpi_THeader.cSysName, "Rotate", TASKINI_NAME)
            bTurn = Not CBool(nIni)
        End If


        '''' ���ʔ��f
        If tsbRot.Checked = bTurn Then Return
        tsbRot.Checked = bTurn
        tsbRot_Click(Nothing, Nothing)
    End Sub


    ''' <summary>
    ''' ���������L�����Z��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub AutoDownCancel()
        ' ���C�����x��0�̂Ƃ������́A�L�����Z������
        mcls_CheckKizu.AutoStopCancel()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\���摜 �}�E�X����֌W

    Dim m_bIsImgDrag As Boolean = False             ' �摜�����N���b�N�ł���ł���Œ��H
    Dim m_PointS As Point                           ' �J�n�����}�E�X�n�_

    ''' <summary>
    ''' �}�E�X�J�[�\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picFrameImg_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picFrameImg.MouseMove
        Dim CalcAns As tClass.tClass_ImageDsp.CALC_ANS = mcls_RpiView.GetImg_CalcAns()
        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf

        Dim nowPos As Point = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)      ' ���݂̃J�[�\���ʒu

        ' �擪����̌��摜�nPixel
        Dim pixel As Point
        pixel.X = mcls_RpiView.GetPos_DpToRp_X(DspInf.DspPixelTotalOffset.X + nowPos.X)  'CInt((DspInf.DspPixelTotalOffset.X / CalcAns.RateDsp.dW) + (nowPos.X / CalcAns.RateDsp.dW))
        pixel.Y = mcls_RpiView.GetPos_DpToRp_Y(DspInf.DspPixelTotalOffset.Y + nowPos.Y) 'CInt((DspInf.DspPixelTotalOffset.Y / CalcAns.RateDsp.dH) + (nowPos.Y / CalcAns.RateDsp.dH))
        tslTotalPixel.Text = String.Format("{0:#,##0}/ {1:#,##0}", pixel.X, pixel.Y)

        ' �\���摜��m
        Dim pixelF As PointF
        pixelF.X = mcls_RpiView.GetPos_DpToLen_X(DspInf.DspPixelTotalOffset.X + nowPos.X) 'CSng(DspInf.DspPixelTotalOffset.X * CalcAns.FinalRes.dW) + CSng(nowPos.X * CalcAns.FinalRes.dW)
        pixelF.Y = mcls_RpiView.GetPos_DpToLen_mm(DspInf.DspPixelTotalOffset.Y + nowPos.Y) 'CSng(DspInf.DspPixelTotalOffset.Y * CalcAns.FinalRes.dH) + CSng(nowPos.Y * CalcAns.FinalRes.dH)
        tslTotalMm.Text = String.Format("{0:#,##0.000}/ {1:#,##0.000}", pixelF.X / 1000, pixelF.Y / 1000)

        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseMove(e) Then
                Return
            End If
        End If



        '''' �摜�ړ�
        If m_bIsImgDrag Then
            Dim PointE As Point = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)
            Dim p As Point

            If hsBar.Enabled Then
                p.X = LimitByScrollMinMax(hsBar, (m_PointS.X - PointE.X) + hsBar.Value)
                If Not hsBar.Value = p.X Then   ' ��
                    If hsBar Is scrHaba Then
                        Call mcls_RpiView.ScrollW(p.X, True)
                    Else
                        Call mcls_RpiView.ScrollH(p.X, True)
                    End If
                End If
            End If

            If vsBar.Enabled Then
                p.Y = LimitByScrollMinMax(vsBar, (m_PointS.Y - PointE.Y) + vsBar.Value)

                If Not vsBar.Value = p.Y Then   ' ��
                    If vsBar Is scrNagate Then
                        Call mcls_RpiView.ScrollH(p.Y, True)
                    Else
                        Call mcls_RpiView.ScrollW(p.Y, True)
                    End If
                End If
            End If

            ' ���ݒl���ăZ�b�g
            m_PointS = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)

            ' �S���I�������ĕ`��
            Call RePaint()
        End If
    End Sub


    ''' <summary>
    ''' �}�E�X�_�E��
    ''' </summary>
    Private Sub picFrameImg_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picFrameImg.MouseDown
        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseDown(e) Then
                Return
            End If
        End If

        If e.Button = Windows.Forms.MouseButtons.Left Then
            m_bIsImgDrag = True
            m_PointS = picFrameImg.PointToClient(Windows.Forms.Cursor.Position)
        End If
    End Sub


    ''' <summary>
    ''' �}�X�E�A�b�v
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picFrameImg_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles picFrameImg.MouseUp
        Dim evDetail As RpiPlugin.IMouseInputDetail = TryCast(m_active_mouse_handler, RpiPlugin.IMouseInputDetail)
        If evDetail IsNot Nothing Then
            If evDetail.OnMouseUp(e) Then
                Return
            End If
        End If

        If e.Button = Windows.Forms.MouseButtons.Left Then
            m_bIsImgDrag = False
        End If

        ' �{�^���Ƃ��Ƀt�H�[�J�X�𓖂ĂĂ����Ȃ��ƁA
        btnRituNon.Select()
    End Sub

    ''' <summary>
    ''' �����X�N���[��(�ĕ`��) �� �x���`��Ή�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timDsp_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles timDsp.Tick
        mcls_StopWatch.CounterCycle()


        If mcls_RpiView.IsRpi() Then
            Dim bMyMove As Boolean = True           ' �����Œʔ�true�B�O���A����false
            Dim nSpeed As Integer = 0               ' ����̃��C�����x
            'If IsNumeric(tscSpeed.Text) Then nSpeed = CInt(tscSpeed.Text) 
            nSpeed = mfrm_FrmCont.LineSpeed

            Dim nIndex As Integer = scrNagate.Value ' �O��̋��� Y[DP]
            Dim nNewIndex As Integer                ' ����̐i�ݗ� Y[DP]
            Dim nNewIndexX As Integer               ' ����̐i�ݗ� X[DP]

            Dim bMyTaskActive As Boolean = Form.ActiveForm IsNot Nothing        ' ���^�X�N���̂��A�N�e�B�u�� true


            ' ================================================
            ' �A�g�p
            '''' ���H����ԂɒǏ] 
            If Not m_memTrkPtr.Equals(IntPtr.Zero) AndAlso Not bMyTaskActive Then
                bMyMove = False

                '' ���L�������Ǎ�
                Dim tMemVal As TYPE_TRK_ITEM                        ' ���H���̏��\����
                Dim nMemOffset As Integer = SIZE_TYPE_HEAD + SIZE_TYPE_ITEM * g_typStartArgs.nId        ' ���\���̈ʒu
                tClass_GlovalMemory.GetMappedT(m_memTrkPtr, nMemOffset, tMemVal)

                ' �O��Ɠ��������Ȃ�
                'If tMemVal.nY = Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) And _
                'tMemVal.nX = Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) Then
                If 1 >= Math.Abs(tMemVal.nY - Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y))) And _
                1 >= Math.Abs(tMemVal.nX - Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X))) Then
                    nSpeed = 0
                Else
                    nNewIndex = mcls_RpiView.GetPos_LenToDp_Y(tMemVal.nY)
                    nNewIndex = LimitByScrollMinMax(scrNagate, nNewIndex)

                    nNewIndexX = mcls_RpiView.GetPos_LenToDp_X(tMemVal.nX)
                    nNewIndexX = LimitByScrollMinMax(scrHaba, nNewIndexX)

                    nSpeed = 1 ' �A�����ē������悤�ɁA0�ȊO���Z�b�g
                End If
            End If

            '''' �Ȉ՘A���ɒǏ] 
            If Not m_memSyncPtr.Equals(IntPtr.Zero) AndAlso Not bMyTaskActive Then
                bMyMove = False

                Dim tMemVal As TYPE_SYNC                        ' ���H���̏��\����
                tClass_GlovalMemory.GetMappedT(m_memSyncPtr, 0, tMemVal)

                ' �O��Ɠ��������Ȃ�
                'If tMemVal.nY = Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - m_nSyncOffsetY And _
                'tMemVal.nX = Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - m_nSyncOffsetX Then
                If 1 >= Math.Abs(tMemVal.nY - (Int(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - m_nSyncOffsetY)) And _
                1 >= Math.Abs(tMemVal.nX - (Int(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - m_nSyncOffsetX)) Then
                    nSpeed = 0
                Else
                    nNewIndex = mcls_RpiView.GetPos_LenToDp_Y(tMemVal.nY + m_nSyncOffsetY)  ' �I�t�Z�b�g�������Z
                    nNewIndex = LimitByScrollMinMax(scrNagate, nNewIndex)

                    nNewIndexX = mcls_RpiView.GetPos_LenToDp_X(tMemVal.nX + m_nSyncOffsetX)  ' �I�t�Z�b�g�������Z
                    nNewIndexX = LimitByScrollMinMax(scrHaba, nNewIndexX)

                    nSpeed = 1 ' �A�����ē������悤�ɁA0�ȊO���Z�b�g
                End If
            End If


            'System.Console.WriteLine("{0}  {1}", bMyTaskActive, nNewIndex)


            ' ================================================
            ' ���ʂ̏���
            '' ����̐i�ݗʎZ�o
            If bMyMove Then     ' �A�g�������A���A�N�e�B�u���̂�
                Dim nwk As Integer
                Dim dwk As Double

                dwk = nSpeed / 60.0         '[mm/msec]
                dwk = dwk * mcls_StopWatch.GetCycleTime()   ' �^�C�}�[���ԕӂ� ���ꂾ���i��
                nwk = mcls_RpiView.GetPos_LenToDp_Y(dwk)   'dwk / mcls_RpiView.GetImg_CalcAns().FinalRes.dH		' �\�����W�n��pixel
                If 0 = nwk AndAlso 0 < nSpeed Then nwk = 1
                If 0 = nwk AndAlso 0 > nSpeed Then nwk = -1
                nNewIndex = LimitByScrollMinMax(scrNagate, nIndex + nwk)
                'If nNewIndex <> nIndex + nwk Then
                '	nNewIndex = scrNagate.Minimum
                'End If
            End If


            ' ================================================
            ' �ړ�
            If nSpeed = 0 Then
                If Not mcls_RpiView.GetDspAriaInf_DspAllOk() Then
                    'Call mcls_RpiView.ScrollH(nIndex, True)
                    Call mcls_RpiView.ReSetImg(mcls_RpiView.GetDspAriaInf.nDspFno)
                    Call RePaint()
                End If
            Else
                Call mcls_RpiView.ScrollH(nNewIndex, True)
                If Not bMyMove Then mcls_RpiView.ScrollW(nNewIndexX, True) ' �A���̂Ƃ��̂�

                Call RePaint()
            End If



            ' ================================================
            ' �G���h����
            '''' �r�}�b�v�ύX (��ʒ����ʒu��n��)
            Dim nCenter As Integer = CInt(nIndex + (picFrameImg.ClientSize.Height / 2)) + 1  ' +1�́A������19.9999�ɂȂ�΍�B
            Call mfrm_KizuMap.DspJumpRow(CInt(mcls_RpiView.GetPos_DpToLen_Y(nCenter)))

            '' �\���t���[�����[�g
            tslFPS.Text = String.Format("{0:0.0}", (1000.0 / mcls_StopWatch.GetAveTime()))



            ' ==========================================
            ' �r�ڋ߃����v�`��
            Dim nRanpGr() As Integer = mcls_CheckKizu.RanpStat()    ' �r�ڋ߃����v��gid���擾
            For ii As Integer = 0 To mcnt_Ranp.Length - 1
                If 0 = nRanpGr(ii) Then
                    mcnt_Ranp(ii).BackColor = SystemColors.ControlDark
                Else
                    Dim gr As clsParamManager.PARAM_GRADE = mcls_Param.GetParamGrade(nRanpGr(ii))
                    mcnt_Ranp(ii).BackColor = gr.clrDspColor
                End If
            Next


            ''''' �A�g�p �Ƃ��ċ��L�������Ɉʒu������
            Call SetMem_ValLen()
        End If
    End Sub

    ''' <summary>
    ''' �������葬�x
    ''' </summary>
    Private Sub tscSpeed_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles tscSpeed.TextChanged
        mfrm_FrmCont.LineSpeed = CInt(tscSpeed.Text)

        ' ���C�����x0mpm�̎��ɂ́A��U�t�H�[�J�X���͂����B
        If 0 = CInt(tscSpeed.Text) Then
            btnRituNon.Select()
        End If
    End Sub

    ''' <summary>
    ''' ���L�������ʒu������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetMem_ValLen()
        If Not m_memTrkPtr.Equals(IntPtr.Zero) Then
            Call SetMemTrk_ValLen()
        End If
        If Not m_memSyncPtr.Equals(IntPtr.Zero) Then
            Call SetMemSync_ValLen()
        End If
    End Sub



    ''' <summary>
    ''' �㉺���`�F�b�N
    ''' </summary>
    ''' <param name="scr"></param>
    ''' <param name="val"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Shared Function LimitByScrollMinMax(ByVal scr As ScrollBar, ByVal val As Integer) As Integer
        If val > (scr.Maximum - scr.LargeChange + 1) Then Return scr.Maximum - scr.LargeChange + 1
        If val < scr.Minimum Then Return scr.Minimum
        Return val
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �r�I�[�o�[���C

    ''' <summary>
    ''' �r�I�[�o�[���C���Z�b�g
    ''' </summary>
    ''' <param name="nDspFrameNoS">�J�n�J�n�t���[��No(1�I���W��)</param>
    ''' <param name="nDspFrameNoE">�ΏۏI���t���[��No(1�I���W��)</param>
    ''' <remarks></remarks>
    Private Sub SetKizuOvl(ByVal nDspFrameNoS As Integer, ByVal nDspFrameNoE As Integer)
        '''' �O���񏉊���
        mcls_RpiView.ResetOverLayInf()

        '''' �V�����I�[�o�[���C���
        Dim oly As tClass.tClass_ImageDsp.OVERLAY_INF = Nothing
        oly.bIsDsp = m_bDrawKizuOvl
        oly.LineWidth = 1
        oly.HoseiX = 5
        oly.HoseiY = 5

        For Each data As DATA_DETECT_INF In m_lstDetect
            ' ����̕\���ΏہH
            If Not data.bDsp Then Continue For
            If nDspFrameNoS > data.nFrameNo Or nDspFrameNoE < data.nFrameNo Then Continue For
            Dim gr As clsParamManager.PARAM_GRADE = mcls_Param.GetParamGrade(data.gId)


            ' �g
            oly.Tag = data
            oly.ovlPos1 = New Point(data.nFrameXmin + mcls_RpiView.GetRpi_THeader.caminfo(data.nCamPos).nBaseLeft, _
               data.nFrameYmin + (data.nFrameNo - nDspFrameNoS) * mcls_RpiView.GetRpi_Header.height)
            oly.ovlPos2 = New Point(data.nFrameXmax + mcls_RpiView.GetRpi_THeader.caminfo(data.nCamPos).nBaseLeft, _
             data.nFrameYmax + (data.nFrameNo - nDspFrameNoS) * mcls_RpiView.GetRpi_Header.height)
            oly.Color = gr.clrDspColor



            ' �r��
            If mfrm_DspSet.g_bTypeDsp Then
                Dim ty As clsParamManager.PARAM_TYPE = mcls_Param.GetParamType(data.tId)
                oly.bIsDspType = True
                oly.cBsh = New SolidBrush(oly.Color)
                oly.cFnt = New System.Drawing.Font("�l�r �S�V�b�N", mfrm_DspSet.g_nTypeFSize, System.Drawing.FontStyle.Regular, GraphicsUnit.Pixel)
                oly.sTypeName = ty.sSName
            End If


            ' �N���X�Ƀf�[�^�Z�b�g
            mcls_RpiView.SetOverLayInf(oly)
        Next

    End Sub

    Private Sub tcls_tEv_OverLay_Click(ByVal sender As tClass_ImageDsp.OVERLAY_INF, ByVal e As System.Drawing.Rectangle) Handles mcls_RpiView.tEv_OverLay_Click
        Dim sWk As String
        '''' �������蒆�ł���΁A�I�[�o�[���C�N���b�N�͖�����
        'If mfrm_DspLen.Visible Then Return
        '''' ���ዾ���ł���΁A����
        If mcls_ImgCutDsp.IsDsp Then Return


        Dim sSession As String = "DB_" & mcls_RpiView.GetRpi_THeader.cSysName
        Dim nImgCnt As Integer = tMod.tMod_IniFile.ReadIniInt(sSession, "SELECT_IMG_CNT", TASKINI_NAME, 2)   '���w��̏ꍇ�́A�����Q�摜
        If 0 = nImgCnt Then Return


        '''' �؂�o���摜�|�b�v�A�b�v�\������
        Dim kiridata As DETECT_KIRI_DATA = Nothing
        kiridata.initialize()
        kiridata.data = CType(sender.Tag, DATA_DETECT_INF)


        '''' �摜�f�[�^�擾
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, sSession)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        kiridata.nImgCnt = nImgCnt  '���w��̏ꍇ�́A�����Q�摜


        Try

            '' DB�ڑ�
            If Not tcls_DB.Open() Then
                MsgBox("DB�Ɛڑ����s�B�r�I�[�o�[���C�̕\���͂ł��܂���B")
                Return
            End If


            '' SQL����
            sWk = "SELECT_IMG_" & mcls_RpiView.GetRpi_THeader.nMen.ToString
            Dim sSQL As String = tMod.tMod_IniFile.ReadIniStr(sSession, sWk, TASKINI_NAME)
            sWk = String.Format(sSQL, mcls_RpiView.GetRpi_THeader.cKizukenNo, kiridata.data.nNo)

            sqlRead = tcls_DB.SelectExecute(sWk)
            If sqlRead Is Nothing Then Return
            'sqlRead.Read()
            If Not sqlRead.Read() Then      ' Ver443
                kiridata.nImgCnt = 0
                Exit Try
            End If

            '' �摜�\��
            kiridata.dResY = sqlRead.GetFloat(0)
            kiridata.dResX = sqlRead.GetFloat(1)
            kiridata.nImgSizeY = sqlRead.GetInt32(2)
            kiridata.nImgSizeX = sqlRead.GetInt32(3)
            kiridata.nImgCenterX = sqlRead.GetInt32(4)
            kiridata.nImgCenterY = sqlRead.GetInt32(5)

            Dim nIndex As Integer = 6
            For ii As Integer = 0 To kiridata.nImgCnt - 1
                kiridata.nImgSize(ii) = sqlRead.GetInt32(nIndex)
                nIndex += 1
            Next
            For ii As Integer = 0 To kiridata.nImgCnt - 1
                If 0 <> kiridata.nImgSize(ii) Then kiridata.imgAngle(ii) = tClass.tClass_BmpJpeg.DataToImage(CType(sqlRead.GetValue(nIndex), Byte()))
                nIndex += 1
            Next
        Catch ex As Exception
            'Return
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try



        '''' �؂�o���摜�\����ʐ���
        Call mfrm_Kiri.Set_DbSession(sSession)
        Call mfrm_Kiri.SetProp_ImgKinf(Me.mcls_RpiView.GetImg_ImgKind)

        Call mfrm_Kiri.SetItem_DetectKiriData(kiridata)
        Call mfrm_Kiri.Show(Me)
        Call mfrm_Kiri.DspUpDate()
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����g��
    ''' <summary>
    ''' �g��\���G���A���`�悳�ꂽ
    ''' </summary>
    ''' <param name="tCls_Calc">�v�Z�N���X</param>
    ''' <param name="img">�摜�z��(X�����̘A���͖��Ή�)</param>
    Private Sub mcls_ImgCutDsp_tEv_DispDataSet(ByRef tCls_Calc As tClassLibrary.tClass.tClass_ImageDsp_Calc, ByRef img() As System.Drawing.Image) Handles mcls_ImgCutDsp.tEv_DispDataSet
        Dim DspAriaInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf

        '' �f�[�^�������Z�b�g
        tCls_Calc = mcls_RpiView.GetCls_ImgDsp()
        ReDim img(DspAriaInf.nDspFrameCnt - 1)
        For ii As Integer = 0 To img.Length - 1
            Dim imginf As tClass_ViewRpiThread.CLS_IMG_INF = mcls_RpiView.GetCls_Img_inf(ii + DspAriaInf.nDspFno + 1)

            If imginf Is Nothing Then Continue For
            img(ii) = imginf.pImgRaw
        Next
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �U�z�}
    ''' <summary>
    ''' �U�z�}
    ''' </summary>
    Private Sub tsbDefMap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDefMap.Click
        If Not mfrm_DefMap.Visible Then
            If Not m_bResutl Then Return

            Call mfrm_DefMap.Show(Me)
            Call mfrm_DefMap.MapDisp()
        Else
            mfrm_DefMap.Visible = False
        End If
        'Me.Select()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �r�}�b�v
    ''' <summary>
    ''' �r�}�b�v
    ''' </summary>
    Private Sub tsbKizuMap_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbKizuMap.Click
        If Not mfrm_KizuMap.Visible Then
            If Not m_bResutl Then Return

            Call mfrm_KizuMap.Show(Me)
            Call mfrm_KizuMap.MapDisp()
        Else
            mfrm_KizuMap.Visible = False
        End If
        'Me.Select()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �r�ꗗ
    ''' <summary>
    ''' �r�ꗗ
    ''' </summary>
    Private Sub tsbKizuList_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbKizuList.Click
        If Not mfrm_KizuList.Visible Then
            If Not m_bResutl Then Return

            Call mfrm_KizuList.Show(Me)
            Call mfrm_KizuList.MapDisp()
        Else
            mfrm_KizuList.Visible = False
        End If
        'Me.Select()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���b�h��
    Private Sub tsbGridLine_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbGridLine.Click
        mfrm_GridLine.ShowToggle(Me)

        Me.Select()
        ' �S���I�������ĕ`��
        Call RePaint()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �W�����v���
    Private Sub tsbJump_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbJump.Click
        Call mfrm_FrmCont.ShowToggle(Me)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �C�Ӑ؂�o��

    Private Sub tsbKiriInput_Click(sender As System.Object, e As System.EventArgs) Handles tsbKiriInput.Click
        Call mfrm_KiriInput.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �u�b�N�}�[�N
    Private Sub tsbBookmark_Click(sender As System.Object, e As System.EventArgs) Handles tsbBookmark.Click
        mfrm_Bookmark.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���͈ꗗ
    Private Sub tsbInputList_Click(sender As System.Object, e As System.EventArgs) Handles tsbInputList.Click
        Call mfrm_InputList.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �R�C�����ꗗ

    ''' <summary>
    ''' �R�C�����ꗗ
    ''' </summary>
    Private Sub tsbCoilItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbCoilItem.Click
        If mfrm_CoilItem Is Nothing OrElse Not mfrm_CoilItem.Visible Then
            If Not m_bResutl Then Return

            mfrm_CoilItem = New frmPopCoilItem
            Call mfrm_CoilItem.SetCls_RpiView(mcls_RpiView)
            mfrm_CoilItem.Show(Me)
        Else
            mfrm_CoilItem.Close()
            mfrm_CoilItem = Nothing
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\���ݒ�

    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    Private Sub tsbDspSet_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDspSet.Click
        Call mfrm_DspSet.ShowToggle(Me)
    End Sub

    ''' <summary>
    ''' �I�[�o�[���C�蓮�␳�ݒ�
    ''' </summary>
    Private Sub tsbReSetOvl_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbReSetOVL.Click
        Call mfrm_ReSetOvl.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �e��ݒ���
    ''' <summary>
    ''' �e��ݒ��ʕ\��
    ''' </summary>
    Private Sub tsbSetting_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSetting.Click
        Call mfrm_Setting.ShowToggle(Me)
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �R���g���[���ݒ�
    Private Sub AutoDownUmu()
        mcls_CheckKizu.Enable = mfrm_FrmCont.AutoDownUmu
    End Sub






    ''' <summary>
    ''' �c�[���{�^���̃N���b�N�B��ʂ̃}�E�X���͂��Ƃ�A��
    ''' </summary>
    Private Sub tsb_MouseR_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbDspBig.Click
        '�A�N�e�B�u�ɂ���{�^��
        Dim fobj As RpiPlugin.IMouseToolStrip = Nothing
        Dim bFoundHandler As Boolean = False

        For Each tsv As KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip) In mev_selectors
            If tsv.Key Is sender Then
                '����i�ǂ̃v���O�C���j�̃{�^���������ꂽ�����������B

                If Not tsv.Key.Checked Then
                    '���łɃA�N�e�B�u�ȃ{�^�����܂�������
                    '�������B�itsbBig�̃A�N�e�B�u���ł��悢�����j
                    tsv.Key.Checked = True
                    Exit Sub
                End If

                If tsv.Value IsNot Nothing Then
                    '�{�^���ɑΉ�����N���X���݂�����
                    If tsv.Value.Activate(True) Then
                        fobj = tsv.Value
                        bFoundHandler = True
                    End If

                    '�A�N�e�B�u�������ۂ��ꂽ
                    Exit For
                Else
                    bFoundHandler = True
                End If
            End If
        Next

        If Not bFoundHandler Then
            Me.Select()
            Exit Sub
        End If

        '�`�F�b�N�{�b�N�X�����W�I�{�^���炵������B
        For Each tsv As KeyValuePair(Of ToolStripButton, RpiPlugin.IMouseToolStrip) In mev_selectors
            If tsv.Key IsNot sender Then
                tsv.Key.Checked = False
            End If
        Next

        '���܂܂�Active�������N���X�ɁA�����ʒm
        If fobj IsNot m_active_mouse_handler Then
            If m_active_mouse_handler IsNot Nothing Then
                m_active_mouse_handler.Activate(False)
            End If
        End If

        mcls_ImgCutDsp.DspStop = (fobj IsNot Nothing)
        m_active_mouse_handler = fobj

        '�t�H�[���ɃL�[�t�H�[�J�X�������Ă���
        Me.Select()

        ' �S���I�������ĕ`��
        'Call RePaint()
    End Sub





    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �R�C������


    ''' <summary>
    ''' �R�C��������ʕ\��
    ''' </summary>
    Private Sub tsbSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearch.Click
        Call mfrm_Search.Show(Nothing)
        mfrm_Search.Activate()
    End Sub
    ''' <summary>
    ''' �R�C�����X�g��̃R�C���ɐؑ�
    ''' </summary>
    Private Sub tsbSearchUp_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearchUp.Click
        Call mfrm_Search.SelectChangeCoil(-1)   ' ��͎�O�Ȃ̂ŁA�| ����
    End Sub
    Private Sub tsbSearchDown_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearchDown.Click
        Call mfrm_Search.SelectChangeCoil(1)
    End Sub
    ''' <summary>
    ''' �\�����]
    ''' </summary>
    Private Sub tsbSearchOU_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSearchOU.Click
        'If Not mcls_RpiView.IsRpi Then Return

        Call mfrm_Search.SelectChangeMen()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �o�͐ݒ�


    ''' <summary>
    ''' �o��
    ''' </summary>
    Private Sub tsbPrint_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbPrint.Click
        Call mfrm_Print.ShowToggle(Me)
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �A�g�p

    ''' <summary>
    ''' ���L�������ɏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetMemTrk_ValLen()
        '''' ��Exe���A�N�e�B�u�̏ꍇ�̂݁A���ʏ�����
        ' �w�b�_�[��
        If Not Nothing Is Form.ActiveForm Then
            Dim tMemHead As New TYPE_TRK_HEAD
            ' �Z�b�g
            tMemHead.nActiveID = g_typStartArgs.nId

            ' ����
            tClass_GlovalMemory.SetMappedT(m_memTrkPtr, 0, tMemHead)

            Dim tMemVal As New TYPE_TRK_ITEM                        ' ���H���̏��\����
            Dim nMemOffset As Integer = SIZE_TYPE_HEAD + SIZE_TYPE_ITEM * g_typStartArgs.nId        ' ���\���̈ʒu

            ' �Z�b�g
            tMemVal.nY = CInt(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y))
            tMemVal.nX = CInt(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X))
            ' ����
            tClass_GlovalMemory.SetMappedT(m_memTrkPtr, nMemOffset, tMemVal)
        End If
    End Sub


    ''' <summary>
    ''' �Ȉ՘A�g ���L�������ڑ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tsbSync_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tsbSync.Click
        '''' �O���A�g���́A�s��
        If g_bOutTrk Then
            tsbSync.Checked = False
            Return
        End If

        '''' ���L����������
        If tsbSync.Checked Then
            '''' ���L���������� (�A�g�̒��łP�ł��o����Ζ��Ȃ�)
            m_memSyncPtr = tClass_GlovalMemory.AllocMapped(SIZE_TYPE_SYNC, TBL_RPI_SYNC_NAME)   ' Lock or Open
            If IntPtr.Zero = m_memSyncPtr Then
                MsgBox("���L�������������s")
                tsbSync.Checked = False
                Return
            End If


            Dim tMemVal As TYPE_SYNC                        ' ���H���̏��\����
            tClass_GlovalMemory.GetMappedT(m_memSyncPtr, 0, tMemVal)



            ' �덷�Z�o
            m_nSyncOffsetY = CInt(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - tMemVal.nY
            m_nSyncOffsetX = CInt(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - tMemVal.nX

        Else
            If IntPtr.Zero <> m_memSyncPtr Then
                tClass_GlovalMemory.FreeMapped(m_memSyncPtr, TBL_RPI_SYNC_NAME)
                m_memSyncPtr = IntPtr.Zero
                m_nSyncOffsetY = 0
                m_nSyncOffsetX = 0
            End If
        End If
    End Sub
    ''' <summary>
    ''' ���L�������ɏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetMemSync_ValLen()
        ' ��Exe���A�N�e�B�u�̏ꍇ�̂݁A���ʏ�����
        If Not Nothing Is Form.ActiveForm Then
            Dim tMemVal As New TYPE_SYNC                        ' ���H���̏��\����
            tMemVal.nY = CInt(mcls_RpiView.GetPos_DpToLen_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y)) - m_nSyncOffsetY  ' �I�t�Z�b�g���͖����ɂ����l
            tMemVal.nX = CInt(mcls_RpiView.GetPos_DpToLen_X(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.X)) - m_nSyncOffsetX

            ' ����
            tClass_GlovalMemory.SetMappedT(m_memSyncPtr, 0, tMemVal)
        End If
    End Sub
End Class

