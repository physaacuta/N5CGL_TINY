'*********************************************************************************
' �G���g���[
'	[Ver]
'		Ver.01    2012/05/31  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports KizuLibCLI
Imports System.Reflection

Public Module modMain
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���[�o���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public gcls_Log As tClass_LogManager                                ' ���O�Ǘ��N���X
    Public gcls_LogEx As tClass_LogManager                              ' ���O�Ǘ��N���X(�d�v)
    Public g_NOA As Integer                                             ' NOA�ڑ��t���O (0:�I�����C��  1:���u)
    Public g_bOutTrk As Boolean                                         ' �O���A���L�� (false:�]��=�P�Ɠ���  true:�A������ )
    Public g_typStartArgs As START_ARGS                                 ' �N������
    Public g_nFormPos As Integer

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ���C���X���b�h�Ǘ��p (�قڕK�{�̊�{�̌`)
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod               ' �����N���X �i�ʃX���b�h�Ȃ̂Œ��Ӂj
    Private m_ap As ApplicationContext                                  ' �R���e�L�X�g

    Private WithEvents m_trmFrmThread As New Timer                      ' ���[���X���b�g�p�X���b�h�����ʃX���b�h�ւ̋��n��
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)       ' ���[���X���b�g�̃L���[

    Private m_frm As frmMenu                                            ' �A�N�Z�X�t�H�[��

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �C�x���g��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �ғ���Ԓʒm ��M�C�x���g
    Public Event tEV_MailSlotRecv_FACT_SO_GAMENN_01(ByVal que_l As COMMON_QUE_L)
    ' �R�C�����擾�����ʒm
    Public Event tEV_MailSlotRecv_FACT_SO_GAMENN_02(ByVal que_l As COMMON_QUE_STR)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�o�b�O���O�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private gcls_frmDebugLog As Form
    Private lbxLog As ListBox
    Private lbxLogEx As ListBox

    Private Sub frmDebugLog_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs)
        e.Cancel = True
        CType(sender, Form).Hide()
    End Sub

    ''' <summary>
    ''' notepad���J��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDebugLog_DoubleClicked(ByVal sender As Object, ByVal e As EventArgs)
        gcls_Log.ShowEdit()
    End Sub
    ''' <summary>
    ''' notepad���J��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDebugLogEX_DoubleClicked(ByVal sender As Object, ByVal e As EventArgs)
        gcls_LogEx.ShowEdit()
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
            gcls_Log.Write(no, log)
            Select Case no
                Case LNO.NON
                Case LNO.MSG
                Case LNO.ERR
                    gcls_LogEx.Write(no, log)
                Case LNO.INF
                Case LNO.WAR
                    gcls_LogEx.Write(no, log)
                Case Else
            End Select

        Catch ex As Exception
            gcls_LogEx.Write(LNO.ERR, String.Format("���O�����݃t�H�[�}�b�g�G���[[{0}]", ex.Message))
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
            gcls_LogEx.Write(no, log)
        Catch ex As Exception
            gcls_LogEx.Write(LNO.ERR, String.Format("���O(�d�v)�����݃t�H�[�}�b�g�G���[[{0}]", ex.Message))
        End Try
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �G���g���[�|�C���g
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        ' �F�؏��� (�������A�ꎞ�I�Ƀ��[�U�[�`�F�b�N�����B�ŏI�I�ɂ͕K���L���ɂ��邱��)
        If ENABLE_USERCHECK Then
            If Not tNinsho.IsUserCheck(SO_GAMENN) Then
                Exit Sub
            End If
        End If

        Task_First()

        gcls_Log.Write(LNO.MSG, "�I�y���[�^��ʋN��")
        gcls_LogEx.Write(LNO.MSG, "�I�y���[�^��ʋN��")

        'Dim cmds As String() = System.Environment.GetCommandLineArgs()
        'For Each cmd As String In cmds
        '    gcls_Log.Write(LNO.INF, String.Format("�R�}���h���C������[{0}]", cmd))
        'Next

        '' ��ʕ\��
        'If cmds.Length >= 2 Then
        '    ' �R�}���h����������ꍇ
        '    m_frm = New frmMenu(10)
        'Else
        '    ' �R�}���h�����������ꍇ
        '    m_frm = New frmMenu
        'End If
        'm_frm.Show()

        ' ��ʕ\��
        m_frm = New frmMenu
        m_frm.Show()

        '' Mail�X���b�h�����ʃX���b�h�ւ̋��n���^�C�}�[�J�n()
        m_trmFrmThread.Interval = 100
        m_trmFrmThread.Start()

        '' ���C���X���b�h����
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThread���R�[�������܂� �����Ńu���b�N

        Task_Last()

    End Sub

    ''' <summary>
    ''' �^�X�N������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_First()

        '���C��ID
        KizuLibCLI.KizuMod.SetLineID()

        '' ini�t�@�C����� �萔�I�ȃO���[�o�[���ϐ��ǂݍ���
        g_NOA = tMod.tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)

        ' ��ʎ󂯓n���p�L���[������
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        ' �O���A���L��
        g_bOutTrk = CBool(IIf(1 = System.Environment.GetCommandLineArgs.Length, False, True))

        g_nFormPos = 1
        '''' �O���A���L��̏ꍇ�̂݁A�������
        If g_bOutTrk Then
            g_typStartArgs.sArgs = System.Environment.GetCommandLineArgs
            For Each sVal As String In g_typStartArgs.sArgs
                sVal.Trim()
                If -1 <> sVal.IndexOf(START_FORM_POS) Then
                    g_typStartArgs.nFormPos = CInt(sVal.Replace(START_FORM_POS, "").Trim())
                    g_nFormPos = g_typStartArgs.nFormPos
                End If
            Next
        End If

        '' �^�X�N�C�j�V����
        If 0 <> mcls_Mod.Task_Init(SO_GAMENN) Then
            If NOA_ONLINE = g_NOA Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", SO_GAMENN))
            End If

            End
        End If

        InitFrmDebugLog()
        gcls_frmDebugLog.Show()
        gcls_frmDebugLog.Hide()

        '' ���O�N���X����
        If gcls_frmDebugLog IsNot Nothing Then
            gcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, lbxLog)
            gcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, lbxLogEx)
        Else
            gcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, Nothing)
            gcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, Nothing)
        End If
    End Sub

    ''' <summary>
    ''' �^�X�N�I������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_Last()
        '' �I��
        m_frm.Close()
        m_frm.Dispose()
        m_frm = Nothing

        If gcls_frmDebugLog IsNot Nothing Then
            gcls_frmDebugLog.Dispose()
            gcls_frmDebugLog = Nothing
        End If


        ' ���̑��N���X���
        gcls_Log.Dispose()
        gcls_Log = Nothing

        ' �^�X�N�I��
        mcls_Mod.Task_Exit()

    End Sub

    ''' <summary>
    ''' �^�X�N�I������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()
        m_ap.ExitThread()                                       ' ���C���X���b�h�I��
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
    ''' ���[���X���b�g��M
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_trmFrmThread_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_trmFrmThread.Tick
        Dim que_l As COMMON_QUE_L = m_MailQue.GetItem(0)
        If que_l.nEventNo = 0 Then Return

        ' COMMON_QUE_L �� COMMON_QUE_STR �ɕϊ�����
        Dim que_str As COMMON_QUE_STR = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_STR)), COMMON_QUE_STR)


        Select Case que_l.nEventNo

            '//-----------------------------------------------------------
            Case FACT_SO_GAMENN_01  ' �ғ���Ԓʒm
                gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("�ғ���Ԓʒm��M �^�]���[�h[{0}]  PLG�敪[{1}] �Жʌ������s����[{2}] �ғ����[{3}]", que_l.data(0), que_l.data(1), que_l.data(2), que_l.data(3)))

                ' �C�x���g���s
                RaiseEvent tEV_MailSlotRecv_FACT_SO_GAMENN_01(que_l)

            Case FACT_KS_END        ' �^�X�N�I��
                gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�^�X�N�I���ʒm��M")

                m_ap.ExitThread()

            Case FACT_KS_PCNAME_CHANGE
                ''PC���ǂݒ���
                'KizuLibCLI.KizuMod.GetPcName()

            Case Else
                If NOA_ONLINE = g_NOA Then
                    KizuLibCLI.KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "EventNo = [" & que_l.nEventNo & "]"))
                End If
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "EventNo = [" & que_l.nEventNo & "]")
        End Select

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
    ''' ��ʍ�����PC�̉𑜓x�ɍ��킹�Đݒ�
    ''' </summary>
    ''' <param name="frmData">�t�H�[��</param>
    ''' <remarks></remarks>
    Public Sub SetPcHeight(ByRef frmData As Form)

        '��ʂ̍�����PC�𑜓x�̍������傫���ꍇ
        Dim nScreenHeigth As Integer = Screen.FromControl(frmData).Bounds.Height
        If nScreenHeigth < frmData.Height Then
            frmData.Height = nScreenHeigth      ' PC�̉𑜓x�������ɐݒ�
        End If

    End Sub

    ''' <summary>
    ''' ��ʕ���PC�̉𑜓x�ɍ��킹�Đݒ�
    ''' </summary>
    ''' <param name="frmData">�t�H�[��</param>
    ''' <remarks></remarks>
    Public Sub SetPcWidth(ByRef frmData As Form)

        '��ʂ̕���PC�𑜓x�̕����傫���ꍇ
        Dim nScreenWidth As Integer = Screen.FromControl(frmData).Bounds.Width
        If nScreenWidth < frmData.Width Then
            frmData.Width = nScreenWidth      ' PC�̉𑜓x�𕝂ɐݒ�
        End If

    End Sub

    ''' <summary>
    ''' �o�C�g�z��̎w��ʒu����w�蒷����HEX��������擾����
    ''' </summary>
    ''' <param name="byt"></param>
    ''' <param name="start"></param>
    ''' <param name="len"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetHexString(ByVal byt() As Byte, ByVal start As Integer, ByVal len As Integer) As String
        Dim str As String = ""

        Try
            For ii As Integer = start To start + len - 1
                str &= String.Format("{0:X2} ", byt(ii))
            Next
        Catch ex As Exception
            Return ""
        End Try

        Return str
    End Function

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
