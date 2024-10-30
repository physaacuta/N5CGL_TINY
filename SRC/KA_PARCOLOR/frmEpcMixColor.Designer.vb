<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcMixColor
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
        Me.dgvCamSet = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.camName = New KA_PARCOLOR.CamName()
        Me.grpBase.SuspendLayout()
        Me.GroupBox6.SuspendLayout()
        CType(Me.dgvCamSet, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.GroupBox6)
        Me.grpBase.Text = "混色分離設定"
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(12, 908)
        Me.grpFunction.Size = New System.Drawing.Size(1240, 68)
        '
        'btnF02
        '
        Me.btnF02.Size = New System.Drawing.Size(93, 32)
        '
        'lblF02_b
        '
        Me.lblF02_b.Size = New System.Drawing.Size(93, 32)
        '
        'btnF03
        '
        Me.btnF03.Size = New System.Drawing.Size(93, 32)
        '
        'lblF03_b
        '
        Me.lblF03_b.Size = New System.Drawing.Size(93, 32)
        '
        'btnF01
        '
        Me.btnF01.Size = New System.Drawing.Size(93, 32)
        '
        'lblF01_b
        '
        Me.lblF01_b.Size = New System.Drawing.Size(93, 32)
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Size = New System.Drawing.Size(93, 32)
        Me.btnF12.Text = "終了"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Size = New System.Drawing.Size(93, 32)
        Me.btnF11.Text = "設定"
        '
        'btnF10
        '
        Me.btnF10.Size = New System.Drawing.Size(93, 32)
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Size = New System.Drawing.Size(93, 32)
        Me.btnF09.Text = "切替"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Size = New System.Drawing.Size(93, 32)
        Me.btnF08.Text = "画面保存"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Size = New System.Drawing.Size(93, 32)
        Me.btnF07.Text = "画面印刷"
        '
        'btnF06
        '
        Me.btnF06.Size = New System.Drawing.Size(93, 32)
        '
        'btnF05
        '
        Me.btnF05.Size = New System.Drawing.Size(93, 32)
        '
        'btnF04
        '
        Me.btnF04.Size = New System.Drawing.Size(93, 32)
        '
        'lblF12_b
        '
        Me.lblF12_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF11_b
        '
        Me.lblF11_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF10_b
        '
        Me.lblF10_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF09_b
        '
        Me.lblF09_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF08_b
        '
        Me.lblF08_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF07_b
        '
        Me.lblF07_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF06_b
        '
        Me.lblF06_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF05_b
        '
        Me.lblF05_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF04_b
        '
        Me.lblF04_b.Size = New System.Drawing.Size(93, 32)
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.dgvCamSet)
        Me.GroupBox6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox6.Location = New System.Drawing.Point(12, 28)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(1048, 516)
        Me.GroupBox6.TabIndex = 50
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "カメラセット別"
        '
        'dgvCamSet
        '
        Me.dgvCamSet.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCamSet.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCamSet.Location = New System.Drawing.Point(8, 24)
        Me.dgvCamSet.Name = "dgvCamSet"
        Me.dgvCamSet.RowTemplate.Height = 21
        Me.dgvCamSet.Size = New System.Drawing.Size(1032, 480)
        Me.dgvCamSet.TabIndex = 1
        '
        'camName
        '
        Me.camName.Location = New System.Drawing.Point(4, 412)
        Me.camName.Name = "camName"
        Me.camName.Size = New System.Drawing.Size(188, 224)
        Me.camName.TabIndex = 9
        '
        'frmEpcMixColor
        '
        Me.ClientSize = New System.Drawing.Size(1268, 989)
        Me.Controls.Add(Me.camName)
        Me.MaximizeBox = False
        Me.Name = "frmEpcMixColor"
        Me.Text = "混色分離設定画面"
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.camName, 0)
        Me.grpBase.ResumeLayout(False)
        Me.GroupBox6.ResumeLayout(False)
        CType(Me.dgvCamSet, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCamSet As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents camName As KA_PARCOLOR.CamName

End Class
