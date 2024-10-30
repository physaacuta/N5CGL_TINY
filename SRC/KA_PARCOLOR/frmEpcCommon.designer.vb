<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmEpcCommon
    Inherits frmMain_AutoParSet_SysName

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.camName = New KA_PARCOLOR.CamName()
        Me.ofdFile = New System.Windows.Forms.OpenFileDialog()
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.dgvCamSet = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.dgvCommon = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpBase.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        CType(Me.dgvCamSet, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox6.SuspendLayout()
        CType(Me.dgvCommon, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.GroupBox4)
        Me.grpBase.Controls.Add(Me.GroupBox6)
        Me.grpBase.Text = "固定パラメータ設定"
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
        'camName
        '
        Me.camName.Location = New System.Drawing.Point(4, 412)
        Me.camName.Name = "camName"
        Me.camName.Size = New System.Drawing.Size(188, 224)
        Me.camName.TabIndex = 8
        '
        'ofdFile
        '
        Me.ofdFile.FileName = "OpenFileDialog1"
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.dgvCamSet)
        Me.GroupBox4.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox4.Location = New System.Drawing.Point(636, 20)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(416, 495)
        Me.GroupBox4.TabIndex = 55
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "カメラセット別"
        '
        'dgvCamSet
        '
        Me.dgvCamSet.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCamSet.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCamSet.Location = New System.Drawing.Point(6, 24)
        Me.dgvCamSet.Name = "dgvCamSet"
        Me.dgvCamSet.RowTemplate.Height = 21
        Me.dgvCamSet.Size = New System.Drawing.Size(403, 462)
        Me.dgvCamSet.TabIndex = 50
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.dgvCommon)
        Me.GroupBox6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox6.Location = New System.Drawing.Point(8, 20)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(624, 896)
        Me.GroupBox6.TabIndex = 54
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "共通"
        '
        'dgvCommon
        '
        Me.dgvCommon.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCommon.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCommon.Location = New System.Drawing.Point(8, 24)
        Me.dgvCommon.Name = "dgvCommon"
        Me.dgvCommon.RowTemplate.Height = 21
        Me.dgvCommon.Size = New System.Drawing.Size(604, 864)
        Me.dgvCommon.TabIndex = 50
        '
        'frmEpcCommon
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.camName)
        Me.Name = "frmEpcCommon"
        Me.Text = "固定パラメータ設定画面"
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.camName, 0)
        Me.grpBase.ResumeLayout(False)
        Me.GroupBox4.ResumeLayout(False)
        CType(Me.dgvCamSet, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox6.ResumeLayout(False)
        CType(Me.dgvCommon, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents camName As KA_PARCOLOR.CamName
    Friend WithEvents ofdFile As System.Windows.Forms.OpenFileDialog
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCamSet As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCommon As tClassLibrary.tCnt.tCnt_DataGridView
End Class
