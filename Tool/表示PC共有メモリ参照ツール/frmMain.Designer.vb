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
		Me.btnCoilResult = New System.Windows.Forms.Button()
		Me.btnKizuInf = New System.Windows.Forms.Button()
		Me.btnCoilInf = New System.Windows.Forms.Button()
		Me.bntMapRow = New System.Windows.Forms.Button()
		Me.butPointInf = New System.Windows.Forms.Button()
		Me.btnStatusInf = New System.Windows.Forms.Button()
		Me.Label1 = New System.Windows.Forms.Label()
		Me.btnCycleInf = New System.Windows.Forms.Button()
		Me.btnFile = New System.Windows.Forms.Button()
		Me.btnDefMap = New System.Windows.Forms.Button()
		Me.btnHyDefList = New System.Windows.Forms.Button()
		Me.btnAlarm = New System.Windows.Forms.Button()
		Me.btnMaeInf = New System.Windows.Forms.Button()
		Me.btnLongPd = New System.Windows.Forms.Button()
		Me.SuspendLayout()
		'
		'btnCoilResult
		'
		Me.btnCoilResult.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnCoilResult.Location = New System.Drawing.Point(336, 132)
		Me.btnCoilResult.Name = "btnCoilResult"
		Me.btnCoilResult.Size = New System.Drawing.Size(152, 52)
		Me.btnCoilResult.TabIndex = 20
		Me.btnCoilResult.Text = "コイル実績"
		Me.btnCoilResult.UseVisualStyleBackColor = True
		'
		'btnKizuInf
		'
		Me.btnKizuInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnKizuInf.Location = New System.Drawing.Point(176, 132)
		Me.btnKizuInf.Name = "btnKizuInf"
		Me.btnKizuInf.Size = New System.Drawing.Size(152, 52)
		Me.btnKizuInf.TabIndex = 19
		Me.btnKizuInf.Text = "疵情報"
		Me.btnKizuInf.UseVisualStyleBackColor = True
		'
		'btnCoilInf
		'
		Me.btnCoilInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnCoilInf.Location = New System.Drawing.Point(336, 72)
		Me.btnCoilInf.Name = "btnCoilInf"
		Me.btnCoilInf.Size = New System.Drawing.Size(152, 52)
		Me.btnCoilInf.TabIndex = 17
		Me.btnCoilInf.Text = "コイル情報"
		Me.btnCoilInf.UseVisualStyleBackColor = True
		'
		'bntMapRow
		'
		Me.bntMapRow.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.bntMapRow.Location = New System.Drawing.Point(16, 132)
		Me.bntMapRow.Name = "bntMapRow"
		Me.bntMapRow.Size = New System.Drawing.Size(152, 52)
		Me.bntMapRow.TabIndex = 18
		Me.bntMapRow.Text = "マップ位置"
		Me.bntMapRow.UseVisualStyleBackColor = True
		'
		'butPointInf
		'
		Me.butPointInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.butPointInf.Location = New System.Drawing.Point(176, 72)
		Me.butPointInf.Name = "butPointInf"
		Me.butPointInf.Size = New System.Drawing.Size(152, 52)
		Me.butPointInf.TabIndex = 16
		Me.butPointInf.Text = "ポインタ情報"
		Me.butPointInf.UseVisualStyleBackColor = True
		'
		'btnStatusInf
		'
		Me.btnStatusInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnStatusInf.Location = New System.Drawing.Point(16, 72)
		Me.btnStatusInf.Name = "btnStatusInf"
		Me.btnStatusInf.Size = New System.Drawing.Size(152, 52)
		Me.btnStatusInf.TabIndex = 15
		Me.btnStatusInf.Text = "ステータス情報"
		Me.btnStatusInf.UseVisualStyleBackColor = True
		'
		'Label1
		'
		Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 24.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label1.Location = New System.Drawing.Point(16, 8)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(412, 40)
		Me.Label1.TabIndex = 14
		Me.Label1.Text = "表示PC共有メモリ参照ツール"
		'
		'btnCycleInf
		'
		Me.btnCycleInf.Enabled = False
		Me.btnCycleInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnCycleInf.Location = New System.Drawing.Point(16, 256)
		Me.btnCycleInf.Name = "btnCycleInf"
		Me.btnCycleInf.Size = New System.Drawing.Size(152, 52)
		Me.btnCycleInf.TabIndex = 24
		Me.btnCycleInf.UseVisualStyleBackColor = True
		'
		'btnFile
		'
		Me.btnFile.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnFile.Location = New System.Drawing.Point(336, 256)
		Me.btnFile.Name = "btnFile"
		Me.btnFile.Size = New System.Drawing.Size(152, 52)
		Me.btnFile.TabIndex = 26
		Me.btnFile.Text = "ファイル操作"
		Me.btnFile.UseVisualStyleBackColor = True
		'
		'btnDefMap
		'
		Me.btnDefMap.Enabled = False
		Me.btnDefMap.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnDefMap.Location = New System.Drawing.Point(176, 192)
		Me.btnDefMap.Name = "btnDefMap"
		Me.btnDefMap.Size = New System.Drawing.Size(152, 52)
		Me.btnDefMap.TabIndex = 22
		Me.btnDefMap.UseVisualStyleBackColor = True
		'
		'btnHyDefList
		'
		Me.btnHyDefList.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnHyDefList.Location = New System.Drawing.Point(16, 192)
		Me.btnHyDefList.Name = "btnHyDefList"
		Me.btnHyDefList.Size = New System.Drawing.Size(152, 52)
		Me.btnHyDefList.TabIndex = 21
		Me.btnHyDefList.Text = "重欠陥一覧表示" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ポインタ情報"
		Me.btnHyDefList.UseVisualStyleBackColor = True
		'
		'btnAlarm
		'
		Me.btnAlarm.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnAlarm.Location = New System.Drawing.Point(176, 256)
		Me.btnAlarm.Name = "btnAlarm"
		Me.btnAlarm.Size = New System.Drawing.Size(152, 52)
		Me.btnAlarm.TabIndex = 25
		Me.btnAlarm.Text = "警報情報"
		Me.btnAlarm.UseVisualStyleBackColor = True
		'
		'btnMaeInf
		'
		Me.btnMaeInf.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnMaeInf.Location = New System.Drawing.Point(336, 192)
		Me.btnMaeInf.Name = "btnMaeInf"
		Me.btnMaeInf.Size = New System.Drawing.Size(152, 52)
		Me.btnMaeInf.TabIndex = 23
		Me.btnMaeInf.Text = "前工程疵情報"
		Me.btnMaeInf.UseVisualStyleBackColor = True
		'
		'btnLongPd
		'
		Me.btnLongPd.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnLongPd.Location = New System.Drawing.Point(16, 316)
		Me.btnLongPd.Name = "btnLongPd"
		Me.btnLongPd.Size = New System.Drawing.Size(152, 52)
		Me.btnLongPd.TabIndex = 24
		Me.btnLongPd.Text = "長周期情報"
		Me.btnLongPd.UseVisualStyleBackColor = True
		'
		'frmMain
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.ClientSize = New System.Drawing.Size(498, 389)
		Me.Controls.Add(Me.btnMaeInf)
		Me.Controls.Add(Me.btnAlarm)
		Me.Controls.Add(Me.btnHyDefList)
		Me.Controls.Add(Me.btnDefMap)
		Me.Controls.Add(Me.btnFile)
		Me.Controls.Add(Me.btnLongPd)
		Me.Controls.Add(Me.btnCycleInf)
		Me.Controls.Add(Me.btnCoilResult)
		Me.Controls.Add(Me.btnKizuInf)
		Me.Controls.Add(Me.btnCoilInf)
		Me.Controls.Add(Me.bntMapRow)
		Me.Controls.Add(Me.butPointInf)
		Me.Controls.Add(Me.btnStatusInf)
		Me.Controls.Add(Me.Label1)
		Me.Name = "frmMain"
		Me.Text = "表示PC共有メモリ参照ツール"
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents btnCoilResult As System.Windows.Forms.Button
	Friend WithEvents btnKizuInf As System.Windows.Forms.Button
	Friend WithEvents btnCoilInf As System.Windows.Forms.Button
    Friend WithEvents bntMapRow As System.Windows.Forms.Button
    Friend WithEvents butPointInf As System.Windows.Forms.Button
    Friend WithEvents btnStatusInf As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents btnCycleInf As System.Windows.Forms.Button
    Friend WithEvents btnFile As System.Windows.Forms.Button
    Friend WithEvents btnDefMap As System.Windows.Forms.Button
    Friend WithEvents btnHyDefList As Button
    Friend WithEvents btnAlarm As Button
    Friend WithEvents btnMaeInf As Button
	Friend WithEvents btnLongPd As Button
End Class
