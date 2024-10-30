<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSearch
    Inherits KizuForm.frmPop_Search2

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

	'NOTE: The following procedure is required by the Windows Form Designer
	'It can be modified using the Windows Form Designer.  
	'Do not modify it using the code editor.
	<System.Diagnostics.DebuggerStepThrough()> _
	Private Sub InitializeComponent()
        Me.grpSpecial = New System.Windows.Forms.GroupBox()
        Me.radNormalDsp = New System.Windows.Forms.RadioButton()
        Me.radDspLikeFFsnd = New System.Windows.Forms.RadioButton()
        Me.radDspLikeTansho = New System.Windows.Forms.RadioButton()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.chkNagate = New System.Windows.Forms.CheckBox()
        Me.chkHaba = New System.Windows.Forms.CheckBox()
        Me.chkKizuNon = New System.Windows.Forms.CheckBox()
        Me.chkCorrect = New System.Windows.Forms.CheckBox()
        Me.txtKanriNo = New System.Windows.Forms.TextBox()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.pnlMsg.SuspendLayout()
        CType(Me.dgvResult, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpSearch.SuspendLayout()
        Me.grpTime.SuspendLayout()
        Me.grpDate.SuspendLayout()
        Me.grpOrder.SuspendLayout()
        Me.grpDisp.SuspendLayout()
        Me.grpGrade.SuspendLayout()
        CType(Me.dgvDispGrade, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpType.SuspendLayout()
        CType(Me.dgvDispType, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpSpecial.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnEnd
        '
        Me.btnEnd.Text = "F12ÅFï¬Ç∂ÇÈ"
        '
        'btnCancel
        '
        '
        'btnSelect
        '
        Me.btnSelect.Text = "F6ÅFëIë"
        '
        'btnSearch
        '
        Me.btnSearch.Text = "F5ÅFåüçı"
        '
        'dgvResult
        '
        Me.dgvResult.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.dgvResult.RowTemplate.Height = 21
        '
        'btnCsv
        '
        '
        'grpSearch
        '
        Me.grpSearch.Controls.Add(Me.txtKanriNo)
        Me.grpSearch.Controls.Add(Me.Label8)
        Me.grpSearch.Size = New System.Drawing.Size(781, 168)
        Me.grpSearch.Controls.SetChildIndex(Me.Label1, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.Label2, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.Label5, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.Label6, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.grpOrder, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.txtCoil, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.txtKizukenNo, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.grpDate, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.grpTime, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.Label8, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.txtKanriNo, 0)
        '
        'txtKizukenNo
        '
        Me.txtKizukenNo.Location = New System.Drawing.Point(148, 44)
        Me.txtKizukenNo.MaxLength = 4
        Me.txtKizukenNo.Size = New System.Drawing.Size(76, 23)
        Me.txtKizukenNo.Text = "1234"
        '
        'Label2
        '
        Me.Label2.Size = New System.Drawing.Size(60, 16)
        Me.Label2.Text = "ñΩóﬂNo"
        '
        'Label1
        '
        Me.Label1.Size = New System.Drawing.Size(60, 16)
        Me.Label1.Text = "ê∂éYNo"
        '
        'grpDisp
        '
        Me.grpDisp.Controls.Add(Me.chkCorrect)
        Me.grpDisp.Controls.Add(Me.chkKizuNon)
        Me.grpDisp.Controls.Add(Me.grpSpecial)
        Me.grpDisp.Controls.Add(Me.GroupBox2)
        Me.grpDisp.Size = New System.Drawing.Size(928, 300)
        Me.grpDisp.Controls.SetChildIndex(Me.grpType, 0)
        Me.grpDisp.Controls.SetChildIndex(Me.grpGrade, 0)
        Me.grpDisp.Controls.SetChildIndex(Me.GroupBox2, 0)
        Me.grpDisp.Controls.SetChildIndex(Me.grpSpecial, 0)
        Me.grpDisp.Controls.SetChildIndex(Me.chkKizuNon, 0)
        Me.grpDisp.Controls.SetChildIndex(Me.chkCorrect, 0)
        '
        'dgvDispGrade
        '
        Me.dgvDispGrade.RowTemplate.Height = 21
        '
        'dgvDispType
        '
        Me.dgvDispType.RowTemplate.Height = 21
        '
        'Label6
        '
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold)
        Me.Label6.Location = New System.Drawing.Point(413, 48)
        Me.Label6.Size = New System.Drawing.Size(159, 13)
        '
        'Label5
        '
        Me.Label5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold)
        Me.Label5.Location = New System.Drawing.Point(413, 28)
        Me.Label5.Size = New System.Drawing.Size(159, 13)
        '
        'grpSpecial
        '
        Me.grpSpecial.Controls.Add(Me.radNormalDsp)
        Me.grpSpecial.Controls.Add(Me.radDspLikeFFsnd)
        Me.grpSpecial.Controls.Add(Me.radDspLikeTansho)
        Me.grpSpecial.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpSpecial.Location = New System.Drawing.Point(624, 11)
        Me.grpSpecial.Name = "grpSpecial"
        Me.grpSpecial.Size = New System.Drawing.Size(146, 109)
        Me.grpSpecial.TabIndex = 28
        Me.grpSpecial.TabStop = False
        Me.grpSpecial.Text = "ì¡éÍÉtÉBÉãÉ^Å[ëIë"
        '
        'radNormalDsp
        '
        Me.radNormalDsp.AutoSize = True
        Me.radNormalDsp.Checked = True
        Me.radNormalDsp.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.radNormalDsp.Location = New System.Drawing.Point(8, 20)
        Me.radNormalDsp.Name = "radNormalDsp"
        Me.radNormalDsp.Size = New System.Drawing.Size(94, 20)
        Me.radNormalDsp.TabIndex = 2
        Me.radNormalDsp.TabStop = True
        Me.radNormalDsp.Text = "ëIëñ≥Çµ"
        Me.radNormalDsp.UseVisualStyleBackColor = True
        '
        'radDspLikeFFsnd
        '
        Me.radDspLikeFFsnd.AutoSize = True
        Me.radDspLikeFFsnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.radDspLikeFFsnd.Location = New System.Drawing.Point(8, 84)
        Me.radDspLikeFFsnd.Name = "radDspLikeFFsnd"
        Me.radDspLikeFFsnd.Size = New System.Drawing.Size(129, 20)
        Me.radDspLikeFFsnd.TabIndex = 1
        Me.radDspLikeFFsnd.Text = "FFëóêM·rÇÃÇ›"
        Me.radDspLikeFFsnd.UseVisualStyleBackColor = True
        '
        'radDspLikeTansho
        '
        Me.radDspLikeTansho.AutoSize = True
        Me.radDspLikeTansho.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.radDspLikeTansho.Location = New System.Drawing.Point(8, 44)
        Me.radDspLikeTansho.Name = "radDspLikeTansho"
        Me.radDspLikeTansho.Size = New System.Drawing.Size(111, 36)
        Me.radDspLikeTansho.TabIndex = 0
        Me.radDspLikeTansho.Text = "íTèùâÊñ Ç∆" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ìØàÍ·rï\é¶" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        Me.radDspLikeTansho.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.chkNagate)
        Me.GroupBox2.Controls.Add(Me.chkHaba)
        Me.GroupBox2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(624, 122)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(144, 100)
        Me.GroupBox2.TabIndex = 26
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "ìWäJï˚ñ@ëIë"
        '
        'chkNagate
        '
        Me.chkNagate.Location = New System.Drawing.Point(6, 17)
        Me.chkNagate.Name = "chkNagate"
        Me.chkNagate.Size = New System.Drawing.Size(130, 44)
        Me.chkNagate.TabIndex = 2
        Me.chkNagate.Text = "í∑éËï˚å¸îΩì]" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "(TÅÀF)"
        Me.chkNagate.UseVisualStyleBackColor = True
        '
        'chkHaba
        '
        Me.chkHaba.Location = New System.Drawing.Point(6, 59)
        Me.chkHaba.Name = "chkHaba"
        Me.chkHaba.Size = New System.Drawing.Size(134, 38)
        Me.chkHaba.TabIndex = 3
        Me.chkHaba.Text = "ïùï˚å¸îΩì]" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "(WÅÀD)" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        Me.chkHaba.UseVisualStyleBackColor = True
        '
        'chkKizuNon
        '
        Me.chkKizuNon.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.chkKizuNon.Location = New System.Drawing.Point(623, 223)
        Me.chkKizuNon.Name = "chkKizuNon"
        Me.chkKizuNon.Size = New System.Drawing.Size(181, 36)
        Me.chkKizuNon.TabIndex = 2
        Me.chkKizuNon.Text = "·rå¬êî0å¬ÇÃ" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ÉRÉCÉãÇï\é¶ÇµÇ»Ç¢"
        Me.chkKizuNon.UseVisualStyleBackColor = True
        '
        'chkCorrect
        '
        Me.chkCorrect.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.chkCorrect.Location = New System.Drawing.Point(624, 260)
        Me.chkCorrect.Name = "chkCorrect"
        Me.chkCorrect.Size = New System.Drawing.Size(181, 36)
        Me.chkCorrect.TabIndex = 29
        Me.chkCorrect.Text = "ÉvÉçÉRÉìí∑Ç≥à íu" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ï‚ê≥ó¶égóp"
        Me.chkCorrect.UseVisualStyleBackColor = True
        '
        'txtKanriNo
        '
        Me.txtKanriNo.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKanriNo.Location = New System.Drawing.Point(228, 44)
        Me.txtKanriNo.MaxLength = 20
        Me.txtKanriNo.Name = "txtKanriNo"
        Me.txtKanriNo.Size = New System.Drawing.Size(172, 22)
        Me.txtKanriNo.TabIndex = 17
        Me.txtKanriNo.Text = "ABCD"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(236, 24)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(60, 16)
        Me.Label8.TabIndex = 19
        Me.Label8.Text = "ä«óùNo"
        '
        'frmSearch
        '
        Me.ClientSize = New System.Drawing.Size(947, 856)
        Me.Name = "frmSearch"
        Me.Text = "åüç∏é¿ê—åüçıâÊñ "
        Me.pnlMsg.ResumeLayout(False)
        Me.pnlMsg.PerformLayout()
        CType(Me.dgvResult, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpSearch.ResumeLayout(False)
        Me.grpSearch.PerformLayout()
        Me.grpTime.ResumeLayout(False)
        Me.grpTime.PerformLayout()
        Me.grpDate.ResumeLayout(False)
        Me.grpDate.PerformLayout()
        Me.grpOrder.ResumeLayout(False)
        Me.grpOrder.PerformLayout()
        Me.grpDisp.ResumeLayout(False)
        Me.grpGrade.ResumeLayout(False)
        CType(Me.dgvDispGrade, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpType.ResumeLayout(False)
        CType(Me.dgvDispType, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpSpecial.ResumeLayout(False)
        Me.grpSpecial.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents grpSpecial As System.Windows.Forms.GroupBox
    Friend WithEvents radNormalDsp As System.Windows.Forms.RadioButton
    Friend WithEvents radDspLikeFFsnd As System.Windows.Forms.RadioButton
    Friend WithEvents radDspLikeTansho As System.Windows.Forms.RadioButton
    Protected WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Protected WithEvents chkNagate As System.Windows.Forms.CheckBox
    Protected WithEvents chkHaba As System.Windows.Forms.CheckBox
    Protected WithEvents chkKizuNon As CheckBox
    Protected WithEvents chkCorrect As CheckBox
    Friend WithEvents Label8 As Label
    Friend WithEvents txtKanriNo As TextBox
End Class
