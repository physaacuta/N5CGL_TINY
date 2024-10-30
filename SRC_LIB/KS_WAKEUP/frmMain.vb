Public Class frmMain
	'/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー定数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Delegate Sub m_deleSetImgLoad(ByVal pc As Integer, ByVal rec As Integer)	   '別スレッドからのコントロールアクセス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nMyID As Integer                              ' 自ID (モニタ番号:0オリジン)

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メソッド定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <param name="nMyID">自ID (モニタ番号:0オリジン)</param>
	''' <remarks></remarks>
	Public Sub New(ByVal nMyID As Integer)

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。
		m_nMyID = nMyID
	End Sub

	''' <summary>
	''' 画面表示
	''' </summary>
	''' <remarks></remarks>
	Public Shadows Sub Show()
		SetImgLoadEntry(0, False)		' 統括
		SetImgLoadEntry(1, False)		' 蓄積

		'' 表示したいモニタの情報にあわせてフォームサイズを変更
		Dim monitor() As System.Windows.Forms.Screen
		monitor = System.Windows.Forms.Screen.AllScreens()
		Me.Bounds = monitor(m_nMyID).Bounds
		Me.TopMost = True				 '最前面		'やめデバック時に非常にやりにくいから


		'' 自画面設定
		'Me.Size = New Size(My.Computer.Screen.Bounds.Width, My.Computer.Screen.Bounds.Height)		' 画面きっちりにする
		' ''Me.TopMost = True				 '最前面		'やめデバック時に非常にやりにくいから

		'' マルチモニタ用に左右に表示
		'If m_nMyID = 0 Then		'左
		'	Me.Location = New Point(0, 0)
		'Else					'右
		'	Me.Location = New Point(Me.Size.Width, 0)
		'End If

		' ベース画面表示
		MyBase.Show()
	End Sub

	''' <summary>
	''' イメージをロードするエントリーメソッド
	''' </summary>
	''' <param name="pc">pc (0:統括 1:蓄積)</param>
	''' <param name="mode">PC ON/OFF</param>
    Public Sub SetImgLoadEntry(ByVal pc As Integer, ByVal mode As Boolean)
        Dim rec As Integer = CInt(IIf(mode, 1, 0))   'イメージリスト (0:OFF 1:ON)

		' 別スレッドからのアクセス可能とする
		If Me.InvokeRequired Then					' 別スレッドからのアクセス要求
			Dim dele As New m_deleSetImgLoad(AddressOf SetImgLoad)
			Me.Invoke(dele, pc, rec)
		Else										' フォーム生成スレッドからのアクセス要求
			Call SetImgLoad(pc, rec)
		End If

    End Sub




	''' <summary>
	''' イメージをロード
	''' </summary>
	''' <param name="pc">pc (0:統括 1:蓄積)</param>
	''' <param name="rec">イメージリスト (0:OFF 1:ON)</param>
	Private Sub SetImgLoad(ByVal pc As Integer, ByVal rec As Integer)
        If pc = modMain.em_Delivery.iTO Then                           ' 統括PC
            picTokatu.Image = imgList.Images(rec)
        ElseIf pc = modMain.em_Delivery.iTI Then                      ' 蓄積PC
            picTikuseki.Image = imgList.Images(rec)
        End If
	End Sub

	''' <summary>
	''' キーダウン
	''' </summary>
	Private Sub frmMain_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
		'If e.KeyValue = Keys.F12 AndAlso e.Modifiers = Keys.Shift Then
		'	'' 画面強制終了通知
		'	Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
		'	que.nEventNo = FACT_KS_END
		'	que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
		'	KizuLibCLI.KizuMod.Send_Mail(KizuLibCLI.KizuMod.GetMyTaskName(), ".", que)

		'ElseIf e.KeyValue = Keys.F11 AndAlso e.Modifiers = Keys.Shift Then	' 統括PCテスト
		'	Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
		'	que.nEventNo = FACT_KS_WAKEUP_01
		'	que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
		'	KizuLibCLI.KizuMod.Send_Mail(KizuLibCLI.KizuMod.GetMyTaskName(), ".", que)

		'ElseIf e.KeyValue = Keys.F10 AndAlso e.Modifiers = Keys.Shift Then	' 蓄積PCテスト
		'	Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
		'	que.nEventNo = FACT_KS_WAKEUP_02
		'	que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
		'	KizuLibCLI.KizuMod.Send_Mail(KizuLibCLI.KizuMod.GetMyTaskName(), ".", que)

		'ElseIf e.KeyValue = Keys.F1 AndAlso e.Modifiers = Keys.Shift Then	' 最全面切替
		'	Me.TopMost = Not Me.TopMost
		'End If

		If e.KeyValue = Keys.F12 AndAlso e.Modifiers = Keys.Shift Then
			'' 画面強制終了通知
			Dim que As COMMON_QUE_L = Nothing
			que.nEventNo = FACT_KS_END
			que.nLineNo = 0
			gcls_Mail.Mail_Send(".", KS_WAKEUP, que)

		ElseIf e.KeyValue = Keys.F11 AndAlso e.Modifiers = Keys.Shift Then	' 統括PCテスト
			Dim que As COMMON_QUE_L = Nothing
			que.nEventNo = FACT_KS_WAKEUP_01
			que.nLineNo = 0
			gcls_Mail.Mail_Send(".", KS_WAKEUP, que)

		ElseIf e.KeyValue = Keys.F10 AndAlso e.Modifiers = Keys.Shift Then	' 蓄積PCテスト
			Dim que As COMMON_QUE_L = Nothing
			que.nEventNo = FACT_KS_WAKEUP_02
			que.nLineNo = 0
			gcls_Mail.Mail_Send(".", KS_WAKEUP, que)

		ElseIf e.KeyValue = Keys.F1 AndAlso e.Modifiers = Keys.Shift Then	' 最全面切替
			Me.TopMost = Not Me.TopMost
		End If
	End Sub

	''' <summary>
	''' タイマー
	''' </summary>
	Private Sub trmCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trmCycle.Tick
		If ProgressBar1.Value = ProgressBar1.Maximum Then
			ProgressBar1.Value = 0
		End If
		ProgressBar1.Increment(1)
	End Sub

End Class
