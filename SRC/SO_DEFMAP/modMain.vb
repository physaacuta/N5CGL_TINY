'*********************************************************************************
' エントリー
'	[Ver]
'		Ver.01    2007/03/31  新規作成
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuLibCLI
Imports System.Reflection

Public Module modMain

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ グローバル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClassLibrary.tClass.tClass_LogManager           ' ログ管理クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' 総合クラス
    Private m_ap As ApplicationContext                                  ' コンテキスト

    Private WithEvents m_trmFrmThread As New Timer                      ' メールスロット用スレッドから画面スレッドへの橋渡し
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)           ' メールスロットのキュー

    Private m_frm As frmMain                                  ' アクセスフォーム

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

        Try
            Task_First()
        Catch ex As Exception
            MessageBox.Show("KizuLibCLI.dll を 確認して下さい。" + vbCrLf + vbCrLf +
                            "---------------------------------------------------------------" + vbCrLf + vbCrLf +
                            ex.ToString, String.Format("タスク生成 失敗"), MessageBoxButtons.OK, MessageBoxIcon.Error)

            Exit Sub
        End Try

        '''' 画面表示
        Dim showMontor As Integer = 2       ' メニュー表示対象のモニターNo (1オリジン)
        '' メニュー画面の表示
        m_frm = New frmMain()
        m_frm.Show(showMontor)

        '' メインスレッド生成
        m_ap = New ApplicationContext
        Application.Run(m_ap)               'ExitThreadがコールされるまで ここでブロック

        Task_Last()

    End Sub

    ''' <summary>
    ''' タスク終了処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TaskEnd()
		m_ap.ExitThread()										' メインスレッド終了
	End Sub

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()

        ' 画面受け渡し用キュー初期化
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '''' タスク初期化        
        KizuLibCLI.KizuMod.SetLineID()  'ラインID
        '' タスクイニシャル
        If 0 <> mcls_Mod.Task_Init(SO_DEFMAP) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", SO_DEFMAP))
            End
        End If

        '' ログクラス生成
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KizuLibCLI.KizuMod.GetMyTaskName(), Nothing, Nothing)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "実績画面起動")
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

        gcls_Log.Dispose()
        gcls_Log = Nothing
        mcls_Mod.Task_Exit()
    End Sub

    ''' <summary>
    ''' メールスロット受信
    ''' </summary>
    ''' <param name="pData"></param>
    ''' <param name="nEvtNo"></param>
    ''' <remarks></remarks>
    Private Sub mcls_Mod_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail

        Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)

        Try

            Select Case nEvtNo

                Case FACT_SO_DEFMAP_01  ' パラメータ変更通知
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("パラメータ変更通知受信[{0}]", que_l.data(0)))
                    m_frm.TranParam(que_l.data(0))

                Case FACT_SO_DEFMAP_02  ' 画面更新通知
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, String.Format("画面更新通知受信"))
                    m_frm.TranDspNew()

                Case FACT_KS_END        ' タスク終了
                    m_ap.ExitThread()

                Case FACT_KS_PCNAME_CHANGE
					''PC名読み直し
                    'KizuLibCLI.KizuMod.GetPcName()

                Case Else
                    KizuLibCLI.KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "EventNo = [" & nEvtNo & "]"))
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "EventNo = [" & nEvtNo & "]")
            End Select

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "メール受信エラー [" & ex.Message & "]")
        End Try

    End Sub

    ''' <summary>
    ''' フォームタイトル設定
    ''' </summary>
    ''' <param name="frmData">フォーム</param>
    ''' <remarks></remarks>
    Public Sub SetTitleName(ByRef frmData As Form)

        frmData.Text = String.Format("[{0}] {1}", SYSTEM_TITLE, frmData.Text)

    End Sub

    ''' <summary>
    ''' 調整済フォントサイズの取得(ラベル)
    ''' </summary>
    ''' <param name="lblLabel">対象ラベル</param>
    ''' <param name="strString">表示文字列</param>
    ''' <param name="nDefaultFontSize">既定フォントサイズ</param>
    ''' <param name="nMaxWidth">最大表示幅</param>
    ''' <returns>最大表示幅に適したフォントサイズ(1pt単位)</returns>
    ''' <remarks>
    ''' フォントサイズが8未満の場合は8を返却する。
    ''' また、既定フォントサイズで最大表示幅を超えない場合は既定フォントサイズを返す
    ''' </remarks>
    Public Function GetLabelFontScaleSize(ByVal lblLabel As Label, ByVal strString As String, ByVal nDefaultFontSize As Single, ByVal nMaxWidth As Integer) As Single
        Dim nFontWidth As Single                    ' 文字表示幅
        Dim nScaleFontSize As Single                ' 調整フォントサイズ
        Dim fntScale As System.Drawing.Font         ' 調整フォント
        Dim nMaxDspWidth As Integer                 ' 文字表示最大幅
        Dim g As Graphics = lblLabel.CreateGraphics()
        Dim fntDefault As Font = New System.Drawing.Font("ＭＳ 明朝", nDefaultFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

        Try

            ' 初期化
            nFontWidth = g.MeasureString(strString, fntDefault).Width
            nScaleFontSize = nDefaultFontSize
            fntScale = New System.Drawing.Font("ＭＳ 明朝", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            ' 文字表示最大幅は表示最大幅から既定フォントサイズ半角１文字分余裕を持たせる
            nMaxDspWidth = nMaxWidth - CShort(g.MeasureString("@", fntDefault).Width)
            If nMaxDspWidth < 0 Then
                nMaxDspWidth = nMaxWidth
            End If

            ' 表示幅内に収まる場合は既定フォントサイズを返す
            If nFontWidth <= nMaxDspWidth Then
                fntScale.Dispose()
                Return nDefaultFontSize
            End If

            ' 1ptずつフォントサイズを小さくして、文字表示最大幅を下回るフォントサイズを算出
            While nScaleFontSize > 0
                nScaleFontSize = nScaleFontSize - 1.0F
                If nScaleFontSize < 8 Then
                    Exit While
                End If

                fntScale = New System.Drawing.Font("ＭＳ 明朝", nScaleFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                nFontWidth = g.MeasureString(strString, fntScale).Width

                If nFontWidth <= nMaxDspWidth Then
                    Exit While
                End If

            End While

            ' 余り小さいと見えないのでフォントサイズが8未満は8にする。
            If nScaleFontSize < 8 Then
                nScaleFontSize = 8
            End If

            fntScale.Dispose()

        Catch ex As Exception

        Finally
            If g IsNot Nothing Then
                g.Dispose()
                g = Nothing
            End If

        End Try

        Return nScaleFontSize
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
