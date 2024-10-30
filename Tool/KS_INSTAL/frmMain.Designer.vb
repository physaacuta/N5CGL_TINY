<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
	Inherits System.Windows.Forms.Form

	'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
	<System.Diagnostics.DebuggerNonUserCode()> _
	Protected Overrides Sub Dispose(ByVal disposing As Boolean)
		Try
			If disposing AndAlso components IsNot Nothing Then
				components.Dispose()
			End If
		Finally
			MyBase.Dispose(disposing)
		End Try
	End Sub

	'Windows フォーム デザイナで必要です。
	Private components As System.ComponentModel.IContainer

	'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
	'Windows フォーム デザイナを使用して変更できます。  
	'コード エディタを使って変更しないでください。
	<System.Diagnostics.DebuggerStepThrough()> _
	Private Sub InitializeComponent()
		Dim TreeNode1 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード2")
		Dim TreeNode2 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード0", New System.Windows.Forms.TreeNode() {TreeNode1})
		Dim TreeNode3 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード3")
		Dim TreeNode4 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード4")
		Dim TreeNode5 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード5")
		Dim TreeNode6 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード1", New System.Windows.Forms.TreeNode() {TreeNode3, TreeNode4, TreeNode5})
		Dim TreeNode7 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード2")
		Dim TreeNode8 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード0", New System.Windows.Forms.TreeNode() {TreeNode7})
		Dim TreeNode9 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード3")
		Dim TreeNode10 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード4")
		Dim TreeNode11 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード5")
		Dim TreeNode12 As System.Windows.Forms.TreeNode = New System.Windows.Forms.TreeNode("ノード1", New System.Windows.Forms.TreeNode() {TreeNode9, TreeNode10, TreeNode11})
		Me.txtSetInstalFolder = New System.Windows.Forms.TextBox
		Me.grpSet = New System.Windows.Forms.GroupBox
		Me.trvSetInf = New System.Windows.Forms.TreeView
		Me.btnSelectFolder = New System.Windows.Forms.Button
		Me.Label1 = New System.Windows.Forms.Label
		Me.lblMsgBox = New System.Windows.Forms.Label
		Me.GroupBox2 = New System.Windows.Forms.GroupBox
		Me.trvNowInf = New System.Windows.Forms.TreeView
		Me.Button1 = New System.Windows.Forms.Button
		Me.Label2 = New System.Windows.Forms.Label
		Me.txtNowInstalFolder = New System.Windows.Forms.TextBox
		Me.rdoInstal1 = New System.Windows.Forms.RadioButton
		Me.GroupBox1 = New System.Windows.Forms.GroupBox
		Me.rdoInstal3 = New System.Windows.Forms.RadioButton
		Me.rdoInstal2 = New System.Windows.Forms.RadioButton
		Me.GroupBox3 = New System.Windows.Forms.GroupBox
		Me.btnAllUninstall = New System.Windows.Forms.Button
		Me.btnExecute = New System.Windows.Forms.Button
		Me.GroupBox4 = New System.Windows.Forms.GroupBox
		Me.Label4 = New System.Windows.Forms.Label
		Me.Label3 = New System.Windows.Forms.Label
		Me.grpSet.SuspendLayout()
		Me.GroupBox2.SuspendLayout()
		Me.GroupBox1.SuspendLayout()
		Me.GroupBox3.SuspendLayout()
		Me.GroupBox4.SuspendLayout()
		Me.SuspendLayout()
		'
		'txtSetInstalFolder
		'
		Me.txtSetInstalFolder.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.txtSetInstalFolder.Location = New System.Drawing.Point(12, 40)
		Me.txtSetInstalFolder.Name = "txtSetInstalFolder"
		Me.txtSetInstalFolder.Size = New System.Drawing.Size(248, 23)
		Me.txtSetInstalFolder.TabIndex = 0
		'
		'grpSet
		'
		Me.grpSet.Controls.Add(Me.trvSetInf)
		Me.grpSet.Controls.Add(Me.btnSelectFolder)
		Me.grpSet.Controls.Add(Me.Label1)
		Me.grpSet.Controls.Add(Me.txtSetInstalFolder)
		Me.grpSet.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.grpSet.ForeColor = System.Drawing.Color.DarkBlue
		Me.grpSet.Location = New System.Drawing.Point(292, 8)
		Me.grpSet.Name = "grpSet"
		Me.grpSet.Size = New System.Drawing.Size(308, 468)
		Me.grpSet.TabIndex = 1
		Me.grpSet.TabStop = False
		Me.grpSet.Text = "アンインストール設定"
		'
		'trvSetInf
		'
		Me.trvSetInf.CheckBoxes = True
		Me.trvSetInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.trvSetInf.Location = New System.Drawing.Point(12, 72)
		Me.trvSetInf.Name = "trvSetInf"
		TreeNode1.Name = "ノード2"
		TreeNode1.Text = "ノード2"
		TreeNode2.Name = "ノード0"
		TreeNode2.Text = "ノード0"
		TreeNode3.Name = "ノード3"
		TreeNode3.Text = "ノード3"
		TreeNode4.Name = "ノード4"
		TreeNode4.Text = "ノード4"
		TreeNode5.Name = "ノード5"
		TreeNode5.Text = "ノード5"
		TreeNode6.Name = "ノード1"
		TreeNode6.Text = "ノード1"
		Me.trvSetInf.Nodes.AddRange(New System.Windows.Forms.TreeNode() {TreeNode2, TreeNode6})
		Me.trvSetInf.Size = New System.Drawing.Size(284, 388)
		Me.trvSetInf.TabIndex = 4
		'
		'btnSelectFolder
		'
		Me.btnSelectFolder.BackColor = System.Drawing.Color.Coral
		Me.btnSelectFolder.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnSelectFolder.ForeColor = System.Drawing.SystemColors.ControlText
		Me.btnSelectFolder.Location = New System.Drawing.Point(264, 40)
		Me.btnSelectFolder.Name = "btnSelectFolder"
		Me.btnSelectFolder.Size = New System.Drawing.Size(28, 24)
		Me.btnSelectFolder.TabIndex = 2
		Me.btnSelectFolder.Text = "…"
		Me.btnSelectFolder.UseVisualStyleBackColor = False
		'
		'Label1
		'
		Me.Label1.AutoSize = True
		Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
		Me.Label1.Location = New System.Drawing.Point(8, 24)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(250, 13)
		Me.Label1.TabIndex = 1
		Me.Label1.Text = "全疵検査システム 共通インストールフォルダ"
		'
		'lblMsgBox
		'
		Me.lblMsgBox.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(255, Byte), Integer), CType(CType(192, Byte), Integer))
		Me.lblMsgBox.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblMsgBox.Location = New System.Drawing.Point(612, 16)
		Me.lblMsgBox.Name = "lblMsgBox"
		Me.lblMsgBox.Size = New System.Drawing.Size(268, 160)
		Me.lblMsgBox.TabIndex = 3
		Me.lblMsgBox.Text = "Label2"
		Me.lblMsgBox.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'GroupBox2
		'
		Me.GroupBox2.Controls.Add(Me.trvNowInf)
		Me.GroupBox2.Controls.Add(Me.Button1)
		Me.GroupBox2.Controls.Add(Me.Label2)
		Me.GroupBox2.Controls.Add(Me.txtNowInstalFolder)
		Me.GroupBox2.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.GroupBox2.ForeColor = System.Drawing.Color.DarkBlue
		Me.GroupBox2.Location = New System.Drawing.Point(8, 8)
		Me.GroupBox2.Name = "GroupBox2"
		Me.GroupBox2.Size = New System.Drawing.Size(272, 468)
		Me.GroupBox2.TabIndex = 1
		Me.GroupBox2.TabStop = False
		Me.GroupBox2.Text = "現在のインストール状態"
		'
		'trvNowInf
		'
		Me.trvNowInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.trvNowInf.Location = New System.Drawing.Point(12, 72)
		Me.trvNowInf.Name = "trvNowInf"
		TreeNode7.Name = "ノード2"
		TreeNode7.Text = "ノード2"
		TreeNode8.Name = "ノード0"
		TreeNode8.Text = "ノード0"
		TreeNode9.Name = "ノード3"
		TreeNode9.Text = "ノード3"
		TreeNode10.Name = "ノード4"
		TreeNode10.Text = "ノード4"
		TreeNode11.Name = "ノード5"
		TreeNode11.Text = "ノード5"
		TreeNode12.Name = "ノード1"
		TreeNode12.Text = "ノード1"
		Me.trvNowInf.Nodes.AddRange(New System.Windows.Forms.TreeNode() {TreeNode8, TreeNode12})
		Me.trvNowInf.Size = New System.Drawing.Size(248, 388)
		Me.trvNowInf.TabIndex = 4
		'
		'Button1
		'
		Me.Button1.BackColor = System.Drawing.Color.Coral
		Me.Button1.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Button1.ForeColor = System.Drawing.SystemColors.ControlText
		Me.Button1.Location = New System.Drawing.Point(348, 40)
		Me.Button1.Name = "Button1"
		Me.Button1.Size = New System.Drawing.Size(24, 24)
		Me.Button1.TabIndex = 2
		Me.Button1.Text = "…"
		Me.Button1.UseVisualStyleBackColor = False
		'
		'Label2
		'
		Me.Label2.AutoSize = True
		Me.Label2.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
		Me.Label2.Location = New System.Drawing.Point(8, 24)
		Me.Label2.Name = "Label2"
		Me.Label2.Size = New System.Drawing.Size(250, 13)
		Me.Label2.TabIndex = 1
		Me.Label2.Text = "全疵検査システム 共通インストールフォルダ"
		'
		'txtNowInstalFolder
		'
		Me.txtNowInstalFolder.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.txtNowInstalFolder.Location = New System.Drawing.Point(12, 40)
		Me.txtNowInstalFolder.Name = "txtNowInstalFolder"
		Me.txtNowInstalFolder.ReadOnly = True
		Me.txtNowInstalFolder.Size = New System.Drawing.Size(248, 23)
		Me.txtNowInstalFolder.TabIndex = 0
		'
		'rdoInstal1
		'
		Me.rdoInstal1.Appearance = System.Windows.Forms.Appearance.Button
		Me.rdoInstal1.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.rdoInstal1.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.rdoInstal1.ForeColor = System.Drawing.Color.Black
		Me.rdoInstal1.Location = New System.Drawing.Point(16, 28)
		Me.rdoInstal1.Name = "rdoInstal1"
		Me.rdoInstal1.Size = New System.Drawing.Size(236, 24)
		Me.rdoInstal1.TabIndex = 4
		Me.rdoInstal1.Text = "インストール"
		Me.rdoInstal1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		Me.rdoInstal1.UseVisualStyleBackColor = False
		'
		'GroupBox1
		'
		Me.GroupBox1.Controls.Add(Me.rdoInstal3)
		Me.GroupBox1.Controls.Add(Me.rdoInstal2)
		Me.GroupBox1.Controls.Add(Me.rdoInstal1)
		Me.GroupBox1.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle))
		Me.GroupBox1.ForeColor = System.Drawing.Color.DarkBlue
		Me.GroupBox1.Location = New System.Drawing.Point(612, 196)
		Me.GroupBox1.Name = "GroupBox1"
		Me.GroupBox1.Size = New System.Drawing.Size(268, 116)
		Me.GroupBox1.TabIndex = 5
		Me.GroupBox1.TabStop = False
		Me.GroupBox1.Text = "セットアップ選択"
		'
		'rdoInstal3
		'
		Me.rdoInstal3.Appearance = System.Windows.Forms.Appearance.Button
		Me.rdoInstal3.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.rdoInstal3.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.rdoInstal3.ForeColor = System.Drawing.Color.Black
		Me.rdoInstal3.Location = New System.Drawing.Point(16, 84)
		Me.rdoInstal3.Name = "rdoInstal3"
		Me.rdoInstal3.Size = New System.Drawing.Size(236, 24)
		Me.rdoInstal3.TabIndex = 4
		Me.rdoInstal3.Text = "アンインストール"
		Me.rdoInstal3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		Me.rdoInstal3.UseVisualStyleBackColor = False
		'
		'rdoInstal2
		'
		Me.rdoInstal2.Appearance = System.Windows.Forms.Appearance.Button
		Me.rdoInstal2.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.rdoInstal2.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.rdoInstal2.ForeColor = System.Drawing.Color.Black
		Me.rdoInstal2.Location = New System.Drawing.Point(16, 56)
		Me.rdoInstal2.Name = "rdoInstal2"
		Me.rdoInstal2.Size = New System.Drawing.Size(236, 24)
		Me.rdoInstal2.TabIndex = 4
		Me.rdoInstal2.Text = "アップデート"
		Me.rdoInstal2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		Me.rdoInstal2.UseVisualStyleBackColor = False
		'
		'GroupBox3
		'
		Me.GroupBox3.Controls.Add(Me.btnAllUninstall)
		Me.GroupBox3.Controls.Add(Me.btnExecute)
		Me.GroupBox3.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle))
		Me.GroupBox3.ForeColor = System.Drawing.Color.DarkBlue
		Me.GroupBox3.Location = New System.Drawing.Point(612, 320)
		Me.GroupBox3.Name = "GroupBox3"
		Me.GroupBox3.Size = New System.Drawing.Size(268, 104)
		Me.GroupBox3.TabIndex = 6
		Me.GroupBox3.TabStop = False
		Me.GroupBox3.Text = "コマンド実行"
		'
		'btnAllUninstall
		'
		Me.btnAllUninstall.BackColor = System.Drawing.Color.Coral
		Me.btnAllUninstall.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Bold)
		Me.btnAllUninstall.ForeColor = System.Drawing.Color.Black
		Me.btnAllUninstall.Location = New System.Drawing.Point(16, 72)
		Me.btnAllUninstall.Name = "btnAllUninstall"
		Me.btnAllUninstall.Size = New System.Drawing.Size(236, 24)
		Me.btnAllUninstall.TabIndex = 5
		Me.btnAllUninstall.Text = "全システムアンインストール"
		Me.btnAllUninstall.UseVisualStyleBackColor = False
		'
		'btnExecute
		'
		Me.btnExecute.BackColor = System.Drawing.Color.Coral
		Me.btnExecute.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Bold)
		Me.btnExecute.ForeColor = System.Drawing.Color.Black
		Me.btnExecute.Location = New System.Drawing.Point(16, 28)
		Me.btnExecute.Name = "btnExecute"
		Me.btnExecute.Size = New System.Drawing.Size(236, 24)
		Me.btnExecute.TabIndex = 3
		Me.btnExecute.Text = "インストール"
		Me.btnExecute.UseVisualStyleBackColor = False
		'
		'GroupBox4
		'
		Me.GroupBox4.Controls.Add(Me.Label4)
		Me.GroupBox4.Controls.Add(Me.Label3)
		Me.GroupBox4.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.GroupBox4.Location = New System.Drawing.Point(612, 440)
		Me.GroupBox4.Name = "GroupBox4"
		Me.GroupBox4.Size = New System.Drawing.Size(268, 36)
		Me.GroupBox4.TabIndex = 7
		Me.GroupBox4.TabStop = False
		'
		'Label4
		'
		Me.Label4.AutoSize = True
		Me.Label4.Enabled = False
		Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label4.ForeColor = System.Drawing.Color.DimGray
		Me.Label4.Location = New System.Drawing.Point(184, 12)
		Me.Label4.Name = "Label4"
		Me.Label4.Size = New System.Drawing.Size(80, 16)
		Me.Label4.TabIndex = 0
		Me.Label4.Text = "Ver 1.01"
		'
		'Label3
		'
		Me.Label3.AutoSize = True
		Me.Label3.Enabled = False
		Me.Label3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label3.ForeColor = System.Drawing.Color.DimGray
		Me.Label3.Location = New System.Drawing.Point(4, 12)
		Me.Label3.Name = "Label3"
		Me.Label3.Size = New System.Drawing.Size(178, 16)
		Me.Label3.TabIndex = 0
		Me.Label3.Text = "全疵検インストーラー"
		'
		'frmMain
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.BackColor = System.Drawing.Color.PeachPuff
		Me.ClientSize = New System.Drawing.Size(889, 486)
		Me.Controls.Add(Me.GroupBox4)
		Me.Controls.Add(Me.GroupBox3)
		Me.Controls.Add(Me.GroupBox1)
		Me.Controls.Add(Me.lblMsgBox)
		Me.Controls.Add(Me.GroupBox2)
		Me.Controls.Add(Me.grpSet)
		Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D
		Me.MaximizeBox = False
		Me.MinimizeBox = False
		Me.Name = "frmMain"
		Me.ShowIcon = False
		Me.Text = "全疵検インストーラー"
		Me.grpSet.ResumeLayout(False)
		Me.grpSet.PerformLayout()
		Me.GroupBox2.ResumeLayout(False)
		Me.GroupBox2.PerformLayout()
		Me.GroupBox1.ResumeLayout(False)
		Me.GroupBox3.ResumeLayout(False)
		Me.GroupBox4.ResumeLayout(False)
		Me.GroupBox4.PerformLayout()
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents txtSetInstalFolder As System.Windows.Forms.TextBox
	Friend WithEvents grpSet As System.Windows.Forms.GroupBox
	Friend WithEvents btnSelectFolder As System.Windows.Forms.Button
	Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents lblMsgBox As System.Windows.Forms.Label
	Friend WithEvents trvSetInf As System.Windows.Forms.TreeView
	Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
	Friend WithEvents trvNowInf As System.Windows.Forms.TreeView
	Friend WithEvents Button1 As System.Windows.Forms.Button
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents txtNowInstalFolder As System.Windows.Forms.TextBox
	Friend WithEvents rdoInstal1 As System.Windows.Forms.RadioButton
 Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
 Friend WithEvents rdoInstal3 As System.Windows.Forms.RadioButton
 Friend WithEvents rdoInstal2 As System.Windows.Forms.RadioButton
 Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
 Friend WithEvents btnAllUninstall As System.Windows.Forms.Button
 Friend WithEvents btnExecute As System.Windows.Forms.Button
 Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
 Friend WithEvents Label3 As System.Windows.Forms.Label
 Friend WithEvents Label4 As System.Windows.Forms.Label

End Class
