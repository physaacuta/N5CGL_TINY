<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyDefectInf
    Inherits System.Windows.Forms.Form

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
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
        Me.txtTitle = New System.Windows.Forms.Label()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbRecord = New System.Windows.Forms.ComboBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.nudPoint = New System.Windows.Forms.NumericUpDown()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.dgvDefect1 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.dgvPath = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.txtMeiNo = New System.Windows.Forms.TextBox()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.txtKizuKenNo = New System.Windows.Forms.TextBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.txtNum = New System.Windows.Forms.TextBox()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.btnJmp = New System.Windows.Forms.Button()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.txtJmpKizu = New System.Windows.Forms.TextBox()
        CType(Me.nudPoint, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvDefect1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvPath, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(0, 8)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(171, 21)
        Me.txtTitle.TabIndex = 30
        Me.txtTitle.Text = "疵情報テーブル"
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(552, 4)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 35
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(552, 60)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 34
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(552, 32)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 33
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.txtInfo.Location = New System.Drawing.Point(176, 4)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.txtInfo.Size = New System.Drawing.Size(368, 80)
        Me.txtInfo.TabIndex = 36
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(4, 33)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(78, 18)
        Me.Label1.TabIndex = 38
        Me.Label1.Text = "レコード"
        '
        'cmbRecord
        '
        Me.cmbRecord.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbRecord.FormattingEnabled = True
        Me.cmbRecord.Location = New System.Drawing.Point(84, 30)
        Me.cmbRecord.Name = "cmbRecord"
        Me.cmbRecord.Size = New System.Drawing.Size(84, 24)
        Me.cmbRecord.TabIndex = 37
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(4, 58)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(78, 18)
        Me.Label2.TabIndex = 39
        Me.Label2.Text = "ポイント"
        '
        'nudPoint
        '
        Me.nudPoint.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.nudPoint.Location = New System.Drawing.Point(84, 56)
        Me.nudPoint.Maximum = New Decimal(New Integer() {200000, 0, 0, 0})
        Me.nudPoint.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudPoint.Name = "nudPoint"
        Me.nudPoint.Size = New System.Drawing.Size(84, 23)
        Me.nudPoint.TabIndex = 40
        Me.nudPoint.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudPoint.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(14, 200)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(59, 16)
        Me.Label3.TabIndex = 42
        Me.Label3.Text = "疵情報"
        '
        'dgvDefect1
        '
        Me.dgvDefect1.Location = New System.Drawing.Point(12, 220)
        Me.dgvDefect1.Name = "dgvDefect1"
        Me.dgvDefect1.RowTemplate.Height = 21
        Me.dgvDefect1.Size = New System.Drawing.Size(660, 376)
        Me.dgvDefect1.TabIndex = 41
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(12, 600)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(76, 16)
        Me.Label4.TabIndex = 44
        Me.Label4.Text = "画像パス"
        '
        'dgvPath
        '
        Me.dgvPath.Location = New System.Drawing.Point(12, 621)
        Me.dgvPath.Name = "dgvPath"
        Me.dgvPath.RowTemplate.Height = 21
        Me.dgvPath.Size = New System.Drawing.Size(568, 86)
        Me.dgvPath.TabIndex = 45
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.Label8)
        Me.GroupBox1.Controls.Add(Me.txtMeiNo)
        Me.GroupBox1.Controls.Add(Me.Label5)
        Me.GroupBox1.Controls.Add(Me.txtKizuKenNo)
        Me.GroupBox1.Controls.Add(Me.Label6)
        Me.GroupBox1.Controls.Add(Me.txtNum)
        Me.GroupBox1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(4, 144)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(608, 52)
        Me.GroupBox1.TabIndex = 73
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ヘッダー情報"
        '
        'Label8
        '
        Me.Label8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label8.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(264, 20)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(60, 24)
        Me.Label8.TabIndex = 78
        Me.Label8.Text = "命令No"
        Me.Label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtMeiNo
        '
        Me.txtMeiNo.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtMeiNo.Location = New System.Drawing.Point(324, 20)
        Me.txtMeiNo.Name = "txtMeiNo"
        Me.txtMeiNo.Size = New System.Drawing.Size(56, 23)
        Me.txtMeiNo.TabIndex = 79
        Me.txtMeiNo.Text = "1234"
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label5.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(4, 20)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(68, 24)
        Me.Label5.TabIndex = 4
        Me.Label5.Text = "管理No"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtKizuKenNo
        '
        Me.txtKizuKenNo.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKizuKenNo.Location = New System.Drawing.Point(72, 20)
        Me.txtKizuKenNo.Name = "txtKizuKenNo"
        Me.txtKizuKenNo.Size = New System.Drawing.Size(180, 23)
        Me.txtKizuKenNo.TabIndex = 27
        Me.txtKizuKenNo.Text = "12345678901234567890"
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(388, 20)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(128, 24)
        Me.Label6.TabIndex = 5
        Me.Label6.Text = "現在の格納件数"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtNum
        '
        Me.txtNum.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtNum.Location = New System.Drawing.Point(516, 20)
        Me.txtNum.Name = "txtNum"
        Me.txtNum.Size = New System.Drawing.Size(80, 23)
        Me.txtNum.TabIndex = 28
        Me.txtNum.Text = "123456"
        Me.txtNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.btnJmp)
        Me.GroupBox2.Controls.Add(Me.Label10)
        Me.GroupBox2.Controls.Add(Me.txtJmpKizu)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(4, 88)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(324, 48)
        Me.GroupBox2.TabIndex = 89
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "検索"
        '
        'btnJmp
        '
        Me.btnJmp.Location = New System.Drawing.Point(243, 20)
        Me.btnJmp.Name = "btnJmp"
        Me.btnJmp.Size = New System.Drawing.Size(75, 23)
        Me.btnJmp.TabIndex = 31
        Me.btnJmp.Text = "GO"
        Me.btnJmp.UseVisualStyleBackColor = True
        '
        'Label10
        '
        Me.Label10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label10.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.Location = New System.Drawing.Point(4, 20)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(76, 24)
        Me.Label10.TabIndex = 29
        Me.Label10.Text = "疵No"
        Me.Label10.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtJmpKizu
        '
        Me.txtJmpKizu.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtJmpKizu.Location = New System.Drawing.Point(80, 20)
        Me.txtJmpKizu.Name = "txtJmpKizu"
        Me.txtJmpKizu.Size = New System.Drawing.Size(80, 23)
        Me.txtJmpKizu.TabIndex = 30
        Me.txtJmpKizu.Text = "1"
        Me.txtJmpKizu.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'frmHyDefectInf
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(690, 712)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.dgvPath)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.dgvDefect1)
        Me.Controls.Add(Me.nudPoint)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbRecord)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.txtTitle)
        Me.Name = "frmHyDefectInf"
        Me.Text = "疵情報テーブル [TBL_HY_DEFECTINF]"
        CType(Me.nudPoint, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvDefect1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvPath, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txtTitle As System.Windows.Forms.Label
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents cmbRecord As System.Windows.Forms.ComboBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents nudPoint As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents dgvDefect1 As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents dgvPath As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents txtKizuKenNo As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents txtNum As System.Windows.Forms.TextBox
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents txtMeiNo As System.Windows.Forms.TextBox
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents btnJmp As System.Windows.Forms.Button
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents txtJmpKizu As System.Windows.Forms.TextBox
End Class
