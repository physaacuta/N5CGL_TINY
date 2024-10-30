<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_TimeSet
	Inherits tClassForm.frmPop_Btn2

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
        Me.components = New System.ComponentModel.Container
        Me.lblClock = New System.Windows.Forms.Label
        Me.dtpTime = New System.Windows.Forms.DateTimePicker
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.mclMonth = New System.Windows.Forms.MonthCalendar
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.timClock = New System.Windows.Forms.Timer(Me.components)
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(264, 204)
        '
        'btnF01
        '
        Me.btnF01.Tag = 0
        '
        'btnF02
        '
        Me.btnF02.Tag = 11
        '
        'lblClock
        '
        Me.lblClock.AutoSize = True
        Me.lblClock.Font = New System.Drawing.Font("ÇlÇr ñæí©", 18.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblClock.Location = New System.Drawing.Point(8, 8)
        Me.lblClock.Name = "lblClock"
        Me.lblClock.Size = New System.Drawing.Size(257, 24)
        Me.lblClock.TabIndex = 8
        Me.lblClock.Text = "2006/12/29 12:13:50"
        '
        'dtpTime
        '
        Me.dtpTime.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpTime.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpTime.Location = New System.Drawing.Point(16, 24)
        Me.dtpTime.Name = "dtpTime"
        Me.dtpTime.ShowUpDown = True
        Me.dtpTime.Size = New System.Drawing.Size(116, 26)
        Me.dtpTime.TabIndex = 11
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.mclMonth)
        Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox1.Location = New System.Drawing.Point(8, 40)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(244, 248)
        Me.GroupBox1.TabIndex = 12
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ì˙ït"
        '
        'mclMonth
        '
        Me.mclMonth.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.mclMonth.Location = New System.Drawing.Point(16, 24)
        Me.mclMonth.MaxSelectionCount = 1
        Me.mclMonth.Name = "mclMonth"
        Me.mclMonth.ShowToday = False
        Me.mclMonth.TabIndex = 11
        Me.mclMonth.TitleBackColor = System.Drawing.Color.Silver
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dtpTime)
        Me.GroupBox2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox2.Location = New System.Drawing.Point(264, 40)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(144, 60)
        Me.GroupBox2.TabIndex = 13
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "éûçè"
        '
        'timClock
        '
        Me.timClock.Enabled = True
        Me.timClock.Interval = 500
        '
        'frmPop_TimeSet
        '
        Me.ClientSize = New System.Drawing.Size(493, 294)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.lblClock)
        Me.Name = "frmPop_TimeSet"
        Me.Text = "·råüópéûçèê›íËâÊñ ÇÃêeÉtÉHÅ[ÉÄ"
        Me.Controls.SetChildIndex(Me.lblClock, 0)
        Me.Controls.SetChildIndex(Me.GroupBox1, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.GroupBox2, 0)
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
	Protected Friend WithEvents lblClock As System.Windows.Forms.Label
    Protected Friend WithEvents dtpTime As System.Windows.Forms.DateTimePicker
 Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
 Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
 Protected Friend WithEvents timClock As System.Windows.Forms.Timer
 Protected Friend WithEvents mclMonth As System.Windows.Forms.MonthCalendar

End Class
