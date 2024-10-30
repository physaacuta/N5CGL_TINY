'*********************************************************************************
'�@�d���׈ꗗ��ʃ��C���N���X
'	[Ver]
'		Ver.01    2008/07/04  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports Microsoft.Win32

Imports KizuLibCLI

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tMod
Imports System.Reflection

#Const tDebug = False

Public Module modMain
    '///////////////////////////////////////////////////////////////////////////
    '�萔
    '///////////////////////////////////////////////////////////////////////////
    Private Const DEFLIST_DSP_PATTERN As EM_DSP_PATTERN = EM_DSP_PATTERN.nPatternNo01   '�\����ʐ�


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���[�o���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_NOA As Integer                                                     ' NOA�ڑ��t���O (0:�I�����C��  1:���u)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    '�p�����[�^��`
    Public Enum EM_PARAM_MODE
        nALL = 0    '�S��
        nType       '�r��E�O���[�h
        nList       '�r�ꗗ
    End Enum

    '��ʕ\���p�^�[��
    Public Enum EM_DSP_PATTERN
        nPatternNo01 = 1
        nPatternNo02
        nPatternNo03
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' �����N���X
    Private m_ap As ApplicationContext                                  ' �R���e�L�X�g
    Private mcls_Log As tClass_LogManager                               ' ���O�Ǘ��N���X
    Private mcls_LogEx As tClass_LogManager                             ' ���O�Ǘ��N���X(�d�v)
    Private mcls_Param As clsParamManager                               ' �p�����[�^���Ǘ�

    Private WithEvents m_timGetMail As New Timer                        ' ���[���X���b�g�p�X���b�h�����ʃX���b�h�ւ̋��n��
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)           ' ���[���X���b�g�̃L���[

    Private mcls_BG As clsProductBackColor                              ' ���Y�ԍ��w�i�F�ݒ�
    Private m_frmCglDefList_01(1) As frmCglDefList_Pattern01
    Private m_frmCglDefList_02(1) As frmCglDefList_Pattern02
    Private m_frmCglDefList_03(1) As frmCglDefList_Pattern03


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�o�b�O���O�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private gcls_frmDebugLog As Form
    Private lbxLog As ListBox
    Private lbxLogEx As ListBox

    Private Sub frmDebugLog_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs)
        If e.CloseReason <> CloseReason.WindowsShutDown And
            e.CloseReason <> CloseReason.TaskManagerClosing And
            e.CloseReason <> CloseReason.FormOwnerClosing And
            e.CloseReason <> CloseReason.ApplicationExitCall Then
            e.Cancel = True
            CType(sender, Form).Hide()
        End If
    End Sub

    ''' <summary>
    ''' notepad���J��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDebugLog_DoubleClicked(ByVal sender As Object, ByVal e As EventArgs)
        mcls_Log.ShowEdit()
    End Sub
    ''' <summary>
    ''' notepad���J��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDebugLogEX_DoubleClicked(ByVal sender As Object, ByVal e As EventArgs)
        mcls_LogEx.ShowEdit()
    End Sub

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitFrmDebugLog()
        gcls_frmDebugLog = New Form
        gcls_frmDebugLog.Text = "�f�o�b�O���O"
        gcls_frmDebugLog.Size = New Size(1024, 400)
        lbxLog = New ListBox
        lbxLogEx = New ListBox

        Dim splitContainer As New SplitContainer
        splitContainer.Dock = DockStyle.Fill

        splitContainer.Panel1.Controls.Add(lbxLogEx)
        splitContainer.Panel2.Controls.Add(lbxLog)
        splitContainer.Orientation = Orientation.Horizontal
        lbxLog.Dock = DockStyle.Fill
        lbxLogEx.Dock = DockStyle.Fill
        gcls_frmDebugLog.Controls.Add(splitContainer)

        AddHandler gcls_frmDebugLog.FormClosing, AddressOf frmDebugLog_FormClosing
        AddHandler lbxLog.DoubleClick, AddressOf frmDebugLog_DoubleClicked
        AddHandler lbxLogEx.DoubleClick, AddressOf frmDebugLogEX_DoubleClicked
    End Sub


    ''' <summary>
    ''' �f�o�b�O���O��\������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ShowDebugLog()
        If gcls_frmDebugLog IsNot Nothing Then
            gcls_frmDebugLog.Show()
            ' �\������ԏ�Ɏ����Ă���
            gcls_frmDebugLog.TopMost = True
            'gcls_frmDebugLog.TopMost = False
        End If
    End Sub

    ''' <summary>
    ''' ���ʃ��O�o��
    ''' </summary>
    ''' <param name="no"></param>
    ''' <param name="fmt"></param>
    ''' <param name="param"></param>
    ''' <remarks>���O�o�͂̋L�q�������Ȃ�̂ŁA�Ǝ��Œ�`����</remarks>
    Public Sub Log(ByVal no As LNO, ByVal fmt As String, ByVal ParamArray param() As Object)
        Try
            ' ���O�̐擪�Ɍ��݂̎g�p�������ʂ�\������
            Dim log As String = String.Format("[{0,7:###,###}kb] ", My.Application.Info.WorkingSet / 1024)
            log &= String.Format(fmt, param)
            mcls_Log.Write(no, log)
            Select Case no
                Case LNO.NON
                Case LNO.MSG
                Case LNO.ERR
                    mcls_LogEx.Write(no, log)
                Case LNO.INF
                Case LNO.WAR
                    mcls_LogEx.Write(no, log)
                Case Else
            End Select

        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("���O�����݃t�H�[�}�b�g�G���[[{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' ���ʃ��O�o��
    ''' </summary>
    ''' <param name="no"></param>
    ''' <param name="fmt"></param>
    ''' <param name="param"></param>
    ''' <remarks>���O�o�͂̋L�q�������Ȃ�̂ŁA�Ǝ��Œ�`����</remarks>
    Public Sub LogEx(ByVal no As LNO, ByVal fmt As String, ByVal ParamArray param() As Object)
        Try
            ' ���O�̐擪�Ɍ��݂̎g�p�������ʂ�\������
            Dim log As String = String.Format("[{0,7:###,###}kb] ", My.Application.Info.WorkingSet / 1024)
            log &= String.Format(fmt, param)
            mcls_LogEx.Write(no, log)
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("���O(�d�v)�����݃t�H�[�}�b�g�G���[[{0}]", ex.Message))
        End Try
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �G���g���[�|�C���g
    ''' </summary>
    ''' <remarks></remarks>
    Public Function Main() As Integer
        'Windows�V���b�g�_�E���A�ċN���ʒm�擾�C�x���g
        AddHandler SystemEvents.SessionEnding, AddressOf SystemEvents_SessionEnding

        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll �� �m�F���ĉ������B" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("�^�X�N���� ���s"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Function
        End Try

        mcls_LogEx.Write(LNO.MSG, "�d���׈ꗗ��ʋN��")
        LogEx(LNO.MSG, String.Format("���O(�d�v)�J�n"))
        Log(LNO.MSG, String.Format("���O�J�n"))

        Dim nMyID As Integer                        ' ��ʕ\��ID
        Dim monitor() As System.Windows.Forms.Screen
        Dim nCnt As Integer
        monitor = System.Windows.Forms.Screen.AllScreens()
        nCnt = monitor.Length    '���j�^��

        Try
            '-------------------------------------------------------------------
            'KizuTask.ini�̎擾
            '-------------------------------------------------------------------
            '�\���r�摜��`
            Dim nImageWidth As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_WIDTH", TASKINI_NAME, 25)
            If nImageWidth <= 0 Then
                nImageWidth = 25
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���r�摜��`�ُ�[{0}]", nImageWidth))
            End If
            '�\���{��
            Dim nImageRate05 As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_RATE_05", TASKINI_NAME, 100)
            If nImageRate05 <= 0 Then
                nImageRate05 = 100
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���r�摜�{����`�ُ�[{0}]", nImageRate05))
            End If
            Dim nImageRate14 As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_RATE_14", TASKINI_NAME, 75)
            If nImageRate14 <= 0 Then
                nImageRate14 = 100
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���r�摜�{����`�ُ�[{0}]", nImageRate14))
            End If
            Dim nImageRate30 As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_RATE_30", TASKINI_NAME, 50)
            If nImageRate30 <= 0 Then
                nImageRate30 = 100
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���r�摜�{����`�ُ�[{0}]", nImageRate30))
            End If
            '��ʕ\�����[�h
            Dim nDspMode As Integer = tMod.ReadIniInt(HY_DEFLIST, "DSP_TYPE", TASKINI_NAME)
            '-------------------------------------------------------------------
            ' �p�����[�^���ǂݏo��
            '-------------------------------------------------------------------
            mcls_Param = New clsParamManager(mcls_LogEx)
            While (Not mcls_Param.ReadAllParam())                               '�p�����[�^�ǂݏo����������܂ŔS��
                Application.DoEvents()
                Threading.Thread.Sleep(1000)
            End While
            ''-------------------------------------------------------------------
            '' �d���׈ꗗ��ʕ\��
            ''-------------------------------------------------------------------
            '���Y�ԍ��w�i�F�ݒ�
            mcls_BG = New clsProductBackColor(mcls_Log)
            mcls_BG.ReadPnoBGinit()

            'TOP��\��
            If 0 = nDspMode Or 1 = nDspMode Then

                '��ʕ\���ʒu
                nMyID = tMod.ReadIniInt(HY_DEFLIST, "DSP_POS_TOP", TASKINI_NAME, 1)

                '���j�^�ԍ��Ȃ��ꍇ�́A�\���ł��郂�j�^�ɕ\��
                If nCnt < nMyID Then nMyID = nCnt
                If 0 >= nMyID Then nMyID = 1

                Select Case DEFLIST_DSP_PATTERN
                    Case EM_DSP_PATTERN.nPatternNo01
                        m_frmCglDefList_01(0) = New frmCglDefList_Pattern01
                        m_frmCglDefList_01(0).DspInitForm(0, nMyID, mcls_Log, mcls_Param, mcls_BG, nImageWidth, nImageRate05, EM_DSP_PATTERN.nPatternNo01)
                    Case EM_DSP_PATTERN.nPatternNo02
                        m_frmCglDefList_02(0) = New frmCglDefList_Pattern02
                        m_frmCglDefList_02(0).DspInitForm(0, nMyID, mcls_Log, mcls_Param, mcls_BG, nImageWidth, nImageRate14, EM_DSP_PATTERN.nPatternNo02)
                    Case EM_DSP_PATTERN.nPatternNo03
                        m_frmCglDefList_03(0) = New frmCglDefList_Pattern03
                        m_frmCglDefList_03(0).DspInitForm(0, nMyID, mcls_Log, mcls_Param, mcls_BG, nImageWidth, nImageRate30, EM_DSP_PATTERN.nPatternNo03)
                End Select

                '��ʕ\��
                Call DspDefList(DEFLIST_DSP_PATTERN, 0)

            End If

            'BOT��\��
            If 0 = nDspMode Or 2 = nDspMode Then

                '��ʕ\���ʒu
                nMyID = tMod.ReadIniInt(HY_DEFLIST, "DSP_POS_BOT", TASKINI_NAME, 1)

                '���j�^�ԍ��Ȃ��ꍇ�́A�\���ł��郂�j�^�ɕ\��
                If nCnt < nMyID Then nMyID = nCnt

                Select Case DEFLIST_DSP_PATTERN
                    Case EM_DSP_PATTERN.nPatternNo01
                        m_frmCglDefList_01(1) = New frmCglDefList_Pattern01
                        m_frmCglDefList_01(1).DspInitForm(1, nMyID, mcls_Log, mcls_Param, mcls_BG, nImageWidth, nImageRate05, EM_DSP_PATTERN.nPatternNo01)
                    Case EM_DSP_PATTERN.nPatternNo02
                        m_frmCglDefList_02(1) = New frmCglDefList_Pattern02
                        m_frmCglDefList_02(1).DspInitForm(1, nMyID, mcls_Log, mcls_Param, mcls_BG, nImageWidth, nImageRate14, EM_DSP_PATTERN.nPatternNo02)
                    Case EM_DSP_PATTERN.nPatternNo03
                        m_frmCglDefList_03(1) = New frmCglDefList_Pattern03
                        m_frmCglDefList_03(1).DspInitForm(1, nMyID, mcls_Log, mcls_Param, mcls_BG, nImageWidth, nImageRate30, EM_DSP_PATTERN.nPatternNo03)
                End Select

                '��ʕ\��
                Call DspDefList(DEFLIST_DSP_PATTERN, 1)

            End If

            For ii As Integer = 0 To UBound(m_frmCglDefList_01)
                Select Case DEFLIST_DSP_PATTERN
                    Case EM_DSP_PATTERN.nPatternNo01
                        If Not m_frmCglDefList_01(ii) Is Nothing Then
                            AddHandler m_frmCglDefList_01(ii).DspDefList, AddressOf DspDefList                     '��ʕ\���ݒ�
                            AddHandler m_frmCglDefList_01(ii).ModeClickEvent, AddressOf ChangeDspModeCapl          '�����X�V�E�ꎞ��~�{�^���N���b�N����
                            AddHandler m_frmCglDefList_01(ii).ev_ChangeOverlyVisible, AddressOf ChangeOverLay      '�I�[�o�[���C�\���ύX
                            AddHandler m_frmCglDefList_01(ii).ev_ChangeMemoriVisible, AddressOf ChangeMemori       '�ڐ���\���ύX
                            AddHandler m_frmCglDefList_01(ii).StopSelectedIndexChanged, AddressOf ChangeStopTimer  '�ꎞ��~�ύX�C�x���g
                            AddHandler m_frmCglDefList_01(ii).PopSelectedIndexChanged, AddressOf ChangePopUpTimer  '�|�b�v�A�b�v�ύX�C�x���g
                        End If
                    Case EM_DSP_PATTERN.nPatternNo02
                        If Not m_frmCglDefList_02(ii) Is Nothing Then
                            AddHandler m_frmCglDefList_02(ii).DspDefList, AddressOf DspDefList                     '��ʕ\���ݒ�
                            AddHandler m_frmCglDefList_02(ii).ModeClickEvent, AddressOf ChangeDspModeCapl          '�����X�V�E�ꎞ��~�{�^���N���b�N����
                            AddHandler m_frmCglDefList_02(ii).ev_ChangeOverlyVisible, AddressOf ChangeOverLay      '�I�[�o�[���C�\���ύX
                            AddHandler m_frmCglDefList_02(ii).ev_ChangeMemoriVisible, AddressOf ChangeMemori       '�ڐ���\���ύX
                            AddHandler m_frmCglDefList_02(ii).StopSelectedIndexChanged, AddressOf ChangeStopTimer  '�ꎞ��~�ύX�C�x���g
                            AddHandler m_frmCglDefList_02(ii).PopSelectedIndexChanged, AddressOf ChangePopUpTimer  '�|�b�v�A�b�v�ύX�C�x���g
                        End If
                    Case EM_DSP_PATTERN.nPatternNo03
                        If Not m_frmCglDefList_03(ii) Is Nothing Then
                            AddHandler m_frmCglDefList_03(ii).DspDefList, AddressOf DspDefList                     '��ʕ\���ݒ�
                            AddHandler m_frmCglDefList_03(ii).ModeClickEvent, AddressOf ChangeDspModeCapl          '�����X�V�E�ꎞ��~�{�^���N���b�N����
                            AddHandler m_frmCglDefList_03(ii).ev_ChangeOverlyVisible, AddressOf ChangeOverLay      '�I�[�o�[���C�\���ύX
                            AddHandler m_frmCglDefList_03(ii).ev_ChangeMemoriVisible, AddressOf ChangeMemori       '�ڐ���\���ύX
                            AddHandler m_frmCglDefList_03(ii).StopSelectedIndexChanged, AddressOf ChangeStopTimer  '�ꎞ��~�ύX�C�x���g
                            AddHandler m_frmCglDefList_03(ii).PopSelectedIndexChanged, AddressOf ChangePopUpTimer  '�|�b�v�A�b�v�ύX�C�x���g
                        End If
                End Select
            Next ii

        Catch ex As Exception

        End Try


        '���[���X���b�g��M�Ď��̊J�n
        m_timGetMail.Interval = 500
        m_timGetMail.Start()

        '' ���C���X���b�h����
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThread���R�[�������܂� �����Ńu���b�N


        '-----------------------------------------------------------------------
        ' ���C���X���b�h�I��
        '-----------------------------------------------------------------------
        Task_Last()

        End
    End Function

    ''' <summary>
    ''' �^�X�N������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_First()

        ' ��ʎ󂯓n���p�L���[������
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '-----------------------------------------------------------------------
        '�r���^�X�N�J�n���� (�����ł�����ƃ��O����o���Ȃ�)
        '-----------------------------------------------------------------------
        '�^�X�N�C�j�V���� (DB�ւ̃A�N�Z�X�͖���)
        KizuMod.SetLineID()

        '' ini�t�@�C����� �萔�I�ȃO���[�o�[���ϐ��ǂݍ���
        g_NOA = tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)
        If 0 <> mcls_Mod.Task_Init(HY_DEFLIST) Then
            KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", HY_DEFLIST))
            End
        End If
        InitFrmDebugLog()
        gcls_frmDebugLog.Show()
        gcls_frmDebugLog.Hide()

        '' ���O�N���X����()
        If gcls_frmDebugLog IsNot Nothing Then
            mcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, lbxLog)
            mcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, lbxLogEx)
        Else
            mcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, Nothing)
            mcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, Nothing)
        End If

    End Sub

    ''' <summary>
    ''' �^�X�N�I������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_Last()

        Try
            If m_timGetMail Is Nothing = False Then
                m_timGetMail.Stop()                                           ' ���[���X���b�g�Ď���~
            End If

            If m_MailQue Is Nothing = False Then                                ' ���[���X���b�g�L���[�폜
                m_MailQue.Dispose()
                m_MailQue = Nothing
            End If

            For ii As Integer = 0 To UBound(m_frmCglDefList_01)
                If Not m_frmCglDefList_01(ii) Is Nothing Then
                    m_frmCglDefList_01(ii).Close()
                    m_frmCglDefList_01(ii).Dispose()
                    m_frmCglDefList_01(ii) = Nothing
                End If
                If Not m_frmCglDefList_02(ii) Is Nothing Then
                    m_frmCglDefList_02(ii).Close()
                    m_frmCglDefList_02(ii).Dispose()
                    m_frmCglDefList_02(ii) = Nothing
                End If
                If Not m_frmCglDefList_03(ii) Is Nothing Then
                    m_frmCglDefList_03(ii).Close()
                    m_frmCglDefList_03(ii).Dispose()
                    m_frmCglDefList_03(ii) = Nothing
                End If
            Next ii

            If Not mcls_Param Is Nothing Then                                   '�p�����[�^�}�l�[�W���̉��
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If

        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("{0} [{1}]", "���C�������I���ُ�", ex.Message))
        End Try

        RemoveHandler SystemEvents.SessionEnding,
           AddressOf SystemEvents_SessionEnding                                 ' Windows�V���b�g�_�E���A�ċN���ʒm�擾�C�x���g

        mcls_LogEx.Write(LNO.MSG, "�d���׈ꗗ��ʏI��")
        'If gcls_frmDebugLog IsNot Nothing Then
        '    gcls_frmDebugLog.Dispose()
        '    gcls_frmDebugLog = Nothing
        'End If
        If mcls_Log IsNot Nothing Then
            mcls_Log.Dispose()
            mcls_Log = Nothing
        End If
        If mcls_LogEx IsNot Nothing Then
            mcls_LogEx.Dispose()
            mcls_LogEx = Nothing
        End If

        mcls_Mod.Task_Exit()                                                    ' ���C���X���b�h�I��
    End Sub

    ''' <summary>
    ''' ���[���X���b�g��M
    ''' </summary>
    ''' <param name="pData">�f�[�^</param>
    ''' <param name="nEvtNo">�C�x���g�ԍ�</param>
    ''' <remarks></remarks>
    Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
        Dim ii As Integer = 0
        Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)

        '''' ��ʃX���b�h�֓n��
        m_MailQue.SetItem(que_l)
    End Sub

    ''' <summary>
    ''' �^�C�}�[����[���[���擾]
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub m_timGetMail_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timGetMail.Tick
        '���[���擾
        Dim que_l As COMMON_QUE_L = m_MailQue.GetItem(0)

        '���[���Ȃ�
        If 0 = que_l.nEventNo Then
            Exit Sub
        End If

        Try
            Select Case que_l.nEventNo
                Case FACT_HY_DEFLIST_01                      '�p�����[�^�ύX�ʒm
                    Log(LNO.MSG, "�p�����[�^�ύX�ʒm")
                    LogEx(LNO.MSG, "�p�����[�^�ύX�ʒm")
                    mcls_Param.ReadAllParam()

                    For ii As Integer = 0 To UBound(m_frmCglDefList_01)
                        Select Case DEFLIST_DSP_PATTERN
                            Case EM_DSP_PATTERN.nPatternNo01
                                If Not m_frmCglDefList_01(ii) Is Nothing Then
                                    m_frmCglDefList_01(ii).DspUpdate()          '��ʍX�V
                                End If
                            Case EM_DSP_PATTERN.nPatternNo02
                                If Not m_frmCglDefList_02(ii) Is Nothing Then
                                    m_frmCglDefList_02(ii).DspUpdate()          '��ʍX�V
                                End If
                            Case EM_DSP_PATTERN.nPatternNo03
                                If Not m_frmCglDefList_03(ii) Is Nothing Then
                                    m_frmCglDefList_03(ii).DspUpdate()          '��ʍX�V
                                End If
                        End Select
                    Next ii

                Case FACT_KS_PCNAME_CHANGE                  'PC���̕ύX�ʒm
                    Log(LNO.MSG, "PC���̕ύX�ʒm")
                    LogEx(LNO.MSG, "PC���̕ύX�ʒm")

                Case FACT_KS_END                            '�^�X�N�I���ʒm
                    mcls_Log.Write(tClass_LogManager.LNO.MSG, "�^�X�N�I���ʒm")
                    mcls_LogEx.Write(tClass_LogManager.LNO.MSG, "�^�X�N�I���ʒm")

                    For ii As Integer = 0 To UBound(m_frmCglDefList_01)
                        If Not m_frmCglDefList_01(ii) Is Nothing Then
                            m_frmCglDefList_01(ii).Close()
                            m_frmCglDefList_01(ii).Dispose()
                            m_frmCglDefList_01(ii) = Nothing
                        End If
                        If Not m_frmCglDefList_02(ii) Is Nothing Then
                            m_frmCglDefList_02(ii).Close()
                            m_frmCglDefList_02(ii).Dispose()
                            m_frmCglDefList_02(ii) = Nothing
                        End If
                        If Not m_frmCglDefList_03(ii) Is Nothing Then
                            m_frmCglDefList_03(ii).Close()
                            m_frmCglDefList_03(ii).Dispose()
                            m_frmCglDefList_03(ii) = Nothing
                        End If
                    Next ii

                    m_ap.ExitThread()                       '�X���b�h�I��
                Case Else                                   '�N���v���ُ�
                    mcls_LogEx.Write(LNO.ERR, String.Format("�N���v���ُ�[{0}]", que_l.nEventNo))
                    KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "err_code =[" & que_l.nEventNo & "]"))
            End Select
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("���[���X���b�g��M�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' Windows�V���b�g�_�E���A�ċN���ʒm�擾�C�x���g
    ''' </summary>
    ''' <param name="sender">�C�x���g������</param>
    ''' <param name="e">�C�x���g�p�����[�^</param>
    ''' <remarks></remarks>
    Private Sub SystemEvents_SessionEnding(ByVal sender As Object, ByVal e As Microsoft.Win32.SessionEndingEventArgs)
        Try
            Dim que As KizuLibCLI.COMMON_QUE_S = Nothing
            mcls_Log.Write(LNO.MSG, "Windows�V���b�g�_�E���A�ċN���ʒm")
            que.nEventNo = FACT_KS_END                                          ' �^�X�N�I��
            que.nLineNo = KizuMod.GetLineID()
            KizuMod.Send_Mail(KizuMod.GetMyTaskName(), ".", que)                ' �����֏I���ʒm
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("Windows�V���b�g�_�E���A�ċN���ʒm�擾�C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub


    '/////////////////////////////////////////////////////////////////////////////////
    '��ʃn���h���C�x���g
    '/////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �����X�V�A�ꎞ��~�{�^���N���b�N����
    ''' </summary>
    ''' <param name="nOuDiv">�\���敪 0:�\ 1:��</param>
    ''' <remarks></remarks>
    Private Sub ChangeDspModeCapl(ByVal nOuDiv As Integer, ByVal nPattern As modMain.EM_DSP_PATTERN)

        Select Case nPattern
            Case EM_DSP_PATTERN.nPatternNo01
                m_frmCglDefList_01(nOuDiv).ChangeMode()
            Case EM_DSP_PATTERN.nPatternNo02
                m_frmCglDefList_02(nOuDiv).ChangeMode()
            Case EM_DSP_PATTERN.nPatternNo03
                m_frmCglDefList_03(nOuDiv).ChangeMode()
        End Select

    End Sub


    ''' <summary>
    ''' �p�����[�^�ύX�ʒm
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Send_Mail()
        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_TO_DEFECT_01
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' �f�[�^��
        que.data(0) = 0                     ' �x�����s�L�[  0�Œ�
        que.data(1) = CH_PARAM_DEFLIST      ' �p�����[�^�e�[�u��ID
        que.data(2) = 0                     ' �ύX�����\�ʏ��No (�����\�ʏ��No:1�`  �S��:CH_SCODE_ALL  ����:0)

        KizuLibCLI.KizuMod.Send_Mail(TO_DEFECT, "*", que)

    End Sub

    ''' <summary>
    ''' ���[�J��INI�Ƀf�[�^��������
    ''' </summary>
    ''' <param name="strKey">�L�[</param>
    ''' <param name="strVal">�l</param>
    ''' <remarks></remarks>
    Public Sub WriteIniData(ByVal strKey As String, ByVal strVal As String)

        tMod.WriteIni(HY_DEFLIST, strKey, LOCALINI_NAME, strVal)

    End Sub

    ''' <summary>
    ''' ��ʕ\��
    ''' </summary>
    ''' <param name="nImagePattern">�\���p�^�[��</param>
    ''' <param name="nOuDiv">�\���敪(0:�\,1:��)</param>
    ''' <remarks></remarks>
    Private Sub DspDefList(ByVal nImagePattern As EM_DSP_PATTERN, ByVal nOuDiv As Integer)

        Select Case nImagePattern
            Case EM_DSP_PATTERN.nPatternNo01
                m_frmCglDefList_01(nOuDiv).Visible = True
            Case EM_DSP_PATTERN.nPatternNo02
                m_frmCglDefList_02(nOuDiv).Visible = True
            Case EM_DSP_PATTERN.nPatternNo03
                m_frmCglDefList_03(nOuDiv).Visible = True
        End Select

    End Sub

    ''' <summary>
    ''' �I�[�o���C�\���`�F���W
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeOverLay(ByVal bOnOff As Boolean)

        Dim ii As Integer

        'TOP,BOT�ɒʒm
        For ii = 0 To m_frmCglDefList_01.Length - 1
            If Not m_frmCglDefList_01(ii) Is Nothing Then
                m_frmCglDefList_01(ii).Overlay = bOnOff
            End If

            If Not m_frmCglDefList_02(ii) Is Nothing Then
                m_frmCglDefList_02(ii).Overlay = bOnOff
            End If

            If Not m_frmCglDefList_03(ii) Is Nothing Then
                m_frmCglDefList_03(ii).Overlay = bOnOff
            End If
        Next ii

    End Sub

    ''' <summary>
    ''' �ڐ���\���`�F���W
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeMemori(ByVal bOnOff As Boolean)

        Dim ii As Integer

        'TOP,BOT�ɒʒm
        For ii = 0 To m_frmCglDefList_01.Length - 1
            If Not m_frmCglDefList_01(ii) Is Nothing Then
                m_frmCglDefList_01(ii).Memori = bOnOff
            End If

            If Not m_frmCglDefList_02(ii) Is Nothing Then
                m_frmCglDefList_02(ii).Memori = bOnOff
            End If

            If Not m_frmCglDefList_03(ii) Is Nothing Then
                m_frmCglDefList_03(ii).Memori = bOnOff
            End If
        Next ii

    End Sub


    ''' <summary>
    ''' �ꎞ��~�|�b�v�A�b�v�^�C�}�[�ύX
    ''' </summary>
    ''' <param name="nOuDiv">�\���敪 0:�\ 1:��</param>
    ''' <remarks></remarks>
    Private Sub ChangeStopTimer(ByVal nOuDiv As Integer, ByVal nVal As Integer)

        If Not m_frmCglDefList_01(nOuDiv) Is Nothing Then
            m_frmCglDefList_01(nOuDiv).ChangeStopTimer(nVal)
        End If

        If Not m_frmCglDefList_02(nOuDiv) Is Nothing Then
            m_frmCglDefList_02(nOuDiv).ChangeStopTimer(nVal)
        End If

        If Not m_frmCglDefList_03(nOuDiv) Is Nothing Then
            m_frmCglDefList_03(nOuDiv).ChangeStopTimer(nVal)
        End If

    End Sub


    ''' <summary>
    ''' �|�b�v�A�b�v�^�C�}�[�ύX
    ''' </summary>
    ''' <param name="nOuDiv">�\���敪 0:�\ 1:��</param>
    ''' <remarks></remarks>
    Private Sub ChangePopUpTimer(ByVal nOuDiv As Integer, ByVal nVal As Integer)

        If Not m_frmCglDefList_01(nOuDiv) Is Nothing Then
            m_frmCglDefList_01(nOuDiv).ChangePopTimer(nVal)
        End If

        If Not m_frmCglDefList_02(nOuDiv) Is Nothing Then
            m_frmCglDefList_02(nOuDiv).ChangePopTimer(nVal)
        End If

        If Not m_frmCglDefList_03(nOuDiv) Is Nothing Then
            m_frmCglDefList_03(nOuDiv).ChangePopTimer(nVal)
        End If

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�H�[���^�C�g���ݒ�
    ''' </summary>
    ''' <param name="frmData">�t�H�[��</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

        frmData.Text = String.Format("[{0}] {1}", SYSTEM_TITLE, frmData.Text)

    End Sub

    ''' <summary>
    ''' KizuLibCLI.DLL�̓Ǎ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit�؂�ւ��Ή�</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function
End Module
