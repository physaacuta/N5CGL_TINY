<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmJisseki
    Inherits tClassForm.frmBase_Main

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.vsbMap = New tClassLibrary.tCnt.tCnt_VScrollBar()
        Me.Button1 = New System.Windows.Forms.Button()
        Me.Button2 = New System.Windows.Forms.Button()
        Me.Button3 = New System.Windows.Forms.Button()
        Me.Button4 = New System.Windows.Forms.Button()
        Me.Button5 = New System.Windows.Forms.Button()
        Me.Button6 = New System.Windows.Forms.Button()
        Me.Label21 = New System.Windows.Forms.Label()
        Me.Label28 = New System.Windows.Forms.Label()
        Me.Label32 = New System.Windows.Forms.Label()
        Me.Label34 = New System.Windows.Forms.Label()
        Me.Label35 = New System.Windows.Forms.Label()
        Me.Label37 = New System.Windows.Forms.Label()
        Me.TableLayoutPanel3 = New System.Windows.Forms.TableLayoutPanel()
        Me.TableLayoutPanel4 = New System.Windows.Forms.TableLayoutPanel()
        Me.TableLayoutPanel5 = New System.Windows.Forms.TableLayoutPanel()
        Me.UcDefect2 = New SO_GAMENN.ucDefect()
        Me.UcDefect1 = New SO_GAMENN.ucDefect()
        Me.TableLayoutPanel6 = New System.Windows.Forms.TableLayoutPanel()
        Me.TableLayoutPanel7 = New System.Windows.Forms.TableLayoutPanel()
        Me.dgvDeep = New SO_GAMENN.tCnt_DataGridView_turn()
        Me.dgvOth1 = New SO_GAMENN.tCnt_DataGridView_turn()
        Me.TableLayoutPanel8 = New System.Windows.Forms.TableLayoutPanel()
        Me.dgvMap = New SO_GAMENN.tCnt_DataGridView_turn()
        Me.pnlMap = New System.Windows.Forms.Panel()
        Me.lblKensaPoint = New System.Windows.Forms.Label()
        Me.UcCoil1 = New SO_GAMENN.ucCoil()
        Me.btnShift = New System.Windows.Forms.Button()
        Me.TableLayoutPanel3.SuspendLayout()
        Me.TableLayoutPanel4.SuspendLayout()
        Me.TableLayoutPanel5.SuspendLayout()
        Me.TableLayoutPanel6.SuspendLayout()
        Me.TableLayoutPanel7.SuspendLayout()
        CType(Me.dgvDeep, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvOth1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TableLayoutPanel8.SuspendLayout()
        CType(Me.dgvMap, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlMap.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(6, 930)
        Me.grpFunction.Size = New System.Drawing.Size(1263, 58)
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF02.Size = New System.Drawing.Size(90, 26)
        Me.btnF02.Text = "次材"
        '
        'lblF02_b
        '
        Me.lblF02_b.Size = New System.Drawing.Size(90, 26)
        '
        'btnF03
        '
        Me.btnF03.Enabled = True
        Me.btnF03.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF03.Size = New System.Drawing.Size(90, 26)
        Me.btnF03.Text = "前疵"
        '
        'lblF03_b
        '
        Me.lblF03_b.Size = New System.Drawing.Size(90, 26)
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF01.Size = New System.Drawing.Size(90, 26)
        Me.btnF01.Text = "前材"
        '
        'lblF01_b
        '
        Me.lblF01_b.Size = New System.Drawing.Size(90, 26)
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF12.Size = New System.Drawing.Size(90, 26)
        Me.btnF12.Text = "メニュー"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF11.Size = New System.Drawing.Size(90, 26)
        Me.btnF11.Text = "画面保存"
        '
        'btnF10
        '
        Me.btnF10.Enabled = True
        Me.btnF10.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF10.Size = New System.Drawing.Size(90, 26)
        Me.btnF10.Text = "画面印刷"
        '
        'btnF09
        '
        Me.btnF09.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF09.Size = New System.Drawing.Size(90, 26)
        Me.btnF09.Text = "疵マップ"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF08.Size = New System.Drawing.Size(90, 26)
        Me.btnF08.Text = "5m→30m"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF07.Size = New System.Drawing.Size(90, 26)
        Me.btnF07.Text = "裏面表示"
        '
        'btnF06
        '
        Me.btnF06.Enabled = True
        Me.btnF06.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF06.Size = New System.Drawing.Size(90, 26)
        Me.btnF06.Text = "疵混入率"
        '
        'btnF05
        '
        Me.btnF05.Enabled = True
        Me.btnF05.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF05.Size = New System.Drawing.Size(90, 26)
        Me.btnF05.Text = "検索"
        '
        'btnF04
        '
        Me.btnF04.Enabled = True
        Me.btnF04.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF04.Size = New System.Drawing.Size(90, 26)
        Me.btnF04.Text = "次疵"
        '
        'lblF12_b
        '
        Me.lblF12_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF11_b
        '
        Me.lblF11_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF10_b
        '
        Me.lblF10_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF09_b
        '
        Me.lblF09_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF08_b
        '
        Me.lblF08_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF07_b
        '
        Me.lblF07_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF06_b
        '
        Me.lblF06_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF05_b
        '
        Me.lblF05_b.Size = New System.Drawing.Size(90, 26)
        '
        'lblF04_b
        '
        Me.lblF04_b.Size = New System.Drawing.Size(90, 26)
        '
        'vsbMap
        '
        Me.vsbMap.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.vsbMap.Location = New System.Drawing.Point(576, 0)
        Me.vsbMap.Name = "vsbMap"
        Me.vsbMap.Size = New System.Drawing.Size(16, 774)
        Me.vsbMap.TabIndex = 216
        '
        'Button1
        '
        Me.Button1.BackColor = System.Drawing.SystemColors.Control
        Me.Button1.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Button1.Location = New System.Drawing.Point(20, 4)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(84, 24)
        Me.Button1.TabIndex = 24
        Me.Button1.Text = "一時停止"
        Me.Button1.UseVisualStyleBackColor = False
        '
        'Button2
        '
        Me.Button2.BackColor = System.Drawing.SystemColors.Control
        Me.Button2.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Button2.Location = New System.Drawing.Point(540, 4)
        Me.Button2.Name = "Button2"
        Me.Button2.Size = New System.Drawing.Size(84, 24)
        Me.Button2.TabIndex = 29
        Me.Button2.Text = "警告OFF"
        Me.Button2.UseVisualStyleBackColor = False
        '
        'Button3
        '
        Me.Button3.BackColor = System.Drawing.SystemColors.Control
        Me.Button3.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Button3.Location = New System.Drawing.Point(124, 4)
        Me.Button3.Name = "Button3"
        Me.Button3.Size = New System.Drawing.Size(84, 24)
        Me.Button3.TabIndex = 28
        Me.Button3.Text = "ｵﾍﾟﾚｰﾀ画面"
        Me.Button3.UseVisualStyleBackColor = False
        '
        'Button4
        '
        Me.Button4.BackColor = System.Drawing.SystemColors.Control
        Me.Button4.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Button4.Location = New System.Drawing.Point(436, 4)
        Me.Button4.Name = "Button4"
        Me.Button4.Size = New System.Drawing.Size(84, 24)
        Me.Button4.TabIndex = 25
        Me.Button4.Text = "画面印刷"
        Me.Button4.UseVisualStyleBackColor = False
        '
        'Button5
        '
        Me.Button5.BackColor = System.Drawing.SystemColors.Control
        Me.Button5.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Button5.Location = New System.Drawing.Point(228, 4)
        Me.Button5.Name = "Button5"
        Me.Button5.Size = New System.Drawing.Size(84, 24)
        Me.Button5.TabIndex = 26
        Me.Button5.Text = "5m->50m"
        Me.Button5.UseVisualStyleBackColor = False
        '
        'Button6
        '
        Me.Button6.BackColor = System.Drawing.SystemColors.Control
        Me.Button6.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Button6.Location = New System.Drawing.Point(332, 4)
        Me.Button6.Name = "Button6"
        Me.Button6.Size = New System.Drawing.Size(84, 24)
        Me.Button6.TabIndex = 27
        Me.Button6.Text = "ｵｰﾊﾞﾚｲ有無"
        Me.Button6.UseVisualStyleBackColor = False
        '
        'Label21
        '
        Me.Label21.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label21.Location = New System.Drawing.Point(0, 0)
        Me.Label21.Margin = New System.Windows.Forms.Padding(0)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(20, 24)
        Me.Label21.TabIndex = 35
        Me.Label21.Text = "F6"
        Me.Label21.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label28
        '
        Me.Label28.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label28.Location = New System.Drawing.Point(416, 4)
        Me.Label28.Margin = New System.Windows.Forms.Padding(0)
        Me.Label28.Name = "Label28"
        Me.Label28.Size = New System.Drawing.Size(20, 24)
        Me.Label28.TabIndex = 34
        Me.Label28.Text = "F5"
        Me.Label28.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label32
        '
        Me.Label32.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label32.Location = New System.Drawing.Point(312, 4)
        Me.Label32.Margin = New System.Windows.Forms.Padding(0)
        Me.Label32.Name = "Label32"
        Me.Label32.Size = New System.Drawing.Size(20, 24)
        Me.Label32.TabIndex = 33
        Me.Label32.Text = "F4"
        Me.Label32.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label34
        '
        Me.Label34.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label34.Location = New System.Drawing.Point(208, 4)
        Me.Label34.Margin = New System.Windows.Forms.Padding(0)
        Me.Label34.Name = "Label34"
        Me.Label34.Size = New System.Drawing.Size(20, 24)
        Me.Label34.TabIndex = 32
        Me.Label34.Text = "F3"
        Me.Label34.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label35
        '
        Me.Label35.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label35.Location = New System.Drawing.Point(104, 4)
        Me.Label35.Margin = New System.Windows.Forms.Padding(0)
        Me.Label35.Name = "Label35"
        Me.Label35.Size = New System.Drawing.Size(20, 24)
        Me.Label35.TabIndex = 31
        Me.Label35.Text = "F2"
        Me.Label35.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label37
        '
        Me.Label37.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label37.Location = New System.Drawing.Point(0, 4)
        Me.Label37.Margin = New System.Windows.Forms.Padding(0)
        Me.Label37.Name = "Label37"
        Me.Label37.Size = New System.Drawing.Size(20, 24)
        Me.Label37.TabIndex = 30
        Me.Label37.Text = "F1"
        Me.Label37.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'TableLayoutPanel3
        '
        Me.TableLayoutPanel3.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel3.ColumnCount = 1
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Controls.Add(Me.TableLayoutPanel4, 0, 1)
        Me.TableLayoutPanel3.Controls.Add(Me.UcCoil1, 0, 0)
        Me.TableLayoutPanel3.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel3.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel3.Name = "TableLayoutPanel3"
        Me.TableLayoutPanel3.RowCount = 2
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 66.0!))
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Size = New System.Drawing.Size(1270, 928)
        Me.TableLayoutPanel3.TabIndex = 1
        '
        'TableLayoutPanel4
        '
        Me.TableLayoutPanel4.ColumnCount = 2
        Me.TableLayoutPanel4.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel4.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 624.0!))
        Me.TableLayoutPanel4.Controls.Add(Me.TableLayoutPanel5, 0, 0)
        Me.TableLayoutPanel4.Controls.Add(Me.TableLayoutPanel6, 1, 0)
        Me.TableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel4.Location = New System.Drawing.Point(0, 66)
        Me.TableLayoutPanel4.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel4.Name = "TableLayoutPanel4"
        Me.TableLayoutPanel4.RowCount = 1
        Me.TableLayoutPanel4.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel4.Size = New System.Drawing.Size(1270, 862)
        Me.TableLayoutPanel4.TabIndex = 253
        '
        'TableLayoutPanel5
        '
        Me.TableLayoutPanel5.ColumnCount = 2
        Me.TableLayoutPanel5.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel5.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel5.Controls.Add(Me.UcDefect2, 0, 0)
        Me.TableLayoutPanel5.Controls.Add(Me.UcDefect1, 1, 0)
        Me.TableLayoutPanel5.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel5.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel5.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel5.Name = "TableLayoutPanel5"
        Me.TableLayoutPanel5.RowCount = 1
        Me.TableLayoutPanel5.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel5.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 862.0!))
        Me.TableLayoutPanel5.Size = New System.Drawing.Size(646, 862)
        Me.TableLayoutPanel5.TabIndex = 0
        '
        'UcDefect2
        '
        Me.UcDefect2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcDefect2.Location = New System.Drawing.Point(0, 0)
        Me.UcDefect2.Margin = New System.Windows.Forms.Padding(0)
        Me.UcDefect2.Name = "UcDefect2"
        Me.UcDefect2.Size = New System.Drawing.Size(323, 862)
        Me.UcDefect2.TabIndex = 232
        Me.UcDefect2.Title = "一つ前の疵"
        Me.UcDefect2.TitleColor = System.Drawing.Color.Lime
        '
        'UcDefect1
        '
        Me.UcDefect1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcDefect1.Location = New System.Drawing.Point(323, 0)
        Me.UcDefect1.Margin = New System.Windows.Forms.Padding(0)
        Me.UcDefect1.Name = "UcDefect1"
        Me.UcDefect1.Size = New System.Drawing.Size(323, 862)
        Me.UcDefect1.TabIndex = 230
        Me.UcDefect1.Title = "最新の疵"
        Me.UcDefect1.TitleColor = System.Drawing.Color.Aqua
        '
        'TableLayoutPanel6
        '
        Me.TableLayoutPanel6.ColumnCount = 1
        Me.TableLayoutPanel6.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel6.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.TableLayoutPanel6.Controls.Add(Me.TableLayoutPanel7, 0, 1)
        Me.TableLayoutPanel6.Controls.Add(Me.TableLayoutPanel8, 0, 0)
        Me.TableLayoutPanel6.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel6.Location = New System.Drawing.Point(646, 0)
        Me.TableLayoutPanel6.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel6.Name = "TableLayoutPanel6"
        Me.TableLayoutPanel6.RowCount = 2
        Me.TableLayoutPanel6.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel6.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 88.0!))
        Me.TableLayoutPanel6.Size = New System.Drawing.Size(624, 862)
        Me.TableLayoutPanel6.TabIndex = 1
        '
        'TableLayoutPanel7
        '
        Me.TableLayoutPanel7.ColumnCount = 4
        Me.TableLayoutPanel7.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 10.0!))
        Me.TableLayoutPanel7.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.00249!))
        Me.TableLayoutPanel7.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 49.99751!))
        Me.TableLayoutPanel7.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.TableLayoutPanel7.Controls.Add(Me.dgvDeep, 1, 0)
        Me.TableLayoutPanel7.Controls.Add(Me.dgvOth1, 2, 0)
        Me.TableLayoutPanel7.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel7.Location = New System.Drawing.Point(0, 774)
        Me.TableLayoutPanel7.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel7.Name = "TableLayoutPanel7"
        Me.TableLayoutPanel7.RowCount = 1
        Me.TableLayoutPanel7.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel7.Size = New System.Drawing.Size(624, 88)
        Me.TableLayoutPanel7.TabIndex = 0
        '
        'dgvDeep
        '
        Me.dgvDeep.bTanspose = False
        Me.dgvDeep.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvDeep.Cursor = System.Windows.Forms.Cursors.Hand
        Me.dgvDeep.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvDeep.GridColor = System.Drawing.SystemColors.Control
        Me.dgvDeep.Location = New System.Drawing.Point(10, 3)
        Me.dgvDeep.Margin = New System.Windows.Forms.Padding(0, 3, 1, 0)
        Me.dgvDeep.Name = "dgvDeep"
        Me.dgvDeep.RowTemplate.Height = 21
        Me.dgvDeep.Size = New System.Drawing.Size(291, 85)
        Me.dgvDeep.TabIndex = 100
        '
        'dgvOth1
        '
        Me.dgvOth1.bTanspose = False
        Me.dgvOth1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvOth1.Cursor = System.Windows.Forms.Cursors.Hand
        Me.dgvOth1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvOth1.GridColor = System.Drawing.SystemColors.Control
        Me.dgvOth1.Location = New System.Drawing.Point(302, 3)
        Me.dgvOth1.Margin = New System.Windows.Forms.Padding(0, 3, 1, 0)
        Me.dgvOth1.Name = "dgvOth1"
        Me.dgvOth1.RowTemplate.Height = 21
        Me.dgvOth1.Size = New System.Drawing.Size(290, 85)
        Me.dgvOth1.TabIndex = 102
        '
        'TableLayoutPanel8
        '
        Me.TableLayoutPanel8.ColumnCount = 3
        Me.TableLayoutPanel8.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 10.0!))
        Me.TableLayoutPanel8.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel8.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 16.0!))
        Me.TableLayoutPanel8.Controls.Add(Me.vsbMap, 2, 0)
        Me.TableLayoutPanel8.Controls.Add(Me.dgvMap, 1, 0)
        Me.TableLayoutPanel8.Controls.Add(Me.pnlMap, 0, 0)
        Me.TableLayoutPanel8.Dock = System.Windows.Forms.DockStyle.Left
        Me.TableLayoutPanel8.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel8.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel8.Name = "TableLayoutPanel8"
        Me.TableLayoutPanel8.RowCount = 1
        Me.TableLayoutPanel8.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel8.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 774.0!))
        Me.TableLayoutPanel8.Size = New System.Drawing.Size(592, 774)
        Me.TableLayoutPanel8.TabIndex = 1
        '
        'dgvMap
        '
        Me.dgvMap.bTanspose = False
        Me.dgvMap.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvMap.GridColor = System.Drawing.SystemColors.Control
        Me.dgvMap.Location = New System.Drawing.Point(10, 0)
        Me.dgvMap.Margin = New System.Windows.Forms.Padding(0)
        Me.dgvMap.Name = "dgvMap"
        Me.dgvMap.RowTemplate.Height = 21
        Me.dgvMap.Size = New System.Drawing.Size(566, 774)
        Me.dgvMap.TabIndex = 212
        '
        'pnlMap
        '
        Me.pnlMap.Controls.Add(Me.lblKensaPoint)
        Me.pnlMap.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnlMap.Location = New System.Drawing.Point(0, 0)
        Me.pnlMap.Margin = New System.Windows.Forms.Padding(0)
        Me.pnlMap.Name = "pnlMap"
        Me.pnlMap.Size = New System.Drawing.Size(10, 774)
        Me.pnlMap.TabIndex = 217
        '
        'lblKensaPoint
        '
        Me.lblKensaPoint.BackColor = System.Drawing.Color.Green
        Me.lblKensaPoint.Location = New System.Drawing.Point(0, 526)
        Me.lblKensaPoint.Margin = New System.Windows.Forms.Padding(0)
        Me.lblKensaPoint.Name = "lblKensaPoint"
        Me.lblKensaPoint.Size = New System.Drawing.Size(10, 20)
        Me.lblKensaPoint.TabIndex = 0
        Me.lblKensaPoint.Visible = False
        '
        'UcCoil1
        '
        Me.UcCoil1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcCoil1.Location = New System.Drawing.Point(0, 1)
        Me.UcCoil1.Margin = New System.Windows.Forms.Padding(0, 1, 0, 1)
        Me.UcCoil1.Name = "UcCoil1"
        Me.UcCoil1.Size = New System.Drawing.Size(1270, 64)
        Me.UcCoil1.TabIndex = 254
        '
        'btnShift
        '
        Me.btnShift.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.btnShift.Location = New System.Drawing.Point(2, 936)
        Me.btnShift.Name = "btnShift"
        Me.btnShift.Size = New System.Drawing.Size(8, 48)
        Me.btnShift.TabIndex = 291
        Me.btnShift.UseVisualStyleBackColor = True
        '
        'frmJisseki
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1272, 990)
        Me.Controls.Add(Me.btnShift)
        Me.Controls.Add(Me.TableLayoutPanel3)
        Me.Name = "frmJisseki"
        Me.Text = "[1CAPL中央] 実績画面"
        Me.Controls.SetChildIndex(Me.TableLayoutPanel3, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.btnShift, 0)
        Me.TableLayoutPanel3.ResumeLayout(False)
        Me.TableLayoutPanel4.ResumeLayout(False)
        Me.TableLayoutPanel5.ResumeLayout(False)
        Me.TableLayoutPanel6.ResumeLayout(False)
        Me.TableLayoutPanel7.ResumeLayout(False)
        CType(Me.dgvDeep, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvOth1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TableLayoutPanel8.ResumeLayout(False)
        CType(Me.dgvMap, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlMap.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents Button2 As System.Windows.Forms.Button
    Friend WithEvents Button3 As System.Windows.Forms.Button
    Friend WithEvents Button4 As System.Windows.Forms.Button
    Friend WithEvents Button5 As System.Windows.Forms.Button
    Friend WithEvents Button6 As System.Windows.Forms.Button
    Protected Friend WithEvents Label21 As System.Windows.Forms.Label
    Protected Friend WithEvents Label28 As System.Windows.Forms.Label
    Protected Friend WithEvents Label32 As System.Windows.Forms.Label
    Protected Friend WithEvents Label34 As System.Windows.Forms.Label
    Protected Friend WithEvents Label35 As System.Windows.Forms.Label
    Protected Friend WithEvents Label37 As System.Windows.Forms.Label
    Friend WithEvents vsbMap As tClassLibrary.tCnt.tCnt_VScrollBar
    Friend WithEvents dgvDeep As SO_GAMENN.tCnt_DataGridView_turn
    Friend WithEvents dgvMap As SO_GAMENN.tCnt_DataGridView_turn
    Friend WithEvents UcDefect1 As SO_GAMENN.ucDefect
    Friend WithEvents UcDefect2 As SO_GAMENN.ucDefect
    Friend WithEvents TableLayoutPanel3 As TableLayoutPanel
    Friend WithEvents TableLayoutPanel4 As TableLayoutPanel
    Friend WithEvents TableLayoutPanel5 As TableLayoutPanel
    Friend WithEvents TableLayoutPanel6 As TableLayoutPanel
    Friend WithEvents TableLayoutPanel7 As TableLayoutPanel
    Friend WithEvents TableLayoutPanel8 As TableLayoutPanel
    Friend WithEvents pnlMap As Panel
    Friend WithEvents lblKensaPoint As Label
    Friend WithEvents dgvOth1 As tCnt_DataGridView_turn
    Friend WithEvents UcCoil1 As ucCoil
    Friend WithEvents btnShift As Button
End Class
