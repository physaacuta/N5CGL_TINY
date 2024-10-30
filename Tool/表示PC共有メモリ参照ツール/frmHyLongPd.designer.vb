<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyLongPd
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
		Me.txtGuidanceFlag = New System.Windows.Forms.TextBox()
		Me.btnRead = New System.Windows.Forms.Button()
		Me.btnWrite = New System.Windows.Forms.Button()
		Me.dgvRec = New tClassLibrary.tCnt.tCnt_DataGridView()
		Me.GroupBox2 = New System.Windows.Forms.GroupBox()
		Me.GroupBox3 = New System.Windows.Forms.GroupBox()
		Me.chkUpdate = New System.Windows.Forms.CheckBox()
		Me.txtTitle = New System.Windows.Forms.Label()
		CType(Me.dgvRec, System.ComponentModel.ISupportInitialize).BeginInit()
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
		Me.Label1.Text = "ガイダンス表示フラグ"
		Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'txtGuidanceFlag
		'
		Me.txtGuidanceFlag.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.txtGuidanceFlag.Location = New System.Drawing.Point(188, 20)
		Me.txtGuidanceFlag.Name = "txtGuidanceFlag"
		Me.txtGuidanceFlag.Size = New System.Drawing.Size(60, 23)
		Me.txtGuidanceFlag.TabIndex = 2
		Me.txtGuidanceFlag.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
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
		'dgvRec
		'
		Me.dgvRec.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvRec.Location = New System.Drawing.Point(4, 20)
		Me.dgvRec.Name = "dgvRec"
		Me.dgvRec.RowTemplate.Height = 21
		Me.dgvRec.Size = New System.Drawing.Size(364, 208)
		Me.dgvRec.TabIndex = 41
		'
		'GroupBox2
		'
		Me.GroupBox2.Controls.Add(Me.Label1)
		Me.GroupBox2.Controls.Add(Me.txtGuidanceFlag)
		Me.GroupBox2.Location = New System.Drawing.Point(4, 44)
		Me.GroupBox2.Name = "GroupBox2"
		Me.GroupBox2.Size = New System.Drawing.Size(520, 56)
		Me.GroupBox2.TabIndex = 43
		Me.GroupBox2.TabStop = False
		Me.GroupBox2.Text = "全体ステータス"
		'
		'GroupBox3
		'
		Me.GroupBox3.Controls.Add(Me.dgvRec)
		Me.GroupBox3.Location = New System.Drawing.Point(4, 104)
		Me.GroupBox3.Name = "GroupBox3"
		Me.GroupBox3.Size = New System.Drawing.Size(380, 236)
		Me.GroupBox3.TabIndex = 44
		Me.GroupBox3.TabStop = False
		Me.GroupBox3.Text = "コイル単位"
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
		Me.txtTitle.Size = New System.Drawing.Size(217, 21)
		Me.txtTitle.TabIndex = 46
		Me.txtTitle.Text = "長周期情報テーブル"
		'
		'frmHyLongPd
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(7.0!, 13.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.ClientSize = New System.Drawing.Size(684, 347)
		Me.Controls.Add(Me.txtTitle)
		Me.Controls.Add(Me.chkUpdate)
		Me.Controls.Add(Me.GroupBox3)
		Me.Controls.Add(Me.GroupBox2)
		Me.Controls.Add(Me.btnRead)
		Me.Controls.Add(Me.btnWrite)
		Me.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
		Me.MaximizeBox = False
		Me.Name = "frmHyLongPd"
		Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
		Me.Text = "長周期情報 [TBL_HY_LONGPD]"
		CType(Me.dgvRec, System.ComponentModel.ISupportInitialize).EndInit()
		Me.GroupBox2.ResumeLayout(False)
		Me.GroupBox2.PerformLayout()
		Me.GroupBox3.ResumeLayout(False)
		Me.ResumeLayout(False)
		Me.PerformLayout()

	End Sub
	Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents txtGuidanceFlag As System.Windows.Forms.TextBox
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
	Friend WithEvents dgvRec As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents Label9 As System.Windows.Forms.Label
	Friend WithEvents Label10 As System.Windows.Forms.Label
	Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
	Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
	Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
	Friend WithEvents txtKiki1Text As System.Windows.Forms.TextBox
	Friend WithEvents txtKensa1Text As System.Windows.Forms.TextBox
	Friend WithEvents txtKiki0Text As System.Windows.Forms.TextBox
	Friend WithEvents txtKensa0Text As System.Windows.Forms.TextBox
	Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
	Friend WithEvents txtTitle As System.Windows.Forms.Label
End Class
