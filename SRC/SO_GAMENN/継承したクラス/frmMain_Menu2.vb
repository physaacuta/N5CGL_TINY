'*********************************************************************************
' メニュー画面の親フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' メニュー画面の親フォーム
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Menu2

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_clMenuSelectColor As Color = Color.FromArgb(255, 64, 128)	' メニュー選択時の色
	Private m_clMenuEnableColor As Color = Color.FromArgb(0, 0, 192)		' メニュー未選択時の色
	Private m_clMenuDesableColor As Color = Color.DimGray				' メニュー無効時の色

	Private m_cMenuText(11) As String									' メニューのラベル文字
	Private m_bMenuEnable(11) As Boolean									' メニューの有効無効
	Protected Friend my_lblMenuF(11) As Label							' メニュー文字コントロール
	Protected Friend my_lblMenuF_Line(11) As Label						' メニュー下線コントロール


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' メニュー選択時の色
	''' </summary>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆メニュー画面専用プロパティ") _
	 , System.ComponentModel.Description("メニュー選択時の色")> _
	Public Property tProp_MenuSelectColor() As Color
		Get
			Return m_clMenuSelectColor
		End Get
		Set(ByVal Value As Color)
			m_clMenuSelectColor = Value
			Call setMenuText_Format()
		End Set
	End Property
	''' <summary>
	''' メニュー未選択時の色
	''' </summary>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆メニュー画面専用プロパティ") _
	 , System.ComponentModel.Description("メニュー未選択時の色")> _
	Public Property tProp_MenuEnableColor() As Color
		Get
			Return m_clMenuEnableColor
		End Get
		Set(ByVal Value As Color)
			m_clMenuEnableColor = Value
			Call setMenuText_Format()
		End Set
	End Property
	''' <summary>
	''' メニュー無効時の色
	''' </summary>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("☆メニュー画面専用プロパティ") _
	 , System.ComponentModel.Description("メニュー無効時の色")> _
	Public Property tProp_MenuDesableColor() As Color
		Get
			Return m_clMenuDesableColor
		End Get
		Set(ByVal Value As Color)
			m_clMenuDesableColor = Value
			Call setMenuText_Format()
		End Set
	End Property

	''' <summary>
	''' メニューのラベル文字
	''' </summary>
	''' <value></value>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Content) _
	 , System.ComponentModel.Category("☆メニュー画面専用プロパティ") _
	 , System.ComponentModel.Description("メニューのラベル文字 (直接labelコントロールのtextの禁止 こっちを使う事)")> _
	Public Property tProp_MenuText() As String()
		Get
			Return m_cMenuText
		End Get
		Set(ByVal Value As String())
			If Value.Length <> m_cMenuText.Length Then Exit Property

			m_cMenuText = Value
			Call setMenuText_Format()
		End Set
	End Property
	Public Property tProp_MenuText(ByVal index As Integer) As String
		Get
			Return m_cMenuText(index)
		End Get
		Set(ByVal Value As String)
			m_cMenuText(index) = Value
			Call setMenuText_Format()
		End Set
	End Property
	
	''' <summary>
	''' メニューの状態
	''' </summary>
	''' <value></value>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Content) _
	 , System.ComponentModel.Category("☆メニュー画面専用プロパティ") _
	 , System.ComponentModel.Description("メニューの状態 (直接labelコントロールのtextの禁止 こっちを使う事)")> _
	Public Property tProp_MenuEnable() As Boolean()
		Get
			Return m_bMenuEnable
		End Get
		Set(ByVal Value As Boolean())
			If Value.Length <> m_cMenuText.Length Then Exit Property

			m_bMenuEnable = Value
			Call setMenuText_Format()
		End Set
	End Property
	Public Property tProp_MenuEnable(ByVal index As Integer) As Boolean
		Get
			Return m_bMenuEnable(index)
		End Get
		Set(ByVal Value As Boolean)

			m_bMenuEnable(index) = Value
			Call setMenuText_Format()
		End Set
	End Property



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <remarks></remarks>
	Public Sub New()

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。
		'''' メニューのコントロール配列を生成する
		my_lblMenuF(0) = lblMenuF01
		my_lblMenuF(1) = lblMenuF02
		my_lblMenuF(2) = lblMenuF03
		my_lblMenuF(3) = lblMenuF04
		my_lblMenuF(4) = lblMenuF05
		my_lblMenuF(5) = lblMenuF06
		my_lblMenuF(6) = lblMenuF07
		my_lblMenuF(7) = lblMenuF08
		my_lblMenuF(8) = lblMenuF09
		my_lblMenuF(9) = lblMenuF10
		my_lblMenuF(10) = lblMenuF11
		my_lblMenuF(11) = lblMenuF12

		my_lblMenuF_Line(0) = lblMenuF01_l
		my_lblMenuF_Line(1) = lblMenuF02_l
		my_lblMenuF_Line(2) = lblMenuF03_l
		my_lblMenuF_Line(3) = lblMenuF04_l
		my_lblMenuF_Line(4) = lblMenuF05_l
		my_lblMenuF_Line(5) = lblMenuF06_l
		my_lblMenuF_Line(6) = lblMenuF07_l
		my_lblMenuF_Line(7) = lblMenuF08_l
		my_lblMenuF_Line(8) = lblMenuF09_l
		my_lblMenuF_Line(9) = lblMenuF10_l
		my_lblMenuF_Line(10) = lblMenuF11_l
		my_lblMenuF_Line(11) = lblMenuF12_l
		'' メニュー番号セット
		For ii As Integer = 0 To my_lblMenuF.Length - 1
			my_lblMenuF(ii).Tag = ii
			my_lblMenuF_Line(ii).Tag = ii

			m_cMenuText(ii) = ""
			m_bMenuEnable(ii) = False

			' イベントコードで追加 量が多いためめんどくさいから
''			AddHandler m_lblMenuF(ii).MouseHover, AddressOf lblMenuF_MouseHover		' メニューラベルにマウスが乗った (イベントが来るのみ時間が掛かる為ヤメ)
			AddHandler my_lblMenuF(ii).MouseMove, AddressOf lblMenuF_MouseMove	   ' メニューラベルにマウスが乗った
			AddHandler my_lblMenuF(ii).MouseLeave, AddressOf lblMenuF_MouseLeave		' メニューラベルにマウスが離れた
			AddHandler my_lblMenuF(ii).Click, AddressOf lblMenuF_Click				' メニューラベルをクリック

			' マウスポインタセット
			my_lblMenuF(ii).Cursor = Cursors.Hand

		Next
	End Sub

	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmMain_Menu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'' ラベルセット
		Call setMenuText_Format()
	End Sub

	''' <summary>
	''' メニューラベルにマウスが動いた
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lblMenuF_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
		Dim FncNo As Integer = CInt(CType(sender, Label).Tag)		' No取り出し
		If Not m_bMenuEnable(FncNo) Then Exit Sub
		If my_lblMenuF(FncNo).ForeColor = m_clMenuSelectColor Then Exit Sub
		'色替え
		my_lblMenuF(FncNo).ForeColor = m_clMenuSelectColor
		my_lblMenuF_Line(FncNo).BackColor = m_clMenuSelectColor
		'フォント換え (標準に)
		my_lblMenuF(FncNo).Font = New Font(my_lblMenuF(FncNo).Font, FontStyle.Italic)
	End Sub

	''' <summary>
	''' メニューラベルにマウスが離れた
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lblMenuF_MouseLeave(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim FncNo As Integer = CInt(CType(sender, Label).Tag)		' No取り出し
		If Not m_bMenuEnable(FncNo) Then Exit Sub
		'色替え
		my_lblMenuF(FncNo).ForeColor = m_clMenuEnableColor
		my_lblMenuF_Line(FncNo).BackColor = m_clMenuEnableColor
		'フォント換え (斜めに)
		my_lblMenuF(FncNo).Font = New Font(my_lblMenuF(FncNo).Font, FontStyle.Bold)
	End Sub
	''' <summary>
	''' メニューラベルをクリック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lblMenuF_Click(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim FncNo As Integer = CInt(CType(sender, Label).Tag)		' No取り出し
		If Not m_bMenuEnable(FncNo) Then Exit Sub
		'' ファンクションボタンを押されたと同じ動作をさせる
		Call MyBase.SetEvFunctionButton(FncNo, my_btnFunction(FncNo).Enabled, Control.ModifierKeys())
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部アクセス メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' メニューテキストをセット
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetMenuText_Format()
		For ii As Integer = 0 To my_lblMenuF.Length - 1

			my_lblMenuF(ii).Text = String.Format(" F{0,-6}{1}", ii + 1, m_cMenuText(ii))

			If m_bMenuEnable(ii) Then
				my_lblMenuF(ii).ForeColor = m_clMenuEnableColor
				my_lblMenuF_Line(ii).BackColor = m_clMenuEnableColor
			Else
				my_lblMenuF(ii).ForeColor = m_clMenuDesableColor
				my_lblMenuF_Line(ii).BackColor = m_clMenuDesableColor
			End If
		Next
	End Sub


End Class
