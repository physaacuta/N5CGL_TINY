<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmDspCoil
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
        Me.BtnClose = New System.Windows.Forms.Button()
        Me.BtnCSV = New System.Windows.Forms.Button()
        Me.dgvCoil = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpDspNowCoil = New System.Windows.Forms.GroupBox()
        Me.BtnCoilBack = New System.Windows.Forms.Button()
        Me.BtnCoilNext = New System.Windows.Forms.Button()
        CType(Me.dgvCoil, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpDspNowCoil.SuspendLayout()
        Me.SuspendLayout()
        '
        'BtnClose
        '
        Me.BtnClose.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.BtnClose.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.BtnClose.Location = New System.Drawing.Point(766, 744)
        Me.BtnClose.Name = "BtnClose"
        Me.BtnClose.Size = New System.Drawing.Size(120, 32)
        Me.BtnClose.TabIndex = 3
        Me.BtnClose.Text = "F12ÅFï¬Ç∂ÇÈ"
        Me.BtnClose.UseVisualStyleBackColor = True
        '
        'BtnCSV
        '
        Me.BtnCSV.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.BtnCSV.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.BtnCSV.ForeColor = System.Drawing.Color.Black
        Me.BtnCSV.Location = New System.Drawing.Point(626, 744)
        Me.BtnCSV.Name = "BtnCSV"
        Me.BtnCSV.Size = New System.Drawing.Size(132, 32)
        Me.BtnCSV.TabIndex = 5
        Me.BtnCSV.Text = "F11ÅFCSVèoóÕ"
        Me.BtnCSV.UseVisualStyleBackColor = True
        '
        'dgvCoil
        '
        Me.dgvCoil.AllowUserToAddRows = False
        Me.dgvCoil.AllowUserToDeleteRows = False
        Me.dgvCoil.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvCoil.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCoil.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCoil.Cursor = System.Windows.Forms.Cursors.Default
        Me.dgvCoil.Location = New System.Drawing.Point(8, 24)
        Me.dgvCoil.Name = "dgvCoil"
        Me.dgvCoil.ReadOnly = True
        Me.dgvCoil.RowTemplate.Height = 21
        Me.dgvCoil.Size = New System.Drawing.Size(878, 707)
        Me.dgvCoil.TabIndex = 7
        '
        'grpDspNowCoil
        '
        Me.grpDspNowCoil.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.grpDspNowCoil.Controls.Add(Me.dgvCoil)
        Me.grpDspNowCoil.Cursor = System.Windows.Forms.Cursors.Hand
        Me.grpDspNowCoil.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle))
        Me.grpDspNowCoil.ForeColor = System.Drawing.Color.Navy
        Me.grpDspNowCoil.Location = New System.Drawing.Point(0, 0)
        Me.grpDspNowCoil.Name = "grpDspNowCoil"
        Me.grpDspNowCoil.Size = New System.Drawing.Size(894, 739)
        Me.grpDspNowCoil.TabIndex = 4
        Me.grpDspNowCoil.TabStop = False
        Me.grpDspNowCoil.Text = "ÉRÉCÉãèÓïÒ"
        '
        'BtnCoilBack
        '
        Me.BtnCoilBack.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.BtnCoilBack.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.BtnCoilBack.ForeColor = System.Drawing.Color.Black
        Me.BtnCoilBack.Location = New System.Drawing.Point(487, 744)
        Me.BtnCoilBack.Name = "BtnCoilBack"
        Me.BtnCoilBack.Size = New System.Drawing.Size(132, 32)
        Me.BtnCoilBack.TabIndex = 7
        Me.BtnCoilBack.Text = "F2ÅFëOÉRÉCÉã"
        Me.BtnCoilBack.UseVisualStyleBackColor = True
        '
        'BtnCoilNext
        '
        Me.BtnCoilNext.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.BtnCoilNext.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.BtnCoilNext.ForeColor = System.Drawing.Color.Black
        Me.BtnCoilNext.Location = New System.Drawing.Point(347, 744)
        Me.BtnCoilNext.Name = "BtnCoilNext"
        Me.BtnCoilNext.Size = New System.Drawing.Size(132, 32)
        Me.BtnCoilNext.TabIndex = 8
        Me.BtnCoilNext.Text = "F1ÅFéüÉRÉCÉã"
        Me.BtnCoilNext.UseVisualStyleBackColor = True
        '
        'frmDspCoil
        '
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None
        Me.BackColor = System.Drawing.SystemColors.Control
        Me.ClientSize = New System.Drawing.Size(895, 780)
        Me.Controls.Add(Me.BtnCoilBack)
        Me.Controls.Add(Me.BtnCoilNext)
        Me.Controls.Add(Me.BtnClose)
        Me.Controls.Add(Me.BtnCSV)
        Me.Controls.Add(Me.grpDspNowCoil)
        Me.Name = "frmDspCoil"
        Me.Text = "ÉRÉCÉãèÓïÒâÊñ "
        Me.TopMost = False
        CType(Me.dgvCoil, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpDspNowCoil.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents BtnClose As System.Windows.Forms.Button
    Friend WithEvents BtnCSV As System.Windows.Forms.Button
    Friend WithEvents dgvCoil As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents grpDspNowCoil As System.Windows.Forms.GroupBox
    Friend WithEvents BtnCoilBack As System.Windows.Forms.Button
    Friend WithEvents BtnCoilNext As System.Windows.Forms.Button

End Class
