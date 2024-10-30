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

Public Class frmPassword

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
    Private m_sPass As String = ""                                         'パスワード

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="sPass">パスワード</param>
    ''' <param name="sPassChar">PasswordChar</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal sPass As String, Optional ByVal sPassChar As Char = Nothing, Optional ByVal nImeMode As Windows.Forms.ImeMode = Windows.Forms.ImeMode.Disable, Optional ByVal sTitol As String = "")

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        'パスワード設定
        m_sPass = sPass
        txtPass.PasswordChar = sPassChar
        txtPass.ImeMode = nImeMode

        'パスワードファイルの設定値の確認
        If "" = m_sPass Then
            'パスワードファイル未設定またはパスワードが空白場合はOKボタン押下不可とする。
            txtPass.Enabled = False
            btnF01.Enabled = False
        End If

        If "" <> sTitol.Trim Then
            Me.Text = sTitol
        End If
        'lblMsg.Text = "パスワードを入力してください" & vbNewLine & "パスワードは" & m_sPass & "です。"

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Pass_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        'タイトル設定
        Call SetTitleName(Me)

        ''キャンセルボタンをアクティブにする
        'btnF02.Select()
        txtPass.Select()
    End Sub

    ''' <summary>
    ''' ファンクションボタンクリック
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Pass_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
        Dim strMsg As String

        'OKならパスワードチェック
        If 0 = FncNo Then

            If Not CheckPass() Then
                strMsg = "パスワードが違います！"
                strMsg = strMsg & vbNewLine & "再度,入力をして下さい。"
                Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "パスワード入力", tMod.GetColorToInt(Color.Red))
                frmMsg.ShowDialog()
                Exit Sub
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
            'パスワード未入力
            Return False
        End If

        'パスワードファイルの設定値の確認
        If "" = m_sPass Then
            'パスワードファイル未設定またはパスワードが空白
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


    Private Sub txtPass_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles txtPass.KeyDown
        If e.KeyData = Keys.Enter Then
            Call Me.frmPop_Pass_tEv_FunctionButton(0, e.KeyData)
        End If
    End Sub

End Class
