<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcReduction
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
        Me.dgvReduction = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvReduction, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.dgvReduction)
        Me.grpBase.Text = "�k�����ݒ�"
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "�I��"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "�ݒ�"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "�ؑ�"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "��ʕۑ�"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "��ʈ��"
        '
        'dgvReduction
        '
        Me.dgvReduction.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvReduction.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvReduction.Location = New System.Drawing.Point(12, 28)
        Me.dgvReduction.Name = "dgvReduction"
        Me.dgvReduction.RowTemplate.Height = 21
        Me.dgvReduction.Size = New System.Drawing.Size(1048, 882)
        Me.dgvReduction.TabIndex = 68
        '
        'frmEpcReduction
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmEpcReduction"
        Me.Text = "�k�����ݒ���"
        Me.grpBase.ResumeLayout(False)
        CType(Me.dgvReduction, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvReduction As tClassLibrary.tCnt.tCnt_DataGridView

End Class
