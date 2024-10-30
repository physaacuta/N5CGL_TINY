'*********************************************************************************
'　重欠陥一覧画面メインクラス
'	[Ver]
'		Ver.01    2008/07/04  初版
'
'	[メモ]
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
    '定数
    '///////////////////////////////////////////////////////////////////////////
    Private Const DEFLIST_DSP_PATTERN As EM_DSP_PATTERN = EM_DSP_PATTERN.nPatternNo01   '表示画面数


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ グローバル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_NOA As Integer                                                     ' NOA接続フラグ (0:オンライン  1:遠隔)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    'パラメータ定義
    Public Enum EM_PARAM_MODE
        nALL = 0    '全て
        nType       '疵種・グレード
        nList       '疵一覧
    End Enum

    '画面表示パターン
    Public Enum EM_DSP_PATTERN
        nPatternNo01 = 1
        nPatternNo02
        nPatternNo03
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' 総合クラス
    Private m_ap As ApplicationContext                                  ' コンテキスト
    Private mcls_Log As tClass_LogManager                               ' ログ管理クラス
    Private mcls_LogEx As tClass_LogManager                             ' ログ管理クラス(重要)
    Private mcls_Param As clsParamManager                               ' パラメータ情報管理

    Private WithEvents m_timGetMail As New Timer                        ' メールスロット用スレッドから画面スレッドへの橋渡し
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)           ' メールスロットのキュー

    Private mcls_BG As clsProductBackColor                              ' 生産番号背景色設定
    Private m_frmCglDefList_01(1) As frmCglDefList_Pattern01
    Private m_frmCglDefList_02(1) As frmCglDefList_Pattern02
    Private m_frmCglDefList_03(1) As frmCglDefList_Pattern03


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ デバッグログ関連
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
    ''' <remarks></remarks>
    Public Function Main() As Integer
        'Windowsシャットダウン、再起動通知取得イベント
        AddHandler SystemEvents.SessionEnding, AddressOf SystemEvents_SessionEnding

        ' ※KizuLibCLI 32bit / 64bit 切り替え用
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll を 確認して下さい。" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("タスク生成 失敗"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Function
        End Try

        mcls_LogEx.Write(LNO.MSG, "重欠陥一覧画面起動")
        LogEx(LNO.MSG, String.Format("ログ(重要)開始"))
        Log(LNO.MSG, String.Format("ログ開始"))

        Dim nMyID As Integer                        ' 画面表示ID
        Dim monitor() As System.Windows.Forms.Screen
        Dim nCnt As Integer
        monitor = System.Windows.Forms.Screen.AllScreens()
        nCnt = monitor.Length    'モニタ数

        Try
            '-------------------------------------------------------------------
            'KizuTask.iniの取得
            '-------------------------------------------------------------------
            '表示疵画像定義
            Dim nImageWidth As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_WIDTH", TASKINI_NAME, 25)
            If nImageWidth <= 0 Then
                nImageWidth = 25
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示疵画像定義異常[{0}]", nImageWidth))
            End If
            '表示倍率
            Dim nImageRate05 As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_RATE_05", TASKINI_NAME, 100)
            If nImageRate05 <= 0 Then
                nImageRate05 = 100
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示疵画像倍率定義異常[{0}]", nImageRate05))
            End If
            Dim nImageRate14 As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_RATE_14", TASKINI_NAME, 75)
            If nImageRate14 <= 0 Then
                nImageRate14 = 100
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示疵画像倍率定義異常[{0}]", nImageRate14))
            End If
            Dim nImageRate30 As Integer = tMod_IniFile.ReadIniInt(HY_DEFLIST, "IMAGE_RATE_30", TASKINI_NAME, 50)
            If nImageRate30 <= 0 Then
                nImageRate30 = 100
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示疵画像倍率定義異常[{0}]", nImageRate30))
            End If
            '画面表示モード
            Dim nDspMode As Integer = tMod.ReadIniInt(HY_DEFLIST, "DSP_TYPE", TASKINI_NAME)
            '-------------------------------------------------------------------
            ' パラメータ情報読み出し
            '-------------------------------------------------------------------
            mcls_Param = New clsParamManager(mcls_LogEx)
            While (Not mcls_Param.ReadAllParam())                               'パラメータ読み出し成功するまで粘る
                Application.DoEvents()
                Threading.Thread.Sleep(1000)
            End While
            ''-------------------------------------------------------------------
            '' 重欠陥一覧画面表示
            ''-------------------------------------------------------------------
            '生産番号背景色設定
            mcls_BG = New clsProductBackColor(mcls_Log)
            mcls_BG.ReadPnoBGinit()

            'TOPを表示
            If 0 = nDspMode Or 1 = nDspMode Then

                '画面表示位置
                nMyID = tMod.ReadIniInt(HY_DEFLIST, "DSP_POS_TOP", TASKINI_NAME, 1)

                'モニタ番号ない場合は、表示できるモニタに表示
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

                '画面表示
                Call DspDefList(DEFLIST_DSP_PATTERN, 0)

            End If

            'BOTを表示
            If 0 = nDspMode Or 2 = nDspMode Then

                '画面表示位置
                nMyID = tMod.ReadIniInt(HY_DEFLIST, "DSP_POS_BOT", TASKINI_NAME, 1)

                'モニタ番号ない場合は、表示できるモニタに表示
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

                '画面表示
                Call DspDefList(DEFLIST_DSP_PATTERN, 1)

            End If

            For ii As Integer = 0 To UBound(m_frmCglDefList_01)
                Select Case DEFLIST_DSP_PATTERN
                    Case EM_DSP_PATTERN.nPatternNo01
                        If Not m_frmCglDefList_01(ii) Is Nothing Then
                            AddHandler m_frmCglDefList_01(ii).DspDefList, AddressOf DspDefList                     '画面表示設定
                            AddHandler m_frmCglDefList_01(ii).ModeClickEvent, AddressOf ChangeDspModeCapl          '自動更新・一時停止ボタンクリック処理
                            AddHandler m_frmCglDefList_01(ii).ev_ChangeOverlyVisible, AddressOf ChangeOverLay      'オーバーレイ表示変更
                            AddHandler m_frmCglDefList_01(ii).ev_ChangeMemoriVisible, AddressOf ChangeMemori       '目盛り表示変更
                            AddHandler m_frmCglDefList_01(ii).StopSelectedIndexChanged, AddressOf ChangeStopTimer  '一時停止変更イベント
                            AddHandler m_frmCglDefList_01(ii).PopSelectedIndexChanged, AddressOf ChangePopUpTimer  'ポップアップ変更イベント
                        End If
                    Case EM_DSP_PATTERN.nPatternNo02
                        If Not m_frmCglDefList_02(ii) Is Nothing Then
                            AddHandler m_frmCglDefList_02(ii).DspDefList, AddressOf DspDefList                     '画面表示設定
                            AddHandler m_frmCglDefList_02(ii).ModeClickEvent, AddressOf ChangeDspModeCapl          '自動更新・一時停止ボタンクリック処理
                            AddHandler m_frmCglDefList_02(ii).ev_ChangeOverlyVisible, AddressOf ChangeOverLay      'オーバーレイ表示変更
                            AddHandler m_frmCglDefList_02(ii).ev_ChangeMemoriVisible, AddressOf ChangeMemori       '目盛り表示変更
                            AddHandler m_frmCglDefList_02(ii).StopSelectedIndexChanged, AddressOf ChangeStopTimer  '一時停止変更イベント
                            AddHandler m_frmCglDefList_02(ii).PopSelectedIndexChanged, AddressOf ChangePopUpTimer  'ポップアップ変更イベント
                        End If
                    Case EM_DSP_PATTERN.nPatternNo03
                        If Not m_frmCglDefList_03(ii) Is Nothing Then
                            AddHandler m_frmCglDefList_03(ii).DspDefList, AddressOf DspDefList                     '画面表示設定
                            AddHandler m_frmCglDefList_03(ii).ModeClickEvent, AddressOf ChangeDspModeCapl          '自動更新・一時停止ボタンクリック処理
                            AddHandler m_frmCglDefList_03(ii).ev_ChangeOverlyVisible, AddressOf ChangeOverLay      'オーバーレイ表示変更
                            AddHandler m_frmCglDefList_03(ii).ev_ChangeMemoriVisible, AddressOf ChangeMemori       '目盛り表示変更
                            AddHandler m_frmCglDefList_03(ii).StopSelectedIndexChanged, AddressOf ChangeStopTimer  '一時停止変更イベント
                            AddHandler m_frmCglDefList_03(ii).PopSelectedIndexChanged, AddressOf ChangePopUpTimer  'ポップアップ変更イベント
                        End If
                End Select
            Next ii

        Catch ex As Exception

        End Try


        'メールスロット受信監視の開始
        m_timGetMail.Interval = 500
        m_timGetMail.Start()

        '' メインスレッド生成
        m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThreadがコールされるまで ここでブロック


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

        ' 画面受け渡し用キュー初期化
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '-----------------------------------------------------------------------
        '疵検タスク開始処理 (ここでこけるとログすら出せない)
        '-----------------------------------------------------------------------
        'タスクイニシャル (DBへのアクセスは無し)
        KizuMod.SetLineID()

        '' iniファイルより 定数的なグローバール変数読み込み
        g_NOA = tMod_IniFile.ReadIniInt(INI_COMMON, "NOA", TASKINI_NAME)
        If 0 <> mcls_Mod.Task_Init(HY_DEFLIST) Then
            KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", HY_DEFLIST))
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

    End Sub

    ''' <summary>
    ''' タスク終了処理時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_Last()

        Try
            If m_timGetMail Is Nothing = False Then
                m_timGetMail.Stop()                                           ' メールスロット監視停止
            End If

            If m_MailQue Is Nothing = False Then                                ' メールスロットキュー削除
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

            If Not mcls_Param Is Nothing Then                                   'パラメータマネージャの解放
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If

        Catch ex As Exception
            mcls_LogEx.Write(LNO.ERR, String.Format("{0} [{1}]", "メイン処理終了異常", ex.Message))
        End Try

        RemoveHandler SystemEvents.SessionEnding,
           AddressOf SystemEvents_SessionEnding                                 ' Windowsシャットダウン、再起動通知取得イベント

        mcls_LogEx.Write(LNO.MSG, "重欠陥一覧画面終了")
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
                Case FACT_HY_DEFLIST_01                      'パラメータ変更通知
                    Log(LNO.MSG, "パラメータ変更通知")
                    LogEx(LNO.MSG, "パラメータ変更通知")
                    mcls_Param.ReadAllParam()

                    For ii As Integer = 0 To UBound(m_frmCglDefList_01)
                        Select Case DEFLIST_DSP_PATTERN
                            Case EM_DSP_PATTERN.nPatternNo01
                                If Not m_frmCglDefList_01(ii) Is Nothing Then
                                    m_frmCglDefList_01(ii).DspUpdate()          '画面更新
                                End If
                            Case EM_DSP_PATTERN.nPatternNo02
                                If Not m_frmCglDefList_02(ii) Is Nothing Then
                                    m_frmCglDefList_02(ii).DspUpdate()          '画面更新
                                End If
                            Case EM_DSP_PATTERN.nPatternNo03
                                If Not m_frmCglDefList_03(ii) Is Nothing Then
                                    m_frmCglDefList_03(ii).DspUpdate()          '画面更新
                                End If
                        End Select
                    Next ii

                Case FACT_KS_PCNAME_CHANGE                  'PC名称変更通知
                    Log(LNO.MSG, "PC名称変更通知")
                    LogEx(LNO.MSG, "PC名称変更通知")

                Case FACT_KS_END                            'タスク終了通知
                    mcls_Log.Write(tClass_LogManager.LNO.MSG, "タスク終了通知")
                    mcls_LogEx.Write(tClass_LogManager.LNO.MSG, "タスク終了通知")

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
            mcls_Log.Write(LNO.MSG, "Windowsシャットダウン、再起動通知")
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
    ''' 自動更新、一時停止ボタンクリック処理
    ''' </summary>
    ''' <param name="nOuDiv">表裏区分 0:表 1:裏</param>
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
    ''' パラメータ変更通知
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Send_Mail()
        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_TO_DEFECT_01
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' データ部
        que.data(0) = 0                     ' 遅延実行キー  0固定
        que.data(1) = CH_PARAM_DEFLIST      ' パラメータテーブルID
        que.data(2) = 0                     ' 変更検査表面状態No (検査表面状態No:1～  全部:CH_SCODE_ALL  無し:0)

        KizuLibCLI.KizuMod.Send_Mail(TO_DEFECT, "*", que)

    End Sub

    ''' <summary>
    ''' ローカルINIにデータ書き込み
    ''' </summary>
    ''' <param name="strKey">キー</param>
    ''' <param name="strVal">値</param>
    ''' <remarks></remarks>
    Public Sub WriteIniData(ByVal strKey As String, ByVal strVal As String)

        tMod.WriteIni(HY_DEFLIST, strKey, LOCALINI_NAME, strVal)

    End Sub

    ''' <summary>
    ''' 画面表示
    ''' </summary>
    ''' <param name="nImagePattern">表示パターン</param>
    ''' <param name="nOuDiv">表裏区分(0:表,1:裏)</param>
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
    ''' オーバレイ表示チェンジ
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeOverLay(ByVal bOnOff As Boolean)

        Dim ii As Integer

        'TOP,BOTに通知
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
    ''' 目盛り表示チェンジ
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeMemori(ByVal bOnOff As Boolean)

        Dim ii As Integer

        'TOP,BOTに通知
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
    ''' 一時停止ポップアップタイマー変更
    ''' </summary>
    ''' <param name="nOuDiv">表裏区分 0:表 1:裏</param>
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
    ''' ポップアップタイマー変更
    ''' </summary>
    ''' <param name="nOuDiv">表裏区分 0:表 1:裏</param>
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
