Public Class CamName

    'Private mcnt_grb(3 - 1) As GroupBox
    'Private mcnt_lbl(3 - 1) As Label
    Private mcnt_grb(2 - 1) As GroupBox
    Private mcnt_lbl(2 - 1) As Label
	Public Sub New()

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。

		mcnt_grb(0) = grbCam1
		mcnt_grb(1) = grbCam2
        'mcnt_grb(2) = grbSide1
		mcnt_lbl(0) = lblCam1
		mcnt_lbl(1) = lblCam2
        'mcnt_lbl(2) = lblSide1

	End Sub

	''' <summary>
	''' エリア表示非表示設定
	''' </summary>
	''' <param name="nIndex">どっち(0オリジン)</param>
	''' <param name="bMode">表示非表示</param>
	''' <remarks></remarks>
	Public Sub VisibleNameAria(ByVal nIndex As Integer, ByVal bMode As Boolean)
		mcnt_grb(nIndex).Visible = bMode
	End Sub

	''' <summary>
	''' 名称セット
	''' </summary>
	''' <param name="nIndex">どっち(0オリジン)</param>
	''' <param name="sVal">アイテム</param>
	''' <remarks></remarks>
	Public Sub SetName(ByVal nIndex As Integer, ByVal sVal As List(Of String))
		mcnt_lbl(nIndex).Text = ""
		For Each s As String In sVal
			If 0 <> mcnt_lbl(nIndex).Text.Length Then mcnt_lbl(nIndex).Text &= vbCrLf
			mcnt_lbl(nIndex).Text &= s
		Next
	End Sub

	''' <summary>
	''' タイトルセット
	''' </summary>
	''' <param name="nIndex"></param>
	''' <param name="sVal"></param>
	''' <remarks></remarks>
	Public Sub SetTitle(ByVal nIndex As Integer, ByVal sVal As String)
		mcnt_grb(nIndex).Text = sVal
	End Sub
End Class
