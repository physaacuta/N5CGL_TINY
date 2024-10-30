'*********************************************************************************
'�@�^�X�N�Ď�
'	[Ver]
'		Ver.01    2015/01/27  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices
Imports System.Drawing


Public Class frmMain
    '********************************************************************
    'API��`
    '********************************************************************
    <DllImport("user32.dll", SetLastError:=True)> _
    Private Shared Function ShowWindow( _
    ByVal hWnd As System.IntPtr, ByVal style As Integer) As Integer
    End Function

    <DllImport("user32.dll", SetLastError:=True)> _
    Private Shared Function SetForegroundWindow( _
    ByVal hWnd As System.IntPtr) As Integer
    End Function



    Private Const SW_HIDE As Integer = 0
    Private Const SW_SHOWNORMAL As Integer = 1
    Private Const SW_NORMAL As Integer = 1
    Private Const SW_SHOWMINIMIZED As Integer = 2
    Private Const SW_SHOWMAXIMIZED As Integer = 3
    Private Const SW_MAXIMIZE As Integer = 3
    Private Const SW_SHOWNOACTIVATE As Integer = 4
    Private Const SW_SHOW As Integer = 5
    Private Const SW_MINIMIZE As Integer = 6
    Private Const SW_SHOWMINNOACTIVE As Integer = 7
    Private Const SW_SHOWNA As Integer = 8
    Private Const SW_RESTORE As Integer = 9
    Private Const SW_SHOWDEFAULT As Integer = 10
    Private Const SW_FORCEMINIMIZE As Integer = 11
    Private Const SW_MAX As Integer = 11

    '---------------------------------------------------------------------------
    ' �萔��`
    '---------------------------------------------------------------------------
    Private Const m_ini As String = ".\KS_APLIST.ini"
    Private Const KS_APLIST As String = "KS_APLIST"

    Private Const MAX_EXECUTE_TASK As Integer = 50

    '---------------------------------------------------------------------------
    ' �萔��`
    '---------------------------------------------------------------------------
    Public Structure BASE_INFO
        Dim strName As String
        Dim strIniPath As String
        Dim strTask() As String
        Public Sub Inisialize()
            strName = ""
            strIniPath = ""
            ReDim strTask(10 - 1)
        End Sub
    End Structure

    Public Structure TASK_INFO
        Dim strLineName As String
        Dim strTaskName As String
    End Structure

    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DISP_DATA
        emNo = 0                    ' No
        emLineName                  ' ���C������
        emTaskName                  ' �^�X�N����
        emDspBtn                    ' �\���{�^��
        emStatus                    ' �^�X�N���(��\��)
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                            ' ���O�Ǘ��N���X

    Private m_TimerCount As Integer                                         ' �Ď��������[sec]

    Private WithEvents m_nfyIcon As System.Windows.Forms.NotifyIcon         ' �^�X�N�g���C�̃A�C�R�� �R���g���[��
    Private m_icNomal As Icon                                               ' �ʏ�A�C�R��(���펞)
    Private m_icNG As Icon                                                  ' �ʏ�A�C�R��(�ُ펞)

    Private m_lstBase As New List(Of BASE_INFO)                             ' �Ď��Ώۃ��X�g
    Private m_lstDspInfo As New List(Of TASK_INFO)                          ' �Ď��^�X�N�ꗗ


    Private m_nStatus() As Integer                                          ' �O��l�ۑ����[�N


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�֐�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
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
    Private Sub frmApList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        '��d�N���`�F�b�N
        If Process.GetProcessesByName(Process.GetCurrentProcess.ProcessName).Length > 1 Then
            MsgBox("���ɋN�����ł��B")
            End
        End If

        '���O�N���X����
        mcls_Log = New tClass.tClass_LogManager(KS_APLIST, Nothing, Nothing)

        mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�Ď��c�[���N��")
        '' �~�{�^���𖳌��ɂ���
        tMod.subFormEndButton_Desebled(Me.Handle)

        '' �^�X�N�g���C�̃A�C�R���擾
        Try                                                         ' �A�C�R���ݒ�
            '' ���ߍ��܂ꂽ���\�[�X����A�C�R�������o��
            Dim MyAsm As Reflection.Assembly = Reflection.Assembly.GetExecutingAssembly()
            m_icNomal = New Icon(MyAsm.GetManifestResourceStream("KS_APLIST.KS_APLIST.ico"))
            m_icNG = New Icon(MyAsm.GetManifestResourceStream("KS_APLIST.KS_APLIST_NG.ico"))
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�A�C�R���ݒ�G���[ [" & ex.Message & "]")
        End Try

        '' �^�X�N�g���C�̃A�C�R�� �R���g���[�� �C���X�^���X����
        m_nfyIcon = New System.Windows.Forms.NotifyIcon
        m_nfyIcon.ContextMenu = CreateNotifyIconMenu_Def()      '�^�X�N�g���C�̃A�C�R�����j���[��`
        m_nfyIcon.Text = "�^�X�N�Ď�(KS_APLIST)"
        IconChange(True)    ' �ʏ�A�C�R���Z�b�g
        m_nfyIcon.Visible = True

        '������
        m_lstBase.Clear()
        m_lstDspInfo.Clear()

        '�ݒ�t�@�C���ǂݍ���
        GetIniFile()

        '�Ď��Ώۃ^�X�N�̈ꗗ���쐬
        MakeTargetTaskList()

        '�f�[�^�O���b�h�r���[������
        dgvInitialize(dgvTask)

        '����\��
        InitDataDisp()

        '��ԕ\��
        DataDisp()

        ' �^�C�}�[�X�^�[�g
        tmrWatch.Interval = m_TimerCount * 1000
        tmrWatch.Start()

        'Me.Visible = False
        'Me.ShowInTaskbar = False
    End Sub

    ''' <summary>
    ''' ��\���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEnd.Click
        mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�~�{�^������")
        Me.Hide()
        'If MsgBox("�I�����܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question Or MsgBoxStyle.DefaultButton2, "�d�v�Ȃ��m�点") = MsgBoxResult.Yes Then
        '	Call SetEventExit()
        '	' �^�X�N�I���ʒm���M
        '	Me.Close()
        'End If
    End Sub

    ''' <summary>
    ''' �I������
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetEventExit()
        tmrWatch.Stop()
        m_nfyIcon.Visible = False
        m_nfyIcon.Dispose()
        mcls_Log.Dispose()
        mcls_Log = Nothing
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �ݒ�t�@�C���Ǎ��֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' INI�t�@�C���Ǎ�(Base)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetIniFile()
        '�Ď�����
        m_TimerCount = tMod.ReadIniInt(KS_APLIST, "CHECK_TIMER_COUNT", m_ini, 10)
        '�ΏۃZ�N�V����
        For ii As Integer = 0 To 10 - 1
            Dim strKey As String = ""
            strKey = String.Format("TARGET_SECTION_{0}", ii + 1)
            Dim strSection As String = tMod.ReadIniStr(KS_APLIST, strKey, m_ini, "")
            If "" = strSection Then
                Exit For
            Else
                GetIniFile_Target(strSection)
            End If
        Next

    End Sub

    ''' <summary>
    ''' INI�t�@�C���Ǎ�(��)
    ''' </summary>
    ''' <param name="strSection">�Z�N�V����</param>
    ''' <remarks></remarks>
    Private Sub GetIniFile_Target(ByVal strSection As String)
        Dim typInfo As BASE_INFO = Nothing
        typInfo.Inisialize()
        typInfo.strName = tMod.ReadIniStr(strSection, "NAME", m_ini, "")
        typInfo.strIniPath = tMod.ReadIniStr(strSection, "INI_PATH", m_ini, "")
        For ii As Integer = 0 To 10 - 1
            Dim strKey As String = ""
            strKey = String.Format("TASK_{0}", ii + 1)
            Dim cWk As String = tMod.ReadIniStr(strSection, strKey, m_ini, "")
            If "" = cWk Then
                Exit For
            Else
                typInfo.strTask(ii) = cWk
            End If
        Next
        m_lstBase.Add(typInfo)

    End Sub

    ''' <summary>
    ''' �Ď��Ώۃ^�X�N�̈ꗗ���쐬����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeTargetTaskList()
        m_lstDspInfo.Clear()
        For Each data As BASE_INFO In m_lstBase
            Dim dt As TASK_INFO

            'KizuTask.ini�̋N���^�X�N��o�^
            For ii As Integer = 0 To MAX_EXECUTE_TASK - 1
                Dim strKey As String = ""
                strKey = String.Format("TASK{0}", ii + 1)
                Dim strWk As String = tMod.ReadIniStr("EXECUTE", strKey, data.strIniPath, "")
                If "" = strWk Then
                    Exit For
                End If
                Dim strAry() As String = strWk.Split(CChar(","))
                dt.strLineName = data.strName
                '#=========================================================
                '# �v���Z�X��� (1�`50�܂�)
                '#  TASK1	: �N���^�X�N��, �N���v���Z�X(0:�ʏ� 1:�ŏ��� 2:�ő剻 3:��\��), �z���g�̋N��EXE����(�N���^�X�N���Ɠ����Ȃ疳����OK)
                '#=========================================================
                If strAry.Length > 2 Then
                    dt.strTaskName = Trim(strAry(2))
                Else
                    dt.strTaskName = Trim(strAry(0))
                End If
                m_lstDspInfo.Add(dt)
            Next ii
            '�ʎw��̋N���^�X�N��o�^
            For ii As Integer = 0 To data.strTask.Length - 1
                If "" = data.strTask(ii) Then
                    Exit For
                End If
                dt.strLineName = data.strName
                dt.strTaskName = data.strTask(ii)
                m_lstDspInfo.Add(dt)
            Next ii
        Next
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �^�X�N�g���C�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �^�X�N�g���C�̃A�C�R�����j���[��`
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CreateNotifyIconMenu_Def() As ContextMenu
        '' ���j���[���ڂ̐���
        Dim closeItem1 As New MenuItem("�c�[���I��", AddressOf m_menuItem_Click)
        '' ���s�ݒ�
        closeItem1.BarBreak = True  '1�ŉ��s

        '' �R���e�L�X�g���j���[��`
        Dim menu As New ContextMenu
        menu.MenuItems.Add(closeItem1)
        Return menu
    End Function

    ''' <summary>
    ''' �^�X�N�g���C�̃A�C�R�����j���[�N���b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_menuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs)
        ' ���j���[�ɂ�菈������
        Dim menu As MenuItem = CType(sender, MenuItem)
        Select Case menu.Index
            Case 0                                  ' 
                If MsgBox("�I�����܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.Question Or MsgBoxStyle.DefaultButton2, "�d�v�Ȃ��m�点") = MsgBoxResult.Yes Then
                    mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�c�[���I���I��")
                    Call SetEventExit()
                    ' �^�X�N�I���ʒm���M
                    Me.Close()
                End If
        End Select

    End Sub

    ''' <summary>
    ''' �^�X�N�g���C�A�C�R�� �_�u���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nfyIcon_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_nfyIcon.DoubleClick
        'mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "�^�X�N�g���C�A�C�R�� �_�u���N���b�N")

        '' �t�H�[���\�����Ȃ����
        If Me.Visible Then
            Me.Hide()
        Else
            '' �t�H�[���\��
            Me.Show()
            Me.TopMost = True   '�ŏ��̂ݍőO�ʂɏo��
            Me.TopMost = False
        End If
    End Sub

    ''' <summary>
    ''' �^�X�N�g���C�̃A�C�R���ύX
    ''' </summary>
    ''' <param name="bNomal">true:����  false:�ُ�</param>
    ''' <remarks></remarks>
    Public Sub IconChange(ByVal bNomal As Boolean)
        If bNomal Then
            m_nfyIcon.Icon = m_icNomal
        Else
            m_nfyIcon.Icon = m_icNG
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �^�C�}�[�֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �Ď��^�C�}�[����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrWatch_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrWatch.Tick
        '��ԕ\��
        DataDisp()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As DataGridView)
        '' ��{��ݒ�
        'Call dgv.dgvInitialize(dgv)
        Call DataGridViewSelectionCancel(dgv)                           ' �I��F����

        ' �Ǝ��̍��ڂ�ݒ�
        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                                ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                                         ' �Z���G���[�̕\��
            .ShowCellToolTips = False                                       ' �c�[���q���g�̕\��
            .ShowEditingIcon = False                                        ' �ҏW���̃Z���̖���
            .ShowRowErrors = False                                          ' �G���[�s�̖���
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��
            '.ColumnHeadersHeight = 42										' �w�b�_�[�̍���

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                               ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            .RowCount = 0


            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 24                                       ' �w�b�_�[�̍���

            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "No"
                clm.Width = 26
                clm.ReadOnly = True
                clm.Frozen = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns.Add(clm)
            End Using

            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "Name"
                clm.Width = 60
                clm.ReadOnly = True
                clm.Frozen = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns.Add(clm)
            End Using

            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "�^�X�N����"
                clm.Width = 88
                clm.ReadOnly = True
                clm.Frozen = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns.Add(clm)
            End Using

            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = ""
                clm.Width = 40
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.DefaultCellStyle.BackColor = Color.LightGray

                clm.DefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                Dim insBtnText As DataGridViewButtonColumn = CType(clm, DataGridViewButtonColumn)
                insBtnText.UseColumnTextForButtonValue = True
                insBtnText.Text = "�\��"
                .Columns.Add(clm)
            End Using

            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "���"
                clm.Width = 0
                clm.Visible = False
                clm.ReadOnly = True
                clm.Frozen = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns.Add(clm)
            End Using


        End With
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f�[�^�O���b�h�r���[ �I���s �w�i�F�������Ή�
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �I���L�����Z��
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[�R���g���[��</param>
    ''' <remarks></remarks>
    Public Overridable Sub DataGridViewSelectionCancel(ByVal dgv As DataGridView)
        ' ���̊֐��ɃC�x���g�n���h�����t
        AddHandler dgv.SelectionChanged, AddressOf dgvData_SelectionChanged
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�I���𖳌���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        CType(sender, DataGridView).CurrentCell.Selected = False
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ����\���A�����l�Z�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDataDisp()
        ReDim m_nStatus(m_lstDspInfo.Count - 1)

        For ii As Integer = 0 To m_lstDspInfo.Count - 1
            dgvTask.RowCount += 1
            dgvTask(EM_DISP_DATA.emNo, ii).Value = ii + 1
            dgvTask(EM_DISP_DATA.emLineName, ii).Value = m_lstDspInfo(ii).strLineName
            dgvTask(EM_DISP_DATA.emTaskName, ii).Value = m_lstDspInfo(ii).strTaskName
            dgvTask(EM_DISP_DATA.emStatus, ii).Value = 1
            m_nStatus(ii) = 1
        Next
    End Sub

    ''' <summary>
    ''' ��ԕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DataDisp()
        '�Ď��O�ɑS�Ĉُ�ɂ���
        Set_StatusInisialize()
        '�^�X�N�����邩�`�F�b�N
        TaskUmuCheck()
        '��ʕ\���X�V
        DspReflesh()
    End Sub

    ''' <summary>
    ''' �Ď��O�ɑS�Ĉُ�ɂ���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Set_StatusInisialize()
        For ii As Integer = 0 To dgvTask.RowCount - 1
            dgvTask(EM_DISP_DATA.emStatus, ii).Value = 1
            dgvTask(EM_DISP_DATA.emStatus, ii).Tag = IntPtr.Zero
        Next
    End Sub

    ''' <summary>
    ''' �^�X�N�����邩�`�F�b�N
    ''' </summary>
    ''' <returns>Ture:���� False:���Ȃ�</returns>
    ''' <remarks></remarks>
    Public Function TaskUmuCheck() As Boolean
        Dim pAry() As Process = Process.GetProcesses
        Dim p As Process

        'Dim iP As Integer = 0

        '''' �v���Z�X�`�F�b�N
        For Each p In pAry
            Try
                'Console.WriteLine("{0}:{1} {2}", iP, p.MainWindowTitle, p.MainModule.ModuleName)
                '' �t�@�C�����`�F�b�N
                For ii As Integer = 0 To dgvTask.RowCount - 1
                    Dim strTaskName As String = String.Format(CStr(dgvTask(EM_DISP_DATA.emTaskName, ii).Value) & ".exe")
                    If strTaskName = p.MainModule.ModuleName Then

                        dgvTask(EM_DISP_DATA.emStatus, ii).Value = 0                    '������Z�b�g
                        dgvTask(EM_DISP_DATA.emStatus, ii).Tag = p.MainWindowHandle     '�E�B���h�E �n���h��
                        Exit For
                    End If
                Next
            Catch ex As Exception
                ' MainModule ���������̂������
            End Try
            p.Dispose()
            'iP += 1
        Next p
        Return True
    End Function

    ''' <summary>
    ''' ��ʕ\��(�X�V�p)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspReflesh()
        Dim bStatus As Boolean = True           '�S�̃^�X�N��ԃt���O(1�ł��ُ킪����Έُ�)
        '�ʃ^�X�N��ԐF��
        For ii As Integer = 0 To dgvTask.RowCount - 1
            If 0 = CInt(dgvTask(EM_DISP_DATA.emStatus, ii).Value) Then
                '����F���Z�b�g
                dgvTask(EM_DISP_DATA.emNo, ii).Style.BackColor = Color.White
                dgvTask(EM_DISP_DATA.emLineName, ii).Style.BackColor = Color.White
                dgvTask(EM_DISP_DATA.emTaskName, ii).Style.BackColor = Color.White
                '�O��=1������=0�̎��A���O�o�� 
                If 1 = m_nStatus(ii) Then
                    mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, _
                                    String.Format("�N�� [{0}][{1}]", _
                                    CStr(dgvTask(EM_DISP_DATA.emLineName, ii).Value), _
                                    CStr(dgvTask(EM_DISP_DATA.emTaskName, ii).Value)))
                End If
            Else
                bStatus = False
                '�ُ�F���Z�b�g
                dgvTask(EM_DISP_DATA.emNo, ii).Style.BackColor = Color.Red
                dgvTask(EM_DISP_DATA.emLineName, ii).Style.BackColor = Color.Red
                dgvTask(EM_DISP_DATA.emTaskName, ii).Style.BackColor = Color.Red
                '�O��=0������=1�̎��A���O�o�� 
                If 0 = m_nStatus(ii) Then
                    mcls_Log.Write(tClass.tClass_LogManager.LNO.WAR, _
                                    String.Format("��~ [{0}][{1}]", _
                                    CStr(dgvTask(EM_DISP_DATA.emLineName, ii).Value), _
                                    CStr(dgvTask(EM_DISP_DATA.emTaskName, ii).Value)))
                End If
            End If
            '����l�ۑ�
            m_nStatus(ii) = CInt(dgvTask(EM_DISP_DATA.emStatus, ii).Value)
        Next ii

        '�S�̃^�X�N��ԐF��
        If bStatus Then
            lblStatus.Text = "����"
            lblStatus.BackColor = Color.Cyan
            IconChange(True)
        Else
            lblStatus.Text = "�ُ�"
            lblStatus.BackColor = Color.Red
            IconChange(False)
        End If

        dgvTask.Refresh()
    End Sub

    ''' <summary>
    ''' �\���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTask_CellClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTask.CellClick
        If e.RowIndex < 0 Then      ' �I���s���s���Ȃ�I���
            Exit Sub
        End If
        '�\���{�^������
        If e.ColumnIndex = EM_DISP_DATA.emDspBtn Then
            Dim hWnd As IntPtr = CType(dgvTask(EM_DISP_DATA.emStatus, e.RowIndex).Tag, IntPtr)
            If hWnd = IntPtr.Zero Then
                Exit Sub
            End If
            '�\������
            ShowWindow(hWnd, SW_SHOWNORMAL)
            SetForegroundWindow(hWnd)
            Dim err As Integer = Marshal.GetLastWin32Error()
        End If

    End Sub


End Class
