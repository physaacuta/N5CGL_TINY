'*********************************************************************************
'　ダミー画面クラス
'	[Ver]
'		Ver.01    2012/07/19  初版
'
'	[メモ]
'		
'*********************************************************************************
Public Class frmDummy

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDummy_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        ' タイトル設定
        Call SetTitleName(Me)

    End Sub
End Class
