'*********************************************************************************
'　統括PC共有メモリ参照ツールメインクラス
'	[Ver]
'		Ver.01    2019/04/03  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports Microsoft.Win32

Imports KizuLibCLI
Imports System.Reflection

Public Module modMain

    Private m_ap As ApplicationContext          ' コンテキスト
    Private m_frm As frmMain                    ' アクセスフォーム

    ''' <summary>
    ''' 初期起動
    ''' </summary>
    ''' <remarks></remarks>
    Public Function Main() As Integer

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

        '' メニュー画面の表示
        m_frm = New frmMain()
        m_frm.Show()

        '' メインスレッド生成
        m_ap = New ApplicationContext
        Application.Run(m_ap)                   'ExitThreadがコールされるまで ここでブロック

        Task_Last()

    End Function

    ''' <summary>
    ''' タスク終了処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()

        m_ap.ExitThread()                       ' メインスレッド終了

    End Sub

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()

        ' KizuLibCLIの読込時のエラーを検知する為、KizuLibCLI.dllに定義されているクラスを作成
        Dim clsDummy As KizuMod = New KizuMod
        clsDummy.Dispose()

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
