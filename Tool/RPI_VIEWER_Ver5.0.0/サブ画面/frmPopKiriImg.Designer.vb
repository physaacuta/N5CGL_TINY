<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopKiriImg
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
        Me.lblXds = New System.Windows.Forms.Label()
        Me.lblY = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.lblTid = New System.Windows.Forms.Label()
        Me.lblGid = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.lblNo = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.lblXws = New System.Windows.Forms.Label()
        Me.btnClose = New System.Windows.Forms.Button()
        Me.tlpMain = New System.Windows.Forms.TableLayoutPanel()
        Me.picImg2 = New System.Windows.Forms.PictureBox()
        Me.picImg1 = New System.Windows.Forms.PictureBox()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.lblName1 = New System.Windows.Forms.Label()
        Me.Panel2 = New System.Windows.Forms.Panel()
        Me.lblName2 = New System.Windows.Forms.Label()
        Me.vsbImg = New tClassLibrary.tCnt.tCnt_VScrollBar()
        Me.picImg3 = New System.Windows.Forms.PictureBox()
        Me.Panel4 = New System.Windows.Forms.Panel()
        Me.lblName3 = New System.Windows.Forms.Label()
        Me.Panel5 = New System.Windows.Forms.Panel()
        Me.lblName4 = New System.Windows.Forms.Label()
        Me.Panel3 = New System.Windows.Forms.Panel()
        Me.hsbImg = New tClassLibrary.tCnt.tCnt_HScrollBar()
        Me.cmbPicRate = New System.Windows.Forms.ComboBox()
        Me.picImg4 = New System.Windows.Forms.PictureBox()
        Me.lblT = New System.Windows.Forms.Label()
        Me.lblG = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.lblFno = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.lblCamset = New System.Windows.Forms.Label()
        Me.lblFnum = New System.Windows.Forms.Label()
        Me.tlpMain.SuspendLayout()
        CType(Me.picImg2, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.picImg1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.Panel1.SuspendLayout()
        Me.Panel2.SuspendLayout()
        CType(Me.picImg3, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.Panel4.SuspendLayout()
        Me.Panel5.SuspendLayout()
        Me.Panel3.SuspendLayout()
        CType(Me.picImg4, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'lblXds
        '
        Me.lblXds.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblXds.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblXds.Location = New System.Drawing.Point(472, 24)
        Me.lblXds.Name = "lblXds"
        Me.lblXds.Size = New System.Drawing.Size(72, 24)
        Me.lblXds.TabIndex = 23
        Me.lblXds.Text = "500"
        Me.lblXds.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'lblY
        '
        Me.lblY.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblY.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblY.Location = New System.Drawing.Point(340, 24)
        Me.lblY.Name = "lblY"
        Me.lblY.Size = New System.Drawing.Size(100, 24)
        Me.lblY.TabIndex = 24
        Me.lblY.Text = "1000.000"
        Me.lblY.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label9
        '
        Me.Label9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label9.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label9.Location = New System.Drawing.Point(444, 24)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(28, 24)
        Me.Label9.TabIndex = 21
        Me.Label9.Text = "DS"
        Me.Label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label7.Location = New System.Drawing.Point(292, 24)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(48, 24)
        Me.Label7.TabIndex = 22
        Me.Label7.Text = "長手"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label4.Location = New System.Drawing.Point(44, 24)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(48, 24)
        Me.Label4.TabIndex = 27
        Me.Label4.Text = "Gr"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label3.Location = New System.Drawing.Point(44, 0)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(48, 24)
        Me.Label3.TabIndex = 26
        Me.Label3.Text = "疵種"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblTid
        '
        Me.lblTid.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblTid.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblTid.Location = New System.Drawing.Point(92, 0)
        Me.lblTid.Name = "lblTid"
        Me.lblTid.Size = New System.Drawing.Size(64, 24)
        Me.lblTid.TabIndex = 25
        Me.lblTid.Text = "11"
        Me.lblTid.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblGid
        '
        Me.lblGid.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblGid.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblGid.Location = New System.Drawing.Point(92, 24)
        Me.lblGid.Name = "lblGid"
        Me.lblGid.Size = New System.Drawing.Size(64, 24)
        Me.lblGid.TabIndex = 28
        Me.lblGid.Text = "11"
        Me.lblGid.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label5.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label5.Location = New System.Drawing.Point(292, 0)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(48, 24)
        Me.Label5.TabIndex = 22
        Me.Label5.Text = "疵No"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblNo
        '
        Me.lblNo.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblNo.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblNo.Location = New System.Drawing.Point(340, 0)
        Me.lblNo.Name = "lblNo"
        Me.lblNo.Size = New System.Drawing.Size(60, 24)
        Me.lblNo.TabIndex = 24
        Me.lblNo.Text = "32500"
        Me.lblNo.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label8
        '
        Me.Label8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label8.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label8.Location = New System.Drawing.Point(548, 24)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(28, 24)
        Me.Label8.TabIndex = 21
        Me.Label8.Text = "WS"
        Me.Label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblXws
        '
        Me.lblXws.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblXws.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblXws.Location = New System.Drawing.Point(576, 24)
        Me.lblXws.Name = "lblXws"
        Me.lblXws.Size = New System.Drawing.Size(72, 24)
        Me.lblXws.TabIndex = 23
        Me.lblXws.Text = "500"
        Me.lblXws.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'btnClose
        '
        Me.btnClose.Location = New System.Drawing.Point(4, 0)
        Me.btnClose.Name = "btnClose"
        Me.btnClose.Size = New System.Drawing.Size(36, 48)
        Me.btnClose.TabIndex = 29
        Me.btnClose.Text = "閉じる"
        Me.btnClose.UseVisualStyleBackColor = True
        '
        'tlpMain
        '
        Me.tlpMain.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tlpMain.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.[Single]
        Me.tlpMain.ColumnCount = 5
        Me.tlpMain.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25.0!))
        Me.tlpMain.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25.0!))
        Me.tlpMain.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25.0!))
        Me.tlpMain.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25.0!))
        Me.tlpMain.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 23.0!))
        Me.tlpMain.Controls.Add(Me.picImg2, 1, 1)
        Me.tlpMain.Controls.Add(Me.picImg1, 0, 1)
        Me.tlpMain.Controls.Add(Me.Panel1, 0, 0)
        Me.tlpMain.Controls.Add(Me.Panel2, 1, 0)
        Me.tlpMain.Controls.Add(Me.vsbImg, 4, 1)
        Me.tlpMain.Controls.Add(Me.picImg3, 2, 1)
        Me.tlpMain.Controls.Add(Me.Panel4, 2, 0)
        Me.tlpMain.Controls.Add(Me.Panel5, 3, 0)
        Me.tlpMain.Controls.Add(Me.Panel3, 3, 2)
        Me.tlpMain.Controls.Add(Me.picImg4, 3, 1)
        Me.tlpMain.Location = New System.Drawing.Point(4, 52)
        Me.tlpMain.Name = "tlpMain"
        Me.tlpMain.RowCount = 3
        Me.tlpMain.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 24.0!))
        Me.tlpMain.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpMain.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.tlpMain.Size = New System.Drawing.Size(860, 401)
        Me.tlpMain.TabIndex = 30
        '
        'picImg2
        '
        Me.picImg2.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.picImg2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picImg2.Location = New System.Drawing.Point(212, 29)
        Me.picImg2.Name = "picImg2"
        Me.picImg2.Size = New System.Drawing.Size(201, 347)
        Me.picImg2.TabIndex = 18
        Me.picImg2.TabStop = False
        '
        'picImg1
        '
        Me.picImg1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.picImg1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picImg1.Location = New System.Drawing.Point(4, 29)
        Me.picImg1.Name = "picImg1"
        Me.picImg1.Size = New System.Drawing.Size(201, 347)
        Me.picImg1.TabIndex = 19
        Me.picImg1.TabStop = False
        '
        'Panel1
        '
        Me.Panel1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Panel1.Controls.Add(Me.lblName1)
        Me.Panel1.Location = New System.Drawing.Point(4, 4)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(201, 18)
        Me.Panel1.TabIndex = 20
        '
        'lblName1
        '
        Me.lblName1.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.lblName1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblName1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblName1.Location = New System.Drawing.Point(0, 0)
        Me.lblName1.Name = "lblName1"
        Me.lblName1.Size = New System.Drawing.Size(201, 18)
        Me.lblName1.TabIndex = 0
        Me.lblName1.Tag = "乱反射３"
        Me.lblName1.Text = "乱反射３"
        Me.lblName1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Panel2
        '
        Me.Panel2.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Panel2.Controls.Add(Me.lblName2)
        Me.Panel2.Location = New System.Drawing.Point(212, 4)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(201, 18)
        Me.Panel2.TabIndex = 21
        '
        'lblName2
        '
        Me.lblName2.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.lblName2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblName2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblName2.Location = New System.Drawing.Point(0, 0)
        Me.lblName2.Name = "lblName2"
        Me.lblName2.Size = New System.Drawing.Size(201, 18)
        Me.lblName2.TabIndex = 1
        Me.lblName2.Tag = "乱反射２"
        Me.lblName2.Text = "乱反射２"
        Me.lblName2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'vsbImg
        '
        Me.vsbImg.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.vsbImg.Location = New System.Drawing.Point(841, 26)
        Me.vsbImg.Name = "vsbImg"
        Me.vsbImg.Size = New System.Drawing.Size(18, 353)
        Me.vsbImg.TabIndex = 23
        '
        'picImg3
        '
        Me.picImg3.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.picImg3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picImg3.Location = New System.Drawing.Point(420, 29)
        Me.picImg3.Name = "picImg3"
        Me.picImg3.Size = New System.Drawing.Size(201, 347)
        Me.picImg3.TabIndex = 18
        Me.picImg3.TabStop = False
        '
        'Panel4
        '
        Me.Panel4.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Panel4.Controls.Add(Me.lblName3)
        Me.Panel4.Location = New System.Drawing.Point(420, 4)
        Me.Panel4.Name = "Panel4"
        Me.Panel4.Size = New System.Drawing.Size(201, 18)
        Me.Panel4.TabIndex = 25
        '
        'lblName3
        '
        Me.lblName3.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.lblName3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblName3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblName3.Location = New System.Drawing.Point(0, 0)
        Me.lblName3.Name = "lblName3"
        Me.lblName3.Size = New System.Drawing.Size(201, 18)
        Me.lblName3.TabIndex = 1
        Me.lblName3.Tag = "乱反射"
        Me.lblName3.Text = "乱反射"
        Me.lblName3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Panel5
        '
        Me.Panel5.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Panel5.Controls.Add(Me.lblName4)
        Me.Panel5.Location = New System.Drawing.Point(628, 4)
        Me.Panel5.Name = "Panel5"
        Me.Panel5.Size = New System.Drawing.Size(201, 18)
        Me.Panel5.TabIndex = 26
        '
        'lblName4
        '
        Me.lblName4.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.lblName4.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblName4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblName4.Location = New System.Drawing.Point(0, 0)
        Me.lblName4.Name = "lblName4"
        Me.lblName4.Size = New System.Drawing.Size(201, 18)
        Me.lblName4.TabIndex = 1
        Me.lblName4.Tag = "正反射"
        Me.lblName4.Text = "正反射"
        Me.lblName4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Panel3
        '
        Me.Panel3.Controls.Add(Me.hsbImg)
        Me.Panel3.Controls.Add(Me.cmbPicRate)
        Me.Panel3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel3.Location = New System.Drawing.Point(625, 380)
        Me.Panel3.Margin = New System.Windows.Forms.Padding(0)
        Me.Panel3.Name = "Panel3"
        Me.Panel3.Size = New System.Drawing.Size(207, 20)
        Me.Panel3.TabIndex = 24
        '
        'hsbImg
        '
        Me.hsbImg.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.hsbImg.Location = New System.Drawing.Point(56, 0)
        Me.hsbImg.Name = "hsbImg"
        Me.hsbImg.Size = New System.Drawing.Size(151, 20)
        Me.hsbImg.TabIndex = 22
        '
        'cmbPicRate
        '
        Me.cmbPicRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbPicRate.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbPicRate.FormattingEnabled = True
        Me.cmbPicRate.Location = New System.Drawing.Point(4, 0)
        Me.cmbPicRate.Name = "cmbPicRate"
        Me.cmbPicRate.Size = New System.Drawing.Size(52, 20)
        Me.cmbPicRate.TabIndex = 27
        '
        'picImg4
        '
        Me.picImg4.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.picImg4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picImg4.Location = New System.Drawing.Point(628, 29)
        Me.picImg4.Name = "picImg4"
        Me.picImg4.Size = New System.Drawing.Size(201, 347)
        Me.picImg4.TabIndex = 18
        Me.picImg4.TabStop = False
        '
        'lblT
        '
        Me.lblT.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblT.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblT.Location = New System.Drawing.Point(156, 0)
        Me.lblT.Name = "lblT"
        Me.lblT.Size = New System.Drawing.Size(128, 24)
        Me.lblT.TabIndex = 25
        Me.lblT.Text = "11"
        Me.lblT.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblG
        '
        Me.lblG.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblG.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblG.Location = New System.Drawing.Point(156, 24)
        Me.lblG.Name = "lblG"
        Me.lblG.Size = New System.Drawing.Size(128, 24)
        Me.lblG.TabIndex = 28
        Me.lblG.Text = "11"
        Me.lblG.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label6.Location = New System.Drawing.Point(400, 0)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(40, 24)
        Me.Label6.TabIndex = 22
        Me.Label6.Text = "FNo"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblFno
        '
        Me.lblFno.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblFno.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblFno.Location = New System.Drawing.Point(440, 0)
        Me.lblFno.Name = "lblFno"
        Me.lblFno.Size = New System.Drawing.Size(52, 24)
        Me.lblFno.TabIndex = 24
        Me.lblFno.Text = "16"
        Me.lblFno.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label12
        '
        Me.Label12.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label12.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label12.Location = New System.Drawing.Point(556, 0)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(64, 24)
        Me.Label12.TabIndex = 22
        Me.Label12.Text = "camset"
        Me.Label12.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblCamset
        '
        Me.lblCamset.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblCamset.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblCamset.Location = New System.Drawing.Point(620, 0)
        Me.lblCamset.Name = "lblCamset"
        Me.lblCamset.Size = New System.Drawing.Size(28, 24)
        Me.lblCamset.TabIndex = 24
        Me.lblCamset.Text = "16"
        Me.lblCamset.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'lblFnum
        '
        Me.lblFnum.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblFnum.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblFnum.Location = New System.Drawing.Point(492, 0)
        Me.lblFnum.Name = "lblFnum"
        Me.lblFnum.Size = New System.Drawing.Size(64, 24)
        Me.lblFnum.TabIndex = 24
        Me.lblFnum.Text = "16"
        Me.lblFnum.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'frmPopKiriImg
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(864, 454)
        Me.ControlBox = False
        Me.Controls.Add(Me.tlpMain)
        Me.Controls.Add(Me.btnClose)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.lblG)
        Me.Controls.Add(Me.lblGid)
        Me.Controls.Add(Me.lblT)
        Me.Controls.Add(Me.lblTid)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.lblXws)
        Me.Controls.Add(Me.lblXds)
        Me.Controls.Add(Me.lblCamset)
        Me.Controls.Add(Me.lblFnum)
        Me.Controls.Add(Me.lblFno)
        Me.Controls.Add(Me.lblNo)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.lblY)
        Me.Controls.Add(Me.Label12)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.Label7)
        Me.Name = "frmPopKiriImg"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
        Me.Text = "切り出し画像表示"
        Me.tlpMain.ResumeLayout(False)
        CType(Me.picImg2, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.picImg1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.Panel1.ResumeLayout(False)
        Me.Panel2.ResumeLayout(False)
        CType(Me.picImg3, System.ComponentModel.ISupportInitialize).EndInit()
        Me.Panel4.ResumeLayout(False)
        Me.Panel5.ResumeLayout(False)
        Me.Panel3.ResumeLayout(False)
        CType(Me.picImg4, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents lblXds As System.Windows.Forms.Label
    Friend WithEvents lblY As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents lblTid As System.Windows.Forms.Label
    Friend WithEvents lblGid As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents lblNo As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents lblXws As System.Windows.Forms.Label
    Friend WithEvents btnClose As System.Windows.Forms.Button
    Friend WithEvents tlpMain As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents picImg2 As System.Windows.Forms.PictureBox
    Friend WithEvents picImg1 As System.Windows.Forms.PictureBox
    Friend WithEvents Panel1 As System.Windows.Forms.Panel
    Friend WithEvents lblName1 As System.Windows.Forms.Label
    Friend WithEvents Panel2 As System.Windows.Forms.Panel
    Friend WithEvents vsbImg As tClassLibrary.tCnt.tCnt_VScrollBar
    Friend WithEvents lblT As System.Windows.Forms.Label
    Friend WithEvents lblG As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents lblFno As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents lblCamset As System.Windows.Forms.Label
    Friend WithEvents lblFnum As System.Windows.Forms.Label
    Friend WithEvents lblName2 As System.Windows.Forms.Label
    Friend WithEvents picImg3 As System.Windows.Forms.PictureBox
    Friend WithEvents picImg4 As System.Windows.Forms.PictureBox
    Friend WithEvents Panel4 As System.Windows.Forms.Panel
    Friend WithEvents lblName3 As System.Windows.Forms.Label
    Friend WithEvents Panel5 As System.Windows.Forms.Panel
    Friend WithEvents lblName4 As System.Windows.Forms.Label
    Friend WithEvents Panel3 As System.Windows.Forms.Panel
    Friend WithEvents hsbImg As tClassLibrary.tCnt.tCnt_HScrollBar
    Public WithEvents cmbPicRate As System.Windows.Forms.ComboBox
End Class
