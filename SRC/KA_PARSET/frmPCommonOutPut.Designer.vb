<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPCommonOutPut
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
        Me.grpPComOut = New System.Windows.Forms.GroupBox()
        Me.dgvPComOutEtc = New System.Windows.Forms.DataGridView()
        Me.dgvPComOut = New System.Windows.Forms.DataGridView()
        Me.grpBase.SuspendLayout()
        Me.grpPComOut.SuspendLayout()
        CType(Me.dgvPComOutEtc, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvPComOut, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.grpPComOut)
        Me.grpBase.Text = ""
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "終了"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "設定"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "切替"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "画面保存"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "画面印刷"
        '
        'grpPComOut
        '
        Me.grpPComOut.Controls.Add(Me.dgvPComOutEtc)
        Me.grpPComOut.Controls.Add(Me.dgvPComOut)
        Me.grpPComOut.Location = New System.Drawing.Point(27, 25)
        Me.grpPComOut.Name = "grpPComOut"
        Me.grpPComOut.Size = New System.Drawing.Size(594, 878)
        Me.grpPComOut.TabIndex = 0
        Me.grpPComOut.TabStop = False
        Me.grpPComOut.Text = "検査パターン固有出力設定"
        '
        'dgvPComOutEtc
        '
        Me.dgvPComOutEtc.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvPComOutEtc.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvPComOutEtc.Location = New System.Drawing.Point(8, 847)
        Me.dgvPComOutEtc.Name = "dgvPComOutEtc"
        Me.dgvPComOutEtc.RowTemplate.Height = 21
        Me.dgvPComOutEtc.Size = New System.Drawing.Size(551, 22)
        Me.dgvPComOutEtc.TabIndex = 65
        '
        'dgvPComOut
        '
        Me.dgvPComOut.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvPComOut.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvPComOut.Location = New System.Drawing.Point(8, 24)
        Me.dgvPComOut.Name = "dgvPComOut"
        Me.dgvPComOut.RowTemplate.Height = 21
        Me.dgvPComOut.Size = New System.Drawing.Size(568, 819)
        Me.dgvPComOut.TabIndex = 64
        '
        'frmPCommonOutPut
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmPCommonOutPut"
        Me.Text = "検査パターン固有出力設定画面"
        Me.grpBase.ResumeLayout(False)
        Me.grpPComOut.ResumeLayout(False)
        CType(Me.dgvPComOutEtc, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvPComOut, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents grpPComOut As System.Windows.Forms.GroupBox
    Friend WithEvents dgvPComOut As System.Windows.Forms.DataGridView
    Friend WithEvents dgvPComOutEtc As System.Windows.Forms.DataGridView

End Class
