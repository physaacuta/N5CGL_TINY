Imports System.ComponentModel

Public Class frmPopGridLine
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    Public Event tEv_RePaint()                                                          ' 再描画

    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 公開変数
    Public g_nX As Integer                  ' X間隔 [mm]
    Public g_nY As Integer                  ' Y間隔 [mm]
    Public g_nOffsetX As Integer            ' Xズラス [mm]
    Public g_nOffsetY As Integer            ' Yズラス [mm]

    Private m_nDefVal As Integer = 0        ' デフォルト値 [mm] ※ピッチ指定時はピッチ


    ''/ /////////////////////////////////////////////////////////////////////////////////
    '' コイルチェンジ
    ''' <summary>
    ''' コイル初期値
    ''' </summary>
    ''' <param name="nJumpVal">ジャンプ値 [mm]</param>
    ''' <remarks></remarks>
    Public Sub ChangeDefVal(ByVal nJumpVal As Integer)
        If 0 >= nJumpVal Then nJumpVal = CInt(nudY.Value)

        m_nDefVal = nJumpVal
        nudY.Value = nJumpVal
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 処理
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPop_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True

        ' 初期値セット
        Call SetData()
    End Sub

    Protected Overrides Sub OnClosing(e As CancelEventArgs)
        MyBase.OnClosing(e)
        RaiseEvent tEv_RePaint()
    End Sub

    '''' <summary>
    '''' 結果反映
    '''' </summary>
    'Private Sub btnExec_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExec.Click
    '	Call SetData()
    'End Sub

    ''' <summary>
    ''' データセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetData()
        g_nX = CInt(nudX.Value)
        g_nY = CInt(nudY.Value)
        g_nOffsetX = CInt(trbOffsetX.Value)
        g_nOffsetY = CInt(trbOffsetY.Value)

        ' イベント通知
        RaiseEvent tEv_RePaint()
    End Sub

    ''' <summary>
    ''' 値変更
    ''' </summary>
    Private Sub nud_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudY.ValueChanged, nudX.ValueChanged, trbOffsetY.ValueChanged, trbOffsetX.ValueChanged
        Call SetData()
    End Sub


    ''' <summary>
    ''' 初期値
    ''' </summary>
    Private Sub btnReLoad_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReLoad.Click
        ' 設定
        nudY.Value = m_nDefVal
    End Sub

End Class