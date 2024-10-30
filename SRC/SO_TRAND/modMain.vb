'*********************************************************************************
'　スタート
'	[Ver]
'		Ver.01    2009/12/17  初版
'
'	[メモ]
'		
'*********************************************************************************

Option Strict On
Imports tClassLibrary
Imports KizuLibCLI
Imports System.Reflection


Module modMain

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開定数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public Const GC_MAX_SELECT_TYPE As Integer = 5								' 最大選択疵種
	Public Const GC_MAX_PROTDATA As Integer = 1000								' 最大コイル件数


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ グローバル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Public gcls_Log As tClass.tClass_LogManager = Nothing               ' ログ管理クラス (本体)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ローカル変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' メインスレッド管理用 (ほぼ必須の基本の形)
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' 総合クラス （別スレッドなので注意）
    Private m_ap As ApplicationContext									' コンテキスト

	Private WithEvents m_trmFrmThread As New Timer                      ' メールスロット用スレッドから画面スレッドへの橋渡し
    Private m_MailQue As tClass.tClass_Queue(Of COMMON_QUE_L)           ' メールスロットのキュー

    Private mfrm_Main As frmMain										' アクセスフォーム



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 基本定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' エントリーポイント
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

        '''' 画面表示
        Dim showMontor As Integer = 2       ' メニュー表示対象のモニターNo (1オリジン)
        '' メニュー画面の表示
        mfrm_Main = New frmMain
		mfrm_Main.Show(showMontor)


		'' Mailスレッドから画面スレッドへの橋渡しタイマー開始()
		m_trmFrmThread.Interval = 100
		m_trmFrmThread.Start()

		'' メインスレッド生成
		m_ap = New ApplicationContext
        Application.Run(m_ap)           'ExitThreadがコールされるまで ここでブロック
        m_ap.Dispose()

        Task_Last()

    End Function

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()

        ' 画面受け渡し用キュー初期化
        m_MailQue = New tClass.tClass_Queue(Of COMMON_QUE_L)

        mcls_Mod = New KizuMod

        '''' タスク初期化
        KizuLibCLI.KizuMod.SetLineID()
        '' タスクイニシャル (DBへのアクセスは無し)
        If 0 <> mcls_Mod.Task_Init(SO_TRAND) Then
            KizuLibCLI.KizuMod.SysLog(SYSNO_TASK_INIT_ERR, String.Format("[{0}]", SO_TRAND))
            End
        End If

        '' ログクラス生成
        gcls_Log = New tClassLibrary.tClass.tClass_LogManager(KizuMod.GetMyTaskName(), Nothing, Nothing)
        gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "トレンド表示画面起動")
    End Sub

    ''' <summary>
    ''' タスク終了処理時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_Last()

        ' フォーム開放
        mfrm_Main.Close()
        mfrm_Main.Dispose()
        mfrm_Main = Nothing

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

        ' スレッドが生成されていなかったら終了
        If m_ap Is Nothing Then
            gcls_Log.Dispose()
            gcls_Log = Nothing
            End
        End If

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


		Select Case que_l.nEventNo

			'//-----------------------------------------------------------
			Case FACT_SO_TRAND_01       ' 画面更新通知
                Call SendMailDefMap(FACT_SO_DEFMAP_02)
                Call mfrm_Main.gfrm_Setting_EvDsp()

				'//-----------------------------------------------------------
			Case FACT_KS_DB_STATEUS		'DB状態変更通知

				'//-----------------------------------------------------------
			Case FACT_KS_PCNAME_CHANGE
				'処理なし

				'//-----------------------------------------------------------
			Case FACT_KS_END		' タスク終了
				gcls_Log.Write(tClass.tClass_LogManager.LNO.MSG, "タスク終了通知受信")

				m_ap.ExitThread()	' スレッド終了

			Case Else
				KizuLibCLI.KizuMod.SysLog(SYSNO_MAILNO_ERR, String.Format("[{0}]", "err_code = [" & que_l.nEventNo & "]"))
		End Select

	End Sub

    ''' <summary>
    ''' 疵点マップ画面更新通知
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SendMailDefMap(ByVal nEventNo As Integer)
        Try
            Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
            que.inisialize()

            que.nEventNo = nEventNo
            que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

            KizuLibCLI.KizuMod.Send_Mail(SO_DEFMAP, ".", que)

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "疵点マップ画面更新通知エラー [" + ex.Message.ToString + "]")
        Finally

        End Try
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
