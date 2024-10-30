<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ucDefect
    Inherits System.Windows.Forms.UserControl

    'UserControl はコンポーネント一覧をクリーンアップするために dispose をオーバーライドします。
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
		Me.tblLayoutPnl = New System.Windows.Forms.TableLayoutPanel()
		Me.dgvDefInfo = New tClassLibrary.tCnt.tCnt_DataGridView()
		Me.picImg2 = New System.Windows.Forms.PictureBox()
		Me.picImg1 = New System.Windows.Forms.PictureBox()
		Me.pnlImgInfo1 = New System.Windows.Forms.Panel()
		Me.lblCamNo = New System.Windows.Forms.Label()
		Me.lblImgRight01 = New System.Windows.Forms.Label()
		Me.lblDefTitle = New System.Windows.Forms.Label()
		Me.lblImgLeft01 = New System.Windows.Forms.Label()
		Me.lblImg01 = New System.Windows.Forms.Label()
		Me.pnlImgInfo2 = New System.Windows.Forms.Panel()
		Me.lblImgRight02 = New System.Windows.Forms.Label()
		Me.lblImgLeft02 = New System.Windows.Forms.Label()
		Me.lblImg02 = New System.Windows.Forms.Label()
		Me.pnlTyGrInfo = New System.Windows.Forms.Panel()
		Me.lblType = New System.Windows.Forms.Label()
		Me.lblGrade = New System.Windows.Forms.Label()
		Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
		Me.lblKizuNo = New System.Windows.Forms.Label()
		Me.lblYName = New System.Windows.Forms.Label()
		Me.lblY = New System.Windows.Forms.Label()
		Me.lblKizuNoName = New System.Windows.Forms.Label()
		Me.tblLayoutPnl.SuspendLayout()
		CType(Me.dgvDefInfo, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.picImg2, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.picImg1, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.pnlImgInfo1.SuspendLayout()
		Me.pnlImgInfo2.SuspendLayout()
		Me.pnlTyGrInfo.SuspendLayout()
		Me.TableLayoutPanel1.SuspendLayout()
		Me.SuspendLayout()
		'
		'tblLayoutPnl
		'
		Me.tblLayoutPnl.ColumnCount = 1
		Me.tblLayoutPnl.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
		Me.tblLayoutPnl.Controls.Add(Me.dgvDefInfo, 0, 6)
		Me.tblLayoutPnl.Controls.Add(Me.picImg2, 0, 3)
		Me.tblLayoutPnl.Controls.Add(Me.picImg1, 0, 1)
		Me.tblLayoutPnl.Controls.Add(Me.pnlImgInfo1, 0, 0)
		Me.tblLayoutPnl.Controls.Add(Me.pnlImgInfo2, 0, 2)
		Me.tblLayoutPnl.Controls.Add(Me.pnlTyGrInfo, 0, 4)
		Me.tblLayoutPnl.Controls.Add(Me.TableLayoutPanel1, 0, 5)
		Me.tblLayoutPnl.Dock = System.Windows.Forms.DockStyle.Fill
		Me.tblLayoutPnl.Location = New System.Drawing.Point(0, 0)
		Me.tblLayoutPnl.Margin = New System.Windows.Forms.Padding(0)
		Me.tblLayoutPnl.Name = "tblLayoutPnl"
		Me.tblLayoutPnl.RowCount = 7
		Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 28.0!))
		Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
		Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 28.0!))
		Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
		Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
		Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 36.0!))
		Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 92.0!))
		Me.tblLayoutPnl.Size = New System.Drawing.Size(323, 862)
		Me.tblLayoutPnl.TabIndex = 220
		'
		'dgvDefInfo
		'
		Me.dgvDefInfo.Dock = System.Windows.Forms.DockStyle.Fill
		Me.dgvDefInfo.Location = New System.Drawing.Point(0, 770)
		Me.dgvDefInfo.Margin = New System.Windows.Forms.Padding(0)
		Me.dgvDefInfo.Name = "dgvDefInfo"
		Me.dgvDefInfo.RowTemplate.Height = 21
		Me.dgvDefInfo.Size = New System.Drawing.Size(323, 92)
		Me.dgvDefInfo.TabIndex = 58
		'
		'picImg2
		'
		Me.picImg2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.picImg2.Dock = System.Windows.Forms.DockStyle.Fill
		Me.picImg2.Location = New System.Drawing.Point(0, 379)
		Me.picImg2.Margin = New System.Windows.Forms.Padding(0)
		Me.picImg2.Name = "picImg2"
		Me.picImg2.Size = New System.Drawing.Size(323, 323)
		Me.picImg2.TabIndex = 49
		Me.picImg2.TabStop = False
		'
		'picImg1
		'
		Me.picImg1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.picImg1.Dock = System.Windows.Forms.DockStyle.Fill
		Me.picImg1.Location = New System.Drawing.Point(0, 28)
		Me.picImg1.Margin = New System.Windows.Forms.Padding(0)
		Me.picImg1.Name = "picImg1"
		Me.picImg1.Size = New System.Drawing.Size(323, 323)
		Me.picImg1.TabIndex = 45
		Me.picImg1.TabStop = False
		'
		'pnlImgInfo1
		'
		Me.pnlImgInfo1.Controls.Add(Me.lblCamNo)
		Me.pnlImgInfo1.Controls.Add(Me.lblImgRight01)
		Me.pnlImgInfo1.Controls.Add(Me.lblDefTitle)
		Me.pnlImgInfo1.Controls.Add(Me.lblImgLeft01)
		Me.pnlImgInfo1.Controls.Add(Me.lblImg01)
		Me.pnlImgInfo1.Dock = System.Windows.Forms.DockStyle.Fill
		Me.pnlImgInfo1.Location = New System.Drawing.Point(0, 0)
		Me.pnlImgInfo1.Margin = New System.Windows.Forms.Padding(0)
		Me.pnlImgInfo1.Name = "pnlImgInfo1"
		Me.pnlImgInfo1.Size = New System.Drawing.Size(323, 28)
		Me.pnlImgInfo1.TabIndex = 55
		'
		'lblCamNo
		'
		Me.lblCamNo.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblCamNo.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.lblCamNo.Font = New System.Drawing.Font("ＭＳ 明朝", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblCamNo.ForeColor = System.Drawing.Color.FromArgb(CType(CType(0, Byte), Integer), CType(CType(0, Byte), Integer), CType(CType(192, Byte), Integer))
		Me.lblCamNo.Location = New System.Drawing.Point(151, 3)
		Me.lblCamNo.Margin = New System.Windows.Forms.Padding(0)
		Me.lblCamNo.Name = "lblCamNo"
		Me.lblCamNo.Size = New System.Drawing.Size(18, 22)
		Me.lblCamNo.TabIndex = 54
		Me.lblCamNo.Text = "0"
		Me.lblCamNo.TextAlign = System.Drawing.ContentAlignment.MiddleRight
		'
		'lblImgRight01
		'
		Me.lblImgRight01.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblImgRight01.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.lblImgRight01.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblImgRight01.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblImgRight01.ForeColor = System.Drawing.Color.Black
		Me.lblImgRight01.Location = New System.Drawing.Point(282, 0)
		Me.lblImgRight01.Margin = New System.Windows.Forms.Padding(0)
		Me.lblImgRight01.Name = "lblImgRight01"
		Me.lblImgRight01.Size = New System.Drawing.Size(40, 28)
		Me.lblImgRight01.TabIndex = 52
		Me.lblImgRight01.Text = "WS"
		Me.lblImgRight01.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'lblDefTitle
		'
		Me.lblDefTitle.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblDefTitle.BackColor = System.Drawing.Color.Transparent
		Me.lblDefTitle.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblDefTitle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblDefTitle.ForeColor = System.Drawing.Color.FromArgb(CType(CType(0, Byte), Integer), CType(CType(0, Byte), Integer), CType(CType(192, Byte), Integer))
		Me.lblDefTitle.Location = New System.Drawing.Point(169, 0)
		Me.lblDefTitle.Margin = New System.Windows.Forms.Padding(0)
		Me.lblDefTitle.Name = "lblDefTitle"
		Me.lblDefTitle.Size = New System.Drawing.Size(114, 28)
		Me.lblDefTitle.TabIndex = 46
		Me.lblDefTitle.Text = "最新ああああ"
		Me.lblDefTitle.TextAlign = System.Drawing.ContentAlignment.MiddleRight
		'
		'lblImgLeft01
		'
		Me.lblImgLeft01.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
		Me.lblImgLeft01.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.lblImgLeft01.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblImgLeft01.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblImgLeft01.ForeColor = System.Drawing.Color.Black
		Me.lblImgLeft01.Location = New System.Drawing.Point(0, 0)
		Me.lblImgLeft01.Margin = New System.Windows.Forms.Padding(0)
		Me.lblImgLeft01.Name = "lblImgLeft01"
		Me.lblImgLeft01.Size = New System.Drawing.Size(40, 28)
		Me.lblImgLeft01.TabIndex = 50
		Me.lblImgLeft01.Text = "DS"
		Me.lblImgLeft01.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'lblImg01
		'
		Me.lblImg01.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Left) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblImg01.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.lblImg01.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblImg01.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblImg01.ForeColor = System.Drawing.Color.Black
		Me.lblImg01.Location = New System.Drawing.Point(40, 0)
		Me.lblImg01.Margin = New System.Windows.Forms.Padding(0)
		Me.lblImg01.Name = "lblImg01"
		Me.lblImg01.Size = New System.Drawing.Size(243, 28)
		Me.lblImg01.TabIndex = 51
		Me.lblImg01.Text = "輝度画像"
		Me.lblImg01.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'pnlImgInfo2
		'
		Me.pnlImgInfo2.Controls.Add(Me.lblImgRight02)
		Me.pnlImgInfo2.Controls.Add(Me.lblImgLeft02)
		Me.pnlImgInfo2.Controls.Add(Me.lblImg02)
		Me.pnlImgInfo2.Dock = System.Windows.Forms.DockStyle.Fill
		Me.pnlImgInfo2.Location = New System.Drawing.Point(0, 351)
		Me.pnlImgInfo2.Margin = New System.Windows.Forms.Padding(0)
		Me.pnlImgInfo2.Name = "pnlImgInfo2"
		Me.pnlImgInfo2.Size = New System.Drawing.Size(323, 28)
		Me.pnlImgInfo2.TabIndex = 56
		'
		'lblImgRight02
		'
		Me.lblImgRight02.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblImgRight02.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.lblImgRight02.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblImgRight02.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblImgRight02.ForeColor = System.Drawing.Color.Black
		Me.lblImgRight02.Location = New System.Drawing.Point(282, 0)
		Me.lblImgRight02.Margin = New System.Windows.Forms.Padding(0)
		Me.lblImgRight02.Name = "lblImgRight02"
		Me.lblImgRight02.Size = New System.Drawing.Size(40, 28)
		Me.lblImgRight02.TabIndex = 55
		Me.lblImgRight02.Text = "WS"
		Me.lblImgRight02.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'lblImgLeft02
		'
		Me.lblImgLeft02.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
		Me.lblImgLeft02.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.lblImgLeft02.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblImgLeft02.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblImgLeft02.ForeColor = System.Drawing.Color.Black
		Me.lblImgLeft02.Location = New System.Drawing.Point(0, 0)
		Me.lblImgLeft02.Margin = New System.Windows.Forms.Padding(0)
		Me.lblImgLeft02.Name = "lblImgLeft02"
		Me.lblImgLeft02.Size = New System.Drawing.Size(40, 28)
		Me.lblImgLeft02.TabIndex = 53
		Me.lblImgLeft02.Text = "DS"
		Me.lblImgLeft02.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'lblImg02
		'
		Me.lblImg02.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Left) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblImg02.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
		Me.lblImg02.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblImg02.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblImg02.ForeColor = System.Drawing.Color.Black
		Me.lblImg02.Location = New System.Drawing.Point(40, 0)
		Me.lblImg02.Margin = New System.Windows.Forms.Padding(0)
		Me.lblImg02.Name = "lblImg02"
		Me.lblImg02.Size = New System.Drawing.Size(243, 28)
		Me.lblImg02.TabIndex = 54
		Me.lblImg02.Text = "差画像"
		Me.lblImg02.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'pnlTyGrInfo
		'
		Me.pnlTyGrInfo.Controls.Add(Me.lblType)
		Me.pnlTyGrInfo.Controls.Add(Me.lblGrade)
		Me.pnlTyGrInfo.Dock = System.Windows.Forms.DockStyle.Fill
		Me.pnlTyGrInfo.Location = New System.Drawing.Point(0, 702)
		Me.pnlTyGrInfo.Margin = New System.Windows.Forms.Padding(0)
		Me.pnlTyGrInfo.Name = "pnlTyGrInfo"
		Me.pnlTyGrInfo.Size = New System.Drawing.Size(323, 32)
		Me.pnlTyGrInfo.TabIndex = 57
		'
		'lblType
		'
		Me.lblType.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Left) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblType.BackColor = System.Drawing.Color.White
		Me.lblType.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblType.Font = New System.Drawing.Font("ＭＳ 明朝", 20.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblType.ForeColor = System.Drawing.Color.Black
		Me.lblType.Location = New System.Drawing.Point(0, 0)
		Me.lblType.Margin = New System.Windows.Forms.Padding(0)
		Me.lblType.Name = "lblType"
		Me.lblType.Size = New System.Drawing.Size(231, 32)
		Me.lblType.TabIndex = 2
		Me.lblType.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'lblGrade
		'
		Me.lblGrade.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblGrade.BackColor = System.Drawing.SystemColors.Control
		Me.lblGrade.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblGrade.Font = New System.Drawing.Font("ＭＳ 明朝", 20.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblGrade.ForeColor = System.Drawing.Color.Black
		Me.lblGrade.Location = New System.Drawing.Point(231, 0)
		Me.lblGrade.Margin = New System.Windows.Forms.Padding(0)
		Me.lblGrade.Name = "lblGrade"
		Me.lblGrade.Size = New System.Drawing.Size(92, 32)
		Me.lblGrade.TabIndex = 2
		Me.lblGrade.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'TableLayoutPanel1
		'
		Me.TableLayoutPanel1.ColumnCount = 4
		Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 65.0!))
		Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80.0!))
		Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 35.0!))
		Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 72.0!))
		Me.TableLayoutPanel1.Controls.Add(Me.lblKizuNo, 3, 0)
		Me.TableLayoutPanel1.Controls.Add(Me.lblYName, 0, 0)
		Me.TableLayoutPanel1.Controls.Add(Me.lblY, 1, 0)
		Me.TableLayoutPanel1.Controls.Add(Me.lblKizuNoName, 2, 0)
		Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
		Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 734)
		Me.TableLayoutPanel1.Margin = New System.Windows.Forms.Padding(0)
		Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
		Me.TableLayoutPanel1.RowCount = 1
		Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
		Me.TableLayoutPanel1.Size = New System.Drawing.Size(323, 36)
		Me.TableLayoutPanel1.TabIndex = 59
		'
		'lblKizuNo
		'
		Me.lblKizuNo.AutoSize = True
		Me.lblKizuNo.BackColor = System.Drawing.Color.White
		Me.lblKizuNo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblKizuNo.Dock = System.Windows.Forms.DockStyle.Fill
		Me.lblKizuNo.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblKizuNo.Location = New System.Drawing.Point(250, 0)
		Me.lblKizuNo.Margin = New System.Windows.Forms.Padding(0)
		Me.lblKizuNo.Name = "lblKizuNo"
		Me.lblKizuNo.Size = New System.Drawing.Size(73, 36)
		Me.lblKizuNo.TabIndex = 3
		Me.lblKizuNo.Text = "123456"
		Me.lblKizuNo.TextAlign = System.Drawing.ContentAlignment.MiddleRight
		'
		'lblYName
		'
		Me.lblYName.AutoEllipsis = True
		Me.lblYName.AutoSize = True
		Me.lblYName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblYName.Dock = System.Windows.Forms.DockStyle.Fill
		Me.lblYName.Font = New System.Drawing.Font("ＭＳ 明朝", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblYName.Location = New System.Drawing.Point(0, 0)
		Me.lblYName.Margin = New System.Windows.Forms.Padding(0)
		Me.lblYName.Name = "lblYName"
		Me.lblYName.Size = New System.Drawing.Size(111, 36)
		Me.lblYName.TabIndex = 0
		Me.lblYName.Text = "出側[m]"
		Me.lblYName.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'lblY
		'
		Me.lblY.AutoSize = True
		Me.lblY.BackColor = System.Drawing.Color.White
		Me.lblY.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblY.Dock = System.Windows.Forms.DockStyle.Fill
		Me.lblY.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblY.Location = New System.Drawing.Point(111, 0)
		Me.lblY.Margin = New System.Windows.Forms.Padding(0)
		Me.lblY.Name = "lblY"
		Me.lblY.Size = New System.Drawing.Size(80, 36)
		Me.lblY.TabIndex = 1
		Me.lblY.Text = "12345.6"
		Me.lblY.TextAlign = System.Drawing.ContentAlignment.MiddleRight
		'
		'lblKizuNoName
		'
		Me.lblKizuNoName.AutoEllipsis = True
		Me.lblKizuNoName.AutoSize = True
		Me.lblKizuNoName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblKizuNoName.Dock = System.Windows.Forms.DockStyle.Fill
		Me.lblKizuNoName.Font = New System.Drawing.Font("ＭＳ 明朝", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblKizuNoName.Location = New System.Drawing.Point(191, 0)
		Me.lblKizuNoName.Margin = New System.Windows.Forms.Padding(0)
		Me.lblKizuNoName.Name = "lblKizuNoName"
		Me.lblKizuNoName.Size = New System.Drawing.Size(59, 36)
		Me.lblKizuNoName.TabIndex = 2
		Me.lblKizuNoName.Text = "疵No"
		Me.lblKizuNoName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'ucDefect
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.Controls.Add(Me.tblLayoutPnl)
		Me.Name = "ucDefect"
		Me.Size = New System.Drawing.Size(323, 862)
		Me.tblLayoutPnl.ResumeLayout(False)
		CType(Me.dgvDefInfo, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.picImg2, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.picImg1, System.ComponentModel.ISupportInitialize).EndInit()
		Me.pnlImgInfo1.ResumeLayout(False)
		Me.pnlImgInfo2.ResumeLayout(False)
		Me.pnlTyGrInfo.ResumeLayout(False)
		Me.TableLayoutPanel1.ResumeLayout(False)
		Me.TableLayoutPanel1.PerformLayout()
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents tblLayoutPnl As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents lblDefTitle As System.Windows.Forms.Label
    Friend WithEvents picImg2 As System.Windows.Forms.PictureBox
    Friend WithEvents picImg1 As System.Windows.Forms.PictureBox
    Friend WithEvents pnlImgInfo1 As System.Windows.Forms.Panel
    Friend WithEvents lblImgRight01 As System.Windows.Forms.Label
    Friend WithEvents lblImgLeft01 As System.Windows.Forms.Label
    Friend WithEvents lblImg01 As System.Windows.Forms.Label
    Friend WithEvents pnlImgInfo2 As System.Windows.Forms.Panel
    Friend WithEvents lblImgRight02 As System.Windows.Forms.Label
    Friend WithEvents lblImgLeft02 As System.Windows.Forms.Label
    Friend WithEvents lblImg02 As System.Windows.Forms.Label
    Friend WithEvents pnlTyGrInfo As System.Windows.Forms.Panel
    Friend WithEvents lblType As System.Windows.Forms.Label
    Friend WithEvents lblGrade As System.Windows.Forms.Label
    Friend WithEvents dgvDefInfo As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents TableLayoutPanel1 As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents lblYName As System.Windows.Forms.Label
    Friend WithEvents lblKizuNo As System.Windows.Forms.Label
    Friend WithEvents lblY As System.Windows.Forms.Label
    Friend WithEvents lblKizuNoName As System.Windows.Forms.Label
    Friend WithEvents lblCamNo As Label
End Class
