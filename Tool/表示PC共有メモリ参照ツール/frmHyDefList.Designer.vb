<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyDefList
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
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.txtTitle = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.nudRec_O = New System.Windows.Forms.NumericUpDown()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.dgvPath_o = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvDeflist_o = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.txtRec_o = New System.Windows.Forms.TextBox()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.dgvPath_u = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvDefect_u = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvDeflist_u = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.txtRec_u = New System.Windows.Forms.TextBox()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.nudRec_U = New System.Windows.Forms.NumericUpDown()
        Me.dgvDefect_o = New tClassLibrary.tCnt.tCnt_DataGridView()
        CType(Me.nudRec_O, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        CType(Me.dgvPath_o, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvDeflist_o, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        CType(Me.dgvPath_u, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvDefect_u, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvDeflist_u, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudRec_U, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvDefect_o, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.txtInfo.Location = New System.Drawing.Point(228, 4)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(272, 80)
        Me.txtInfo.TabIndex = 45
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(504, 4)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 44
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(504, 60)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 43
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(504, 32)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 42
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(4, 8)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(217, 21)
        Me.txtTitle.TabIndex = 41
        Me.txtTitle.Text = "疵一覧情報テーブル"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(128, 54)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(78, 18)
        Me.Label1.TabIndex = 47
        Me.Label1.Text = "レコード"
        '
        'nudRec_O
        '
        Me.nudRec_O.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.nudRec_O.Location = New System.Drawing.Point(208, 52)
        Me.nudRec_O.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nudRec_O.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudRec_O.Name = "nudRec_O"
        Me.nudRec_O.Size = New System.Drawing.Size(84, 23)
        Me.nudRec_O.TabIndex = 48
        Me.nudRec_O.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudRec_O.Value = New Decimal(New Integer() {25, 0, 0, 0})
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.dgvPath_o)
        Me.GroupBox1.Controls.Add(Me.dgvDeflist_o)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.txtRec_o)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.nudRec_O)
        Me.GroupBox1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(12, 92)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(300, 676)
        Me.GroupBox1.TabIndex = 49
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "疵一覧情報 表"
        '
        'dgvPath_o
        '
        Me.dgvPath_o.Location = New System.Drawing.Point(8, 568)
        Me.dgvPath_o.Name = "dgvPath_o"
        Me.dgvPath_o.RowTemplate.Height = 21
        Me.dgvPath_o.Size = New System.Drawing.Size(284, 100)
        Me.dgvPath_o.TabIndex = 52
        '
        'dgvDeflist_o
        '
        Me.dgvDeflist_o.Location = New System.Drawing.Point(8, 80)
        Me.dgvDeflist_o.Name = "dgvDeflist_o"
        Me.dgvDeflist_o.RowTemplate.Height = 21
        Me.dgvDeflist_o.Size = New System.Drawing.Size(284, 182)
        Me.dgvDeflist_o.TabIndex = 51
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(60, 30)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(146, 18)
        Me.Label2.TabIndex = 50
        Me.Label2.Text = "最新レコード位置"
        '
        'txtRec_o
        '
        Me.txtRec_o.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRec_o.Location = New System.Drawing.Point(208, 28)
        Me.txtRec_o.Name = "txtRec_o"
        Me.txtRec_o.Size = New System.Drawing.Size(80, 23)
        Me.txtRec_o.TabIndex = 49
        Me.txtRec_o.Text = "123456"
        Me.txtRec_o.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dgvPath_u)
        Me.GroupBox2.Controls.Add(Me.dgvDefect_u)
        Me.GroupBox2.Controls.Add(Me.dgvDeflist_u)
        Me.GroupBox2.Controls.Add(Me.Label3)
        Me.GroupBox2.Controls.Add(Me.txtRec_u)
        Me.GroupBox2.Controls.Add(Me.Label4)
        Me.GroupBox2.Controls.Add(Me.nudRec_U)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(316, 92)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(300, 676)
        Me.GroupBox2.TabIndex = 52
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "疵一覧情報 裏"
        '
        'dgvPath_u
        '
        Me.dgvPath_u.Location = New System.Drawing.Point(8, 568)
        Me.dgvPath_u.Name = "dgvPath_u"
        Me.dgvPath_u.RowTemplate.Height = 21
        Me.dgvPath_u.Size = New System.Drawing.Size(284, 100)
        Me.dgvPath_u.TabIndex = 53
        '
        'dgvDefect_u
        '
        Me.dgvDefect_u.Location = New System.Drawing.Point(8, 292)
        Me.dgvDefect_u.Name = "dgvDefect_u"
        Me.dgvDefect_u.RowTemplate.Height = 21
        Me.dgvDefect_u.Size = New System.Drawing.Size(284, 268)
        Me.dgvDefect_u.TabIndex = 52
        '
        'dgvDeflist_u
        '
        Me.dgvDeflist_u.Location = New System.Drawing.Point(8, 80)
        Me.dgvDeflist_u.Name = "dgvDeflist_u"
        Me.dgvDeflist_u.RowTemplate.Height = 21
        Me.dgvDeflist_u.Size = New System.Drawing.Size(284, 182)
        Me.dgvDeflist_u.TabIndex = 51
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(60, 30)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(146, 18)
        Me.Label3.TabIndex = 50
        Me.Label3.Text = "最新レコード位置"
        '
        'txtRec_u
        '
        Me.txtRec_u.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRec_u.Location = New System.Drawing.Point(208, 28)
        Me.txtRec_u.Name = "txtRec_u"
        Me.txtRec_u.Size = New System.Drawing.Size(80, 23)
        Me.txtRec_u.TabIndex = 49
        Me.txtRec_u.Text = "123456"
        Me.txtRec_u.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(128, 54)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(78, 18)
        Me.Label4.TabIndex = 47
        Me.Label4.Text = "レコード"
        '
        'nudRec_U
        '
        Me.nudRec_U.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.nudRec_U.Location = New System.Drawing.Point(208, 52)
        Me.nudRec_U.Maximum = New Decimal(New Integer() {30, 0, 0, 0})
        Me.nudRec_U.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudRec_U.Name = "nudRec_U"
        Me.nudRec_U.Size = New System.Drawing.Size(84, 23)
        Me.nudRec_U.TabIndex = 48
        Me.nudRec_U.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudRec_U.Value = New Decimal(New Integer() {25, 0, 0, 0})
        '
        'dgvDefect_o
        '
        Me.dgvDefect_o.Location = New System.Drawing.Point(20, 384)
        Me.dgvDefect_o.Name = "dgvDefect_o"
        Me.dgvDefect_o.RowTemplate.Height = 21
        Me.dgvDefect_o.Size = New System.Drawing.Size(284, 268)
        Me.dgvDefect_o.TabIndex = 52
        '
        'frmHyDefList
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(627, 773)
        Me.Controls.Add(Me.dgvDefect_o)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.txtTitle)
        Me.Name = "frmHyDefList"
        Me.Text = "疵一覧表示ポインタ情報 [TBL_HY_DEFLIST]"
        CType(Me.nudRec_O, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.dgvPath_o, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvDeflist_o, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        CType(Me.dgvPath_u, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvDefect_u, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvDeflist_u, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudRec_U, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvDefect_o, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents txtTitle As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents nudRec_O As System.Windows.Forms.NumericUpDown
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents txtRec_o As System.Windows.Forms.TextBox
    Friend WithEvents dgvDeflist_o As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvDeflist_u As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents txtRec_u As System.Windows.Forms.TextBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents nudRec_U As System.Windows.Forms.NumericUpDown
    Friend WithEvents dgvDefect_o As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvDefect_u As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvPath_o As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvPath_u As tClassLibrary.tCnt.tCnt_DataGridView
End Class
