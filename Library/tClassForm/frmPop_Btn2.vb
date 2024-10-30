'*********************************************************************************
' ポップアップ２ボタン画面クラス
'	[Ver]
'		Ver.01    2008/02/01  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_Btn2

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmPop_Btn2_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' 定義
		MyBase.AddFunctionButton(btnF01, 0)
		MyBase.AddFunctionButton(btnF02, 11)
	End Sub
End Class
