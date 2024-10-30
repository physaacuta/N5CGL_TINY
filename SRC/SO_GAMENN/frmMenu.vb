'*********************************************************************************
' ���j���[���
'	[Ver]
'		Ver.01    2012/05/23  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

''' <summary>
''' ���j���[���
''' </summary>
''' <remarks></remarks>
Public Class frmMenu

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DISP_TOP As Boolean = True    '�\���ʒu�őO��
    Private Const m_sTFApp_Path As String = ".\HY_TANSHO.exe"           ' �T����ʋN���A�v��
    Private Const DEFAULT_GAMEN_SIZE_X As Integer = 1280
    Private Const DEFAULT_GAMEN_SIZE_Y As Integer = 1024


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���[�J���ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents frm_DbCheck As KizuForm.frmPop_DbCheck           'DB�`�F�b�NPopUp�N���X
    Public frm_SystemStatus As frmSystemStatus                          '������~�J�n�ݒ���

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    Public Sub New()

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Dim clsMonitor() As Screen = Screen.AllScreens()                    '���j�^�[�ꗗ

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        ' �_�~�[��ʂ̋N��
        Dim bDmySet As Integer = tMod.ReadIniInt(SO_GAMENN, "DMY_SET", TASKINI_NAME, 0)
        If 2 = bDmySet Then                                             ' �_�~�[��ʂ�\��
            Call DummyShow(clsMonitor.Length)
        ElseIf 0 = bDmySet AndAlso NOA_ONLINE = g_NOA Then              ' NOA�t���O���g�p �I�����C��
            Call DummyShow(clsMonitor.Length)
        End If

        '�I�y���[�^��ʋN���ǉ�
        Dim bValDispTop As Boolean = CBool(tMod.ReadIniInt(SO_GAMENN, "DISP_TOP_FLG", TASKINI_NAME, 0))
        '-------------------------------------------------------------------
        ' �őO��   ��ʕ\���ݒ�
        '-------------------------------------------------------------------
        If DISP_TOP = bValDispTop Then      '�őO�ʕ\���iDISP_TOP_FLG=1�j
            Me.TopMost = True
            If g_bOutTrk Then
                MyBase.SetLocationForm(g_typStartArgs.nFormPos, False)
                g_nFormPos = g_typStartArgs.nFormPos
            End If
        Else                                '�ʏ�\��(DISP_TOP_FLG=1�ȊO)
            Me.TopMost = False
        End If

        ' �I�����C�������u��
        If NOA_ONLINE = g_NOA Then
            '''' �t�@���N�V�����L�[��`
            ' �T�����
            ' �����ʂ̏ꍇ�͒T����ʃ{�^���͖����ɂ���K�v���L�邪�A�I�����C�������ł͔���ł��Ȃ�
            ' HY_TANSHO���N�����Ă��邩�ǂ����Ŕ���ł���΂����̂����A������@��������Ȃ��B
            ' ���s�t�@�C�� HY_TANSHO.EXE �����݂��邩�ǂ����Ŕ��肷�邱�ƂƂ���B
            Dim appPath As String = System.Reflection.Assembly.GetExecutingAssembly().Location
            appPath = IO.Path.GetDirectoryName(appPath)
            Dim strTanshoPath As String = IO.Path.Combine(appPath, "HY_TANSHO.EXE")
            If System.IO.File.Exists(strTanshoPath) Then
                tProp_MenuEnable(0) = True
                my_btnFunction(0).Enabled = True
            Else
                tProp_MenuEnable(0) = False
                my_btnFunction(0).Enabled = False
            End If

            '�őO�ʃ��[�h�ł���΁A�I���{�^�����g�p�\�ɂ���B
            '�T����ʂ���̋N���p
            'If DISP_TOP = bValDispTop Then      '      '�őO�ʕ\���iDISP_TOP_FLG=1�j
            '    ' �I���{�^��(�g�p�\)
            '    tProp_MenuEnable(11) = True
            '    my_btnFunction(11).Enabled = True
            'Else
            ' �I���{�^��(�g�p�s��)
            tProp_MenuEnable(11) = False
            my_btnFunction(11).Enabled = False
            'End If

        ElseIf NOA_DEBUG = g_NOA Then

            '�f�o�b�O��
            ' �t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            '''' �t�@���N�V�����L�[��`
            ' �T�����
            tProp_MenuEnable(0) = True
            my_btnFunction(0).Enabled = True

            ' �I���{�^��
            tProp_MenuEnable(11) = True
            my_btnFunction(11).Enabled = True

        ElseIf NOA_KAISEKI = g_NOA Then

            '��͎�
            ' �t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE

            '''' �t�@���N�V�����L�[��`
            ' �T�����
            tProp_MenuEnable(0) = False
            my_btnFunction(0).Enabled = False

            ' �I���{�^��
            tProp_MenuEnable(11) = True
            my_btnFunction(11).Enabled = True

        Else

            '���u��
            ' �t�H�[�����
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            'Me.tProp_EndButtonEnable = False	'���j���[��ʂ����� �~�{�^���L��Ƃ���B

            '''' �t�@���N�V�����L�[��`
            ' �T�����
            tProp_MenuEnable(0) = False
            my_btnFunction(0).Enabled = False
            ' �����J�n��~�{�^��
            tProp_MenuEnable(10) = False
            my_btnFunction(10).Enabled = False
            ' �I���{�^��
            tProp_MenuEnable(11) = True
            my_btnFunction(11).Enabled = True

        End If

        If clsMonitor.Length < g_nFormPos Then                              '�\�����������j�^�[���Ȃ��ꍇ
            g_nFormPos = 1                                                  '�v���C�}���f�B�X�v���C�ɕ\��
        End If
        Dim nH As Integer = clsMonitor(g_nFormPos - 1).Bounds.Size.Height
        Dim nW As Integer = clsMonitor(g_nFormPos - 1).Bounds.Size.Width

        If DEFAULT_GAMEN_SIZE_Y < nH Then
            nH = DEFAULT_GAMEN_SIZE_Y
        End If
        If DEFAULT_GAMEN_SIZE_X < nW Then
            nW = DEFAULT_GAMEN_SIZE_X
        End If
        Me.Location = clsMonitor(g_nFormPos - 1).Bounds.Location
        Me.Width = nW
        Me.Height = nH

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h���� (frmBase����̃I�[�o�[���C�h)
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        ' DB�ڑ��m�F��ʕ\��
        frm_DbCheck = New KizuForm.frmPop_DbCheck(INI_DB, g_NOA)
        frm_DbCheck.ShowDialog()

        '���̏������Y�ꂸ�Ɏ��s()
        MyBase.LoadEnding()
    End Sub

    ''' <summary>
    ''' ��� ���悤�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' ���C���X���b�h�I���ʒm
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' DB�ڑ��`�F�N�����C�x���g
    ''' </summary>
    ''' <param name="dbMode">DB�ڑ����</param>
    ''' <remarks></remarks>
    Private Sub frm_DbCheck_tEv_FunctionButton(ByVal dbMode As Boolean) Handles frm_DbCheck.tEv_FunctionButton
        ' �t�H�[���̎n��
        frm_DbCheck.Dispose()
        frm_DbCheck = Nothing

        If Not dbMode Then              ' �q����Ȃ��āA�I���{�^����������
            ' ���C���X���b�h�I���ʒm
            modMain.TaskEnd()
        End If
    End Sub

    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmMenu_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �����I���͈�ԍŏ��Ɏ�t
        If 11 = FncNo AndAlso onKey = Keys.Shift Then
            If MsgBox("�����I�����܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question Or MsgBoxStyle.DefaultButton2, "�d�v�Ȃ��m�点") = MsgBoxResult.Yes Then
                Me.Close()
                'modMain.TaskEnd()						' �I�t���C�����́~�{�^���΍�Ƃ��� Closing�ŏ�������
            End If
        End If

        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub
        Try
            '' �t�@���N�V�����ؑ�
            Select Case FncNo                           ' 0�I���W��
                Case 1 - 1                              ' F01 �T�����
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.INF, String.Format("�T����� �N���v��!!"))
                    '�T����ʃ^�X�N�N��
                    If TanshoStart() = True Then
                        If Not frmDummy Is Nothing Then frmDummy.Dispose()
                        frmDummy = Nothing
                        Me.Close()
                    Else
                        MsgBox("�T����ʂ��N���o���܂���ł���", MsgBoxStyle.OkOnly Or MsgBoxStyle.Exclamation)
                    End If

                    '//-----------------------------------------------------------
                Case 2 - 1                              ' F02 

                    '//-----------------------------------------------------------
                Case 3 - 1                              ' F03 �ŐV�R�C�����\�����
                    Dim frm As New frmDspCoil()
                    frm.ShowDialog()
                    frm.Dispose()
                    frm = Nothing

                    '//-----------------------------------------------------------
                Case 4 - 1                              ' F04 �o���P���r������CSV�o�͉��
                    Dim frm As New frmPop_Dekon()
                    frm.ShowDialog()
                    frm.SaveCheck()                     ' �����t�B���^�[��ini�t�@�C���ۑ�
                    frm.Dispose()
                    frm = Nothing

                    '//-----------------------------------------------------------
                Case 5 - 1                              ' F05 ���щ��
                    Dim frm As New frmJisseki()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 6 - 1                              ' F06 

                    '//-----------------------------------------------------------
                Case 7 - 1                              ' F07 �@���ԃg�����h���
                    Dim frm As New frmKikiTrand
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 8 - 1                              ' F08 ������ԃ��O���
                    Dim frm As New frmStatusLog()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 9 - 1                              ' F09 �̏Ⴭ�O���
                    Dim frm As New frmLog()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 10 - 1                             ' F10 RAS�\�����
                    Dim frm As New frmRas()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 11 - 1                             ' F11 �����J�n��~������
                    frm_SystemStatus = New frmSystemStatus
                    frm_SystemStatus.ShowDialog()
                    frm_SystemStatus.Dispose()
                    Me.BringToFront()

                    '//-----------------------------------------------------------
                Case 12 - 1                             ' �I��
                    If Not frmDummy Is Nothing Then frmDummy.Dispose()
                    frmDummy = Nothing
                    Me.Close()
                    'modMain.TaskEnd()						' �I�t���C�����́~�{�^���΍�Ƃ��� Closing�ŏ�������
            End Select
        Catch ex As Exception

        End Try


    End Sub

    ''' <summary>
    ''' �T����ʋN������
    ''' </summary>
    ''' <remarks></remarks>
    Private Function TanshoStart() As Boolean
        Dim bRet As Boolean = True

        Try
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("HY_TANSHO[{0}] �N�������J�n", m_sTFApp_Path))
            Shell(m_sTFApp_Path, AppWinStyle.NormalFocus, False)    ' HY_TANSHO �N��
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("HY_TANSHO[{0}] �N���������� �I�����܂�", m_sTFApp_Path))
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("HY_TANSHO �N�������ُ� [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �����ʋN������
    ''' </summary>
    ''' <param name="strPath">���s�t�H���_�p�X</param>
    ''' <param name="strExe">���s�t�@�C����</param>
    ''' <returns></returns>
    Private Function SousaStart(ByVal strPath As String, ByVal strExe As String) As Boolean
        Dim bRet As Boolean = True
        Dim strSrcPath As String = System.IO.Directory.GetCurrentDirectory()
        Try
            gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("SO_GAMENN[{0}] �N�������J�n", strPath + strExe))

            ' ��ƃf�B���N�g����ύX
            System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetFullPath(strPath))
            ' SO_GAMENN �N��
            Shell(strPath + strExe, AppWinStyle.NormalFocus, False)

            gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("SO_GAMENN[{0}] �N���������� �I�����܂�", strPath + strExe))
        Catch ex As Exception
            ' ���s�������ƃf�B���N�g����߂�
            System.IO.Directory.SetCurrentDirectory(strSrcPath)
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("SO_GAMENN �N�������ُ� [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �_�~�[��ʋN������
    ''' </summary>
    ''' <param name="nCnt">���j�^��</param>
    Private Sub DummyShow(ByVal nCnt As Integer)

        ' ���j�^�����P��ʈȉ��̏ꍇ�̓_�~�[��ʂ͕\�����Ȃ�
        If 1 >= nCnt Then Exit Sub

        ' ���j�^�������[�v
        For nMonitorNo As Integer = 1 To nCnt
            ' Dummy��ʋN��
            Dim frm As New frmDummy()
            frm.Show(nMonitorNo, True)
        Next

    End Sub

End Class
