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
        Me.components = New System.ComponentModel.Container
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.btnSetOffset = New System.Windows.Forms.Button
        Me.ckbSeq = New System.Windows.Forms.CheckBox
        Me.timCycle = New System.Windows.Forms.Timer(Me.components)
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.tlp11 = New System.Windows.Forms.TableLayoutPanel
        Me.cmb11_Rpi = New System.Windows.Forms.ComboBox
        Me.tlp13 = New System.Windows.Forms.TableLayoutPanel
        Me.lbl13_1 = New System.Windows.Forms.Label
        Me.lbl13_2 = New System.Windows.Forms.Label
        Me.lbl13_3 = New System.Windows.Forms.Label
        Me.tlp22 = New System.Windows.Forms.TableLayoutPanel
        Me.lbl22_1 = New System.Windows.Forms.Label
        Me.lbl22_2 = New System.Windows.Forms.Label
        Me.lbl22_3 = New System.Windows.Forms.Label
        Me.lbl22_4 = New System.Windows.Forms.Label
        Me.Label1 = New System.Windows.Forms.Label
        Me.cmbDspSelect = New System.Windows.Forms.ComboBox
        Me.cntItem1 = New RPI_SEQ.cntItem
        Me.cntItem2 = New RPI_SEQ.cntItem
        Me.cntItem3 = New RPI_SEQ.cntItem
        Me.cntItem4 = New RPI_SEQ.cntItem
        Me.cntItem5 = New RPI_SEQ.cntItem
        Me.TableLayoutPanel1.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.tlp11.SuspendLayout()
        Me.tlp13.SuspendLayout()
        Me.tlp22.SuspendLayout()
        Me.SuspendLayout()
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel1.ColumnCount = 1
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.cntItem1, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.cntItem2, 0, 1)
        Me.TableLayoutPanel1.Controls.Add(Me.cntItem3, 0, 2)
        Me.TableLayoutPanel1.Controls.Add(Me.cntItem4, 0, 3)
        Me.TableLayoutPanel1.Controls.Add(Me.cntItem5, 0, 4)
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 72)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 5
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(721, 356)
        Me.TableLayoutPanel1.TabIndex = 0
        '
        'GroupBox1
        '
        Me.GroupBox1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.GroupBox1.Controls.Add(Me.btnSetOffset)
        Me.GroupBox1.Controls.Add(Me.ckbSeq)
        Me.GroupBox1.Location = New System.Drawing.Point(0, 0)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(248, 68)
        Me.GroupBox1.TabIndex = 1
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "共通項目"
        '
        'btnSetOffset
        '
        Me.btnSetOffset.Location = New System.Drawing.Point(160, 16)
        Me.btnSetOffset.Name = "btnSetOffset"
        Me.btnSetOffset.Size = New System.Drawing.Size(80, 48)
        Me.btnSetOffset.TabIndex = 1
        Me.btnSetOffset.Text = "現在位置で" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "オフセット調整"
        Me.btnSetOffset.UseVisualStyleBackColor = True
        '
        'ckbSeq
        '
        Me.ckbSeq.Appearance = System.Windows.Forms.Appearance.Button
        Me.ckbSeq.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.ckbSeq.Location = New System.Drawing.Point(8, 16)
        Me.ckbSeq.Name = "ckbSeq"
        Me.ckbSeq.Size = New System.Drawing.Size(120, 48)
        Me.ckbSeq.TabIndex = 0
        Me.ckbSeq.Text = "連動解除"
        Me.ckbSeq.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.ckbSeq.UseVisualStyleBackColor = True
        '
        'timCycle
        '
        Me.timCycle.Interval = 60
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.tlp11)
        Me.GroupBox2.Controls.Add(Me.tlp13)
        Me.GroupBox2.Controls.Add(Me.tlp22)
        Me.GroupBox2.Controls.Add(Me.Label1)
        Me.GroupBox2.Controls.Add(Me.cmbDspSelect)
        Me.GroupBox2.Location = New System.Drawing.Point(452, 0)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(232, 68)
        Me.GroupBox2.TabIndex = 2
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "表示設定"
        '
        'tlp11
        '
        Me.tlp11.BackColor = System.Drawing.SystemColors.GradientActiveCaption
        Me.tlp11.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.OutsetDouble
        Me.tlp11.ColumnCount = 1
        Me.tlp11.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333!))
        Me.tlp11.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333!))
        Me.tlp11.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333!))
        Me.tlp11.Controls.Add(Me.cmb11_Rpi, 0, 0)
        Me.tlp11.Location = New System.Drawing.Point(64, 20)
        Me.tlp11.Name = "tlp11"
        Me.tlp11.RowCount = 1
        Me.tlp11.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlp11.Size = New System.Drawing.Size(48, 44)
        Me.tlp11.TabIndex = 5
        '
        'cmb11_Rpi
        '
        Me.cmb11_Rpi.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmb11_Rpi.FormattingEnabled = True
        Me.cmb11_Rpi.Location = New System.Drawing.Point(6, 6)
        Me.cmb11_Rpi.Name = "cmb11_Rpi"
        Me.cmb11_Rpi.Size = New System.Drawing.Size(36, 20)
        Me.cmb11_Rpi.TabIndex = 0
        Me.cmb11_Rpi.TabStop = False
        '
        'tlp13
        '
        Me.tlp13.BackColor = System.Drawing.SystemColors.GradientActiveCaption
        Me.tlp13.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.OutsetDouble
        Me.tlp13.ColumnCount = 3
        Me.tlp13.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333!))
        Me.tlp13.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333!))
        Me.tlp13.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333!))
        Me.tlp13.Controls.Add(Me.lbl13_1, 0, 0)
        Me.tlp13.Controls.Add(Me.lbl13_2, 1, 0)
        Me.tlp13.Controls.Add(Me.lbl13_3, 2, 0)
        Me.tlp13.Location = New System.Drawing.Point(116, 20)
        Me.tlp13.Name = "tlp13"
        Me.tlp13.RowCount = 1
        Me.tlp13.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlp13.Size = New System.Drawing.Size(52, 44)
        Me.tlp13.TabIndex = 5
        '
        'lbl13_1
        '
        Me.lbl13_1.AutoSize = True
        Me.lbl13_1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbl13_1.Location = New System.Drawing.Point(6, 3)
        Me.lbl13_1.Name = "lbl13_1"
        Me.lbl13_1.Size = New System.Drawing.Size(7, 38)
        Me.lbl13_1.TabIndex = 0
        Me.lbl13_1.Text = "1"
        Me.lbl13_1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lbl13_2
        '
        Me.lbl13_2.AutoSize = True
        Me.lbl13_2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbl13_2.Location = New System.Drawing.Point(22, 3)
        Me.lbl13_2.Name = "lbl13_2"
        Me.lbl13_2.Size = New System.Drawing.Size(7, 38)
        Me.lbl13_2.TabIndex = 1
        Me.lbl13_2.Text = "2"
        Me.lbl13_2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lbl13_3
        '
        Me.lbl13_3.AutoSize = True
        Me.lbl13_3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbl13_3.Location = New System.Drawing.Point(38, 3)
        Me.lbl13_3.Name = "lbl13_3"
        Me.lbl13_3.Size = New System.Drawing.Size(8, 38)
        Me.lbl13_3.TabIndex = 2
        Me.lbl13_3.Text = "3"
        Me.lbl13_3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'tlp22
        '
        Me.tlp22.BackColor = System.Drawing.SystemColors.GradientActiveCaption
        Me.tlp22.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.OutsetDouble
        Me.tlp22.ColumnCount = 2
        Me.tlp22.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tlp22.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tlp22.Controls.Add(Me.lbl22_1, 0, 0)
        Me.tlp22.Controls.Add(Me.lbl22_2, 1, 0)
        Me.tlp22.Controls.Add(Me.lbl22_3, 0, 1)
        Me.tlp22.Controls.Add(Me.lbl22_4, 1, 1)
        Me.tlp22.Location = New System.Drawing.Point(172, 20)
        Me.tlp22.Name = "tlp22"
        Me.tlp22.RowCount = 2
        Me.tlp22.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tlp22.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tlp22.Size = New System.Drawing.Size(52, 44)
        Me.tlp22.TabIndex = 5
        '
        'lbl22_1
        '
        Me.lbl22_1.AutoSize = True
        Me.lbl22_1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbl22_1.Location = New System.Drawing.Point(6, 3)
        Me.lbl22_1.Name = "lbl22_1"
        Me.lbl22_1.Size = New System.Drawing.Size(15, 17)
        Me.lbl22_1.TabIndex = 0
        Me.lbl22_1.Text = "１"
        Me.lbl22_1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lbl22_2
        '
        Me.lbl22_2.AutoSize = True
        Me.lbl22_2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbl22_2.Location = New System.Drawing.Point(30, 3)
        Me.lbl22_2.Name = "lbl22_2"
        Me.lbl22_2.Size = New System.Drawing.Size(16, 17)
        Me.lbl22_2.TabIndex = 1
        Me.lbl22_2.Text = "２"
        Me.lbl22_2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lbl22_3
        '
        Me.lbl22_3.AutoSize = True
        Me.lbl22_3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbl22_3.Location = New System.Drawing.Point(6, 23)
        Me.lbl22_3.Name = "lbl22_3"
        Me.lbl22_3.Size = New System.Drawing.Size(15, 18)
        Me.lbl22_3.TabIndex = 2
        Me.lbl22_3.Text = "３"
        Me.lbl22_3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lbl22_4
        '
        Me.lbl22_4.AutoSize = True
        Me.lbl22_4.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lbl22_4.Location = New System.Drawing.Point(30, 23)
        Me.lbl22_4.Name = "lbl22_4"
        Me.lbl22_4.Size = New System.Drawing.Size(16, 18)
        Me.lbl22_4.TabIndex = 3
        Me.lbl22_4.Text = "４"
        Me.lbl22_4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(8, 20)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(55, 12)
        Me.Label1.TabIndex = 4
        Me.Label1.Text = "モニタ番号"
        '
        'cmbDspSelect
        '
        Me.cmbDspSelect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbDspSelect.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbDspSelect.FormattingEnabled = True
        Me.cmbDspSelect.Location = New System.Drawing.Point(8, 36)
        Me.cmbDspSelect.Name = "cmbDspSelect"
        Me.cmbDspSelect.Size = New System.Drawing.Size(52, 23)
        Me.cmbDspSelect.TabIndex = 3
        '
        'cntItem1
        '
        Me.cntItem1.AllowDrop = True
        Me.cntItem1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.cntItem1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.cntItem1.Location = New System.Drawing.Point(3, 3)
        Me.cntItem1.Name = "cntItem1"
        Me.cntItem1.Prop_No = -1
        Me.cntItem1.Prop_Rate = 1
        Me.cntItem1.Size = New System.Drawing.Size(715, 65)
        Me.cntItem1.TabIndex = 0
        '
        'cntItem2
        '
        Me.cntItem2.AllowDrop = True
        Me.cntItem2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.cntItem2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.cntItem2.Location = New System.Drawing.Point(3, 74)
        Me.cntItem2.Name = "cntItem2"
        Me.cntItem2.Prop_No = -1
        Me.cntItem2.Prop_Rate = 1
        Me.cntItem2.Size = New System.Drawing.Size(715, 65)
        Me.cntItem2.TabIndex = 1
        '
        'cntItem3
        '
        Me.cntItem3.AllowDrop = True
        Me.cntItem3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.cntItem3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.cntItem3.Location = New System.Drawing.Point(3, 145)
        Me.cntItem3.Name = "cntItem3"
        Me.cntItem3.Prop_No = -1
        Me.cntItem3.Prop_Rate = 1
        Me.cntItem3.Size = New System.Drawing.Size(715, 65)
        Me.cntItem3.TabIndex = 2
        '
        'cntItem4
        '
        Me.cntItem4.AllowDrop = True
        Me.cntItem4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.cntItem4.Dock = System.Windows.Forms.DockStyle.Fill
        Me.cntItem4.Location = New System.Drawing.Point(3, 216)
        Me.cntItem4.Name = "cntItem4"
        Me.cntItem4.Prop_No = -1
        Me.cntItem4.Prop_Rate = 1
        Me.cntItem4.Size = New System.Drawing.Size(715, 65)
        Me.cntItem4.TabIndex = 3
        '
        'cntItem5
        '
        Me.cntItem5.AllowDrop = True
        Me.cntItem5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.cntItem5.Dock = System.Windows.Forms.DockStyle.Fill
        Me.cntItem5.Location = New System.Drawing.Point(3, 287)
        Me.cntItem5.Name = "cntItem5"
        Me.cntItem5.Prop_No = -1
        Me.cntItem5.Prop_Rate = 1
        Me.cntItem5.Size = New System.Drawing.Size(715, 66)
        Me.cntItem5.TabIndex = 4
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(721, 428)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Name = "frmMain"
        Me.Text = "RPI連動ツール"
        Me.TableLayoutPanel1.ResumeLayout(False)
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.tlp11.ResumeLayout(False)
        Me.tlp13.ResumeLayout(False)
        Me.tlp13.PerformLayout()
        Me.tlp22.ResumeLayout(False)
        Me.tlp22.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents TableLayoutPanel1 As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents cntItem1 As RPI_SEQ.cntItem
    Friend WithEvents cntItem2 As RPI_SEQ.cntItem
    Friend WithEvents cntItem3 As RPI_SEQ.cntItem
    Friend WithEvents cntItem4 As RPI_SEQ.cntItem
    Friend WithEvents cntItem5 As RPI_SEQ.cntItem
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents ckbSeq As System.Windows.Forms.CheckBox
    Friend WithEvents btnSetOffset As System.Windows.Forms.Button
    Friend WithEvents timCycle As System.Windows.Forms.Timer
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents cmbDspSelect As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents tlp22 As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents tlp13 As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents lbl22_1 As System.Windows.Forms.Label
    Friend WithEvents lbl22_2 As System.Windows.Forms.Label
    Friend WithEvents lbl22_3 As System.Windows.Forms.Label
    Friend WithEvents lbl22_4 As System.Windows.Forms.Label
    Friend WithEvents lbl13_1 As System.Windows.Forms.Label
    Friend WithEvents lbl13_2 As System.Windows.Forms.Label
    Friend WithEvents lbl13_3 As System.Windows.Forms.Label
    Friend WithEvents tlp11 As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents cmb11_Rpi As System.Windows.Forms.ComboBox

End Class
