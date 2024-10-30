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
        Me.components = New System.ComponentModel.Container()
        Dim ListViewItem1 As System.Windows.Forms.ListViewItem = New System.Windows.Forms.ListViewItem("ああああ")
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.picFrameImg = New System.Windows.Forms.PictureBox()
        Me.spcMain = New System.Windows.Forms.SplitContainer()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.lsvRpiHeader = New System.Windows.Forms.ListView()
        Me.ColumnHeader1 = CType(New System.Windows.Forms.ColumnHeader(), System.Windows.Forms.ColumnHeader)
        Me.ColumnHeader2 = CType(New System.Windows.Forms.ColumnHeader(), System.Windows.Forms.ColumnHeader)
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.btnSave = New System.Windows.Forms.Button()
        Me.nudFrameNo = New System.Windows.Forms.NumericUpDown()
        Me.lblEdge = New System.Windows.Forms.Label()
        Me.lblInfPos = New System.Windows.Forms.Label()
        Me.lblImgSize = New System.Windows.Forms.Label()
        Me.lblImgPos = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.vsBar = New tClassLibrary.tCnt.tCnt_VScrollBar()
        Me.pnlHBar = New System.Windows.Forms.Panel()
        Me.hsBar = New tClassLibrary.tCnt.tCnt_HScrollBar()
        Me.btnRituNon = New System.Windows.Forms.Button()
        Me.tlpRanp = New System.Windows.Forms.TableLayoutPanel()
        Me.pnlLed1 = New System.Windows.Forms.Panel()
        Me.pnlLed2 = New System.Windows.Forms.Panel()
        Me.pnlLed3 = New System.Windows.Forms.Panel()
        Me.pnlLed4 = New System.Windows.Forms.Panel()
        Me.pnlLed5 = New System.Windows.Forms.Panel()
        Me.pnlLed6 = New System.Windows.Forms.Panel()
        Me.pnlLed7 = New System.Windows.Forms.Panel()
        Me.pnlLed8 = New System.Windows.Forms.Panel()
        Me.pnlLed9 = New System.Windows.Forms.Panel()
        Me.tlsMainMenu = New System.Windows.Forms.ToolStrip()
        Me.tsbSearch = New System.Windows.Forms.ToolStripButton()
        Me.tsbSearchUp = New System.Windows.Forms.ToolStripButton()
        Me.tsbSearchDown = New System.Windows.Forms.ToolStripButton()
        Me.tsbSearchOU = New System.Windows.Forms.ToolStripButton()
        Me.tsbFolder = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator2 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbDspBig = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator6 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbSync = New System.Windows.Forms.ToolStripButton()
        Me.tsbMemori = New System.Windows.Forms.ToolStripButton()
        Me.tsbOVL = New System.Windows.Forms.ToolStripButton()
        Me.tsbReSetOVL = New System.Windows.Forms.ToolStripButton()
        Me.tsbGridLine = New System.Windows.Forms.ToolStripButton()
        Me.tsbEdge = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator13 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbJump = New System.Windows.Forms.ToolStripButton()
        Me.tsbBookmark = New System.Windows.Forms.ToolStripButton()
        Me.tsbInputList = New System.Windows.Forms.ToolStripButton()
        Me.tsbKiriInput = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator8 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbDspSet = New System.Windows.Forms.ToolStripButton()
        Me.tsbBrite = New System.Windows.Forms.ToolStripButton()
        Me.tsbSetting = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator9 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbCoilItem = New System.Windows.Forms.ToolStripButton()
        Me.tsbKizuMap = New System.Windows.Forms.ToolStripButton()
        Me.tsbKizuList = New System.Windows.Forms.ToolStripButton()
        Me.tsbDefMap = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator4 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbDSWS = New System.Windows.Forms.ToolStripButton()
        Me.tsbTB = New System.Windows.Forms.ToolStripButton()
        Me.tsbRot = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator5 = New System.Windows.Forms.ToolStripSeparator()
        Me.tscRate = New System.Windows.Forms.ToolStripComboBox()
        Me.tscSpeed = New System.Windows.Forms.ToolStripComboBox()
        Me.tscChannel = New System.Windows.Forms.ToolStripComboBox()
        Me.ToolStripSeparator10 = New System.Windows.Forms.ToolStripSeparator()
        Me.tspbReadRpi = New System.Windows.Forms.ToolStripProgressBar()
        Me.ToolStripSeparator1 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbPrint = New System.Windows.Forms.ToolStripButton()
        Me.ToolStripSeparator3 = New System.Windows.Forms.ToolStripSeparator()
        Me.tsbInfMode = New System.Windows.Forms.ToolStripButton()
        Me.tsbImgQuarty = New System.Windows.Forms.ToolStripButton()
        Me.timDsp = New System.Windows.Forms.Timer(Me.components)
        Me.timThreadRpiRead = New System.Windows.Forms.Timer(Me.components)
        Me.tlsMainMenu2 = New System.Windows.Forms.ToolStrip()
        Me.ToolStripLabel7 = New System.Windows.Forms.ToolStripLabel()
        Me.tslFrameNowCnt = New System.Windows.Forms.ToolStripLabel()
        Me.ToolStripLabel8 = New System.Windows.Forms.ToolStripLabel()
        Me.tslFrameAllCnt = New System.Windows.Forms.ToolStripLabel()
        Me.ToolStripSeparator7 = New System.Windows.Forms.ToolStripSeparator()
        Me.ToolStripLabel9 = New System.Windows.Forms.ToolStripLabel()
        Me.tslTotalPixel = New System.Windows.Forms.ToolStripLabel()
        Me.ToolStripSeparator11 = New System.Windows.Forms.ToolStripSeparator()
        Me.ToolStripLabel11 = New System.Windows.Forms.ToolStripLabel()
        Me.tslTotalMm = New System.Windows.Forms.ToolStripLabel()
        Me.ToolStripLabel1 = New System.Windows.Forms.ToolStripLabel()
        Me.ToolStripSeparator12 = New System.Windows.Forms.ToolStripSeparator()
        Me.tslFPS = New System.Windows.Forms.ToolStripLabel()
        Me.ToolStripLabel3 = New System.Windows.Forms.ToolStripLabel()
        Me.ToolStripSeparator14 = New System.Windows.Forms.ToolStripSeparator()
        CType(Me.picFrameImg, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.spcMain.Panel1.SuspendLayout()
        Me.spcMain.Panel2.SuspendLayout()
        Me.spcMain.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        CType(Me.nudFrameNo, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlHBar.SuspendLayout()
        Me.tlpRanp.SuspendLayout()
        Me.tlsMainMenu.SuspendLayout()
        Me.tlsMainMenu2.SuspendLayout()
        Me.SuspendLayout()
        '
        'picFrameImg
        '
        Me.picFrameImg.BackColor = System.Drawing.Color.White
        Me.picFrameImg.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picFrameImg.Dock = System.Windows.Forms.DockStyle.Fill
        Me.picFrameImg.Location = New System.Drawing.Point(0, 0)
        Me.picFrameImg.Name = "picFrameImg"
        Me.picFrameImg.Size = New System.Drawing.Size(763, 320)
        Me.picFrameImg.TabIndex = 0
        Me.picFrameImg.TabStop = False
        '
        'spcMain
        '
        Me.spcMain.BackColor = System.Drawing.Color.FromArgb(CType(CType(255, Byte), Integer), CType(CType(224, Byte), Integer), CType(CType(192, Byte), Integer))
        Me.spcMain.Dock = System.Windows.Forms.DockStyle.Fill
        Me.spcMain.FixedPanel = System.Windows.Forms.FixedPanel.Panel1
        Me.spcMain.Location = New System.Drawing.Point(0, 0)
        Me.spcMain.Name = "spcMain"
        '
        'spcMain.Panel1
        '
        Me.spcMain.Panel1.BackColor = System.Drawing.SystemColors.Control
        Me.spcMain.Panel1.Controls.Add(Me.GroupBox2)
        Me.spcMain.Panel1.Controls.Add(Me.GroupBox1)
        '
        'spcMain.Panel2
        '
        Me.spcMain.Panel2.BackColor = System.Drawing.SystemColors.Control
        Me.spcMain.Panel2.Controls.Add(Me.picFrameImg)
        Me.spcMain.Panel2.Controls.Add(Me.vsBar)
        Me.spcMain.Panel2.Controls.Add(Me.pnlHBar)
        Me.spcMain.Panel2.Controls.Add(Me.tlpRanp)
        Me.spcMain.Size = New System.Drawing.Size(987, 342)
        Me.spcMain.SplitterDistance = 205
        Me.spcMain.TabIndex = 5
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.lsvRpiHeader)
        Me.GroupBox2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.GroupBox2.Location = New System.Drawing.Point(0, 99)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(205, 243)
        Me.GroupBox2.TabIndex = 2
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "RPIヘッダー情報"
        '
        'lsvRpiHeader
        '
        Me.lsvRpiHeader.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.ColumnHeader1, Me.ColumnHeader2})
        Me.lsvRpiHeader.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lsvRpiHeader.FullRowSelect = True
        Me.lsvRpiHeader.GridLines = True
        ListViewItem1.StateImageIndex = 0
        Me.lsvRpiHeader.Items.AddRange(New System.Windows.Forms.ListViewItem() {ListViewItem1})
        Me.lsvRpiHeader.LabelWrap = False
        Me.lsvRpiHeader.Location = New System.Drawing.Point(3, 15)
        Me.lsvRpiHeader.MultiSelect = False
        Me.lsvRpiHeader.Name = "lsvRpiHeader"
        Me.lsvRpiHeader.Size = New System.Drawing.Size(199, 225)
        Me.lsvRpiHeader.TabIndex = 0
        Me.lsvRpiHeader.TabStop = False
        Me.lsvRpiHeader.UseCompatibleStateImageBehavior = False
        Me.lsvRpiHeader.View = System.Windows.Forms.View.Details
        '
        'ColumnHeader1
        '
        Me.ColumnHeader1.Text = "名称"
        Me.ColumnHeader1.Width = 113
        '
        'ColumnHeader2
        '
        Me.ColumnHeader2.Text = "値"
        Me.ColumnHeader2.Width = 80
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.btnSave)
        Me.GroupBox1.Controls.Add(Me.nudFrameNo)
        Me.GroupBox1.Controls.Add(Me.lblEdge)
        Me.GroupBox1.Controls.Add(Me.lblInfPos)
        Me.GroupBox1.Controls.Add(Me.lblImgSize)
        Me.GroupBox1.Controls.Add(Me.lblImgPos)
        Me.GroupBox1.Controls.Add(Me.Label5)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.Label3)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Dock = System.Windows.Forms.DockStyle.Top
        Me.GroupBox1.Location = New System.Drawing.Point(0, 0)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(205, 99)
        Me.GroupBox1.TabIndex = 2
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "RPIフレーム情報"
        '
        'btnSave
        '
        Me.btnSave.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSave.Location = New System.Drawing.Point(168, 12)
        Me.btnSave.Name = "btnSave"
        Me.btnSave.Size = New System.Drawing.Size(36, 20)
        Me.btnSave.TabIndex = 2
        Me.btnSave.Text = "保存"
        Me.btnSave.UseVisualStyleBackColor = True
        '
        'nudFrameNo
        '
        Me.nudFrameNo.Location = New System.Drawing.Point(116, 12)
        Me.nudFrameNo.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudFrameNo.Name = "nudFrameNo"
        Me.nudFrameNo.Size = New System.Drawing.Size(56, 19)
        Me.nudFrameNo.TabIndex = 1
        Me.nudFrameNo.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'lblEdge
        '
        Me.lblEdge.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblEdge.Location = New System.Drawing.Point(116, 80)
        Me.lblEdge.Name = "lblEdge"
        Me.lblEdge.Size = New System.Drawing.Size(85, 16)
        Me.lblEdge.TabIndex = 0
        '
        'lblInfPos
        '
        Me.lblInfPos.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblInfPos.Location = New System.Drawing.Point(116, 64)
        Me.lblInfPos.Name = "lblInfPos"
        Me.lblInfPos.Size = New System.Drawing.Size(85, 16)
        Me.lblInfPos.TabIndex = 0
        '
        'lblImgSize
        '
        Me.lblImgSize.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblImgSize.Location = New System.Drawing.Point(116, 48)
        Me.lblImgSize.Name = "lblImgSize"
        Me.lblImgSize.Size = New System.Drawing.Size(85, 16)
        Me.lblImgSize.TabIndex = 0
        '
        'lblImgPos
        '
        Me.lblImgPos.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblImgPos.Location = New System.Drawing.Point(116, 32)
        Me.lblImgPos.Name = "lblImgPos"
        Me.lblImgPos.Size = New System.Drawing.Size(85, 16)
        Me.lblImgPos.TabIndex = 0
        Me.lblImgPos.Text = "0"
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label5.Location = New System.Drawing.Point(4, 12)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(105, 16)
        Me.Label5.TabIndex = 0
        Me.Label5.Text = "読込フレームNo"
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Location = New System.Drawing.Point(5, 80)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(105, 16)
        Me.Label4.TabIndex = 0
        Me.Label4.Text = "エッジ情報 (1のみ)"
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Location = New System.Drawing.Point(5, 64)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(105, 16)
        Me.Label3.TabIndex = 0
        Me.Label3.Text = "付加情報ｵﾌｾｯﾄ"
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Location = New System.Drawing.Point(5, 48)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(105, 16)
        Me.Label2.TabIndex = 0
        Me.Label2.Text = "画像ｻｲｽﾞ"
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Location = New System.Drawing.Point(5, 32)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(105, 16)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "ﾌｧｲﾙ先頭ｵﾌｾｯﾄ"
        '
        'vsBar
        '
        Me.vsBar.Dock = System.Windows.Forms.DockStyle.Right
        Me.vsBar.LargeChange = 1
        Me.vsBar.Location = New System.Drawing.Point(763, 0)
        Me.vsBar.Name = "vsBar"
        Me.vsBar.Size = New System.Drawing.Size(15, 320)
        Me.vsBar.TabIndex = 1
        Me.vsBar.TabStop = True
        '
        'pnlHBar
        '
        Me.pnlHBar.Controls.Add(Me.hsBar)
        Me.pnlHBar.Controls.Add(Me.btnRituNon)
        Me.pnlHBar.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.pnlHBar.Location = New System.Drawing.Point(0, 320)
        Me.pnlHBar.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlHBar.Name = "pnlHBar"
        Me.pnlHBar.Size = New System.Drawing.Size(778, 12)
        Me.pnlHBar.TabIndex = 4
        '
        'hsBar
        '
        Me.hsBar.Dock = System.Windows.Forms.DockStyle.Fill
        Me.hsBar.LargeChange = 1
        Me.hsBar.Location = New System.Drawing.Point(0, 0)
        Me.hsBar.Maximum = 1
        Me.hsBar.Name = "hsBar"
        Me.hsBar.Size = New System.Drawing.Size(763, 12)
        Me.hsBar.TabIndex = 2
        '
        'btnRituNon
        '
        Me.btnRituNon.Dock = System.Windows.Forms.DockStyle.Right
        Me.btnRituNon.Font = New System.Drawing.Font("ＭＳ 明朝", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRituNon.Image = CType(resources.GetObject("btnRituNon.Image"), System.Drawing.Image)
        Me.btnRituNon.Location = New System.Drawing.Point(763, 0)
        Me.btnRituNon.Margin = New System.Windows.Forms.Padding(0)
        Me.btnRituNon.Name = "btnRituNon"
        Me.btnRituNon.Size = New System.Drawing.Size(15, 12)
        Me.btnRituNon.TabIndex = 3
        Me.btnRituNon.UseVisualStyleBackColor = True
        '
        'tlpRanp
        '
        Me.tlpRanp.BackColor = System.Drawing.SystemColors.Control
        Me.tlpRanp.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Outset
        Me.tlpRanp.ColumnCount = 10
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11185!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.10522!))
        Me.tlpRanp.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 83.0!))
        Me.tlpRanp.Controls.Add(Me.pnlLed1, 0, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed2, 1, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed3, 2, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed4, 3, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed5, 4, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed6, 5, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed7, 6, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed8, 7, 0)
        Me.tlpRanp.Controls.Add(Me.pnlLed9, 8, 0)
        Me.tlpRanp.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.tlpRanp.Location = New System.Drawing.Point(0, 332)
        Me.tlpRanp.Margin = New System.Windows.Forms.Padding(0)
        Me.tlpRanp.Name = "tlpRanp"
        Me.tlpRanp.RowCount = 1
        Me.tlpRanp.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpRanp.Size = New System.Drawing.Size(778, 10)
        Me.tlpRanp.TabIndex = 20
        '
        'pnlLed1
        '
        Me.pnlLed1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed1.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed1.Location = New System.Drawing.Point(2, 2)
        Me.pnlLed1.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed1.Name = "pnlLed1"
        Me.pnlLed1.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed1.TabIndex = 0
        '
        'pnlLed2
        '
        Me.pnlLed2.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed2.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed2.Location = New System.Drawing.Point(78, 2)
        Me.pnlLed2.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed2.Name = "pnlLed2"
        Me.pnlLed2.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed2.TabIndex = 1
        '
        'pnlLed3
        '
        Me.pnlLed3.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed3.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed3.Location = New System.Drawing.Point(154, 2)
        Me.pnlLed3.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed3.Name = "pnlLed3"
        Me.pnlLed3.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed3.TabIndex = 1
        '
        'pnlLed4
        '
        Me.pnlLed4.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed4.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed4.Location = New System.Drawing.Point(230, 2)
        Me.pnlLed4.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed4.Name = "pnlLed4"
        Me.pnlLed4.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed4.TabIndex = 1
        '
        'pnlLed5
        '
        Me.pnlLed5.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed5.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed5.Location = New System.Drawing.Point(306, 2)
        Me.pnlLed5.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed5.Name = "pnlLed5"
        Me.pnlLed5.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed5.TabIndex = 1
        '
        'pnlLed6
        '
        Me.pnlLed6.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed6.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed6.Location = New System.Drawing.Point(382, 2)
        Me.pnlLed6.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed6.Name = "pnlLed6"
        Me.pnlLed6.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed6.TabIndex = 1
        '
        'pnlLed7
        '
        Me.pnlLed7.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed7.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed7.Location = New System.Drawing.Point(458, 2)
        Me.pnlLed7.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed7.Name = "pnlLed7"
        Me.pnlLed7.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed7.TabIndex = 1
        '
        'pnlLed8
        '
        Me.pnlLed8.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed8.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed8.Location = New System.Drawing.Point(534, 2)
        Me.pnlLed8.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed8.Name = "pnlLed8"
        Me.pnlLed8.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed8.TabIndex = 1
        '
        'pnlLed9
        '
        Me.pnlLed9.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlLed9.BackColor = System.Drawing.SystemColors.ControlDark
        Me.pnlLed9.Location = New System.Drawing.Point(610, 2)
        Me.pnlLed9.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlLed9.Name = "pnlLed9"
        Me.pnlLed9.Size = New System.Drawing.Size(74, 6)
        Me.pnlLed9.TabIndex = 1
        '
        'tlsMainMenu
        '
        Me.tlsMainMenu.CanOverflow = False
        Me.tlsMainMenu.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.tlsMainMenu.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        Me.tlsMainMenu.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.tsbSearch, Me.tsbSearchUp, Me.tsbSearchDown, Me.tsbSearchOU, Me.tsbFolder, Me.ToolStripSeparator2, Me.tsbDspBig, Me.ToolStripSeparator6, Me.tsbSync, Me.tsbMemori, Me.tsbOVL, Me.tsbReSetOVL, Me.tsbGridLine, Me.tsbEdge, Me.ToolStripSeparator13, Me.tsbJump, Me.tsbBookmark, Me.tsbInputList, Me.tsbKiriInput, Me.ToolStripSeparator8, Me.tsbDspSet, Me.tsbBrite, Me.tsbSetting, Me.ToolStripSeparator9, Me.tsbCoilItem, Me.tsbKizuMap, Me.tsbKizuList, Me.tsbDefMap, Me.ToolStripSeparator4, Me.tsbDSWS, Me.tsbTB, Me.tsbRot, Me.ToolStripSeparator5, Me.tscRate, Me.tscSpeed, Me.tscChannel, Me.ToolStripSeparator10, Me.tspbReadRpi, Me.ToolStripSeparator1, Me.tsbPrint, Me.ToolStripSeparator3, Me.tsbInfMode, Me.tsbImgQuarty})
        Me.tlsMainMenu.Location = New System.Drawing.Point(0, 342)
        Me.tlsMainMenu.Name = "tlsMainMenu"
        Me.tlsMainMenu.RenderMode = System.Windows.Forms.ToolStripRenderMode.System
        Me.tlsMainMenu.Size = New System.Drawing.Size(987, 26)
        Me.tlsMainMenu.TabIndex = 6
        Me.tlsMainMenu.Text = "ToolStrip1"
        '
        'tsbSearch
        '
        Me.tsbSearch.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbSearch.Image = CType(resources.GetObject("tsbSearch.Image"), System.Drawing.Image)
        Me.tsbSearch.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSearch.Name = "tsbSearch"
        Me.tsbSearch.Size = New System.Drawing.Size(23, 23)
        Me.tsbSearch.Text = "F5:コイル検索"
        '
        'tsbSearchUp
        '
        Me.tsbSearchUp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbSearchUp.Image = CType(resources.GetObject("tsbSearchUp.Image"), System.Drawing.Image)
        Me.tsbSearchUp.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSearchUp.Name = "tsbSearchUp"
        Me.tsbSearchUp.Size = New System.Drawing.Size(23, 23)
        Me.tsbSearchUp.Text = "コイルリスト上へ"
        '
        'tsbSearchDown
        '
        Me.tsbSearchDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbSearchDown.Image = CType(resources.GetObject("tsbSearchDown.Image"), System.Drawing.Image)
        Me.tsbSearchDown.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSearchDown.Name = "tsbSearchDown"
        Me.tsbSearchDown.Size = New System.Drawing.Size(23, 23)
        Me.tsbSearchDown.Text = "コイルリスト下へ"
        '
        'tsbSearchOU
        '
        Me.tsbSearchOU.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbSearchOU.Image = CType(resources.GetObject("tsbSearchOU.Image"), System.Drawing.Image)
        Me.tsbSearchOU.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSearchOU.Name = "tsbSearchOU"
        Me.tsbSearchOU.Size = New System.Drawing.Size(23, 23)
        Me.tsbSearchOU.Text = "表裏反転"
        '
        'tsbFolder
        '
        Me.tsbFolder.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbFolder.Image = CType(resources.GetObject("tsbFolder.Image"), System.Drawing.Image)
        Me.tsbFolder.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbFolder.Name = "tsbFolder"
        Me.tsbFolder.Size = New System.Drawing.Size(23, 23)
        Me.tsbFolder.Text = "ToolStripButton1"
        Me.tsbFolder.ToolTipText = "RPI保存フォルダ表示"
        '
        'ToolStripSeparator2
        '
        Me.ToolStripSeparator2.Name = "ToolStripSeparator2"
        Me.ToolStripSeparator2.Size = New System.Drawing.Size(6, 26)
        '
        'tsbDspBig
        '
        Me.tsbDspBig.Checked = True
        Me.tsbDspBig.CheckOnClick = True
        Me.tsbDspBig.CheckState = System.Windows.Forms.CheckState.Checked
        Me.tsbDspBig.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbDspBig.Image = CType(resources.GetObject("tsbDspBig.Image"), System.Drawing.Image)
        Me.tsbDspBig.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbDspBig.Name = "tsbDspBig"
        Me.tsbDspBig.Size = New System.Drawing.Size(23, 23)
        Me.tsbDspBig.Text = "虫眼鏡(表示中に左ｸﾘｯｸでｸﾘｯﾌﾟﾎﾞｰﾄﾞへｺﾋﾟｰ)"
        '
        'ToolStripSeparator6
        '
        Me.ToolStripSeparator6.Name = "ToolStripSeparator6"
        Me.ToolStripSeparator6.Size = New System.Drawing.Size(6, 26)
        '
        'tsbSync
        '
        Me.tsbSync.CheckOnClick = True
        Me.tsbSync.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbSync.Image = CType(resources.GetObject("tsbSync.Image"), System.Drawing.Image)
        Me.tsbSync.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSync.Name = "tsbSync"
        Me.tsbSync.Size = New System.Drawing.Size(23, 23)
        Me.tsbSync.Text = "複数連動"
        '
        'tsbMemori
        '
        Me.tsbMemori.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbMemori.Image = CType(resources.GetObject("tsbMemori.Image"), System.Drawing.Image)
        Me.tsbMemori.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbMemori.Name = "tsbMemori"
        Me.tsbMemori.Size = New System.Drawing.Size(23, 23)
        Me.tsbMemori.Text = "ToolStripButton1"
        Me.tsbMemori.ToolTipText = "メモリ表示切替"
        '
        'tsbOVL
        '
        Me.tsbOVL.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbOVL.Image = CType(resources.GetObject("tsbOVL.Image"), System.Drawing.Image)
        Me.tsbOVL.ImageTransparentColor = System.Drawing.Color.Black
        Me.tsbOVL.Name = "tsbOVL"
        Me.tsbOVL.Size = New System.Drawing.Size(23, 23)
        Me.tsbOVL.Text = "ToolStripButton1"
        Me.tsbOVL.ToolTipText = "オーバーレイ表示"
        '
        'tsbReSetOVL
        '
        Me.tsbReSetOVL.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbReSetOVL.Image = CType(resources.GetObject("tsbReSetOVL.Image"), System.Drawing.Image)
        Me.tsbReSetOVL.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbReSetOVL.Name = "tsbReSetOVL"
        Me.tsbReSetOVL.Size = New System.Drawing.Size(23, 23)
        Me.tsbReSetOVL.Text = "オーバーレイ手動補正"
        Me.tsbReSetOVL.ToolTipText = "オーバーレイ位置手動調整"
        '
        'tsbGridLine
        '
        Me.tsbGridLine.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbGridLine.Image = CType(resources.GetObject("tsbGridLine.Image"), System.Drawing.Image)
        Me.tsbGridLine.ImageTransparentColor = System.Drawing.Color.Black
        Me.tsbGridLine.Name = "tsbGridLine"
        Me.tsbGridLine.Size = New System.Drawing.Size(23, 23)
        Me.tsbGridLine.Text = "グリッド線"
        '
        'tsbEdge
        '
        Me.tsbEdge.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbEdge.Image = CType(resources.GetObject("tsbEdge.Image"), System.Drawing.Image)
        Me.tsbEdge.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbEdge.Name = "tsbEdge"
        Me.tsbEdge.Size = New System.Drawing.Size(23, 23)
        Me.tsbEdge.Text = "ToolStripButton1"
        Me.tsbEdge.ToolTipText = "エッジ位置"
        '
        'ToolStripSeparator13
        '
        Me.ToolStripSeparator13.Name = "ToolStripSeparator13"
        Me.ToolStripSeparator13.Size = New System.Drawing.Size(6, 26)
        '
        'tsbJump
        '
        Me.tsbJump.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbJump.Image = CType(resources.GetObject("tsbJump.Image"), System.Drawing.Image)
        Me.tsbJump.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbJump.Name = "tsbJump"
        Me.tsbJump.Size = New System.Drawing.Size(23, 23)
        Me.tsbJump.Text = "ToolStripButton1"
        Me.tsbJump.ToolTipText = "ジャンプ、自動送り等"
        '
        'tsbBookmark
        '
        Me.tsbBookmark.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbBookmark.Image = CType(resources.GetObject("tsbBookmark.Image"), System.Drawing.Image)
        Me.tsbBookmark.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbBookmark.Name = "tsbBookmark"
        Me.tsbBookmark.Size = New System.Drawing.Size(23, 23)
        Me.tsbBookmark.Text = "ブックマーク"
        Me.tsbBookmark.ToolTipText = "ブックマーク"
        '
        'tsbInputList
        '
        Me.tsbInputList.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbInputList.Image = CType(resources.GetObject("tsbInputList.Image"), System.Drawing.Image)
        Me.tsbInputList.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbInputList.Name = "tsbInputList"
        Me.tsbInputList.Size = New System.Drawing.Size(23, 23)
        Me.tsbInputList.ToolTipText = "選択位置連携"
        '
        'tsbKiriInput
        '
        Me.tsbKiriInput.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbKiriInput.Image = CType(resources.GetObject("tsbKiriInput.Image"), System.Drawing.Image)
        Me.tsbKiriInput.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbKiriInput.Name = "tsbKiriInput"
        Me.tsbKiriInput.Size = New System.Drawing.Size(23, 23)
        Me.tsbKiriInput.Text = "ToolStripButton1"
        Me.tsbKiriInput.ToolTipText = "任意選択"
        '
        'ToolStripSeparator8
        '
        Me.ToolStripSeparator8.Name = "ToolStripSeparator8"
        Me.ToolStripSeparator8.Size = New System.Drawing.Size(6, 26)
        '
        'tsbDspSet
        '
        Me.tsbDspSet.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbDspSet.Image = CType(resources.GetObject("tsbDspSet.Image"), System.Drawing.Image)
        Me.tsbDspSet.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbDspSet.Name = "tsbDspSet"
        Me.tsbDspSet.Size = New System.Drawing.Size(23, 23)
        Me.tsbDspSet.Text = "表示設定"
        '
        'tsbBrite
        '
        Me.tsbBrite.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbBrite.Image = CType(resources.GetObject("tsbBrite.Image"), System.Drawing.Image)
        Me.tsbBrite.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbBrite.Name = "tsbBrite"
        Me.tsbBrite.Size = New System.Drawing.Size(23, 23)
        Me.tsbBrite.Text = "ToolStripButton1"
        Me.tsbBrite.ToolTipText = "輝度調整"
        '
        'tsbSetting
        '
        Me.tsbSetting.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbSetting.Image = CType(resources.GetObject("tsbSetting.Image"), System.Drawing.Image)
        Me.tsbSetting.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbSetting.Name = "tsbSetting"
        Me.tsbSetting.Size = New System.Drawing.Size(23, 23)
        Me.tsbSetting.Text = "ToolStripButton1"
        Me.tsbSetting.ToolTipText = "各種設定"
        Me.tsbSetting.Visible = False
        '
        'ToolStripSeparator9
        '
        Me.ToolStripSeparator9.Name = "ToolStripSeparator9"
        Me.ToolStripSeparator9.Size = New System.Drawing.Size(6, 26)
        '
        'tsbCoilItem
        '
        Me.tsbCoilItem.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbCoilItem.Image = CType(resources.GetObject("tsbCoilItem.Image"), System.Drawing.Image)
        Me.tsbCoilItem.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbCoilItem.Name = "tsbCoilItem"
        Me.tsbCoilItem.Size = New System.Drawing.Size(23, 23)
        Me.tsbCoilItem.Text = "コイル情報"
        '
        'tsbKizuMap
        '
        Me.tsbKizuMap.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbKizuMap.Image = CType(resources.GetObject("tsbKizuMap.Image"), System.Drawing.Image)
        Me.tsbKizuMap.ImageTransparentColor = System.Drawing.Color.Black
        Me.tsbKizuMap.Name = "tsbKizuMap"
        Me.tsbKizuMap.Size = New System.Drawing.Size(23, 23)
        Me.tsbKizuMap.Text = "疵マップ"
        '
        'tsbKizuList
        '
        Me.tsbKizuList.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbKizuList.Image = CType(resources.GetObject("tsbKizuList.Image"), System.Drawing.Image)
        Me.tsbKizuList.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbKizuList.Name = "tsbKizuList"
        Me.tsbKizuList.Size = New System.Drawing.Size(23, 23)
        Me.tsbKizuList.Text = "疵一覧"
        '
        'tsbDefMap
        '
        Me.tsbDefMap.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbDefMap.Image = CType(resources.GetObject("tsbDefMap.Image"), System.Drawing.Image)
        Me.tsbDefMap.ImageTransparentColor = System.Drawing.Color.Black
        Me.tsbDefMap.Name = "tsbDefMap"
        Me.tsbDefMap.Size = New System.Drawing.Size(23, 23)
        Me.tsbDefMap.Text = "散布図"
        '
        'ToolStripSeparator4
        '
        Me.ToolStripSeparator4.Name = "ToolStripSeparator4"
        Me.ToolStripSeparator4.Size = New System.Drawing.Size(6, 26)
        '
        'tsbDSWS
        '
        Me.tsbDSWS.CheckOnClick = True
        Me.tsbDSWS.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbDSWS.Image = CType(resources.GetObject("tsbDSWS.Image"), System.Drawing.Image)
        Me.tsbDSWS.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbDSWS.Name = "tsbDSWS"
        Me.tsbDSWS.Size = New System.Drawing.Size(23, 23)
        Me.tsbDSWS.Text = "ToolStripButton1"
        Me.tsbDSWS.ToolTipText = "DSWS反転"
        '
        'tsbTB
        '
        Me.tsbTB.CheckOnClick = True
        Me.tsbTB.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbTB.Image = CType(resources.GetObject("tsbTB.Image"), System.Drawing.Image)
        Me.tsbTB.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbTB.Name = "tsbTB"
        Me.tsbTB.Size = New System.Drawing.Size(23, 23)
        Me.tsbTB.Text = "ToolStripButton1"
        Me.tsbTB.ToolTipText = "先端、尾端反転"
        '
        'tsbRot
        '
        Me.tsbRot.CheckOnClick = True
        Me.tsbRot.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbRot.Image = CType(resources.GetObject("tsbRot.Image"), System.Drawing.Image)
        Me.tsbRot.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None
        Me.tsbRot.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbRot.Name = "tsbRot"
        Me.tsbRot.Size = New System.Drawing.Size(23, 23)
        Me.tsbRot.Text = "ToolStripButton1"
        Me.tsbRot.ToolTipText = "横倒し"
        '
        'ToolStripSeparator5
        '
        Me.ToolStripSeparator5.Name = "ToolStripSeparator5"
        Me.ToolStripSeparator5.Size = New System.Drawing.Size(6, 26)
        '
        'tscRate
        '
        Me.tscRate.AutoSize = False
        Me.tscRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.tscRate.DropDownWidth = 80
        Me.tscRate.Items.AddRange(New Object() {"全幅表示", "元画像1/4倍", "元画像1/2倍", "元画像1倍", "元画像2倍", "元画像3倍", "元画像4倍", "元画像5倍"})
        Me.tscRate.Name = "tscRate"
        Me.tscRate.Size = New System.Drawing.Size(100, 23)
        '
        'tscSpeed
        '
        Me.tscSpeed.AutoSize = False
        Me.tscSpeed.DropDownWidth = 30
        Me.tscSpeed.Items.AddRange(New Object() {"500", "300", "250", "200", "150", "120", "90", "60", "30", "10", "0", "-10", "-30", "-60", "-120", "-150", "-200", "-250", "-300", "-500"})
        Me.tscSpeed.Name = "tscSpeed"
        Me.tscSpeed.Size = New System.Drawing.Size(50, 23)
        '
        'tscChannel
        '
        Me.tscChannel.AutoSize = False
        Me.tscChannel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.tscChannel.DropDownWidth = 30
        Me.tscChannel.Items.AddRange(New Object() {"0ch"})
        Me.tscChannel.Name = "tscChannel"
        Me.tscChannel.Size = New System.Drawing.Size(40, 23)
        '
        'ToolStripSeparator10
        '
        Me.ToolStripSeparator10.Name = "ToolStripSeparator10"
        Me.ToolStripSeparator10.Size = New System.Drawing.Size(6, 26)
        '
        'tspbReadRpi
        '
        Me.tspbReadRpi.Name = "tspbReadRpi"
        Me.tspbReadRpi.Size = New System.Drawing.Size(30, 23)
        Me.tspbReadRpi.Step = 1
        Me.tspbReadRpi.Style = System.Windows.Forms.ProgressBarStyle.Continuous
        Me.tspbReadRpi.ToolTipText = "RPI非同期読込(赤：読込中)"
        '
        'ToolStripSeparator1
        '
        Me.ToolStripSeparator1.Name = "ToolStripSeparator1"
        Me.ToolStripSeparator1.Size = New System.Drawing.Size(6, 26)
        '
        'tsbPrint
        '
        Me.tsbPrint.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbPrint.Image = CType(resources.GetObject("tsbPrint.Image"), System.Drawing.Image)
        Me.tsbPrint.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbPrint.Name = "tsbPrint"
        Me.tsbPrint.Size = New System.Drawing.Size(23, 23)
        Me.tsbPrint.Text = "出力設定"
        Me.tsbPrint.ToolTipText = "画像出力/保存"
        '
        'ToolStripSeparator3
        '
        Me.ToolStripSeparator3.Name = "ToolStripSeparator3"
        Me.ToolStripSeparator3.Size = New System.Drawing.Size(6, 26)
        '
        'tsbInfMode
        '
        Me.tsbInfMode.CheckOnClick = True
        Me.tsbInfMode.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbInfMode.Image = CType(resources.GetObject("tsbInfMode.Image"), System.Drawing.Image)
        Me.tsbInfMode.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbInfMode.Name = "tsbInfMode"
        Me.tsbInfMode.Size = New System.Drawing.Size(23, 23)
        Me.tsbInfMode.Text = "ToolStripButton1"
        Me.tsbInfMode.ToolTipText = "RPI情報エリア 表示／非表示 切替"
        '
        'tsbImgQuarty
        '
        Me.tsbImgQuarty.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        Me.tsbImgQuarty.Image = CType(resources.GetObject("tsbImgQuarty.Image"), System.Drawing.Image)
        Me.tsbImgQuarty.ImageTransparentColor = System.Drawing.Color.Magenta
        Me.tsbImgQuarty.Name = "tsbImgQuarty"
        Me.tsbImgQuarty.Size = New System.Drawing.Size(23, 23)
        Me.tsbImgQuarty.Text = "開発用"
        '
        'timDsp
        '
        '
        'timThreadRpiRead
        '
        Me.timThreadRpiRead.Interval = 500
        '
        'tlsMainMenu2
        '
        Me.tlsMainMenu2.CanOverflow = False
        Me.tlsMainMenu2.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.tlsMainMenu2.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden
        Me.tlsMainMenu2.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.ToolStripLabel7, Me.tslFrameNowCnt, Me.ToolStripLabel8, Me.tslFrameAllCnt, Me.ToolStripSeparator7, Me.ToolStripLabel9, Me.tslTotalPixel, Me.ToolStripSeparator11, Me.ToolStripLabel11, Me.tslTotalMm, Me.ToolStripLabel1, Me.ToolStripSeparator12, Me.tslFPS, Me.ToolStripLabel3})
        Me.tlsMainMenu2.Location = New System.Drawing.Point(0, 368)
        Me.tlsMainMenu2.Name = "tlsMainMenu2"
        Me.tlsMainMenu2.RenderMode = System.Windows.Forms.ToolStripRenderMode.System
        Me.tlsMainMenu2.Size = New System.Drawing.Size(987, 25)
        Me.tlsMainMenu2.TabIndex = 7
        Me.tlsMainMenu2.Text = "ToolStrip1"
        '
        'ToolStripLabel7
        '
        Me.ToolStripLabel7.Name = "ToolStripLabel7"
        Me.ToolStripLabel7.Size = New System.Drawing.Size(55, 22)
        Me.ToolStripLabel7.Text = "Frame="
        '
        'tslFrameNowCnt
        '
        Me.tslFrameNowCnt.Name = "tslFrameNowCnt"
        Me.tslFrameNowCnt.Size = New System.Drawing.Size(14, 22)
        Me.tslFrameNowCnt.Text = "0"
        '
        'ToolStripLabel8
        '
        Me.ToolStripLabel8.Name = "ToolStripLabel8"
        Me.ToolStripLabel8.Size = New System.Drawing.Size(12, 22)
        Me.ToolStripLabel8.Text = "/"
        '
        'tslFrameAllCnt
        '
        Me.tslFrameAllCnt.Name = "tslFrameAllCnt"
        Me.tslFrameAllCnt.Size = New System.Drawing.Size(14, 22)
        Me.tslFrameAllCnt.Text = "0"
        '
        'ToolStripSeparator7
        '
        Me.ToolStripSeparator7.Name = "ToolStripSeparator7"
        Me.ToolStripSeparator7.Size = New System.Drawing.Size(6, 25)
        '
        'ToolStripLabel9
        '
        Me.ToolStripLabel9.Name = "ToolStripLabel9"
        Me.ToolStripLabel9.Size = New System.Drawing.Size(96, 22)
        Me.ToolStripLabel9.Text = "元画像PixelXY="
        '
        'tslTotalPixel
        '
        Me.tslTotalPixel.Name = "tslTotalPixel"
        Me.tslTotalPixel.Size = New System.Drawing.Size(30, 22)
        Me.tslTotalPixel.Text = "0/ 0"
        '
        'ToolStripSeparator11
        '
        Me.ToolStripSeparator11.Name = "ToolStripSeparator11"
        Me.ToolStripSeparator11.Size = New System.Drawing.Size(6, 25)
        '
        'ToolStripLabel11
        '
        Me.ToolStripLabel11.Name = "ToolStripLabel11"
        Me.ToolStripLabel11.Size = New System.Drawing.Size(41, 22)
        Me.ToolStripLabel11.Text = "距離="
        '
        'tslTotalMm
        '
        Me.tslTotalMm.Name = "tslTotalMm"
        Me.tslTotalMm.Size = New System.Drawing.Size(80, 22)
        Me.tslTotalMm.Text = "0.000/ 0.000"
        '
        'ToolStripLabel1
        '
        Me.ToolStripLabel1.Name = "ToolStripLabel1"
        Me.ToolStripLabel1.Size = New System.Drawing.Size(29, 22)
        Me.ToolStripLabel1.Text = "[m]"
        '
        'ToolStripSeparator12
        '
        Me.ToolStripSeparator12.Name = "ToolStripSeparator12"
        Me.ToolStripSeparator12.Size = New System.Drawing.Size(6, 25)
        '
        'tslFPS
        '
        Me.tslFPS.Name = "tslFPS"
        Me.tslFPS.Size = New System.Drawing.Size(15, 22)
        Me.tslFPS.Text = "0"
        '
        'ToolStripLabel3
        '
        Me.ToolStripLabel3.Name = "ToolStripLabel3"
        Me.ToolStripLabel3.Size = New System.Drawing.Size(25, 22)
        Me.ToolStripLabel3.Text = "fps"
        '
        'ToolStripSeparator14
        '
        Me.ToolStripSeparator14.Name = "ToolStripSeparator14"
        Me.ToolStripSeparator14.Size = New System.Drawing.Size(6, 26)
        '
        'frmMain
        '
        Me.AllowDrop = True
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(987, 393)
        Me.Controls.Add(Me.spcMain)
        Me.Controls.Add(Me.tlsMainMenu)
        Me.Controls.Add(Me.tlsMainMenu2)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.KeyPreview = True
        Me.Name = "frmMain"
        Me.Text = "RPI表示ツール"
        CType(Me.picFrameImg, System.ComponentModel.ISupportInitialize).EndInit()
        Me.spcMain.Panel1.ResumeLayout(False)
        Me.spcMain.Panel2.ResumeLayout(False)
        Me.spcMain.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox1.ResumeLayout(False)
        CType(Me.nudFrameNo, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlHBar.ResumeLayout(False)
        Me.tlpRanp.ResumeLayout(False)
        Me.tlsMainMenu.ResumeLayout(False)
        Me.tlsMainMenu.PerformLayout()
        Me.tlsMainMenu2.ResumeLayout(False)
        Me.tlsMainMenu2.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents picFrameImg As System.Windows.Forms.PictureBox
    Friend WithEvents spcMain As System.Windows.Forms.SplitContainer
    Private WithEvents vsBar As tClassLibrary.tCnt.tCnt_VScrollBar
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents tlsMainMenu As System.Windows.Forms.ToolStrip
    Friend WithEvents tsbInfMode As System.Windows.Forms.ToolStripButton
    Friend WithEvents lsvRpiHeader As System.Windows.Forms.ListView
    Friend WithEvents ColumnHeader1 As System.Windows.Forms.ColumnHeader
    Friend WithEvents ColumnHeader2 As System.Windows.Forms.ColumnHeader
    Friend WithEvents ToolStripSeparator2 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents lblEdge As System.Windows.Forms.Label
    Friend WithEvents lblInfPos As System.Windows.Forms.Label
    Friend WithEvents lblImgSize As System.Windows.Forms.Label
    Friend WithEvents lblImgPos As System.Windows.Forms.Label
    Friend WithEvents pnlHBar As System.Windows.Forms.Panel
    Friend WithEvents btnRituNon As System.Windows.Forms.Button
    Friend WithEvents hsBar As tClassLibrary.tCnt.tCnt_HScrollBar
    Friend WithEvents tscRate As System.Windows.Forms.ToolStripComboBox
    Friend WithEvents tsbMemori As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripSeparator3 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents nudFrameNo As System.Windows.Forms.NumericUpDown
    Friend WithEvents ToolStripSeparator6 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents ToolStripSeparator4 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents ToolStripSeparator5 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents tsbTB As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripSeparator9 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents btnSave As System.Windows.Forms.Button
    Friend WithEvents timDsp As System.Windows.Forms.Timer
    Friend WithEvents tscSpeed As System.Windows.Forms.ToolStripComboBox
    Friend WithEvents tscChannel As System.Windows.Forms.ToolStripComboBox
    Friend WithEvents ToolStripSeparator10 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents tspbReadRpi As System.Windows.Forms.ToolStripProgressBar
    Friend WithEvents timThreadRpiRead As System.Windows.Forms.Timer
    Friend WithEvents tsbFolder As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbRot As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbDSWS As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbBrite As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbImgQuarty As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbDefMap As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbOVL As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbSearch As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbEdge As System.Windows.Forms.ToolStripButton
    Friend WithEvents tlsMainMenu2 As System.Windows.Forms.ToolStrip
    Friend WithEvents ToolStripLabel7 As System.Windows.Forms.ToolStripLabel
    Friend WithEvents tslFrameNowCnt As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ToolStripLabel8 As System.Windows.Forms.ToolStripLabel
    Friend WithEvents tslFrameAllCnt As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ToolStripSeparator7 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents ToolStripLabel9 As System.Windows.Forms.ToolStripLabel
    Friend WithEvents tslTotalPixel As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ToolStripSeparator11 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents ToolStripLabel11 As System.Windows.Forms.ToolStripLabel
    Friend WithEvents tslTotalMm As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ToolStripLabel1 As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ToolStripSeparator12 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents tslFPS As System.Windows.Forms.ToolStripLabel
    Friend WithEvents ToolStripLabel3 As System.Windows.Forms.ToolStripLabel
    Friend WithEvents tsbGridLine As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbKizuMap As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbDspBig As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbKizuList As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbCoilItem As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbDspSet As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbPrint As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbSearchUp As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbSearchDown As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripSeparator1 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents tsbSearchOU As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbJump As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbReSetOVL As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbSync As System.Windows.Forms.ToolStripButton
    Friend WithEvents tlpRanp As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents pnlLed1 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed2 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed3 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed4 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed5 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed6 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed7 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed8 As System.Windows.Forms.Panel
    Friend WithEvents pnlLed9 As System.Windows.Forms.Panel
    Friend WithEvents tsbKiriInput As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbBookmark As System.Windows.Forms.ToolStripButton
    Friend WithEvents tsbSetting As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripSeparator13 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents tsbInputList As System.Windows.Forms.ToolStripButton
    Friend WithEvents ToolStripSeparator8 As System.Windows.Forms.ToolStripSeparator
    Friend WithEvents ToolStripSeparator14 As ToolStripSeparator

End Class
