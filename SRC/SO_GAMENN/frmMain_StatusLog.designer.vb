<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmMain_StatusLog
    Inherits tClassForm.frmBase_Main

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
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
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.lblClock = New System.Windows.Forms.Label()
        Me.prbReadRow = New System.Windows.Forms.ProgressBar()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.lblRowCount = New System.Windows.Forms.Label()
        Me.grpFilter = New System.Windows.Forms.GroupBox()
        Me.Panel13 = New System.Windows.Forms.Panel()
        Me.grpFilter_Time = New System.Windows.Forms.GroupBox()
        Me.chkEndDay = New System.Windows.Forms.CheckBox()
        Me.chkStartDay = New System.Windows.Forms.CheckBox()
        Me.dtpEndDay = New System.Windows.Forms.DateTimePicker()
        Me.dtpEndTime = New System.Windows.Forms.DateTimePicker()
        Me.dtpStartDay = New System.Windows.Forms.DateTimePicker()
        Me.dtpStartTime = New System.Windows.Forms.DateTimePicker()
        Me.grpFilter_KUBUN = New System.Windows.Forms.GroupBox()
        Me.dgvLog = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.timClock = New System.Windows.Forms.Timer(Me.components)
        Me.Label2 = New System.Windows.Forms.Label()
        Me.nudRowMaxCount = New System.Windows.Forms.NumericUpDown()
        Me.TableLayoutPanel2 = New System.Windows.Forms.TableLayoutPanel()
        Me.TableLayoutPanel3 = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel14 = New System.Windows.Forms.Panel()
        Me.grpFilter.SuspendLayout()
        Me.Panel13.SuspendLayout()
        Me.grpFilter_Time.SuspendLayout()
        CType(Me.dgvLog, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudRowMaxCount, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TableLayoutPanel2.SuspendLayout()
        Me.TableLayoutPanel3.SuspendLayout()
        Me.Panel14.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "ÉÅÉjÉÖÅ["
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "âÊñ ï€ë∂"
        '
        'btnF10
        '
        Me.btnF10.Enabled = True
        Me.btnF10.Text = "âÊñ àÛç¸"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "CSVèoóÕ"
        '
        'btnF05
        '
        Me.btnF05.Enabled = True
        Me.btnF05.Text = "ÉçÉOï\é¶"
        '
        'btnF04
        '
        Me.btnF04.Text = "∑¨›æŸ"
        '
        'lblClock
        '
        Me.lblClock.AutoSize = True
        Me.lblClock.Font = New System.Drawing.Font("ÇlÇr ñæí©", 16.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblClock.Location = New System.Drawing.Point(3, 0)
        Me.lblClock.Name = "lblClock"
        Me.lblClock.Size = New System.Drawing.Size(238, 22)
        Me.lblClock.TabIndex = 7
        Me.lblClock.Text = "2006/12/29 12:13:50"
        '
        'prbReadRow
        '
        Me.prbReadRow.Location = New System.Drawing.Point(273, 3)
        Me.prbReadRow.Maximum = 10000
        Me.prbReadRow.Name = "prbReadRow"
        Me.prbReadRow.Size = New System.Drawing.Size(268, 24)
        Me.prbReadRow.Step = 1
        Me.prbReadRow.Style = System.Windows.Forms.ProgressBarStyle.Continuous
        Me.prbReadRow.TabIndex = 8
        Me.prbReadRow.Visible = False
        '
        'Label9
        '
        Me.Label9.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.Location = New System.Drawing.Point(12, 5)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(156, 19)
        Me.Label9.TabIndex = 5
        Me.Label9.Text = "ÉçÉOï\é¶åèêîÅF"
        '
        'lblRowCount
        '
        Me.lblRowCount.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblRowCount.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblRowCount.Location = New System.Drawing.Point(172, 5)
        Me.lblRowCount.Name = "lblRowCount"
        Me.lblRowCount.Size = New System.Drawing.Size(76, 19)
        Me.lblRowCount.TabIndex = 6
        Me.lblRowCount.Text = "0"
        Me.lblRowCount.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'grpFilter
        '
        Me.grpFilter.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.grpFilter.Controls.Add(Me.Panel13)
        Me.grpFilter.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter.ForeColor = System.Drawing.Color.Navy
        Me.grpFilter.Location = New System.Drawing.Point(3, 684)
        Me.grpFilter.Name = "grpFilter"
        Me.grpFilter.Size = New System.Drawing.Size(1252, 219)
        Me.grpFilter.TabIndex = 9
        Me.grpFilter.TabStop = False
        Me.grpFilter.Text = "ÉtÉBÉãÉ^Å[çÄñ⁄"
        '
        'Panel13
        '
        Me.Panel13.AutoScroll = True
        Me.Panel13.Controls.Add(Me.grpFilter_KUBUN)
        Me.Panel13.Controls.Add(Me.grpFilter_Time)
        Me.Panel13.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel13.Location = New System.Drawing.Point(3, 22)
        Me.Panel13.Name = "Panel13"
        Me.Panel13.Size = New System.Drawing.Size(1246, 194)
        Me.Panel13.TabIndex = 4
        '
        'grpFilter_Time
        '
        Me.grpFilter_Time.Controls.Add(Me.chkEndDay)
        Me.grpFilter_Time.Controls.Add(Me.chkStartDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpEndDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpEndTime)
        Me.grpFilter_Time.Controls.Add(Me.dtpStartDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpStartTime)
        Me.grpFilter_Time.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_Time.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_Time.Location = New System.Drawing.Point(10, 0)
        Me.grpFilter_Time.Name = "grpFilter_Time"
        Me.grpFilter_Time.Size = New System.Drawing.Size(376, 88)
        Me.grpFilter_Time.TabIndex = 2
        Me.grpFilter_Time.TabStop = False
        Me.grpFilter_Time.Text = "éûä‘ÉtÉBÉãÉ^Å["
        '
        'chkEndDay
        '
        Me.chkEndDay.AutoSize = True
        Me.chkEndDay.CheckAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.chkEndDay.Checked = True
        Me.chkEndDay.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkEndDay.Location = New System.Drawing.Point(8, 60)
        Me.chkEndDay.Name = "chkEndDay"
        Me.chkEndDay.Size = New System.Drawing.Size(95, 20)
        Me.chkEndDay.TabIndex = 7
        Me.chkEndDay.Text = "èIóπéûä‘"
        Me.chkEndDay.UseVisualStyleBackColor = True
        '
        'chkStartDay
        '
        Me.chkStartDay.AutoSize = True
        Me.chkStartDay.CheckAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.chkStartDay.Checked = True
        Me.chkStartDay.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkStartDay.Location = New System.Drawing.Point(8, 28)
        Me.chkStartDay.Name = "chkStartDay"
        Me.chkStartDay.Size = New System.Drawing.Size(95, 20)
        Me.chkStartDay.TabIndex = 7
        Me.chkStartDay.Text = "äJénéûä‘"
        Me.chkStartDay.UseVisualStyleBackColor = True
        '
        'dtpEndDay
        '
        Me.dtpEndDay.CustomFormat = "yyyy/MM/dd"
        Me.dtpEndDay.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpEndDay.Format = System.Windows.Forms.DateTimePickerFormat.Custom
        Me.dtpEndDay.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpEndDay.Location = New System.Drawing.Point(108, 56)
        Me.dtpEndDay.Name = "dtpEndDay"
        Me.dtpEndDay.Size = New System.Drawing.Size(140, 26)
        Me.dtpEndDay.TabIndex = 5
        '
        'dtpEndTime
        '
        Me.dtpEndTime.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpEndTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpEndTime.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpEndTime.Location = New System.Drawing.Point(252, 56)
        Me.dtpEndTime.Name = "dtpEndTime"
        Me.dtpEndTime.ShowUpDown = True
        Me.dtpEndTime.Size = New System.Drawing.Size(116, 26)
        Me.dtpEndTime.TabIndex = 4
        '
        'dtpStartDay
        '
        Me.dtpStartDay.CustomFormat = "yyyy/MM/dd"
        Me.dtpStartDay.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpStartDay.Format = System.Windows.Forms.DateTimePickerFormat.Custom
        Me.dtpStartDay.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpStartDay.Location = New System.Drawing.Point(108, 24)
        Me.dtpStartDay.Name = "dtpStartDay"
        Me.dtpStartDay.Size = New System.Drawing.Size(140, 26)
        Me.dtpStartDay.TabIndex = 3
        '
        'dtpStartTime
        '
        Me.dtpStartTime.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpStartTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpStartTime.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpStartTime.Location = New System.Drawing.Point(252, 24)
        Me.dtpStartTime.Name = "dtpStartTime"
        Me.dtpStartTime.ShowUpDown = True
        Me.dtpStartTime.Size = New System.Drawing.Size(116, 26)
        Me.dtpStartTime.TabIndex = 2
        '
        'grpFilter_KUBUN
        '
        Me.grpFilter_KUBUN.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_KUBUN.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_KUBUN.Location = New System.Drawing.Point(392, 0)
        Me.grpFilter_KUBUN.Name = "grpFilter_KUBUN"
        Me.grpFilter_KUBUN.Size = New System.Drawing.Size(825, 175)
        Me.grpFilter_KUBUN.TabIndex = 3
        Me.grpFilter_KUBUN.TabStop = False
        Me.grpFilter_KUBUN.Text = "ãÊï™ÉtÉBÉãÉ^Å["
        '
        'dgvLog
        '
        Me.dgvLog.AllowUserToAddRows = False
        Me.dgvLog.AllowUserToDeleteRows = False
        Me.dgvLog.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvLog.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvLog.Location = New System.Drawing.Point(0, 36)
        Me.dgvLog.Margin = New System.Windows.Forms.Padding(0)
        Me.dgvLog.Name = "dgvLog"
        Me.dgvLog.ReadOnly = True
        Me.dgvLog.RowTemplate.Height = 21
        Me.dgvLog.Size = New System.Drawing.Size(1258, 645)
        Me.dgvLog.TabIndex = 10
        '
        'timClock
        '
        Me.timClock.Enabled = True
        Me.timClock.Interval = 500
        '
        'Label2
        '
        Me.Label2.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(252, 5)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(20, 19)
        Me.Label2.TabIndex = 12
        Me.Label2.Text = "/"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'nudRowMaxCount
        '
        Me.nudRowMaxCount.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.nudRowMaxCount.Increment = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudRowMaxCount.Location = New System.Drawing.Point(276, 1)
        Me.nudRowMaxCount.Maximum = New Decimal(New Integer() {999999, 0, 0, 0})
        Me.nudRowMaxCount.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudRowMaxCount.Name = "nudRowMaxCount"
        Me.nudRowMaxCount.Size = New System.Drawing.Size(92, 26)
        Me.nudRowMaxCount.TabIndex = 13
        Me.nudRowMaxCount.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudRowMaxCount.Value = New Decimal(New Integer() {1000, 0, 0, 0})
        '
        'TableLayoutPanel2
        '
        Me.TableLayoutPanel2.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel2.ColumnCount = 1
        Me.TableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel2.Controls.Add(Me.grpFilter, 0, 2)
        Me.TableLayoutPanel2.Controls.Add(Me.dgvLog, 0, 1)
        Me.TableLayoutPanel2.Controls.Add(Me.TableLayoutPanel3, 0, 0)
        Me.TableLayoutPanel2.Location = New System.Drawing.Point(12, 1)
        Me.TableLayoutPanel2.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel2.Name = "TableLayoutPanel2"
        Me.TableLayoutPanel2.RowCount = 3
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 36.0!))
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 225.0!))
        Me.TableLayoutPanel2.Size = New System.Drawing.Size(1258, 906)
        Me.TableLayoutPanel2.TabIndex = 14
        '
        'TableLayoutPanel3
        '
        Me.TableLayoutPanel3.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel3.ColumnCount = 3
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 270.0!))
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 370.0!))
        Me.TableLayoutPanel3.Controls.Add(Me.lblClock, 0, 0)
        Me.TableLayoutPanel3.Controls.Add(Me.prbReadRow, 1, 0)
        Me.TableLayoutPanel3.Controls.Add(Me.Panel14, 2, 0)
        Me.TableLayoutPanel3.Location = New System.Drawing.Point(0, 3)
        Me.TableLayoutPanel3.Margin = New System.Windows.Forms.Padding(0, 3, 0, 3)
        Me.TableLayoutPanel3.Name = "TableLayoutPanel3"
        Me.TableLayoutPanel3.RowCount = 1
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Size = New System.Drawing.Size(1258, 30)
        Me.TableLayoutPanel3.TabIndex = 11
        '
        'Panel14
        '
        Me.Panel14.Controls.Add(Me.Label9)
        Me.Panel14.Controls.Add(Me.lblRowCount)
        Me.Panel14.Controls.Add(Me.nudRowMaxCount)
        Me.Panel14.Controls.Add(Me.Label2)
        Me.Panel14.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel14.Location = New System.Drawing.Point(888, 0)
        Me.Panel14.Margin = New System.Windows.Forms.Padding(0)
        Me.Panel14.Name = "Panel14"
        Me.Panel14.Size = New System.Drawing.Size(370, 30)
        Me.Panel14.TabIndex = 9
        '
        'frmMain_StatusLog
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.TableLayoutPanel2)
        Me.Name = "frmMain_StatusLog"
        Me.Text = "·råüópÉçÉOâÊñ ÇÃêeÉtÉHÅ[ÉÄ"
        Me.Controls.SetChildIndex(Me.TableLayoutPanel2, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.grpFilter.ResumeLayout(False)
        Me.Panel13.ResumeLayout(False)
        Me.grpFilter_Time.ResumeLayout(False)
        Me.grpFilter_Time.PerformLayout()
        CType(Me.dgvLog, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudRowMaxCount, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TableLayoutPanel2.ResumeLayout(False)
        Me.TableLayoutPanel3.ResumeLayout(False)
        Me.TableLayoutPanel3.PerformLayout()
        Me.Panel14.ResumeLayout(False)
        Me.Panel14.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Protected Friend WithEvents lblClock As System.Windows.Forms.Label
    Protected Friend WithEvents prbReadRow As System.Windows.Forms.ProgressBar
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Protected Friend WithEvents lblRowCount As System.Windows.Forms.Label
    Protected Friend WithEvents grpFilter As System.Windows.Forms.GroupBox
    Protected Friend WithEvents grpFilter_KUBUN As System.Windows.Forms.GroupBox
    Protected Friend WithEvents grpFilter_Time As System.Windows.Forms.GroupBox
    Protected Friend WithEvents dtpEndDay As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpEndTime As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpStartDay As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpStartTime As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents timClock As System.Windows.Forms.Timer
    Protected Friend WithEvents chkEndDay As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkStartDay As System.Windows.Forms.CheckBox
    Protected Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents nudRowMaxCount As System.Windows.Forms.NumericUpDown
    Protected Friend WithEvents dgvLog As tClassLibrary.tCnt.tCnt_DataGridView
    Protected Friend WithEvents TableLayoutPanel2 As TableLayoutPanel
    Friend WithEvents Panel13 As Panel
    Friend WithEvents TableLayoutPanel3 As TableLayoutPanel
    Friend WithEvents Panel14 As Panel
End Class
