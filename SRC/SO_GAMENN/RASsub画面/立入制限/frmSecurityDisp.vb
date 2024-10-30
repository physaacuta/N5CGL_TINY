Option Strict On

Imports tClassLibrary
Imports KizuForm

Public Class frmSecurityDisp

    '===========================================================
    ' 定数
    '===========================================================
    ' ====================
    ' 要素の文字
    ' ====================
    Private Const m_sNonString As String = "不明"     ' 不明時の色
    Private Const m_sOpenString As String = "開"      ' 開状態時の色
    Private Const m_sCloseString As String = "閉"     ' 閉状態時の色

    ' ====================
    ' タイマー時間
    ' ====================
    Private Const ERR_CHECK_TIME As Integer = 500     'エラー項目ﾌﾞﾘﾝｸ処理タイマー
    Private m_lblData As New List(Of Control)

    '===========================================================
    ' 列挙型
    '===========================================================
    ' ====================
    ' 要素の状態
    ' ====================
    Enum EM_STATUS
        emNon = -1                                  ' 不明
        emClose = 0                                 ' 閉状態（正常）
        emOpen = 1                                  ' 開状態（異常）
    End Enum

    '===========================================================
    ' メンバー変数
    '===========================================================
    Private m_nData As Integer()                ' 実データ

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        'タイトル設定
        modMain.SetTitleName(Me)

        ' 表示間隔を0.5秒間隔にする
        timDisp.Interval = ERR_CHECK_TIME
        SetLabel()
        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)
    End Sub

    Private Sub SetLabel()
        m_lblData.Add(lblStat11)    ' ｻｰﾊﾞｰﾗｯｸ・制御盤　扉状態
        m_lblData.Add(lblStat12)    ' ｻｰﾊﾞｰﾗｯｸ・制御盤　鍵状態
    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    cmdEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' 閉じるボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.Close()
    End Sub

    ''' <summary>
    ''' ラベルにデータをセット
    ''' </summary>
    ''' <param name="strName">名前</param>
    ''' <param name="nStat">機器状態</param>
    ''' <remarks></remarks>
    Public Sub SetData(ByVal strName As String, ByVal nStat As Integer, ByVal clrDspColor As Color)
        ' 機器IDが同じな場合
        For ii As Integer = 0 To m_lblData.Count - 1
            If m_lblData(ii).Name = strName Then
                ' 機器IDが同じな場合
                Select Case nStat
                    Case EM_STATUS.emClose      ' 閉状態
                        m_lblData(ii).Text = m_sCloseString
                    Case EM_STATUS.emOpen       ' 開状態
                        m_lblData(ii).Text = m_sOpenString
                    Case Else                   ' その他
                        m_lblData(ii).Text = m_sNonString
                End Select
                m_lblData(ii).BackColor = clrDspColor
            End If
        Next
    End Sub

    ''' <summary>
    ''' F02ボタンの状態を返す
    ''' </summary>
    ''' <returns>true:F02正常 false:F02異常</returns>
    ''' <remarks></remarks>
    Public Function ChkF02btn() As Boolean
        Dim nRet As Boolean = True  ' 戻り値

        ' 全ラベルループ
        For ii As Integer = 0 To m_lblData.Count - 1
            ' 状態が閉状態以外の場合
            If m_sCloseString <> m_lblData(ii).Text Then
                ' 戻り値FALSE
                nRet = False
                Exit For
            End If
        Next
        Return nRet
    End Function
End Class