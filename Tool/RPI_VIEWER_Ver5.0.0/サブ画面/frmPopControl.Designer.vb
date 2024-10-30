<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopControl
    Inherits PopBaseForm

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
        Me.nudY = New System.Windows.Forms.NumericUpDown()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.btnAddY = New System.Windows.Forms.Button()
        Me.btnSubY = New System.Windows.Forms.Button()
        Me.btnF = New System.Windows.Forms.Button()
        Me.btnT = New System.Windows.Forms.Button()
        Me.btnSub05 = New System.Windows.Forms.Button()
        Me.btnSub01 = New System.Windows.Forms.Button()
        Me.btnAdd01 = New System.Windows.Forms.Button()
        Me.btnAdd05 = New System.Windows.Forms.Button()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.nudSkip = New System.Windows.Forms.NumericUpDown()
        Me.btnReLoad = New System.Windows.Forms.Button()
        Me.btnSkip = New System.Windows.Forms.Button()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.Label22 = New System.Windows.Forms.Label()
        Me.btnAdd100 = New System.Windows.Forms.Button()
        Me.btnAdd1 = New System.Windows.Forms.Button()
        Me.btnAdd50 = New System.Windows.Forms.Button()
        Me.btnAdd10 = New System.Windows.Forms.Button()
        Me.Label21 = New System.Windows.Forms.Label()
        Me.btnSub50 = New System.Windows.Forms.Button()
        Me.btnSub10 = New System.Windows.Forms.Button()
        Me.btnSub100 = New System.Windows.Forms.Button()
        Me.nudCalc = New System.Windows.Forms.NumericUpDown()
        Me.btnSub1 = New System.Windows.Forms.Button()
        Me.btnSubCalc = New System.Windows.Forms.Button()
        Me.btnAddCalc = New System.Windows.Forms.Button()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.btnSpeed8 = New System.Windows.Forms.Button()
        Me.btnSpeed7 = New System.Windows.Forms.Button()
        Me.btnSpeed6 = New System.Windows.Forms.Button()
        Me.btnSpeed5 = New System.Windows.Forms.Button()
        Me.btnSpeed4 = New System.Windows.Forms.Button()
        Me.btnSpeed3 = New System.Windows.Forms.Button()
        Me.btnSpeed2 = New System.Windows.Forms.Button()
        Me.btnDivSpeedDown = New System.Windows.Forms.Button()
        Me.btnDivSpeedUp = New System.Windows.Forms.Button()
        Me.btnSpeed0 = New System.Windows.Forms.Button()
        Me.btnSpeed1 = New System.Windows.Forms.Button()
        Me.lblSpeedNow = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.lblSpeedMax = New System.Windows.Forms.Label()
        Me.lblSpeedMin = New System.Windows.Forms.Label()
        Me.trbSpeed = New System.Windows.Forms.TrackBar()
        Me.grpAutoDown1 = New System.Windows.Forms.GroupBox()
        Me.chkDownUmu = New System.Windows.Forms.CheckBox()
        Me.cmbAutoStop = New System.Windows.Forms.ComboBox()
        Me.cmbAutoDown = New System.Windows.Forms.ComboBox()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label19 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label18 = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.nudAutoDownMpm = New System.Windows.Forms.NumericUpDown()
        Me.nudAutoStopTime = New System.Windows.Forms.NumericUpDown()
        Me.nudAutoE = New System.Windows.Forms.NumericUpDown()
        Me.nudAutoS = New System.Windows.Forms.NumericUpDown()
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.nudRanpS = New System.Windows.Forms.NumericUpDown()
        Me.nudRanpE = New System.Windows.Forms.NumericUpDown()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.Label17 = New System.Windows.Forms.Label()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.btnAutoExec = New System.Windows.Forms.Button()
        Me.Label20 = New System.Windows.Forms.Label()
        Me.grpAutoDown = New System.Windows.Forms.Panel()
        CType(Me.nudY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudSkip, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        CType(Me.nudCalc, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        CType(Me.trbSpeed, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpAutoDown1.SuspendLayout()
        CType(Me.nudAutoDownMpm, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudAutoStopTime, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudAutoE, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudAutoS, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox4.SuspendLayout()
        CType(Me.nudRanpS, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudRanpE, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpAutoDown.SuspendLayout()
        Me.SuspendLayout()
        '
        'nudY
        '
        Me.nudY.DecimalPlaces = 3
        Me.nudY.Location = New System.Drawing.Point(104, 16)
        Me.nudY.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudY.Minimum = New Decimal(New Integer() {1, 0, 0, 196608})
        Me.nudY.Name = "nudY"
        Me.nudY.Size = New System.Drawing.Size(92, 19)
        Me.nudY.TabIndex = 7
        Me.nudY.TabStop = False
        Me.nudY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudY.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(196, 20)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(22, 12)
        Me.Label3.TabIndex = 3
        Me.Label3.Text = "[m]"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(48, 20)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(53, 12)
        Me.Label1.TabIndex = 4
        Me.Label1.Text = "長手間隔"
        '
        'btnAddY
        '
        Me.btnAddY.Location = New System.Drawing.Point(272, 16)
        Me.btnAddY.Name = "btnAddY"
        Me.btnAddY.Size = New System.Drawing.Size(40, 20)
        Me.btnAddY.TabIndex = 3
        Me.btnAddY.Text = "＋"
        Me.btnAddY.UseVisualStyleBackColor = True
        '
        'btnSubY
        '
        Me.btnSubY.Location = New System.Drawing.Point(228, 16)
        Me.btnSubY.Name = "btnSubY"
        Me.btnSubY.Size = New System.Drawing.Size(40, 20)
        Me.btnSubY.TabIndex = 2
        Me.btnSubY.Text = "－"
        Me.btnSubY.UseVisualStyleBackColor = True
        '
        'btnF
        '
        Me.btnF.Location = New System.Drawing.Point(8, 88)
        Me.btnF.Name = "btnF"
        Me.btnF.Size = New System.Drawing.Size(40, 44)
        Me.btnF.TabIndex = 11
        Me.btnF.Text = "先端"
        Me.btnF.UseVisualStyleBackColor = True
        '
        'btnT
        '
        Me.btnT.Location = New System.Drawing.Point(316, 88)
        Me.btnT.Name = "btnT"
        Me.btnT.Size = New System.Drawing.Size(40, 44)
        Me.btnT.TabIndex = 16
        Me.btnT.Text = "尾端"
        Me.btnT.UseVisualStyleBackColor = True
        '
        'btnSub05
        '
        Me.btnSub05.Location = New System.Drawing.Point(96, 88)
        Me.btnSub05.Name = "btnSub05"
        Me.btnSub05.Size = New System.Drawing.Size(40, 20)
        Me.btnSub05.TabIndex = 12
        Me.btnSub05.Tag = "-0.5"
        Me.btnSub05.Text = "-0.5"
        Me.btnSub05.UseVisualStyleBackColor = True
        '
        'btnSub01
        '
        Me.btnSub01.Location = New System.Drawing.Point(140, 88)
        Me.btnSub01.Name = "btnSub01"
        Me.btnSub01.Size = New System.Drawing.Size(40, 20)
        Me.btnSub01.TabIndex = 13
        Me.btnSub01.Tag = "-0.1"
        Me.btnSub01.Text = "-0.1"
        Me.btnSub01.UseVisualStyleBackColor = True
        '
        'btnAdd01
        '
        Me.btnAdd01.Location = New System.Drawing.Point(184, 88)
        Me.btnAdd01.Name = "btnAdd01"
        Me.btnAdd01.Size = New System.Drawing.Size(40, 20)
        Me.btnAdd01.TabIndex = 14
        Me.btnAdd01.Tag = "0.1"
        Me.btnAdd01.Text = "0.1"
        Me.btnAdd01.UseVisualStyleBackColor = True
        '
        'btnAdd05
        '
        Me.btnAdd05.Location = New System.Drawing.Point(228, 88)
        Me.btnAdd05.Name = "btnAdd05"
        Me.btnAdd05.Size = New System.Drawing.Size(40, 20)
        Me.btnAdd05.TabIndex = 15
        Me.btnAdd05.Tag = "0.5"
        Me.btnAdd05.Text = "0.5"
        Me.btnAdd05.UseVisualStyleBackColor = True
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(20, 44)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(77, 12)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "指定位置移動"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(196, 44)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(22, 12)
        Me.Label4.TabIndex = 3
        Me.Label4.Text = "[m]"
        '
        'nudSkip
        '
        Me.nudSkip.DecimalPlaces = 3
        Me.nudSkip.Location = New System.Drawing.Point(104, 40)
        Me.nudSkip.Maximum = New Decimal(New Integer() {1000000, 0, 0, 0})
        Me.nudSkip.Minimum = New Decimal(New Integer() {1, 0, 0, 196608})
        Me.nudSkip.Name = "nudSkip"
        Me.nudSkip.Size = New System.Drawing.Size(92, 19)
        Me.nudSkip.TabIndex = 7
        Me.nudSkip.TabStop = False
        Me.nudSkip.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudSkip.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'btnReLoad
        '
        Me.btnReLoad.Location = New System.Drawing.Point(8, 16)
        Me.btnReLoad.Name = "btnReLoad"
        Me.btnReLoad.Size = New System.Drawing.Size(40, 20)
        Me.btnReLoad.TabIndex = 1
        Me.btnReLoad.Text = "初期"
        Me.btnReLoad.UseVisualStyleBackColor = True
        '
        'btnSkip
        '
        Me.btnSkip.Location = New System.Drawing.Point(228, 40)
        Me.btnSkip.Name = "btnSkip"
        Me.btnSkip.Size = New System.Drawing.Size(84, 20)
        Me.btnSkip.TabIndex = 4
        Me.btnSkip.Text = "移動(Jump)"
        Me.btnSkip.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.btnReLoad)
        Me.GroupBox1.Controls.Add(Me.btnT)
        Me.GroupBox1.Controls.Add(Me.Label22)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.btnAdd100)
        Me.GroupBox1.Controls.Add(Me.btnAdd1)
        Me.GroupBox1.Controls.Add(Me.btnAdd50)
        Me.GroupBox1.Controls.Add(Me.btnAdd05)
        Me.GroupBox1.Controls.Add(Me.btnAdd10)
        Me.GroupBox1.Controls.Add(Me.Label21)
        Me.GroupBox1.Controls.Add(Me.Label3)
        Me.GroupBox1.Controls.Add(Me.btnAdd01)
        Me.GroupBox1.Controls.Add(Me.btnSub50)
        Me.GroupBox1.Controls.Add(Me.btnSub10)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.btnSub01)
        Me.GroupBox1.Controls.Add(Me.btnSub100)
        Me.GroupBox1.Controls.Add(Me.nudCalc)
        Me.GroupBox1.Controls.Add(Me.nudY)
        Me.GroupBox1.Controls.Add(Me.btnSub1)
        Me.GroupBox1.Controls.Add(Me.btnSub05)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.btnF)
        Me.GroupBox1.Controls.Add(Me.nudSkip)
        Me.GroupBox1.Controls.Add(Me.btnSubCalc)
        Me.GroupBox1.Controls.Add(Me.btnSubY)
        Me.GroupBox1.Controls.Add(Me.btnAddCalc)
        Me.GroupBox1.Controls.Add(Me.btnAddY)
        Me.GroupBox1.Controls.Add(Me.btnSkip)
        Me.GroupBox1.Location = New System.Drawing.Point(4, 4)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(364, 136)
        Me.GroupBox1.TabIndex = 17
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "JUMP"
        '
        'Label22
        '
        Me.Label22.AutoSize = True
        Me.Label22.Location = New System.Drawing.Point(48, 68)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(53, 12)
        Me.Label22.TabIndex = 4
        Me.Label22.Text = "計測結果"
        '
        'btnAdd100
        '
        Me.btnAdd100.Location = New System.Drawing.Point(272, 112)
        Me.btnAdd100.Name = "btnAdd100"
        Me.btnAdd100.Size = New System.Drawing.Size(40, 20)
        Me.btnAdd100.TabIndex = 15
        Me.btnAdd100.Tag = "100"
        Me.btnAdd100.Text = "100"
        Me.btnAdd100.UseVisualStyleBackColor = True
        '
        'btnAdd1
        '
        Me.btnAdd1.Location = New System.Drawing.Point(272, 88)
        Me.btnAdd1.Name = "btnAdd1"
        Me.btnAdd1.Size = New System.Drawing.Size(40, 20)
        Me.btnAdd1.TabIndex = 15
        Me.btnAdd1.Tag = "1"
        Me.btnAdd1.Text = "1"
        Me.btnAdd1.UseVisualStyleBackColor = True
        '
        'btnAdd50
        '
        Me.btnAdd50.Location = New System.Drawing.Point(228, 112)
        Me.btnAdd50.Name = "btnAdd50"
        Me.btnAdd50.Size = New System.Drawing.Size(40, 20)
        Me.btnAdd50.TabIndex = 15
        Me.btnAdd50.Tag = "50"
        Me.btnAdd50.Text = "50"
        Me.btnAdd50.UseVisualStyleBackColor = True
        '
        'btnAdd10
        '
        Me.btnAdd10.Location = New System.Drawing.Point(184, 112)
        Me.btnAdd10.Name = "btnAdd10"
        Me.btnAdd10.Size = New System.Drawing.Size(40, 20)
        Me.btnAdd10.TabIndex = 14
        Me.btnAdd10.Tag = "10"
        Me.btnAdd10.Text = "10"
        Me.btnAdd10.UseVisualStyleBackColor = True
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Location = New System.Drawing.Point(196, 68)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(22, 12)
        Me.Label21.TabIndex = 3
        Me.Label21.Text = "[m]"
        '
        'btnSub50
        '
        Me.btnSub50.Location = New System.Drawing.Point(96, 112)
        Me.btnSub50.Name = "btnSub50"
        Me.btnSub50.Size = New System.Drawing.Size(40, 20)
        Me.btnSub50.TabIndex = 13
        Me.btnSub50.Tag = "-50"
        Me.btnSub50.Text = "-50"
        Me.btnSub50.UseVisualStyleBackColor = True
        '
        'btnSub10
        '
        Me.btnSub10.Location = New System.Drawing.Point(140, 112)
        Me.btnSub10.Name = "btnSub10"
        Me.btnSub10.Size = New System.Drawing.Size(40, 20)
        Me.btnSub10.TabIndex = 13
        Me.btnSub10.Tag = "-10"
        Me.btnSub10.Text = "-10"
        Me.btnSub10.UseVisualStyleBackColor = True
        '
        'btnSub100
        '
        Me.btnSub100.Location = New System.Drawing.Point(52, 112)
        Me.btnSub100.Name = "btnSub100"
        Me.btnSub100.Size = New System.Drawing.Size(40, 20)
        Me.btnSub100.TabIndex = 12
        Me.btnSub100.Tag = "-100"
        Me.btnSub100.Text = "-100"
        Me.btnSub100.UseVisualStyleBackColor = True
        '
        'nudCalc
        '
        Me.nudCalc.DecimalPlaces = 3
        Me.nudCalc.Location = New System.Drawing.Point(104, 64)
        Me.nudCalc.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudCalc.Minimum = New Decimal(New Integer() {1, 0, 0, 196608})
        Me.nudCalc.Name = "nudCalc"
        Me.nudCalc.Size = New System.Drawing.Size(92, 19)
        Me.nudCalc.TabIndex = 7
        Me.nudCalc.TabStop = False
        Me.nudCalc.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudCalc.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'btnSub1
        '
        Me.btnSub1.Location = New System.Drawing.Point(52, 88)
        Me.btnSub1.Name = "btnSub1"
        Me.btnSub1.Size = New System.Drawing.Size(40, 20)
        Me.btnSub1.TabIndex = 12
        Me.btnSub1.Tag = "-1"
        Me.btnSub1.Text = "-1"
        Me.btnSub1.UseVisualStyleBackColor = True
        '
        'btnSubCalc
        '
        Me.btnSubCalc.Location = New System.Drawing.Point(228, 64)
        Me.btnSubCalc.Name = "btnSubCalc"
        Me.btnSubCalc.Size = New System.Drawing.Size(40, 20)
        Me.btnSubCalc.TabIndex = 2
        Me.btnSubCalc.Text = "－"
        Me.btnSubCalc.UseVisualStyleBackColor = True
        '
        'btnAddCalc
        '
        Me.btnAddCalc.Location = New System.Drawing.Point(272, 64)
        Me.btnAddCalc.Name = "btnAddCalc"
        Me.btnAddCalc.Size = New System.Drawing.Size(40, 20)
        Me.btnAddCalc.TabIndex = 3
        Me.btnAddCalc.Text = "＋"
        Me.btnAddCalc.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.btnSpeed8)
        Me.GroupBox2.Controls.Add(Me.btnSpeed7)
        Me.GroupBox2.Controls.Add(Me.btnSpeed6)
        Me.GroupBox2.Controls.Add(Me.btnSpeed5)
        Me.GroupBox2.Controls.Add(Me.btnSpeed4)
        Me.GroupBox2.Controls.Add(Me.btnSpeed3)
        Me.GroupBox2.Controls.Add(Me.btnSpeed2)
        Me.GroupBox2.Controls.Add(Me.btnDivSpeedDown)
        Me.GroupBox2.Controls.Add(Me.btnDivSpeedUp)
        Me.GroupBox2.Controls.Add(Me.btnSpeed0)
        Me.GroupBox2.Controls.Add(Me.btnSpeed1)
        Me.GroupBox2.Controls.Add(Me.lblSpeedNow)
        Me.GroupBox2.Controls.Add(Me.Label5)
        Me.GroupBox2.Controls.Add(Me.lblSpeedMax)
        Me.GroupBox2.Controls.Add(Me.lblSpeedMin)
        Me.GroupBox2.Controls.Add(Me.trbSpeed)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(4, 144)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(364, 116)
        Me.GroupBox2.TabIndex = 18
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "自動送り"
        '
        'btnSpeed8
        '
        Me.btnSpeed8.Location = New System.Drawing.Point(324, 20)
        Me.btnSpeed8.Name = "btnSpeed8"
        Me.btnSpeed8.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed8.TabIndex = 16
        Me.btnSpeed8.Tag = "150"
        Me.btnSpeed8.Text = "150"
        Me.btnSpeed8.UseVisualStyleBackColor = True
        '
        'btnSpeed7
        '
        Me.btnSpeed7.Location = New System.Drawing.Point(288, 20)
        Me.btnSpeed7.Name = "btnSpeed7"
        Me.btnSpeed7.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed7.TabIndex = 16
        Me.btnSpeed7.Tag = "100"
        Me.btnSpeed7.Text = "100"
        Me.btnSpeed7.UseVisualStyleBackColor = True
        '
        'btnSpeed6
        '
        Me.btnSpeed6.Location = New System.Drawing.Point(252, 20)
        Me.btnSpeed6.Name = "btnSpeed6"
        Me.btnSpeed6.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed6.TabIndex = 16
        Me.btnSpeed6.Tag = "80"
        Me.btnSpeed6.Text = "80"
        Me.btnSpeed6.UseVisualStyleBackColor = True
        '
        'btnSpeed5
        '
        Me.btnSpeed5.Location = New System.Drawing.Point(216, 20)
        Me.btnSpeed5.Name = "btnSpeed5"
        Me.btnSpeed5.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed5.TabIndex = 16
        Me.btnSpeed5.Tag = "60"
        Me.btnSpeed5.Text = "60"
        Me.btnSpeed5.UseVisualStyleBackColor = True
        '
        'btnSpeed4
        '
        Me.btnSpeed4.Location = New System.Drawing.Point(180, 20)
        Me.btnSpeed4.Name = "btnSpeed4"
        Me.btnSpeed4.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed4.TabIndex = 16
        Me.btnSpeed4.Tag = "30"
        Me.btnSpeed4.Text = "30"
        Me.btnSpeed4.UseVisualStyleBackColor = True
        '
        'btnSpeed3
        '
        Me.btnSpeed3.Location = New System.Drawing.Point(144, 20)
        Me.btnSpeed3.Name = "btnSpeed3"
        Me.btnSpeed3.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed3.TabIndex = 16
        Me.btnSpeed3.Tag = "20"
        Me.btnSpeed3.Text = "20"
        Me.btnSpeed3.UseVisualStyleBackColor = True
        '
        'btnSpeed2
        '
        Me.btnSpeed2.Location = New System.Drawing.Point(108, 20)
        Me.btnSpeed2.Name = "btnSpeed2"
        Me.btnSpeed2.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed2.TabIndex = 16
        Me.btnSpeed2.Tag = "10"
        Me.btnSpeed2.Text = "10"
        Me.btnSpeed2.UseVisualStyleBackColor = True
        '
        'btnDivSpeedDown
        '
        Me.btnDivSpeedDown.Location = New System.Drawing.Point(144, 44)
        Me.btnDivSpeedDown.Name = "btnDivSpeedDown"
        Me.btnDivSpeedDown.Size = New System.Drawing.Size(68, 20)
        Me.btnDivSpeedDown.TabIndex = 16
        Me.btnDivSpeedDown.Tag = "-1"
        Me.btnDivSpeedDown.Text = "逆転"
        Me.btnDivSpeedDown.UseVisualStyleBackColor = True
        '
        'btnDivSpeedUp
        '
        Me.btnDivSpeedUp.Location = New System.Drawing.Point(72, 44)
        Me.btnDivSpeedUp.Name = "btnDivSpeedUp"
        Me.btnDivSpeedUp.Size = New System.Drawing.Size(68, 20)
        Me.btnDivSpeedUp.TabIndex = 16
        Me.btnDivSpeedUp.Tag = "1"
        Me.btnDivSpeedUp.Text = "正転"
        Me.btnDivSpeedUp.UseVisualStyleBackColor = True
        '
        'btnSpeed0
        '
        Me.btnSpeed0.Location = New System.Drawing.Point(8, 20)
        Me.btnSpeed0.Name = "btnSpeed0"
        Me.btnSpeed0.Size = New System.Drawing.Size(60, 88)
        Me.btnSpeed0.TabIndex = 16
        Me.btnSpeed0.Tag = "0"
        Me.btnSpeed0.Text = "停止"
        Me.btnSpeed0.UseVisualStyleBackColor = True
        '
        'btnSpeed1
        '
        Me.btnSpeed1.Location = New System.Drawing.Point(72, 20)
        Me.btnSpeed1.Name = "btnSpeed1"
        Me.btnSpeed1.Size = New System.Drawing.Size(32, 20)
        Me.btnSpeed1.TabIndex = 16
        Me.btnSpeed1.Tag = "5"
        Me.btnSpeed1.Text = "5"
        Me.btnSpeed1.UseVisualStyleBackColor = True
        '
        'lblSpeedNow
        '
        Me.lblSpeedNow.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblSpeedNow.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSpeedNow.Location = New System.Drawing.Point(280, 72)
        Me.lblSpeedNow.Name = "lblSpeedNow"
        Me.lblSpeedNow.Size = New System.Drawing.Size(72, 28)
        Me.lblSpeedNow.TabIndex = 15
        Me.lblSpeedNow.Text = "0"
        Me.lblSpeedNow.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(284, 52)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(59, 12)
        Me.Label5.TabIndex = 15
        Me.Label5.Text = "速度[mpm]"
        '
        'lblSpeedMax
        '
        Me.lblSpeedMax.AutoSize = True
        Me.lblSpeedMax.Location = New System.Drawing.Point(240, 97)
        Me.lblSpeedMax.Name = "lblSpeedMax"
        Me.lblSpeedMax.Size = New System.Drawing.Size(23, 12)
        Me.lblSpeedMax.TabIndex = 15
        Me.lblSpeedMax.Text = "150"
        '
        'lblSpeedMin
        '
        Me.lblSpeedMin.AutoSize = True
        Me.lblSpeedMin.Location = New System.Drawing.Point(72, 97)
        Me.lblSpeedMin.Name = "lblSpeedMin"
        Me.lblSpeedMin.Size = New System.Drawing.Size(29, 12)
        Me.lblSpeedMin.TabIndex = 15
        Me.lblSpeedMin.Text = "-150"
        '
        'trbSpeed
        '
        Me.trbSpeed.LargeChange = 10
        Me.trbSpeed.Location = New System.Drawing.Point(76, 67)
        Me.trbSpeed.Maximum = 150
        Me.trbSpeed.Minimum = -150
        Me.trbSpeed.Name = "trbSpeed"
        Me.trbSpeed.Size = New System.Drawing.Size(196, 45)
        Me.trbSpeed.SmallChange = 5
        Me.trbSpeed.TabIndex = 14
        Me.trbSpeed.TickFrequency = 10
        '
        'grpAutoDown1
        '
        Me.grpAutoDown1.Controls.Add(Me.chkDownUmu)
        Me.grpAutoDown1.Controls.Add(Me.cmbAutoStop)
        Me.grpAutoDown1.Controls.Add(Me.cmbAutoDown)
        Me.grpAutoDown1.Controls.Add(Me.Label13)
        Me.grpAutoDown1.Controls.Add(Me.Label12)
        Me.grpAutoDown1.Controls.Add(Me.Label19)
        Me.grpAutoDown1.Controls.Add(Me.Label10)
        Me.grpAutoDown1.Controls.Add(Me.Label9)
        Me.grpAutoDown1.Controls.Add(Me.Label6)
        Me.grpAutoDown1.Controls.Add(Me.Label18)
        Me.grpAutoDown1.Controls.Add(Me.Label11)
        Me.grpAutoDown1.Controls.Add(Me.Label8)
        Me.grpAutoDown1.Controls.Add(Me.Label7)
        Me.grpAutoDown1.Controls.Add(Me.nudAutoDownMpm)
        Me.grpAutoDown1.Controls.Add(Me.nudAutoStopTime)
        Me.grpAutoDown1.Controls.Add(Me.nudAutoE)
        Me.grpAutoDown1.Controls.Add(Me.nudAutoS)
        Me.grpAutoDown1.Location = New System.Drawing.Point(4, 4)
        Me.grpAutoDown1.Name = "grpAutoDown1"
        Me.grpAutoDown1.Size = New System.Drawing.Size(168, 172)
        Me.grpAutoDown1.TabIndex = 19
        Me.grpAutoDown1.TabStop = False
        Me.grpAutoDown1.Text = "自動減速"
        '
        'chkDownUmu
        '
        Me.chkDownUmu.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkDownUmu.CheckAlign = System.Drawing.ContentAlignment.TopLeft
        Me.chkDownUmu.Location = New System.Drawing.Point(8, 16)
        Me.chkDownUmu.Name = "chkDownUmu"
        Me.chkDownUmu.Size = New System.Drawing.Size(88, 24)
        Me.chkDownUmu.TabIndex = 12
        Me.chkDownUmu.Text = "自動減速有無"
        Me.chkDownUmu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkDownUmu.UseVisualStyleBackColor = True
        '
        'cmbAutoStop
        '
        Me.cmbAutoStop.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbAutoStop.FormattingEnabled = True
        Me.cmbAutoStop.Location = New System.Drawing.Point(52, 148)
        Me.cmbAutoStop.Name = "cmbAutoStop"
        Me.cmbAutoStop.Size = New System.Drawing.Size(92, 20)
        Me.cmbAutoStop.TabIndex = 11
        '
        'cmbAutoDown
        '
        Me.cmbAutoDown.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbAutoDown.FormattingEnabled = True
        Me.cmbAutoDown.Location = New System.Drawing.Point(52, 128)
        Me.cmbAutoDown.Name = "cmbAutoDown"
        Me.cmbAutoDown.Size = New System.Drawing.Size(92, 20)
        Me.cmbAutoDown.TabIndex = 11
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Location = New System.Drawing.Point(8, 152)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(41, 12)
        Me.Label13.TabIndex = 9
        Me.Label13.Text = "停止Gr"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(8, 132)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(41, 12)
        Me.Label12.TabIndex = 9
        Me.Label12.Text = "減速Gr"
        '
        'Label19
        '
        Me.Label19.AutoSize = True
        Me.Label19.Location = New System.Drawing.Point(8, 92)
        Me.Label19.Name = "Label19"
        Me.Label19.Size = New System.Drawing.Size(53, 12)
        Me.Label19.TabIndex = 9
        Me.Label19.Text = "減速速度"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Location = New System.Drawing.Point(8, 112)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(53, 12)
        Me.Label10.TabIndex = 9
        Me.Label10.Text = "停止秒数"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Location = New System.Drawing.Point(8, 72)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(53, 12)
        Me.Label9.TabIndex = 9
        Me.Label9.Text = "終了位置"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(8, 52)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(53, 12)
        Me.Label6.TabIndex = 9
        Me.Label6.Text = "開始位置"
        '
        'Label18
        '
        Me.Label18.AutoSize = True
        Me.Label18.Location = New System.Drawing.Point(124, 92)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(37, 12)
        Me.Label18.TabIndex = 8
        Me.Label18.Text = "[mpm]"
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(124, 112)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(19, 12)
        Me.Label11.TabIndex = 8
        Me.Label11.Text = "[s]"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(124, 72)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(22, 12)
        Me.Label8.TabIndex = 8
        Me.Label8.Text = "[m]"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(124, 52)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(22, 12)
        Me.Label7.TabIndex = 8
        Me.Label7.Text = "[m]"
        '
        'nudAutoDownMpm
        '
        Me.nudAutoDownMpm.Location = New System.Drawing.Point(64, 88)
        Me.nudAutoDownMpm.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudAutoDownMpm.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudAutoDownMpm.Name = "nudAutoDownMpm"
        Me.nudAutoDownMpm.Size = New System.Drawing.Size(60, 19)
        Me.nudAutoDownMpm.TabIndex = 10
        Me.nudAutoDownMpm.TabStop = False
        Me.nudAutoDownMpm.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudAutoDownMpm.Value = New Decimal(New Integer() {30, 0, 0, 0})
        '
        'nudAutoStopTime
        '
        Me.nudAutoStopTime.DecimalPlaces = 1
        Me.nudAutoStopTime.Location = New System.Drawing.Point(64, 108)
        Me.nudAutoStopTime.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudAutoStopTime.Name = "nudAutoStopTime"
        Me.nudAutoStopTime.Size = New System.Drawing.Size(60, 19)
        Me.nudAutoStopTime.TabIndex = 10
        Me.nudAutoStopTime.TabStop = False
        Me.nudAutoStopTime.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudAutoStopTime.Value = New Decimal(New Integer() {5, 0, 0, 65536})
        '
        'nudAutoE
        '
        Me.nudAutoE.DecimalPlaces = 1
        Me.nudAutoE.Location = New System.Drawing.Point(64, 68)
        Me.nudAutoE.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudAutoE.Name = "nudAutoE"
        Me.nudAutoE.Size = New System.Drawing.Size(60, 19)
        Me.nudAutoE.TabIndex = 10
        Me.nudAutoE.TabStop = False
        Me.nudAutoE.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'nudAutoS
        '
        Me.nudAutoS.DecimalPlaces = 1
        Me.nudAutoS.Location = New System.Drawing.Point(64, 48)
        Me.nudAutoS.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudAutoS.Name = "nudAutoS"
        Me.nudAutoS.Size = New System.Drawing.Size(60, 19)
        Me.nudAutoS.TabIndex = 10
        Me.nudAutoS.TabStop = False
        Me.nudAutoS.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.nudRanpS)
        Me.GroupBox4.Controls.Add(Me.nudRanpE)
        Me.GroupBox4.Controls.Add(Me.Label14)
        Me.GroupBox4.Controls.Add(Me.Label15)
        Me.GroupBox4.Controls.Add(Me.Label17)
        Me.GroupBox4.Controls.Add(Me.Label16)
        Me.GroupBox4.Location = New System.Drawing.Point(180, 4)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(152, 56)
        Me.GroupBox4.TabIndex = 19
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "自動接近ランプ"
        '
        'nudRanpS
        '
        Me.nudRanpS.DecimalPlaces = 1
        Me.nudRanpS.Location = New System.Drawing.Point(64, 12)
        Me.nudRanpS.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudRanpS.Name = "nudRanpS"
        Me.nudRanpS.Size = New System.Drawing.Size(60, 19)
        Me.nudRanpS.TabIndex = 10
        Me.nudRanpS.TabStop = False
        Me.nudRanpS.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'nudRanpE
        '
        Me.nudRanpE.DecimalPlaces = 1
        Me.nudRanpE.Location = New System.Drawing.Point(64, 32)
        Me.nudRanpE.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudRanpE.Name = "nudRanpE"
        Me.nudRanpE.Size = New System.Drawing.Size(60, 19)
        Me.nudRanpE.TabIndex = 10
        Me.nudRanpE.TabStop = False
        Me.nudRanpE.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Location = New System.Drawing.Point(124, 16)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(22, 12)
        Me.Label14.TabIndex = 8
        Me.Label14.Text = "[m]"
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Location = New System.Drawing.Point(124, 36)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(22, 12)
        Me.Label15.TabIndex = 8
        Me.Label15.Text = "[m]"
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Location = New System.Drawing.Point(8, 36)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(53, 12)
        Me.Label17.TabIndex = 9
        Me.Label17.Text = "終了位置"
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Location = New System.Drawing.Point(8, 16)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(53, 12)
        Me.Label16.TabIndex = 9
        Me.Label16.Text = "開始位置"
        '
        'btnAutoExec
        '
        Me.btnAutoExec.Location = New System.Drawing.Point(180, 64)
        Me.btnAutoExec.Name = "btnAutoExec"
        Me.btnAutoExec.Size = New System.Drawing.Size(152, 24)
        Me.btnAutoExec.TabIndex = 13
        Me.btnAutoExec.Text = "自動関係 設定反映"
        Me.btnAutoExec.UseVisualStyleBackColor = True
        '
        'Label20
        '
        Me.Label20.AutoSize = True
        Me.Label20.Location = New System.Drawing.Point(176, 92)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(186, 24)
        Me.Label20.TabIndex = 15
        Me.Label20.Text = "※150mpm以上での自動減速は、" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "　　正常に動作しない場合があります。"
        '
        'grpAutoDown
        '
        Me.grpAutoDown.Controls.Add(Me.btnAutoExec)
        Me.grpAutoDown.Controls.Add(Me.grpAutoDown1)
        Me.grpAutoDown.Controls.Add(Me.GroupBox4)
        Me.grpAutoDown.Controls.Add(Me.Label20)
        Me.grpAutoDown.Location = New System.Drawing.Point(0, 264)
        Me.grpAutoDown.Name = "grpAutoDown"
        Me.grpAutoDown.Size = New System.Drawing.Size(368, 180)
        Me.grpAutoDown.TabIndex = 20
        '
        'frmPopControl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(371, 446)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.grpAutoDown)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopControl"
        Me.ShowIcon = False
        Me.Text = "操作管理"
        CType(Me.nudY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudSkip, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.nudCalc, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        CType(Me.trbSpeed, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpAutoDown1.ResumeLayout(False)
        Me.grpAutoDown1.PerformLayout()
        CType(Me.nudAutoDownMpm, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudAutoStopTime, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudAutoE, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudAutoS, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox4.PerformLayout()
        CType(Me.nudRanpS, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudRanpE, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpAutoDown.ResumeLayout(False)
        Me.grpAutoDown.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents nudY As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents btnAddY As System.Windows.Forms.Button
    Friend WithEvents btnSubY As System.Windows.Forms.Button
    Friend WithEvents btnF As System.Windows.Forms.Button
    Friend WithEvents btnT As System.Windows.Forms.Button
    Friend WithEvents btnSub05 As System.Windows.Forms.Button
    Friend WithEvents btnSub01 As System.Windows.Forms.Button
    Friend WithEvents btnAdd01 As System.Windows.Forms.Button
    Friend WithEvents btnAdd05 As System.Windows.Forms.Button
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents nudSkip As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnReLoad As System.Windows.Forms.Button
    Friend WithEvents btnSkip As System.Windows.Forms.Button
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents btnAdd50 As System.Windows.Forms.Button
    Friend WithEvents btnAdd10 As System.Windows.Forms.Button
    Friend WithEvents btnSub50 As System.Windows.Forms.Button
    Friend WithEvents btnSub10 As System.Windows.Forms.Button
    Friend WithEvents btnSub100 As System.Windows.Forms.Button
    Friend WithEvents btnSub1 As System.Windows.Forms.Button
    Friend WithEvents btnAdd100 As System.Windows.Forms.Button
    Friend WithEvents btnAdd1 As System.Windows.Forms.Button
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents btnSpeed8 As System.Windows.Forms.Button
    Friend WithEvents btnSpeed7 As System.Windows.Forms.Button
    Friend WithEvents btnSpeed6 As System.Windows.Forms.Button
    Friend WithEvents btnSpeed5 As System.Windows.Forms.Button
    Friend WithEvents btnSpeed4 As System.Windows.Forms.Button
    Friend WithEvents btnSpeed3 As System.Windows.Forms.Button
    Friend WithEvents btnSpeed2 As System.Windows.Forms.Button
    Friend WithEvents btnDivSpeedDown As System.Windows.Forms.Button
    Friend WithEvents btnDivSpeedUp As System.Windows.Forms.Button
    Friend WithEvents btnSpeed0 As System.Windows.Forms.Button
    Friend WithEvents btnSpeed1 As System.Windows.Forms.Button
    Friend WithEvents lblSpeedNow As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents lblSpeedMax As System.Windows.Forms.Label
    Friend WithEvents lblSpeedMin As System.Windows.Forms.Label
    Friend WithEvents trbSpeed As System.Windows.Forms.TrackBar
    Friend WithEvents grpAutoDown1 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents nudAutoStopTime As System.Windows.Forms.NumericUpDown
    Friend WithEvents nudAutoE As System.Windows.Forms.NumericUpDown
    Friend WithEvents nudAutoS As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents cmbAutoDown As System.Windows.Forms.ComboBox
    Friend WithEvents cmbAutoStop As System.Windows.Forms.ComboBox
    Friend WithEvents nudRanpS As System.Windows.Forms.NumericUpDown
    Friend WithEvents nudRanpE As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents Label17 As System.Windows.Forms.Label
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents Label19 As System.Windows.Forms.Label
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents nudAutoDownMpm As System.Windows.Forms.NumericUpDown
    Friend WithEvents chkDownUmu As System.Windows.Forms.CheckBox
    Friend WithEvents btnAutoExec As System.Windows.Forms.Button
    Friend WithEvents Label20 As System.Windows.Forms.Label
    Friend WithEvents grpAutoDown As System.Windows.Forms.Panel
    Friend WithEvents Label22 As System.Windows.Forms.Label
    Friend WithEvents Label21 As System.Windows.Forms.Label
    Friend WithEvents nudCalc As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnSubCalc As System.Windows.Forms.Button
    Friend WithEvents btnAddCalc As System.Windows.Forms.Button
End Class
