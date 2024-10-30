<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcCropShrink
    Inherits frmMain_ParSet_SysName

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
        Me.dgvCropShrink = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvCropShrink, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.dgvCropShrink)
        Me.grpBase.Text = "ä‘à¯Ç´ê›íË"
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "èIóπ"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "ê›íË"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "êÿë÷"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "âÊñ ï€ë∂"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "âÊñ àÛç¸"
        '
        'dgvCropShrink
        '
        Me.dgvCropShrink.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCropShrink.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCropShrink.Location = New System.Drawing.Point(12, 28)
        Me.dgvCropShrink.Name = "dgvCropShrink"
        Me.dgvCropShrink.RowTemplate.Height = 21
        Me.dgvCropShrink.Size = New System.Drawing.Size(1012, 886)
        Me.dgvCropShrink.TabIndex = 68
        '
        'frmEpcCropShrink
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmEpcCropShrink"
        Me.Text = "ä‘à¯Ç´ê›íËâÊñ "
        Me.grpBase.ResumeLayout(False)
        CType(Me.dgvCropShrink, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents dgvCropShrink As tClassLibrary.tCnt.tCnt_DataGridView

End Class
