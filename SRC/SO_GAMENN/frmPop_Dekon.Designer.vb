<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_Dekon
    Inherits KizuForm.frmPop_Search

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
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        Me.grpDate.SuspendLayout()
        Me.grpTime.SuspendLayout()
        Me.grpSearch.SuspendLayout()
        Me.grpDisp.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        CType(Me.picSearch, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlMsg.SuspendLayout()
        CType(Me.dgvResult, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnEnd
        '
        Me.btnEnd.Location = New System.Drawing.Point(820, 62)
        Me.btnEnd.Text = "F12ÅFï¬Ç∂ÇÈ"
        '
        'grpSearch
        '
        Me.grpSearch.Controls.Add(Me.Label7)
        Me.grpSearch.Controls.Add(Me.Label6)
        Me.grpSearch.Size = New System.Drawing.Size(788, 168)
        Me.grpSearch.Controls.SetChildIndex(Me.Label6, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.Label1, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.Label2, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.GroupBox1, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.txtCoil, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.txtSaveName, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.grpDate, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.grpTime, 0)
        Me.grpSearch.Controls.SetChildIndex(Me.Label7, 0)
        '
        'Label2
        '
        Me.Label2.Size = New System.Drawing.Size(60, 16)
        Me.Label2.Text = "ä«óùNo"
        '
        'Label1
        '
        Me.Label1.Size = New System.Drawing.Size(60, 16)
        Me.Label1.Text = "ê∂éYNO"
        '
        'grpDisp
        '
        Me.grpDisp.Size = New System.Drawing.Size(788, 300)
        '
        'GroupBox2
        '
        Me.GroupBox2.Size = New System.Drawing.Size(144, 164)
        Me.GroupBox2.Visible = False
        '
        'picSearch
        '
        Me.picSearch.Size = New System.Drawing.Size(804, 492)
        '
        'btnCancel
        '
        Me.btnCancel.Location = New System.Drawing.Point(696, 508)
        Me.btnCancel.Visible = False
        '
        'btnSelect
        '
        Me.btnSelect.Location = New System.Drawing.Point(572, 508)
        Me.btnSelect.Visible = False
        '
        'btnSearch
        '
        Me.btnSearch.Location = New System.Drawing.Point(820, 20)
        Me.btnSearch.Text = "F5ÅFåüçı"
        '
        'chkNagate
        '
        Me.chkNagate.Visible = False
        '
        'chkHaba
        '
        Me.chkHaba.Visible = False
        '
        'dgvResult
        '
        Me.dgvResult.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvResult.Location = New System.Drawing.Point(4, 548)
        Me.dgvResult.RowTemplate.Height = 21
        Me.dgvResult.Size = New System.Drawing.Size(952, 328)
        '
        'btnCsv
        '
        Me.btnCsv.Location = New System.Drawing.Point(820, 508)
        '
        'Label6
        '
        Me.Label6.Location = New System.Drawing.Point(360, 24)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(184, 20)
        Me.Label6.TabIndex = 7
        Me.Label6.Text = "% ÅF 0ï∂éöà»è„ÇÃàÍív"
        '
        'Label7
        '
        Me.Label7.Location = New System.Drawing.Point(360, 44)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(184, 20)
        Me.Label7.TabIndex = 8
        Me.Label7.Text = "_ ÅF îCà”1ï∂éöÇÃàÍív"
        '
        'frmPop_Dekon
        '
        Me.ClientSize = New System.Drawing.Size(958, 889)
        Me.Name = "frmPop_Dekon"
        Me.Text = "èoë§íPà ·rç¨ì¸ó¶CSVèoóÕâÊñ "
        Me.TopMost = False
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.grpDate.ResumeLayout(False)
        Me.grpDate.PerformLayout()
        Me.grpTime.ResumeLayout(False)
        Me.grpTime.PerformLayout()
        Me.grpSearch.ResumeLayout(False)
        Me.grpSearch.PerformLayout()
        Me.grpDisp.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox4.ResumeLayout(False)
        CType(Me.picSearch, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlMsg.ResumeLayout(False)
        Me.pnlMsg.PerformLayout()
        CType(Me.dgvResult, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label

End Class
