<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmHyPoint
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
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbRecord = New System.Windows.Forms.ComboBox()
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.txtTitle = New System.Windows.Forms.Label()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.txtRecDisp3 = New System.Windows.Forms.TextBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.txtRecDisp2 = New System.Windows.Forms.TextBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label30 = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.txtRecDisp0 = New System.Windows.Forms.TextBox()
        Me.txtRecDisp1 = New System.Windows.Forms.TextBox()
        Me.Label32 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.txtRecWrite1 = New System.Windows.Forms.TextBox()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.txtRecWrite0 = New System.Windows.Forms.TextBox()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label29 = New System.Windows.Forms.Label()
        Me.txtRecWriteBase = New System.Windows.Forms.TextBox()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.txtRecDispBase = New System.Windows.Forms.TextBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        Me.Label35 = New System.Windows.Forms.Label()
        Me.Label36 = New System.Windows.Forms.Label()
        Me.Label37 = New System.Windows.Forms.Label()
        Me.Label38 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label18 = New System.Windows.Forms.Label()
        Me.txtRowDisp000 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp010 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp001 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp011 = New System.Windows.Forms.TextBox()
        Me.Label19 = New System.Windows.Forms.Label()
        Me.Label22 = New System.Windows.Forms.Label()
        Me.Label31 = New System.Windows.Forms.Label()
        Me.txtRowWrite000 = New System.Windows.Forms.TextBox()
        Me.txtRowWrite010 = New System.Windows.Forms.TextBox()
        Me.txtRowLast00 = New System.Windows.Forms.TextBox()
        Me.txtRowWrite001 = New System.Windows.Forms.TextBox()
        Me.txtRowWrite011 = New System.Windows.Forms.TextBox()
        Me.txtRowLast01 = New System.Windows.Forms.TextBox()
        Me.txtRowWrite002 = New System.Windows.Forms.TextBox()
        Me.txtRowWrite012 = New System.Windows.Forms.TextBox()
        Me.txtRowLast02 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp002 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp012 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp032 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp022 = New System.Windows.Forms.TextBox()
        Me.Label56 = New System.Windows.Forms.Label()
        Me.txtRowDisp031 = New System.Windows.Forms.TextBox()
        Me.Label55 = New System.Windows.Forms.Label()
        Me.txtRowDisp021 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp020 = New System.Windows.Forms.TextBox()
        Me.txtRowDisp030 = New System.Windows.Forms.TextBox()
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.Label20 = New System.Windows.Forms.Label()
        Me.Label21 = New System.Windows.Forms.Label()
        Me.Label23 = New System.Windows.Forms.Label()
        Me.Label24 = New System.Windows.Forms.Label()
        Me.txtLenDisp02 = New System.Windows.Forms.TextBox()
        Me.txtLenDisp03 = New System.Windows.Forms.TextBox()
        Me.txtLenDisp00 = New System.Windows.Forms.TextBox()
        Me.txtLenDisp01 = New System.Windows.Forms.TextBox()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.txtKizukenNo = New System.Windows.Forms.TextBox()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.txtDispStatus = New System.Windows.Forms.TextBox()
        Me.txtRefresh = New System.Windows.Forms.TextBox()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(12, 232)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(78, 18)
        Me.Label1.TabIndex = 45
        Me.Label1.Text = "レコード"
        '
        'cmbRecord
        '
        Me.cmbRecord.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbRecord.FormattingEnabled = True
        Me.cmbRecord.Location = New System.Drawing.Point(92, 229)
        Me.cmbRecord.Name = "cmbRecord"
        Me.cmbRecord.Size = New System.Drawing.Size(84, 24)
        Me.cmbRecord.TabIndex = 44
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.txtInfo.Location = New System.Drawing.Point(248, 4)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(451, 80)
        Me.txtInfo.TabIndex = 43
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(913, 4)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 42
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(913, 60)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 41
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(913, 32)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 40
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(4, 8)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(240, 21)
        Me.txtTitle.TabIndex = 39
        Me.txtTitle.Text = "ポインタ情報テーブル"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.txtRecDisp3)
        Me.GroupBox2.Controls.Add(Me.Label6)
        Me.GroupBox2.Controls.Add(Me.txtRecDisp2)
        Me.GroupBox2.Controls.Add(Me.Label3)
        Me.GroupBox2.Controls.Add(Me.Label30)
        Me.GroupBox2.Controls.Add(Me.Label11)
        Me.GroupBox2.Controls.Add(Me.txtRecDisp0)
        Me.GroupBox2.Controls.Add(Me.txtRecDisp1)
        Me.GroupBox2.Controls.Add(Me.Label32)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(526, 104)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(328, 154)
        Me.GroupBox2.TabIndex = 128
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "通板ポイント"
        '
        'txtRecDisp3
        '
        Me.txtRecDisp3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecDisp3.Location = New System.Drawing.Point(174, 116)
        Me.txtRecDisp3.Name = "txtRecDisp3"
        Me.txtRecDisp3.Size = New System.Drawing.Size(140, 23)
        Me.txtRecDisp3.TabIndex = 117
        Me.txtRecDisp3.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label6.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(16, 116)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(158, 24)
        Me.Label6.TabIndex = 118
        Me.Label6.Text = "表示位置(裏面)"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtRecDisp2
        '
        Me.txtRecDisp2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecDisp2.Location = New System.Drawing.Point(174, 92)
        Me.txtRecDisp2.Name = "txtRecDisp2"
        Me.txtRecDisp2.Size = New System.Drawing.Size(140, 23)
        Me.txtRecDisp2.TabIndex = 113
        Me.txtRecDisp2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(16, 92)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(158, 24)
        Me.Label3.TabIndex = 114
        Me.Label3.Text = "表示位置(表面)"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label30
        '
        Me.Label30.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label30.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label30.Location = New System.Drawing.Point(174, 20)
        Me.Label30.Name = "Label30"
        Me.Label30.Size = New System.Drawing.Size(140, 24)
        Me.Label30.TabIndex = 35
        Me.Label30.Text = "現在レコード"
        Me.Label30.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label11
        '
        Me.Label11.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label11.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label11.Location = New System.Drawing.Point(16, 44)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(158, 24)
        Me.Label11.TabIndex = 37
        Me.Label11.Text = "カメラ位置(表面)"
        Me.Label11.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtRecDisp0
        '
        Me.txtRecDisp0.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecDisp0.Location = New System.Drawing.Point(174, 44)
        Me.txtRecDisp0.Name = "txtRecDisp0"
        Me.txtRecDisp0.Size = New System.Drawing.Size(140, 23)
        Me.txtRecDisp0.TabIndex = 5
        Me.txtRecDisp0.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRecDisp1
        '
        Me.txtRecDisp1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecDisp1.Location = New System.Drawing.Point(174, 68)
        Me.txtRecDisp1.Name = "txtRecDisp1"
        Me.txtRecDisp1.Size = New System.Drawing.Size(140, 23)
        Me.txtRecDisp1.TabIndex = 102
        Me.txtRecDisp1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label32
        '
        Me.Label32.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label32.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label32.Location = New System.Drawing.Point(16, 68)
        Me.Label32.Name = "Label32"
        Me.Label32.Size = New System.Drawing.Size(158, 24)
        Me.Label32.TabIndex = 106
        Me.Label32.Text = "カメラ位置(裏面)"
        Me.Label32.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.txtRecWrite1)
        Me.GroupBox1.Controls.Add(Me.Label15)
        Me.GroupBox1.Controls.Add(Me.txtRecWrite0)
        Me.GroupBox1.Controls.Add(Me.Label12)
        Me.GroupBox1.Controls.Add(Me.Label29)
        Me.GroupBox1.Controls.Add(Me.txtRecWriteBase)
        Me.GroupBox1.Controls.Add(Me.Label13)
        Me.GroupBox1.Controls.Add(Me.txtRecDispBase)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(188, 102)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(312, 154)
        Me.GroupBox1.TabIndex = 129
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "レコード情報"
        '
        'txtRecWrite1
        '
        Me.txtRecWrite1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecWrite1.Location = New System.Drawing.Point(244, 68)
        Me.txtRecWrite1.Name = "txtRecWrite1"
        Me.txtRecWrite1.Size = New System.Drawing.Size(60, 23)
        Me.txtRecWrite1.TabIndex = 39
        Me.txtRecWrite1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label15
        '
        Me.Label15.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label15.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label15.Location = New System.Drawing.Point(204, 68)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(40, 24)
        Me.Label15.TabIndex = 45
        Me.Label15.Text = "裏"
        Me.Label15.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'txtRecWrite0
        '
        Me.txtRecWrite0.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecWrite0.Location = New System.Drawing.Point(244, 44)
        Me.txtRecWrite0.Name = "txtRecWrite0"
        Me.txtRecWrite0.Size = New System.Drawing.Size(60, 23)
        Me.txtRecWrite0.TabIndex = 38
        Me.txtRecWrite0.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label12
        '
        Me.Label12.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label12.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label12.Location = New System.Drawing.Point(204, 44)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(40, 24)
        Me.Label12.TabIndex = 44
        Me.Label12.Text = "表"
        Me.Label12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label29
        '
        Me.Label29.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label29.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label29.Location = New System.Drawing.Point(4, 44)
        Me.Label29.Name = "Label29"
        Me.Label29.Size = New System.Drawing.Size(200, 48)
        Me.Label29.TabIndex = 43
        Me.Label29.Text = "検査装置の現在レコード"
        Me.Label29.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtRecWriteBase
        '
        Me.txtRecWriteBase.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecWriteBase.Location = New System.Drawing.Point(244, 16)
        Me.txtRecWriteBase.Name = "txtRecWriteBase"
        Me.txtRecWriteBase.Size = New System.Drawing.Size(60, 23)
        Me.txtRecWriteBase.TabIndex = 37
        Me.txtRecWriteBase.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label13
        '
        Me.Label13.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label13.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label13.Location = New System.Drawing.Point(4, 16)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(240, 24)
        Me.Label13.TabIndex = 42
        Me.Label13.Text = "書き込みレコード"
        Me.Label13.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtRecDispBase
        '
        Me.txtRecDispBase.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRecDispBase.Location = New System.Drawing.Point(244, 96)
        Me.txtRecDispBase.Name = "txtRecDispBase"
        Me.txtRecDispBase.Size = New System.Drawing.Size(60, 23)
        Me.txtRecDispBase.TabIndex = 40
        Me.txtRecDispBase.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(4, 96)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(240, 24)
        Me.Label2.TabIndex = 46
        Me.Label2.Text = "表示基点位置の現在表示コイルレコード"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.GroupBox5)
        Me.GroupBox3.Controls.Add(Me.GroupBox4)
        Me.GroupBox3.Controls.Add(Me.Label16)
        Me.GroupBox3.Controls.Add(Me.txtKizukenNo)
        Me.GroupBox3.Controls.Add(Me.Label14)
        Me.GroupBox3.Controls.Add(Me.txtDispStatus)
        Me.GroupBox3.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox3.Location = New System.Drawing.Point(4, 264)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(1029, 362)
        Me.GroupBox3.TabIndex = 130
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "各レコード単位のポインタ情報"
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.Label35)
        Me.GroupBox5.Controls.Add(Me.Label36)
        Me.GroupBox5.Controls.Add(Me.Label37)
        Me.GroupBox5.Controls.Add(Me.Label38)
        Me.GroupBox5.Controls.Add(Me.Label7)
        Me.GroupBox5.Controls.Add(Me.Label8)
        Me.GroupBox5.Controls.Add(Me.Label18)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp000)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp010)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp001)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp011)
        Me.GroupBox5.Controls.Add(Me.Label19)
        Me.GroupBox5.Controls.Add(Me.Label22)
        Me.GroupBox5.Controls.Add(Me.Label31)
        Me.GroupBox5.Controls.Add(Me.txtRowWrite000)
        Me.GroupBox5.Controls.Add(Me.txtRowWrite010)
        Me.GroupBox5.Controls.Add(Me.txtRowLast00)
        Me.GroupBox5.Controls.Add(Me.txtRowWrite001)
        Me.GroupBox5.Controls.Add(Me.txtRowWrite011)
        Me.GroupBox5.Controls.Add(Me.txtRowLast01)
        Me.GroupBox5.Controls.Add(Me.txtRowWrite002)
        Me.GroupBox5.Controls.Add(Me.txtRowWrite012)
        Me.GroupBox5.Controls.Add(Me.txtRowLast02)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp002)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp012)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp032)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp022)
        Me.GroupBox5.Controls.Add(Me.Label56)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp031)
        Me.GroupBox5.Controls.Add(Me.Label55)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp021)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp020)
        Me.GroupBox5.Controls.Add(Me.txtRowDisp030)
        Me.GroupBox5.Location = New System.Drawing.Point(484, 28)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(501, 262)
        Me.GroupBox5.TabIndex = 179
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "各行"
        '
        'Label35
        '
        Me.Label35.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label35.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label35.Location = New System.Drawing.Point(119, 134)
        Me.Label35.Name = "Label35"
        Me.Label35.Size = New System.Drawing.Size(158, 24)
        Me.Label35.TabIndex = 128
        Me.Label35.Text = "表示位置(裏面)"
        Me.Label35.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label36
        '
        Me.Label36.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label36.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label36.Location = New System.Drawing.Point(119, 110)
        Me.Label36.Name = "Label36"
        Me.Label36.Size = New System.Drawing.Size(158, 24)
        Me.Label36.TabIndex = 127
        Me.Label36.Text = "表示位置(表面)"
        Me.Label36.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label37
        '
        Me.Label37.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label37.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label37.Location = New System.Drawing.Point(119, 62)
        Me.Label37.Name = "Label37"
        Me.Label37.Size = New System.Drawing.Size(158, 24)
        Me.Label37.TabIndex = 125
        Me.Label37.Text = "カメラ位置(表面)"
        Me.Label37.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label38
        '
        Me.Label38.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label38.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label38.Location = New System.Drawing.Point(119, 86)
        Me.Label38.Name = "Label38"
        Me.Label38.Size = New System.Drawing.Size(158, 24)
        Me.Label38.TabIndex = 126
        Me.Label38.Text = "カメラ位置(裏面)"
        Me.Label38.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.Location = New System.Drawing.Point(276, 37)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(64, 24)
        Me.Label7.TabIndex = 58
        Me.Label7.Text = "最小"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label8
        '
        Me.Label8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label8.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(340, 37)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(64, 24)
        Me.Label8.TabIndex = 59
        Me.Label8.Text = "小"
        Me.Label8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label18
        '
        Me.Label18.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label18.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label18.Location = New System.Drawing.Point(404, 37)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(64, 24)
        Me.Label18.TabIndex = 60
        Me.Label18.Text = "大"
        Me.Label18.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'txtRowDisp000
        '
        Me.txtRowDisp000.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp000.Location = New System.Drawing.Point(276, 61)
        Me.txtRowDisp000.Name = "txtRowDisp000"
        Me.txtRowDisp000.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp000.TabIndex = 19
        Me.txtRowDisp000.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp010
        '
        Me.txtRowDisp010.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp010.Location = New System.Drawing.Point(276, 85)
        Me.txtRowDisp010.Name = "txtRowDisp010"
        Me.txtRowDisp010.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp010.TabIndex = 22
        Me.txtRowDisp010.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp001
        '
        Me.txtRowDisp001.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp001.Location = New System.Drawing.Point(340, 61)
        Me.txtRowDisp001.Name = "txtRowDisp001"
        Me.txtRowDisp001.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp001.TabIndex = 20
        Me.txtRowDisp001.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp011
        '
        Me.txtRowDisp011.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp011.Location = New System.Drawing.Point(340, 85)
        Me.txtRowDisp011.Name = "txtRowDisp011"
        Me.txtRowDisp011.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp011.TabIndex = 23
        Me.txtRowDisp011.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label19
        '
        Me.Label19.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label19.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label19.Location = New System.Drawing.Point(8, 62)
        Me.Label19.Name = "Label19"
        Me.Label19.Size = New System.Drawing.Size(113, 96)
        Me.Label19.TabIndex = 50
        Me.Label19.Text = "通板行"
        Me.Label19.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label22
        '
        Me.Label22.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label22.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label22.Location = New System.Drawing.Point(9, 167)
        Me.Label22.Name = "Label22"
        Me.Label22.Size = New System.Drawing.Size(228, 48)
        Me.Label22.TabIndex = 55
        Me.Label22.Text = "現在書込み行"
        Me.Label22.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label31
        '
        Me.Label31.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label31.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label31.Location = New System.Drawing.Point(9, 219)
        Me.Label31.Name = "Label31"
        Me.Label31.Size = New System.Drawing.Size(268, 24)
        Me.Label31.TabIndex = 55
        Me.Label31.Text = "検査完了行"
        Me.Label31.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'txtRowWrite000
        '
        Me.txtRowWrite000.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowWrite000.Location = New System.Drawing.Point(277, 167)
        Me.txtRowWrite000.Name = "txtRowWrite000"
        Me.txtRowWrite000.Size = New System.Drawing.Size(64, 23)
        Me.txtRowWrite000.TabIndex = 25
        Me.txtRowWrite000.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowWrite010
        '
        Me.txtRowWrite010.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowWrite010.Location = New System.Drawing.Point(277, 191)
        Me.txtRowWrite010.Name = "txtRowWrite010"
        Me.txtRowWrite010.Size = New System.Drawing.Size(64, 23)
        Me.txtRowWrite010.TabIndex = 28
        Me.txtRowWrite010.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowLast00
        '
        Me.txtRowLast00.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowLast00.Location = New System.Drawing.Point(277, 219)
        Me.txtRowLast00.Name = "txtRowLast00"
        Me.txtRowLast00.Size = New System.Drawing.Size(64, 23)
        Me.txtRowLast00.TabIndex = 28
        Me.txtRowLast00.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowWrite001
        '
        Me.txtRowWrite001.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowWrite001.Location = New System.Drawing.Point(341, 167)
        Me.txtRowWrite001.Name = "txtRowWrite001"
        Me.txtRowWrite001.Size = New System.Drawing.Size(64, 23)
        Me.txtRowWrite001.TabIndex = 26
        Me.txtRowWrite001.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowWrite011
        '
        Me.txtRowWrite011.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowWrite011.Location = New System.Drawing.Point(341, 191)
        Me.txtRowWrite011.Name = "txtRowWrite011"
        Me.txtRowWrite011.Size = New System.Drawing.Size(64, 23)
        Me.txtRowWrite011.TabIndex = 29
        Me.txtRowWrite011.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowLast01
        '
        Me.txtRowLast01.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowLast01.Location = New System.Drawing.Point(341, 219)
        Me.txtRowLast01.Name = "txtRowLast01"
        Me.txtRowLast01.Size = New System.Drawing.Size(64, 23)
        Me.txtRowLast01.TabIndex = 29
        Me.txtRowLast01.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowWrite002
        '
        Me.txtRowWrite002.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowWrite002.Location = New System.Drawing.Point(405, 167)
        Me.txtRowWrite002.Name = "txtRowWrite002"
        Me.txtRowWrite002.Size = New System.Drawing.Size(64, 23)
        Me.txtRowWrite002.TabIndex = 27
        Me.txtRowWrite002.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowWrite012
        '
        Me.txtRowWrite012.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowWrite012.Location = New System.Drawing.Point(405, 191)
        Me.txtRowWrite012.Name = "txtRowWrite012"
        Me.txtRowWrite012.Size = New System.Drawing.Size(64, 23)
        Me.txtRowWrite012.TabIndex = 30
        Me.txtRowWrite012.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowLast02
        '
        Me.txtRowLast02.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowLast02.Location = New System.Drawing.Point(405, 219)
        Me.txtRowLast02.Name = "txtRowLast02"
        Me.txtRowLast02.Size = New System.Drawing.Size(64, 23)
        Me.txtRowLast02.TabIndex = 30
        Me.txtRowLast02.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp002
        '
        Me.txtRowDisp002.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp002.Location = New System.Drawing.Point(404, 61)
        Me.txtRowDisp002.Name = "txtRowDisp002"
        Me.txtRowDisp002.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp002.TabIndex = 21
        Me.txtRowDisp002.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp012
        '
        Me.txtRowDisp012.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp012.Location = New System.Drawing.Point(404, 85)
        Me.txtRowDisp012.Name = "txtRowDisp012"
        Me.txtRowDisp012.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp012.TabIndex = 24
        Me.txtRowDisp012.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp032
        '
        Me.txtRowDisp032.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp032.Location = New System.Drawing.Point(404, 133)
        Me.txtRowDisp032.Name = "txtRowDisp032"
        Me.txtRowDisp032.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp032.TabIndex = 142
        Me.txtRowDisp032.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp022
        '
        Me.txtRowDisp022.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp022.Location = New System.Drawing.Point(404, 109)
        Me.txtRowDisp022.Name = "txtRowDisp022"
        Me.txtRowDisp022.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp022.TabIndex = 139
        Me.txtRowDisp022.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label56
        '
        Me.Label56.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label56.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label56.Location = New System.Drawing.Point(237, 167)
        Me.Label56.Name = "Label56"
        Me.Label56.Size = New System.Drawing.Size(40, 24)
        Me.Label56.TabIndex = 135
        Me.Label56.Text = "表"
        Me.Label56.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'txtRowDisp031
        '
        Me.txtRowDisp031.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp031.Location = New System.Drawing.Point(340, 133)
        Me.txtRowDisp031.Name = "txtRowDisp031"
        Me.txtRowDisp031.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp031.TabIndex = 141
        Me.txtRowDisp031.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label55
        '
        Me.Label55.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label55.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label55.Location = New System.Drawing.Point(237, 191)
        Me.Label55.Name = "Label55"
        Me.Label55.Size = New System.Drawing.Size(40, 24)
        Me.Label55.TabIndex = 136
        Me.Label55.Text = "裏"
        Me.Label55.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'txtRowDisp021
        '
        Me.txtRowDisp021.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp021.Location = New System.Drawing.Point(340, 109)
        Me.txtRowDisp021.Name = "txtRowDisp021"
        Me.txtRowDisp021.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp021.TabIndex = 138
        Me.txtRowDisp021.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp020
        '
        Me.txtRowDisp020.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp020.Location = New System.Drawing.Point(276, 109)
        Me.txtRowDisp020.Name = "txtRowDisp020"
        Me.txtRowDisp020.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp020.TabIndex = 137
        Me.txtRowDisp020.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRowDisp030
        '
        Me.txtRowDisp030.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRowDisp030.Location = New System.Drawing.Point(276, 133)
        Me.txtRowDisp030.Name = "txtRowDisp030"
        Me.txtRowDisp030.Size = New System.Drawing.Size(64, 23)
        Me.txtRowDisp030.TabIndex = 140
        Me.txtRowDisp030.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.Label20)
        Me.GroupBox4.Controls.Add(Me.Label21)
        Me.GroupBox4.Controls.Add(Me.Label23)
        Me.GroupBox4.Controls.Add(Me.Label24)
        Me.GroupBox4.Controls.Add(Me.txtLenDisp02)
        Me.GroupBox4.Controls.Add(Me.txtLenDisp03)
        Me.GroupBox4.Controls.Add(Me.txtLenDisp00)
        Me.GroupBox4.Controls.Add(Me.txtLenDisp01)
        Me.GroupBox4.Controls.Add(Me.Label5)
        Me.GroupBox4.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox4.Location = New System.Drawing.Point(4, 92)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(474, 198)
        Me.GroupBox4.TabIndex = 127
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "通板距離[m]"
        '
        'Label20
        '
        Me.Label20.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label20.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label20.Location = New System.Drawing.Point(142, 116)
        Me.Label20.Name = "Label20"
        Me.Label20.Size = New System.Drawing.Size(158, 24)
        Me.Label20.TabIndex = 128
        Me.Label20.Text = "表示位置(裏面)"
        Me.Label20.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label21
        '
        Me.Label21.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label21.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label21.Location = New System.Drawing.Point(142, 92)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(158, 24)
        Me.Label21.TabIndex = 127
        Me.Label21.Text = "表示位置(表面)"
        Me.Label21.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label23
        '
        Me.Label23.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label23.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label23.Location = New System.Drawing.Point(142, 44)
        Me.Label23.Name = "Label23"
        Me.Label23.Size = New System.Drawing.Size(158, 24)
        Me.Label23.TabIndex = 125
        Me.Label23.Text = "カメラ位置(表面)"
        Me.Label23.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label24
        '
        Me.Label24.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label24.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label24.Location = New System.Drawing.Point(142, 68)
        Me.Label24.Name = "Label24"
        Me.Label24.Size = New System.Drawing.Size(158, 24)
        Me.Label24.TabIndex = 126
        Me.Label24.Text = "カメラ位置(裏面)"
        Me.Label24.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtLenDisp02
        '
        Me.txtLenDisp02.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtLenDisp02.Location = New System.Drawing.Point(300, 92)
        Me.txtLenDisp02.Name = "txtLenDisp02"
        Me.txtLenDisp02.Size = New System.Drawing.Size(80, 23)
        Me.txtLenDisp02.TabIndex = 133
        Me.txtLenDisp02.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtLenDisp03
        '
        Me.txtLenDisp03.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtLenDisp03.Location = New System.Drawing.Point(300, 116)
        Me.txtLenDisp03.Name = "txtLenDisp03"
        Me.txtLenDisp03.Size = New System.Drawing.Size(80, 23)
        Me.txtLenDisp03.TabIndex = 134
        Me.txtLenDisp03.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtLenDisp00
        '
        Me.txtLenDisp00.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtLenDisp00.Location = New System.Drawing.Point(300, 44)
        Me.txtLenDisp00.Name = "txtLenDisp00"
        Me.txtLenDisp00.Size = New System.Drawing.Size(80, 23)
        Me.txtLenDisp00.TabIndex = 129
        Me.txtLenDisp00.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtLenDisp01
        '
        Me.txtLenDisp01.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtLenDisp01.Location = New System.Drawing.Point(300, 68)
        Me.txtLenDisp01.Name = "txtLenDisp01"
        Me.txtLenDisp01.Size = New System.Drawing.Size(80, 23)
        Me.txtLenDisp01.TabIndex = 130
        Me.txtLenDisp01.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label5.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(14, 44)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(128, 96)
        Me.Label5.TabIndex = 43
        Me.Label5.Text = "各通板ポイント通板距離[m]"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label16
        '
        Me.Label16.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label16.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label16.Location = New System.Drawing.Point(4, 32)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(120, 24)
        Me.Label16.TabIndex = 43
        Me.Label16.Text = "管理No"
        Me.Label16.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtKizukenNo
        '
        Me.txtKizukenNo.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKizukenNo.Location = New System.Drawing.Point(124, 32)
        Me.txtKizukenNo.Name = "txtKizukenNo"
        Me.txtKizukenNo.Size = New System.Drawing.Size(180, 23)
        Me.txtKizukenNo.TabIndex = 41
        Me.txtKizukenNo.Text = "12345678901234567890"
        '
        'Label14
        '
        Me.Label14.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Label14.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label14.Location = New System.Drawing.Point(4, 60)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(120, 24)
        Me.Label14.TabIndex = 44
        Me.Label14.Text = "画面表示状態"
        Me.Label14.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtDispStatus
        '
        Me.txtDispStatus.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtDispStatus.Location = New System.Drawing.Point(124, 60)
        Me.txtDispStatus.Name = "txtDispStatus"
        Me.txtDispStatus.Size = New System.Drawing.Size(40, 23)
        Me.txtDispStatus.TabIndex = 42
        Me.txtDispStatus.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtRefresh
        '
        Me.txtRefresh.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRefresh.Location = New System.Drawing.Point(120, 120)
        Me.txtRefresh.Name = "txtRefresh"
        Me.txtRefresh.Size = New System.Drawing.Size(60, 23)
        Me.txtRefresh.TabIndex = 131
        Me.txtRefresh.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(8, 120)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(112, 24)
        Me.Label4.TabIndex = 132
        Me.Label4.Text = "更新中フラグ"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'frmHyPoint
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1048, 569)
        Me.Controls.Add(Me.txtRefresh)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbRecord)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.txtTitle)
        Me.Name = "frmHyPoint"
        Me.Text = "ポインタ情報テーブル [TBL_HY_POINT]"
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox4.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents cmbRecord As System.Windows.Forms.ComboBox
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents txtTitle As System.Windows.Forms.Label
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents Label30 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents txtRecDisp0 As System.Windows.Forms.TextBox
    Friend WithEvents txtRecDisp1 As System.Windows.Forms.TextBox
    Friend WithEvents Label32 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents txtRecWrite1 As System.Windows.Forms.TextBox
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents txtRecWrite0 As System.Windows.Forms.TextBox
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Label29 As System.Windows.Forms.Label
    Friend WithEvents txtRecWriteBase As System.Windows.Forms.TextBox
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents txtRecDispBase As System.Windows.Forms.TextBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents txtLenDisp02 As System.Windows.Forms.TextBox
    Friend WithEvents txtLenDisp03 As System.Windows.Forms.TextBox
    Friend WithEvents txtLenDisp00 As System.Windows.Forms.TextBox
    Friend WithEvents txtLenDisp01 As System.Windows.Forms.TextBox
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents txtKizukenNo As System.Windows.Forms.TextBox
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents txtDispStatus As System.Windows.Forms.TextBox
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents txtRowDisp000 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp010 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp001 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp011 As System.Windows.Forms.TextBox
    Friend WithEvents Label19 As System.Windows.Forms.Label
    Friend WithEvents Label22 As System.Windows.Forms.Label
    Friend WithEvents Label31 As System.Windows.Forms.Label
    Friend WithEvents txtRowWrite000 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowWrite010 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowLast00 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowWrite001 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowWrite011 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowLast01 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowWrite002 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowWrite012 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowLast02 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp002 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp012 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp032 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp022 As System.Windows.Forms.TextBox
    Friend WithEvents Label56 As System.Windows.Forms.Label
    Friend WithEvents txtRowDisp031 As System.Windows.Forms.TextBox
    Friend WithEvents Label55 As System.Windows.Forms.Label
    Friend WithEvents txtRowDisp021 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp020 As System.Windows.Forms.TextBox
    Friend WithEvents txtRowDisp030 As System.Windows.Forms.TextBox
    Friend WithEvents txtRefresh As System.Windows.Forms.TextBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents txtRecDisp3 As TextBox
    Friend WithEvents Label6 As Label
    Friend WithEvents txtRecDisp2 As TextBox
    Friend WithEvents Label3 As Label
    Friend WithEvents Label35 As Label
    Friend WithEvents Label36 As Label
    Friend WithEvents Label37 As Label
    Friend WithEvents Label38 As Label
    Friend WithEvents Label20 As Label
    Friend WithEvents Label21 As Label
    Friend WithEvents Label23 As Label
    Friend WithEvents Label24 As Label
End Class
