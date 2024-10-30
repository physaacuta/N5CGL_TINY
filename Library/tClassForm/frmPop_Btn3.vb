'*********************************************************************************
' ポップアップ３ボタン画面クラス
'	[Ver]
'		Ver.01    2008/02/01  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_Btn3

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ボタン有効無効切替
	''' </summary>
	''' <param name="Index">インデックス (0オリジン)</param>
	''' <param name="bEnable">有効無効</param>
	''' <remarks></remarks>
	Public Sub SetButtonEnable(ByVal Index As Integer, ByVal bEnable As Boolean)
		Select Case Index
			Case 0
				btnF01.Enabled = bEnable
			Case 1
				btnF02.Enabled = bEnable
			Case 2
				btnF03.Enabled = bEnable
		End Select
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
	Private Sub frmPop_Btn3_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		btnF01.Enabled = False
		btnF02.Enabled = False
		btnF03.Enabled = False

		' 定義
		MyBase.AddFunctionButton(btnF01, 0)
		MyBase.AddFunctionButton(btnF02, 4)
		MyBase.AddFunctionButton(btnF03, 11)
	End Sub


End Class