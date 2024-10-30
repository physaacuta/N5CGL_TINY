'*********************************************************************************
'　探傷画面メインクラス
'	[Ver]
'		Ver.01    2012/05/09  初版
'
'	[メモ]
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
    '/ グローバル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_NOA As Integer                                                     ' NOA接続フラグ (0:オンライン  1:遠隔)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                           ' 総合クラス

    Private m_ap As ApplicationContext                                          ' コンテキスト
    Private mcls_Log As tClass_LogManager                                       ' ログ管理クラス
    Private mcls_LogEx As tClass_LogManager                                     ' ログ管理クラス(重要)
    Private mcls_Param As clsParamManager                                       ' パラメータ情報管理
    Private mcls_PreParam As clsPreParamManager                                 ' 前工程パラメータ管理クラス

    Private WithEvents m_lstTansho As List(Of frmTansho)                        ' 探傷画面
    Private m_nSyncMode As EM_SYNC_MODE                                         ' 画面の同期モード

    Private WithEvents m_timGetMail As New Timer                                ' メールスロット用スレッドから画面スレッドへの橋渡し
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)                   ' メールスロットのキュー

    Private WithEvents m_timBrink As New Timer                                  'ブリンク表示タイマー

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ デバッグログ関連
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
    ''' notepadを開く
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDebugLog_DoubleClicked(ByVal sender As Object, ByVal e As EventArgs)
        mcls_Log.ShowEdit()
    End Sub
    ''' <summary>
    ''' notepadを開く
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
            mcls_LogEx.Write(LNO.ERR, String.Format("ログ書込みフォーマットエラー[{0}]", ex.Message))
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
            mcls_LogEx.Write(no, log)
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("ログ(重要)書込みフォーマットエラー[{0}]", ex.Message))
        End Try
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' エントリーポイント
    ''' </summary>
    ''' <remarks>※Main内で</remarks>
    Public Function Main() As Integer
        ' ※KizuLibCLI 32bit / 64bit 切り替え用
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        'Windowsシャットダウン、再起動通知取得イベント
        AddHandler SystemEvents.SessionEnding, AddressOf SystemEvents_SessionEnding

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll を 確認して下さい。" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("タスク生成 失敗"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Function
        End Try

        Try
            '-------------------------------------------------------------------
            'KizuTask.iniの取得
            '-------------------------------------------------------------------
            '出力総画面数
            Dim nFormTotalNum As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "FROM_TOTAL_NUM", TASKINI_NAME, 1)
            '疵マップ更新秒数
            Dim nMapInterval As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "MAP_INTERVAL", TASKINI_NAME, 1000)
            'ステータス更新秒数
            Dim nStatusInterval As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "STATUS_INTERVAL", TASKINI_NAME, 500)
            'コイル更新秒数
            Dim nCoilInterval As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "COIL_INTERVAL", TASKINI_NAME, 1000)
            '表示疵画像定義
            Dim nImageWidth As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "IMAGE_WIDTH", TASKINI_NAME, 25)
            If nImageWidth <= 0 Then
                nImageWidth = 25
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示疵画像定義異常[{0}]", nImageWidth))
            End If
            '表示疵画像定義
            Dim nTcmImageWidth As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "TCM_IMAGE_WIDTH", TASKINI_NAME, 25)
            If nTcmImageWidth <= 0 Then
                nTcmImageWidth = 25
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCM表示疵画像定義異常[{0}]", nTcmImageWidth))
            End If
            '同期モード設定
            m_nSyncMode = CType(tMod_IniFile.ReadIniInt(HY_TANSHO, "SYNC_MODE", TASKINI_NAME, 2), EM_SYNC_MODE)
            If m_nSyncMode <> EM_SYNC_MODE.ALL And m_nSyncMode <> EM_SYNC_MODE.BY_DSP And m_nSyncMode <> EM_SYNC_MODE.NONE Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("同期モード設定異常[{0}]", m_nSyncMode))
                m_nSyncMode = EM_SYNC_MODE.NONE
            End If

            Dim nWk As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "SYNC_MODE_UMU", TASKINI_NAME, 0)
            Dim bSyncUmu As Boolean = False
            If 1 = nWk And nFormTotalNum > 1 Then
                If 1 = nFormTotalNum Then
                    Log(LNO.MSG, String.Format("modeMain 同期モード有無強制変更(出力総画面数=1のため) ini定義={0}", nWk))
                    bSyncUmu = False
                Else
                    bSyncUmu = True
                End If
            Else
                bSyncUmu = False
            End If
            '一時停止からnコイル分切替りで、自動的に一時停止解除。
            Dim nTempStopN As Integer = tMod_IniFile.ReadIniInt(HY_TANSHO, "TEMP_STOP_CANCEL", TASKINI_NAME, DSP_IDX_OLD_MAX)
            If nTempStopN < 1 Then
                nTempStopN = 1
                Log(LNO.ERR, String.Format("modeMain 自動的に一時停止解除するコイル本数定義異常1[{0}]", nTempStopN))
            End If
            If nTempStopN > DSP_IDX_OLD_MAX Then
                nTempStopN = DSP_IDX_OLD_MAX
                Log(LNO.ERR, String.Format("modeMain 自動的に一時停止解除するコイル本数定義異常2[{0}]", nTempStopN))
            End If

            '-------------------------------------------------------------------
            ' パラメータ情報読み出し
            '-------------------------------------------------------------------
            mcls_Param = New clsParamManager(mcls_LogEx)
            While (Not mcls_Param.ReadAllParam())                               'パラメータ読み出し成功するまで粘る
                Application.DoEvents()
                Threading.Thread.Sleep(1000)
            End While

            mcls_PreParam = New clsPreParamManager(mcls_LogEx)                    ' 前工程パラメータ管理
            While (mcls_PreParam.ReadAll() = False)                             ' パラメータ読み出し成功するまで粘る
                Threading.Thread.Sleep(1000)
            End While

            '-------------------------------------------------------------------
            ' 探傷画面表示
            '-------------------------------------------------------------------
            m_lstTansho = New List(Of frmTansho)
            For nFormID As Integer = 1 To nFormTotalNum
                Dim clsTansho As frmTansho = New frmTansho(nFormID, mcls_Log, mcls_Param, mcls_PreParam)
                clsTansho.MapInterval = nMapInterval                            '疵マップ情報更新間隔設定
                clsTansho.StatusInterval = nStatusInterval                      'ステータス情報更新間隔設定
                clsTansho.CoilInterval = nCoilInterval                          'コイル情報更新間隔設定
                If NOA_ONLINE = g_NOA Then
                    'オンライン時
                    clsTansho.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iONLINE
                Else
                    '遠隔時&デバッグ時
                    'フォーム状態
                    clsTansho.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iOFFLINE

                End If
                m_lstTansho.Add(clsTansho)
                clsTansho.SetImageWidth(nImageWidth)
                clsTansho.SetTcmImageWidth(nTcmImageWidth)
                '探傷画面表示
                clsTansho.Show()
                'イベント設定
                'スクロール同期変更
                AddHandler clsTansho.ev_ChangeScrollSync, AddressOf frmTansho_ChangeScrollSync
                'スクロール値変更
                AddHandler clsTansho.ev_ChangeScrollValue, AddressOf frmTansho_ChangeScrollValue
            Next
            m_lstTansho(0).Focus()

            '-------------------------------------------------------------------
            'メインスレッド起動
            '-------------------------------------------------------------------
            'ブリンク表示タイマー
            m_timBrink.Interval = 1000
            m_timBrink.Start()

            'メールスロット受信監視の開始
            m_timGetMail.Interval = 500
            m_timGetMail.Start()
            m_ap = New ApplicationContext
            Application.Run(m_ap)                                               'ExitThreadがコールされるまで ここでブロック
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("modeMain メイン処理異常[{0}]", ex.Message))
        End Try


        '-----------------------------------------------------------------------
        ' メインスレッド終了
        '-----------------------------------------------------------------------
        Task_Last()

        End
    End Function

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()

        ' 前準備
        '' iniファイルより 定数的なグローバール変数読み込み
        g_NOA = tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)

        ' 画面受け渡し用キュー初期化
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '''' タスク初期化
        KizuLibCLI.KizuMod.SetLineID()
        'タスクイニシャル (DBへのアクセスは無し)
        If 0 <> mcls_Mod.Task_Init(HY_TANSHO) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", HY_TANSHO))
            End
        End If
        InitFrmDebugLog()
        gcls_frmDebugLog.Show()
        gcls_frmDebugLog.Hide()

        '' ログクラス生成()
        If gcls_frmDebugLog IsNot Nothing Then
            mcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, lbxLog)
            mcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, lbxLogEx)
        Else
            mcls_Log = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, Nothing)
            mcls_LogEx = New tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName() & "_Ex", Nothing, Nothing)
        End If

        mcls_LogEx.Write(LNO.MSG, "探傷画面起動")
        LogEx(LNO.MSG, String.Format("ログ(重要)開始"))
        Log(LNO.MSG, String.Format("ログ開始"))

    End Sub

    ''' <summary>
    ''' タスク終了処理時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_Last()

        Try
            If Not m_timBrink Is Nothing Then
                m_timBrink.Stop()                                               '自動スクロールタイマー停止
            End If
            If m_timGetMail Is Nothing = False Then
                m_timGetMail.Stop()                                             ' メールスロット監視停止
            End If

            If m_MailQue Is Nothing = False Then                                ' メールスロットキュー削除
                m_MailQue.Dispose()
                m_MailQue = Nothing
            End If

            If Not m_lstTansho Is Nothing Then                                  '探傷画面作成の場合は解放
                For Each clsTansho As frmTansho In m_lstTansho
                    clsTansho.Close()
                    clsTansho.Dispose()
                    clsTansho = Nothing
                Next
            End If

            If Not mcls_Param Is Nothing Then                                   'パラメータマネージャの解放
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If

            If Not mcls_PreParam Is Nothing Then                                'パラメータマネージャの解放
                mcls_PreParam.Dispose()
                mcls_PreParam = Nothing
            End If

        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("{0} [{1}]", "メイン処理終了異常", ex.Message))
        End Try

        RemoveHandler SystemEvents.SessionEnding,
           AddressOf SystemEvents_SessionEnding                                 ' Windowsシャットダウン、再起動通知取得イベント


        mcls_LogEx.Write(LNO.MSG, "探傷画面終了")
        If mcls_Log IsNot Nothing Then
            mcls_Log.Dispose()
            mcls_Log = Nothing
        End If
        If mcls_LogEx IsNot Nothing Then
            mcls_LogEx.Dispose()
            mcls_LogEx = Nothing
        End If

        mcls_Mod.Task_Exit()                                                    ' メインスレッド終了
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
    ''' タイマー処理[メール取得]
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub m_timGetMail_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_timGetMail.Tick
        'メール取得
        Dim que_l As COMMON_QUE_L = m_MailQue.GetItem(0)

        'メールなし
        If 0 = que_l.nEventNo Then
            Exit Sub
        End If

        Try
            Select Case que_l.nEventNo
                Case FACT_HY_TANSHO_01                      'パラメータ変更通知
                    Log(LNO.MSG, "パラメータ変更通知")
                    LogEx(LNO.MSG, "パラメータ変更通知")
                    mcls_Param.ReadAllParam()
                    mcls_PreParam.ReadAll()
                Case FACT_HY_TANSHO_02                      'イニシャル通知
                    Log(LNO.MSG, "イニシャル通知")
                    LogEx(LNO.MSG, "イニシャル通知")
                    mcls_Param.ReadAllParam()
                    mcls_PreParam.ReadAll()
                    For ii As Integer = 0 To m_lstTansho.Count - 1
                        If Not m_lstTansho(ii) Is Nothing Then
                            m_lstTansho(ii).DspInit()
                        End If
                    Next
                Case FACT_KS_DB_STATEUS                     'DB状態変更通知
                    Log(LNO.MSG, "DB状態変更通知")
                    LogEx(LNO.MSG, "DB状態変更通知")
                    Dim bDBStatus As Boolean = False
                    If que_l.data(0) = 0 Then               'DB正常
                        bDBStatus = True
                    End If
                    For ii As Integer = 0 To m_lstTansho.Count - 1
                        If Not m_lstTansho(ii) Is Nothing Then
                            m_lstTansho(ii).DBStatus = bDBStatus
                        End If
                    Next

                Case FACT_KS_PCNAME_CHANGE                  'PC名称変更通知
                    Log(LNO.MSG, "PC名称変更通知")
                    LogEx(LNO.MSG, "PC名称変更通知")

                Case FACT_KS_END                            'タスク終了通知
                    mcls_Log.Write(tClass_LogManager.LNO.MSG, "タスク終了通知")
                    mcls_LogEx.Write(tClass_LogManager.LNO.MSG, "タスク終了通知")
                    For ii As Integer = 0 To m_lstTansho.Count - 1
                        If Not m_lstTansho(ii) Is Nothing Then
                            m_lstTansho(ii).frmTansho_Disposed()
                        End If
                    Next
                    m_ap.ExitThread()                       'スレッド終了
                Case Else                                   '起動要因異常
                    mcls_LogEx.Write(LNO.ERR, String.Format("起動要因異常[{0}]", que_l.nEventNo))
                    KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "err_code =[" & que_l.nEventNo & "]"))
            End Select
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("メールスロット受信異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' Windowsシャットダウン、再起動通知取得イベント
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub SystemEvents_SessionEnding(ByVal sender As Object, ByVal e As Microsoft.Win32.SessionEndingEventArgs)
        Try
            Dim que As KizuLibCLI.COMMON_QUE_S = Nothing
            mcls_LogEx.Write(LNO.MSG, "Windowsシャットダウン、再起動通知")
            que.nEventNo = FACT_KS_END                                          ' タスク終了
            que.nLineNo = KizuMod.GetLineID()
            KizuMod.Send_Mail(KizuMod.GetMyTaskName(), ".", que)                ' 自分へ終了通知
        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("Windowsシャットダウン、再起動通知取得イベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    '/////////////////////////////////////////////////////////////////////////////////
    '画面ハンドルイベント
    '/////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' スクロール値変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="nValue">スクロール量</param>
    ''' <remarks></remarks>
    Public Sub frmTansho_ChangeScrollValue(ByVal sender As frmTansho, ByVal nValue As Integer, ByVal emScrool As EM_MAP_SCROOL)
        Try
            Dim nDspPos As Integer = sender.KensaNo
            For Each frm As frmTansho In m_lstTansho
                If frm.Equals(sender) Then                                      'イベント発生元の場合は抜ける
                    Continue For
                End If

                If m_lstTansho(0).Stage = m_lstTansho(1).Stage Then
                    If frm.ScrollSync Then
                        Select Case m_nSyncMode                                     '同期モード毎に設定する
                            Case EM_SYNC_MODE.ALL                                   '全画面同期モード
                                frm.SetScrollValue(nValue, emScrool)
                            Case EM_SYNC_MODE.BY_DSP                                '検査台毎モード
                                If frm.KensaNo = nDspPos Then
                                    frm.SetScrollValue(nValue, emScrool)
                                End If
                        End Select
                    End If
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("スクロール値変更異常[{0}],[{1}]", nValue, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' スクロール同期変更
    ''' </summary>
    ''' <param name="sender">変更のあった探傷画面</param>
    ''' <param name="bSync">変更後のモード True:同期 False:非同期</param>
    ''' <remarks></remarks>
    Public Sub frmTansho_ChangeScrollSync(ByVal sender As frmTansho, ByVal bSync As Boolean)
        Try
            Dim nDspPos As Integer = sender.KensaNo
            For Each frm As frmTansho In m_lstTansho
                If frm.Equals(sender) Then                                      'イベント発生元の場合は抜ける
                    Continue For
                End If
                Select Case m_nSyncMode                                         '同期モード毎に設定する
                    Case EM_SYNC_MODE.ALL                                       '全画面同期モード
                        frm.ScrollSync = bSync
                    Case EM_SYNC_MODE.BY_DSP                                    '検査台毎モード
                        If frm.KensaNo = nDspPos Then
                            frm.ScrollSync = bSync
                        End If
                End Select
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("スクロール同期変更異常[{0}],[{1}]", bSync, ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ブリンク表示
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
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ブリンク表示異常[{0}]", ex.Message))
        End Try
    End Sub

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
