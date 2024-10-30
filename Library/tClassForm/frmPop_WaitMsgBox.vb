'*********************************************************************************
' 処理中表示画面クラス
'	[Ver]
'		Ver.01    2007/07/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On


Public Class frmPop_WaitMsgBox
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ファンクションボタン押下イベント (ベースの置き換え)
	''' </summary>
	''' <param name="dbMode">DBの状態</param>
	''' <remarks></remarks>
	Public Shadows Event tEv_FunctionButton(ByVal dbMode As Boolean)



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバー変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_bPrgBarAuto As Boolean							' プログレスバー 更新 (true:自動 false:ユーザー)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' メッセージ１
	''' </summary>
	Public WriteOnly Property tProp_Msg1(Optional ByVal index As Integer = 0) As String
		Set(ByVal Value As String)
			If index = 0 Then
				lblMsg1.Text = Value
			Else
				lblMsg2.Text = Value
			End If
		End Set
	End Property


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' プロブレスバー 自動更新 初期設定
	''' </summary>
	''' <param name="nPrgBarMaxVal">プログレスバー 最大値</param>
	''' <param name="nPrgBarTimer">プログレスバー 更新タイマー値 [s] bPrgBarAutoがtrue時のみ使用</param>
	''' <remarks></remarks>
	Public Sub SetPrgBarAutoStart(ByVal nPrgBarMaxVal As Integer, Optional ByVal nPrgBarTimer As Integer = 1000)
		m_bPrgBarAuto = True

		If ProgressBar1.Value > nPrgBarMaxVal Then
			ProgressBar1.Value = nPrgBarMaxVal
		End If
		ProgressBar1.Maximum = nPrgBarMaxVal


		' タイマースタート
		trmCycle.Interval = nPrgBarTimer * 1000
		trmCycle.Start()
	End Sub

	''' <summary>
	''' プロブレスバー 手動更新 初期設定
	''' </summary>
	''' <param name="nPrgBarMaxVal">プログレスバー 最大値</param>
	''' <remarks></remarks>
	Public Sub SetPrgBarUserStart(ByVal nPrgBarMaxVal As Integer)
		m_bPrgBarAuto = False

		ProgressBar1.Minimum = 0
		ProgressBar1.Value = 0
		ProgressBar1.Maximum = nPrgBarMaxVal
	End Sub

	''' <summary>
	''' プロブレスバー 更新
	''' </summary>
	''' <param name="nValue">現在値</param>
	''' <remarks></remarks>
	Public Sub SetPrgBarUserSet(ByVal nValue As Integer)
		If ProgressBar1.Maximum < nValue Then
			ProgressBar1.Value = ProgressBar1.Maximum
		ElseIf ProgressBar1.Minimum > nValue Then
			ProgressBar1.Value = ProgressBar1.Minimum
		Else
			ProgressBar1.Value = nValue
		End If
	End Sub

	''' <summary>
	''' 画面更新
	''' </summary>
	''' <remarks></remarks>
	Public Shadows Sub Refresh()
		Application.DoEvents()
		MyBase.Refresh()
	End Sub

	''' <summary>
	''' タイマー
	''' </summary>
	Private Sub trmCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trmCycle.Tick
		' プログレスバーセット
		If ProgressBar1.Value = ProgressBar1.Maximum Then
			ProgressBar1.Value = 0
		End If
		ProgressBar1.Increment(1)
	End Sub

	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_WaitMsgBox_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		Me.TopMost = True
	End Sub
End Class
