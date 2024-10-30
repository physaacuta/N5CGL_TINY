'*******************************************************************************
'   表示設定画面
'
'  [Ver]
'       Ver.01  2017/09/08  初版
'
'	[メモ]
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Public Class frmSetting

    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)

    '///////////////////////////////////////////////////////////////////////////
    '定数
    '///////////////////////////////////////////////////////////////////////////
    Private Const M_C_TIMER_LIST As Integer = 12                     ' タイマーコンボボックスリスト数(5秒刻みで60秒まで表示したいから12)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public g_nStopTime As Integer = 0                               ' 一時停止解除タイマー秒数
    Public g_nPopTimerVal As Integer = 0                            ' ポップアップ画像表示解除タイマー秒数

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンボボックスに表示するデータ
    ''' </summary>
    ''' <remarks>ポップアップクローズ時間</remarks>
    Public Class cls_CmbItem

        ''' <summary>秒数</summary>
        Public time As Integer                              '秒数
        ''' <summary>表示名</summary>
        Public name As String                               '表示名

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="nTime">秒数</param>
        ''' <param name="strName">表示名</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal nTime As Integer, ByVal strName As String)
            time = nTime
            name = strName
        End Sub

        ''' <summary>
        ''' 構造体の表示名称
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return name
        End Function
    End Class


    ''' <summary>
    ''' 画面表示コンボボックスに表示するデータ
    ''' </summary>
    ''' <remarks>画面表示枚数</remarks>
    Public Class cls_CmbPatternItem

        ''' <summary>パターン</summary>
        Public pattrn As modMain.EM_DSP_PATTERN        'パターン
        ''' <summary>表示名</summary>
        Public name As String           '表示名

        ''' <summary>
        ''' コンストラクタ
        ''' </summary>
        ''' <param name="nPattern">パターン</param>
        ''' <param name="strName">表示名</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal nPattern As modMain.EM_DSP_PATTERN, ByVal strName As String)
            pattrn = nPattern
            name = strName
        End Sub

        ''' <summary>
        ''' 構造体の表示名称
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return name
        End Function
    End Class


    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ポップアップ画像表示解除タイマー秒数取得／設定
    ''' </summary>
    ''' <returns></returns>
    Public Property PopUpCloseTime() As Integer
        Get
            Return g_nPopTimerVal
        End Get
        Set(value As Integer)
            g_nPopTimerVal = value
            'コンボボックス選択状態変更
            For Each cls_cmbData As cls_CmbItem In cmbPopUp.Items
                If cls_cmbData.time = g_nPopTimerVal Then
                    cmbPopUp.SelectedIndex = cmbPopUp.Items.IndexOf(cls_cmbData)
                    Exit For
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 一時停止解除タイマー秒数取得／設定
    ''' </summary>
    ''' <returns></returns>
    Public Property StopCloseTime() As Integer
        Get
            Return g_nStopTime
        End Get
        Set(value As Integer)
            g_nStopTime = value
            'コンボボックス選択状態変更
            For Each cls_cmbData As cls_CmbItem In cmbStop.Items
                If cls_cmbData.time = g_nStopTime Then
                    cmbStop.SelectedIndex = cmbStop.Items.IndexOf(cls_cmbData)
                    Exit For
                End If
            Next
        End Set
    End Property

    '---------------------------------------------------------------------------
    ' メソッド
    '---------------------------------------------------------------------------

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        'ポップアップクローズタイマー値セット
        cmbPopUp.Items.Clear()
        cmbPopUp.Items.Add(New cls_CmbItem(0, "なし"))
        For ii As Integer = 1 To M_C_TIMER_LIST    'リストの先頭は「なし」の為、最初は飛ばす
            Dim str_name As String = CStr(ii * 5) & "秒"
            cmbPopUp.Items.Add(New cls_CmbItem(5 * ii, str_name))
        Next ii
        If -1 = cmbPopUp.SelectedIndex Then cmbPopUp.SelectedIndex = 0

        '一時停止解除タイマー値セット
        cmbStop.Items.Clear()
        cmbStop.Items.Add(New cls_CmbItem(0, "なし"))
        For ii As Integer = 1 To M_C_TIMER_LIST    'リストの先頭は「なし」の為、最初は飛ばす
            Dim str_name As String = CStr(ii * 5) & "秒"
            cmbStop.Items.Add(New cls_CmbItem(5 * ii, str_name))
        Next ii
        If -1 = cmbStop.SelectedIndex Then cmbStop.SelectedIndex = 0

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSetting_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        btnF01.DialogResult = Windows.Forms.DialogResult.OK
        btnF02.DialogResult = Windows.Forms.DialogResult.Cancel

    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタンクリック
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

        Select Case FncNo

            Case 0

                Me.DialogResult = btnF01.DialogResult
                '表示データを設定値に反映する。
                Call Set_Setting()
            Case 11
                Me.DialogResult = btnF02.DialogResult
                'キャンセルの場合、表示を元の設定値に戻す。
                Call Cancel_Setting()

        End Select

        Me.Hide()

        '呼び元へ通知
        RaiseEvent tEv_FunctionButton(FncNo, onKey)
    End Sub

    ''' <summary>
    ''' ＯＫ(表示データを設定値に反映する)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Set_Setting()
        g_nStopTime = CType(cmbStop.Items(cmbStop.SelectedIndex), cls_CmbItem).time
        g_nPopTimerVal = CType(cmbPopUp.Items(cmbPopUp.SelectedIndex), cls_CmbItem).time
    End Sub


    ''' <summary>
    ''' キャンセル(表示を元の設定値に戻す。)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Cancel_Setting()
        '一時停止タイマー
        For Each cls_cmbData As cls_CmbItem In cmbStop.Items
            If cls_cmbData.time = g_nStopTime Then
                cmbStop.SelectedIndex = cmbStop.Items.IndexOf(cls_cmbData)
                Exit For
            End If
        Next
        'ポップアップ表示解除タイマー
        For Each cls_cmbData As cls_CmbItem In cmbPopUp.Items
            If cls_cmbData.time = g_nPopTimerVal Then
                cmbPopUp.SelectedIndex = cmbPopUp.Items.IndexOf(cls_cmbData)
                Exit For
            End If
        Next
    End Sub


End Class
