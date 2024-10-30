'*********************************************************************************
'�@�T����ʃ��C���N���X
'	[Ver]
'		Ver.01    2012/05/09  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports Microsoft.Win32

Imports KizuLibCLI
Imports System.Reflection

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

Public Module modMain
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���[�o���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_NOA As Integer                                                     ' NOA�ڑ��t���O (0:�I�����C��  1:���u)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                           ' �����N���X

    Private m_ap As ApplicationContext                                          ' �R���e�L�X�g
    Private mcls_Log As tClass_LogManager                                       ' ���O�Ǘ��N���X
    Private mcls_LogEx As tClass_LogManager                                     ' ���O�Ǘ��N���X(�d�v)
    Private mcls_Param As clsParamManager                                       ' �p�����[�^���Ǘ�
    Private mcls_PreParam As clsPreParamManager                                 ' �O�H���p�����[�^�Ǘ��N���X

    Private WithEvents m_lstTansho As List(Of frmTansho)                        ' �T�����
    Private m_nSyncMode As EM_SYNC_MODE                                         ' ��ʂ̓������[�h

    Private WithEvents m_timGetMail As New Timer                                ' ���[���X���b�g�p�X���b�h�����ʃX���b�h�ւ̋��n��
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)                   ' ���[���X���b�g�̃L���[

    Private WithEvents m_timBrink As New Timer                                  '�u�����N�\���^�C�}�[

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�o�b�O���O�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private gcls_frmDebugLog As Form
    Private lbxLog As ListBox
    Private lbxLogEx As ListBox

    Private Sub frmDebugLog_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs)
        'If e.CloseReason <> CloseReason.WindowsShutDown Then
        '    e.Cancel = True
        '    CType(sender, Form).Hide()
        'End If
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
    ''' <remarks>��Main����</remarks>
    Public Function Main() As Integer
        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        'Windows�V���b�g�_�E���A�ċN���ʒm�擾�C�x���g
        AddHandler SystemEvents.SessionEnding, AddressOf SystemEvents_SessionEnding

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll �� �m�F���ĉ������B" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("�^�X�N���� ���s"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Function
        End Try

        Try
            '-------------------------------------------------------------------
            'KizuTask.ini�̎擾
            '-------------------------------------------------------------------
            '�o�͑���ʐ�
            Dim nFormTotalNum As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "FROM_TOTAL_NUM", TASKINI_NAME, 1)
            '�r�}�b�v�X�V�b��
            Dim nMapInterval As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "MAP_INTERVAL", TASKINI_NAME, 1000)
            '�X�e�[�^�X�X�V�b��
            Dim nStatusInterval As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "STATUS_INTERVAL", TASKINI_NAME, 500)
            '�R�C���X�V�b��
            Dim nCoilInterval As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "COIL_INTERVAL", TASKINI_NAME, 1000)
            '�\���r�摜��`
            Dim nImageWidth As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "IMAGE_WIDTH", TASKINI_NAME, 25)
            If nImageWidth <= 0 Then
                nImageWidth = 25
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�\���r�摜��`�ُ�[{0}]", nImageWidth))
            End If
            '�\���r�摜��`
            Dim nTcmImageWidth As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "TCM_IMAGE_WIDTH", TASKINI_NAME, 25)
            If nTcmImageWidth <= 0 Then
                nTcmImageWidth = 25
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM�\���r�摜��`�ُ�[{0}]", nTcmImageWidth))
            End If
            '�������[�h�ݒ�
            m_nSyncMode = CType(tMod_IniFile.ReadIniInt(HY_TANSHO, "SYNC_MODE", TASKINI_NAME, 2), EM_SYNC_MODE)
            If m_nSyncMode <> EM_SYNC_MODE.ALL And m_nSyncMode <> EM_SYNC_MODE.BY_DSP And m_nSyncMode <> EM_SYNC_MODE.NONE Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�������[�h�ݒ�ُ�[{0}]", m_nSyncMode))
                m_nSyncMode = EM_SYNC_MODE.NONE
            End If

            Dim nWk As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "SYNC_MODE_UMU", TASKINI_NAME, 0)
            Dim bSyncUmu As Boolean = False
            If 1 = nWk And nFormTotalNum > 1 Then
                If 1 = nFormTotalNum Then
                    Log(LNO.MSG, String.Format("modeMain �������[�h�L�������ύX(�o�͑���ʐ�=1�̂���) ini��`={0}", nWk))
                    bSyncUmu = False
                Else
                    bSyncUmu = True
                End If
            Else
                bSyncUmu = False
            End If
            '�ꎞ��~����n�R�C�����ؑւ�ŁA�����I�Ɉꎞ��~�����B
            Dim nTempStopN As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "TEMP_STOP_CANCEL", TASKINI_NAME, DSP_IDX_OLD_MAX)
            If nTempStopN < 1 Then
                nTempStopN = 1
                Log(LNO.ERR, String.Format("modeMain �����I�Ɉꎞ��~��������R�C���{����`�ُ�1[{0}]", nTempStopN))
            End If
            If nTempStopN > DSP_IDX_OLD_MAX Then
                nTempStopN = DSP_IDX_OLD_MAX
                Log(LNO.ERR, String.Format("modeMain �����I�Ɉꎞ��~��������R�C���{����`�ُ�2[{0}]", nTempStopN))
            End If

            '-------------------------------------------------------------------
            ' �p�����[�^���ǂݏo��
            '-------------------------------------------------------------------
            mcls_Param = New clsParamManager(mcls_LogEx)
            While (Not mcls_Param.ReadAllParam())                               '�p�����[�^�ǂݏo����������܂ŔS��
                Application.DoEvents()
                Threading.Thread.Sleep(1000)
            End While

            mcls_PreParam = New clsPreParamManager(mcls_LogEx)                    ' �O�H���p�����[�^�Ǘ�
            While (mcls_PreParam.ReadAll() = False)                             ' �p�����[�^�ǂݏo����������܂ŔS��
                Threading.Thread.Sleep(1000)
            End While

            '-------------------------------------------------------------------
            ' �T����ʕ\��
            '-------------------------------------------------------------------
            m_lstTansho = New List(Of frmTansho)
            For nFormID As Integer = 1 To nFormTotalNum
                Dim clsTansho As frmTansho = New frmTansho(nFormID, mcls_Log, mcls_Param, mcls_PreParam)
                clsTansho.MapInterval = nMapInterval                            '�r�}�b�v���X�V�Ԋu�ݒ�
                clsTansho.StatusInterval = nStatusInterval                      '�X�e�[�^�X���X�V�Ԋu�ݒ�
                clsTansho.CoilInterval = nCoilInterval                          '�R�C�����X�V�Ԋu�ݒ�
                If NOA_ONLINE = g_NOA Then
                    '�I�����C����
                    clsTansho.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iONLINE
                Else
                    '���u��&�f�o�b�O��
                    '�t�H�[�����
                    clsTansho.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iOFFLINE

                End If
                m_lstTansho.Add(clsTansho)
                clsTansho.SetImageWidth(nImageWidth)
                clsTansho.SetTcmImageWidth(nTcmImageWidth)
                '�T����ʕ\��
                clsTansho.Show()
                '�C�x���g�ݒ�
                '�X�N���[�������ύX
                AddHandler clsTansho.ev_ChangeScrollSync, AddressOf frmTansho_ChangeScrollSync
                '�X�N���[���l�ύX
                AddHandler clsTansho.ev_ChangeScrollValue, AddressOf frmTansho_ChangeScrollValue
            Next
            m_lstTansho(0).Focus()

            '-------------------------------------------------------------------
            '���C���X���b�h�N��
            '-------------------------------------------------------------------
            '�u�����N�\���^�C�}�[
            m_timBrink.Interval = 1000
            m_timBrink.Start()

            '���[���X���b�g��M�Ď��̊J�n
            m_timGetMail.Interval = 500
            m_timGetMail.Start()
            m_ap = New ApplicationContext
            Application.Run(m_ap)                                               'ExitThread���R�[�������܂� �����Ńu���b�N
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("modeMain ���C�������ُ�[{0}]", ex.Message))
        End Try


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

        ' �O����
        '' ini�t�@�C����� �萔�I�ȃO���[�o�[���ϐ��ǂݍ���
        g_NOA = tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)

        ' ��ʎ󂯓n���p�L���[������
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '''' �^�X�N������
        KizuLibCLI.KizuMod.SetLineID()
        '�^�X�N�C�j�V���� (DB�ւ̃A�N�Z�X�͖���)
        If 0 <> mcls_Mod.Task_Init(HY_TANSHO) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", HY_TANSHO))
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

        mcls_LogEx.Write(LNO.MSG, "�T����ʋN��")
        LogEx(LNO.MSG, String.Format("���O(�d�v)�J�n"))
        Log(LNO.MSG, String.Format("���O�J�n"))

    End Sub

    ''' <summary>
    ''' �^�X�N�I������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_Last()

        Try
            If Not m_timBrink Is Nothing Then
                m_timBrink.Stop()                                               '�����X�N���[���^�C�}�[��~
            End If
            If m_timGetMail Is Nothing = False Then
                m_timGetMail.Stop()                                             ' ���[���X���b�g�Ď���~
            End If

            If m_MailQue Is Nothing = False Then                                ' ���[���X���b�g�L���[�폜
                m_MailQue.Dispose()
                m_MailQue = Nothing
            End If

            If Not m_lstTansho Is Nothing Then                                  '�T����ʍ쐬�̏ꍇ�͉��
                For Each clsTansho As frmTansho In m_lstTansho
                    clsTansho.Close()
                    clsTansho.Dispose()
                    clsTansho = Nothing
                Next
            End If

            If Not mcls_Param Is Nothing Then                                   '�p�����[�^�}�l�[�W���̉��
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If

            If Not mcls_PreParam Is Nothing Then                                '�p�����[�^�}�l�[�W���̉��
                mcls_PreParam.Dispose()
                mcls_PreParam = Nothing
            End If

        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("{0} [{1}]", "���C�������I���ُ�", ex.Message))
        End Try

        RemoveHandler SystemEvents.SessionEnding,
           AddressOf SystemEvents_SessionEnding                                 ' Windows�V���b�g�_�E���A�ċN���ʒm�擾�C�x���g


        mcls_LogEx.Write(LNO.MSG, "�T����ʏI��")
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
                Case FACT_HY_TANSHO_01                      '�p�����[�^�ύX�ʒm
                    Log(LNO.MSG, "�p�����[�^�ύX�ʒm")
                    LogEx(LNO.MSG, "�p�����[�^�ύX�ʒm")
                    mcls_Param.ReadAllParam()
                    mcls_PreParam.ReadAll()
                Case FACT_HY_TANSHO_02                      '�C�j�V�����ʒm
                    Log(LNO.MSG, "�C�j�V�����ʒm")
                    LogEx(LNO.MSG, "�C�j�V�����ʒm")
                    mcls_Param.ReadAllParam()
                    mcls_PreParam.ReadAll()
                    For ii As Integer = 0 To m_lstTansho.Count - 1
                        If Not m_lstTansho(ii) Is Nothing Then
                            m_lstTansho(ii).DspInit()
                        End If
                    Next
                Case FACT_KS_DB_STATEUS                     'DB��ԕύX�ʒm
                    Log(LNO.MSG, "DB��ԕύX�ʒm")
                    LogEx(LNO.MSG, "DB��ԕύX�ʒm")
                    Dim bDBStatus As Boolean = False
                    If que_l.data(0) = 0 Then               'DB����
                        bDBStatus = True
                    End If
                    For ii As Integer = 0 To m_lstTansho.Count - 1
                        If Not m_lstTansho(ii) Is Nothing Then
                            m_lstTansho(ii).DBStatus = bDBStatus
                        End If
                    Next

                Case FACT_KS_PCNAME_CHANGE                  'PC���̕ύX�ʒm
                    Log(LNO.MSG, "PC���̕ύX�ʒm")
                    LogEx(LNO.MSG, "PC���̕ύX�ʒm")

                Case FACT_KS_END                            '�^�X�N�I���ʒm
                    mcls_Log.Write(tClass_LogManager.LNO.MSG, "�^�X�N�I���ʒm")
                    mcls_LogEx.Write(tClass_LogManager.LNO.MSG, "�^�X�N�I���ʒm")
                    For ii As Integer = 0 To m_lstTansho.Count - 1
                        If Not m_lstTansho(ii) Is Nothing Then
                            m_lstTansho(ii).frmTansho_Disposed()
                        End If
                    Next
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
            mcls_LogEx.Write(LNO.MSG, "Windows�V���b�g�_�E���A�ċN���ʒm")
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
    ''' �X�N���[���l�ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="nValue">�X�N���[����</param>
    ''' <remarks></remarks>
    Public Sub frmTansho_ChangeScrollValue(ByVal sender As frmTansho, ByVal nValue As Integer, ByVal emScrool As EM_MAP_SCROOL)
        Try
            Dim nDspPos As Integer = sender.KensaNo
            For Each frm As frmTansho In m_lstTansho
                If frm.Equals(sender) Then                                      '�C�x���g�������̏ꍇ�͔�����
                    Continue For
                End If

                If m_lstTansho(0).Stage = m_lstTansho(1).Stage Then
                    If frm.ScrollSync Then
                        Select Case m_nSyncMode                                     '�������[�h���ɐݒ肷��
                            Case EM_SYNC_MODE.ALL                                   '�S��ʓ������[�h
                                frm.SetScrollValue(nValue, emScrool)
                            Case EM_SYNC_MODE.BY_DSP                                '�����䖈���[�h
                                If frm.KensaNo = nDspPos Then
                                    frm.SetScrollValue(nValue, emScrool)
                                End If
                        End Select
                    End If
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�N���[���l�ύX�ُ�[{0}],[{1}]", nValue, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �X�N���[�������ύX
    ''' </summary>
    ''' <param name="sender">�ύX�̂������T�����</param>
    ''' <param name="bSync">�ύX��̃��[�h True:���� False:�񓯊�</param>
    ''' <remarks></remarks>
    Public Sub frmTansho_ChangeScrollSync(ByVal sender As frmTansho, ByVal bSync As Boolean)
        Try
            Dim nDspPos As Integer = sender.KensaNo
            For Each frm As frmTansho In m_lstTansho
                If frm.Equals(sender) Then                                      '�C�x���g�������̏ꍇ�͔�����
                    Continue For
                End If
                Select Case m_nSyncMode                                         '�������[�h���ɐݒ肷��
                    Case EM_SYNC_MODE.ALL                                       '�S��ʓ������[�h
                        frm.ScrollSync = bSync
                    Case EM_SYNC_MODE.BY_DSP                                    '�����䖈���[�h
                        If frm.KensaNo = nDspPos Then
                            frm.ScrollSync = bSync
                        End If
                End Select
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�N���[�������ύX�ُ�[{0}],[{1}]", bSync, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �u�����N�\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_timBrink_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timBrink.Tick
        Try
            For Each frm As frmTansho In m_lstTansho
                frm.m_timBrink_Tick()
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�u�����N�\���ُ�[{0}]", ex.Message))
        End Try
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
