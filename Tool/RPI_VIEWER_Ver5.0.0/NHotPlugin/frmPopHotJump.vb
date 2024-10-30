Imports System.Windows.Forms
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Public Class frmPopHotJump
    Implements RpiPlugin.IPluginActivatable, RpiPlugin.IToolStrip, RpiPlugin.IRpiOpen

    Private mcls_RpiView As tClass_ViewRpiThread

    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 200
        End Get
    End Property

    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "frmPopHotJump"
        End Get
    End Property

    Dim m_parent As Form
    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v
        m_parent = mainfrm
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


        nudFontSize.Value = HotRollInfo.DrawProp.fontsize
        nudLineWidth.Value = HotRollInfo.DrawProp.linewidth
        cmbDashStyle.SelectedIndex = HotRollInfo.DrawProp.linestyle
        m_stopemit = False
    End Sub


    Private Sub OnHandleDspClick(sender As Object, e As EventArgs)
        '
        If Me.Visible Then
            Me.Visible = False
            Return
        End If

        Me.Show(m_parent)
    End Sub


    Sub ErrorMode(bError As Boolean)
        TextBox1.Visible = bError
        dgvCoil.Visible = Not bError
        dgvPitch.Visible = Not bError
    End Sub

    Dim m_rolls As HotRollInfo

    Public Sub Init()
        m_rolls  = nothing
        dgvPitch.EndEdit()
        m_rolls = HotRollInfo.GlobalRollInfo
        If m_rolls Is Nothing Then Exit Sub
        If Not String.IsNullOrEmpty(m_rolls.m_errormsg) Then
            TextBox1.Text = m_rolls.m_errormsg
            ErrorMode(True)
            Return
        End If

        dgvPitch.Rows.Clear()
        For Each pitch As RollPitch In m_rolls
            dgvPitch.Rows.Add(pitch.Name, pitch.mmPitch)
        Next

        dgvCoil.Rows.Clear()
        dgvCoil.Rows.Add("コイルNo", m_rolls.コイルNO)
        dgvCoil.Rows.Add("管理No", m_rolls.管理NO)
        For ii As Integer = 1 To 7
            dgvCoil.Rows.Add("ロール径 F" & ii, m_rolls.mmRollDiam(ii))
        Next

        For ii As Integer = 1 To 7
            dgvCoil.Rows.Add("F" & ii & "出側厚", m_rolls.mmAtu(ii))
        Next

        For ii As Integer = 1 To 7
            dgvCoil.Rows.Add("コイル進行量 F" & ii, m_rolls.mmCoilSpeed(ii))
        Next

        For ii As Integer = 1 To 7
            dgvCoil.Rows.Add("先進率 F" & ii, m_rolls.Adv(ii))
        Next
        ErrorMode(False)
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

        If m_rolls Is Nothing Then Return 0

        For Each pitch As RollPitch In m_rolls
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
        checkboxes.AddRange(New CheckBox() {CheckBox9, CheckBox8, CheckBox7, CheckBox6, CheckBox5, CheckBox4, CheckBox3, CheckBox2, CheckBox14, CheckBox13, CheckBox12, CheckBox11, CheckBox10, CheckBox1})

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

    Private Sub dgvPitch_CellValidated(sender As Object, e As DataGridViewCellEventArgs) Handles dgvPitch.CellValidated
        If m_rolls Is Nothing Then Exit Sub
        If e.ColumnIndex = 1 Then
            Dim sVal As String = dgvPitch.Item(e.ColumnIndex, e.RowIndex).Value.ToString
            Dim nVal As Integer = 0
            If Integer.TryParse(sVal, nVal) Then

                If m_rolls(e.RowIndex).mmPitch <> nVal Then
                    m_rolls(e.RowIndex).mmPitch = nVal

                    mcls_RpiView.RePaint()
                End If
            End If
        End If
    End Sub


    Private m_stopemit As Boolean = True
    Private Sub nudFontSize_ValueChanged(sender As Object, e As EventArgs) Handles nudFontSize.ValueChanged
        If m_stopemit Then Exit Sub
        HotRollInfo.DrawProp.fontsize = CInt(nudFontSize.Value)
        HotRollInfo.DrawProp.Emit()
    End Sub

    Private Sub nudLineWidth_ValueChanged(sender As Object, e As EventArgs) Handles nudLineWidth.ValueChanged
        If m_stopemit Then Exit Sub
        HotRollInfo.DrawProp.linewidth = CInt(nudLineWidth.Value)
        HotRollInfo.DrawProp.Emit()

    End Sub

    Private Sub cmbDashStyle_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbDashStyle.SelectedIndexChanged
        If m_stopemit Then Exit Sub
        HotRollInfo.DrawProp.linestyle = CType(cmbDashStyle.SelectedIndex, Drawing.Drawing2D.DashStyle)
        HotRollInfo.DrawProp.Emit()

    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        HotRollInfo.DrawProp.Save()

    End Sub
End Class