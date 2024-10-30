Imports System.Windows.Forms
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Public Class frmPopHotRoll
    Implements RpiPlugin.IPluginActivatable, RpiPlugin.IToolStrip, RpiPlugin.IRpiOpen, RpiPlugin.IKeyInput

    Private mbGlobalFunctionKey As Boolean
    Private mcls_RpiView As tClass_ViewRpiThread

    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 180
        End Get
    End Property

    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "frmPopHotRoll"
        End Get
    End Property

    Private m_sedoc As PeropertiesBase
    Private m_imdoc As PeropertiesBase
    Const SECTION As String = "DrawRollPitch"
    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v
        m_sedoc = RpiPlugin.PeropertiesBase.GetPropertyMember(Of PeropertiesBase)(mainfrm, "displayshrinker")
        m_imdoc = RpiPlugin.PeropertiesBase.GetPropertyMember(Of PeropertiesBase)(mainfrm, "imagecontrol")
        AddHandler m_sedoc.OnValueChanged, AddressOf OnChangeShrink
        AddHandler m_imdoc.OnValueChanged, AddressOf OnChangeShrink

        mbGlobalFunctionKey = tClassLibrary.tMod.ReadIniInt(SECTION, "GlobalFunctionKey", HotRollInfo.TASKINI_HOTPL, 1) = 1
    End Sub

    Private m_bStopEmit As Boolean = False

    Const displayfmt As String = "0.000000"

    Private Sub OnChangeShrink(name As String, doc As PeropertiesBase)
        m_bStopEmit = True
        If name = "DisplayShrinker" Then
            lblNowResY.Text = CDbl(m_sedoc.GetValue("nResY")).ToString(displayfmt)
            lblBaseResY.Text = CDbl(m_sedoc.GetValue("nBaseResY")).ToString(displayfmt)
        End If
        If name = "PropImageControl" Then
            chkContrast.Checked = CBool(m_imdoc.GetValue("bContMode"))
            lblContrast.Text = m_imdoc.GetValue("nContrastX").ToString()
            lblBright.Text = m_imdoc.GetValue("nBright").ToString()
        End If
        m_bStopEmit = False
    End Sub




    Dim m_original_res_y As Single
    Public Sub OnRpiOpen(rpi As tClass_ReadRPI) Implements IRpiOpen.OnRpiOpen
        m_original_res_y = rpi.GetRpiHeader.res_y
    End Sub
    Public Sub OnRpiClose() Implements IRpiOpen.OnRpiClose
    End Sub

    Private m_tsbDrawHotRoll As New System.Windows.Forms.ToolStripButton()


    Function GetItemIndex(strip As ToolStrip, name As String) As Integer
        Dim items() As ToolStripItem = strip.Items.Find(name, False)
        If items.Length = 0 Then
            Return strip.Items.Count
        End If
        Return strip.Items.IndexOf(items(0))
    End Function

    Public Sub InitToolbar(mainstrip() As ToolStrip) Implements IToolStrip.InitToolbar
        'm_tsbDrawHotRoll.CheckOnClick = True
        m_tsbDrawHotRoll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        m_tsbDrawHotRoll.Image = LabelIcon.Image
        m_tsbDrawHotRoll.ImageTransparentColor = System.Drawing.Color.Magenta
        m_tsbDrawHotRoll.Name = "tsbDrawHotRoll"
        m_tsbDrawHotRoll.Size = New System.Drawing.Size(23, 23)
        m_tsbDrawHotRoll.Text = "熱延ロール長表示"
        AddHandler m_tsbDrawHotRoll.Click, AddressOf OnHandleDspClick

        Dim ipos As Integer = GetItemIndex(mainstrip(0), "tsbJump")
        mainstrip(0).Items.Insert(ipos, m_tsbDrawHotRoll)
    End Sub
    Private Sub OnHandleDspClick(sender As Object, e As EventArgs)
        '
        If Me.Visible Then
            Me.Visible = False
            Return
        End If

        Dim f As Form = Form.ActiveForm
        Me.Show(f)
    End Sub


    Public Sub Init()
        Dim rolls As HotRollInfo = HotRollInfo.GlobalRollInfo
        If rolls Is Nothing Then Exit Sub
        If Not String.IsNullOrEmpty(rolls.m_errormsg) Then
            Return
        End If
    End Sub

    Dim checkboxes As New List(Of CheckBox)

    Private Function GetSelectedName() As String
        For Each ckbox As CheckBox In checkboxes
            If ckbox.Checked Then
                Return ckbox.Text
            End If
        Next
        Return String.Empty
    End Function

    Private Function GetSelectedLengthMM() As Integer
        Dim title As String = GetSelectedName()

        Dim rolls As HotRollInfo = HotRollInfo.GlobalRollInfo
        If rolls Is Nothing Then Return 0

        For Each pitch As RollPitch In rolls
            If pitch.Name = title Then
                Return pitch.mmPitch
            End If

        Next
        Return 0
    End Function


    Private Sub CheckBox_Click(sender As Object, e As EventArgs)
        For Each ckbox As CheckBox In checkboxes
            If ckbox Is sender Then
            Else
                ckbox.Checked = False
            End If
        Next
    End Sub

    Private Sub frmPopHotJump_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        checkboxes.AddRange(New CheckBox() {CheckBox1, CheckBox2, CheckBox3, CheckBox4, CheckBox5, CheckBox6, CheckBox7})

        For Each ckbox As CheckBox In checkboxes
            AddHandler ckbox.Click, AddressOf CheckBox_Click
        Next

        AddHandler HotRollInfo.OnUpdateGlobalRollInfo, AddressOf OnUpdateGlobalRollInfo
        Init()
    End Sub

    Private Sub OnUpdateGlobalRollInfo(sender As Object, e As EventArgs)
        Init()
    End Sub

    Private Sub fFormClosing(sender As Object, e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        If e.CloseReason = CloseReason.UserClosing Then
            e.Cancel = True
            Me.Visible = False
        End If
    End Sub


    Sub ScrollMM(deltaMM As Integer)
        Dim rate_dh As Double = (m_original_res_y / mcls_RpiView.GetImg_ImgInf.dResH)
        Dim dp As Integer = mcls_RpiView.GetPos_LenToDp_Y(deltaMM / rate_dh)

        Dim nVal As Integer = dp + mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y

        Call mcls_RpiView.ScrollH(nVal, True)     ' スクロールかプログラムかは、引数により変更
        Call mcls_RpiView.RePaint()
    End Sub

    Private Sub btnForward_Click(sender As Object, e As EventArgs) Handles btnForward.Click
        ScrollMM(GetSelectedLengthMM)
    End Sub

    Private Sub btnBackward_Click(sender As Object, e As EventArgs) Handles btnBackward.Click
        ScrollMM(-GetSelectedLengthMM())
    End Sub

    ''' <summary>
    ''' コントラスト調整スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbContrast_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbContrast.ValueChanged
        '設定値格納
        If m_imdoc IsNot Nothing Then
            If chkContrast.Checked Then
                m_imdoc.SetValue("nContrastX", trbContrast.Value)
                m_imdoc.Emit()
            End If
        End If
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ Y
    ''' <summary>
    ''' 縦分解能変更(倍率指定)
    ''' </summary>
    Private Sub btnChageY_Dev_Click(sender As System.Object, e As System.EventArgs) Handles btnChageY_Dev.Click
        m_sedoc.SetValue("nResY", CDbl(m_sedoc.GetValue("nBaseResY")) * nudResY_Dev.Value)
        m_sedoc.Emit()
    End Sub

    ''' <summary>
    ''' 縦分解能変更(デフォルトに)
    ''' </summary>
    Private Sub btnChageY_Init_Click(sender As System.Object, e As System.EventArgs) Handles btnChageY_Init.Click
        m_sedoc.SetValue("nResY", m_sedoc.GetValue("nBaseResY"))
        m_sedoc.Emit()
    End Sub



    ''' <summary>
    ''' 明るさ調整スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbBright_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbBright.ValueChanged
        '設定値格納
        If m_imdoc IsNot Nothing And Not m_bStopEmit Then
            If chkContrast.Checked Then
                m_imdoc.SetValue("nBright", trbBright.Value)
                m_imdoc.Emit()
            End If
        End If
    End Sub

    ''' <summary>
    ''' コントラスト有効化
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub chkContrast_CheckedChanged(sender As Object, e As EventArgs) Handles chkContrast.CheckedChanged
        If m_imdoc IsNot Nothing And Not m_bStopEmit Then
            m_imdoc.SetValue("nContrastX", trbContrast.Value)
            m_imdoc.SetValue("nBright", trbBright.Value)
            m_imdoc.SetValue("m_Filter", IIf(chkContrast.Checked, 1, 0))
            m_imdoc.Emit()
        End If
    End Sub


    Private Function IKeyInput_OnKeyDown(e As KeyEventArgs) As Boolean Implements IKeyInput.OnKeyDown
        If Not Me.Visible And Not mbGlobalFunctionKey Then Return False
        If HotRollInfo.GlobalRollInfo Is Nothing Then Return False
        Dim name As String = ""
        If Keys.F1 <= e.KeyCode And e.KeyCode <= Keys.F7 Then
            name = "F" + (e.KeyCode - Keys.F1 + 1).ToString
        Else
            Return False
        End If

        If mbGlobalFunctionKey And Keys.F5 = e.KeyCode Then
            Return False
        End If

        Dim pitch As Integer = 0
        Dim rolls As HotRollInfo = HotRollInfo.GlobalRollInfo
        For Each rp As RollPitch In rolls
            If rp.Name = name Then
                pitch = rp.mmPitch
            End If
        Next

        If e.Modifiers = Keys.Shift Then
            pitch = -pitch
        End If
        ScrollMM(pitch)
        Return True
    End Function
End Class
