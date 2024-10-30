<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmMain
    Inherits tClassForm.frmBase

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.GroupBox9 = New System.Windows.Forms.GroupBox()
        Me.lblTo_NowBk = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox11 = New System.Windows.Forms.GroupBox()
        Me.btnTO_Move = New System.Windows.Forms.Button()
        Me.btnTO_Synclo = New System.Windows.Forms.Button()
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.btnTO_ChangeTo = New System.Windows.Forms.Button()
        Me.btnTO_ChangeBk = New System.Windows.Forms.Button()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.lblTO_DbStatusB = New System.Windows.Forms.Label()
        Me.lblTo_B = New System.Windows.Forms.Label()
        Me.lblTo_A = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.lblTO_DbStatusA = New System.Windows.Forms.Label()
        Me.lblMsgBox = New System.Windows.Forms.Label()
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.lblHy_Now02 = New System.Windows.Forms.Label()
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.lblHy_Now01 = New System.Windows.Forms.Label()
        Me.GroupBox7 = New System.Windows.Forms.GroupBox()
        Me.cmbHy_Select = New System.Windows.Forms.ComboBox()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.btnHy_Change = New System.Windows.Forms.Button()
        Me.btnHy_ChangeOff = New System.Windows.Forms.Button()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.GroupBox14 = New System.Windows.Forms.GroupBox()
        Me.GroupBox13 = New System.Windows.Forms.GroupBox()
        Me.cmbRpi_Select = New System.Windows.Forms.ComboBox()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.btnRpi_Change = New System.Windows.Forms.Button()
        Me.btnRpi_ChangeOff = New System.Windows.Forms.Button()
        Me.grpRpi1 = New System.Windows.Forms.GroupBox()
        Me.lblRpi_Now = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.GroupBox15 = New System.Windows.Forms.GroupBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.GroupBox16 = New System.Windows.Forms.GroupBox()
        Me.btnSO_ChangeKa = New System.Windows.Forms.Button()
        Me.btnSO_ChangeSo = New System.Windows.Forms.Button()
        Me.GroupBox17 = New System.Windows.Forms.GroupBox()
        Me.lblSo_NowKa = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox9.SuspendLayout()
        Me.GroupBox11.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox6.SuspendLayout()
        Me.GroupBox7.SuspendLayout()
        Me.GroupBox14.SuspendLayout()
        Me.GroupBox13.SuspendLayout()
        Me.grpRpi1.SuspendLayout()
        Me.GroupBox15.SuspendLayout()
        Me.GroupBox16.SuspendLayout()
        Me.GroupBox17.SuspendLayout()
        Me.SuspendLayout()
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.GroupBox9)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.GroupBox11)
        Me.GroupBox1.Controls.Add(Me.GroupBox4)
        Me.GroupBox1.Controls.Add(Me.GroupBox3)
        Me.GroupBox1.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.ForeColor = System.Drawing.Color.DarkBlue
        Me.GroupBox1.Location = New System.Drawing.Point(8, 44)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(1252, 312)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "統括兼蓄積機能 切替支援"
        '
        'GroupBox9
        '
        Me.GroupBox9.Controls.Add(Me.lblTo_NowBk)
        Me.GroupBox9.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox9.ForeColor = System.Drawing.Color.Black
        Me.GroupBox9.Location = New System.Drawing.Point(8, 24)
        Me.GroupBox9.Name = "GroupBox9"
        Me.GroupBox9.Size = New System.Drawing.Size(251, 68)
        Me.GroupBox9.TabIndex = 2
        Me.GroupBox9.TabStop = False
        Me.GroupBox9.Text = "バックアップ機で実行中の機能"
        '
        'lblTo_NowBk
        '
        Me.lblTo_NowBk.BackColor = System.Drawing.Color.Tomato
        Me.lblTo_NowBk.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTo_NowBk.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTo_NowBk.Location = New System.Drawing.Point(8, 24)
        Me.lblTo_NowBk.Name = "lblTo_NowBk"
        Me.lblTo_NowBk.Size = New System.Drawing.Size(235, 36)
        Me.lblTo_NowBk.TabIndex = 0
        Me.lblTo_NowBk.Text = "取得中"
        Me.lblTo_NowBk.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label1
        '
        Me.Label1.BackColor = System.Drawing.Color.White
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.ForeColor = System.Drawing.Color.Black
        Me.Label1.Location = New System.Drawing.Point(503, 98)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(737, 206)
        Me.Label1.TabIndex = 5
        Me.Label1.Text = resources.GetString("Label1.Text")
        '
        'GroupBox11
        '
        Me.GroupBox11.Controls.Add(Me.btnTO_Move)
        Me.GroupBox11.Controls.Add(Me.btnTO_Synclo)
        Me.GroupBox11.Font = New System.Drawing.Font("MS UI Gothic", 12.0!)
        Me.GroupBox11.ForeColor = System.Drawing.Color.Black
        Me.GroupBox11.Location = New System.Drawing.Point(8, 204)
        Me.GroupBox11.Name = "GroupBox11"
        Me.GroupBox11.Size = New System.Drawing.Size(398, 100)
        Me.GroupBox11.TabIndex = 4
        Me.GroupBox11.TabStop = False
        Me.GroupBox11.Text = "同期機能"
        '
        'btnTO_Move
        '
        Me.btnTO_Move.BackColor = System.Drawing.SystemColors.Control
        Me.btnTO_Move.Location = New System.Drawing.Point(7, 60)
        Me.btnTO_Move.Name = "btnTO_Move"
        Me.btnTO_Move.Size = New System.Drawing.Size(382, 32)
        Me.btnTO_Move.TabIndex = 0
        Me.btnTO_Move.Text = "B系→A系 実績移動"
        Me.btnTO_Move.UseVisualStyleBackColor = False
        '
        'btnTO_Synclo
        '
        Me.btnTO_Synclo.BackColor = System.Drawing.SystemColors.Control
        Me.btnTO_Synclo.Location = New System.Drawing.Point(7, 20)
        Me.btnTO_Synclo.Name = "btnTO_Synclo"
        Me.btnTO_Synclo.Size = New System.Drawing.Size(382, 32)
        Me.btnTO_Synclo.TabIndex = 0
        Me.btnTO_Synclo.Text = "B系→A系 マスター同期"
        Me.btnTO_Synclo.UseVisualStyleBackColor = False
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.btnTO_ChangeTo)
        Me.GroupBox4.Controls.Add(Me.btnTO_ChangeBk)
        Me.GroupBox4.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox4.ForeColor = System.Drawing.Color.Black
        Me.GroupBox4.Location = New System.Drawing.Point(8, 98)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(486, 100)
        Me.GroupBox4.TabIndex = 3
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "切替"
        '
        'btnTO_ChangeTo
        '
        Me.btnTO_ChangeTo.BackColor = System.Drawing.SystemColors.Control
        Me.btnTO_ChangeTo.Location = New System.Drawing.Point(8, 60)
        Me.btnTO_ChangeTo.Name = "btnTO_ChangeTo"
        Me.btnTO_ChangeTo.Size = New System.Drawing.Size(470, 32)
        Me.btnTO_ChangeTo.TabIndex = 0
        Me.btnTO_ChangeTo.Text = "{0}(A系)で統括兼蓄積機能を実行"
        Me.btnTO_ChangeTo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnTO_ChangeTo.UseVisualStyleBackColor = False
        '
        'btnTO_ChangeBk
        '
        Me.btnTO_ChangeBk.BackColor = System.Drawing.SystemColors.Control
        Me.btnTO_ChangeBk.Location = New System.Drawing.Point(8, 20)
        Me.btnTO_ChangeBk.Name = "btnTO_ChangeBk"
        Me.btnTO_ChangeBk.Size = New System.Drawing.Size(470, 32)
        Me.btnTO_ChangeBk.TabIndex = 0
        Me.btnTO_ChangeBk.Text = "{0}(B系)で統括兼蓄積機能を実行"
        Me.btnTO_ChangeBk.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnTO_ChangeBk.UseVisualStyleBackColor = False
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.lblTO_DbStatusB)
        Me.GroupBox3.Controls.Add(Me.lblTo_B)
        Me.GroupBox3.Controls.Add(Me.lblTo_A)
        Me.GroupBox3.Controls.Add(Me.Label3)
        Me.GroupBox3.Controls.Add(Me.Label2)
        Me.GroupBox3.Controls.Add(Me.lblTO_DbStatusA)
        Me.GroupBox3.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox3.ForeColor = System.Drawing.Color.Black
        Me.GroupBox3.Location = New System.Drawing.Point(268, 24)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(467, 68)
        Me.GroupBox3.TabIndex = 2
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "データベース状態"
        '
        'lblTO_DbStatusB
        '
        Me.lblTO_DbStatusB.BackColor = System.Drawing.Color.Tomato
        Me.lblTO_DbStatusB.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTO_DbStatusB.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTO_DbStatusB.Location = New System.Drawing.Point(370, 24)
        Me.lblTO_DbStatusB.Name = "lblTO_DbStatusB"
        Me.lblTO_DbStatusB.Size = New System.Drawing.Size(88, 36)
        Me.lblTO_DbStatusB.TabIndex = 8
        Me.lblTO_DbStatusB.Text = "取得中"
        Me.lblTO_DbStatusB.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblTo_B
        '
        Me.lblTo_B.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTo_B.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTo_B.Location = New System.Drawing.Point(257, 24)
        Me.lblTo_B.Name = "lblTo_B"
        Me.lblTo_B.Size = New System.Drawing.Size(112, 36)
        Me.lblTo_B.TabIndex = 7
        Me.lblTo_B.Text = "B"
        Me.lblTo_B.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblTo_A
        '
        Me.lblTo_A.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTo_A.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTo_A.Location = New System.Drawing.Point(28, 24)
        Me.lblTo_A.Name = "lblTo_A"
        Me.lblTo_A.Size = New System.Drawing.Size(112, 36)
        Me.lblTo_A.TabIndex = 6
        Me.lblTo_A.Text = "A"
        Me.lblTo_A.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(235, 24)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(24, 32)
        Me.Label3.TabIndex = 5
        Me.Label3.Text = "B" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "系"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(6, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(24, 32)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "A" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "系"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblTO_DbStatusA
        '
        Me.lblTO_DbStatusA.BackColor = System.Drawing.Color.Tomato
        Me.lblTO_DbStatusA.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTO_DbStatusA.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTO_DbStatusA.Location = New System.Drawing.Point(141, 24)
        Me.lblTO_DbStatusA.Name = "lblTO_DbStatusA"
        Me.lblTO_DbStatusA.Size = New System.Drawing.Size(88, 36)
        Me.lblTO_DbStatusA.TabIndex = 0
        Me.lblTO_DbStatusA.Text = "取得中"
        Me.lblTO_DbStatusA.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblMsgBox
        '
        Me.lblMsgBox.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(255, Byte), Integer), CType(CType(192, Byte), Integer))
        Me.lblMsgBox.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMsgBox.ForeColor = System.Drawing.Color.Red
        Me.lblMsgBox.Location = New System.Drawing.Point(8, 4)
        Me.lblMsgBox.Name = "lblMsgBox"
        Me.lblMsgBox.Size = New System.Drawing.Size(624, 32)
        Me.lblMsgBox.TabIndex = 1
        Me.lblMsgBox.Text = "DBの状態を取得中です。" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "しばらくお待ちください。"
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.GroupBox2)
        Me.GroupBox5.Controls.Add(Me.GroupBox6)
        Me.GroupBox5.Controls.Add(Me.GroupBox7)
        Me.GroupBox5.Controls.Add(Me.Label8)
        Me.GroupBox5.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox5.ForeColor = System.Drawing.Color.DarkBlue
        Me.GroupBox5.Location = New System.Drawing.Point(8, 362)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(1252, 170)
        Me.GroupBox5.TabIndex = 3
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "表示機能 切替支援"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.lblHy_Now02)
        Me.GroupBox2.Font = New System.Drawing.Font("MS UI Gothic", 11.0!)
        Me.GroupBox2.ForeColor = System.Drawing.Color.Black
        Me.GroupBox2.Location = New System.Drawing.Point(9, 96)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(260, 68)
        Me.GroupBox2.TabIndex = 2
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "表示オフラインPC02で実行中の機能"
        Me.GroupBox2.Visible = False
        '
        'lblHy_Now02
        '
        Me.lblHy_Now02.BackColor = System.Drawing.Color.Tomato
        Me.lblHy_Now02.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblHy_Now02.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblHy_Now02.ForeColor = System.Drawing.Color.Black
        Me.lblHy_Now02.Location = New System.Drawing.Point(8, 24)
        Me.lblHy_Now02.Name = "lblHy_Now02"
        Me.lblHy_Now02.Size = New System.Drawing.Size(244, 36)
        Me.lblHy_Now02.TabIndex = 0
        Me.lblHy_Now02.Text = "取得中"
        Me.lblHy_Now02.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.lblHy_Now01)
        Me.GroupBox6.Font = New System.Drawing.Font("MS UI Gothic", 11.0!)
        Me.GroupBox6.ForeColor = System.Drawing.Color.Black
        Me.GroupBox6.Location = New System.Drawing.Point(8, 24)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(260, 68)
        Me.GroupBox6.TabIndex = 2
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "表示オフラインPC01で実行中の機能"
        '
        'lblHy_Now01
        '
        Me.lblHy_Now01.BackColor = System.Drawing.Color.Tomato
        Me.lblHy_Now01.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblHy_Now01.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblHy_Now01.ForeColor = System.Drawing.Color.Black
        Me.lblHy_Now01.Location = New System.Drawing.Point(8, 24)
        Me.lblHy_Now01.Name = "lblHy_Now01"
        Me.lblHy_Now01.Size = New System.Drawing.Size(244, 36)
        Me.lblHy_Now01.TabIndex = 0
        Me.lblHy_Now01.Text = "取得中"
        Me.lblHy_Now01.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'GroupBox7
        '
        Me.GroupBox7.Controls.Add(Me.cmbHy_Select)
        Me.GroupBox7.Controls.Add(Me.Label5)
        Me.GroupBox7.Controls.Add(Me.btnHy_Change)
        Me.GroupBox7.Controls.Add(Me.btnHy_ChangeOff)
        Me.GroupBox7.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox7.ForeColor = System.Drawing.Color.Black
        Me.GroupBox7.Location = New System.Drawing.Point(280, 24)
        Me.GroupBox7.Name = "GroupBox7"
        Me.GroupBox7.Size = New System.Drawing.Size(443, 132)
        Me.GroupBox7.TabIndex = 1
        Me.GroupBox7.TabStop = False
        Me.GroupBox7.Text = "切替"
        '
        'cmbHy_Select
        '
        Me.cmbHy_Select.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbHy_Select.FormattingEnabled = True
        Me.cmbHy_Select.Location = New System.Drawing.Point(72, 64)
        Me.cmbHy_Select.Name = "cmbHy_Select"
        Me.cmbHy_Select.Size = New System.Drawing.Size(363, 24)
        Me.cmbHy_Select.TabIndex = 1
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(4, 68)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(72, 16)
        Me.Label5.TabIndex = 4
        Me.Label5.Text = "切替機能"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'btnHy_Change
        '
        Me.btnHy_Change.BackColor = System.Drawing.SystemColors.Control
        Me.btnHy_Change.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnHy_Change.Location = New System.Drawing.Point(72, 92)
        Me.btnHy_Change.Name = "btnHy_Change"
        Me.btnHy_Change.Size = New System.Drawing.Size(363, 32)
        Me.btnHy_Change.TabIndex = 0
        Me.btnHy_Change.Text = "{0}で上記選択機能を実行"
        Me.btnHy_Change.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnHy_Change.UseVisualStyleBackColor = False
        '
        'btnHy_ChangeOff
        '
        Me.btnHy_ChangeOff.BackColor = System.Drawing.SystemColors.Control
        Me.btnHy_ChangeOff.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnHy_ChangeOff.Location = New System.Drawing.Point(8, 16)
        Me.btnHy_ChangeOff.Name = "btnHy_ChangeOff"
        Me.btnHy_ChangeOff.Size = New System.Drawing.Size(427, 32)
        Me.btnHy_ChangeOff.TabIndex = 0
        Me.btnHy_ChangeOff.Text = "{0}で{1}を実行(従来に戻る)"
        Me.btnHy_ChangeOff.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnHy_ChangeOff.UseVisualStyleBackColor = False
        '
        'Label8
        '
        Me.Label8.BackColor = System.Drawing.Color.White
        Me.Label8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label8.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.ForeColor = System.Drawing.Color.Black
        Me.Label8.Location = New System.Drawing.Point(729, 20)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(511, 132)
        Me.Label8.TabIndex = 0
        Me.Label8.Text = resources.GetString("Label8.Text")
        '
        'GroupBox14
        '
        Me.GroupBox14.Controls.Add(Me.GroupBox13)
        Me.GroupBox14.Controls.Add(Me.grpRpi1)
        Me.GroupBox14.Controls.Add(Me.Label9)
        Me.GroupBox14.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox14.ForeColor = System.Drawing.Color.DarkBlue
        Me.GroupBox14.Location = New System.Drawing.Point(8, 700)
        Me.GroupBox14.Name = "GroupBox14"
        Me.GroupBox14.Size = New System.Drawing.Size(1250, 177)
        Me.GroupBox14.TabIndex = 5
        Me.GroupBox14.TabStop = False
        Me.GroupBox14.Text = "全長画像処理機能 切替支援"
        '
        'GroupBox13
        '
        Me.GroupBox13.Controls.Add(Me.cmbRpi_Select)
        Me.GroupBox13.Controls.Add(Me.Label7)
        Me.GroupBox13.Controls.Add(Me.btnRpi_Change)
        Me.GroupBox13.Controls.Add(Me.btnRpi_ChangeOff)
        Me.GroupBox13.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox13.ForeColor = System.Drawing.Color.Black
        Me.GroupBox13.Location = New System.Drawing.Point(280, 24)
        Me.GroupBox13.Name = "GroupBox13"
        Me.GroupBox13.Size = New System.Drawing.Size(443, 132)
        Me.GroupBox13.TabIndex = 3
        Me.GroupBox13.TabStop = False
        Me.GroupBox13.Text = "切替"
        '
        'cmbRpi_Select
        '
        Me.cmbRpi_Select.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbRpi_Select.FormattingEnabled = True
        Me.cmbRpi_Select.Location = New System.Drawing.Point(72, 64)
        Me.cmbRpi_Select.Name = "cmbRpi_Select"
        Me.cmbRpi_Select.Size = New System.Drawing.Size(363, 24)
        Me.cmbRpi_Select.TabIndex = 1
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(4, 68)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(72, 16)
        Me.Label7.TabIndex = 4
        Me.Label7.Text = "切替機能"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'btnRpi_Change
        '
        Me.btnRpi_Change.BackColor = System.Drawing.SystemColors.Control
        Me.btnRpi_Change.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRpi_Change.Location = New System.Drawing.Point(72, 92)
        Me.btnRpi_Change.Name = "btnRpi_Change"
        Me.btnRpi_Change.Size = New System.Drawing.Size(363, 32)
        Me.btnRpi_Change.TabIndex = 0
        Me.btnRpi_Change.Text = "{0}で上記選択機能を実行"
        Me.btnRpi_Change.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnRpi_Change.UseVisualStyleBackColor = False
        '
        'btnRpi_ChangeOff
        '
        Me.btnRpi_ChangeOff.BackColor = System.Drawing.SystemColors.Control
        Me.btnRpi_ChangeOff.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRpi_ChangeOff.Location = New System.Drawing.Point(8, 16)
        Me.btnRpi_ChangeOff.Name = "btnRpi_ChangeOff"
        Me.btnRpi_ChangeOff.Size = New System.Drawing.Size(427, 32)
        Me.btnRpi_ChangeOff.TabIndex = 0
        Me.btnRpi_ChangeOff.Text = "{0}で{1}を実行(従来に戻る)"
        Me.btnRpi_ChangeOff.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnRpi_ChangeOff.UseVisualStyleBackColor = False
        '
        'grpRpi1
        '
        Me.grpRpi1.Controls.Add(Me.lblRpi_Now)
        Me.grpRpi1.Font = New System.Drawing.Font("MS UI Gothic", 11.0!)
        Me.grpRpi1.ForeColor = System.Drawing.Color.Black
        Me.grpRpi1.Location = New System.Drawing.Point(8, 24)
        Me.grpRpi1.Name = "grpRpi1"
        Me.grpRpi1.Size = New System.Drawing.Size(260, 69)
        Me.grpRpi1.TabIndex = 2
        Me.grpRpi1.TabStop = False
        Me.grpRpi1.Text = "全長処理BKPCで実行中の機能"
        '
        'lblRpi_Now
        '
        Me.lblRpi_Now.BackColor = System.Drawing.Color.Tomato
        Me.lblRpi_Now.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblRpi_Now.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblRpi_Now.ForeColor = System.Drawing.Color.Black
        Me.lblRpi_Now.Location = New System.Drawing.Point(8, 24)
        Me.lblRpi_Now.Name = "lblRpi_Now"
        Me.lblRpi_Now.Size = New System.Drawing.Size(244, 36)
        Me.lblRpi_Now.TabIndex = 0
        Me.lblRpi_Now.Text = "取得中"
        Me.lblRpi_Now.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label9
        '
        Me.Label9.BackColor = System.Drawing.Color.White
        Me.Label9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label9.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.ForeColor = System.Drawing.Color.Black
        Me.Label9.Location = New System.Drawing.Point(729, 20)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(511, 136)
        Me.Label9.TabIndex = 0
        Me.Label9.Text = resources.GetString("Label9.Text")
        '
        'GroupBox15
        '
        Me.GroupBox15.Controls.Add(Me.Label6)
        Me.GroupBox15.Controls.Add(Me.GroupBox16)
        Me.GroupBox15.Controls.Add(Me.GroupBox17)
        Me.GroupBox15.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle))
        Me.GroupBox15.ForeColor = System.Drawing.Color.DarkBlue
        Me.GroupBox15.Location = New System.Drawing.Point(9, 536)
        Me.GroupBox15.Name = "GroupBox15"
        Me.GroupBox15.Size = New System.Drawing.Size(1252, 156)
        Me.GroupBox15.TabIndex = 6
        Me.GroupBox15.TabStop = False
        Me.GroupBox15.Text = "操作機能 切替支援"
        '
        'Label6
        '
        Me.Label6.BackColor = System.Drawing.Color.White
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.ForeColor = System.Drawing.Color.Black
        Me.Label6.Location = New System.Drawing.Point(729, 20)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(511, 128)
        Me.Label6.TabIndex = 3
        Me.Label6.Text = resources.GetString("Label6.Text")
        '
        'GroupBox16
        '
        Me.GroupBox16.Controls.Add(Me.btnSO_ChangeKa)
        Me.GroupBox16.Controls.Add(Me.btnSO_ChangeSo)
        Me.GroupBox16.Font = New System.Drawing.Font("MS UI Gothic", 12.0!)
        Me.GroupBox16.ForeColor = System.Drawing.Color.Black
        Me.GroupBox16.Location = New System.Drawing.Point(280, 24)
        Me.GroupBox16.Name = "GroupBox16"
        Me.GroupBox16.Size = New System.Drawing.Size(443, 100)
        Me.GroupBox16.TabIndex = 3
        Me.GroupBox16.TabStop = False
        Me.GroupBox16.Text = "切替"
        '
        'btnSO_ChangeKa
        '
        Me.btnSO_ChangeKa.BackColor = System.Drawing.SystemColors.Control
        Me.btnSO_ChangeKa.Location = New System.Drawing.Point(8, 20)
        Me.btnSO_ChangeKa.Name = "btnSO_ChangeKa"
        Me.btnSO_ChangeKa.Size = New System.Drawing.Size(427, 32)
        Me.btnSO_ChangeKa.TabIndex = 2
        Me.btnSO_ChangeKa.Text = "{0}(B系)で操作機能を実行"
        Me.btnSO_ChangeKa.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnSO_ChangeKa.UseVisualStyleBackColor = False
        '
        'btnSO_ChangeSo
        '
        Me.btnSO_ChangeSo.BackColor = System.Drawing.SystemColors.Control
        Me.btnSO_ChangeSo.Location = New System.Drawing.Point(8, 60)
        Me.btnSO_ChangeSo.Name = "btnSO_ChangeSo"
        Me.btnSO_ChangeSo.Size = New System.Drawing.Size(427, 32)
        Me.btnSO_ChangeSo.TabIndex = 1
        Me.btnSO_ChangeSo.Text = "{0}(A系)で操作機能を実行"
        Me.btnSO_ChangeSo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        Me.btnSO_ChangeSo.UseVisualStyleBackColor = False
        '
        'GroupBox17
        '
        Me.GroupBox17.Controls.Add(Me.lblSo_NowKa)
        Me.GroupBox17.Font = New System.Drawing.Font("MS UI Gothic", 11.0!)
        Me.GroupBox17.ForeColor = System.Drawing.Color.Black
        Me.GroupBox17.Location = New System.Drawing.Point(8, 24)
        Me.GroupBox17.Name = "GroupBox17"
        Me.GroupBox17.Size = New System.Drawing.Size(260, 68)
        Me.GroupBox17.TabIndex = 2
        Me.GroupBox17.TabStop = False
        Me.GroupBox17.Text = "解析PCで実行中の機能"
        '
        'lblSo_NowKa
        '
        Me.lblSo_NowKa.BackColor = System.Drawing.Color.Tomato
        Me.lblSo_NowKa.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSo_NowKa.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSo_NowKa.ForeColor = System.Drawing.Color.Black
        Me.lblSo_NowKa.Location = New System.Drawing.Point(8, 24)
        Me.lblSo_NowKa.Name = "lblSo_NowKa"
        Me.lblSo_NowKa.Size = New System.Drawing.Size(244, 36)
        Me.lblSo_NowKa.TabIndex = 0
        Me.lblSo_NowKa.Text = "取得中"
        Me.lblSo_NowKa.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.Bisque
        Me.ClientSize = New System.Drawing.Size(1264, 884)
        Me.Controls.Add(Me.GroupBox15)
        Me.Controls.Add(Me.GroupBox14)
        Me.Controls.Add(Me.GroupBox5)
        Me.Controls.Add(Me.lblMsgBox)
        Me.Controls.Add(Me.GroupBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmMain"
        Me.ShowIcon = False
        Me.Text = "パソコン バックアップ切替画面"
        Me.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iNON
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox9.ResumeLayout(False)
        Me.GroupBox11.ResumeLayout(False)
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox6.ResumeLayout(False)
        Me.GroupBox7.ResumeLayout(False)
        Me.GroupBox7.PerformLayout()
        Me.GroupBox14.ResumeLayout(False)
        Me.GroupBox13.ResumeLayout(False)
        Me.GroupBox13.PerformLayout()
        Me.grpRpi1.ResumeLayout(False)
        Me.GroupBox15.ResumeLayout(False)
        Me.GroupBox16.ResumeLayout(False)
        Me.GroupBox17.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents lblMsgBox As System.Windows.Forms.Label
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents lblTO_DbStatusA As System.Windows.Forms.Label
    Friend WithEvents lblTo_A As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents lblTo_B As System.Windows.Forms.Label
    Friend WithEvents lblTO_DbStatusB As System.Windows.Forms.Label
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents btnTO_ChangeTo As System.Windows.Forms.Button
    Friend WithEvents btnTO_ChangeBk As System.Windows.Forms.Button
    Friend WithEvents GroupBox11 As System.Windows.Forms.GroupBox
    Friend WithEvents btnTO_Move As System.Windows.Forms.Button
    Friend WithEvents btnTO_Synclo As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents lblHy_Now01 As System.Windows.Forms.Label
    Friend WithEvents GroupBox7 As System.Windows.Forms.GroupBox
    Friend WithEvents cmbHy_Select As System.Windows.Forms.ComboBox
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents btnHy_Change As System.Windows.Forms.Button
    Friend WithEvents btnHy_ChangeOff As System.Windows.Forms.Button
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents GroupBox14 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox13 As System.Windows.Forms.GroupBox
    Friend WithEvents cmbRpi_Select As System.Windows.Forms.ComboBox
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents btnRpi_Change As System.Windows.Forms.Button
    Friend WithEvents btnRpi_ChangeOff As System.Windows.Forms.Button
    Friend WithEvents grpRpi1 As System.Windows.Forms.GroupBox
    Friend WithEvents lblRpi_Now As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents GroupBox9 As System.Windows.Forms.GroupBox
    Friend WithEvents lblTo_NowBk As System.Windows.Forms.Label
    Friend WithEvents GroupBox15 As System.Windows.Forms.GroupBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents GroupBox16 As System.Windows.Forms.GroupBox
    Friend WithEvents btnSO_ChangeKa As System.Windows.Forms.Button
    Friend WithEvents btnSO_ChangeSo As System.Windows.Forms.Button
    Friend WithEvents GroupBox17 As System.Windows.Forms.GroupBox
    Friend WithEvents lblSo_NowKa As System.Windows.Forms.Label
    Friend WithEvents GroupBox2 As GroupBox
    Friend WithEvents lblHy_Now02 As Label
End Class
