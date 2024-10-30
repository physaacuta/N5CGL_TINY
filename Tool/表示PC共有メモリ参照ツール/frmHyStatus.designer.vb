<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyStatus
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
        Me.Label1 = New System.Windows.Forms.Label()
        Me.txtUnten = New System.Windows.Forms.TextBox()
        Me.txtKadou = New System.Windows.Forms.TextBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.txtKensa0 = New System.Windows.Forms.TextBox()
        Me.txtKiki0 = New System.Windows.Forms.TextBox()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.txtKiki1 = New System.Windows.Forms.TextBox()
        Me.txtKensa1 = New System.Windows.Forms.TextBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.txtCnt = New System.Windows.Forms.TextBox()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.dgvNgInf = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.txtKiki1Text = New System.Windows.Forms.TextBox()
        Me.txtKensa1Text = New System.Windows.Forms.TextBox()
        Me.txtKiki0Text = New System.Windows.Forms.TextBox()
        Me.txtKensa0Text = New System.Windows.Forms.TextBox()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.txtKadouText = New System.Windows.Forms.TextBox()
        Me.txtUntenText = New System.Windows.Forms.TextBox()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.txtTitle = New System.Windows.Forms.Label()
        CType(Me.dgvNgInf, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(8, 20)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(180, 24)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "運転状態"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtUnten
        '
        Me.txtUnten.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtUnten.Location = New System.Drawing.Point(188, 20)
        Me.txtUnten.Name = "txtUnten"
        Me.txtUnten.Size = New System.Drawing.Size(60, 23)
        Me.txtUnten.TabIndex = 2
        Me.txtUnten.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtKadou
        '
        Me.txtKadou.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKadou.Location = New System.Drawing.Point(188, 48)
        Me.txtKadou.Name = "txtKadou"
        Me.txtKadou.Size = New System.Drawing.Size(60, 23)
        Me.txtKadou.TabIndex = 4
        Me.txtKadou.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(8, 48)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(180, 24)
        Me.Label2.TabIndex = 3
        Me.Label2.Text = "稼動状態"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtKensa0
        '
        Me.txtKensa0.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKensa0.Location = New System.Drawing.Point(188, 36)
        Me.txtKensa0.Name = "txtKensa0"
        Me.txtKensa0.Size = New System.Drawing.Size(32, 19)
        Me.txtKensa0.TabIndex = 6
        Me.txtKensa0.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtKiki0
        '
        Me.txtKiki0.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKiki0.Location = New System.Drawing.Point(188, 64)
        Me.txtKiki0.Name = "txtKiki0"
        Me.txtKiki0.Size = New System.Drawing.Size(32, 19)
        Me.txtKiki0.TabIndex = 8
        Me.txtKiki0.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(8, 64)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(180, 24)
        Me.Label4.TabIndex = 7
        Me.Label4.Text = "機器異常度"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtKiki1
        '
        Me.txtKiki1.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKiki1.Location = New System.Drawing.Point(376, 64)
        Me.txtKiki1.Name = "txtKiki1"
        Me.txtKiki1.Size = New System.Drawing.Size(32, 19)
        Me.txtKiki1.TabIndex = 12
        Me.txtKiki1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtKensa1
        '
        Me.txtKensa1.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKensa1.Location = New System.Drawing.Point(376, 36)
        Me.txtKensa1.Name = "txtKensa1"
        Me.txtKensa1.Size = New System.Drawing.Size(32, 19)
        Me.txtKensa1.TabIndex = 10
        Me.txtKensa1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(8, 36)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(180, 24)
        Me.Label6.TabIndex = 9
        Me.Label6.Text = "検査異常度"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.Location = New System.Drawing.Point(8, 92)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(180, 24)
        Me.Label7.TabIndex = 13
        Me.Label7.Text = "異常セット件数"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtCnt
        '
        Me.txtCnt.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtCnt.Location = New System.Drawing.Point(188, 92)
        Me.txtCnt.Name = "txtCnt"
        Me.txtCnt.Size = New System.Drawing.Size(180, 23)
        Me.txtCnt.TabIndex = 14
        Me.txtCnt.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(560, 36)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 40
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(560, 64)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 39
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'dgvNgInf
        '
        Me.dgvNgInf.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvNgInf.Location = New System.Drawing.Point(4, 20)
        Me.dgvNgInf.Name = "dgvNgInf"
        Me.dgvNgInf.RowTemplate.Height = 21
        Me.dgvNgInf.Size = New System.Drawing.Size(664, 128)
        Me.dgvNgInf.TabIndex = 41
        '
        'Label9
        '
        Me.Label9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label9.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.Location = New System.Drawing.Point(188, 12)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(184, 24)
        Me.Label9.TabIndex = 5
        Me.Label9.Text = "表"
        Me.Label9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label10
        '
        Me.Label10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label10.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.Location = New System.Drawing.Point(376, 12)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(184, 24)
        Me.Label10.TabIndex = 5
        Me.Label10.Text = "裏"
        Me.Label10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.Label6)
        Me.GroupBox1.Controls.Add(Me.Label10)
        Me.GroupBox1.Controls.Add(Me.Label9)
        Me.GroupBox1.Controls.Add(Me.txtKiki1Text)
        Me.GroupBox1.Controls.Add(Me.txtKensa1Text)
        Me.GroupBox1.Controls.Add(Me.txtKiki0Text)
        Me.GroupBox1.Controls.Add(Me.txtKensa0Text)
        Me.GroupBox1.Controls.Add(Me.txtKensa0)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.txtCnt)
        Me.GroupBox1.Controls.Add(Me.txtKiki0)
        Me.GroupBox1.Controls.Add(Me.Label7)
        Me.GroupBox1.Controls.Add(Me.txtKensa1)
        Me.GroupBox1.Controls.Add(Me.txtKiki1)
        Me.GroupBox1.Location = New System.Drawing.Point(4, 129)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(568, 124)
        Me.GroupBox1.TabIndex = 42
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ｽﾃｰﾀｽ"
        '
        'txtKiki1Text
        '
        Me.txtKiki1Text.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKiki1Text.Location = New System.Drawing.Point(408, 64)
        Me.txtKiki1Text.Name = "txtKiki1Text"
        Me.txtKiki1Text.ReadOnly = True
        Me.txtKiki1Text.Size = New System.Drawing.Size(152, 19)
        Me.txtKiki1Text.TabIndex = 6
        '
        'txtKensa1Text
        '
        Me.txtKensa1Text.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKensa1Text.Location = New System.Drawing.Point(408, 36)
        Me.txtKensa1Text.Name = "txtKensa1Text"
        Me.txtKensa1Text.ReadOnly = True
        Me.txtKensa1Text.Size = New System.Drawing.Size(152, 19)
        Me.txtKensa1Text.TabIndex = 6
        '
        'txtKiki0Text
        '
        Me.txtKiki0Text.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKiki0Text.Location = New System.Drawing.Point(220, 64)
        Me.txtKiki0Text.Name = "txtKiki0Text"
        Me.txtKiki0Text.ReadOnly = True
        Me.txtKiki0Text.Size = New System.Drawing.Size(152, 19)
        Me.txtKiki0Text.TabIndex = 6
        '
        'txtKensa0Text
        '
        Me.txtKensa0Text.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKensa0Text.Location = New System.Drawing.Point(220, 36)
        Me.txtKensa0Text.Name = "txtKensa0Text"
        Me.txtKensa0Text.ReadOnly = True
        Me.txtKensa0Text.Size = New System.Drawing.Size(152, 19)
        Me.txtKensa0Text.TabIndex = 6
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.Label1)
        Me.GroupBox2.Controls.Add(Me.txtKadouText)
        Me.GroupBox2.Controls.Add(Me.txtUntenText)
        Me.GroupBox2.Controls.Add(Me.txtUnten)
        Me.GroupBox2.Controls.Add(Me.Label2)
        Me.GroupBox2.Controls.Add(Me.txtKadou)
        Me.GroupBox2.Location = New System.Drawing.Point(4, 44)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(520, 79)
        Me.GroupBox2.TabIndex = 43
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "ｽﾃｰﾀｽ情報"
        '
        'txtKadouText
        '
        Me.txtKadouText.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtKadouText.Location = New System.Drawing.Point(248, 48)
        Me.txtKadouText.Name = "txtKadouText"
        Me.txtKadouText.ReadOnly = True
        Me.txtKadouText.Size = New System.Drawing.Size(260, 23)
        Me.txtKadouText.TabIndex = 2
        '
        'txtUntenText
        '
        Me.txtUntenText.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtUntenText.Location = New System.Drawing.Point(248, 20)
        Me.txtUntenText.Name = "txtUntenText"
        Me.txtUntenText.ReadOnly = True
        Me.txtUntenText.Size = New System.Drawing.Size(260, 23)
        Me.txtUntenText.TabIndex = 2
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.dgvNgInf)
        Me.GroupBox3.Location = New System.Drawing.Point(4, 258)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(672, 152)
        Me.GroupBox3.TabIndex = 44
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "機器情報"
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(560, 8)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 45
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(8, 12)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(263, 21)
        Me.txtTitle.TabIndex = 46
        Me.txtTitle.Text = "ステータス情報テーブル"
        '
        'frmHyStatus
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(7.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(684, 423)
        Me.Controls.Add(Me.txtTitle)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.btnWrite)
        Me.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "frmHyStatus"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "ステータス情報 [TBL_HY_STATUS]"
        CType(Me.dgvNgInf, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents txtUnten As System.Windows.Forms.TextBox
    Friend WithEvents txtKadou As System.Windows.Forms.TextBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents txtKensa0 As System.Windows.Forms.TextBox
    Friend WithEvents txtKiki0 As System.Windows.Forms.TextBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents txtKiki1 As System.Windows.Forms.TextBox
    Friend WithEvents txtKensa1 As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents txtCnt As System.Windows.Forms.TextBox
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents dgvNgInf As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents txtKiki1Text As System.Windows.Forms.TextBox
    Friend WithEvents txtKensa1Text As System.Windows.Forms.TextBox
    Friend WithEvents txtKiki0Text As System.Windows.Forms.TextBox
    Friend WithEvents txtKensa0Text As System.Windows.Forms.TextBox
    Friend WithEvents txtKadouText As System.Windows.Forms.TextBox
    Friend WithEvents txtUntenText As System.Windows.Forms.TextBox
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents txtTitle As System.Windows.Forms.Label
End Class
