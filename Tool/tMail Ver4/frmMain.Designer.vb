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
        Me.components = New System.ComponentModel.Container
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.btnMailOpen = New System.Windows.Forms.Button
        Me.txtSendPC = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.txtSendTask = New System.Windows.Forms.TextBox
        Me.Label4 = New System.Windows.Forms.Label
        Me.txtRecvTask = New System.Windows.Forms.TextBox
        Me.Label5 = New System.Windows.Forms.Label
        Me.btnMailClose = New System.Windows.Forms.Button
        Me.gpbRecv = New System.Windows.Forms.GroupBox
        Me.Label20 = New System.Windows.Forms.Label
        Me.Label32 = New System.Windows.Forms.Label
        Me.Label31 = New System.Windows.Forms.Label
        Me.Label21 = New System.Windows.Forms.Label
        Me.Label22 = New System.Windows.Forms.Label
        Me.Label23 = New System.Windows.Forms.Label
        Me.Label24 = New System.Windows.Forms.Label
        Me.Label25 = New System.Windows.Forms.Label
        Me.Label26 = New System.Windows.Forms.Label
        Me.Label27 = New System.Windows.Forms.Label
        Me.Label28 = New System.Windows.Forms.Label
        Me.Label29 = New System.Windows.Forms.Label
        Me.Label30 = New System.Windows.Forms.Label
        Me.Label35 = New System.Windows.Forms.Label
        Me.Label7 = New System.Windows.Forms.Label
        Me.txtRecvLineNo = New System.Windows.Forms.TextBox
        Me.txtRecv_MIX = New System.Windows.Forms.TextBox
        Me.txtRecv_Str = New System.Windows.Forms.TextBox
        Me.txtRecv_INT9 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT8 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT7 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT6 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT5 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT4 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT3 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT2 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT1 = New System.Windows.Forms.TextBox
        Me.txtRecv_INT0 = New System.Windows.Forms.TextBox
        Me.txtRecvEvtNo = New System.Windows.Forms.TextBox
        Me.txtSendEvtNo = New System.Windows.Forms.TextBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.Label8 = New System.Windows.Forms.Label
        Me.txtSend_INT0 = New System.Windows.Forms.TextBox
        Me.Label9 = New System.Windows.Forms.Label
        Me.txtSend_INT1 = New System.Windows.Forms.TextBox
        Me.Label10 = New System.Windows.Forms.Label
        Me.txtSend_INT2 = New System.Windows.Forms.TextBox
        Me.Label11 = New System.Windows.Forms.Label
        Me.txtSend_INT3 = New System.Windows.Forms.TextBox
        Me.Label12 = New System.Windows.Forms.Label
        Me.txtSend_INT4 = New System.Windows.Forms.TextBox
        Me.Label13 = New System.Windows.Forms.Label
        Me.txtSend_INT5 = New System.Windows.Forms.TextBox
        Me.Label14 = New System.Windows.Forms.Label
        Me.txtSend_INT6 = New System.Windows.Forms.TextBox
        Me.Label15 = New System.Windows.Forms.Label
        Me.txtSend_INT7 = New System.Windows.Forms.TextBox
        Me.Label16 = New System.Windows.Forms.Label
        Me.txtSend_INT8 = New System.Windows.Forms.TextBox
        Me.Label17 = New System.Windows.Forms.Label
        Me.txtSend_INT9 = New System.Windows.Forms.TextBox
        Me.Label18 = New System.Windows.Forms.Label
        Me.Label19 = New System.Windows.Forms.Label
        Me.txtSend_STR = New System.Windows.Forms.TextBox
        Me.btnSend_Int = New System.Windows.Forms.Button
        Me.btnSend_Str = New System.Windows.Forms.Button
        Me.btnSend_Mix = New System.Windows.Forms.Button
        Me.lstMsg = New System.Windows.Forms.ListBox
        Me.tmrRecving = New System.Windows.Forms.Timer(Me.components)
        Me.txtSendLineNo = New System.Windows.Forms.TextBox
        Me.Label33 = New System.Windows.Forms.Label
        Me.txtSend_STR2 = New System.Windows.Forms.TextBox
        Me.txtSend_STR3 = New System.Windows.Forms.TextBox
        Me.txtSend_STR4 = New System.Windows.Forms.TextBox
        Me.txtSend_STR5 = New System.Windows.Forms.TextBox
        Me.chkTrans = New System.Windows.Forms.CheckBox
        Me.txtUdpAddr = New System.Windows.Forms.TextBox
        Me.gpbRecv.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnMailOpen
        '
        Me.btnMailOpen.Location = New System.Drawing.Point(120, 64)
        Me.btnMailOpen.Name = "btnMailOpen"
        Me.btnMailOpen.Size = New System.Drawing.Size(44, 20)
        Me.btnMailOpen.TabIndex = 0
        Me.btnMailOpen.TabStop = False
        Me.btnMailOpen.Text = "生成"
        Me.btnMailOpen.UseVisualStyleBackColor = True
        '
        'txtSendPC
        '
        Me.txtSendPC.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSendPC.Location = New System.Drawing.Point(4, 40)
        Me.txtSendPC.Name = "txtSendPC"
        Me.txtSendPC.Size = New System.Drawing.Size(96, 19)
        Me.txtSendPC.TabIndex = 1
        Me.txtSendPC.Text = "."
        Me.txtSendPC.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'Label1
        '
        Me.Label1.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.Label1.Location = New System.Drawing.Point(4, 0)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(96, 20)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "メール送信"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.Label2.Location = New System.Drawing.Point(120, 0)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(96, 20)
        Me.Label2.TabIndex = 2
        Me.Label2.Text = "メール受信"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(4, 28)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(68, 12)
        Me.Label3.TabIndex = 3
        Me.Label3.Text = "送信先PC名"
        '
        'txtSendTask
        '
        Me.txtSendTask.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSendTask.Location = New System.Drawing.Point(4, 76)
        Me.txtSendTask.Name = "txtSendTask"
        Me.txtSendTask.Size = New System.Drawing.Size(96, 19)
        Me.txtSendTask.TabIndex = 2
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(4, 64)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(78, 12)
        Me.Label4.TabIndex = 3
        Me.Label4.Text = "送信先タスク名"
        '
        'txtRecvTask
        '
        Me.txtRecvTask.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecvTask.Location = New System.Drawing.Point(120, 40)
        Me.txtRecvTask.Name = "txtRecvTask"
        Me.txtRecvTask.Size = New System.Drawing.Size(96, 19)
        Me.txtRecvTask.TabIndex = 50
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(120, 28)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(54, 12)
        Me.Label5.TabIndex = 3
        Me.Label5.Text = "自タスク名"
        '
        'btnMailClose
        '
        Me.btnMailClose.Location = New System.Drawing.Point(168, 64)
        Me.btnMailClose.Name = "btnMailClose"
        Me.btnMailClose.Size = New System.Drawing.Size(44, 20)
        Me.btnMailClose.TabIndex = 0
        Me.btnMailClose.TabStop = False
        Me.btnMailClose.Text = "開放"
        Me.btnMailClose.UseVisualStyleBackColor = True
        '
        'gpbRecv
        '
        Me.gpbRecv.Controls.Add(Me.Label20)
        Me.gpbRecv.Controls.Add(Me.Label32)
        Me.gpbRecv.Controls.Add(Me.Label31)
        Me.gpbRecv.Controls.Add(Me.Label21)
        Me.gpbRecv.Controls.Add(Me.Label22)
        Me.gpbRecv.Controls.Add(Me.Label23)
        Me.gpbRecv.Controls.Add(Me.Label24)
        Me.gpbRecv.Controls.Add(Me.Label25)
        Me.gpbRecv.Controls.Add(Me.Label26)
        Me.gpbRecv.Controls.Add(Me.Label27)
        Me.gpbRecv.Controls.Add(Me.Label28)
        Me.gpbRecv.Controls.Add(Me.Label29)
        Me.gpbRecv.Controls.Add(Me.Label30)
        Me.gpbRecv.Controls.Add(Me.Label35)
        Me.gpbRecv.Controls.Add(Me.Label7)
        Me.gpbRecv.Controls.Add(Me.txtRecvLineNo)
        Me.gpbRecv.Controls.Add(Me.txtRecv_MIX)
        Me.gpbRecv.Controls.Add(Me.txtRecv_Str)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT9)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT8)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT7)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT6)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT5)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT4)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT3)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT2)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT1)
        Me.gpbRecv.Controls.Add(Me.txtRecv_INT0)
        Me.gpbRecv.Controls.Add(Me.txtRecvEvtNo)
        Me.gpbRecv.Location = New System.Drawing.Point(104, 88)
        Me.gpbRecv.Name = "gpbRecv"
        Me.gpbRecv.Size = New System.Drawing.Size(188, 264)
        Me.gpbRecv.TabIndex = 4
        Me.gpbRecv.TabStop = False
        Me.gpbRecv.Text = "受信データ"
        '
        'Label20
        '
        Me.Label20.AutoSize = True
        Me.Label20.Location = New System.Drawing.Point(8, 40)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(65, 12)
        Me.Label20.TabIndex = 23
        Me.Label20.Text = "[数値データ]"
        '
        'Label32
        '
        Me.Label32.AutoSize = True
        Me.Label32.Location = New System.Drawing.Point(12, 208)
        Me.Label32.Name = "Label32"
        Me.Label32.Size = New System.Drawing.Size(65, 12)
        Me.Label32.TabIndex = 9
        Me.Label32.Text = "[混合データ]"
        '
        'Label31
        '
        Me.Label31.AutoSize = True
        Me.Label31.Location = New System.Drawing.Point(12, 156)
        Me.Label31.Name = "Label31"
        Me.Label31.Size = New System.Drawing.Size(65, 12)
        Me.Label31.TabIndex = 9
        Me.Label31.Text = "[文字データ]"
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Location = New System.Drawing.Point(96, 136)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(19, 12)
        Me.Label21.TabIndex = 24
        Me.Label21.Text = "[9]"
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Location = New System.Drawing.Point(96, 116)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(19, 12)
        Me.Label22.TabIndex = 22
        Me.Label22.Text = "[8]"
        '
        'Label23
        '
        Me.Label23.AutoSize = True
        Me.Label23.Location = New System.Drawing.Point(96, 96)
        Me.Label23.Name = "Label23"
        Me.Label23.Size = New System.Drawing.Size(19, 12)
        Me.Label23.TabIndex = 20
        Me.Label23.Text = "[7]"
        '
        'Label24
        '
        Me.Label24.AutoSize = True
        Me.Label24.Location = New System.Drawing.Point(96, 76)
        Me.Label24.Name = "Label24"
        Me.Label24.Size = New System.Drawing.Size(19, 12)
        Me.Label24.TabIndex = 21
        Me.Label24.Text = "[6]"
        '
        'Label25
        '
        Me.Label25.AutoSize = True
        Me.Label25.Location = New System.Drawing.Point(96, 56)
        Me.Label25.Name = "Label25"
        Me.Label25.Size = New System.Drawing.Size(19, 12)
        Me.Label25.TabIndex = 25
        Me.Label25.Text = "[5]"
        '
        'Label26
        '
        Me.Label26.AutoSize = True
        Me.Label26.Location = New System.Drawing.Point(8, 136)
        Me.Label26.Name = "Label26"
        Me.Label26.Size = New System.Drawing.Size(19, 12)
        Me.Label26.TabIndex = 29
        Me.Label26.Text = "[4]"
        '
        'Label27
        '
        Me.Label27.AutoSize = True
        Me.Label27.Location = New System.Drawing.Point(8, 116)
        Me.Label27.Name = "Label27"
        Me.Label27.Size = New System.Drawing.Size(19, 12)
        Me.Label27.TabIndex = 30
        Me.Label27.Text = "[3]"
        '
        'Label28
        '
        Me.Label28.AutoSize = True
        Me.Label28.Location = New System.Drawing.Point(8, 96)
        Me.Label28.Name = "Label28"
        Me.Label28.Size = New System.Drawing.Size(19, 12)
        Me.Label28.TabIndex = 28
        Me.Label28.Text = "[2]"
        '
        'Label29
        '
        Me.Label29.AutoSize = True
        Me.Label29.Location = New System.Drawing.Point(8, 76)
        Me.Label29.Name = "Label29"
        Me.Label29.Size = New System.Drawing.Size(19, 12)
        Me.Label29.TabIndex = 26
        Me.Label29.Text = "[1]"
        '
        'Label30
        '
        Me.Label30.AutoSize = True
        Me.Label30.Location = New System.Drawing.Point(8, 56)
        Me.Label30.Name = "Label30"
        Me.Label30.Size = New System.Drawing.Size(19, 12)
        Me.Label30.TabIndex = 27
        Me.Label30.Text = "[0]"
        '
        'Label35
        '
        Me.Label35.AutoSize = True
        Me.Label35.Location = New System.Drawing.Point(100, 20)
        Me.Label35.Name = "Label35"
        Me.Label35.Size = New System.Drawing.Size(40, 12)
        Me.Label35.TabIndex = 6
        Me.Label35.Text = "LineNo"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(8, 20)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(36, 12)
        Me.Label7.TabIndex = 6
        Me.Label7.Text = "EvtNo"
        '
        'txtRecvLineNo
        '
        Me.txtRecvLineNo.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecvLineNo.Location = New System.Drawing.Point(140, 16)
        Me.txtRecvLineNo.Name = "txtRecvLineNo"
        Me.txtRecvLineNo.ReadOnly = True
        Me.txtRecvLineNo.Size = New System.Drawing.Size(44, 19)
        Me.txtRecvLineNo.TabIndex = 1
        Me.txtRecvLineNo.TabStop = False
        Me.txtRecvLineNo.Text = "0"
        '
        'txtRecv_MIX
        '
        Me.txtRecv_MIX.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_MIX.Location = New System.Drawing.Point(12, 220)
        Me.txtRecv_MIX.Multiline = True
        Me.txtRecv_MIX.Name = "txtRecv_MIX"
        Me.txtRecv_MIX.ReadOnly = True
        Me.txtRecv_MIX.Size = New System.Drawing.Size(172, 36)
        Me.txtRecv_MIX.TabIndex = 1
        Me.txtRecv_MIX.TabStop = False
        '
        'txtRecv_Str
        '
        Me.txtRecv_Str.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_Str.Location = New System.Drawing.Point(12, 168)
        Me.txtRecv_Str.Multiline = True
        Me.txtRecv_Str.Name = "txtRecv_Str"
        Me.txtRecv_Str.ReadOnly = True
        Me.txtRecv_Str.Size = New System.Drawing.Size(172, 36)
        Me.txtRecv_Str.TabIndex = 1
        Me.txtRecv_Str.TabStop = False
        '
        'txtRecv_INT9
        '
        Me.txtRecv_INT9.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT9.Location = New System.Drawing.Point(116, 132)
        Me.txtRecv_INT9.Name = "txtRecv_INT9"
        Me.txtRecv_INT9.ReadOnly = True
        Me.txtRecv_INT9.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT9.TabIndex = 1
        Me.txtRecv_INT9.TabStop = False
        Me.txtRecv_INT9.Text = "0"
        '
        'txtRecv_INT8
        '
        Me.txtRecv_INT8.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT8.Location = New System.Drawing.Point(116, 112)
        Me.txtRecv_INT8.Name = "txtRecv_INT8"
        Me.txtRecv_INT8.ReadOnly = True
        Me.txtRecv_INT8.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT8.TabIndex = 1
        Me.txtRecv_INT8.TabStop = False
        Me.txtRecv_INT8.Text = "0"
        '
        'txtRecv_INT7
        '
        Me.txtRecv_INT7.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT7.Location = New System.Drawing.Point(116, 92)
        Me.txtRecv_INT7.Name = "txtRecv_INT7"
        Me.txtRecv_INT7.ReadOnly = True
        Me.txtRecv_INT7.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT7.TabIndex = 1
        Me.txtRecv_INT7.TabStop = False
        Me.txtRecv_INT7.Text = "0"
        '
        'txtRecv_INT6
        '
        Me.txtRecv_INT6.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT6.Location = New System.Drawing.Point(116, 72)
        Me.txtRecv_INT6.Name = "txtRecv_INT6"
        Me.txtRecv_INT6.ReadOnly = True
        Me.txtRecv_INT6.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT6.TabIndex = 1
        Me.txtRecv_INT6.TabStop = False
        Me.txtRecv_INT6.Text = "0"
        '
        'txtRecv_INT5
        '
        Me.txtRecv_INT5.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT5.Location = New System.Drawing.Point(116, 52)
        Me.txtRecv_INT5.Name = "txtRecv_INT5"
        Me.txtRecv_INT5.ReadOnly = True
        Me.txtRecv_INT5.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT5.TabIndex = 1
        Me.txtRecv_INT5.TabStop = False
        Me.txtRecv_INT5.Text = "0"
        '
        'txtRecv_INT4
        '
        Me.txtRecv_INT4.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT4.Location = New System.Drawing.Point(28, 132)
        Me.txtRecv_INT4.Name = "txtRecv_INT4"
        Me.txtRecv_INT4.ReadOnly = True
        Me.txtRecv_INT4.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT4.TabIndex = 1
        Me.txtRecv_INT4.TabStop = False
        Me.txtRecv_INT4.Text = "0"
        '
        'txtRecv_INT3
        '
        Me.txtRecv_INT3.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT3.Location = New System.Drawing.Point(28, 112)
        Me.txtRecv_INT3.Name = "txtRecv_INT3"
        Me.txtRecv_INT3.ReadOnly = True
        Me.txtRecv_INT3.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT3.TabIndex = 1
        Me.txtRecv_INT3.TabStop = False
        Me.txtRecv_INT3.Text = "0"
        '
        'txtRecv_INT2
        '
        Me.txtRecv_INT2.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT2.Location = New System.Drawing.Point(28, 92)
        Me.txtRecv_INT2.Name = "txtRecv_INT2"
        Me.txtRecv_INT2.ReadOnly = True
        Me.txtRecv_INT2.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT2.TabIndex = 1
        Me.txtRecv_INT2.TabStop = False
        Me.txtRecv_INT2.Text = "0"
        '
        'txtRecv_INT1
        '
        Me.txtRecv_INT1.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT1.Location = New System.Drawing.Point(28, 72)
        Me.txtRecv_INT1.Name = "txtRecv_INT1"
        Me.txtRecv_INT1.ReadOnly = True
        Me.txtRecv_INT1.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT1.TabIndex = 1
        Me.txtRecv_INT1.TabStop = False
        Me.txtRecv_INT1.Text = "0"
        '
        'txtRecv_INT0
        '
        Me.txtRecv_INT0.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecv_INT0.Location = New System.Drawing.Point(28, 52)
        Me.txtRecv_INT0.Name = "txtRecv_INT0"
        Me.txtRecv_INT0.ReadOnly = True
        Me.txtRecv_INT0.Size = New System.Drawing.Size(68, 19)
        Me.txtRecv_INT0.TabIndex = 1
        Me.txtRecv_INT0.TabStop = False
        Me.txtRecv_INT0.Text = "0"
        '
        'txtRecvEvtNo
        '
        Me.txtRecvEvtNo.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtRecvEvtNo.Location = New System.Drawing.Point(44, 16)
        Me.txtRecvEvtNo.Name = "txtRecvEvtNo"
        Me.txtRecvEvtNo.ReadOnly = True
        Me.txtRecvEvtNo.Size = New System.Drawing.Size(52, 19)
        Me.txtRecvEvtNo.TabIndex = 1
        Me.txtRecvEvtNo.TabStop = False
        Me.txtRecvEvtNo.Text = "0"
        '
        'txtSendEvtNo
        '
        Me.txtSendEvtNo.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSendEvtNo.Location = New System.Drawing.Point(4, 112)
        Me.txtSendEvtNo.Name = "txtSendEvtNo"
        Me.txtSendEvtNo.Size = New System.Drawing.Size(56, 19)
        Me.txtSendEvtNo.TabIndex = 3
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(4, 100)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(49, 12)
        Me.Label6.TabIndex = 3
        Me.Label6.Text = "ｲﾍﾞﾝﾄNo"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(4, 156)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(19, 12)
        Me.Label8.TabIndex = 9
        Me.Label8.Text = "[0]"
        '
        'txtSend_INT0
        '
        Me.txtSend_INT0.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT0.Location = New System.Drawing.Point(24, 152)
        Me.txtSend_INT0.Name = "txtSend_INT0"
        Me.txtSend_INT0.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT0.TabIndex = 5
        Me.txtSend_INT0.Text = "0"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Location = New System.Drawing.Point(4, 140)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(65, 12)
        Me.Label9.TabIndex = 9
        Me.Label9.Text = "[数値データ]"
        '
        'txtSend_INT1
        '
        Me.txtSend_INT1.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT1.Location = New System.Drawing.Point(24, 172)
        Me.txtSend_INT1.Name = "txtSend_INT1"
        Me.txtSend_INT1.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT1.TabIndex = 6
        Me.txtSend_INT1.Text = "0"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Location = New System.Drawing.Point(4, 176)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(19, 12)
        Me.Label10.TabIndex = 9
        Me.Label10.Text = "[1]"
        '
        'txtSend_INT2
        '
        Me.txtSend_INT2.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT2.Location = New System.Drawing.Point(24, 192)
        Me.txtSend_INT2.Name = "txtSend_INT2"
        Me.txtSend_INT2.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT2.TabIndex = 7
        Me.txtSend_INT2.Text = "0"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(4, 196)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(19, 12)
        Me.Label11.TabIndex = 9
        Me.Label11.Text = "[2]"
        '
        'txtSend_INT3
        '
        Me.txtSend_INT3.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT3.Location = New System.Drawing.Point(24, 212)
        Me.txtSend_INT3.Name = "txtSend_INT3"
        Me.txtSend_INT3.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT3.TabIndex = 8
        Me.txtSend_INT3.Text = "0"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(4, 216)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(19, 12)
        Me.Label12.TabIndex = 9
        Me.Label12.Text = "[3]"
        '
        'txtSend_INT4
        '
        Me.txtSend_INT4.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT4.Location = New System.Drawing.Point(24, 232)
        Me.txtSend_INT4.Name = "txtSend_INT4"
        Me.txtSend_INT4.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT4.TabIndex = 9
        Me.txtSend_INT4.Text = "0"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Location = New System.Drawing.Point(4, 236)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(19, 12)
        Me.Label13.TabIndex = 9
        Me.Label13.Text = "[4]"
        '
        'txtSend_INT5
        '
        Me.txtSend_INT5.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT5.Location = New System.Drawing.Point(24, 252)
        Me.txtSend_INT5.Name = "txtSend_INT5"
        Me.txtSend_INT5.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT5.TabIndex = 10
        Me.txtSend_INT5.Text = "0"
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Location = New System.Drawing.Point(4, 256)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(19, 12)
        Me.Label14.TabIndex = 9
        Me.Label14.Text = "[5]"
        '
        'txtSend_INT6
        '
        Me.txtSend_INT6.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT6.Location = New System.Drawing.Point(24, 272)
        Me.txtSend_INT6.Name = "txtSend_INT6"
        Me.txtSend_INT6.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT6.TabIndex = 11
        Me.txtSend_INT6.Text = "0"
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Location = New System.Drawing.Point(4, 276)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(19, 12)
        Me.Label15.TabIndex = 9
        Me.Label15.Text = "[6]"
        '
        'txtSend_INT7
        '
        Me.txtSend_INT7.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT7.Location = New System.Drawing.Point(24, 292)
        Me.txtSend_INT7.Name = "txtSend_INT7"
        Me.txtSend_INT7.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT7.TabIndex = 12
        Me.txtSend_INT7.Text = "0"
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(4, 296)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(19, 12)
        Me.Label16.TabIndex = 9
        Me.Label16.Text = "[7]"
        '
        'txtSend_INT8
        '
        Me.txtSend_INT8.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT8.Location = New System.Drawing.Point(24, 312)
        Me.txtSend_INT8.Name = "txtSend_INT8"
        Me.txtSend_INT8.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT8.TabIndex = 13
        Me.txtSend_INT8.Text = "0"
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Location = New System.Drawing.Point(4, 316)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(19, 12)
        Me.Label17.TabIndex = 9
        Me.Label17.Text = "[8]"
        '
        'txtSend_INT9
        '
        Me.txtSend_INT9.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_INT9.Location = New System.Drawing.Point(24, 332)
        Me.txtSend_INT9.Name = "txtSend_INT9"
        Me.txtSend_INT9.Size = New System.Drawing.Size(76, 19)
        Me.txtSend_INT9.TabIndex = 14
        Me.txtSend_INT9.Text = "0"
        '
        'Label18
        '
        Me.Label18.AutoSize = True
        Me.Label18.Location = New System.Drawing.Point(4, 336)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(19, 12)
        Me.Label18.TabIndex = 9
        Me.Label18.Text = "[9]"
        '
        'Label19
        '
        Me.Label19.AutoSize = True
        Me.Label19.Location = New System.Drawing.Point(4, 360)
        Me.Label19.Name = "Label19"
        Me.Label19.Size = New System.Drawing.Size(65, 12)
        Me.Label19.TabIndex = 9
        Me.Label19.Text = "[文字データ]"
        '
        'txtSend_STR
        '
        Me.txtSend_STR.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_STR.Location = New System.Drawing.Point(4, 376)
        Me.txtSend_STR.Name = "txtSend_STR"
        Me.txtSend_STR.Size = New System.Drawing.Size(288, 19)
        Me.txtSend_STR.TabIndex = 15
        '
        'btnSend_Int
        '
        Me.btnSend_Int.Location = New System.Drawing.Point(4, 508)
        Me.btnSend_Int.Name = "btnSend_Int"
        Me.btnSend_Int.Size = New System.Drawing.Size(80, 20)
        Me.btnSend_Int.TabIndex = 0
        Me.btnSend_Int.TabStop = False
        Me.btnSend_Int.Tag = "0"
        Me.btnSend_Int.Text = "数値送信"
        Me.btnSend_Int.UseVisualStyleBackColor = True
        '
        'btnSend_Str
        '
        Me.btnSend_Str.Location = New System.Drawing.Point(92, 508)
        Me.btnSend_Str.Name = "btnSend_Str"
        Me.btnSend_Str.Size = New System.Drawing.Size(80, 20)
        Me.btnSend_Str.TabIndex = 0
        Me.btnSend_Str.TabStop = False
        Me.btnSend_Str.Tag = "1"
        Me.btnSend_Str.Text = "文字送信"
        Me.btnSend_Str.UseVisualStyleBackColor = True
        '
        'btnSend_Mix
        '
        Me.btnSend_Mix.Location = New System.Drawing.Point(180, 508)
        Me.btnSend_Mix.Name = "btnSend_Mix"
        Me.btnSend_Mix.Size = New System.Drawing.Size(80, 20)
        Me.btnSend_Mix.TabIndex = 0
        Me.btnSend_Mix.TabStop = False
        Me.btnSend_Mix.Tag = "2"
        Me.btnSend_Mix.Text = "混合送信"
        Me.btnSend_Mix.UseVisualStyleBackColor = True
        '
        'lstMsg
        '
        Me.lstMsg.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lstMsg.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lstMsg.FormattingEnabled = True
        Me.lstMsg.ItemHeight = 11
        Me.lstMsg.Location = New System.Drawing.Point(4, 536)
        Me.lstMsg.Name = "lstMsg"
        Me.lstMsg.Size = New System.Drawing.Size(288, 158)
        Me.lstMsg.TabIndex = 10
        Me.lstMsg.TabStop = False
        '
        'tmrRecving
        '
        Me.tmrRecving.Interval = 500
        '
        'txtSendLineNo
        '
        Me.txtSendLineNo.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSendLineNo.Location = New System.Drawing.Point(60, 112)
        Me.txtSendLineNo.Name = "txtSendLineNo"
        Me.txtSendLineNo.Size = New System.Drawing.Size(40, 19)
        Me.txtSendLineNo.TabIndex = 4
        Me.txtSendLineNo.Text = "0"
        '
        'Label33
        '
        Me.Label33.AutoSize = True
        Me.Label33.Location = New System.Drawing.Point(60, 100)
        Me.Label33.Name = "Label33"
        Me.Label33.Size = New System.Drawing.Size(40, 12)
        Me.Label33.TabIndex = 3
        Me.Label33.Text = "ﾗｲﾝNo"
        '
        'txtSend_STR2
        '
        Me.txtSend_STR2.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_STR2.Location = New System.Drawing.Point(4, 396)
        Me.txtSend_STR2.Name = "txtSend_STR2"
        Me.txtSend_STR2.Size = New System.Drawing.Size(288, 19)
        Me.txtSend_STR2.TabIndex = 16
        '
        'txtSend_STR3
        '
        Me.txtSend_STR3.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_STR3.Location = New System.Drawing.Point(4, 416)
        Me.txtSend_STR3.Name = "txtSend_STR3"
        Me.txtSend_STR3.Size = New System.Drawing.Size(288, 19)
        Me.txtSend_STR3.TabIndex = 17
        '
        'txtSend_STR4
        '
        Me.txtSend_STR4.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_STR4.Location = New System.Drawing.Point(4, 436)
        Me.txtSend_STR4.Name = "txtSend_STR4"
        Me.txtSend_STR4.Size = New System.Drawing.Size(288, 19)
        Me.txtSend_STR4.TabIndex = 18
        '
        'txtSend_STR5
        '
        Me.txtSend_STR5.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSend_STR5.Location = New System.Drawing.Point(4, 456)
        Me.txtSend_STR5.Name = "txtSend_STR5"
        Me.txtSend_STR5.Size = New System.Drawing.Size(288, 19)
        Me.txtSend_STR5.TabIndex = 19
        '
        'chkTrans
        '
        Me.chkTrans.AutoSize = True
        Me.chkTrans.Location = New System.Drawing.Point(8, 484)
        Me.chkTrans.Name = "chkTrans"
        Me.chkTrans.Size = New System.Drawing.Size(76, 16)
        Me.chkTrans.TabIndex = 51
        Me.chkTrans.Text = "メール転送"
        Me.chkTrans.UseVisualStyleBackColor = True
        '
        'txtUdpAddr
        '
        Me.txtUdpAddr.Enabled = False
        Me.txtUdpAddr.Location = New System.Drawing.Point(88, 484)
        Me.txtUdpAddr.Name = "txtUdpAddr"
        Me.txtUdpAddr.Size = New System.Drawing.Size(200, 19)
        Me.txtUdpAddr.TabIndex = 52
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(294, 707)
        Me.Controls.Add(Me.txtUdpAddr)
        Me.Controls.Add(Me.chkTrans)
        Me.Controls.Add(Me.lstMsg)
        Me.Controls.Add(Me.Label19)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.Label18)
        Me.Controls.Add(Me.Label17)
        Me.Controls.Add(Me.Label16)
        Me.Controls.Add(Me.Label15)
        Me.Controls.Add(Me.Label14)
        Me.Controls.Add(Me.Label13)
        Me.Controls.Add(Me.Label12)
        Me.Controls.Add(Me.Label11)
        Me.Controls.Add(Me.Label10)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.txtSend_INT9)
        Me.Controls.Add(Me.txtSend_INT8)
        Me.Controls.Add(Me.txtSend_INT7)
        Me.Controls.Add(Me.txtSend_INT6)
        Me.Controls.Add(Me.txtSend_INT5)
        Me.Controls.Add(Me.txtSend_INT4)
        Me.Controls.Add(Me.txtSend_INT3)
        Me.Controls.Add(Me.txtSend_INT2)
        Me.Controls.Add(Me.txtSend_INT1)
        Me.Controls.Add(Me.txtSend_INT0)
        Me.Controls.Add(Me.gpbRecv)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.Label33)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.txtRecvTask)
        Me.Controls.Add(Me.txtSend_STR5)
        Me.Controls.Add(Me.txtSend_STR4)
        Me.Controls.Add(Me.txtSend_STR3)
        Me.Controls.Add(Me.txtSend_STR2)
        Me.Controls.Add(Me.txtSend_STR)
        Me.Controls.Add(Me.txtSendLineNo)
        Me.Controls.Add(Me.txtSendEvtNo)
        Me.Controls.Add(Me.txtSendTask)
        Me.Controls.Add(Me.txtSendPC)
        Me.Controls.Add(Me.btnMailClose)
        Me.Controls.Add(Me.btnSend_Mix)
        Me.Controls.Add(Me.btnSend_Str)
        Me.Controls.Add(Me.btnSend_Int)
        Me.Controls.Add(Me.btnMailOpen)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.Name = "frmMain"
        Me.Text = "メール送受信ツール"
        Me.gpbRecv.ResumeLayout(False)
        Me.gpbRecv.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
 Friend WithEvents btnMailOpen As System.Windows.Forms.Button
 Friend WithEvents txtSendPC As System.Windows.Forms.TextBox
 Friend WithEvents Label1 As System.Windows.Forms.Label
 Friend WithEvents Label2 As System.Windows.Forms.Label
 Friend WithEvents Label3 As System.Windows.Forms.Label
 Friend WithEvents txtSendTask As System.Windows.Forms.TextBox
 Friend WithEvents Label4 As System.Windows.Forms.Label
 Friend WithEvents txtRecvTask As System.Windows.Forms.TextBox
 Friend WithEvents Label5 As System.Windows.Forms.Label
 Friend WithEvents btnMailClose As System.Windows.Forms.Button
 Friend WithEvents gpbRecv As System.Windows.Forms.GroupBox
 Friend WithEvents Label7 As System.Windows.Forms.Label
 Friend WithEvents txtSendEvtNo As System.Windows.Forms.TextBox
 Friend WithEvents Label6 As System.Windows.Forms.Label
 Friend WithEvents Label8 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT0 As System.Windows.Forms.TextBox
 Friend WithEvents Label9 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT1 As System.Windows.Forms.TextBox
 Friend WithEvents Label10 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT2 As System.Windows.Forms.TextBox
 Friend WithEvents Label11 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT3 As System.Windows.Forms.TextBox
 Friend WithEvents Label12 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT4 As System.Windows.Forms.TextBox
 Friend WithEvents Label13 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT5 As System.Windows.Forms.TextBox
 Friend WithEvents Label14 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT6 As System.Windows.Forms.TextBox
 Friend WithEvents Label15 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT7 As System.Windows.Forms.TextBox
 Friend WithEvents Label16 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT8 As System.Windows.Forms.TextBox
 Friend WithEvents Label17 As System.Windows.Forms.Label
 Friend WithEvents txtSend_INT9 As System.Windows.Forms.TextBox
 Friend WithEvents Label18 As System.Windows.Forms.Label
 Friend WithEvents Label19 As System.Windows.Forms.Label
 Friend WithEvents txtSend_STR As System.Windows.Forms.TextBox
 Friend WithEvents btnSend_Int As System.Windows.Forms.Button
 Friend WithEvents btnSend_Str As System.Windows.Forms.Button
 Friend WithEvents btnSend_Mix As System.Windows.Forms.Button
 Friend WithEvents lstMsg As System.Windows.Forms.ListBox
 Friend WithEvents tmrRecving As System.Windows.Forms.Timer
 Friend WithEvents Label20 As System.Windows.Forms.Label
 Friend WithEvents Label21 As System.Windows.Forms.Label
 Friend WithEvents Label22 As System.Windows.Forms.Label
 Friend WithEvents Label23 As System.Windows.Forms.Label
 Friend WithEvents Label24 As System.Windows.Forms.Label
 Friend WithEvents Label25 As System.Windows.Forms.Label
 Friend WithEvents Label26 As System.Windows.Forms.Label
 Friend WithEvents Label27 As System.Windows.Forms.Label
 Friend WithEvents Label28 As System.Windows.Forms.Label
 Friend WithEvents Label29 As System.Windows.Forms.Label
 Friend WithEvents Label30 As System.Windows.Forms.Label
 Friend WithEvents Label32 As System.Windows.Forms.Label
 Friend WithEvents Label31 As System.Windows.Forms.Label
 Friend WithEvents Label35 As System.Windows.Forms.Label
 Friend WithEvents txtSendLineNo As System.Windows.Forms.TextBox
 Friend WithEvents Label33 As System.Windows.Forms.Label
 Friend WithEvents txtRecvEvtNo As System.Windows.Forms.TextBox
 Friend WithEvents txtRecvLineNo As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT0 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_MIX As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_Str As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT9 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT8 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT7 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT6 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT5 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT4 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT3 As System.Windows.Forms.TextBox
 Friend WithEvents txtRecv_INT2 As System.Windows.Forms.TextBox
	Friend WithEvents txtRecv_INT1 As System.Windows.Forms.TextBox
	Friend WithEvents txtSend_STR2 As System.Windows.Forms.TextBox
	Friend WithEvents txtSend_STR3 As System.Windows.Forms.TextBox
	Friend WithEvents txtSend_STR4 As System.Windows.Forms.TextBox
    Friend WithEvents txtSend_STR5 As System.Windows.Forms.TextBox
    Friend WithEvents chkTrans As System.Windows.Forms.CheckBox
    Friend WithEvents txtUdpAddr As System.Windows.Forms.TextBox

End Class
