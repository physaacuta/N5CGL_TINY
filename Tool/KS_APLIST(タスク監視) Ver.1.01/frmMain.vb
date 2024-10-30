'*********************************************************************************
'　タスク監視
'	[Ver]
'		Ver.01    2015/01/27  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices
Imports System.Drawing


Public Class frmMain
    '********************************************************************
    'API定義
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
    ' 定数定義
    '---------------------------------------------------------------------------
    Private Const m_ini As String = ".\KS_APLIST.ini"
    Private Const KS_APLIST As String = "KS_APLIST"

    Private Const MAX_EXECUTE_TASK As Integer = 50

    '---------------------------------------------------------------------------
    ' 定数定義
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

    ' データグリッドビューの列
    Private Enum EM_DISP_DATA
        emNo = 0                    ' No
        emLineName                  ' ライン名称
        emTaskName                  ' タスク略称
        emDspBtn                    ' 表示ボタン
        emStatus                    ' タスク状態(非表示)
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                            ' ログ管理クラス

    Private m_TimerCount As Integer                                         ' 監視する周期[sec]

    Private WithEvents m_nfyIcon As System.Windows.Forms.NotifyIcon         ' タスクトレイのアイコン コントロール
    Private m_icNomal As Icon                                               ' 通常アイコン(正常時)
    Private m_icNG As Icon                                                  ' 通常アイコン(異常時)

    Private m_lstBase As New List(Of BASE_INFO)                             ' 監視対象リスト
    Private m_lstDspInfo As New List(Of TASK_INFO)                          ' 監視タスク一覧


    Private m_nStatus() As Integer                                          ' 前回値保存ワーク


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー関数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmApList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        '二重起動チェック
        If Process.GetProcessesByName(Process.GetCurrentProcess.ProcessName).Length > 1 Then
            MsgBox("既に起動中です。")
            End
        End If

        'ログクラス生成
        mcls_Log = New tClass.tClass_LogManager(KS_APLIST, Nothing, Nothing)

        mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "監視ツール起動")
        '' ×ボタンを無効にする
        tMod.subFormEndButton_Desebled(Me.Handle)

        '' タスクトレイのアイコン取得
        Try                                                         ' アイコン設定
            '' 埋め込まれたリソースからアイコンを取り出し
            Dim MyAsm As Reflection.Assembly = Reflection.Assembly.GetExecutingAssembly()
            m_icNomal = New Icon(MyAsm.GetManifestResourceStream("KS_APLIST.KS_APLIST.ico"))
            m_icNG = New Icon(MyAsm.GetManifestResourceStream("KS_APLIST.KS_APLIST_NG.ico"))
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "アイコン設定エラー [" & ex.Message & "]")
        End Try

        '' タスクトレイのアイコン コントロール インスタンス生成
        m_nfyIcon = New System.Windows.Forms.NotifyIcon
        m_nfyIcon.ContextMenu = CreateNotifyIconMenu_Def()      'タスクトレイのアイコンメニュー定義
        m_nfyIcon.Text = "タスク監視(KS_APLIST)"
        IconChange(True)    ' 通常アイコンセット
        m_nfyIcon.Visible = True

        '初期化
        m_lstBase.Clear()
        m_lstDspInfo.Clear()

        '設定ファイル読み込み
        GetIniFile()

        '監視対象タスクの一覧を作成
        MakeTargetTaskList()

        'データグリッドビュー初期化
        dgvInitialize(dgvTask)

        '初回表示
        InitDataDisp()

        '状態表示
        DataDisp()

        ' タイマースタート
        tmrWatch.Interval = m_TimerCount * 1000
        tmrWatch.Start()

        'Me.Visible = False
        'Me.ShowInTaskbar = False
    End Sub

    ''' <summary>
    ''' 非表示ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEnd.Click
        mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "×ボタン押下")
        Me.Hide()
        'If MsgBox("終了しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question Or MsgBoxStyle.DefaultButton2, "重要なお知らせ") = MsgBoxResult.Yes Then
        '	Call SetEventExit()
        '	' タスク終了通知送信
        '	Me.Close()
        'End If
    End Sub

    ''' <summary>
    ''' 終了処理
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
    '/ 設定ファイル読込関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' INIファイル読込(Base)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetIniFile()
        '監視周期
        m_TimerCount = tMod.ReadIniInt(KS_APLIST, "CHECK_TIMER_COUNT", m_ini, 10)
        '対象セクション
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
    ''' INIファイル読込(個別)
    ''' </summary>
    ''' <param name="strSection">セクション</param>
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
    ''' 監視対象タスクの一覧を作成する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeTargetTaskList()
        m_lstDspInfo.Clear()
        For Each data As BASE_INFO In m_lstBase
            Dim dt As TASK_INFO

            'KizuTask.iniの起動タスクを登録
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
                '# プロセス情報 (1～50まで)
                '#  TASK1	: 起動タスク名, 起動プロセス(0:通常 1:最小化 2:最大化 3:非表示), ホントの起動EXE名称(起動タスク名と同じなら無しでOK)
                '#=========================================================
                If strAry.Length > 2 Then
                    dt.strTaskName = Trim(strAry(2))
                Else
                    dt.strTaskName = Trim(strAry(0))
                End If
                m_lstDspInfo.Add(dt)
            Next ii
            '個別指定の起動タスクを登録
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
    '/ タスクトレイ関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' タスクトレイのアイコンメニュー定義
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CreateNotifyIconMenu_Def() As ContextMenu
        '' メニュー項目の製作
        Dim closeItem1 As New MenuItem("ツール終了", AddressOf m_menuItem_Click)
        '' 改行設定
        closeItem1.BarBreak = True  '1で改行

        '' コンテキストメニュー定義
        Dim menu As New ContextMenu
        menu.MenuItems.Add(closeItem1)
        Return menu
    End Function

    ''' <summary>
    ''' タスクトレイのアイコンメニュークリックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_menuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs)
        ' メニューにより処理分け
        Dim menu As MenuItem = CType(sender, MenuItem)
        Select Case menu.Index
            Case 0                                  ' 
                If MsgBox("終了しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question Or MsgBoxStyle.DefaultButton2, "重要なお知らせ") = MsgBoxResult.Yes Then
                    mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "ツール終了選択")
                    Call SetEventExit()
                    ' タスク終了通知送信
                    Me.Close()
                End If
        End Select

    End Sub

    ''' <summary>
    ''' タスクトレイアイコン ダブルクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nfyIcon_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_nfyIcon.DoubleClick
        'mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "タスクトレイアイコン ダブルクリック")

        '' フォーム表示中なら消す
        If Me.Visible Then
            Me.Hide()
        Else
            '' フォーム表示
            Me.Show()
            Me.TopMost = True   '最初のみ最前面に出す
            Me.TopMost = False
        End If
    End Sub

    ''' <summary>
    ''' タスクトレイのアイコン変更
    ''' </summary>
    ''' <param name="bNomal">true:正常  false:異常</param>
    ''' <remarks></remarks>
    Public Sub IconChange(ByVal bNomal As Boolean)
        If bNomal Then
            m_nfyIcon.Icon = m_icNomal
        Else
            m_nfyIcon.Icon = m_icNG
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ タイマー関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 監視タイマー処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrWatch_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrWatch.Tick
        '状態表示
        DataDisp()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As DataGridView)
        '' 基本を設定
        'Call dgv.dgvInitialize(dgv)
        Call DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        ' 独自の項目を設定
        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                                ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False                                         ' セルエラーの表示
            .ShowCellToolTips = False                                       ' ツールヒントの表示
            .ShowEditingIcon = False                                        ' 編集中のセルの明示
            .ShowRowErrors = False                                          ' エラー行の明示
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示
            '.ColumnHeadersHeight = 42										' ヘッダーの高さ

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = False                                               ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            .RowCount = 0


            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor
            .DefaultCellStyle().SelectionForeColor = Color.Black
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 24                                       ' ヘッダーの高さ

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
                clm.HeaderText = "タスク略称"
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

                clm.DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                Dim insBtnText As DataGridViewButtonColumn = CType(clm, DataGridViewButtonColumn)
                insBtnText.UseColumnTextForButtonValue = True
                insBtnText.Text = "表示"
                .Columns.Add(clm)
            End Using

            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "状態"
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
    '/ データグリッドビュー 選択行 背景色無効化対応
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 選択キャンセル
    ''' </summary>
    ''' <param name="dgv">データグリッドビューコントロール</param>
    ''' <remarks></remarks>
    Public Overridable Sub DataGridViewSelectionCancel(ByVal dgv As DataGridView)
        ' 下の関数にイベントハンドル割付
        AddHandler dgv.SelectionChanged, AddressOf dgvData_SelectionChanged
    End Sub

    ''' <summary>
    ''' データグリッドビュー選択を無効化
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
    ''' 初回表示、初期値セット
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
    ''' 状態表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DataDisp()
        '監視前に全て異常にする
        Set_StatusInisialize()
        'タスクが居るかチェック
        TaskUmuCheck()
        '画面表示更新
        DspReflesh()
    End Sub

    ''' <summary>
    ''' 監視前に全て異常にする
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Set_StatusInisialize()
        For ii As Integer = 0 To dgvTask.RowCount - 1
            dgvTask(EM_DISP_DATA.emStatus, ii).Value = 1
            dgvTask(EM_DISP_DATA.emStatus, ii).Tag = IntPtr.Zero
        Next
    End Sub

    ''' <summary>
    ''' タスクが居るかチェック
    ''' </summary>
    ''' <returns>Ture:居る False:居ない</returns>
    ''' <remarks></remarks>
    Public Function TaskUmuCheck() As Boolean
        Dim pAry() As Process = Process.GetProcesses
        Dim p As Process

        'Dim iP As Integer = 0

        '''' プロセスチェック
        For Each p In pAry
            Try
                'Console.WriteLine("{0}:{1} {2}", iP, p.MainWindowTitle, p.MainModule.ModuleName)
                '' ファイル名チェック
                For ii As Integer = 0 To dgvTask.RowCount - 1
                    Dim strTaskName As String = String.Format(CStr(dgvTask(EM_DISP_DATA.emTaskName, ii).Value) & ".exe")
                    If strTaskName = p.MainModule.ModuleName Then

                        dgvTask(EM_DISP_DATA.emStatus, ii).Value = 0                    '正常をセット
                        dgvTask(EM_DISP_DATA.emStatus, ii).Tag = p.MainWindowHandle     'ウィンドウ ハンドル
                        Exit For
                    End If
                Next
            Catch ex As Exception
                ' MainModule が無いものもある為
            End Try
            p.Dispose()
            'iP += 1
        Next p
        Return True
    End Function

    ''' <summary>
    ''' 画面表示(更新用)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspReflesh()
        Dim bStatus As Boolean = True           '全体タスク状態フラグ(1つでも異常があれば異常)
        '個別タスク状態色替
        For ii As Integer = 0 To dgvTask.RowCount - 1
            If 0 = CInt(dgvTask(EM_DISP_DATA.emStatus, ii).Value) Then
                '正常色をセット
                dgvTask(EM_DISP_DATA.emNo, ii).Style.BackColor = Color.White
                dgvTask(EM_DISP_DATA.emLineName, ii).Style.BackColor = Color.White
                dgvTask(EM_DISP_DATA.emTaskName, ii).Style.BackColor = Color.White
                '前回=1→今回=0の時、ログ出力 
                If 1 = m_nStatus(ii) Then
                    mcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, _
                                    String.Format("起動 [{0}][{1}]", _
                                    CStr(dgvTask(EM_DISP_DATA.emLineName, ii).Value), _
                                    CStr(dgvTask(EM_DISP_DATA.emTaskName, ii).Value)))
                End If
            Else
                bStatus = False
                '異常色をセット
                dgvTask(EM_DISP_DATA.emNo, ii).Style.BackColor = Color.Red
                dgvTask(EM_DISP_DATA.emLineName, ii).Style.BackColor = Color.Red
                dgvTask(EM_DISP_DATA.emTaskName, ii).Style.BackColor = Color.Red
                '前回=0→今回=1の時、ログ出力 
                If 0 = m_nStatus(ii) Then
                    mcls_Log.Write(tClass.tClass_LogManager.LNO.WAR, _
                                    String.Format("停止 [{0}][{1}]", _
                                    CStr(dgvTask(EM_DISP_DATA.emLineName, ii).Value), _
                                    CStr(dgvTask(EM_DISP_DATA.emTaskName, ii).Value)))
                End If
            End If
            '今回値保存
            m_nStatus(ii) = CInt(dgvTask(EM_DISP_DATA.emStatus, ii).Value)
        Next ii

        '全体タスク状態色替
        If bStatus Then
            lblStatus.Text = "正常"
            lblStatus.BackColor = Color.Cyan
            IconChange(True)
        Else
            lblStatus.Text = "異常"
            lblStatus.BackColor = Color.Red
            IconChange(False)
        End If

        dgvTask.Refresh()
    End Sub

    ''' <summary>
    ''' 表示ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTask_CellClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTask.CellClick
        If e.RowIndex < 0 Then      ' 選択行が不正なら終わる
            Exit Sub
        End If
        '表示ボタン押下
        If e.ColumnIndex = EM_DISP_DATA.emDspBtn Then
            Dim hWnd As IntPtr = CType(dgvTask(EM_DISP_DATA.emStatus, e.RowIndex).Tag, IntPtr)
            If hWnd = IntPtr.Zero Then
                Exit Sub
            End If
            '表示処理
            ShowWindow(hWnd, SW_SHOWNORMAL)
            SetForegroundWindow(hWnd)
            Dim err As Integer = Marshal.GetLastWin32Error()
        End If

    End Sub


End Class
