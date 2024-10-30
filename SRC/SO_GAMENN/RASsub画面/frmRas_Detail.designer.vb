<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmStart
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
        Me.components = New System.ComponentModel.Container()
        Me.cmdEnd = New System.Windows.Forms.Button()
        Me.dgvKiki = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.tmrDsp = New System.Windows.Forms.Timer(Me.components)
        Me.radAll = New System.Windows.Forms.RadioButton()
        Me.radDefult = New System.Windows.Forms.RadioButton()
        Me.radErr = New System.Windows.Forms.RadioButton()
        CType(Me.dgvKiki, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'cmdEnd
        '
        Me.cmdEnd.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cmdEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmdEnd.Location = New System.Drawing.Point(789, 898)
        Me.cmdEnd.Name = "cmdEnd"
        Me.cmdEnd.Size = New System.Drawing.Size(116, 32)
        Me.cmdEnd.TabIndex = 9
        Me.cmdEnd.Text = "F12:ï¬Ç∂ÇÈ"
        Me.cmdEnd.UseVisualStyleBackColor = True
        '
        'dgvKiki
        '
        Me.dgvKiki.AllowUserToAddRows = False
        Me.dgvKiki.AllowUserToDeleteRows = False
        Me.dgvKiki.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvKiki.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvKiki.Location = New System.Drawing.Point(5, 6)
        Me.dgvKiki.Name = "dgvKiki"
        Me.dgvKiki.ReadOnly = True
        Me.dgvKiki.RowTemplate.Height = 21
        Me.dgvKiki.Size = New System.Drawing.Size(924, 882)
        Me.dgvKiki.TabIndex = 8
        '
        'tmrDsp
        '
        Me.tmrDsp.Interval = 10000
        '
        'radAll
        '
        Me.radAll.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.radAll.Appearance = System.Windows.Forms.Appearance.Button
        Me.radAll.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.radAll.Location = New System.Drawing.Point(284, 900)
        Me.radAll.Name = "radAll"
        Me.radAll.Size = New System.Drawing.Size(120, 25)
        Me.radAll.TabIndex = 12
        Me.radAll.TabStop = True
        Me.radAll.Text = "ëSÇƒ"
        Me.radAll.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radAll.UseVisualStyleBackColor = False
        '
        'radDefult
        '
        Me.radDefult.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.radDefult.Appearance = System.Windows.Forms.Appearance.Button
        Me.radDefult.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.radDefult.Location = New System.Drawing.Point(144, 900)
        Me.radDefult.Name = "radDefult"
        Me.radDefult.Size = New System.Drawing.Size(120, 25)
        Me.radDefult.TabIndex = 11
        Me.radDefult.TabStop = True
        Me.radDefult.Text = "ê≥èÌÅ{àŸèÌ"
        Me.radDefult.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radDefult.UseVisualStyleBackColor = False
        '
        'radErr
        '
        Me.radErr.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.radErr.Appearance = System.Windows.Forms.Appearance.Button
        Me.radErr.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.radErr.Location = New System.Drawing.Point(4, 900)
        Me.radErr.Name = "radErr"
        Me.radErr.Size = New System.Drawing.Size(120, 25)
        Me.radErr.TabIndex = 10
        Me.radErr.TabStop = True
        Me.radErr.Text = "àŸèÌèÓïÒÇÃÇ›"
        Me.radErr.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radErr.UseVisualStyleBackColor = False
        '
        'frmStart
        '
        Me.ClientSize = New System.Drawing.Size(935, 936)
        Me.Controls.Add(Me.radAll)
        Me.Controls.Add(Me.radDefult)
        Me.Controls.Add(Me.radErr)
        Me.Controls.Add(Me.cmdEnd)
        Me.Controls.Add(Me.dgvKiki)
        Me.Name = "frmStart"
        Me.Text = "àŸèÌèÛë‘ï\é¶âÊñ "
        Me.TopMost = False
        CType(Me.dgvKiki, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents cmdEnd As System.Windows.Forms.Button
    Friend WithEvents dgvKiki As tClassLibrary.tCnt.tCnt_DataGridView
    Protected Friend WithEvents tmrDsp As System.Windows.Forms.Timer
    Friend WithEvents radAll As System.Windows.Forms.RadioButton
    Friend WithEvents radDefult As System.Windows.Forms.RadioButton
    Friend WithEvents radErr As System.Windows.Forms.RadioButton

End Class
