'*********************************************************************************
'　探傷画面 専用？ ステータス情報表示コントロール
'	[Ver]
'		Ver.01    2008/10/09  初版
'
'	[メモ]
'		
'*********************************************************************************

Option Strict On
Imports tClassLibrary

Public Class ucStatusAria

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_bIsDbErr As Boolean = False										' DB異常フラグ (true:異常)
	Private Shared WithEvents m_tmGuidance As Timer = New Timer()				' ガイダンスクリアタイマー 
	Private Const m_nGuidanceTime As Integer = 10000							' ガイダンス時間(10秒)
	Private Shared bGuidance As Boolean = False									' ガイダンスフラグ


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開プロパティー
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' DB異常フラグ (true:異常)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property tProp_IsDbErr() As Boolean
		Get
			Return m_bIsDbErr
		End Get
		Set(ByVal Value As Boolean)
			m_bIsDbErr = Value
		End Set
	End Property



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開 メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////


	''' <summary>
	''' 全部初期化
	''' </summary>
	''' <remarks></remarks>
	Public Sub DspInit()
		' ガイダンス情報表示
		InitLabel(lblGaidans)
		' 稼動状態表示
		InitLabel(lblKadou)
		' 検査状態表示
		InitLabel(lblKensa)
		' 機器状態表示
		InitLabel(lblKiki)
	End Sub


	''' <summary>
	''' 全部 更新
	''' </summary>
	''' <param name="ou">表裏区分 (0:表 1:裏)</param>
	''' <param name="emUnten">運転状態</param>
	''' <param name="emKadou">稼動状態</param>
	''' <param name="nowstatus">ステータス情報テーブル</param>
	''' <param name="bDbErr">DB異常状態(内部フラグを使わない)</param>
	''' <remarks></remarks>
	Public Overloads Sub DspData(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU, ByVal nowstatus As TYP_STATUS_NOWSYSTEM, ByVal bDbErr As Boolean)
		If bDbErr Then
			' DB異常
			Call DspDBErr(ou, emUnten, emKadou)
		Else
			' DB正常
			Call DspNomal(ou, emUnten, emKadou, nowstatus)
		End If
	End Sub
	Public Overloads Sub DspData(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU, ByVal nowstatus As TYP_STATUS_NOWSYSTEM)
		If m_bIsDbErr Then
			' DB異常
			Call DspDBErr(ou, emUnten, emKadou)
		Else
			' DB正常
			Call DspNomal(ou, emUnten, emKadou, nowstatus)
		End If
	End Sub

	''' <summary>
	''' DB正常時 の更新
	''' </summary>
	''' <param name="ou">表裏区分 (0:表 1:裏)</param>
	''' <param name="emUnten">運転状態</param>
	''' <param name="emKadou">稼動状態</param>
	''' <param name="nowstatus">ステータス情報テーブル</param>
	''' <remarks></remarks>
	Public Sub DspNomal(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU, ByVal nowstatus As TYP_STATUS_NOWSYSTEM)
		' ガイダンス情報表示
		DspGaidans(lblGaidans, ou, nowstatus)
		' 稼動状態表示
		DspKadou(lblKadou, emKadou, emUnten)
		' 検査状態表示
		DspKensa(lblKensa, CType(nowstatus.st_kensa(ou), EM_DIV_KENSA))
		' 機器状態表示
		DspKiki(lblKiki, CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))
	End Sub


	''' <summary>
	''' DB異常時 の更新
	''' </summary>
	''' <param name="ou">表裏区分 (0:表 1:裏)</param>
	''' <param name="emUnten">運転状態</param>
	''' <param name="emKadou">稼動状態</param>
	''' <remarks></remarks>
	Public Sub DspDBErr(ByVal ou As Integer, ByVal emUnten As EM_DIV_UNTEN, ByVal emKadou As EM_DIV_KADOU)
		' 自分で生成
		Dim nowstatus As TYP_STATUS_NOWSYSTEM = Nothing
		nowstatus.initialize()
		nowstatus.nCnt = 1
		nowstatus.typErr(0).msg = "データベース接続異常が発生したため、検査できません。"
		nowstatus.st_kensa(ou) = EM_DIV_KENSA.DIV_KENSA_NG
		nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_JYU

		' 全部表示
		DspNomal(ou, emUnten, emKadou, nowstatus)
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 自イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Sub ucStatusAria_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' 全部初期化
		DspInit()
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 静的 メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' ラベルを初期化する
	''' </summary>
	''' <param name="lbl"></param>
	''' <remarks></remarks>
	Public Shared Sub InitLabel(ByVal lbl As Label)
		lbl.Text = ""
		lbl.BackColor = g_ColorControl
	End Sub


	''' <summary>
	''' ガイダンス情報表示
	''' </summary>
	''' <param name="lbl">ガイダンスラベル</param>
	''' <param name="ou">表裏区分 (0:表 1:裏)</param>
	''' <param name="nowstatus">ステータス情報テーブル</param>
	''' <remarks></remarks>
	Public Shared Sub DspGaidans(ByVal lbl As Label, ByVal ou As Integer, ByVal nowstatus As TYP_STATUS_NOWSYSTEM)
		If 0 = nowstatus.nCnt Then
			If m_tmGuidance Is Nothing Then
				lbl.Text = ""
				lbl.BackColor = g_ColorControl
				bGuidance = False
			End If
		Else

			If lbl.Text <> nowstatus.typErr(0).msg Then bGuidance = False ' メッセージが違う場合は、ガイダンスフラグをFALSE

			If Not bGuidance Then								' ガイダンスフラグをTRUEの場合は処理しない
				lbl.Text = nowstatus.typErr(0).msg				' メッセージセット
				If m_tmGuidance Is Nothing Then
					m_tmGuidance = New Timer()					' タイマオブジェクト生成
				Else
					m_tmGuidance.Stop()							' タイマストップ
				End If
				m_tmGuidance.Enabled = True
				m_tmGuidance.Interval = m_nGuidanceTime			' タイマー時間セット
				m_tmGuidance.Start()							' タイマー起動
				bGuidance = True								' ガイダンスフラグをTRUE
			End If

			'If nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_OK Then
			'	' 自面が正常でも、全体として異常が出ているので、とりあえず軽故障色を出力
			'	lbl.BackColor = DspKikiBackColor(EM_DIV_KIKI.DIV_KIKI_KEI)
			'Else
			'	lbl.BackColor = DspKikiBackColor(CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))
			'End If

			If nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_JYU Then
				lbl.BackColor = DspKikiBackColor(CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))
			ElseIf nowstatus.st_kensa(ou) = 6 Or _
			nowstatus.st_kensa(ou) = 5 Or _
			nowstatus.st_kensa(ou) = 4 Or _
			nowstatus.st_kensa(ou) = EM_DIV_KENSA.DIV_KENSA_NG Then
				lbl.BackColor = DspKensaBackColor(CType(nowstatus.st_kensa(ou), EM_DIV_KENSA))

			ElseIf nowstatus.st_kiki(ou) = EM_DIV_KIKI.DIV_KIKI_KEI Then
				lbl.BackColor = DspKikiBackColor(CType(nowstatus.st_kiki(ou), EM_DIV_KIKI))

			ElseIf nowstatus.st_kiki(ou) <> EM_DIV_KIKI.DIV_KIKI_OK Or nowstatus.st_kensa(ou) <> EM_DIV_KENSA.DIV_KENSA_OK Then
				' 自分の面で何かの異常がある
				lbl.BackColor = DspKikiBackColor(EM_DIV_KIKI.DIV_KIKI_KEI)
			Else
				' 自面が正常でも、全体として異常が出ているので、とりあえず軽故障色を出力
				lbl.BackColor = DspKikiBackColor(EM_DIV_KIKI.DIV_KIKI_KEI)
			End If

		End If
	End Sub

	''' <summary>
	''' ガイダンス情報表示保持タイマ
	''' </summary>
	''' <param name="sender">イベント発生元</param>
	''' <param name="e">イベントパラメータ</param>
	''' <remarks></remarks>
	Private Shared Sub m_tmGuidance_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_tmGuidance.Tick
		m_tmGuidance.Stop()			' タイマーストップ
		m_tmGuidance.Dispose()		' タイマー
		m_tmGuidance = Nothing		' タイマー解放
	End Sub

	''' <summary>
	''' 運転状態表示
	''' </summary>
	''' <param name="emUnten">運転状態</param>
	''' <remarks></remarks>
	Public Shared Function DspUnten(ByVal emUnten As EM_DIV_UNTEN) As String
		Select Case emUnten
			Case EM_DIV_UNTEN.DIV_UNTEN_STOP
				Return "停止"
			Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL
				Return "通常運転"
			Case EM_DIV_UNTEN.DIV_UNTEN_SAMP
				Return "ｻﾝﾌﾟﾙ撮影"
			Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA
				Return "ｶﾒﾗ調整"
			Case EM_DIV_UNTEN.DIV_UNTEN_SMYU
				Return "ｼﾐｭﾚｰｼｮﾝ"
			Case Else
				Return ""
		End Select
	End Function

	''' <summary>
	''' 稼動状態表示
	''' </summary>
	''' <param name="lbl">稼動ラベル</param>
	''' <param name="emKadou">稼動状態</param>
	''' <param name="emUnten">運転状態</param>
	''' <remarks></remarks>
	Public Shared Sub DspKadou(ByVal lbl As Label, ByVal emKadou As EM_DIV_KADOU, ByVal emUnten As EM_DIV_UNTEN)

		' 稼動状態
		Select Case emKadou
			Case EM_DIV_KADOU.DIV_KADOU_INIT
				lbl.Text = "初期化中"
				lbl.BackColor = g_ColorKadouInit
			Case EM_DIV_KADOU.DIV_KADOU_STOP
				lbl.Text = "停止中"
				lbl.BackColor = g_ColorKadouStop
			Case EM_DIV_KADOU.DIV_KADOU_START

				' 運転状態を見て判断する
				If emUnten = EM_DIV_UNTEN.DIV_UNTEN_NOMAL Then
					lbl.Text = "稼動中"
					lbl.BackColor = g_ColorKadouStart
				Else
					lbl.Text = "テスト中"
					lbl.BackColor = g_ColorKadouStart
				End If

			Case EM_DIV_KADOU.DIV_KADOU_RETRY
				lbl.Text = "ﾘﾄﾗｲ中"
				lbl.BackColor = g_ColorKadouRetry
			Case Else
				lbl.Text = "不明"
				lbl.BackColor = g_ColorControl
		End Select
	End Sub


	''' <summary>
	''' 検査状態表示
	''' </summary>
	''' <param name="lbl">検査ラベル</param>
	''' <param name="emKensa">検査状態</param>
	''' <remarks></remarks>
	Public Shared Sub DspKensa(ByVal lbl As Label, ByVal emKensa As EM_DIV_KENSA)
		lbl.Text = DspKensa(emKensa)
		lbl.BackColor = DspKensaBackColor(emKensa)
		lbl.ForeColor = DspKensaForeColor(emKensa)
	End Sub


	''' <summary>
	''' 機器状態表示
	''' </summary>
	''' <param name="lbl">機器ラベル</param>
	''' <param name="emKiki">機器状態</param> 
	''' <remarks></remarks>
	Public Shared Sub DspKiki(ByVal lbl As Label, ByVal emKiki As EM_DIV_KIKI)
		lbl.Text = DspKiki(emKiki)
		lbl.BackColor = DspKikiBackColor(emKiki)
		lbl.ForeColor = DspKikiForeColor(emKiki)
	End Sub




	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ その他もろもろ 静的 メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' 表裏区分の表示
	''' </summary>
	''' <param name="nOU">表裏区分 (0:表 1:裏)</param>
	''' <param name="lbl">対象ラベル</param>
	''' <remarks></remarks>
	Public Shared Sub DspTorB(ByVal nOU As Integer, ByVal lbl As Label)
		If nOU = 0 Then
			lbl.Text = "表面"
		Else
			lbl.Text = "裏面"
		End If
		lbl.BackColor = Color.Blue
		lbl.ForeColor = Color.Yellow
	End Sub

	''' <summary>
	''' 一時停止／自動更新区分の表示
	''' </summary>
	''' <param name="bIsStop">一時停止 (true:一時停止 false:自動更新)</param>
	''' <param name="lbl">対象ラベル</param>
	''' <param name="btn">対象ボタン</param>
	''' <remarks></remarks>
	Public Shared Sub DspStopMode(ByVal bIsStop As Boolean, ByVal lbl As Label, ByVal btn As Button)
		If bIsStop Then
			lbl.Text = "一時停止中"
			lbl.BackColor = Color.Red
			lbl.ForeColor = Color.White
			btn.Text = "自動更新"
		Else
			lbl.Text = "自動更新"
			lbl.BackColor = Color.Blue
			lbl.ForeColor = Color.Yellow
			btn.Text = "一時停止中"
		End If
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 基本
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' 検査状態名称
	''' </summary>
	''' <param name="emKensa">検査状態</param>
	Public Shared Function DspKensa(ByVal emKensa As EM_DIV_KENSA) As String
		' 検査状態
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return ""
			Case CType(-1, EM_DIV_KENSA)
				Return "停止"
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return "正常"
			Case EM_DIV_KENSA.DIV_KENSA_NG
				Return "縮退"
			Case EM_DIV_KENSA.DIV_KENSA_MENTE
				Return "ﾒﾝﾃ中"
			Case CType(4, EM_DIV_KENSA)
				Return "ｴｯｼﾞ不正"
			Case CType(5, EM_DIV_KENSA)
				Return "未保存"
			Case CType(6, EM_DIV_KENSA)
				Return "未検査"
			Case EM_DIV_KENSA.DIV_KENSA_STOP
				Return "停止"
			Case Else
				Return "不明"
		End Select
	End Function


	''' <summary>
	''' 検査状態 背景色
	''' </summary>
	''' <param name="emKensa">検査状態</param>
	Public Shared Function DspKensaBackColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' 検査状態 背景色
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return g_ColorKensaNon
			Case CType(-1, EM_DIV_KENSA)
				Return g_ColorKensaNon
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return g_ColorKensaOk
			Case EM_DIV_KENSA.DIV_KENSA_NG
				Return g_ColorKensaNg
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' 現在プロビジョン
				Return g_ColorMente
			Case CType(4, EM_DIV_KENSA)
				Return g_ColorKensaEdgeNg
			Case CType(5, EM_DIV_KENSA)
				Return g_ColorKensaDbNon
			Case CType(6, EM_DIV_KENSA)
				Return g_ColorKensaSkip
			Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return g_ColorKensaNon
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' 検査状態 文字色
	''' </summary>
	''' <param name="emKensa">検査状態</param>
	Public Shared Function DspKensaForeColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' 検査状態 背景色
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return Color.Black
			Case CType(-1, EM_DIV_KENSA)
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return Color.Yellow
			Case EM_DIV_KENSA.DIV_KENSA_NG
                Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' 現在プロビジョン
                Return Color.Black
			Case CType(4, EM_DIV_KENSA)
				Return Color.Black
			Case CType(5, EM_DIV_KENSA)
				Return Color.Black
			Case CType(6, EM_DIV_KENSA)
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return Color.Black
			Case Else
				Return Color.Black
		End Select
	End Function





	''' <summary>
	''' 機器状態名称
	''' </summary>
	''' <param name="emKiki">機器状態</param>
	Public Shared Function DspKiki(ByVal emKiki As EM_DIV_KIKI) As String
		' 検査状態
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return "正常"
			Case EM_DIV_KIKI.DIV_KIKI_KEI
				Return "軽故障"
			Case EM_DIV_KIKI.DIV_KIKI_JYU
				Return "重故障"
			Case Else
				Return "不明"
		End Select
	End Function

	''' <summary>
	''' 機器状態背景色
	''' </summary>
	''' <param name="emKiki">機器状態</param>
	Public Shared Function DspKikiBackColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' 検査状態
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return g_ColorKikiOk
			Case EM_DIV_KIKI.DIV_KIKI_KEI
				Return g_ColorKikiKei
			Case EM_DIV_KIKI.DIV_KIKI_JYU
				Return g_ColorKikiJyu
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' 機器状態文字色
	''' </summary>
	''' <param name="emKiki">機器状態</param>
	Public Shared Function DspKikiForeColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' 検査状態
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return Color.Yellow
			Case EM_DIV_KIKI.DIV_KIKI_KEI
                Return Color.Black
			Case EM_DIV_KIKI.DIV_KIKI_JYU
                Return Color.Black
			Case Else
                Return Color.Black
		End Select
	End Function


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 疵マップ
	'/ /////////////////////////////////////////////////////////////////////////////////


	''' <summary>
	''' 検査状態 背景色
	''' </summary>
	''' <param name="emKensa">検査状態</param>
	Public Shared Function MapKensaBackColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' 検査状態 背景色
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return g_ColorKensaNon
			Case CType(-1, EM_DIV_KENSA)
				Return g_ColorKensaStop
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return g_ColorKensaOk
			Case EM_DIV_KENSA.DIV_KENSA_NG
				Return g_ColorKensaNg
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' 現在プロビジョン
				Return g_ColorMente
			Case CType(4, EM_DIV_KENSA)
				Return g_ColorKensaEdgeNg
			Case CType(5, EM_DIV_KENSA)
				Return g_ColorKensaDbNon
			Case CType(6, EM_DIV_KENSA)
				Return g_ColorKensaSkip
			Case EM_DIV_KENSA.DIV_KENSA_STOP
                Return g_ColorKensaStop
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' 検査状態 文字色
	''' </summary>
	''' <param name="emKensa">検査状態</param>
	Public Shared Function MapKensaForeColor(ByVal emKensa As EM_DIV_KENSA) As Color
		' 検査状態 背景色
		Select Case emKensa
			Case EM_DIV_KENSA.DIV_KENSA_NON
				Return Color.Black
			Case CType(-1, EM_DIV_KENSA)
				Return Color.White
			Case EM_DIV_KENSA.DIV_KENSA_OK
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_NG
                Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_MENTE	' 現在プロビジョン
                Return Color.Black
			Case CType(4, EM_DIV_KENSA)
				Return Color.Black
			Case CType(5, EM_DIV_KENSA)
				Return Color.Black
			Case CType(6, EM_DIV_KENSA)
				Return Color.Black
			Case EM_DIV_KENSA.DIV_KENSA_STOP
				Return Color.White
			Case Else
				Return Color.Black
		End Select
	End Function
	''' <summary>
	''' 機器状態背景色
	''' </summary>
	''' <param name="emKiki">機器状態</param>
	Public Shared Function MapKikiBackColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' 検査状態
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
				Return g_ColorKikiOk
			Case EM_DIV_KIKI.DIV_KIKI_KEI
				Return g_ColorKikiKei
			Case EM_DIV_KIKI.DIV_KIKI_JYU
				Return g_ColorKikiJyu
			Case Else
				Return g_ColorControl
		End Select
	End Function

	''' <summary>
	''' 機器状態文字色
	''' </summary>
	''' <param name="emKiki">機器状態</param>
	Public Shared Function MapKikiForeColor(ByVal emKiki As EM_DIV_KIKI) As Color
		' 検査状態
		Select Case emKiki
			Case EM_DIV_KIKI.DIV_KIKI_OK
                Return Color.Black
			Case EM_DIV_KIKI.DIV_KIKI_KEI
                Return Color.Black
			Case EM_DIV_KIKI.DIV_KIKI_JYU
                Return Color.Black
			Case Else
                Return Color.Black
		End Select
	End Function


End Class
