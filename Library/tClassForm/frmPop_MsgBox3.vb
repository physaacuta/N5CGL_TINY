'*********************************************************************************
' メッセージボックスの代わりとなるクラス
'	[Ver]
'		Ver.01    2007/07/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_MsgBox3
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ファンクションボタン押下イベント (ベースの置き換え)
	''' </summary>
	''' <param name="FncNo">0オリジン</param>
	''' <remarks></remarks>
	Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)


	''' メンバ変数
	Private m_bBtnMode(2) As Boolean				' 初期ボタン状態


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 公開メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <param name="strMsg"></param>
	''' <param name="strTaitle"></param>
	''' <remarks></remarks>
	Public Sub New(ByVal strMsg As String, Optional ByVal strTaitle As String = "", Optional ByVal msgColor As Integer = 0, _
	 Optional ByVal btnL As Boolean = True, Optional ByVal btnC As Boolean = True, Optional ByVal btnR As Boolean = True, _
	 Optional ByVal retL As DialogResult = DialogResult.None, _
	 Optional ByVal retC As DialogResult = DialogResult.None, _
	 Optional ByVal retR As DialogResult = DialogResult.None)

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。
		lblMsg.Text = strMsg
		lblMsg.ForeColor = Color.FromArgb(msgColor)
		Me.Text = strTaitle

		m_bBtnMode(0) = btnL
		m_bBtnMode(1) = btnC
		m_bBtnMode(2) = btnR
		btnF01.DialogResult = retL
		btnF02.DialogResult = retC
		btnF03.DialogResult = retR
	End Sub
	''' <summary>
	''' メッセージ変更
	''' </summary>
	''' <param name="strMsg">メッセージ変更</param>
	''' <remarks></remarks>
	Public Sub ChangeMsg(ByVal strMsg As String, Optional ByVal msgColor As Integer = 0, Optional ByVal btnL As Boolean = True, Optional ByVal btnC As Boolean = True, Optional ByVal btnR As Boolean = True)
		lblMsg.Text = strMsg
		lblMsg.ForeColor = Color.FromArgb(msgColor)
		m_bBtnMode(0) = btnL
		m_bBtnMode(1) = btnC
		m_bBtnMode(2) = btnR
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_MsgBox_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' 初期化
		Me.TopLevel = True						 ' 最前面

		Me.SetButtonEnable(0, m_bBtnMode(0))
		Me.SetButtonEnable(1, m_bBtnMode(1))
		Me.SetButtonEnable(2, m_bBtnMode(2))

	End Sub
	''' <summary>
	''' ファンクションボタンクリック
	''' </summary>
	''' <param name="FncNo"></param>
	''' <param name="onKey"></param>
	''' <remarks></remarks>
	Private Sub frmPop_MsgBox_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

		Select Case FncNo

			Case 0
				Me.DialogResult = btnF01.DialogResult
			Case 4
				Me.DialogResult = btnF02.DialogResult
			Case 11
				Me.DialogResult = btnF03.DialogResult
		End Select

		' 画面の終了 (インスタンスはそのままだからね)
		Me.Close()

		' 呼び元へ通知
		RaiseEvent tEv_FunctionButton(FncNo, onKey)
	End Sub

End Class
