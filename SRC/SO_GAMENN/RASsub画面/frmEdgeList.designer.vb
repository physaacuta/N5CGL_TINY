<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEdgeList
	Inherits tClassForm.frmPop_Btn3

	'Form overrides dispose to clean up the component list.
	<System.Diagnostics.DebuggerNonUserCode()> _
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
	<System.Diagnostics.DebuggerStepThrough()> _
	Private Sub InitializeComponent()
        Me.grpSearchDate = New System.Windows.Forms.GroupBox
        Me.Label8 = New System.Windows.Forms.Label
        Me.Label5 = New System.Windows.Forms.Label
        Me.dtpTimeEnd = New System.Windows.Forms.DateTimePicker
        Me.dtpDateStart = New System.Windows.Forms.DateTimePicker
        Me.dtpDateEnd = New System.Windows.Forms.DateTimePicker
        Me.dtpTimeStart = New System.Windows.Forms.DateTimePicker
        Me.GroupBox5 = New System.Windows.Forms.GroupBox
        Me.chkALL = New System.Windows.Forms.CheckBox
        Me.Label11 = New System.Windows.Forms.Label
        Me.grpSearchDate.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(24, 176)
        Me.grpFunction.TabIndex = 0
        '
        'btnF01
        '
        Me.btnF01.TabIndex = 0
        Me.btnF01.Tag = 0
        Me.btnF01.Text = "表示"
        '
        'btnF02
        '
        Me.btnF02.TabIndex = 0
        Me.btnF02.Tag = 11
        Me.btnF02.Text = "保存"
        '
        'btnF03
        '
        Me.btnF03.Tag = 11
        Me.btnF03.Text = "閉じる"
        '
        'grpSearchDate
        '
        Me.grpSearchDate.Controls.Add(Me.Label8)
        Me.grpSearchDate.Controls.Add(Me.Label5)
        Me.grpSearchDate.Controls.Add(Me.dtpTimeEnd)
        Me.grpSearchDate.Controls.Add(Me.dtpDateStart)
        Me.grpSearchDate.Controls.Add(Me.dtpDateEnd)
        Me.grpSearchDate.Controls.Add(Me.dtpTimeStart)
        Me.grpSearchDate.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold)
        Me.grpSearchDate.Location = New System.Drawing.Point(12, 12)
        Me.grpSearchDate.Name = "grpSearchDate"
        Me.grpSearchDate.Size = New System.Drawing.Size(340, 100)
        Me.grpSearchDate.TabIndex = 21
        Me.grpSearchDate.TabStop = False
        Me.grpSearchDate.Text = "出力日時選択"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(8, 60)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(59, 16)
        Me.Label8.TabIndex = 4
        Me.Label8.Text = "終了日"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(8, 32)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(59, 16)
        Me.Label5.TabIndex = 3
        Me.Label5.Text = "開始日"
        '
        'dtpTimeEnd
        '
        Me.dtpTimeEnd.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpTimeEnd.Location = New System.Drawing.Point(204, 56)
        Me.dtpTimeEnd.Name = "dtpTimeEnd"
        Me.dtpTimeEnd.ShowUpDown = True
        Me.dtpTimeEnd.Size = New System.Drawing.Size(124, 23)
        Me.dtpTimeEnd.TabIndex = 3
        Me.dtpTimeEnd.Value = New Date(2007, 7, 1, 23, 59, 59, 0)
        '
        'dtpDateStart
        '
        Me.dtpDateStart.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateStart.Location = New System.Drawing.Point(76, 28)
        Me.dtpDateStart.Name = "dtpDateStart"
        Me.dtpDateStart.Size = New System.Drawing.Size(124, 23)
        Me.dtpDateStart.TabIndex = 0
        '
        'dtpDateEnd
        '
        Me.dtpDateEnd.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateEnd.Location = New System.Drawing.Point(76, 56)
        Me.dtpDateEnd.Name = "dtpDateEnd"
        Me.dtpDateEnd.Size = New System.Drawing.Size(124, 23)
        Me.dtpDateEnd.TabIndex = 2
        '
        'dtpTimeStart
        '
        Me.dtpTimeStart.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpTimeStart.Location = New System.Drawing.Point(204, 28)
        Me.dtpTimeStart.Name = "dtpTimeStart"
        Me.dtpTimeStart.ShowUpDown = True
        Me.dtpTimeStart.Size = New System.Drawing.Size(124, 23)
        Me.dtpTimeStart.TabIndex = 1
        Me.dtpTimeStart.Value = New Date(2007, 7, 19, 0, 0, 0, 0)
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.chkALL)
        Me.GroupBox5.Controls.Add(Me.Label11)
        Me.GroupBox5.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold)
        Me.GroupBox5.Location = New System.Drawing.Point(12, 116)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(340, 56)
        Me.GroupBox5.TabIndex = 23
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "出力条件指定"
        '
        'chkALL
        '
        Me.chkALL.AutoSize = True
        Me.chkALL.Location = New System.Drawing.Point(8, 28)
        Me.chkALL.Name = "chkALL"
        Me.chkALL.Size = New System.Drawing.Size(95, 20)
        Me.chkALL.TabIndex = 4
        Me.chkALL.Text = "全コイル"
        Me.chkALL.UseVisualStyleBackColor = True
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(236, 72)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(0, 16)
        Me.Label11.TabIndex = 0
        '
        'frmEdgeList
        '
        Me.ClientSize = New System.Drawing.Size(365, 271)
        Me.Controls.Add(Me.GroupBox5)
        Me.Controls.Add(Me.grpSearchDate)
        Me.Name = "frmEdgeList"
        Me.Text = "ｴｯｼﾞ検出履歴画面"
        Me.TopMost = False
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.grpSearchDate, 0)
        Me.Controls.SetChildIndex(Me.GroupBox5, 0)
        Me.grpSearchDate.ResumeLayout(False)
        Me.grpSearchDate.PerformLayout()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents grpSearchDate As System.Windows.Forms.GroupBox
    Protected WithEvents dtpTimeEnd As System.Windows.Forms.DateTimePicker
    Protected WithEvents dtpTimeStart As System.Windows.Forms.DateTimePicker
    Protected WithEvents Label8 As System.Windows.Forms.Label
    Protected WithEvents Label5 As System.Windows.Forms.Label
    Protected WithEvents dtpDateEnd As System.Windows.Forms.DateTimePicker
    Protected WithEvents dtpDateStart As System.Windows.Forms.DateTimePicker
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents chkALL As System.Windows.Forms.CheckBox

End Class
