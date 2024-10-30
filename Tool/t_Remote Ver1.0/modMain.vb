' AnyCPU にして、OSとAPのビットを合わせておくこと。そうしないとレジストリ書き込みがNG

Imports Microsoft.Win32


Module modMain

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    Private Const mcTITLE As String = "クライアント端末リモート動作制御"


    Private m_ap As ApplicationContext                                  ' コンテキスト
    Private WithEvents m_nfyIcon As System.Windows.Forms.NotifyIcon     ' タスクトレイのアイコン コントロール

    Private m_trAutoCheck As New Threading.Timer(AddressOf AutoCheck)   ' リモート接続状態監視
    Private m_trAutoLogin As New Threading.Timer(AddressOf AutoLogin)   ' 自動コンソールセッションに切り替え

    Private m_nRegMode As Integer = -1   ' -1:初回   0:無効  1:有効

    ''' <summary>
    ''' メイン
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Main()
        Dim icNomal As Icon = Nothing ' 通常アイコン

        '' タスクトレイのアイコン取得
        Try                                                         ' アイコン設定
            '' 埋め込まれたリソースからアイコンを取り出し
            Dim MyAsm As Reflection.Assembly = Reflection.Assembly.GetExecutingAssembly()
            icNomal = New Icon(MyAsm.GetManifestResourceStream("t_Remote.t_Remote.ico"))
        Catch ex As Exception
        End Try

        '' タスクトレイのアイコン コントロール インスタンス生成
        m_nfyIcon = New System.Windows.Forms.NotifyIcon
        m_nfyIcon.ContextMenu = CreateNotifyIconMenu_Def()      'タスクトレイのアイコンメニュー定義
        m_nfyIcon.Text = mcTITLE
        m_nfyIcon.Icon = icNomal
        m_nfyIcon.Visible = True

        '' 実行してもよい？
        Dim bIsSrv As Boolean = IsWindowsServer()
        'bIsSrv = False

        '' メインスレッド生成
        If Not bIsSrv Then
            ' タイマー起動
            m_trAutoCheck.Change(100, 100)   ' 起動
            m_trAutoLogin.Change(10000, 5000)   ' 起動


            m_ap = New ApplicationContext
            Application.Run(m_ap)           ' ExitThreadがコールされるまで ここでブロック


            ' タイマー停止
            m_trAutoCheck.Dispose()
            m_trAutoLogin.Dispose()
        End If

        '' 自動ログイン有効化
        Call RegWrite(True)

        '' 解放
        m_nfyIcon.Dispose()
    End Sub

    ''' <summary>
    ''' タスクトレイのアイコンメニュー定義
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CreateNotifyIconMenu_Def() As ContextMenu

        Dim closeItem() As MenuItem = { _
            New MenuItem("リモート終了", AddressOf m_menuItem_Click)
        }

        '' コンテキストメニュー定義
        Dim menu As New ContextMenu
        menu.MenuItems.AddRange(closeItem)

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
            Case 0
                '' モニタにコンソール画面を表示させる
                ' ※そうしないと、モニタ上ではログイン画面のままとなってしまうため
                Call AllUserLogOff.Change()
        End Select
    End Sub


    '////////////////////////////////////////////////////////////////////
    '// 処理

    ''' <summary>
    ''' 自動リモートチェック (タイマー実行)
    ''' </summary>
    ''' <param name="state"></param>
    ''' <remarks></remarks>
    Private Sub AutoCheck(ByVal state As Object)
        Dim bRemote As Boolean = CheckRemote()

        ' リモート接続された
        If bRemote And 0 <> m_nRegMode Then
            Call RegWrite(False)
            m_nfyIcon.Text = mcTITLE & "[リモート]"

        ElseIf Not bRemote And 1 <> m_nRegMode Then
            Call RegWrite(True)
            m_nfyIcon.Text = mcTITLE & "[ローカル]"
        End If
    End Sub

    ''' <summary>
    ''' リモート状態
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckRemote() As Boolean

        ' リモートデスクトップ経由で実行？
        If Not System.Windows.Forms.SystemInformation.TerminalServerSession Then
            'MsgBox("リモートデスクトップではありません")
            Return False
        End If
        'MsgBox("リモートデスクトップ")
        Return True
    End Function



    ''' <summary>
    ''' 自動コンソールセッションに切り替え
    ''' </summary>
    ''' <param name="state"></param>
    ''' <remarks></remarks>
    Private Sub AutoLogin(ByVal state As Object)
        Dim nActive As Integer = AllUserLogOff.GetActiveSessionId()
        If -1 <> nActive Then Return

        ' 誰もアクティブがいない
        Call AllUserLogOff.Change()
    End Sub

    '////////////////////////////////////////////////////////////////////
    '// 小物
    <System.Runtime.InteropServices.DllImport("shlwapi.dll", SetLastError:=True, EntryPoint:="#437")> _
    Private Function IsOS(os As Integer) As Boolean
    End Function
    Private Const OS_ANYSERVER As Integer = 29

    ''' <summary>
    ''' 現在のOSがサーバーかどうか調べる
    ''' </summary>
    ''' <returns>サーバーならTrue。それ以外はFalse。</returns>
    Private Function IsWindowsServer() As Boolean
        Return IsOS(OS_ANYSERVER)
    End Function


    ''' <summary>
    ''' 自動ログイン レジストリ編集
    ''' </summary>
    ''' <param name="bEnable">true:自動ログイン有効化</param>
    ''' <remarks></remarks>
    Private Sub RegWrite(ByVal bEnable As Boolean)
        Dim regkey As Microsoft.Win32.RegistryKey = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon", True)
        Dim sVal As String = IIf(bEnable, "1", "0")
        regkey.SetValue("ForceAutoLogon", sVal)
        regkey.Close()


        m_nRegMode = IIf(bEnable, 1, 0)
    End Sub

    ''' <summary>
    ''' ダブルクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub m_nfyIcon_DoubleClick(sender As Object, e As System.EventArgs) Handles m_nfyIcon.DoubleClick

        If MsgBox("強制終了", MsgBoxStyle.OkCancel, "終了確認") = MsgBoxResult.Ok Then
            m_ap.ExitThread()
        End If
    End Sub
End Module
