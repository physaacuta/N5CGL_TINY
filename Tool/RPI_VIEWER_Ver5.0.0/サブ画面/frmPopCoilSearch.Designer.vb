<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopCoilSearch
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
        Me.components = New System.ComponentModel.Container
        Dim DataGridViewCellStyle1 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle
        Dim DataGridViewCellStyle2 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle
        Dim DataGridViewCellStyle3 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.GroupBox4 = New System.Windows.Forms.GroupBox
        Me.btnScodeAllNg = New System.Windows.Forms.Button
        Me.btnScodeAllOk = New System.Windows.Forms.Button
        Me.clbScode = New System.Windows.Forms.CheckedListBox
        Me.GroupBox3 = New System.Windows.Forms.GroupBox
        Me.chkDay = New System.Windows.Forms.CheckBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.dtpTimeEnd = New System.Windows.Forms.DateTimePicker
        Me.dtpDateStart = New System.Windows.Forms.DateTimePicker
        Me.dtpDateEnd = New System.Windows.Forms.DateTimePicker
        Me.dtpTimeStart = New System.Windows.Forms.DateTimePicker
        Me.txtKizukenNo = New System.Windows.Forms.TextBox
        Me.txtSelect9 = New System.Windows.Forms.TextBox
        Me.txtSelect4 = New System.Windows.Forms.TextBox
        Me.txtSelect8 = New System.Windows.Forms.TextBox
        Me.txtSelect3 = New System.Windows.Forms.TextBox
        Me.txtSelect7 = New System.Windows.Forms.TextBox
        Me.txtSelect6 = New System.Windows.Forms.TextBox
        Me.txtSelect2 = New System.Windows.Forms.TextBox
        Me.txtSelect5 = New System.Windows.Forms.TextBox
        Me.txtSelect1 = New System.Windows.Forms.TextBox
        Me.txtSelect0 = New System.Windows.Forms.TextBox
        Me.txtCoilNo = New System.Windows.Forms.TextBox
        Me.Label4 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.lblSelect9 = New System.Windows.Forms.Label
        Me.lblKizukenNo = New System.Windows.Forms.Label
        Me.lblSelect8 = New System.Windows.Forms.Label
        Me.lblSelect4 = New System.Windows.Forms.Label
        Me.lblSelect7 = New System.Windows.Forms.Label
        Me.lblSelect3 = New System.Windows.Forms.Label
        Me.lblSelect6 = New System.Windows.Forms.Label
        Me.lblSelect2 = New System.Windows.Forms.Label
        Me.lblSelect5 = New System.Windows.Forms.Label
        Me.lblSelect1 = New System.Windows.Forms.Label
        Me.lblSelect0 = New System.Windows.Forms.Label
        Me.lblCoilNo = New System.Windows.Forms.Label
        Me.btnSearch = New System.Windows.Forms.Button
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.cmbLine = New System.Windows.Forms.ComboBox
        Me.tmrRpiDsp = New System.Windows.Forms.Timer(Me.components)
        Me.btnBetween = New System.Windows.Forms.Button
        Me.txtMsg = New System.Windows.Forms.TextBox
        Me.btnAccessPass = New System.Windows.Forms.Button
        Me.dgvList = New tClassLibrary.tCnt.tCnt_DataGridView
        Me.chkCheckRpi = New System.Windows.Forms.CheckBox
        Me.chkRpiSaveSelect = New System.Windows.Forms.CheckBox
        Me.btnRpiSave = New System.Windows.Forms.Button
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        CType(Me.dgvList, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.GroupBox4)
        Me.GroupBox1.Controls.Add(Me.GroupBox3)
        Me.GroupBox1.Controls.Add(Me.txtKizukenNo)
        Me.GroupBox1.Controls.Add(Me.txtSelect9)
        Me.GroupBox1.Controls.Add(Me.txtSelect4)
        Me.GroupBox1.Controls.Add(Me.txtSelect8)
        Me.GroupBox1.Controls.Add(Me.txtSelect3)
        Me.GroupBox1.Controls.Add(Me.txtSelect7)
        Me.GroupBox1.Controls.Add(Me.txtSelect6)
        Me.GroupBox1.Controls.Add(Me.txtSelect2)
        Me.GroupBox1.Controls.Add(Me.txtSelect5)
        Me.GroupBox1.Controls.Add(Me.txtSelect1)
        Me.GroupBox1.Controls.Add(Me.txtSelect0)
        Me.GroupBox1.Controls.Add(Me.txtCoilNo)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.Label3)
        Me.GroupBox1.Controls.Add(Me.lblSelect9)
        Me.GroupBox1.Controls.Add(Me.lblKizukenNo)
        Me.GroupBox1.Controls.Add(Me.lblSelect8)
        Me.GroupBox1.Controls.Add(Me.lblSelect4)
        Me.GroupBox1.Controls.Add(Me.lblSelect7)
        Me.GroupBox1.Controls.Add(Me.lblSelect3)
        Me.GroupBox1.Controls.Add(Me.lblSelect6)
        Me.GroupBox1.Controls.Add(Me.lblSelect2)
        Me.GroupBox1.Controls.Add(Me.lblSelect5)
        Me.GroupBox1.Controls.Add(Me.lblSelect1)
        Me.GroupBox1.Controls.Add(Me.lblSelect0)
        Me.GroupBox1.Controls.Add(Me.lblCoilNo)
        Me.GroupBox1.Location = New System.Drawing.Point(4, 4)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(776, 204)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "検索条件"
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.btnScodeAllNg)
        Me.GroupBox4.Controls.Add(Me.btnScodeAllOk)
        Me.GroupBox4.Controls.Add(Me.clbScode)
        Me.GroupBox4.Location = New System.Drawing.Point(524, 12)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(248, 148)
        Me.GroupBox4.TabIndex = 7
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "検査表面状態フィルター"
        '
        'btnScodeAllNg
        '
        Me.btnScodeAllNg.Location = New System.Drawing.Point(80, 124)
        Me.btnScodeAllNg.Name = "btnScodeAllNg"
        Me.btnScodeAllNg.Size = New System.Drawing.Size(72, 20)
        Me.btnScodeAllNg.TabIndex = 6
        Me.btnScodeAllNg.Text = "全解除"
        Me.btnScodeAllNg.UseVisualStyleBackColor = True
        '
        'btnScodeAllOk
        '
        Me.btnScodeAllOk.Location = New System.Drawing.Point(4, 124)
        Me.btnScodeAllOk.Name = "btnScodeAllOk"
        Me.btnScodeAllOk.Size = New System.Drawing.Size(72, 20)
        Me.btnScodeAllOk.TabIndex = 6
        Me.btnScodeAllOk.Text = "全選択"
        Me.btnScodeAllOk.UseVisualStyleBackColor = True
        '
        'clbScode
        '
        Me.clbScode.CheckOnClick = True
        Me.clbScode.FormattingEnabled = True
        Me.clbScode.Location = New System.Drawing.Point(4, 16)
        Me.clbScode.Name = "clbScode"
        Me.clbScode.Size = New System.Drawing.Size(240, 102)
        Me.clbScode.TabIndex = 5
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.chkDay)
        Me.GroupBox3.Controls.Add(Me.Label6)
        Me.GroupBox3.Controls.Add(Me.dtpTimeEnd)
        Me.GroupBox3.Controls.Add(Me.dtpDateStart)
        Me.GroupBox3.Controls.Add(Me.dtpDateEnd)
        Me.GroupBox3.Controls.Add(Me.dtpTimeStart)
        Me.GroupBox3.Location = New System.Drawing.Point(8, 148)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(508, 52)
        Me.GroupBox3.TabIndex = 6
        Me.GroupBox3.TabStop = False
        '
        'chkDay
        '
        Me.chkDay.AutoSize = True
        Me.chkDay.Checked = True
        Me.chkDay.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkDay.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.chkDay.Location = New System.Drawing.Point(12, 0)
        Me.chkDay.Name = "chkDay"
        Me.chkDay.Size = New System.Drawing.Size(99, 17)
        Me.chkDay.TabIndex = 5
        Me.chkDay.Text = "日付フィルター"
        Me.chkDay.UseVisualStyleBackColor = True
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold)
        Me.Label6.Location = New System.Drawing.Point(236, 24)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(25, 16)
        Me.Label6.TabIndex = 4
        Me.Label6.Text = "～"
        '
        'dtpTimeEnd
        '
        Me.dtpTimeEnd.Font = New System.Drawing.Font("MS UI Gothic", 11.25!)
        Me.dtpTimeEnd.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpTimeEnd.Location = New System.Drawing.Point(388, 20)
        Me.dtpTimeEnd.Name = "dtpTimeEnd"
        Me.dtpTimeEnd.ShowUpDown = True
        Me.dtpTimeEnd.Size = New System.Drawing.Size(104, 22)
        Me.dtpTimeEnd.TabIndex = 3
        Me.dtpTimeEnd.Value = New Date(2007, 7, 19, 23, 59, 59, 0)
        '
        'dtpDateStart
        '
        Me.dtpDateStart.CalendarFont = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpDateStart.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpDateStart.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateStart.Location = New System.Drawing.Point(8, 20)
        Me.dtpDateStart.Name = "dtpDateStart"
        Me.dtpDateStart.Size = New System.Drawing.Size(120, 22)
        Me.dtpDateStart.TabIndex = 2
        '
        'dtpDateEnd
        '
        Me.dtpDateEnd.CalendarFont = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpDateEnd.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpDateEnd.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateEnd.Location = New System.Drawing.Point(264, 20)
        Me.dtpDateEnd.Name = "dtpDateEnd"
        Me.dtpDateEnd.Size = New System.Drawing.Size(120, 22)
        Me.dtpDateEnd.TabIndex = 2
        '
        'dtpTimeStart
        '
        Me.dtpTimeStart.Font = New System.Drawing.Font("MS UI Gothic", 11.25!)
        Me.dtpTimeStart.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpTimeStart.Location = New System.Drawing.Point(132, 20)
        Me.dtpTimeStart.Name = "dtpTimeStart"
        Me.dtpTimeStart.ShowUpDown = True
        Me.dtpTimeStart.Size = New System.Drawing.Size(104, 22)
        Me.dtpTimeStart.TabIndex = 3
        Me.dtpTimeStart.Value = New Date(2007, 7, 19, 0, 0, 0, 0)
        '
        'txtKizukenNo
        '
        Me.txtKizukenNo.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKizukenNo.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtKizukenNo.Location = New System.Drawing.Point(144, 36)
        Me.txtKizukenNo.Name = "txtKizukenNo"
        Me.txtKizukenNo.Size = New System.Drawing.Size(160, 22)
        Me.txtKizukenNo.TabIndex = 1
        '
        'txtSelect9
        '
        Me.txtSelect9.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect9.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect9.Location = New System.Drawing.Point(408, 120)
        Me.txtSelect9.Name = "txtSelect9"
        Me.txtSelect9.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect9.TabIndex = 1
        '
        'txtSelect4
        '
        Me.txtSelect4.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect4.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect4.Location = New System.Drawing.Point(408, 80)
        Me.txtSelect4.Name = "txtSelect4"
        Me.txtSelect4.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect4.TabIndex = 1
        '
        'txtSelect8
        '
        Me.txtSelect8.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect8.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect8.Location = New System.Drawing.Point(308, 120)
        Me.txtSelect8.Name = "txtSelect8"
        Me.txtSelect8.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect8.TabIndex = 1
        '
        'txtSelect3
        '
        Me.txtSelect3.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect3.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect3.Location = New System.Drawing.Point(308, 80)
        Me.txtSelect3.Name = "txtSelect3"
        Me.txtSelect3.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect3.TabIndex = 1
        '
        'txtSelect7
        '
        Me.txtSelect7.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect7.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect7.Location = New System.Drawing.Point(208, 120)
        Me.txtSelect7.Name = "txtSelect7"
        Me.txtSelect7.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect7.TabIndex = 1
        '
        'txtSelect6
        '
        Me.txtSelect6.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect6.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect6.Location = New System.Drawing.Point(108, 120)
        Me.txtSelect6.Name = "txtSelect6"
        Me.txtSelect6.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect6.TabIndex = 1
        '
        'txtSelect2
        '
        Me.txtSelect2.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect2.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect2.Location = New System.Drawing.Point(208, 80)
        Me.txtSelect2.Name = "txtSelect2"
        Me.txtSelect2.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect2.TabIndex = 1
        '
        'txtSelect5
        '
        Me.txtSelect5.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect5.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect5.Location = New System.Drawing.Point(8, 120)
        Me.txtSelect5.Name = "txtSelect5"
        Me.txtSelect5.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect5.TabIndex = 1
        '
        'txtSelect1
        '
        Me.txtSelect1.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect1.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect1.Location = New System.Drawing.Point(108, 80)
        Me.txtSelect1.Name = "txtSelect1"
        Me.txtSelect1.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect1.TabIndex = 1
        '
        'txtSelect0
        '
        Me.txtSelect0.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtSelect0.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtSelect0.Location = New System.Drawing.Point(8, 80)
        Me.txtSelect0.Name = "txtSelect0"
        Me.txtSelect0.Size = New System.Drawing.Size(100, 22)
        Me.txtSelect0.TabIndex = 1
        '
        'txtCoilNo
        '
        Me.txtCoilNo.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtCoilNo.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtCoilNo.Location = New System.Drawing.Point(8, 36)
        Me.txtCoilNo.Name = "txtCoilNo"
        Me.txtCoilNo.Size = New System.Drawing.Size(132, 22)
        Me.txtCoilNo.TabIndex = 1
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(312, 40)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(122, 13)
        Me.Label4.TabIndex = 0
        Me.Label4.Text = "_ ： 任意1文字の一致"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(312, 20)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(125, 13)
        Me.Label3.TabIndex = 0
        Me.Label3.Text = "% ： 0文字以上の一致"
        '
        'lblSelect9
        '
        Me.lblSelect9.AutoSize = True
        Me.lblSelect9.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect9.Location = New System.Drawing.Point(412, 104)
        Me.lblSelect9.Name = "lblSelect9"
        Me.lblSelect9.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect9.TabIndex = 0
        Me.lblSelect9.Text = "拡張0"
        '
        'lblKizukenNo
        '
        Me.lblKizukenNo.AutoSize = True
        Me.lblKizukenNo.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKizukenNo.Location = New System.Drawing.Point(148, 20)
        Me.lblKizukenNo.Name = "lblKizukenNo"
        Me.lblKizukenNo.Size = New System.Drawing.Size(48, 13)
        Me.lblKizukenNo.TabIndex = 0
        Me.lblKizukenNo.Text = "管理No"
        '
        'lblSelect8
        '
        Me.lblSelect8.AutoSize = True
        Me.lblSelect8.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect8.Location = New System.Drawing.Point(308, 104)
        Me.lblSelect8.Name = "lblSelect8"
        Me.lblSelect8.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect8.TabIndex = 0
        Me.lblSelect8.Text = "拡張0"
        '
        'lblSelect4
        '
        Me.lblSelect4.AutoSize = True
        Me.lblSelect4.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect4.Location = New System.Drawing.Point(412, 64)
        Me.lblSelect4.Name = "lblSelect4"
        Me.lblSelect4.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect4.TabIndex = 0
        Me.lblSelect4.Text = "拡張0"
        '
        'lblSelect7
        '
        Me.lblSelect7.AutoSize = True
        Me.lblSelect7.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect7.Location = New System.Drawing.Point(212, 104)
        Me.lblSelect7.Name = "lblSelect7"
        Me.lblSelect7.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect7.TabIndex = 0
        Me.lblSelect7.Text = "拡張0"
        '
        'lblSelect3
        '
        Me.lblSelect3.AutoSize = True
        Me.lblSelect3.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect3.Location = New System.Drawing.Point(308, 64)
        Me.lblSelect3.Name = "lblSelect3"
        Me.lblSelect3.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect3.TabIndex = 0
        Me.lblSelect3.Text = "拡張0"
        '
        'lblSelect6
        '
        Me.lblSelect6.AutoSize = True
        Me.lblSelect6.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect6.Location = New System.Drawing.Point(112, 104)
        Me.lblSelect6.Name = "lblSelect6"
        Me.lblSelect6.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect6.TabIndex = 0
        Me.lblSelect6.Text = "拡張0"
        '
        'lblSelect2
        '
        Me.lblSelect2.AutoSize = True
        Me.lblSelect2.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect2.Location = New System.Drawing.Point(212, 64)
        Me.lblSelect2.Name = "lblSelect2"
        Me.lblSelect2.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect2.TabIndex = 0
        Me.lblSelect2.Text = "拡張0"
        '
        'lblSelect5
        '
        Me.lblSelect5.AutoSize = True
        Me.lblSelect5.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect5.Location = New System.Drawing.Point(8, 104)
        Me.lblSelect5.Name = "lblSelect5"
        Me.lblSelect5.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect5.TabIndex = 0
        Me.lblSelect5.Text = "拡張0"
        '
        'lblSelect1
        '
        Me.lblSelect1.AutoSize = True
        Me.lblSelect1.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect1.Location = New System.Drawing.Point(112, 64)
        Me.lblSelect1.Name = "lblSelect1"
        Me.lblSelect1.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect1.TabIndex = 0
        Me.lblSelect1.Text = "拡張0"
        '
        'lblSelect0
        '
        Me.lblSelect0.AutoSize = True
        Me.lblSelect0.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSelect0.Location = New System.Drawing.Point(8, 64)
        Me.lblSelect0.Name = "lblSelect0"
        Me.lblSelect0.Size = New System.Drawing.Size(40, 13)
        Me.lblSelect0.TabIndex = 0
        Me.lblSelect0.Text = "拡張0"
        '
        'lblCoilNo
        '
        Me.lblCoilNo.AutoSize = True
        Me.lblCoilNo.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblCoilNo.Location = New System.Drawing.Point(8, 20)
        Me.lblCoilNo.Name = "lblCoilNo"
        Me.lblCoilNo.Size = New System.Drawing.Size(126, 13)
        Me.lblCoilNo.TabIndex = 0
        Me.lblCoilNo.Text = "コイルNo等(対象材No)"
        '
        'btnSearch
        '
        Me.btnSearch.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSearch.Location = New System.Drawing.Point(792, 64)
        Me.btnSearch.Name = "btnSearch"
        Me.btnSearch.Size = New System.Drawing.Size(156, 44)
        Me.btnSearch.TabIndex = 3
        Me.btnSearch.Text = "F5：検索"
        Me.btnSearch.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.cmbLine)
        Me.GroupBox2.Location = New System.Drawing.Point(784, 4)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(172, 56)
        Me.GroupBox2.TabIndex = 18
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "対象ライン"
        '
        'cmbLine
        '
        Me.cmbLine.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbLine.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbLine.FormattingEnabled = True
        Me.cmbLine.Location = New System.Drawing.Point(4, 20)
        Me.cmbLine.Name = "cmbLine"
        Me.cmbLine.Size = New System.Drawing.Size(164, 24)
        Me.cmbLine.TabIndex = 0
        '
        'tmrRpiDsp
        '
        '
        'btnBetween
        '
        Me.btnBetween.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnBetween.Location = New System.Drawing.Point(792, 124)
        Me.btnBetween.Name = "btnBetween"
        Me.btnBetween.Size = New System.Drawing.Size(156, 44)
        Me.btnBetween.TabIndex = 3
        Me.btnBetween.Text = "F8：選択行から" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "前後10件抽出"
        Me.btnBetween.UseVisualStyleBackColor = True
        '
        'txtMsg
        '
        Me.txtMsg.BackColor = System.Drawing.SystemColors.Control
        Me.txtMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold)
        Me.txtMsg.ForeColor = System.Drawing.Color.Black
        Me.txtMsg.Location = New System.Drawing.Point(4, 212)
        Me.txtMsg.Multiline = True
        Me.txtMsg.Name = "txtMsg"
        Me.txtMsg.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.txtMsg.Size = New System.Drawing.Size(776, 40)
        Me.txtMsg.TabIndex = 19
        Me.txtMsg.Text = "10件見つかりました。"
        '
        'btnAccessPass
        '
        Me.btnAccessPass.Location = New System.Drawing.Point(792, 216)
        Me.btnAccessPass.Name = "btnAccessPass"
        Me.btnAccessPass.Size = New System.Drawing.Size(104, 32)
        Me.btnAccessPass.TabIndex = 20
        Me.btnAccessPass.Text = "RPI参照フォルダ" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "アクセス権限設定"
        Me.btnAccessPass.UseVisualStyleBackColor = True
        '
        'dgvList
        '
        Me.dgvList.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        DataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control
        DataGridViewCellStyle1.Font = New System.Drawing.Font("MS UI Gothic", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        DataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText
        DataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.[True]
        Me.dgvList.ColumnHeadersDefaultCellStyle = DataGridViewCellStyle1
        DataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window
        DataGridViewCellStyle2.Font = New System.Drawing.Font("MS UI Gothic", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        DataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText
        DataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.[False]
        Me.dgvList.DefaultCellStyle = DataGridViewCellStyle2
        Me.dgvList.Location = New System.Drawing.Point(4, 252)
        Me.dgvList.Name = "dgvList"
        DataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control
        DataGridViewCellStyle3.Font = New System.Drawing.Font("MS UI Gothic", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        DataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText
        DataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight
        DataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText
        DataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.[True]
        Me.dgvList.RowHeadersDefaultCellStyle = DataGridViewCellStyle3
        Me.dgvList.RowTemplate.Height = 21
        Me.dgvList.Size = New System.Drawing.Size(1129, 451)
        Me.dgvList.TabIndex = 8
        '
        'chkCheckRpi
        '
        Me.chkCheckRpi.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkCheckRpi.Location = New System.Drawing.Point(792, 180)
        Me.chkCheckRpi.Name = "chkCheckRpi"
        Me.chkCheckRpi.Size = New System.Drawing.Size(104, 32)
        Me.chkCheckRpi.TabIndex = 21
        Me.chkCheckRpi.Text = "全長画像" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ﾌｧｲﾙ有無ﾁｪｯｸ"
        Me.chkCheckRpi.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkCheckRpi.UseVisualStyleBackColor = True
        '
        'chkRpiSaveSelect
        '
        Me.chkRpiSaveSelect.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkRpiSaveSelect.AutoSize = True
        Me.chkRpiSaveSelect.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.chkRpiSaveSelect.Location = New System.Drawing.Point(940, 224)
        Me.chkRpiSaveSelect.Name = "chkRpiSaveSelect"
        Me.chkRpiSaveSelect.Size = New System.Drawing.Size(88, 23)
        Me.chkRpiSaveSelect.TabIndex = 22
        Me.chkRpiSaveSelect.Text = "RPI保存選択"
        Me.chkRpiSaveSelect.UseVisualStyleBackColor = True
        '
        'btnRpiSave
        '
        Me.btnRpiSave.Font = New System.Drawing.Font("MS UI Gothic", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRpiSave.Location = New System.Drawing.Point(1032, 224)
        Me.btnRpiSave.Name = "btnRpiSave"
        Me.btnRpiSave.Size = New System.Drawing.Size(88, 24)
        Me.btnRpiSave.TabIndex = 23
        Me.btnRpiSave.Text = "RPI保存実行"
        Me.btnRpiSave.UseVisualStyleBackColor = True
        '
        'frmPopCoilSearch
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1141, 695)
        Me.Controls.Add(Me.btnRpiSave)
        Me.Controls.Add(Me.chkRpiSaveSelect)
        Me.Controls.Add(Me.chkCheckRpi)
        Me.Controls.Add(Me.btnAccessPass)
        Me.Controls.Add(Me.txtMsg)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.dgvList)
        Me.Controls.Add(Me.btnBetween)
        Me.Controls.Add(Me.btnSearch)
        Me.Controls.Add(Me.GroupBox1)
        Me.KeyPreview = True
        Me.MinimizeBox = False
        Me.Name = "frmPopCoilSearch"
        Me.Text = "コイル検索画面"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        CType(Me.dgvList, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents txtKizukenNo As System.Windows.Forms.TextBox
    Friend WithEvents txtCoilNo As System.Windows.Forms.TextBox
    Friend WithEvents lblKizukenNo As System.Windows.Forms.Label
    Friend WithEvents lblCoilNo As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents dtpDateStart As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents Label6 As System.Windows.Forms.Label
    Protected Friend WithEvents dtpTimeEnd As System.Windows.Forms.DateTimePicker
    Friend WithEvents dtpDateEnd As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpTimeStart As System.Windows.Forms.DateTimePicker
    Friend WithEvents btnSearch As System.Windows.Forms.Button
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents cmbLine As System.Windows.Forms.ComboBox
    Friend WithEvents dgvList As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents tmrRpiDsp As System.Windows.Forms.Timer
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents chkDay As System.Windows.Forms.CheckBox
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents clbScode As System.Windows.Forms.CheckedListBox
    Friend WithEvents btnScodeAllNg As System.Windows.Forms.Button
    Friend WithEvents btnScodeAllOk As System.Windows.Forms.Button
    Friend WithEvents txtSelect0 As System.Windows.Forms.TextBox
    Friend WithEvents lblSelect0 As System.Windows.Forms.Label
    Friend WithEvents txtSelect3 As System.Windows.Forms.TextBox
    Friend WithEvents txtSelect2 As System.Windows.Forms.TextBox
    Friend WithEvents txtSelect1 As System.Windows.Forms.TextBox
    Friend WithEvents lblSelect3 As System.Windows.Forms.Label
    Friend WithEvents lblSelect2 As System.Windows.Forms.Label
    Friend WithEvents lblSelect1 As System.Windows.Forms.Label
    Friend WithEvents txtSelect4 As System.Windows.Forms.TextBox
    Friend WithEvents lblSelect4 As System.Windows.Forms.Label
    Friend WithEvents btnBetween As System.Windows.Forms.Button
    Friend WithEvents txtSelect9 As System.Windows.Forms.TextBox
    Friend WithEvents txtSelect8 As System.Windows.Forms.TextBox
    Friend WithEvents txtSelect7 As System.Windows.Forms.TextBox
    Friend WithEvents txtSelect6 As System.Windows.Forms.TextBox
    Friend WithEvents txtSelect5 As System.Windows.Forms.TextBox
    Friend WithEvents lblSelect9 As System.Windows.Forms.Label
    Friend WithEvents lblSelect8 As System.Windows.Forms.Label
    Friend WithEvents lblSelect7 As System.Windows.Forms.Label
    Friend WithEvents lblSelect6 As System.Windows.Forms.Label
    Friend WithEvents lblSelect5 As System.Windows.Forms.Label
    Friend WithEvents txtMsg As System.Windows.Forms.TextBox
    Friend WithEvents btnAccessPass As System.Windows.Forms.Button
    Friend WithEvents chkCheckRpi As System.Windows.Forms.CheckBox
    Friend WithEvents chkRpiSaveSelect As System.Windows.Forms.CheckBox
    Friend WithEvents btnRpiSave As System.Windows.Forms.Button
End Class
