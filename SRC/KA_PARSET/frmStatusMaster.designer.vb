<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmStatusMaster
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
        Me.dgvData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.dgvData)
        Me.grpBase.Text = "�@��}�X�^�[�ݒ�"
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
        'dgvData
        '
        Me.dgvData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvData.Location = New System.Drawing.Point(12, 24)
        Me.dgvData.Name = "dgvData"
        Me.dgvData.RowTemplate.Height = 21
        Me.dgvData.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.dgvData.Size = New System.Drawing.Size(1048, 840)
        Me.dgvData.TabIndex = 8
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("�l�r ����", 12.0!)
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(12, 896)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(792, 16)
        Me.Label1.TabIndex = 9
        Me.Label1.Text = "��2 �D��x��1���ł������Ȃ�܂��B �ݒ肵�Ȃ��ꍇ�⓯��̏ꍇ�́A�@��ID�̏������ݒ肪�D�悳��܂��B"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("�l�r ����", 12.0!)
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(12, 872)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(696, 16)
        Me.Label2.TabIndex = 10
        Me.Label2.Text = "��1 0�́u����v�A1�́u�ُ�v�A�󔒂̓X�e�[�^�X���������܂���B�C�ӂ̒l���ݒ�\�ł��B"
        '
        'frmStatusMaster
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmStatusMaster"
        Me.Text = "�@��}�X�^�[�ݒ���"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label

End Class
