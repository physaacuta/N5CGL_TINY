<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcMerge
    Inherits frmMain_AutoParSet_SysName

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
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.dgvCommon = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.camName = New KA_PARCOLOR.CamName()
        Me.grpBase.SuspendLayout()
        Me.GroupBox6.SuspendLayout()
        CType(Me.dgvCommon, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.GroupBox6)
        Me.grpBase.Text = "���������ݒ�"
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
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.dgvCommon)
        Me.GroupBox6.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox6.Location = New System.Drawing.Point(12, 28)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(632, 520)
        Me.GroupBox6.TabIndex = 49
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "����"
        '
        'dgvCommon
        '
        Me.dgvCommon.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCommon.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCommon.Location = New System.Drawing.Point(12, 24)
        Me.dgvCommon.Name = "dgvCommon"
        Me.dgvCommon.RowTemplate.Height = 21
        Me.dgvCommon.Size = New System.Drawing.Size(607, 488)
        Me.dgvCommon.TabIndex = 50
        '
        'camName
        '
        Me.camName.Location = New System.Drawing.Point(4, 412)
        Me.camName.Name = "camName"
        Me.camName.Size = New System.Drawing.Size(188, 224)
        Me.camName.TabIndex = 9
        '
        'frmEpcMerge
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.camName)
        Me.Name = "frmEpcMerge"
        Me.Text = "���������ݒ���"
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.camName, 0)
        Me.grpBase.ResumeLayout(False)
        Me.GroupBox6.ResumeLayout(False)
        CType(Me.dgvCommon, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCommon As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents camName As KA_PARCOLOR.CamName

End Class
