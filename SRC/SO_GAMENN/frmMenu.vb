'*********************************************************************************
' メニュー画面
'	[Ver]
'		Ver.01    2012/05/23  新規作成
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

''' <summary>
''' メニュー画面
''' </summary>
''' <remarks></remarks>
Public Class frmMenu

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DISP_TOP As Boolean = True    '表示位置最前面
    Private Const m_sTFApp_Path As String = ".\HY_TANSHO.exe"           ' 探傷画面起動アプリ
    Private Const DEFAULT_GAMEN_SIZE_X As Integer = 1280
    Private Const DEFAULT_GAMEN_SIZE_Y As Integer = 1024


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents frm_DbCheck As KizuForm.frmPop_DbCheck           'DBチェックPopUpクラス
    Public frm_SystemStatus As frmSystemStatus                          '検査停止開始設定画面

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
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
    Private Sub frmMenu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Dim clsMonitor() As Screen = Screen.AllScreens()                    'モニター一覧

        'タイトル設定
        Call SetTitleName(Me)

        ' ダミー画面の起動
        Dim bDmySet As Integer = tMod.ReadIniInt(SO_GAMENN, "DMY_SET", TASKINI_NAME, 0)
        If 2 = bDmySet Then                                             ' ダミー画面を表示
            Call DummyShow(clsMonitor.Length)
        ElseIf 0 = bDmySet AndAlso NOA_ONLINE = g_NOA Then              ' NOAフラグを使用 オンライン
            Call DummyShow(clsMonitor.Length)
        End If

        'オペレータ画面起動追加
        Dim bValDispTop As Boolean = CBool(tMod.ReadIniInt(SO_GAMENN, "DISP_TOP_FLG", TASKINI_NAME, 0))
        '-------------------------------------------------------------------
        ' 最前面   画面表示設定
        '-------------------------------------------------------------------
        If DISP_TOP = bValDispTop Then      '最前面表示（DISP_TOP_FLG=1）
            Me.TopMost = True
            If g_bOutTrk Then
                MyBase.SetLocationForm(g_typStartArgs.nFormPos, False)
                g_nFormPos = g_typStartArgs.nFormPos
            End If
        Else                                '通常表示(DISP_TOP_FLG=1以外)
            Me.TopMost = False
        End If

        ' オンラインか遠隔か
        If NOA_ONLINE = g_NOA Then
            '''' ファンクションキー定義
            ' 探傷画面
            ' 操作画面の場合は探傷画面ボタンは無効にする必要が有るが、オンラインだけでは判定できない
            ' HY_TANSHOが起動しているかどうかで判定できればいいのだが、現状方法が見つからない。
            ' 実行ファイル HY_TANSHO.EXE が存在するかどうかで判定することとする。
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

            '最前面モードであれば、終了ボタンを使用可能にする。
            '探傷画面からの起動用
            'If DISP_TOP = bValDispTop Then      '      '最前面表示（DISP_TOP_FLG=1）
            '    ' 終了ボタン(使用可能)
            '    tProp_MenuEnable(11) = True
            '    my_btnFunction(11).Enabled = True
            'Else
            ' 終了ボタン(使用不可)
            tProp_MenuEnable(11) = False
            my_btnFunction(11).Enabled = False
            'End If

        ElseIf NOA_DEBUG = g_NOA Then

            'デバッグ時
            ' フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            '''' ファンクションキー定義
            ' 探傷画面
            tProp_MenuEnable(0) = True
            my_btnFunction(0).Enabled = True

            ' 終了ボタン
            tProp_MenuEnable(11) = True
            my_btnFunction(11).Enabled = True

        ElseIf NOA_KAISEKI = g_NOA Then

            '解析時
            ' フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE

            '''' ファンクションキー定義
            ' 探傷画面
            tProp_MenuEnable(0) = False
            my_btnFunction(0).Enabled = False

            ' 終了ボタン
            tProp_MenuEnable(11) = True
            my_btnFunction(11).Enabled = True

        Else

            '遠隔時
            ' フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            'Me.tProp_EndButtonEnable = False	'メニュー画面だけは ×ボタン有りとする。

            '''' ファンクションキー定義
            ' 探傷画面
            tProp_MenuEnable(0) = False
            my_btnFunction(0).Enabled = False
            ' 検査開始停止ボタン
            tProp_MenuEnable(10) = False
            my_btnFunction(10).Enabled = False
            ' 終了ボタン
            tProp_MenuEnable(11) = True
            my_btnFunction(11).Enabled = True

        End If

        If clsMonitor.Length < g_nFormPos Then                              '表示したいモニターがない場合
            g_nFormPos = 1                                                  'プライマリディスプレイに表示
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
    ''' フォームロード完了 (frmBaseからのオーバーライド)
    ''' </summary>
    ''' <remarks></remarks>
    Protected Overrides Sub LoadEnding()
        ' DB接続確認画面表示
        frm_DbCheck = New KizuForm.frmPop_DbCheck(INI_DB, g_NOA)
        frm_DbCheck.ShowDialog()

        '元の処理も忘れずに実行()
        MyBase.LoadEnding()
    End Sub

    ''' <summary>
    ''' 画面 閉じようとした
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMenu_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' メインスレッド終了通知
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' DB接続チェク完了イベント
    ''' </summary>
    ''' <param name="dbMode">DB接続状態</param>
    ''' <remarks></remarks>
    Private Sub frm_DbCheck_tEv_FunctionButton(ByVal dbMode As Boolean) Handles frm_DbCheck.tEv_FunctionButton
        ' フォームの始末
        frm_DbCheck.Dispose()
        frm_DbCheck = Nothing

        If Not dbMode Then              ' 繋がらなくて、終了ボタンを押した
            ' メインスレッド終了通知
            modMain.TaskEnd()
        End If
    End Sub

    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmMenu_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' 強制終了は一番最初に受付
        If 11 = FncNo AndAlso onKey = Keys.Shift Then
            If MsgBox("強制終了しますか？", MsgBoxStyle.YesNo Or MsgBoxStyle.Question Or MsgBoxStyle.DefaultButton2, "重要なお知らせ") = MsgBoxResult.Yes Then
                Me.Close()
                'modMain.TaskEnd()						' オフライン時の×ボタン対策として Closingで処理する
            End If
        End If

        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub
        Try
            '' ファンクション切替
            Select Case FncNo                           ' 0オリジン
                Case 1 - 1                              ' F01 探傷画面
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.INF, String.Format("探傷画面 起動要求!!"))
                    '探傷画面タスク起動
                    If TanshoStart() = True Then
                        If Not frmDummy Is Nothing Then frmDummy.Dispose()
                        frmDummy = Nothing
                        Me.Close()
                    Else
                        MsgBox("探傷画面を起動出来ませんでした", MsgBoxStyle.OkOnly Or MsgBoxStyle.Exclamation)
                    End If

                    '//-----------------------------------------------------------
                Case 2 - 1                              ' F02 

                    '//-----------------------------------------------------------
                Case 3 - 1                              ' F03 最新コイル情報表示画面
                    Dim frm As New frmDspCoil()
                    frm.ShowDialog()
                    frm.Dispose()
                    frm = Nothing

                    '//-----------------------------------------------------------
                Case 4 - 1                              ' F04 出側単位疵混入率CSV出力画面
                    Dim frm As New frmPop_Dekon()
                    frm.ShowDialog()
                    frm.SaveCheck()                     ' 検索フィルターのiniファイル保存
                    frm.Dispose()
                    frm = Nothing

                    '//-----------------------------------------------------------
                Case 5 - 1                              ' F05 実績画面
                    Dim frm As New frmJisseki()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 6 - 1                              ' F06 

                    '//-----------------------------------------------------------
                Case 7 - 1                              ' F07 機器状態トレンド画面
                    Dim frm As New frmKikiTrand
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 8 - 1                              ' F08 検査状態ログ画面
                    Dim frm As New frmStatusLog()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 9 - 1                              ' F09 故障ログ画面
                    Dim frm As New frmLog()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 10 - 1                             ' F10 RAS表示画面
                    Dim frm As New frmRas()
                    frm.Owner = Me
                    frm.SetParentForm(Me)
                    frm.Show()
                    Me.Visible = False

                    '//-----------------------------------------------------------
                Case 11 - 1                             ' F11 検査開始停止決定画面
                    frm_SystemStatus = New frmSystemStatus
                    frm_SystemStatus.ShowDialog()
                    frm_SystemStatus.Dispose()
                    Me.BringToFront()

                    '//-----------------------------------------------------------
                Case 12 - 1                             ' 終了
                    If Not frmDummy Is Nothing Then frmDummy.Dispose()
                    frmDummy = Nothing
                    Me.Close()
                    'modMain.TaskEnd()						' オフライン時の×ボタン対策として Closingで処理する
            End Select
        Catch ex As Exception

        End Try


    End Sub

    ''' <summary>
    ''' 探傷画面起動処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Function TanshoStart() As Boolean
        Dim bRet As Boolean = True

        Try
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("HY_TANSHO[{0}] 起動処理開始", m_sTFApp_Path))
            Shell(m_sTFApp_Path, AppWinStyle.NormalFocus, False)    ' HY_TANSHO 起動
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("HY_TANSHO[{0}] 起動処理完了 終了します", m_sTFApp_Path))
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("HY_TANSHO 起動処理異常 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' 操作画面起動処理
    ''' </summary>
    ''' <param name="strPath">実行フォルダパス</param>
    ''' <param name="strExe">実行ファイル名</param>
    ''' <returns></returns>
    Private Function SousaStart(ByVal strPath As String, ByVal strExe As String) As Boolean
        Dim bRet As Boolean = True
        Dim strSrcPath As String = System.IO.Directory.GetCurrentDirectory()
        Try
            gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("SO_GAMENN[{0}] 起動処理開始", strPath + strExe))

            ' 作業ディレクトリを変更
            System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetFullPath(strPath))
            ' SO_GAMENN 起動
            Shell(strPath + strExe, AppWinStyle.NormalFocus, False)

            gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("SO_GAMENN[{0}] 起動処理完了 終了します", strPath + strExe))
        Catch ex As Exception
            ' 失敗したら作業ディレクトリを戻す
            System.IO.Directory.SetCurrentDirectory(strSrcPath)
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("SO_GAMENN 起動処理異常 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' ダミー画面起動処理
    ''' </summary>
    ''' <param name="nCnt">モニタ数</param>
    Private Sub DummyShow(ByVal nCnt As Integer)

        ' モニタ数が１画面以下の場合はダミー画面は表示しない
        If 1 >= nCnt Then Exit Sub

        ' モニタ数分ループ
        For nMonitorNo As Integer = 1 To nCnt
            ' Dummy画面起動
            Dim frm As New frmDummy()
            frm.Show(nMonitorNo, True)
        Next

    End Sub

End Class
