<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmDefMapJisseki
    Inherits tClassForm.frmBase_Popup

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.tlpBase = New System.Windows.Forms.TableLayoutPanel()
        Me.tlpBase2 = New System.Windows.Forms.TableLayoutPanel()
        Me.tlpMapBase = New System.Windows.Forms.TableLayoutPanel()
        Me.tblMapU = New System.Windows.Forms.TableLayoutPanel()
        Me.pnlMapHed_U = New System.Windows.Forms.Panel()
        Me.lblFace_U = New System.Windows.Forms.Label()
        Me.lblWs_U = New System.Windows.Forms.Label()
        Me.lblDs_U = New System.Windows.Forms.Label()
        Me.lblMapHed_U = New System.Windows.Forms.Label()
        Me.picMapBot = New System.Windows.Forms.PictureBox()
        Me.tblMapO = New System.Windows.Forms.TableLayoutPanel()
        Me.pnlMapHed_O = New System.Windows.Forms.Panel()
        Me.lblFace_O = New System.Windows.Forms.Label()
        Me.lblWs_O = New System.Windows.Forms.Label()
        Me.lblDs_O = New System.Windows.Forms.Label()
        Me.lblMapHed_O = New System.Windows.Forms.Label()
        Me.picMapTop = New System.Windows.Forms.PictureBox()
        Me.tlpRight = New System.Windows.Forms.TableLayoutPanel()
        Me.grpMixedRate = New System.Windows.Forms.GroupBox()
        Me.dgvMixedRate = New System.Windows.Forms.DataGridView()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.lblCutNo = New System.Windows.Forms.Label()
        Me.grpFunction = New System.Windows.Forms.GroupBox()
        Me.btnEnd = New System.Windows.Forms.Button()
        Me.lblEnd_b = New System.Windows.Forms.Label()
        Me.btnNext = New System.Windows.Forms.Button()
        Me.lblNext_b = New System.Windows.Forms.Label()
        Me.btnPre = New System.Windows.Forms.Button()
        Me.lblPre_b = New System.Windows.Forms.Label()
        Me.lblF01Name = New System.Windows.Forms.Label()
        Me.lblF02Name = New System.Windows.Forms.Label()
        Me.lblF12Name = New System.Windows.Forms.Label()
        Me.pnlSample = New System.Windows.Forms.Panel()
        Me.lblShaCutColor = New System.Windows.Forms.Label()
        Me.lblShaCutName = New System.Windows.Forms.Label()
        Me.grpDspSetting = New System.Windows.Forms.GroupBox()
        Me.cmbMarkerColor = New System.Windows.Forms.ComboBox()
        Me.cmbMarkerLabel = New System.Windows.Forms.ComboBox()
        Me.lblMarkerChange = New System.Windows.Forms.Label()
        Me.lblMarkerLabel = New System.Windows.Forms.Label()
        Me.pnlGuid = New System.Windows.Forms.Panel()
        Me.grbNgGuid = New System.Windows.Forms.GroupBox()
        Me.pnlNgGuid = New System.Windows.Forms.Panel()
        Me.lblNgMsg = New System.Windows.Forms.Label()
        Me.grbGuid = New System.Windows.Forms.GroupBox()
        Me.pnlMsg = New System.Windows.Forms.Panel()
        Me.prbMsg = New System.Windows.Forms.ProgressBar()
        Me.lblMsg = New System.Windows.Forms.Label()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.dgvCoilInf = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.dgvMixedRateIn = New System.Windows.Forms.DataGridView()
        Me.tmrDsp = New System.Windows.Forms.Timer(Me.components)
        Me.tlpBase.SuspendLayout()
        Me.tlpBase2.SuspendLayout()
        Me.tlpMapBase.SuspendLayout()
        Me.tblMapU.SuspendLayout()
        Me.pnlMapHed_U.SuspendLayout()
        CType(Me.picMapBot, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tblMapO.SuspendLayout()
        Me.pnlMapHed_O.SuspendLayout()
        CType(Me.picMapTop, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tlpRight.SuspendLayout()
        Me.grpMixedRate.SuspendLayout()
        CType(Me.dgvMixedRate, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpFunction.SuspendLayout()
        Me.pnlSample.SuspendLayout()
        Me.grpDspSetting.SuspendLayout()
        Me.pnlGuid.SuspendLayout()
        Me.grbNgGuid.SuspendLayout()
        Me.pnlNgGuid.SuspendLayout()
        Me.grbGuid.SuspendLayout()
        Me.pnlMsg.SuspendLayout()
        Me.TableLayoutPanel1.SuspendLayout()
        CType(Me.dgvCoilInf, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.Panel1.SuspendLayout()
        CType(Me.dgvMixedRateIn, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'tlpBase
        '
        Me.tlpBase.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tlpBase.ColumnCount = 1
        Me.tlpBase.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpBase.Controls.Add(Me.tlpBase2, 0, 1)
        Me.tlpBase.Controls.Add(Me.TableLayoutPanel1, 0, 0)
        Me.tlpBase.Location = New System.Drawing.Point(4, 4)
        Me.tlpBase.Name = "tlpBase"
        Me.tlpBase.RowCount = 2
        Me.tlpBase.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 80.0!))
        Me.tlpBase.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpBase.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.tlpBase.Size = New System.Drawing.Size(1264, 996)
        Me.tlpBase.TabIndex = 0
        '
        'tlpBase2
        '
        Me.tlpBase2.ColumnCount = 2
        Me.tlpBase2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpBase2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200.0!))
        Me.tlpBase2.Controls.Add(Me.tlpMapBase, 0, 0)
        Me.tlpBase2.Controls.Add(Me.tlpRight, 1, 0)
        Me.tlpBase2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tlpBase2.Location = New System.Drawing.Point(3, 83)
        Me.tlpBase2.Name = "tlpBase2"
        Me.tlpBase2.RowCount = 1
        Me.tlpBase2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpBase2.Size = New System.Drawing.Size(1258, 910)
        Me.tlpBase2.TabIndex = 243
        '
        'tlpMapBase
        '
        Me.tlpMapBase.ColumnCount = 2
        Me.tlpMapBase.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tlpMapBase.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tlpMapBase.Controls.Add(Me.tblMapU, 0, 0)
        Me.tlpMapBase.Controls.Add(Me.tblMapO, 0, 0)
        Me.tlpMapBase.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tlpMapBase.Location = New System.Drawing.Point(3, 3)
        Me.tlpMapBase.Name = "tlpMapBase"
        Me.tlpMapBase.RowCount = 1
        Me.tlpMapBase.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tlpMapBase.Size = New System.Drawing.Size(1052, 904)
        Me.tlpMapBase.TabIndex = 0
        '
        'tblMapU
        '
        Me.tblMapU.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tblMapU.ColumnCount = 1
        Me.tblMapU.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tblMapU.Controls.Add(Me.pnlMapHed_U, 0, 0)
        Me.tblMapU.Controls.Add(Me.picMapBot, 0, 1)
        Me.tblMapU.Location = New System.Drawing.Point(526, 0)
        Me.tblMapU.Margin = New System.Windows.Forms.Padding(0)
        Me.tblMapU.Name = "tblMapU"
        Me.tblMapU.RowCount = 2
        Me.tblMapU.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
        Me.tblMapU.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tblMapU.Size = New System.Drawing.Size(526, 904)
        Me.tblMapU.TabIndex = 243
        '
        'pnlMapHed_U
        '
        Me.pnlMapHed_U.Controls.Add(Me.lblFace_U)
        Me.pnlMapHed_U.Controls.Add(Me.lblWs_U)
        Me.pnlMapHed_U.Controls.Add(Me.lblDs_U)
        Me.pnlMapHed_U.Controls.Add(Me.lblMapHed_U)
        Me.pnlMapHed_U.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlMapHed_U.Location = New System.Drawing.Point(0, 0)
        Me.pnlMapHed_U.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlMapHed_U.Name = "pnlMapHed_U"
        Me.pnlMapHed_U.Size = New System.Drawing.Size(526, 32)
        Me.pnlMapHed_U.TabIndex = 242
        '
        'lblFace_U
        '
        Me.lblFace_U.Anchor = System.Windows.Forms.AnchorStyles.Top
        Me.lblFace_U.BackColor = System.Drawing.Color.Blue
        Me.lblFace_U.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblFace_U.Font = New System.Drawing.Font("ＭＳ 明朝", 21.75!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblFace_U.ForeColor = System.Drawing.Color.Yellow
        Me.lblFace_U.Location = New System.Drawing.Point(208, 0)
        Me.lblFace_U.Name = "lblFace_U"
        Me.lblFace_U.Size = New System.Drawing.Size(116, 32)
        Me.lblFace_U.TabIndex = 226
        Me.lblFace_U.Text = "裏面"
        Me.lblFace_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblWs_U
        '
        Me.lblWs_U.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblWs_U.BackColor = System.Drawing.SystemColors.Control
        Me.lblWs_U.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblWs_U.ForeColor = System.Drawing.Color.Black
        Me.lblWs_U.Location = New System.Drawing.Point(440, 5)
        Me.lblWs_U.Name = "lblWs_U"
        Me.lblWs_U.Size = New System.Drawing.Size(72, 24)
        Me.lblWs_U.TabIndex = 230
        Me.lblWs_U.Text = "WS"
        Me.lblWs_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblDs_U
        '
        Me.lblDs_U.BackColor = System.Drawing.SystemColors.Control
        Me.lblDs_U.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblDs_U.ForeColor = System.Drawing.Color.Black
        Me.lblDs_U.Location = New System.Drawing.Point(16, 5)
        Me.lblDs_U.Name = "lblDs_U"
        Me.lblDs_U.Size = New System.Drawing.Size(72, 24)
        Me.lblDs_U.TabIndex = 231
        Me.lblDs_U.Text = "DS"
        Me.lblDs_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblMapHed_U
        '
        Me.lblMapHed_U.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblMapHed_U.BackColor = System.Drawing.SystemColors.Control
        Me.lblMapHed_U.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMapHed_U.Font = New System.Drawing.Font("ＭＳ 明朝", 21.75!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMapHed_U.ForeColor = System.Drawing.Color.Black
        Me.lblMapHed_U.Location = New System.Drawing.Point(5, 0)
        Me.lblMapHed_U.Margin = New System.Windows.Forms.Padding(5, 0, 0, 0)
        Me.lblMapHed_U.Name = "lblMapHed_U"
        Me.lblMapHed_U.Size = New System.Drawing.Size(521, 34)
        Me.lblMapHed_U.TabIndex = 224
        Me.lblMapHed_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'picMapBot
        '
        Me.picMapBot.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.picMapBot.BackColor = System.Drawing.Color.White
        Me.picMapBot.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picMapBot.Location = New System.Drawing.Point(5, 32)
        Me.picMapBot.Margin = New System.Windows.Forms.Padding(5, 0, 0, 0)
        Me.picMapBot.Name = "picMapBot"
        Me.picMapBot.Size = New System.Drawing.Size(521, 872)
        Me.picMapBot.TabIndex = 222
        Me.picMapBot.TabStop = False
        '
        'tblMapO
        '
        Me.tblMapO.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tblMapO.ColumnCount = 1
        Me.tblMapO.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tblMapO.Controls.Add(Me.pnlMapHed_O, 0, 0)
        Me.tblMapO.Controls.Add(Me.picMapTop, 0, 1)
        Me.tblMapO.Location = New System.Drawing.Point(0, 0)
        Me.tblMapO.Margin = New System.Windows.Forms.Padding(0)
        Me.tblMapO.Name = "tblMapO"
        Me.tblMapO.RowCount = 2
        Me.tblMapO.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
        Me.tblMapO.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tblMapO.Size = New System.Drawing.Size(526, 904)
        Me.tblMapO.TabIndex = 242
        '
        'pnlMapHed_O
        '
        Me.pnlMapHed_O.Controls.Add(Me.lblFace_O)
        Me.pnlMapHed_O.Controls.Add(Me.lblWs_O)
        Me.pnlMapHed_O.Controls.Add(Me.lblDs_O)
        Me.pnlMapHed_O.Controls.Add(Me.lblMapHed_O)
        Me.pnlMapHed_O.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlMapHed_O.Location = New System.Drawing.Point(0, 0)
        Me.pnlMapHed_O.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlMapHed_O.Name = "pnlMapHed_O"
        Me.pnlMapHed_O.Size = New System.Drawing.Size(526, 32)
        Me.pnlMapHed_O.TabIndex = 242
        '
        'lblFace_O
        '
        Me.lblFace_O.Anchor = System.Windows.Forms.AnchorStyles.Top
        Me.lblFace_O.BackColor = System.Drawing.Color.Blue
        Me.lblFace_O.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblFace_O.Font = New System.Drawing.Font("ＭＳ 明朝", 21.75!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblFace_O.ForeColor = System.Drawing.Color.Yellow
        Me.lblFace_O.Location = New System.Drawing.Point(208, 0)
        Me.lblFace_O.Name = "lblFace_O"
        Me.lblFace_O.Size = New System.Drawing.Size(116, 32)
        Me.lblFace_O.TabIndex = 226
        Me.lblFace_O.Text = "表面"
        Me.lblFace_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblWs_O
        '
        Me.lblWs_O.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblWs_O.BackColor = System.Drawing.SystemColors.Control
        Me.lblWs_O.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblWs_O.ForeColor = System.Drawing.Color.Black
        Me.lblWs_O.Location = New System.Drawing.Point(435, 5)
        Me.lblWs_O.Name = "lblWs_O"
        Me.lblWs_O.Size = New System.Drawing.Size(72, 24)
        Me.lblWs_O.TabIndex = 230
        Me.lblWs_O.Text = "WS"
        Me.lblWs_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblDs_O
        '
        Me.lblDs_O.BackColor = System.Drawing.SystemColors.Control
        Me.lblDs_O.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblDs_O.ForeColor = System.Drawing.Color.Black
        Me.lblDs_O.Location = New System.Drawing.Point(16, 5)
        Me.lblDs_O.Name = "lblDs_O"
        Me.lblDs_O.Size = New System.Drawing.Size(72, 24)
        Me.lblDs_O.TabIndex = 231
        Me.lblDs_O.Text = "DS"
        Me.lblDs_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblMapHed_O
        '
        Me.lblMapHed_O.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblMapHed_O.BackColor = System.Drawing.SystemColors.Control
        Me.lblMapHed_O.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMapHed_O.Font = New System.Drawing.Font("ＭＳ 明朝", 21.75!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMapHed_O.ForeColor = System.Drawing.Color.Black
        Me.lblMapHed_O.Location = New System.Drawing.Point(0, 0)
        Me.lblMapHed_O.Margin = New System.Windows.Forms.Padding(0, 0, 5, 0)
        Me.lblMapHed_O.Name = "lblMapHed_O"
        Me.lblMapHed_O.Size = New System.Drawing.Size(521, 34)
        Me.lblMapHed_O.TabIndex = 224
        Me.lblMapHed_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'picMapTop
        '
        Me.picMapTop.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.picMapTop.BackColor = System.Drawing.Color.White
        Me.picMapTop.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picMapTop.Location = New System.Drawing.Point(0, 32)
        Me.picMapTop.Margin = New System.Windows.Forms.Padding(0, 0, 5, 0)
        Me.picMapTop.Name = "picMapTop"
        Me.picMapTop.Size = New System.Drawing.Size(521, 872)
        Me.picMapTop.TabIndex = 221
        Me.picMapTop.TabStop = False
        '
        'tlpRight
        '
        Me.tlpRight.ColumnCount = 1
        Me.tlpRight.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpRight.Controls.Add(Me.grpMixedRate, 0, 0)
        Me.tlpRight.Controls.Add(Me.grpFunction, 0, 4)
        Me.tlpRight.Controls.Add(Me.pnlSample, 0, 1)
        Me.tlpRight.Controls.Add(Me.grpDspSetting, 0, 3)
        Me.tlpRight.Controls.Add(Me.pnlGuid, 0, 2)
        Me.tlpRight.Dock = System.Windows.Forms.DockStyle.Fill
        Me.tlpRight.Location = New System.Drawing.Point(1061, 3)
        Me.tlpRight.Name = "tlpRight"
        Me.tlpRight.RowCount = 5
        Me.tlpRight.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpRight.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34.0!))
        Me.tlpRight.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 132.0!))
        Me.tlpRight.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 126.0!))
        Me.tlpRight.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 180.0!))
        Me.tlpRight.Size = New System.Drawing.Size(194, 904)
        Me.tlpRight.TabIndex = 1
        '
        'grpMixedRate
        '
        Me.grpMixedRate.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.grpMixedRate.Controls.Add(Me.dgvMixedRate)
        Me.grpMixedRate.Controls.Add(Me.Label1)
        Me.grpMixedRate.Controls.Add(Me.lblCutNo)
        Me.grpMixedRate.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpMixedRate.Location = New System.Drawing.Point(0, 0)
        Me.grpMixedRate.Margin = New System.Windows.Forms.Padding(0)
        Me.grpMixedRate.Name = "grpMixedRate"
        Me.grpMixedRate.Size = New System.Drawing.Size(194, 432)
        Me.grpMixedRate.TabIndex = 238
        Me.grpMixedRate.TabStop = False
        Me.grpMixedRate.Text = "疵混入率一覧"
        '
        'dgvMixedRate
        '
        Me.dgvMixedRate.AllowUserToAddRows = False
        Me.dgvMixedRate.AllowUserToDeleteRows = False
        Me.dgvMixedRate.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvMixedRate.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvMixedRate.Location = New System.Drawing.Point(8, 44)
        Me.dgvMixedRate.Name = "dgvMixedRate"
        Me.dgvMixedRate.ReadOnly = True
        Me.dgvMixedRate.RowHeadersWidth = 18
        Me.dgvMixedRate.RowTemplate.Height = 21
        Me.dgvMixedRate.Size = New System.Drawing.Size(180, 381)
        Me.dgvMixedRate.TabIndex = 5
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(12, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(127, 16)
        Me.Label1.TabIndex = 7
        Me.Label1.Text = "出側カット順番"
        '
        'lblCutNo
        '
        Me.lblCutNo.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblCutNo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblCutNo.Location = New System.Drawing.Point(152, 16)
        Me.lblCutNo.Name = "lblCutNo"
        Me.lblCutNo.Size = New System.Drawing.Size(36, 24)
        Me.lblCutNo.TabIndex = 6
        Me.lblCutNo.Text = "①"
        Me.lblCutNo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'grpFunction
        '
        Me.grpFunction.Controls.Add(Me.btnEnd)
        Me.grpFunction.Controls.Add(Me.lblEnd_b)
        Me.grpFunction.Controls.Add(Me.btnNext)
        Me.grpFunction.Controls.Add(Me.lblNext_b)
        Me.grpFunction.Controls.Add(Me.btnPre)
        Me.grpFunction.Controls.Add(Me.lblPre_b)
        Me.grpFunction.Controls.Add(Me.lblF01Name)
        Me.grpFunction.Controls.Add(Me.lblF02Name)
        Me.grpFunction.Controls.Add(Me.lblF12Name)
        Me.grpFunction.Dock = System.Windows.Forms.DockStyle.Fill
        Me.grpFunction.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFunction.Location = New System.Drawing.Point(0, 724)
        Me.grpFunction.Margin = New System.Windows.Forms.Padding(0)
        Me.grpFunction.Name = "grpFunction"
        Me.grpFunction.Size = New System.Drawing.Size(194, 180)
        Me.grpFunction.TabIndex = 244
        Me.grpFunction.TabStop = False
        '
        'btnEnd
        '
        Me.btnEnd.BackColor = System.Drawing.SystemColors.Control
        Me.btnEnd.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnEnd.Location = New System.Drawing.Point(56, 124)
        Me.btnEnd.Name = "btnEnd"
        Me.btnEnd.Size = New System.Drawing.Size(104, 44)
        Me.btnEnd.TabIndex = 199
        Me.btnEnd.Text = "閉じる"
        Me.btnEnd.UseVisualStyleBackColor = False
        '
        'lblEnd_b
        '
        Me.lblEnd_b.BackColor = System.Drawing.SystemColors.ControlText
        Me.lblEnd_b.Location = New System.Drawing.Point(60, 129)
        Me.lblEnd_b.Name = "lblEnd_b"
        Me.lblEnd_b.Size = New System.Drawing.Size(104, 44)
        Me.lblEnd_b.TabIndex = 200
        Me.lblEnd_b.Text = "Label2"
        '
        'btnNext
        '
        Me.btnNext.BackColor = System.Drawing.SystemColors.Control
        Me.btnNext.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnNext.Location = New System.Drawing.Point(56, 72)
        Me.btnNext.Name = "btnNext"
        Me.btnNext.Size = New System.Drawing.Size(104, 44)
        Me.btnNext.TabIndex = 197
        Me.btnNext.Text = "疵混入率" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "次項"
        Me.btnNext.UseVisualStyleBackColor = False
        '
        'lblNext_b
        '
        Me.lblNext_b.BackColor = System.Drawing.SystemColors.ControlText
        Me.lblNext_b.Location = New System.Drawing.Point(60, 77)
        Me.lblNext_b.Name = "lblNext_b"
        Me.lblNext_b.Size = New System.Drawing.Size(104, 44)
        Me.lblNext_b.TabIndex = 198
        Me.lblNext_b.Text = "Label1"
        '
        'btnPre
        '
        Me.btnPre.BackColor = System.Drawing.SystemColors.Control
        Me.btnPre.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnPre.Location = New System.Drawing.Point(56, 20)
        Me.btnPre.Name = "btnPre"
        Me.btnPre.Size = New System.Drawing.Size(104, 44)
        Me.btnPre.TabIndex = 191
        Me.btnPre.Text = "疵混入率" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "前項"
        Me.btnPre.UseVisualStyleBackColor = False
        '
        'lblPre_b
        '
        Me.lblPre_b.BackColor = System.Drawing.SystemColors.ControlText
        Me.lblPre_b.Location = New System.Drawing.Point(60, 25)
        Me.lblPre_b.Name = "lblPre_b"
        Me.lblPre_b.Size = New System.Drawing.Size(104, 44)
        Me.lblPre_b.TabIndex = 193
        Me.lblPre_b.Text = "Label138"
        '
        'lblF01Name
        '
        Me.lblF01Name.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblF01Name.Location = New System.Drawing.Point(20, 33)
        Me.lblF01Name.Margin = New System.Windows.Forms.Padding(0)
        Me.lblF01Name.Name = "lblF01Name"
        Me.lblF01Name.Size = New System.Drawing.Size(35, 16)
        Me.lblF01Name.TabIndex = 195
        Me.lblF01Name.Text = "F1"
        '
        'lblF02Name
        '
        Me.lblF02Name.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblF02Name.Location = New System.Drawing.Point(20, 86)
        Me.lblF02Name.Margin = New System.Windows.Forms.Padding(0)
        Me.lblF02Name.Name = "lblF02Name"
        Me.lblF02Name.Size = New System.Drawing.Size(35, 16)
        Me.lblF02Name.TabIndex = 196
        Me.lblF02Name.Text = "F2"
        '
        'lblF12Name
        '
        Me.lblF12Name.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblF12Name.Location = New System.Drawing.Point(20, 137)
        Me.lblF12Name.Margin = New System.Windows.Forms.Padding(0)
        Me.lblF12Name.Name = "lblF12Name"
        Me.lblF12Name.Size = New System.Drawing.Size(35, 16)
        Me.lblF12Name.TabIndex = 195
        Me.lblF12Name.Text = "F12"
        '
        'pnlSample
        '
        Me.pnlSample.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlSample.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.pnlSample.Controls.Add(Me.lblShaCutColor)
        Me.pnlSample.Controls.Add(Me.lblShaCutName)
        Me.pnlSample.Location = New System.Drawing.Point(0, 435)
        Me.pnlSample.Margin = New System.Windows.Forms.Padding(0, 3, 0, 3)
        Me.pnlSample.Name = "pnlSample"
        Me.pnlSample.Size = New System.Drawing.Size(194, 28)
        Me.pnlSample.TabIndex = 241
        '
        'lblShaCutColor
        '
        Me.lblShaCutColor.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblShaCutColor.BackColor = System.Drawing.Color.Red
        Me.lblShaCutColor.Font = New System.Drawing.Font("ＭＳ 明朝", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblShaCutColor.Location = New System.Drawing.Point(4, 12)
        Me.lblShaCutColor.Name = "lblShaCutColor"
        Me.lblShaCutColor.Size = New System.Drawing.Size(54, 4)
        Me.lblShaCutColor.TabIndex = 239
        '
        'lblShaCutName
        '
        Me.lblShaCutName.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblShaCutName.BackColor = System.Drawing.SystemColors.Control
        Me.lblShaCutName.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblShaCutName.ForeColor = System.Drawing.Color.Black
        Me.lblShaCutName.Location = New System.Drawing.Point(64, 0)
        Me.lblShaCutName.Name = "lblShaCutName"
        Me.lblShaCutName.Size = New System.Drawing.Size(128, 26)
        Me.lblShaCutName.TabIndex = 238
        Me.lblShaCutName.Text = "シャーカット"
        Me.lblShaCutName.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'grpDspSetting
        '
        Me.grpDspSetting.Controls.Add(Me.cmbMarkerColor)
        Me.grpDspSetting.Controls.Add(Me.cmbMarkerLabel)
        Me.grpDspSetting.Controls.Add(Me.lblMarkerChange)
        Me.grpDspSetting.Controls.Add(Me.lblMarkerLabel)
        Me.grpDspSetting.Dock = System.Windows.Forms.DockStyle.Fill
        Me.grpDspSetting.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpDspSetting.Location = New System.Drawing.Point(0, 598)
        Me.grpDspSetting.Margin = New System.Windows.Forms.Padding(0)
        Me.grpDspSetting.Name = "grpDspSetting"
        Me.grpDspSetting.Size = New System.Drawing.Size(194, 126)
        Me.grpDspSetting.TabIndex = 243
        Me.grpDspSetting.TabStop = False
        Me.grpDspSetting.Text = "表示設定"
        '
        'cmbMarkerColor
        '
        Me.cmbMarkerColor.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbMarkerColor.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbMarkerColor.FormattingEnabled = True
        Me.cmbMarkerColor.Location = New System.Drawing.Point(76, 92)
        Me.cmbMarkerColor.Name = "cmbMarkerColor"
        Me.cmbMarkerColor.Size = New System.Drawing.Size(104, 24)
        Me.cmbMarkerColor.TabIndex = 221
        '
        'cmbMarkerLabel
        '
        Me.cmbMarkerLabel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbMarkerLabel.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbMarkerLabel.FormattingEnabled = True
        Me.cmbMarkerLabel.Location = New System.Drawing.Point(76, 44)
        Me.cmbMarkerLabel.Name = "cmbMarkerLabel"
        Me.cmbMarkerLabel.Size = New System.Drawing.Size(104, 24)
        Me.cmbMarkerLabel.TabIndex = 220
        '
        'lblMarkerChange
        '
        Me.lblMarkerChange.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMarkerChange.Location = New System.Drawing.Point(12, 72)
        Me.lblMarkerChange.Name = "lblMarkerChange"
        Me.lblMarkerChange.Size = New System.Drawing.Size(150, 17)
        Me.lblMarkerChange.TabIndex = 219
        Me.lblMarkerChange.Text = "疵マーカー色替え"
        '
        'lblMarkerLabel
        '
        Me.lblMarkerLabel.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMarkerLabel.Location = New System.Drawing.Point(12, 24)
        Me.lblMarkerLabel.Name = "lblMarkerLabel"
        Me.lblMarkerLabel.Size = New System.Drawing.Size(144, 16)
        Me.lblMarkerLabel.TabIndex = 218
        Me.lblMarkerLabel.Text = "疵マーカーラベル"
        '
        'pnlGuid
        '
        Me.pnlGuid.Controls.Add(Me.grbNgGuid)
        Me.pnlGuid.Controls.Add(Me.grbGuid)
        Me.pnlGuid.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlGuid.Location = New System.Drawing.Point(3, 469)
        Me.pnlGuid.Name = "pnlGuid"
        Me.pnlGuid.Size = New System.Drawing.Size(188, 126)
        Me.pnlGuid.TabIndex = 245
        '
        'grbNgGuid
        '
        Me.grbNgGuid.Controls.Add(Me.pnlNgGuid)
        Me.grbNgGuid.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grbNgGuid.Location = New System.Drawing.Point(0, 0)
        Me.grbNgGuid.Margin = New System.Windows.Forms.Padding(0)
        Me.grbNgGuid.Name = "grbNgGuid"
        Me.grbNgGuid.Padding = New System.Windows.Forms.Padding(0)
        Me.grbNgGuid.Size = New System.Drawing.Size(188, 124)
        Me.grbNgGuid.TabIndex = 242
        Me.grbNgGuid.TabStop = False
        Me.grbNgGuid.Text = "異常ガイダンス"
        Me.grbNgGuid.Visible = False
        '
        'pnlNgGuid
        '
        Me.pnlNgGuid.Controls.Add(Me.lblNgMsg)
        Me.pnlNgGuid.Location = New System.Drawing.Point(8, 24)
        Me.pnlNgGuid.Name = "pnlNgGuid"
        Me.pnlNgGuid.Size = New System.Drawing.Size(172, 92)
        Me.pnlNgGuid.TabIndex = 0
        '
        'lblNgMsg
        '
        Me.lblNgMsg.BackColor = System.Drawing.Color.Khaki
        Me.lblNgMsg.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblNgMsg.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblNgMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblNgMsg.Location = New System.Drawing.Point(0, 0)
        Me.lblNgMsg.Name = "lblNgMsg"
        Me.lblNgMsg.Size = New System.Drawing.Size(172, 92)
        Me.lblNgMsg.TabIndex = 0
        '
        'grbGuid
        '
        Me.grbGuid.Controls.Add(Me.pnlMsg)
        Me.grbGuid.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grbGuid.Location = New System.Drawing.Point(0, 0)
        Me.grbGuid.Name = "grbGuid"
        Me.grbGuid.Size = New System.Drawing.Size(188, 128)
        Me.grbGuid.TabIndex = 237
        Me.grbGuid.TabStop = False
        Me.grbGuid.Text = "表示中ガイダンス"
        Me.grbGuid.Visible = False
        '
        'pnlMsg
        '
        Me.pnlMsg.Controls.Add(Me.prbMsg)
        Me.pnlMsg.Controls.Add(Me.lblMsg)
        Me.pnlMsg.Location = New System.Drawing.Point(8, 28)
        Me.pnlMsg.Name = "pnlMsg"
        Me.pnlMsg.Size = New System.Drawing.Size(172, 92)
        Me.pnlMsg.TabIndex = 226
        Me.pnlMsg.Visible = False
        '
        'prbMsg
        '
        Me.prbMsg.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.prbMsg.Location = New System.Drawing.Point(0, 72)
        Me.prbMsg.Name = "prbMsg"
        Me.prbMsg.Size = New System.Drawing.Size(172, 20)
        Me.prbMsg.Style = System.Windows.Forms.ProgressBarStyle.Continuous
        Me.prbMsg.TabIndex = 1
        '
        'lblMsg
        '
        Me.lblMsg.BackColor = System.Drawing.Color.Khaki
        Me.lblMsg.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblMsg.Dock = System.Windows.Forms.DockStyle.Top
        Me.lblMsg.FlatStyle = System.Windows.Forms.FlatStyle.Popup
        Me.lblMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMsg.Location = New System.Drawing.Point(0, 0)
        Me.lblMsg.Name = "lblMsg"
        Me.lblMsg.Size = New System.Drawing.Size(172, 72)
        Me.lblMsg.TabIndex = 2
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 4
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 10.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 180.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 10.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.dgvCoilInf, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.Panel1, 2, 0)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(3, 3)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 1
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(1258, 74)
        Me.TableLayoutPanel1.TabIndex = 244
        '
        'dgvCoilInf
        '
        Me.dgvCoilInf.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvCoilInf.Location = New System.Drawing.Point(3, 3)
        Me.dgvCoilInf.Name = "dgvCoilInf"
        Me.dgvCoilInf.RowTemplate.Height = 21
        Me.dgvCoilInf.Size = New System.Drawing.Size(1052, 68)
        Me.dgvCoilInf.TabIndex = 243
        '
        'Panel1
        '
        Me.Panel1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Panel1.Controls.Add(Me.Label2)
        Me.Panel1.Controls.Add(Me.dgvMixedRateIn)
        Me.Panel1.Location = New System.Drawing.Point(1068, 0)
        Me.Panel1.Margin = New System.Windows.Forms.Padding(0)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(180, 74)
        Me.Panel1.TabIndex = 244
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(4, -1)
        Me.Label2.Margin = New System.Windows.Forms.Padding(0)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(93, 19)
        Me.Label2.TabIndex = 8
        Me.Label2.Text = "疵混入率"
        '
        'dgvMixedRateIn
        '
        Me.dgvMixedRateIn.AllowUserToAddRows = False
        Me.dgvMixedRateIn.AllowUserToDeleteRows = False
        Me.dgvMixedRateIn.AllowUserToResizeColumns = False
        Me.dgvMixedRateIn.AllowUserToResizeRows = False
        Me.dgvMixedRateIn.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvMixedRateIn.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvMixedRateIn.Location = New System.Drawing.Point(1, 25)
        Me.dgvMixedRateIn.Name = "dgvMixedRateIn"
        Me.dgvMixedRateIn.ReadOnly = True
        Me.dgvMixedRateIn.RowHeadersWidth = 18
        Me.dgvMixedRateIn.RowTemplate.Height = 21
        Me.dgvMixedRateIn.Size = New System.Drawing.Size(180, 45)
        Me.dgvMixedRateIn.TabIndex = 7
        '
        'tmrDsp
        '
        Me.tmrDsp.Interval = 500
        '
        'frmDefMapJisseki
        '
        Me.ClientSize = New System.Drawing.Size(1272, 1008)
        Me.ControlBox = True
        Me.Controls.Add(Me.tlpBase)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Sizable
        Me.MaximizeBox = True
        Me.MinimizeBox = True
        Me.Name = "frmDefMapJisseki"
        Me.Text = "入側単位 疵マップ実績画面"
        Me.TopMost = False
        Me.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iOFFLINE
        Me.tProp_Moveable = True
        Me.tlpBase.ResumeLayout(False)
        Me.tlpBase2.ResumeLayout(False)
        Me.tlpMapBase.ResumeLayout(False)
        Me.tblMapU.ResumeLayout(False)
        Me.pnlMapHed_U.ResumeLayout(False)
        CType(Me.picMapBot, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tblMapO.ResumeLayout(False)
        Me.pnlMapHed_O.ResumeLayout(False)
        CType(Me.picMapTop, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tlpRight.ResumeLayout(False)
        Me.grpMixedRate.ResumeLayout(False)
        Me.grpMixedRate.PerformLayout()
        CType(Me.dgvMixedRate, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpFunction.ResumeLayout(False)
        Me.pnlSample.ResumeLayout(False)
        Me.grpDspSetting.ResumeLayout(False)
        Me.pnlGuid.ResumeLayout(False)
        Me.grbNgGuid.ResumeLayout(False)
        Me.pnlNgGuid.ResumeLayout(False)
        Me.grbGuid.ResumeLayout(False)
        Me.pnlMsg.ResumeLayout(False)
        Me.TableLayoutPanel1.ResumeLayout(False)
        CType(Me.dgvCoilInf, System.ComponentModel.ISupportInitialize).EndInit()
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        CType(Me.dgvMixedRateIn, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents tlpBase As TableLayoutPanel
    Friend WithEvents tlpBase2 As TableLayoutPanel
    Friend WithEvents tlpMapBase As TableLayoutPanel
    Friend WithEvents tlpRight As TableLayoutPanel
    Friend WithEvents grpMixedRate As GroupBox
    Friend WithEvents dgvMixedRate As DataGridView
    Friend WithEvents grpFunction As GroupBox
    Friend WithEvents btnEnd As Button
    Friend WithEvents lblEnd_b As Label
    Friend WithEvents btnNext As Button
    Friend WithEvents lblNext_b As Label
    Friend WithEvents btnPre As Button
    Friend WithEvents lblPre_b As Label
    Protected Friend WithEvents lblF01Name As Label
    Protected Friend WithEvents lblF02Name As Label
    Protected Friend WithEvents lblF12Name As Label
    Friend WithEvents pnlSample As Panel
    Friend WithEvents lblShaCutColor As Label
    Friend WithEvents lblShaCutName As Label
    Friend WithEvents grbNgGuid As GroupBox
    Friend WithEvents pnlNgGuid As Panel
    Protected WithEvents lblNgMsg As Label
    Friend WithEvents grpDspSetting As GroupBox
    Friend WithEvents cmbMarkerColor As ComboBox
    Friend WithEvents cmbMarkerLabel As ComboBox
    Friend WithEvents lblMarkerChange As Label
    Friend WithEvents lblMarkerLabel As Label
    Friend WithEvents tblMapU As TableLayoutPanel
    Friend WithEvents pnlMapHed_U As Panel
    Friend WithEvents lblFace_U As Label
    Friend WithEvents lblWs_U As Label
    Friend WithEvents lblDs_U As Label
    Friend WithEvents lblMapHed_U As Label
    Friend WithEvents picMapBot As PictureBox
    Friend WithEvents tblMapO As TableLayoutPanel
    Friend WithEvents pnlMapHed_O As Panel
    Friend WithEvents lblFace_O As Label
    Friend WithEvents lblWs_O As Label
    Friend WithEvents lblDs_O As Label
    Friend WithEvents lblMapHed_O As Label
    Friend WithEvents picMapTop As PictureBox
    Friend WithEvents tmrDsp As Timer
    Friend WithEvents pnlGuid As Panel
    Friend WithEvents grbGuid As GroupBox
    Protected WithEvents pnlMsg As Panel
    Protected WithEvents prbMsg As ProgressBar
    Protected WithEvents lblMsg As Label
    Friend WithEvents lblCutNo As Label
    Friend WithEvents Label1 As Label
    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents dgvCoilInf As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Panel1 As Panel
    Friend WithEvents Label2 As Label
    Friend WithEvents dgvMixedRateIn As DataGridView
End Class
