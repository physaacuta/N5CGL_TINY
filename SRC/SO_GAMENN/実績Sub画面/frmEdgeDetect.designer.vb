<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEdgeDetect
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
        Me.cmdEnd = New System.Windows.Forms.Button()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.dgvChangeInf_U = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvChangeInf_O = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        CType(Me.dgvChangeInf_U, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvChangeInf_O, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'cmdEnd
        '
        Me.cmdEnd.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cmdEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmdEnd.Location = New System.Drawing.Point(808, 704)
        Me.cmdEnd.Name = "cmdEnd"
        Me.cmdEnd.Size = New System.Drawing.Size(116, 32)
        Me.cmdEnd.TabIndex = 4
        Me.cmdEnd.Text = "F12:ï¬Ç∂ÇÈ"
        Me.cmdEnd.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.dgvChangeInf_U)
        Me.GroupBox1.Controls.Add(Me.dgvChangeInf_O)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(12, 8)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(912, 688)
        Me.GroupBox1.TabIndex = 5
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ÉGÉbÉWåüèoÉÇÅ[ÉhïœâªèÓïÒ"
        '
        'dgvChangeInf_U
        '
        Me.dgvChangeInf_U.Location = New System.Drawing.Point(12, 372)
        Me.dgvChangeInf_U.Name = "dgvChangeInf_U"
        Me.dgvChangeInf_U.RowTemplate.Height = 21
        Me.dgvChangeInf_U.Size = New System.Drawing.Size(888, 300)
        Me.dgvChangeInf_U.TabIndex = 5
        '
        'dgvChangeInf_O
        '
        Me.dgvChangeInf_O.Location = New System.Drawing.Point(12, 44)
        Me.dgvChangeInf_O.Name = "dgvChangeInf_O"
        Me.dgvChangeInf_O.RowTemplate.Height = 21
        Me.dgvChangeInf_O.Size = New System.Drawing.Size(888, 300)
        Me.dgvChangeInf_O.TabIndex = 4
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(16, 352)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(58, 15)
        Me.Label2.TabIndex = 3
        Me.Label2.Text = "Åyó†Åz"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(12, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(58, 15)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "Åyï\Åz"
        '
        'frmEdgeDetect
        '
        Me.ClientSize = New System.Drawing.Size(936, 743)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.cmdEnd)
        Me.Name = "frmEdgeDetect"
        Me.Text = "ÉGÉbÉWåüèoèÛãµâÊñ "
        Me.TopMost = False
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.dgvChangeInf_U, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvChangeInf_O, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents cmdEnd As System.Windows.Forms.Button
	Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents dgvChangeInf_O As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvChangeInf_U As tClassLibrary.tCnt.tCnt_DataGridView

End Class
