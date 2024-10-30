<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmBaseList
    Inherits tClassForm.frmBase_Main

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
        Me.dtpStartDay = New System.Windows.Forms.DateTimePicker()
        Me.dtpStartTime = New System.Windows.Forms.DateTimePicker()
        Me.grpFilter_Time = New System.Windows.Forms.GroupBox()
        Me.chkStartDay = New System.Windows.Forms.CheckBox()
        Me.dgvBaseList = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpFilter_Time.SuspendLayout()
        CType(Me.dgvBaseList, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "RAS" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ï\é¶âÊñ "
        '
        'btnF05
        '
        Me.btnF05.Enabled = True
        Me.btnF05.Text = "ÉçÉOï\é¶"
        '
        'dtpStartDay
        '
        Me.dtpStartDay.CustomFormat = "yyyy/MM/dd"
        Me.dtpStartDay.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpStartDay.Format = System.Windows.Forms.DateTimePickerFormat.Custom
        Me.dtpStartDay.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpStartDay.Location = New System.Drawing.Point(168, 24)
        Me.dtpStartDay.Name = "dtpStartDay"
        Me.dtpStartDay.Size = New System.Drawing.Size(140, 26)
        Me.dtpStartDay.TabIndex = 3
        '
        'dtpStartTime
        '
        Me.dtpStartTime.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpStartTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpStartTime.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpStartTime.Location = New System.Drawing.Point(312, 24)
        Me.dtpStartTime.Name = "dtpStartTime"
        Me.dtpStartTime.ShowUpDown = True
        Me.dtpStartTime.Size = New System.Drawing.Size(116, 26)
        Me.dtpStartTime.TabIndex = 2
        '
        'grpFilter_Time
        '
        Me.grpFilter_Time.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.grpFilter_Time.Controls.Add(Me.chkStartDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpStartDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpStartTime)
        Me.grpFilter_Time.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_Time.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_Time.Location = New System.Drawing.Point(12, 840)
        Me.grpFilter_Time.Name = "grpFilter_Time"
        Me.grpFilter_Time.Size = New System.Drawing.Size(440, 60)
        Me.grpFilter_Time.TabIndex = 12
        Me.grpFilter_Time.TabStop = False
        '
        'chkStartDay
        '
        Me.chkStartDay.AutoSize = True
        Me.chkStartDay.CheckAlign = System.Drawing.ContentAlignment.MiddleRight
        Me.chkStartDay.Checked = True
        Me.chkStartDay.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkStartDay.Location = New System.Drawing.Point(12, 28)
        Me.chkStartDay.Name = "chkStartDay"
        Me.chkStartDay.Size = New System.Drawing.Size(146, 20)
        Me.chkStartDay.TabIndex = 15
        Me.chkStartDay.Text = "ÉIÉìÉâÉCÉìì˙éû"
        Me.chkStartDay.UseVisualStyleBackColor = True
        '
        'dgvBaseList
        '
        Me.dgvBaseList.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvBaseList.Location = New System.Drawing.Point(16, 24)
        Me.dgvBaseList.Name = "dgvBaseList"
        Me.dgvBaseList.RowTemplate.Height = 21
        Me.dgvBaseList.Size = New System.Drawing.Size(1236, 808)
        Me.dgvBaseList.TabIndex = 13
        '
        'frmBaseList
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.dgvBaseList)
        Me.Controls.Add(Me.grpFilter_Time)
        Me.Name = "frmBaseList"
        Me.Text = "äÓñ{ìÆçÏàÍóóâÊñ "
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.grpFilter_Time, 0)
        Me.Controls.SetChildIndex(Me.dgvBaseList, 0)
        Me.grpFilter_Time.ResumeLayout(False)
        Me.grpFilter_Time.PerformLayout()
        CType(Me.dgvBaseList, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Protected Friend WithEvents chkPC_TOKATU As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_SOUSA As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_TIKUSEKI_1 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HYOZI_2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HYOZI_1 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_01 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_04 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_03 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_02 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_05 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_08 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_07 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_06 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_TIKUSEKI_2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_09 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_09 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_12 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_12 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_05 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_08 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_11 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_11 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_04 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_10 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_10 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_07 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_06 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_03 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_02 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_CY_01 As System.Windows.Forms.CheckBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents grpFilter_Kind As System.Windows.Forms.GroupBox
    Protected Friend WithEvents chkKind_HYOZI_U As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_SOUSA As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_UWS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UWS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_OWS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_OWS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_UWQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UWQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_UDQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UDQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_OWQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_OWQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_UDS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_ODQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UDS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_CY_ODS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_ODQ As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_ODS As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HYOZI_O As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_TIKUSEKI As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_TOKATU As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_BACKUP_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_RPISAVE_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HYOZI01_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_TOKATU_N2 As System.Windows.Forms.CheckBox
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Protected Friend WithEvents chkPC_HT_09_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_05_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_08_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_04_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_10_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_07_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_06_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_03_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_02_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HT_01_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_RPIEXEC02_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_KASOU_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_SOUSA_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_RPIEXEC01_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_HYOZI02_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkPC_TIKUSEKI_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_RPIEXEC02_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_RPISAVE_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_RPIEXEC01_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_SOUSA_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_KASOU_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HYOZI01_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UWS_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_OWS_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UWQ_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UDQ_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_OWQ_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_UDS_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_ODQ_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HT_ODS_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_HYOZI02_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_TIKUSEKI_N2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKind_TOKATU_N2 As System.Windows.Forms.CheckBox
    Friend WithEvents TabPage3 As System.Windows.Forms.TabPage
    Protected Friend WithEvents CheckBox1 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox2 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox3 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox4 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox5 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox6 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox7 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox8 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox9 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox10 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox11 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox12 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox13 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox14 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox15 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox16 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox17 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox18 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox19 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox20 As System.Windows.Forms.CheckBox
    Protected Friend WithEvents CheckBox21 As System.Windows.Forms.CheckBox
    Friend WithEvents TabPage4 As System.Windows.Forms.TabPage
    Friend WithEvents tabFilter_Kind As tClassLibrary.tCnt.tCnt_TabControl
    Friend WithEvents KindPage1 As System.Windows.Forms.TabPage
    Friend WithEvents KindPage2 As System.Windows.Forms.TabPage
    Friend WithEvents TabPage5 As System.Windows.Forms.TabPage
    Friend WithEvents TabPage6 As System.Windows.Forms.TabPage
    Friend WithEvents tabFilter_PC As tClassLibrary.tCnt.tCnt_TabControl
    Friend WithEvents PCPage1 As System.Windows.Forms.TabPage
    Friend WithEvents PCPage2 As System.Windows.Forms.TabPage
    Protected Friend WithEvents dtpStartDay As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpStartTime As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents grpFilter_Time As System.Windows.Forms.GroupBox
    Friend WithEvents dgvBaseList As tClassLibrary.tCnt.tCnt_DataGridView
    Protected Friend WithEvents chkStartDay As System.Windows.Forms.CheckBox

End Class
