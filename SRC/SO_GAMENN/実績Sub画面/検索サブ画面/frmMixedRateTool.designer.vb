<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMixedRateTool
	Inherits tClassForm.frmBase_Popup

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
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.dgvMixedRate = New System.Windows.Forms.DataGridView()
        Me.lblProductNo = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.lblMeiNo = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.lblZisekiDiv = New System.Windows.Forms.Label()
        Me.lblColectDiv = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.btnEnd = New System.Windows.Forms.Button()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbID = New System.Windows.Forms.ComboBox()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.lblKizuKen = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.lblGrant = New System.Windows.Forms.Label()
        Me.lblKoban = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        CType(Me.dgvMixedRate, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.dgvMixedRate)
        Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(8, 264)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(736, 280)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "·rç¨ì¸ó¶Å^å¬êîàÍóó"
        '
        'dgvMixedRate
        '
        Me.dgvMixedRate.AllowUserToAddRows = False
        Me.dgvMixedRate.AllowUserToDeleteRows = False
        Me.dgvMixedRate.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvMixedRate.Location = New System.Drawing.Point(6, 20)
        Me.dgvMixedRate.Name = "dgvMixedRate"
        Me.dgvMixedRate.ReadOnly = True
        Me.dgvMixedRate.RowTemplate.Height = 21
        Me.dgvMixedRate.Size = New System.Drawing.Size(723, 253)
        Me.dgvMixedRate.TabIndex = 5
        '
        'lblProductNo
        '
        Me.lblProductNo.BackColor = System.Drawing.SystemColors.Control
        Me.lblProductNo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblProductNo.Font = New System.Drawing.Font("ÇlÇr ñæí©", 26.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblProductNo.ForeColor = System.Drawing.Color.Black
        Me.lblProductNo.Location = New System.Drawing.Point(120, 144)
        Me.lblProductNo.Name = "lblProductNo"
        Me.lblProductNo.Size = New System.Drawing.Size(248, 40)
        Me.lblProductNo.TabIndex = 7
        Me.lblProductNo.Text = "123456789012"
        Me.lblProductNo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label9
        '
        Me.Label9.BackColor = System.Drawing.SystemColors.Control
        Me.Label9.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.ForeColor = System.Drawing.Color.Black
        Me.Label9.Location = New System.Drawing.Point(124, 116)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(104, 28)
        Me.Label9.TabIndex = 8
        Me.Label9.Text = "ê∂éYî‘çÜ"
        Me.Label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblMeiNo
        '
        Me.lblMeiNo.BackColor = System.Drawing.SystemColors.Control
        Me.lblMeiNo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMeiNo.Font = New System.Drawing.Font("ÇlÇr ñæí©", 26.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMeiNo.ForeColor = System.Drawing.Color.Black
        Me.lblMeiNo.Location = New System.Drawing.Point(20, 144)
        Me.lblMeiNo.Name = "lblMeiNo"
        Me.lblMeiNo.Size = New System.Drawing.Size(96, 40)
        Me.lblMeiNo.TabIndex = 5
        Me.lblMeiNo.Text = "1234"
        Me.lblMeiNo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label7
        '
        Me.Label7.BackColor = System.Drawing.SystemColors.Control
        Me.Label7.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.ForeColor = System.Drawing.Color.Black
        Me.Label7.Location = New System.Drawing.Point(24, 116)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(88, 28)
        Me.Label7.TabIndex = 6
        Me.Label7.Text = "ñΩóﬂNo"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblZisekiDiv
        '
        Me.lblZisekiDiv.BackColor = System.Drawing.Color.Blue
        Me.lblZisekiDiv.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblZisekiDiv.Font = New System.Drawing.Font("ÇlÇr ñæí©", 21.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblZisekiDiv.ForeColor = System.Drawing.Color.Black
        Me.lblZisekiDiv.Location = New System.Drawing.Point(340, 220)
        Me.lblZisekiDiv.Name = "lblZisekiDiv"
        Me.lblZisekiDiv.Size = New System.Drawing.Size(180, 40)
        Me.lblZisekiDiv.TabIndex = 9
        Me.lblZisekiDiv.Text = "ê≥èÌ"
        Me.lblZisekiDiv.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblColectDiv
        '
        Me.lblColectDiv.BackColor = System.Drawing.Color.Red
        Me.lblColectDiv.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblColectDiv.Font = New System.Drawing.Font("ÇlÇr ñæí©", 21.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblColectDiv.ForeColor = System.Drawing.Color.Black
        Me.lblColectDiv.Location = New System.Drawing.Point(532, 220)
        Me.lblColectDiv.Name = "lblColectDiv"
        Me.lblColectDiv.Size = New System.Drawing.Size(196, 40)
        Me.lblColectDiv.TabIndex = 10
        Me.lblColectDiv.Text = "àŸèÌ"
        Me.lblColectDiv.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label3
        '
        Me.Label3.BackColor = System.Drawing.SystemColors.Control
        Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.ForeColor = System.Drawing.Color.Black
        Me.Label3.Location = New System.Drawing.Point(340, 192)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(104, 28)
        Me.Label3.TabIndex = 11
        Me.Label3.Text = "é¿ê—ãÊï™"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label4
        '
        Me.Label4.BackColor = System.Drawing.SystemColors.Control
        Me.Label4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.Color.Black
        Me.Label4.Location = New System.Drawing.Point(532, 192)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(104, 28)
        Me.Label4.TabIndex = 12
        Me.Label4.Text = "é˚èWãÊï™"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnEnd
        '
        Me.btnEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnEnd.Location = New System.Drawing.Point(620, 12)
        Me.btnEnd.Name = "btnEnd"
        Me.btnEnd.Size = New System.Drawing.Size(104, 39)
        Me.btnEnd.TabIndex = 17
        Me.btnEnd.Text = "F12:ï¬Ç∂ÇÈ"
        Me.btnEnd.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(20, 20)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(34, 21)
        Me.Label1.TabIndex = 18
        Me.Label1.Text = "ID"
        '
        'cmbID
        '
        Me.cmbID.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbID.FormattingEnabled = True
        Me.cmbID.Location = New System.Drawing.Point(68, 20)
        Me.cmbID.Name = "cmbID"
        Me.cmbID.Size = New System.Drawing.Size(204, 24)
        Me.cmbID.TabIndex = 19
        '
        'btnRead
        '
        Me.btnRead.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRead.Location = New System.Drawing.Point(292, 12)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(104, 39)
        Me.btnRead.TabIndex = 20
        Me.btnRead.Text = "ì«Ç›çûÇ›"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.SystemColors.Control
        Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.Color.Black
        Me.Label2.Location = New System.Drawing.Point(20, 60)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(132, 28)
        Me.Label2.TabIndex = 22
        Me.Label2.Text = "ä«óùNo"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblKizuKen
        '
        Me.lblKizuKen.BackColor = System.Drawing.SystemColors.Control
        Me.lblKizuKen.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKizuKen.Font = New System.Drawing.Font("ÇlÇr ñæí©", 26.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKizuKen.ForeColor = System.Drawing.Color.Black
        Me.lblKizuKen.Location = New System.Drawing.Point(119, 56)
        Me.lblKizuKen.Name = "lblKizuKen"
        Me.lblKizuKen.Size = New System.Drawing.Size(400, 40)
        Me.lblKizuKen.TabIndex = 23
        Me.lblKizuKen.Text = "12345678901234567890"
        Me.lblKizuKen.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label5
        '
        Me.Label5.BackColor = System.Drawing.SystemColors.Control
        Me.Label5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.ForeColor = System.Drawing.Color.Black
        Me.Label5.Location = New System.Drawing.Point(20, 224)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(188, 28)
        Me.Label5.TabIndex = 24
        Me.Label5.Text = "ÉOÉâÉìÉgÉtÉâÉO"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblGrant
        '
        Me.lblGrant.BackColor = System.Drawing.Color.Blue
        Me.lblGrant.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblGrant.Font = New System.Drawing.Font("ÇlÇr ñæí©", 26.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblGrant.ForeColor = System.Drawing.Color.Black
        Me.lblGrant.Location = New System.Drawing.Point(220, 220)
        Me.lblGrant.Name = "lblGrant"
        Me.lblGrant.Size = New System.Drawing.Size(96, 40)
        Me.lblGrant.TabIndex = 25
        Me.lblGrant.Text = "ê≥èÌ"
        Me.lblGrant.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblKoban
        '
        Me.lblKoban.BackColor = System.Drawing.SystemColors.Control
        Me.lblKoban.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKoban.Font = New System.Drawing.Font("ÇlÇr ñæí©", 26.25!, System.Drawing.FontStyle.Bold)
        Me.lblKoban.ForeColor = System.Drawing.Color.Black
        Me.lblKoban.Location = New System.Drawing.Point(376, 144)
        Me.lblKoban.Name = "lblKoban"
        Me.lblKoban.Size = New System.Drawing.Size(224, 40)
        Me.lblKoban.TabIndex = 72
        Me.lblKoban.Text = "123456789012"
        Me.lblKoban.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label6
        '
        Me.Label6.BackColor = System.Drawing.SystemColors.Control
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.ForeColor = System.Drawing.Color.Black
        Me.Label6.Location = New System.Drawing.Point(376, 108)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(104, 28)
        Me.Label6.TabIndex = 73
        Me.Label6.Text = "éqî‘"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'frmMixedRateTool
        '
        Me.ClientSize = New System.Drawing.Size(743, 561)
        Me.ControlBox = True
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.lblKoban)
        Me.Controls.Add(Me.lblGrant)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.lblKizuKen)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.cmbID)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.btnEnd)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.lblColectDiv)
        Me.Controls.Add(Me.lblZisekiDiv)
        Me.Controls.Add(Me.lblProductNo)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.lblMeiNo)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.GroupBox1)
        Me.MaximizeBox = True
        Me.MinimizeBox = True
        Me.Name = "frmMixedRateTool"
        Me.Text = "åüç∏é¿ê—ÉvÉçÉRÉìâÊñ "
        Me.TopMost = False
        Me.GroupBox1.ResumeLayout(False)
        CType(Me.dgvMixedRate, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvMixedRate As System.Windows.Forms.DataGridView
    Friend WithEvents lblProductNo As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents lblMeiNo As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents lblZisekiDiv As System.Windows.Forms.Label
    Friend WithEvents lblColectDiv As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents btnEnd As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents cmbID As System.Windows.Forms.ComboBox
    Friend WithEvents btnRead As System.Windows.Forms.Button
	Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents lblKizuKen As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents lblGrant As System.Windows.Forms.Label
    Friend WithEvents lblKoban As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label

End Class
