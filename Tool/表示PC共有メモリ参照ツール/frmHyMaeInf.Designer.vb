<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyMaeinf
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
        Me.dgvMaeInf = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.dgvPath = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.txtNum = New System.Windows.Forms.TextBox()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.cmbMen = New System.Windows.Forms.ComboBox()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.txtMaeDsp = New System.Windows.Forms.TextBox()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.dgvFF = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.nudPoint = New System.Windows.Forms.NumericUpDown()
        Me.Label9 = New System.Windows.Forms.Label()
        CType(Me.dgvMaeInf, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvPath, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        CType(Me.dgvFF, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudPoint, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(0, 8)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(219, 19)
        Me.txtTitle.TabIndex = 30
        Me.txtTitle.Text = "前工程疵情報テーブル"
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(541, 6)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(112, 24)
        Me.chkUpdate.TabIndex = 35
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(541, 61)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(112, 24)
        Me.btnWrite.TabIndex = 34
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(541, 33)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(112, 24)
        Me.btnRead.TabIndex = 33
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.txtInfo.Location = New System.Drawing.Point(223, 7)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.txtInfo.Size = New System.Drawing.Size(309, 120)
        Me.txtInfo.TabIndex = 36
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(4, 32)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(107, 22)
        Me.Label1.TabIndex = 38
        Me.Label1.Text = "レコード"
        '
        'cmbRecord
        '
        Me.cmbRecord.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbRecord.FormattingEnabled = True
        Me.cmbRecord.Location = New System.Drawing.Point(113, 31)
        Me.cmbRecord.Name = "cmbRecord"
        Me.cmbRecord.Size = New System.Drawing.Size(84, 24)
        Me.cmbRecord.TabIndex = 37
        '
        'dgvMaeInf
        '
        Me.dgvMaeInf.Location = New System.Drawing.Point(340, 212)
        Me.dgvMaeInf.Name = "dgvMaeInf"
        Me.dgvMaeInf.RowTemplate.Height = 21
        Me.dgvMaeInf.Size = New System.Drawing.Size(330, 376)
        Me.dgvMaeInf.TabIndex = 41
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(4, 596)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(76, 16)
        Me.Label4.TabIndex = 44
        Me.Label4.Text = "画像パス"
        '
        'dgvPath
        '
        Me.dgvPath.Location = New System.Drawing.Point(4, 617)
        Me.dgvPath.Name = "dgvPath"
        Me.dgvPath.RowTemplate.Height = 21
        Me.dgvPath.Size = New System.Drawing.Size(568, 86)
        Me.dgvPath.TabIndex = 45
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.Label6)
        Me.GroupBox1.Controls.Add(Me.txtNum)
        Me.GroupBox1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(4, 134)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(236, 52)
        Me.GroupBox1.TabIndex = 73
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ヘッダー情報"
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(9, 20)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(128, 24)
        Me.Label6.TabIndex = 5
        Me.Label6.Text = "現在の格納件数"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtNum
        '
        Me.txtNum.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtNum.Location = New System.Drawing.Point(137, 20)
        Me.txtNum.Name = "txtNum"
        Me.txtNum.Size = New System.Drawing.Size(80, 23)
        Me.txtNum.TabIndex = 28
        Me.txtNum.Text = "123456"
        Me.txtNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.Location = New System.Drawing.Point(346, 193)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(110, 16)
        Me.Label7.TabIndex = 90
        Me.Label7.Text = "前工程疵情報"
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(4, 57)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(107, 21)
        Me.Label2.TabIndex = 39
        Me.Label2.Text = "表裏"
        '
        'cmbMen
        '
        Me.cmbMen.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbMen.FormattingEnabled = True
        Me.cmbMen.Location = New System.Drawing.Point(113, 55)
        Me.cmbMen.Name = "cmbMen"
        Me.cmbMen.Size = New System.Drawing.Size(84, 24)
        Me.cmbMen.TabIndex = 91
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label5.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(9, 20)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(159, 24)
        Me.Label5.TabIndex = 5
        Me.Label5.Text = "前工程疵の表示種類"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtMaeDsp
        '
        Me.txtMaeDsp.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtMaeDsp.Location = New System.Drawing.Point(170, 21)
        Me.txtMaeDsp.Name = "txtMaeDsp"
        Me.txtMaeDsp.Size = New System.Drawing.Size(80, 23)
        Me.txtMaeDsp.TabIndex = 28
        Me.txtMaeDsp.Text = "123456"
        Me.txtMaeDsp.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.Label5)
        Me.GroupBox3.Controls.Add(Me.txtMaeDsp)
        Me.GroupBox3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox3.Location = New System.Drawing.Point(259, 134)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(266, 52)
        Me.GroupBox3.TabIndex = 74
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "共通データ部"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(6, 192)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(60, 16)
        Me.Label8.TabIndex = 93
        Me.Label8.Text = "FF情報"
        '
        'dgvFF
        '
        Me.dgvFF.Location = New System.Drawing.Point(4, 212)
        Me.dgvFF.Name = "dgvFF"
        Me.dgvFF.RowTemplate.Height = 21
        Me.dgvFF.Size = New System.Drawing.Size(330, 376)
        Me.dgvFF.TabIndex = 92
        '
        'nudPoint
        '
        Me.nudPoint.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.nudPoint.Location = New System.Drawing.Point(113, 80)
        Me.nudPoint.Maximum = New Decimal(New Integer() {200000, 0, 0, 0})
        Me.nudPoint.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudPoint.Name = "nudPoint"
        Me.nudPoint.Size = New System.Drawing.Size(84, 23)
        Me.nudPoint.TabIndex = 97
        Me.nudPoint.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudPoint.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'Label9
        '
        Me.Label9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label9.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.Location = New System.Drawing.Point(4, 81)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(107, 21)
        Me.Label9.TabIndex = 96
        Me.Label9.Text = "ポインタ"
        '
        'frmHyMaeinf
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(685, 711)
        Me.Controls.Add(Me.nudPoint)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.dgvFF)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.cmbMen)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.dgvPath)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.dgvMaeInf)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbRecord)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.txtTitle)
        Me.Name = "frmHyMaeinf"
        Me.Text = "前工程疵情報テーブル [TBL_HY_MAE_INF]"
        CType(Me.dgvMaeInf, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvPath, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        CType(Me.dgvFF, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudPoint, System.ComponentModel.ISupportInitialize).EndInit()
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
    Friend WithEvents dgvMaeInf As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents dgvPath As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents txtNum As System.Windows.Forms.TextBox
    Friend WithEvents Label7 As Label
    Friend WithEvents Label2 As Label
    Friend WithEvents cmbMen As ComboBox
    Friend WithEvents Label5 As Label
    Friend WithEvents txtMaeDsp As TextBox
    Friend WithEvents GroupBox3 As GroupBox
    Friend WithEvents Label8 As Label
    Friend WithEvents dgvFF As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents nudPoint As NumericUpDown
    Friend WithEvents Label9 As Label
End Class
