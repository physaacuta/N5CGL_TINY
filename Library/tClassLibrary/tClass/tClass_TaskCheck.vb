'*********************************************************************************
'タスクの起動・停止・チェック等を制御するクラス
'	[Ver]
'		Ver.01    2005/12/30  vs2005 対応
'
'	[メモ]
'
'*********************************************************************************
Option Strict On                                ' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tClass
    ''' <summary>
    ''' タスクの起動・停止・チェック等を制御するクラス
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_TaskCheck
        Implements IDisposable                          ' デストラクタ定義

        '********************************************************************
        '定数定義
        '********************************************************************
        ''' <summary>
        ''' タスクを実行した場合にコール(非同期デリゲート)
        ''' </summary>
        ''' <param name="sender">現インスタンス</param>
        ''' <param name="e">実行したタスク名</param>
        ''' <remarks></remarks>
        Public Delegate Sub tDele_TaskCheck_Event(ByVal sender As Object, ByVal e As String)

        ''' <summary>
        ''' チェックExe情報構造体
        ''' </summary>
        ''' <remarks></remarks>
        Private Structure typTaskinf
            ''' <summary>実行Exeフルパス</summary> 
            Dim strFPath As String
            ''' <summary>タスク名称</summary> 
            Dim strName As String
            ''' <summary>実行スタイル</summary> 
            Dim style As AppWinStyle
        End Structure

        '********************************************************************
        'メンバー変数定義
        '********************************************************************
        Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには
        Private m_wThread As tClass_WorkThread			' メインスレッド
        Private m_evGo As Threading.AutoResetEvent		' 即時実行イベント
        Private m_deleEvent As tDele_TaskCheck_Event	' タスクを実行した時にコール
        Private m_Interval As Integer                   ' タイマ周期 [ms]
        Private m_pTaskInf As ArrayList					' チェックExe情報

        '********************************************************************
        'メソッド定義
        '********************************************************************
        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="deleMethod">コールバック関数アドレス(addressof 関数名)</param>
        ''' <param name="iniFPath">iniファイルフルパス</param>
        ''' <param name="iniSession">セッション</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal deleMethod As tDele_TaskCheck_Event, ByVal iniFPath As String, ByVal iniSession As String)
            '''' メンバ変数セット
            m_deleEvent = deleMethod
            m_pTaskInf = New ArrayList

            '''' iniファイル取り込み
            m_Interval = tMod.tMod_IniFile.ReadIniInt(iniSession, "Interval", iniFPath, 0)		 ' タイマ周期 [s]
            If m_Interval <= 0 Then Throw New ArgumentOutOfRangeException("タイマ周期")
            m_Interval = m_Interval * 1000  '[s]→[ms]
            Dim cnt As Integer
            cnt = tMod.tMod_IniFile.ReadIniInt(iniSession, "CheckMaxCount", iniFPath, 0)
            If cnt < 0 Then Throw New ArgumentOutOfRangeException("最大数異常")
            For ii As Integer = 1 To cnt         '情報取得
                Dim inf As New typTaskinf
                inf.strFPath = tMod.tMod_IniFile.ReadIniStr(iniSession, String.Format("PATH_{0:000}", ii), iniFPath, "")
                If inf.strFPath.Length <= 0 Then Throw New ArgumentOutOfRangeException("ファイルパス異常")
                inf.strName = tMod.tMod_IniFile.ReadIniStr(iniSession, String.Format("NAME_{0:000}", ii), iniFPath, "")
                If inf.strName.Length <= 0 Then Throw New ArgumentOutOfRangeException("チェック名称異常")
                Dim wk As Integer
                wk = tMod.tMod_IniFile.ReadIniInt(iniSession, String.Format("MODE_{0:000}", ii), iniFPath, -1)
                If wk <> 0 And wk <> 1 And wk <> 2 And wk <> 3 Then Throw New ArgumentOutOfRangeException("表示スタイル異常")
                Select Case wk
                    Case 0  '通常
                        inf.style = AppWinStyle.NormalFocus
                    Case 1  '最小 
                        inf.style = AppWinStyle.MinimizedNoFocus
                    Case 2  '最大
                        inf.style = AppWinStyle.MaximizedFocus
                    Case 3  '非表示
                        inf.style = AppWinStyle.Hide
                End Select
                'リストに登録
                m_pTaskInf.Add(inf)
            Next ii

            '''' インスタンス生成
            m_evGo = New Threading.AutoResetEvent(False)
            m_wThread = New tClass_WorkThread
        End Sub
        ''' <param name="iniFPath">iniファイルフルパス</param>
        ''' <param name="iniSession">セッション</param>
        Public Sub New(ByVal iniFPath As String, ByVal iniSession As String)
            Me.New(Nothing, iniFPath, iniSession)
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
                End If

                ' TODO: 共有のアンマネージ リソースを解放します
                Try
                    '' インスタンス確認
                    If m_wThread Is Nothing Then Exit Sub
                    '' メインスレッド終了
                    m_wThread.ThreadStop(5000)
                    m_wThread.Dispose()
                    m_wThread = Nothing
                    m_evGo.Close()
                Catch ex As Exception
                Finally
                End Try
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Disposeデストラクタ
        ''' </summary>
        ''' <remarks>使用後、必ずコールする事</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' スレッド実行
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ThreadStart()
            m_wThread.ThreadStart(AddressOf ThreadMain, Threading.ThreadPriority.Normal, "tClass_TaskCheck", False)
        End Sub

        ''' <summary>
        ''' スレッド終了
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ThreadStop()
            m_wThread.ThreadStop(5000)
        End Sub

        ''' <summary>
        ''' タスクチェック即時実行
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub taskCheckGo()
            m_evGo.Set()
        End Sub

        ''' <summary>
        ''' メインスレッド
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ThreadMain()
            Dim EventAry(1) As Threading.WaitHandle     'イベント
            Dim iRetc As Integer                        'WaitAny 復帰情報

            '''' シグナルセット
            EventAry(0) = m_wThread.g_evStop
            EventAry(1) = m_evGo

            '''' とりあえず即時実行
            m_evGo.Set()

            '''' メインループ
            Try
                While (True)
                    '''' シグナル待ち
                    iRetc = Threading.WaitHandle.WaitAny(EventAry, m_Interval, False)

                    '''' まず終了確認
                    Select Case iRetc
                        Case 0
                            Exit While
                        Case Else   '即時実行,タイムアウト等
                            ' タスクチェック
                            Me.subTaskCheck()
                    End Select
                End While
            Catch ex As Threading.ThreadAbortException
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' タスク有無チェック
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub subTaskCheck()
            Dim inf As typTaskinf

            '''' 登録されたデータをチェック
            For Each inf In m_pTaskInf
                If Not TaskUmuCheck(inf.strName) Then
                    '''' タスク起動
                    StartExe(inf.strFPath, inf.style)
                    '''' イベント通知
                    If Not m_deleEvent Is Nothing Then
                        m_deleEvent.BeginInvoke(Me, inf.strName, Nothing, Nothing)
                    End If
                End If
            Next
        End Sub

 
        '********************************************************************
        '静的メソッド定義
        '********************************************************************
        ''' <summary>
        ''' プロセスをミューテックスに登録 (2重起動チェック)
        ''' </summary>
        ''' <param name="hMutex">Mutexオブジェクト</param>
        ''' <returns>復帰情報 (0:未起動だった 1:起動済み -1:異常)</returns>
        ''' <remarks>登録する名称は アプリケーション名</remarks>
        Public Shared Function ProcessStart(ByRef hMutex As System.Threading.Mutex) As Integer
            return ProcessStart(System.Windows.Forms.Application.ProductName, hMutex)
        End Function
        ''' <param name="strProductName">ミューテックスに登録するプロセス名称</param>
        ''' <param name="hMutex">Mutexオブジェクト</param>
        ''' <returns>復帰情報 (0:未起動だった 1:起動済み -1:異常)</returns>
        Public Shared Function ProcessStart(ByVal strProductName As String, ByRef hMutex As System.Threading.Mutex) As Integer
            hMutex = New System.Threading.Mutex(False, strProductName)    ' ミューテックス

            '''' 2重起動チェック
            Try
                'Mutex のシグナル待ち
                If hMutex.WaitOne(0, False) Then
                    GC.KeepAlive(hMutex)         'hMutex をガベージ コレクション対象から除外 (これ大事)
                    Return 0
                Else
                    Return 1
                End If
            Catch ex As Exception
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' ミューテックスを開放
        ''' </summary>
        ''' <param name="hMutex"></param>
        ''' <remarks></remarks>
        Public Shared Sub ProcessEnd(ByVal hMutex As System.Threading.Mutex)
            '''' オブジェクト終了処理
            Try
                hMutex.Close()              'Mutex を開放する
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' タスクが居るかチェック
        ''' </summary>
        ''' <param name="strTaskName">タスク名 (タスクマネージャーのイメージ名)</param>
        ''' <returns>Ture:居る False:居ない</returns>
        ''' <remarks></remarks>
        Public Shared Function TaskUmuCheck(ByVal strTaskName As String) As Boolean
            Dim pAry() As Process = Process.GetProcesses
            Dim p As Process
            Dim umu As Boolean = False

            '''' プロセスチェック
            For Each p In pAry
                Try
                    '' ファイル名チェック
                    If strTaskName = p.MainModule.ModuleName Then
                        umu = True
                        p.Dispose()
                        Exit For
                    End If
                Catch ex As Exception
                    ' MainModule が無いものもある為
                End Try
                p.Dispose()
            Next p
            Return umu
        End Function

        ''' <summary>
        ''' タスク名称からプロセス名称に変更する
        ''' </summary>
        ''' <param name="strTaskName">タスク名 (タスクマネージャーのイメージ名)</param>
        ''' <returns>取得したプロセス名称</returns>
        ''' <remarks></remarks>
        Public Shared Function getProcessName(ByVal strTaskName As String) As String
            Dim pAry() As Process = Process.GetProcesses
            Dim p As Process
            Dim strProcessName As String = ""

            '''' プロセスチェック
            For Each p In pAry
                '' ファイル名チェック
                If strTaskName = p.MainModule.ModuleName Then
                    strProcessName = p.ProcessName
                    p.Dispose()
                    Exit For
                End If
                p.Dispose()
            Next p
            Return strProcessName
        End Function

        ''' <summary>
        ''' プロセスを強制終了
        ''' </summary>
        ''' <param name="strProcessName">プロセス名</param>
        ''' <remarks></remarks>
        Public Shared Sub Kill(ByVal strProcessName As String)
            Dim pAry() As Process
            Dim p As Process

            '''' 該当プロセス名を持つプロセスを取得 (同一名称のプロセスがあるかも知れない為配列が帰る)
            pAry = Process.GetProcessesByName(strProcessName)
            For Each p In pAry
                p.Kill()        'プロセスを強制終了
                p.Dispose()
            Next
        End Sub

        ''' <summary>
        ''' ウィンドウを持っているプロセスを終了させる
        ''' </summary>
        ''' <param name="strProcessName">プロセス名</param>
        ''' <remarks></remarks>
        Public Shared Sub CloseWindow(ByVal strProcessName As String)
            Dim pAry() As Process
            Dim p As Process
            '''' 該当プロセス名を持つプロセスを取得 (同一名称のプロセスがあるかも知れない為配列が帰る)
            pAry = Process.GetProcessesByName(strProcessName)
            For Each p In pAry
                p.CloseMainWindow()     'プロセスを終了する
                p.Dispose()
            Next
        End Sub

        ''' <summary>
        ''' 指定タスクの起動
        ''' </summary>
        ''' <param name="strFPath">起動タスクのフルパス</param>
        ''' <returns>起動プロセスID</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function StartExe(ByVal strFPath As String) As Integer
            Return StartExe(strFPath, AppWinStyle.NormalFocus)
        End Function
        ''' <param name="strPath">起動タスクのパス</param>
        ''' <param name="strExeName">起動タスク名称</param>
        ''' <returns>起動プロセスID</returns>
        Public Overloads Shared Function StartExe(ByVal strPath As String, ByVal strExeName As String) As Integer
            Return StartExe(System.IO.Path.Combine(strPath, strExeName), AppWinStyle.NormalFocus)
        End Function
        ''' <param name="strPath">起動タスクのパス</param>
        ''' <param name="strExeName">起動タスク名称</param>
        ''' <param name="style">起動状態 (Hide=0 NormalFocus=1 MinimizedFocus=2 MaximizedFocus=3 NormalNoFocus=4 MinimizedNoFocus=6)</param>
        ''' <returns>起動プロセスID</returns>
        Public Overloads Shared Function StartExe(ByVal strPath As String, ByVal strExeName As String, ByVal style As AppWinStyle) As Integer
            Return StartExe(System.IO.Path.Combine(strPath, strExeName), style)
        End Function
        ''' <param name="strFPath">起動タスクのフルパス</param>
        ''' <param name="style">起動状態 (Hide=0 NormalFocus=1 MinimizedFocus=2 MaximizedFocus=3 NormalNoFocus=4 MinimizedNoFocus=6)</param>
        ''' <returns>起動プロセスID</returns>
        Public Overloads Shared Function StartExe(ByVal strFPath As String, ByVal style As AppWinStyle) As Integer
            Try
                '簡単起動版
                Return Microsoft.VisualBasic.Shell(strFPath, style)
            Catch ex As Exception
                Return 0
            End Try
        End Function
        ''' <param name="info">起動情報</param>
        ''' <returns>起動プロセス</returns>
        Public Overloads Shared Function StartExe(ByVal info As ProcessStartInfo) As Process
            Try
                '''' いろいろ起動コマンド等設定可能
                Return Process.Start(info)
            Catch ex As Exception
                Return Nothing
            End Try
        End Function
    End Class
End Namespace
