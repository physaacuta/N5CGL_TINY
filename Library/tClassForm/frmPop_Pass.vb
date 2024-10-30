'*********************************************************************************
' パスワード確認画面クラス
'	[Ver]
'		Ver.01    2007/08/28  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmPop_Pass


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ファンクションボタン押下イベント (ベースの置き換え)
    ''' </summary>
    ''' <param name="FncNo">0オリジン</param>
    ''' <remarks></remarks>
    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ プライベート変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_sPass As String = ""										   'パスワード

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="sPass">パスワード</param>
    ''' <param name="sPassChar">PasswordChar</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal sPass As String, Optional ByVal sPassChar As Char = Nothing, Optional ByVal nImeMode As Windows.Forms.ImeMode = Windows.Forms.ImeMode.Disable)

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        'パスワード設定
        m_sPass = sPass
        txtPass.PasswordChar = sPassChar
        txtPass.ImeMode = nImeMode

        lblMsg.Text = "パスワードを入力してください" & vbNewLine & "パスワードは" & m_sPass & "です。"

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Pass_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        'キャンセルボタンをアクティブにする
        btnF02.Select()
    End Sub

    ''' <summary>
    ''' ファンクションボタンクリック
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Pass_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

        'OKならパスワードチェック
        If 0 = FncNo Then

            If Not CheckPass() Then

				Dim frmMsg As New tClassForm.frmPop_MsgBox("パスワードが違います！", "エラー", tMod.GetColorToInt(Color.Red))
                frmMsg.ShowDialog()
                Me.DialogResult = Windows.Forms.DialogResult.Cancel
            Else
                Me.DialogResult = Windows.Forms.DialogResult.OK
            End If

        Else
            Me.DialogResult = Windows.Forms.DialogResult.Cancel
        End If

        Me.Close()

        '呼び元へ通知
        RaiseEvent tEv_FunctionButton(FncNo, onKey)

    End Sub
    ''' <summary>
    ''' パスワードチェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckPass() As Boolean

        '未入力なら終了
        If "" = txtPass.Text.Trim Then
            Return False
        End If

        'パスワードがあっているかチェック
        If StrConv(m_sPass, vbLowerCase) = txtPass.Text Or _
            StrConv(m_sPass, vbUpperCase) = txtPass.Text Or _
            StrConv(StrConv(m_sPass, vbLowerCase), vbWide) = txtPass.Text Or _
            StrConv(StrConv(m_sPass, vbUpperCase), vbWide) = txtPass.Text Then
            Return True
        End If

        Return False

    End Function

    
End Class
