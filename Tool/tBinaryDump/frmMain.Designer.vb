<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
	Inherits System.Windows.Forms.Form

	'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
	<System.Diagnostics.DebuggerNonUserCode()> _
	Protected Overrides Sub Dispose(ByVal disposing As Boolean)
		If disposing AndAlso components IsNot Nothing Then
			components.Dispose()
		End If
		MyBase.Dispose(disposing)
	End Sub

	'Windows フォーム デザイナで必要です。
	Private components As System.ComponentModel.IContainer

	'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
	'Windows フォーム デザイナを使用して変更できます。  
	'コード エディタを使って変更しないでください。
	<System.Diagnostics.DebuggerStepThrough()> _
	Private Sub InitializeComponent()
Me.dgvList = New System.Windows.Forms.DataGridView
Me.Label1 = New System.Windows.Forms.Label
Me.GroupBox1 = New System.Windows.Forms.GroupBox
Me.lblByte_String = New System.Windows.Forms.Label
Me.lblByte_Byte = New System.Windows.Forms.Label
Me.lblByte_Char = New System.Windows.Forms.Label
Me.lblByte_Double = New System.Windows.Forms.Label
Me.lblByte_Single = New System.Windows.Forms.Label
Me.lblByte_Long = New System.Windows.Forms.Label
Me.Label5 = New System.Windows.Forms.Label
Me.Label15 = New System.Windows.Forms.Label
Me.Label12 = New System.Windows.Forms.Label
Me.lblByte_Int = New System.Windows.Forms.Label
Me.Label13 = New System.Windows.Forms.Label
Me.lblByte_Short = New System.Windows.Forms.Label
Me.Label11 = New System.Windows.Forms.Label
Me.Label9 = New System.Windows.Forms.Label
Me.Label7 = New System.Windows.Forms.Label
Me.Label4 = New System.Windows.Forms.Label
Me.Label3 = New System.Windows.Forms.Label
Me.lblByteSize = New System.Windows.Forms.Label
Me.btnDumpWrite = New System.Windows.Forms.Button
Me.txtDumpData = New System.Windows.Forms.TextBox
Me.Label6 = New System.Windows.Forms.Label
Me.Label10 = New System.Windows.Forms.Label
Me.lblByteItiD = New System.Windows.Forms.Label
Me.lblByteItiH = New System.Windows.Forms.Label
Me.Label2 = New System.Windows.Forms.Label
Me.GroupBox2 = New System.Windows.Forms.GroupBox
Me.GroupBox6 = New System.Windows.Forms.GroupBox
Me.txtDbReadAddless = New System.Windows.Forms.TextBox
Me.btnDbRead = New System.Windows.Forms.Button
Me.Label21 = New System.Windows.Forms.Label
Me.Label19 = New System.Windows.Forms.Label
Me.Label20 = New System.Windows.Forms.Label
Me.Label18 = New System.Windows.Forms.Label
Me.Label17 = New System.Windows.Forms.Label
Me.txtDbReadPass = New System.Windows.Forms.TextBox
Me.txtDbReadUser = New System.Windows.Forms.TextBox
Me.txtDbReadDbName = New System.Windows.Forms.TextBox
Me.txtDbReadSql = New System.Windows.Forms.TextBox
Me.GroupBox5 = New System.Windows.Forms.GroupBox
Me.Label16 = New System.Windows.Forms.Label
Me.Label14 = New System.Windows.Forms.Label
Me.txtMemReadTableSize = New System.Windows.Forms.TextBox
Me.txtMemReadTableName = New System.Windows.Forms.TextBox
Me.btnMemRead = New System.Windows.Forms.Button
Me.GroupBox4 = New System.Windows.Forms.GroupBox
Me.txtDumpReadFileSelect = New System.Windows.Forms.TextBox
Me.btnDumpReadFileSelect = New System.Windows.Forms.Button
Me.btnByteRead = New System.Windows.Forms.Button
Me.btnDumpRead = New System.Windows.Forms.Button
Me.GroupBox3 = New System.Windows.Forms.GroupBox
Me.Label8 = New System.Windows.Forms.Label
Me.btnByteWrite = New System.Windows.Forms.Button
Me.GroupBox7 = New System.Windows.Forms.GroupBox
CType(Me.dgvList, System.ComponentModel.ISupportInitialize).BeginInit()
Me.GroupBox1.SuspendLayout()
Me.GroupBox2.SuspendLayout()
Me.GroupBox6.SuspendLayout()
Me.GroupBox5.SuspendLayout()
Me.GroupBox4.SuspendLayout()
Me.GroupBox3.SuspendLayout()
Me.GroupBox7.SuspendLayout()
Me.SuspendLayout()
'
'dgvList
'
Me.dgvList.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Left) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
Me.dgvList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
Me.dgvList.Location = New System.Drawing.Point(4, 4)
Me.dgvList.Name = "dgvList"
Me.dgvList.RowTemplate.Height = 21
Me.dgvList.Size = New System.Drawing.Size(590, 332)
Me.dgvList.TabIndex = 2
'
'Label1
'
Me.Label1.AutoSize = True
Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label1.Location = New System.Drawing.Point(4, 12)
Me.Label1.Name = "Label1"
Me.Label1.Size = New System.Drawing.Size(38, 13)
Me.Label1.TabIndex = 4
Me.Label1.Text = "サイズ"
'
'GroupBox1
'
Me.GroupBox1.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
Me.GroupBox1.Controls.Add(Me.lblByte_String)
Me.GroupBox1.Controls.Add(Me.lblByte_Byte)
Me.GroupBox1.Controls.Add(Me.lblByte_Char)
Me.GroupBox1.Controls.Add(Me.lblByte_Double)
Me.GroupBox1.Controls.Add(Me.lblByte_Single)
Me.GroupBox1.Controls.Add(Me.lblByte_Long)
Me.GroupBox1.Controls.Add(Me.Label5)
Me.GroupBox1.Controls.Add(Me.Label15)
Me.GroupBox1.Controls.Add(Me.Label12)
Me.GroupBox1.Controls.Add(Me.lblByte_Int)
Me.GroupBox1.Controls.Add(Me.Label13)
Me.GroupBox1.Controls.Add(Me.lblByte_Short)
Me.GroupBox1.Controls.Add(Me.Label11)
Me.GroupBox1.Controls.Add(Me.Label9)
Me.GroupBox1.Controls.Add(Me.Label7)
Me.GroupBox1.Controls.Add(Me.Label4)
Me.GroupBox1.Controls.Add(Me.Label3)
Me.GroupBox1.Location = New System.Drawing.Point(4, 416)
Me.GroupBox1.Name = "GroupBox1"
Me.GroupBox1.Size = New System.Drawing.Size(200, 232)
Me.GroupBox1.TabIndex = 5
Me.GroupBox1.TabStop = False
Me.GroupBox1.Text = "型変換"
'
'lblByte_String
'
Me.lblByte_String.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_String.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_String.Location = New System.Drawing.Point(56, 172)
Me.lblByte_String.Name = "lblByte_String"
Me.lblByte_String.Size = New System.Drawing.Size(136, 52)
Me.lblByte_String.TabIndex = 4
'
'lblByte_Byte
'
Me.lblByte_Byte.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_Byte.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_Byte.Location = New System.Drawing.Point(56, 128)
Me.lblByte_Byte.Name = "lblByte_Byte"
Me.lblByte_Byte.Size = New System.Drawing.Size(136, 20)
Me.lblByte_Byte.TabIndex = 4
Me.lblByte_Byte.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'lblByte_Char
'
Me.lblByte_Char.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_Char.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_Char.Location = New System.Drawing.Point(56, 148)
Me.lblByte_Char.Name = "lblByte_Char"
Me.lblByte_Char.Size = New System.Drawing.Size(136, 20)
Me.lblByte_Char.TabIndex = 4
Me.lblByte_Char.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'lblByte_Double
'
Me.lblByte_Double.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_Double.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_Double.Location = New System.Drawing.Point(56, 100)
Me.lblByte_Double.Name = "lblByte_Double"
Me.lblByte_Double.Size = New System.Drawing.Size(136, 20)
Me.lblByte_Double.TabIndex = 4
Me.lblByte_Double.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'lblByte_Single
'
Me.lblByte_Single.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_Single.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_Single.Location = New System.Drawing.Point(56, 80)
Me.lblByte_Single.Name = "lblByte_Single"
Me.lblByte_Single.Size = New System.Drawing.Size(136, 20)
Me.lblByte_Single.TabIndex = 4
Me.lblByte_Single.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'lblByte_Long
'
Me.lblByte_Long.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_Long.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_Long.Location = New System.Drawing.Point(56, 52)
Me.lblByte_Long.Name = "lblByte_Long"
Me.lblByte_Long.Size = New System.Drawing.Size(136, 20)
Me.lblByte_Long.TabIndex = 4
Me.lblByte_Long.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'Label5
'
Me.Label5.AutoSize = True
Me.Label5.Location = New System.Drawing.Point(8, 184)
Me.Label5.Name = "Label5"
Me.Label5.Size = New System.Drawing.Size(47, 12)
Me.Label5.TabIndex = 4
Me.Label5.Text = "(32byte)"
'
'Label15
'
Me.Label15.AutoSize = True
Me.Label15.Location = New System.Drawing.Point(8, 172)
Me.Label15.Name = "Label15"
Me.Label15.Size = New System.Drawing.Size(35, 12)
Me.Label15.TabIndex = 4
Me.Label15.Text = "String"
'
'Label12
'
Me.Label12.AutoSize = True
Me.Label12.Location = New System.Drawing.Point(8, 132)
Me.Label12.Name = "Label12"
Me.Label12.Size = New System.Drawing.Size(29, 12)
Me.Label12.TabIndex = 4
Me.Label12.Text = "Byte"
'
'lblByte_Int
'
Me.lblByte_Int.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_Int.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_Int.Location = New System.Drawing.Point(56, 32)
Me.lblByte_Int.Name = "lblByte_Int"
Me.lblByte_Int.Size = New System.Drawing.Size(136, 20)
Me.lblByte_Int.TabIndex = 4
Me.lblByte_Int.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'Label13
'
Me.Label13.AutoSize = True
Me.Label13.Location = New System.Drawing.Point(8, 152)
Me.Label13.Name = "Label13"
Me.Label13.Size = New System.Drawing.Size(29, 12)
Me.Label13.TabIndex = 4
Me.Label13.Text = "Char"
'
'lblByte_Short
'
Me.lblByte_Short.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByte_Short.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByte_Short.Location = New System.Drawing.Point(56, 12)
Me.lblByte_Short.Name = "lblByte_Short"
Me.lblByte_Short.Size = New System.Drawing.Size(136, 20)
Me.lblByte_Short.TabIndex = 4
Me.lblByte_Short.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'Label11
'
Me.Label11.AutoSize = True
Me.Label11.Location = New System.Drawing.Point(8, 104)
Me.Label11.Name = "Label11"
Me.Label11.Size = New System.Drawing.Size(40, 12)
Me.Label11.TabIndex = 4
Me.Label11.Text = "Double"
'
'Label9
'
Me.Label9.AutoSize = True
Me.Label9.Location = New System.Drawing.Point(8, 84)
Me.Label9.Name = "Label9"
Me.Label9.Size = New System.Drawing.Size(36, 12)
Me.Label9.TabIndex = 4
Me.Label9.Text = "Single"
Me.Label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'Label7
'
Me.Label7.AutoSize = True
Me.Label7.Location = New System.Drawing.Point(8, 56)
Me.Label7.Name = "Label7"
Me.Label7.Size = New System.Drawing.Size(29, 12)
Me.Label7.TabIndex = 4
Me.Label7.Text = "Long"
'
'Label4
'
Me.Label4.AutoSize = True
Me.Label4.Location = New System.Drawing.Point(8, 36)
Me.Label4.Name = "Label4"
Me.Label4.Size = New System.Drawing.Size(18, 12)
Me.Label4.TabIndex = 4
Me.Label4.Text = "Int"
'
'Label3
'
Me.Label3.AutoSize = True
Me.Label3.Location = New System.Drawing.Point(8, 16)
Me.Label3.Name = "Label3"
Me.Label3.Size = New System.Drawing.Size(32, 12)
Me.Label3.TabIndex = 4
Me.Label3.Text = "Short"
'
'lblByteSize
'
Me.lblByteSize.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByteSize.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByteSize.Location = New System.Drawing.Point(64, 12)
Me.lblByteSize.Name = "lblByteSize"
Me.lblByteSize.Size = New System.Drawing.Size(76, 16)
Me.lblByteSize.TabIndex = 4
Me.lblByteSize.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'btnDumpWrite
'
Me.btnDumpWrite.Location = New System.Drawing.Point(168, 32)
Me.btnDumpWrite.Name = "btnDumpWrite"
Me.btnDumpWrite.Size = New System.Drawing.Size(92, 20)
Me.btnDumpWrite.TabIndex = 6
Me.btnDumpWrite.Text = "ダンプ出力[.log]"
Me.btnDumpWrite.UseVisualStyleBackColor = True
'
'txtDumpData
'
Me.txtDumpData.Location = New System.Drawing.Point(48, 32)
Me.txtDumpData.Name = "txtDumpData"
Me.txtDumpData.Size = New System.Drawing.Size(120, 19)
Me.txtDumpData.TabIndex = 7
'
'Label6
'
Me.Label6.AutoSize = True
Me.Label6.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label6.Location = New System.Drawing.Point(140, 12)
Me.Label6.Name = "Label6"
Me.Label6.Size = New System.Drawing.Size(39, 13)
Me.Label6.TabIndex = 4
Me.Label6.Text = "[byte]"
'
'Label10
'
Me.Label10.AutoSize = True
Me.Label10.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label10.Location = New System.Drawing.Point(4, 40)
Me.Label10.Name = "Label10"
Me.Label10.Size = New System.Drawing.Size(59, 13)
Me.Label10.TabIndex = 4
Me.Label10.Text = "選択位置"
'
'lblByteItiD
'
Me.lblByteItiD.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByteItiD.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByteItiD.Location = New System.Drawing.Point(64, 48)
Me.lblByteItiD.Name = "lblByteItiD"
Me.lblByteItiD.Size = New System.Drawing.Size(76, 16)
Me.lblByteItiD.TabIndex = 4
Me.lblByteItiD.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'lblByteItiH
'
Me.lblByteItiH.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
Me.lblByteItiH.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.lblByteItiH.Location = New System.Drawing.Point(64, 32)
Me.lblByteItiH.Name = "lblByteItiH"
Me.lblByteItiH.Size = New System.Drawing.Size(76, 16)
Me.lblByteItiH.TabIndex = 4
Me.lblByteItiH.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
'
'Label2
'
Me.Label2.AutoSize = True
Me.Label2.Location = New System.Drawing.Point(4, 36)
Me.Label2.Name = "Label2"
Me.Label2.Size = New System.Drawing.Size(45, 12)
Me.Label2.TabIndex = 4
Me.Label2.Text = "ﾌｧｲﾙ名"
'
'GroupBox2
'
Me.GroupBox2.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
Me.GroupBox2.Controls.Add(Me.GroupBox6)
Me.GroupBox2.Controls.Add(Me.GroupBox5)
Me.GroupBox2.Controls.Add(Me.GroupBox4)
Me.GroupBox2.Controls.Add(Me.GroupBox3)
Me.GroupBox2.Location = New System.Drawing.Point(212, 340)
Me.GroupBox2.Name = "GroupBox2"
Me.GroupBox2.Size = New System.Drawing.Size(380, 308)
Me.GroupBox2.TabIndex = 8
Me.GroupBox2.TabStop = False
Me.GroupBox2.Text = "コマンド"
'
'GroupBox6
'
Me.GroupBox6.Controls.Add(Me.txtDbReadAddless)
Me.GroupBox6.Controls.Add(Me.btnDbRead)
Me.GroupBox6.Controls.Add(Me.Label21)
Me.GroupBox6.Controls.Add(Me.Label19)
Me.GroupBox6.Controls.Add(Me.Label20)
Me.GroupBox6.Controls.Add(Me.Label18)
Me.GroupBox6.Controls.Add(Me.Label17)
Me.GroupBox6.Controls.Add(Me.txtDbReadPass)
Me.GroupBox6.Controls.Add(Me.txtDbReadUser)
Me.GroupBox6.Controls.Add(Me.txtDbReadDbName)
Me.GroupBox6.Controls.Add(Me.txtDbReadSql)
Me.GroupBox6.Location = New System.Drawing.Point(8, 16)
Me.GroupBox6.Name = "GroupBox6"
Me.GroupBox6.Size = New System.Drawing.Size(364, 128)
Me.GroupBox6.TabIndex = 12
Me.GroupBox6.TabStop = False
Me.GroupBox6.Text = "データベース バイナリデータアクセス （SQLＳｅｒｖｅｒ専用）"
'
'txtDbReadAddless
'
Me.txtDbReadAddless.Location = New System.Drawing.Point(100, 16)
Me.txtDbReadAddless.Name = "txtDbReadAddless"
Me.txtDbReadAddless.Size = New System.Drawing.Size(132, 19)
Me.txtDbReadAddless.TabIndex = 1
'
'btnDbRead
'
Me.btnDbRead.Location = New System.Drawing.Point(4, 16)
Me.btnDbRead.Name = "btnDbRead"
Me.btnDbRead.Size = New System.Drawing.Size(56, 20)
Me.btnDbRead.TabIndex = 0
Me.btnDbRead.Text = "DB読込"
Me.btnDbRead.UseVisualStyleBackColor = True
'
'Label21
'
Me.Label21.AutoSize = True
Me.Label21.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label21.Location = New System.Drawing.Point(300, 20)
Me.Label21.Name = "Label21"
Me.Label21.Size = New System.Drawing.Size(22, 11)
Me.Label21.TabIndex = 4
Me.Label21.Text = "パス"
'
'Label19
'
Me.Label19.AutoSize = True
Me.Label19.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label19.Location = New System.Drawing.Point(220, 44)
Me.Label19.Name = "Label19"
Me.Label19.Size = New System.Drawing.Size(30, 11)
Me.Label19.TabIndex = 4
Me.Label19.Text = "DB名"
'
'Label20
'
Me.Label20.AutoSize = True
Me.Label20.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label20.Location = New System.Drawing.Point(236, 20)
Me.Label20.Name = "Label20"
Me.Label20.Size = New System.Drawing.Size(26, 11)
Me.Label20.TabIndex = 4
Me.Label20.Text = "user"
'
'Label18
'
Me.Label18.AutoSize = True
Me.Label18.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label18.Location = New System.Drawing.Point(64, 20)
Me.Label18.Name = "Label18"
Me.Label18.Size = New System.Drawing.Size(37, 11)
Me.Label18.TabIndex = 4
Me.Label18.Text = "アドレス"
'
'Label17
'
Me.Label17.AutoSize = True
Me.Label17.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label17.Location = New System.Drawing.Point(4, 44)
Me.Label17.Name = "Label17"
Me.Label17.Size = New System.Drawing.Size(184, 11)
Me.Label17.TabIndex = 4
Me.Label17.Text = "SQL文 ↓ (select句はバイナリ列1コだけ)"
'
'txtDbReadPass
'
Me.txtDbReadPass.Location = New System.Drawing.Point(324, 16)
Me.txtDbReadPass.Name = "txtDbReadPass"
Me.txtDbReadPass.Size = New System.Drawing.Size(36, 19)
Me.txtDbReadPass.TabIndex = 1
Me.txtDbReadPass.Text = "nsc"
'
'txtDbReadUser
'
Me.txtDbReadUser.Location = New System.Drawing.Point(264, 16)
Me.txtDbReadUser.Name = "txtDbReadUser"
Me.txtDbReadUser.Size = New System.Drawing.Size(32, 19)
Me.txtDbReadUser.TabIndex = 1
Me.txtDbReadUser.Text = "sa"
'
'txtDbReadDbName
'
Me.txtDbReadDbName.Location = New System.Drawing.Point(252, 40)
Me.txtDbReadDbName.Name = "txtDbReadDbName"
Me.txtDbReadDbName.Size = New System.Drawing.Size(108, 19)
Me.txtDbReadDbName.TabIndex = 1
Me.txtDbReadDbName.Text = "Database_"
'
'txtDbReadSql
'
Me.txtDbReadSql.Location = New System.Drawing.Point(4, 60)
Me.txtDbReadSql.Multiline = True
Me.txtDbReadSql.Name = "txtDbReadSql"
Me.txtDbReadSql.ScrollBars = System.Windows.Forms.ScrollBars.Both
Me.txtDbReadSql.Size = New System.Drawing.Size(356, 60)
Me.txtDbReadSql.TabIndex = 1
'
'GroupBox5
'
Me.GroupBox5.Controls.Add(Me.Label16)
Me.GroupBox5.Controls.Add(Me.Label14)
Me.GroupBox5.Controls.Add(Me.txtMemReadTableSize)
Me.GroupBox5.Controls.Add(Me.txtMemReadTableName)
Me.GroupBox5.Controls.Add(Me.btnMemRead)
Me.GroupBox5.Location = New System.Drawing.Point(8, 148)
Me.GroupBox5.Name = "GroupBox5"
Me.GroupBox5.Size = New System.Drawing.Size(364, 52)
Me.GroupBox5.TabIndex = 11
Me.GroupBox5.TabStop = False
Me.GroupBox5.Text = "共有メモリ参照"
'
'Label16
'
Me.Label16.AutoSize = True
Me.Label16.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label16.Location = New System.Drawing.Point(240, 16)
Me.Label16.Name = "Label16"
Me.Label16.Size = New System.Drawing.Size(109, 11)
Me.Label16.TabIndex = 4
Me.Label16.Text = "共有メモリテーブルサイズ"
'
'Label14
'
Me.Label14.AutoSize = True
Me.Label14.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label14.Location = New System.Drawing.Point(104, 16)
Me.Label14.Name = "Label14"
Me.Label14.Size = New System.Drawing.Size(105, 11)
Me.Label14.TabIndex = 4
Me.Label14.Text = "共有メモリテーブル名称"
'
'txtMemReadTableSize
'
Me.txtMemReadTableSize.Location = New System.Drawing.Point(240, 28)
Me.txtMemReadTableSize.Name = "txtMemReadTableSize"
Me.txtMemReadTableSize.Size = New System.Drawing.Size(104, 19)
Me.txtMemReadTableSize.TabIndex = 1
'
'txtMemReadTableName
'
Me.txtMemReadTableName.Location = New System.Drawing.Point(100, 28)
Me.txtMemReadTableName.Name = "txtMemReadTableName"
Me.txtMemReadTableName.Size = New System.Drawing.Size(132, 19)
Me.txtMemReadTableName.TabIndex = 1
'
'btnMemRead
'
Me.btnMemRead.Location = New System.Drawing.Point(4, 16)
Me.btnMemRead.Name = "btnMemRead"
Me.btnMemRead.Size = New System.Drawing.Size(88, 20)
Me.btnMemRead.TabIndex = 0
Me.btnMemRead.Text = "共有メモリ読込"
Me.btnMemRead.UseVisualStyleBackColor = True
'
'GroupBox4
'
Me.GroupBox4.Controls.Add(Me.txtDumpReadFileSelect)
Me.GroupBox4.Controls.Add(Me.btnDumpReadFileSelect)
Me.GroupBox4.Controls.Add(Me.btnByteRead)
Me.GroupBox4.Controls.Add(Me.btnDumpRead)
Me.GroupBox4.Location = New System.Drawing.Point(8, 204)
Me.GroupBox4.Name = "GroupBox4"
Me.GroupBox4.Size = New System.Drawing.Size(364, 40)
Me.GroupBox4.TabIndex = 10
Me.GroupBox4.TabStop = False
Me.GroupBox4.Text = "ダンプファイル読込"
'
'txtDumpReadFileSelect
'
Me.txtDumpReadFileSelect.Location = New System.Drawing.Point(140, 16)
Me.txtDumpReadFileSelect.Name = "txtDumpReadFileSelect"
Me.txtDumpReadFileSelect.Size = New System.Drawing.Size(188, 19)
Me.txtDumpReadFileSelect.TabIndex = 1
'
'btnDumpReadFileSelect
'
Me.btnDumpReadFileSelect.Location = New System.Drawing.Point(328, 16)
Me.btnDumpReadFileSelect.Name = "btnDumpReadFileSelect"
Me.btnDumpReadFileSelect.Size = New System.Drawing.Size(28, 20)
Me.btnDumpReadFileSelect.TabIndex = 0
Me.btnDumpReadFileSelect.Text = "･･･"
Me.btnDumpReadFileSelect.UseVisualStyleBackColor = True
'
'btnByteRead
'
Me.btnByteRead.Location = New System.Drawing.Point(68, 16)
Me.btnByteRead.Name = "btnByteRead"
Me.btnByteRead.Size = New System.Drawing.Size(72, 20)
Me.btnByteRead.TabIndex = 0
Me.btnByteRead.Text = "ﾊﾞｲﾅﾘ読込"
Me.btnByteRead.UseVisualStyleBackColor = True
'
'btnDumpRead
'
Me.btnDumpRead.Location = New System.Drawing.Point(4, 16)
Me.btnDumpRead.Name = "btnDumpRead"
Me.btnDumpRead.Size = New System.Drawing.Size(64, 20)
Me.btnDumpRead.TabIndex = 0
Me.btnDumpRead.Text = "ダンプ読込"
Me.btnDumpRead.UseVisualStyleBackColor = True
'
'GroupBox3
'
Me.GroupBox3.Controls.Add(Me.Label8)
Me.GroupBox3.Controls.Add(Me.txtDumpData)
Me.GroupBox3.Controls.Add(Me.btnByteWrite)
Me.GroupBox3.Controls.Add(Me.btnDumpWrite)
Me.GroupBox3.Controls.Add(Me.Label2)
Me.GroupBox3.Location = New System.Drawing.Point(8, 248)
Me.GroupBox3.Name = "GroupBox3"
Me.GroupBox3.Size = New System.Drawing.Size(364, 56)
Me.GroupBox3.TabIndex = 9
Me.GroupBox3.TabStop = False
Me.GroupBox3.Text = "ダンプファイル出力"
'
'Label8
'
Me.Label8.AutoSize = True
Me.Label8.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
Me.Label8.Location = New System.Drawing.Point(8, 16)
Me.Label8.Name = "Label8"
Me.Label8.Size = New System.Drawing.Size(293, 11)
Me.Label8.TabIndex = 4
Me.Label8.Text = "(exe直下にDumpフォルダが出来、  その下にファイルが生成されます)"
'
'btnByteWrite
'
Me.btnByteWrite.Location = New System.Drawing.Point(264, 32)
Me.btnByteWrite.Name = "btnByteWrite"
Me.btnByteWrite.Size = New System.Drawing.Size(96, 20)
Me.btnByteWrite.TabIndex = 6
Me.btnByteWrite.Text = "ﾊﾞｲﾅﾘ出力[.bin]"
Me.btnByteWrite.UseVisualStyleBackColor = True
'
'GroupBox7
'
Me.GroupBox7.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
Me.GroupBox7.Controls.Add(Me.Label6)
Me.GroupBox7.Controls.Add(Me.Label1)
Me.GroupBox7.Controls.Add(Me.lblByteSize)
Me.GroupBox7.Controls.Add(Me.lblByteItiD)
Me.GroupBox7.Controls.Add(Me.lblByteItiH)
Me.GroupBox7.Controls.Add(Me.Label10)
Me.GroupBox7.Location = New System.Drawing.Point(4, 340)
Me.GroupBox7.Name = "GroupBox7"
Me.GroupBox7.Size = New System.Drawing.Size(200, 68)
Me.GroupBox7.TabIndex = 9
Me.GroupBox7.TabStop = False
'
'frmMain
'
Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
Me.ClientSize = New System.Drawing.Size(595, 651)
Me.Controls.Add(Me.GroupBox7)
Me.Controls.Add(Me.GroupBox2)
Me.Controls.Add(Me.GroupBox1)
Me.Controls.Add(Me.dgvList)
Me.Name = "frmMain"
Me.Text = "バイナリデータ参照ツール"
CType(Me.dgvList, System.ComponentModel.ISupportInitialize).EndInit()
Me.GroupBox1.ResumeLayout(False)
Me.GroupBox1.PerformLayout()
Me.GroupBox2.ResumeLayout(False)
Me.GroupBox6.ResumeLayout(False)
Me.GroupBox6.PerformLayout()
Me.GroupBox5.ResumeLayout(False)
Me.GroupBox5.PerformLayout()
Me.GroupBox4.ResumeLayout(False)
Me.GroupBox4.PerformLayout()
Me.GroupBox3.ResumeLayout(False)
Me.GroupBox3.PerformLayout()
Me.GroupBox7.ResumeLayout(False)
Me.GroupBox7.PerformLayout()
Me.ResumeLayout(False)

End Sub
 Friend WithEvents dgvList As System.Windows.Forms.DataGridView
 Friend WithEvents Label1 As System.Windows.Forms.Label
 Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
 Friend WithEvents lblByteSize As System.Windows.Forms.Label
 Friend WithEvents lblByte_Int As System.Windows.Forms.Label
 Friend WithEvents lblByte_Short As System.Windows.Forms.Label
 Friend WithEvents Label4 As System.Windows.Forms.Label
 Friend WithEvents Label3 As System.Windows.Forms.Label
 Friend WithEvents lblByte_String As System.Windows.Forms.Label
 Friend WithEvents lblByte_Char As System.Windows.Forms.Label
 Friend WithEvents lblByte_Double As System.Windows.Forms.Label
 Friend WithEvents lblByte_Single As System.Windows.Forms.Label
 Friend WithEvents lblByte_Long As System.Windows.Forms.Label
 Friend WithEvents Label15 As System.Windows.Forms.Label
 Friend WithEvents Label13 As System.Windows.Forms.Label
 Friend WithEvents Label11 As System.Windows.Forms.Label
 Friend WithEvents Label9 As System.Windows.Forms.Label
 Friend WithEvents Label7 As System.Windows.Forms.Label
 Friend WithEvents Label5 As System.Windows.Forms.Label
 Friend WithEvents btnDumpWrite As System.Windows.Forms.Button
 Friend WithEvents txtDumpData As System.Windows.Forms.TextBox
 Friend WithEvents Label6 As System.Windows.Forms.Label
 Friend WithEvents Label10 As System.Windows.Forms.Label
 Friend WithEvents lblByteItiD As System.Windows.Forms.Label
 Friend WithEvents lblByteItiH As System.Windows.Forms.Label
 Friend WithEvents Label2 As System.Windows.Forms.Label
 Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
 Friend WithEvents Label8 As System.Windows.Forms.Label
 Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
 Friend WithEvents lblByte_Byte As System.Windows.Forms.Label
 Friend WithEvents Label12 As System.Windows.Forms.Label
 Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
 Friend WithEvents btnDumpRead As System.Windows.Forms.Button
 Friend WithEvents txtDumpReadFileSelect As System.Windows.Forms.TextBox
 Friend WithEvents btnDumpReadFileSelect As System.Windows.Forms.Button
 Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
 Friend WithEvents btnMemRead As System.Windows.Forms.Button
 Friend WithEvents Label16 As System.Windows.Forms.Label
 Friend WithEvents Label14 As System.Windows.Forms.Label
 Friend WithEvents txtMemReadTableSize As System.Windows.Forms.TextBox
 Friend WithEvents txtMemReadTableName As System.Windows.Forms.TextBox
 Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
 Friend WithEvents btnDbRead As System.Windows.Forms.Button
 Friend WithEvents Label17 As System.Windows.Forms.Label
 Friend WithEvents txtDbReadSql As System.Windows.Forms.TextBox
 Friend WithEvents Label19 As System.Windows.Forms.Label
 Friend WithEvents Label18 As System.Windows.Forms.Label
 Friend WithEvents txtDbReadDbName As System.Windows.Forms.TextBox
 Friend WithEvents txtDbReadAddless As System.Windows.Forms.TextBox
 Friend WithEvents Label21 As System.Windows.Forms.Label
 Friend WithEvents Label20 As System.Windows.Forms.Label
 Friend WithEvents txtDbReadPass As System.Windows.Forms.TextBox
 Friend WithEvents txtDbReadUser As System.Windows.Forms.TextBox
 Friend WithEvents GroupBox7 As System.Windows.Forms.GroupBox
 Friend WithEvents btnByteWrite As System.Windows.Forms.Button
 Friend WithEvents btnByteRead As System.Windows.Forms.Button

End Class
