<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcEdge
    Inherits frmMain_AutoParSet_UpList_SysName

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
        Me.camName = New KA_PARCOLOR.CamName()
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        Me.dgvPScode = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.dgvCamset = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.dgvCamsetAngleScode = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.lblSCode = New System.Windows.Forms.Label()
        Me.cmbSCode = New System.Windows.Forms.ComboBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.dgvCamSetScode = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
		Me.GroupBox3 = New System.Windows.Forms.GroupBox()
		Me.dgvCamAngleScode = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.pnlItem.SuspendLayout()
        Me.grpBase.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        CType(Me.dgvPScode, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox6.SuspendLayout()
        CType(Me.dgvCamset, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        CType(Me.dgvCamsetAngleScode, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        CType(Me.dgvCamSetScode, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.GroupBox3.SuspendLayout()
		CType(Me.dgvCamAngleScode, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label5)
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.GroupBox2)
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.lblSCode)
        Me.grpBase.Controls.Add(Me.cmbSCode)
        Me.grpBase.Controls.Add(Me.Label6)
        Me.grpBase.Controls.Add(Me.Label7)
		Me.grpBase.Controls.Add(Me.GroupBox3)
        Me.grpBase.Controls.Add(Me.GroupBox1)
        Me.grpBase.Controls.Add(Me.GroupBox5)
        Me.grpBase.Controls.Add(Me.GroupBox6)
        Me.grpBase.Text = "エッジ検出設定"
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Text = "次頁"
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.Text = "前頁"
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
        Me.camName.TabIndex = 10
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.dgvPScode)
        Me.GroupBox5.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox5.ForeColor = System.Drawing.SystemColors.ControlText
		Me.GroupBox5.Location = New System.Drawing.Point(296, 328)
        Me.GroupBox5.Name = "GroupBox5"
		Me.GroupBox5.Size = New System.Drawing.Size(764, 183)
		Me.GroupBox5.TabIndex = 3
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "検査表面状態別"
        '
        'dgvPScode
        '
        Me.dgvPScode.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvPScode.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvPScode.Location = New System.Drawing.Point(12, 24)
        Me.dgvPScode.Name = "dgvPScode"
        Me.dgvPScode.RowTemplate.Height = 21
		Me.dgvPScode.Size = New System.Drawing.Size(532, 153)
        Me.dgvPScode.TabIndex = 0
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.dgvCamset)
        Me.GroupBox6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox6.Location = New System.Drawing.Point(8, 28)
        Me.GroupBox6.Name = "GroupBox6"
		Me.GroupBox6.Size = New System.Drawing.Size(701, 240)
        Me.GroupBox6.TabIndex = 42
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "カメラセット別"
        '
        'dgvCamset
        '
        Me.dgvCamset.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCamset.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCamset.Location = New System.Drawing.Point(12, 24)
        Me.dgvCamset.Name = "dgvCamset"
        Me.dgvCamset.RowTemplate.Height = 21
		Me.dgvCamset.Size = New System.Drawing.Size(679, 210)
        Me.dgvCamset.TabIndex = 1
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dgvCamsetAngleScode)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.ForeColor = System.Drawing.SystemColors.ControlText
		Me.GroupBox2.Location = New System.Drawing.Point(296, 515)
        Me.GroupBox2.Name = "GroupBox2"
		Me.GroupBox2.Size = New System.Drawing.Size(764, 388)
		Me.GroupBox2.TabIndex = 4
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "カメラ角度・カメラセット・検査表面状態別 "
        '
        'dgvCamsetAngleScode
        '
        Me.dgvCamsetAngleScode.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCamsetAngleScode.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCamsetAngleScode.Location = New System.Drawing.Point(12, 20)
        Me.dgvCamsetAngleScode.Name = "dgvCamsetAngleScode"
        Me.dgvCamsetAngleScode.RowTemplate.Height = 21
		Me.dgvCamsetAngleScode.Size = New System.Drawing.Size(748, 362)
        Me.dgvCamsetAngleScode.TabIndex = 1
        '
        'Label4
        '
        Me.Label4.BackColor = System.Drawing.Color.Gray
        Me.Label4.Location = New System.Drawing.Point(4, 296)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(1064, 4)
        Me.Label4.TabIndex = 51
        '
        'lblSCode
        '
        Me.lblSCode.BackColor = System.Drawing.Color.LightSteelBlue
        Me.lblSCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSCode.ForeColor = System.Drawing.SystemColors.ControlText
		Me.lblSCode.Location = New System.Drawing.Point(12, 380)
        Me.lblSCode.Name = "lblSCode"
        Me.lblSCode.Size = New System.Drawing.Size(280, 24)
        Me.lblSCode.TabIndex = 49
        Me.lblSCode.Text = "12345678901234567890123456789012"
        Me.lblSCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCode
        '
        Me.cmbSCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.cmbSCode.FormattingEnabled = True
        Me.cmbSCode.Location = New System.Drawing.Point(12, 332)
        Me.cmbSCode.Name = "cmbSCode"
        Me.cmbSCode.Size = New System.Drawing.Size(156, 24)
		Me.cmbSCode.TabIndex = 0
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
		Me.Label6.Location = New System.Drawing.Point(12, 356)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(280, 24)
		Me.Label6.TabIndex = 0
        Me.Label6.Text = "検査表面状態名称"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(12, 308)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(156, 24)
		Me.Label7.TabIndex = 4
        Me.Label7.Text = "検査表面状態No"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.dgvCamSetScode)
        Me.GroupBox1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.ForeColor = System.Drawing.SystemColors.ControlText
		Me.GroupBox1.Location = New System.Drawing.Point(12, 616)
        Me.GroupBox1.Name = "GroupBox1"
		Me.GroupBox1.Size = New System.Drawing.Size(280, 288)
		Me.GroupBox1.TabIndex = 2
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "カメラセット・検査表面状態別 "
        '
        'dgvCamSetScode
        '
        Me.dgvCamSetScode.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCamSetScode.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvCamSetScode.Location = New System.Drawing.Point(12, 28)
        Me.dgvCamSetScode.Name = "dgvCamSetScode"
        Me.dgvCamSetScode.RowTemplate.Height = 21
		Me.dgvCamSetScode.Size = New System.Drawing.Size(193, 254)
        Me.dgvCamSetScode.TabIndex = 0
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.Color.Gray
        Me.Label2.Location = New System.Drawing.Point(4, 908)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(1064, 4)
        Me.Label2.TabIndex = 53
        '
        'Label1
        '
        Me.Label1.BackColor = System.Drawing.Color.Gray
        Me.Label1.Location = New System.Drawing.Point(1064, 300)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(4, 612)
        Me.Label1.TabIndex = 52
        '
        'Label5
        '
        Me.Label5.BackColor = System.Drawing.Color.Gray
        Me.Label5.Location = New System.Drawing.Point(4, 296)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(4, 612)
        Me.Label5.TabIndex = 54
        '
		'GroupBox3
		'
		Me.GroupBox3.Controls.Add(Me.dgvCamAngleScode)
		Me.GroupBox3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.GroupBox3.ForeColor = System.Drawing.SystemColors.ControlText
		Me.GroupBox3.Location = New System.Drawing.Point(12, 440)
		Me.GroupBox3.Name = "GroupBox3"
		Me.GroupBox3.Size = New System.Drawing.Size(280, 170)
		Me.GroupBox3.TabIndex = 1
		Me.GroupBox3.TabStop = False
		Me.GroupBox3.Text = "カメラセット・検査表面状態別 "
		'
		'dgvCamAngleScode
		'
		Me.dgvCamAngleScode.BorderStyle = System.Windows.Forms.BorderStyle.None
		Me.dgvCamAngleScode.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvCamAngleScode.Location = New System.Drawing.Point(12, 24)
		Me.dgvCamAngleScode.Name = "dgvCamAngleScode"
		Me.dgvCamAngleScode.RowTemplate.Height = 21
		Me.dgvCamAngleScode.Size = New System.Drawing.Size(252, 130)
		Me.dgvCamAngleScode.TabIndex = 0
		'
        'frmEpcEdge
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.camName)
        Me.Name = "frmEpcEdge"
        Me.Text = "エッジ検出設定画面"
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.Controls.SetChildIndex(Me.pnlItem, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.camName, 0)
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        Me.GroupBox5.ResumeLayout(False)
        CType(Me.dgvPScode, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox6.ResumeLayout(False)
        CType(Me.dgvCamset, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        CType(Me.dgvCamsetAngleScode, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        CType(Me.dgvCamSetScode, System.ComponentModel.ISupportInitialize).EndInit()
		Me.GroupBox3.ResumeLayout(False)
		CType(Me.dgvCamAngleScode, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents camName As KA_PARCOLOR.CamName
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCamsetAngleScode As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents lblSCode As System.Windows.Forms.Label
    Friend WithEvents cmbSCode As System.Windows.Forms.ComboBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCamSetScode As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvPScode As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCamset As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label5 As System.Windows.Forms.Label
	Friend WithEvents GroupBox3 As GroupBox
	Friend WithEvents dgvCamAngleScode As tClassLibrary.tCnt.tCnt_DataGridView
End Class
