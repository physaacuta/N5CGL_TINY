'*********************************************************************************
' エントリー
'	[Ver]
'		Ver.01    2012/05/31  新規作成
'
'	[メモ]
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
    '/ グローバル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public gcls_Log As tClass_LogManager                                ' ログ管理クラス
    Public gcls_LogEx As tClass_LogManager                              ' ログ管理クラス(重要)
    Public g_NOA As Integer                                             ' NOA接続フラグ (0:オンライン  1:遠隔)
    Public g_bOutTrk As Boolean                                         ' 外部連動有無 (false:従来=単独動作  true:連動動作 )
    Public g_typStartArgs As START_ARGS                                 ' 起動引数
    Public g_nFormPos As Integer

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' メインスレッド管理用 (ほぼ必須の基本の形)
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod               ' 総合クラス （別スレッドなので注意）
    Private m_ap As ApplicationContext                                  ' コンテキスト

    Private WithEvents m_trmFrmThread As New Timer                      ' メールスロット用スレッドから画面スレッドへの橋渡し
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)       ' メールスロットのキュー

    Private m_frm As frmMenu                                            ' アクセスフォーム

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ イベント定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 稼動状態通知 受信イベント
    Public Event tEV_MailSlotRecv_FACT_SO_GAMENN_01(ByVal que_l As COMMON_QUE_L)
    ' コイル情報取得完了通知
    Public Event tEV_MailSlotRecv_FACT_SO_GAMENN_02(ByVal que_l As COMMON_QUE_STR)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ デバッグログ関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private gcls_frmDebugLog As Form
    Private lbxLog As ListBox
    Private lbxLogEx As ListBox

    Private Sub frmDebugLog_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs)
        e.Cancel = True
        CType(sender, Form).Hide()
    End Sub

    ''' <summary>
    ''' notepadを開く
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDebugLog_DoubleClicked(ByVal sender As Object, ByVal e As EventArgs)
        gcls_Log.ShowEdit()
    End Sub
    ''' <summary>
    ''' notepadを開く
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
        gcls_frmDebugLog.Text = "デバッグログ"
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
    ''' デバッグログを表示する
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ShowDebugLog()
        If gcls_frmDebugLog IsNot Nothing Then
            gcls_frmDebugLog.Show()
            ' 表示を一番上に持ってくる
            gcls_frmDebugLog.TopMost = True
            'gcls_frmDebugLog.TopMost = False
        End If
    End Sub

    ''' <summary>
    ''' 共通ログ出力
    ''' </summary>
    ''' <param name="no"></param>
    ''' <param name="fmt"></param>
    ''' <param name="param"></param>
    ''' <remarks>ログ出力の記述が長くなるので、独自で定義した</remarks>
    Public Sub Log(ByVal no As LNO, ByVal fmt As String, ByVal ParamArray param() As Object)
        Try
            ' ログの先頭に現在の使用メモリ量を表示する
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
            gcls_LogEx.Write(LNO.ERR, String.Format("ログ書込みフォーマットエラー[{0}]", ex.Message))
        End Try
    End Sub
    ''' <summary>
    ''' 共通ログ出力
    ''' </summary>
    ''' <param name="no"></param>
    ''' <param name="fmt"></param>
    ''' <param name="param"></param>
    ''' <remarks>ログ出力の記述が長くなるので、独自で定義した</remarks>
    Public Sub LogEx(ByVal no As LNO, ByVal fmt As String, ByVal ParamArray param() As Object)
        Try
            ' ログの先頭に現在の使用メモリ量を表示する
            Dim log As String = String.Format("[{0,7:###,###}kb] ", My.Application.Info.WorkingSet / 1024)
            log &= String.Format(fmt, param)
            gcls_LogEx.Write(no, log)
        Catch ex As Exception
            gcls_LogEx.Write(LNO.ERR, String.Format("ログ(重要)書込みフォーマットエラー[{0}]", ex.Message))
        End Try
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' エントリーポイント
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        ' ※KizuLibCLI 32bit / 64bit 切り替え用
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        ' 認証処理 (現調中、一時的にユーザーチェック無効。最終的には必ず有効にすること)
        If ENABLE_USERCHECK Then
            If Not tNinsho.IsUserCheck(SO_GAMENN) Then
                Exit Sub
            End If
        End If

        Task_First()

        gcls_Log.Write(LNO.MSG, "オペレータ画面起動")
        gcls_LogEx.Write(LNO.MSG, "オペレータ画面起動")

        'Dim cmds As String() = System.Environment.GetCommandLineArgs()
        'For Each cmd As String In cmds
        '    gcls_Log.Write(LNO.INF, String.Format("コマンドライン引数[{0}]", cmd))
        'Next

        '' 画面表示
        'If cmds.Length >= 2 Then
        '    ' コマンド引数がある場合
        '    m_frm = New frmMenu(10)
        'Else
        '    ' コマンド引数が無い場合
        '    m_frm = New frmMenu
        'End If
        'm_frm.Show()

        ' 画面表示
        m_frm = New frmMenu
        m_frm.Show()

        '' Mailスレッドから画面スレッドへの橋渡しタイマー開始()
        m_trmFrmThread.Interval = 100
        m_trmFrmThread.Start()

        '' メインスレッド生成
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThreadがコールされるまで ここでブロック

        Task_Last()

    End Sub

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()

        'ラインID
        KizuLibCLI.KizuMod.SetLineID()

        '' iniファイルより 定数的なグローバール変数読み込み
        g_NOA = tMod.tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)

        ' 画面受け渡し用キュー初期化
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        ' 外部連動有無
        g_bOutTrk = CBool(IIf(1 = System.Environment.GetCommandLineArgs.Length, False, True))

        g_nFormPos = 1
        '''' 外部連動有りの場合のみ、引数解読
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

        '' タスクイニシャル
        If 0 <> mcls_Mod.Task_Init(SO_GAMENN) Then
            If NOA_ONLINE = g_NOA Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", SO_GAMENN))
            End If

            End
        End If

        InitFrmDebugLog()
        gcls_frmDebugLog.Show()
        gcls_frmDebugLog.Hide()

        '' ログクラス生成
        If gcls_frmDebugLog IsNot Nothing Then
            gcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, lbxLog)
            gcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, lbxLogEx)
        Else
            gcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, Nothing)
            gcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, Nothing)
        End If
    End Sub

    ''' <summary>
    ''' タスク終了処理時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_Last()
        '' 終了
        m_frm.Close()
        m_frm.Dispose()
        m_frm = Nothing

        If gcls_frmDebugLog IsNot Nothing Then
            gcls_frmDebugLog.Dispose()
            gcls_frmDebugLog = Nothing
        End If


        ' その他クラス解放
        gcls_Log.Dispose()
        gcls_Log = Nothing

        ' タスク終了
        mcls_Mod.Task_Exit()

    End Sub

    ''' <summary>
    ''' タスク終了処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()
        m_ap.ExitThread()                                       ' メインスレッド終了
    End Sub


    ''' <summary>
    ''' メールスロット受信
    ''' </summary>
    ''' <param name="pData">データ</param>
    ''' <param name="nEvtNo">イベント番号</param>
    ''' <remarks></remarks>
    Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
        Dim ii As Integer = 0
        Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)

        '''' 画面スレッドへ渡す
        m_MailQue.SetItem(que_l)
    End Sub

    ''' <summary>
    ''' メールスロット受信
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_trmFrmThread_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_trmFrmThread.Tick
        Dim que_l As COMMON_QUE_L = m_MailQue.GetItem(0)
        If que_l.nEventNo = 0 Then Return

        ' COMMON_QUE_L を COMMON_QUE_STR に変換する
        Dim que_str As COMMON_QUE_STR = CType(mcls_Mod.typConv(que_l, GetType(COMMON_QUE_STR)), COMMON_QUE_STR)


        Select Case que_l.nEventNo

            '//-----------------------------------------------------------
            Case FACT_SO_GAMENN_01  ' 稼動状態通知
                gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("稼動状態通知受信 運転モード[{0}]  PLG区分[{1}] 片面検査許可不許可[{2}] 稼動状態[{3}]", que_l.data(0), que_l.data(1), que_l.data(2), que_l.data(3)))

                ' イベント発行
                RaiseEvent tEV_MailSlotRecv_FACT_SO_GAMENN_01(que_l)

            Case FACT_KS_END        ' タスク終了
                gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "タスク終了通知受信")

                m_ap.ExitThread()

            Case FACT_KS_PCNAME_CHANGE
                ''PC名読み直し
                'KizuLibCLI.KizuMod.GetPcName()

            Case Else
                If NOA_ONLINE = g_NOA Then
                    KizuLibCLI.KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "EventNo = [" & que_l.nEventNo & "]"))
                End If
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "EventNo = [" & que_l.nEventNo & "]")
        End Select

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' フォームタイトル設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

        frmData.Text = String.Format("[{0}] {1}", SYSTEM_TITLE, frmData.Text)

    End Sub

    ''' <summary>
    ''' 画面高さをPCの解像度に合わせて設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetPcHeight(ByRef frmData As Form)

        '画面の高さがPC解像度の高さより大きい場合
        Dim nScreenHeigth As Integer = Screen.FromControl(frmData).Bounds.Height
        If nScreenHeigth < frmData.Height Then
            frmData.Height = nScreenHeigth      ' PCの解像度を高さに設定
        End If

    End Sub

    ''' <summary>
    ''' 画面幅をPCの解像度に合わせて設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetPcWidth(ByRef frmData As Form)

        '画面の幅がPC解像度の幅より大きい場合
        Dim nScreenWidth As Integer = Screen.FromControl(frmData).Bounds.Width
        If nScreenWidth < frmData.Width Then
            frmData.Width = nScreenWidth      ' PCの解像度を幅に設定
        End If

    End Sub

    ''' <summary>
    ''' バイト配列の指定位置から指定長さのHEX文字列を取得する
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
    ''' KizuLibCLI.DLLの読込み
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit切り替え対応</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function
End Module
