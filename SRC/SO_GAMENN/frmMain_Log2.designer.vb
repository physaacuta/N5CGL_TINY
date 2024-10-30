<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmMain_Log2
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
        Me.Panel14 = New System.Windows.Forms.Panel()
        Me.grpFilter_PCKIND = New System.Windows.Forms.GroupBox()
        Me.grpFilter_PC = New System.Windows.Forms.GroupBox()
        Me.grpFilter_Time = New System.Windows.Forms.GroupBox()
        Me.chkEndDay = New System.Windows.Forms.CheckBox()
        Me.chkStartDay = New System.Windows.Forms.CheckBox()
        Me.dtpEndDay = New System.Windows.Forms.DateTimePicker()
        Me.dtpEndTime = New System.Windows.Forms.DateTimePicker()
        Me.dtpStartDay = New System.Windows.Forms.DateTimePicker()
        Me.dtpStartTime = New System.Windows.Forms.DateTimePicker()
        Me.m_grpFilter_Code = New System.Windows.Forms.GroupBox()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.txtCode_To = New System.Windows.Forms.TextBox()
        Me.txtCode_Not = New System.Windows.Forms.TextBox()
        Me.txtCode_Select = New System.Windows.Forms.TextBox()
        Me.txtCode_From = New System.Windows.Forms.TextBox()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.grpFilter_Kubun = New System.Windows.Forms.GroupBox()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.chkKubun_H_Evt = New System.Windows.Forms.CheckBox()
        Me.chkKubun_H_Kei = New System.Windows.Forms.CheckBox()
        Me.chkKubun_S_Evt = New System.Windows.Forms.CheckBox()
        Me.chkKubun_S_Kei = New System.Windows.Forms.CheckBox()
        Me.chkKubun_H_Cmt = New System.Windows.Forms.CheckBox()
        Me.chkKubun_H_Jyu = New System.Windows.Forms.CheckBox()
        Me.chkKubun_A_Cmt = New System.Windows.Forms.CheckBox()
        Me.chkKubun_A_Evt = New System.Windows.Forms.CheckBox()
        Me.chkKubun_S_Cmt = New System.Windows.Forms.CheckBox()
        Me.chkKubun_S_Jyu = New System.Windows.Forms.CheckBox()
        Me.chkKubun_A_Kei = New System.Windows.Forms.CheckBox()
        Me.chkKubun_A_Jyu = New System.Windows.Forms.CheckBox()
        Me.dgvLog = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.timClock = New System.Windows.Forms.Timer(Me.components)
        Me.Label2 = New System.Windows.Forms.Label()
        Me.nudRowMaxCount = New System.Windows.Forms.NumericUpDown()
        Me.TableLayoutPanel2 = New System.Windows.Forms.TableLayoutPanel()
        Me.TableLayoutPanel3 = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel13 = New System.Windows.Forms.Panel()
        Me.grpFilter.SuspendLayout()
        Me.Panel14.SuspendLayout()
        Me.grpFilter_Time.SuspendLayout()
        Me.m_grpFilter_Code.SuspendLayout()
        Me.grpFilter_Kubun.SuspendLayout()
        CType(Me.dgvLog, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudRowMaxCount, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TableLayoutPanel2.SuspendLayout()
        Me.TableLayoutPanel3.SuspendLayout()
        Me.Panel13.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Text = "ãÊï™" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ëSëIë"
        '
        'btnF03
        '
        Me.btnF03.Enabled = True
        Me.btnF03.Text = "ãÊï™" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ëSâèú"
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
        'btnF08
        '
        Me.btnF08.Text = "ÉRÉÅÉìÉg" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ï“èW"
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
        Me.Label9.Location = New System.Drawing.Point(3, 5)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(156, 19)
        Me.Label9.TabIndex = 5
        Me.Label9.Text = "ÉçÉOï\é¶åèêîÅF"
        '
        'lblRowCount
        '
        Me.lblRowCount.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblRowCount.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblRowCount.Location = New System.Drawing.Point(171, 5)
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
        Me.grpFilter.Controls.Add(Me.Panel14)
        Me.grpFilter.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter.ForeColor = System.Drawing.Color.Navy
        Me.grpFilter.Location = New System.Drawing.Point(3, 648)
        Me.grpFilter.Name = "grpFilter"
        Me.grpFilter.Size = New System.Drawing.Size(1252, 250)
        Me.grpFilter.TabIndex = 9
        Me.grpFilter.TabStop = False
        Me.grpFilter.Text = "ÉtÉBÉãÉ^Å[çÄñ⁄"
        '
        'Panel14
        '
        Me.Panel14.AutoScroll = True
        Me.Panel14.Controls.Add(Me.grpFilter_PCKIND)
        Me.Panel14.Controls.Add(Me.grpFilter_PC)
        Me.Panel14.Controls.Add(Me.grpFilter_Time)
        Me.Panel14.Controls.Add(Me.m_grpFilter_Code)
        Me.Panel14.Controls.Add(Me.grpFilter_Kubun)
        Me.Panel14.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel14.Location = New System.Drawing.Point(3, 22)
        Me.Panel14.Name = "Panel14"
        Me.Panel14.Size = New System.Drawing.Size(1246, 225)
        Me.Panel14.TabIndex = 0
        '
        'grpFilter_PCKIND
        '
        Me.grpFilter_PCKIND.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_PCKIND.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_PCKIND.Location = New System.Drawing.Point(586, 2)
        Me.grpFilter_PCKIND.Name = "grpFilter_PCKIND"
        Me.grpFilter_PCKIND.Size = New System.Drawing.Size(649, 100)
        Me.grpFilter_PCKIND.TabIndex = 5
        Me.grpFilter_PCKIND.TabStop = False
        Me.grpFilter_PCKIND.Text = "PCã@î\ÉtÉBÉãÉ^Å["
        '
        'grpFilter_PC
        '
        Me.grpFilter_PC.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_PC.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_PC.Location = New System.Drawing.Point(586, 106)
        Me.grpFilter_PC.Name = "grpFilter_PC"
        Me.grpFilter_PC.Size = New System.Drawing.Size(649, 100)
        Me.grpFilter_PC.TabIndex = 3
        Me.grpFilter_PC.TabStop = False
        Me.grpFilter_PC.Text = "PCñºÉtÉBÉãÉ^Å["
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
        Me.grpFilter_Time.Location = New System.Drawing.Point(4, 2)
        Me.grpFilter_Time.Name = "grpFilter_Time"
        Me.grpFilter_Time.Size = New System.Drawing.Size(368, 92)
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
        Me.chkEndDay.Location = New System.Drawing.Point(5, 60)
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
        Me.chkStartDay.Location = New System.Drawing.Point(5, 28)
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
        Me.dtpEndDay.Location = New System.Drawing.Point(105, 56)
        Me.dtpEndDay.Name = "dtpEndDay"
        Me.dtpEndDay.Size = New System.Drawing.Size(140, 26)
        Me.dtpEndDay.TabIndex = 5
        '
        'dtpEndTime
        '
        Me.dtpEndTime.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpEndTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpEndTime.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpEndTime.Location = New System.Drawing.Point(249, 56)
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
        Me.dtpStartDay.Location = New System.Drawing.Point(105, 24)
        Me.dtpStartDay.Name = "dtpStartDay"
        Me.dtpStartDay.Size = New System.Drawing.Size(140, 26)
        Me.dtpStartDay.TabIndex = 3
        '
        'dtpStartTime
        '
        Me.dtpStartTime.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpStartTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpStartTime.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpStartTime.Location = New System.Drawing.Point(249, 24)
        Me.dtpStartTime.Name = "dtpStartTime"
        Me.dtpStartTime.ShowUpDown = True
        Me.dtpStartTime.Size = New System.Drawing.Size(116, 26)
        Me.dtpStartTime.TabIndex = 2
        '
        'm_grpFilter_Code
        '
        Me.m_grpFilter_Code.Controls.Add(Me.Label8)
        Me.m_grpFilter_Code.Controls.Add(Me.Label7)
        Me.m_grpFilter_Code.Controls.Add(Me.txtCode_To)
        Me.m_grpFilter_Code.Controls.Add(Me.txtCode_Not)
        Me.m_grpFilter_Code.Controls.Add(Me.txtCode_Select)
        Me.m_grpFilter_Code.Controls.Add(Me.txtCode_From)
        Me.m_grpFilter_Code.Controls.Add(Me.Label5)
        Me.m_grpFilter_Code.Controls.Add(Me.Label4)
        Me.m_grpFilter_Code.Controls.Add(Me.Label3)
        Me.m_grpFilter_Code.Controls.Add(Me.Label6)
        Me.m_grpFilter_Code.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.m_grpFilter_Code.ForeColor = System.Drawing.Color.Black
        Me.m_grpFilter_Code.Location = New System.Drawing.Point(375, 3)
        Me.m_grpFilter_Code.Name = "m_grpFilter_Code"
        Me.m_grpFilter_Code.Size = New System.Drawing.Size(208, 204)
        Me.m_grpFilter_Code.TabIndex = 3
        Me.m_grpFilter_Code.TabStop = False
        Me.m_grpFilter_Code.Text = "ÉRÅ[ÉhÉtÉBÉãÉ^Å["
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(148, 44)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(42, 16)
        Me.Label8.TabIndex = 9
        Me.Label8.Text = "Ç‹Ç≈"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(56, 44)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(42, 16)
        Me.Label7.TabIndex = 9
        Me.Label7.Text = "Ç©ÇÁ"
        '
        'txtCode_To
        '
        Me.txtCode_To.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtCode_To.Location = New System.Drawing.Point(104, 36)
        Me.txtCode_To.MaxLength = 3
        Me.txtCode_To.Name = "txtCode_To"
        Me.txtCode_To.Size = New System.Drawing.Size(40, 23)
        Me.txtCode_To.TabIndex = 8
        Me.txtCode_To.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtCode_Not
        '
        Me.txtCode_Not.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtCode_Not.Location = New System.Drawing.Point(12, 124)
        Me.txtCode_Not.MaxLength = 32
        Me.txtCode_Not.Name = "txtCode_Not"
        Me.txtCode_Not.Size = New System.Drawing.Size(188, 23)
        Me.txtCode_Not.TabIndex = 8
        '
        'txtCode_Select
        '
        Me.txtCode_Select.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtCode_Select.Location = New System.Drawing.Point(12, 80)
        Me.txtCode_Select.MaxLength = 32
        Me.txtCode_Select.Name = "txtCode_Select"
        Me.txtCode_Select.Size = New System.Drawing.Size(188, 23)
        Me.txtCode_Select.TabIndex = 8
        '
        'txtCode_From
        '
        Me.txtCode_From.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.txtCode_From.Location = New System.Drawing.Point(12, 36)
        Me.txtCode_From.MaxLength = 3
        Me.txtCode_From.Name = "txtCode_From"
        Me.txtCode_From.Size = New System.Drawing.Size(40, 23)
        Me.txtCode_From.TabIndex = 8
        Me.txtCode_From.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(8, 108)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(135, 15)
        Me.Label5.TabIndex = 7
        Me.Label5.Text = "ì¡íËÉRÅ[ÉhëŒè€äO"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(8, 64)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(119, 15)
        Me.Label4.TabIndex = 7
        Me.Label4.Text = "ì¡íËÉRÅ[ÉhéwíË"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(8, 20)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(119, 15)
        Me.Label3.TabIndex = 7
        Me.Label3.Text = "ÉRÅ[ÉhîÕàÕéwíË"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(8, 148)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(179, 36)
        Me.Label6.TabIndex = 7
        Me.Label6.Text = "Å¶  1,2,6ÇÃÇÊÇ§Ç…çiÇËçûÇ›ÇΩÇ¢" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "   ÉRÅ[ÉhÇÉJÉìÉ}Ç≈ãÊêÿÇ¡Çƒéw" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "   íËÇµÇƒâ∫Ç≥Ç¢ÅB"
        '
        'grpFilter_Kubun
        '
        Me.grpFilter_Kubun.Controls.Add(Me.Label11)
        Me.grpFilter_Kubun.Controls.Add(Me.Label10)
        Me.grpFilter_Kubun.Controls.Add(Me.Label1)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_H_Evt)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_H_Kei)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_S_Evt)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_S_Kei)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_H_Cmt)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_H_Jyu)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_A_Cmt)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_A_Evt)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_S_Cmt)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_S_Jyu)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_A_Kei)
        Me.grpFilter_Kubun.Controls.Add(Me.chkKubun_A_Jyu)
        Me.grpFilter_Kubun.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_Kubun.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_Kubun.Location = New System.Drawing.Point(4, 96)
        Me.grpFilter_Kubun.Name = "grpFilter_Kubun"
        Me.grpFilter_Kubun.Size = New System.Drawing.Size(368, 111)
        Me.grpFilter_Kubun.TabIndex = 4
        Me.grpFilter_Kubun.TabStop = False
        Me.grpFilter_Kubun.Text = "ãÊï™ÉtÉBÉãÉ^Å["
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label11.Location = New System.Drawing.Point(5, 76)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(71, 16)
        Me.Label11.TabIndex = 8
        Me.Label11.Text = " ∞ƒﬁ≥™±"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.Location = New System.Drawing.Point(5, 52)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(62, 16)
        Me.Label10.TabIndex = 8
        Me.Label10.Text = "øÃƒ≥™±"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(5, 28)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(42, 16)
        Me.Label1.TabIndex = 8
        Me.Label1.Text = "ëSî "
        '
        'chkKubun_H_Evt
        '
        Me.chkKubun_H_Evt.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_H_Evt.Location = New System.Drawing.Point(293, 72)
        Me.chkKubun_H_Evt.Name = "chkKubun_H_Evt"
        Me.chkKubun_H_Evt.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_H_Evt.TabIndex = 1
        Me.chkKubun_H_Evt.Text = "≤Õﬁ›ƒ"
        Me.chkKubun_H_Evt.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_H_Evt.UseVisualStyleBackColor = True
        '
        'chkKubun_H_Kei
        '
        Me.chkKubun_H_Kei.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_H_Kei.Location = New System.Drawing.Point(221, 72)
        Me.chkKubun_H_Kei.Name = "chkKubun_H_Kei"
        Me.chkKubun_H_Kei.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_H_Kei.TabIndex = 1
        Me.chkKubun_H_Kei.Text = "åyåÃè·"
        Me.chkKubun_H_Kei.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_H_Kei.UseVisualStyleBackColor = True
        '
        'chkKubun_S_Evt
        '
        Me.chkKubun_S_Evt.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_S_Evt.Location = New System.Drawing.Point(293, 48)
        Me.chkKubun_S_Evt.Name = "chkKubun_S_Evt"
        Me.chkKubun_S_Evt.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_S_Evt.TabIndex = 1
        Me.chkKubun_S_Evt.Text = "≤Õﬁ›ƒ"
        Me.chkKubun_S_Evt.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_S_Evt.UseVisualStyleBackColor = True
        '
        'chkKubun_S_Kei
        '
        Me.chkKubun_S_Kei.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_S_Kei.Location = New System.Drawing.Point(221, 48)
        Me.chkKubun_S_Kei.Name = "chkKubun_S_Kei"
        Me.chkKubun_S_Kei.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_S_Kei.TabIndex = 1
        Me.chkKubun_S_Kei.Text = "åyåÃè·"
        Me.chkKubun_S_Kei.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_S_Kei.UseVisualStyleBackColor = True
        '
        'chkKubun_H_Cmt
        '
        Me.chkKubun_H_Cmt.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_H_Cmt.Location = New System.Drawing.Point(77, 72)
        Me.chkKubun_H_Cmt.Name = "chkKubun_H_Cmt"
        Me.chkKubun_H_Cmt.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_H_Cmt.TabIndex = 1
        Me.chkKubun_H_Cmt.Text = "∫“›ƒ"
        Me.chkKubun_H_Cmt.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_H_Cmt.UseVisualStyleBackColor = True
        '
        'chkKubun_H_Jyu
        '
        Me.chkKubun_H_Jyu.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_H_Jyu.Location = New System.Drawing.Point(149, 72)
        Me.chkKubun_H_Jyu.Name = "chkKubun_H_Jyu"
        Me.chkKubun_H_Jyu.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_H_Jyu.TabIndex = 1
        Me.chkKubun_H_Jyu.Text = "èdåÃè·"
        Me.chkKubun_H_Jyu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_H_Jyu.UseVisualStyleBackColor = True
        '
        'chkKubun_A_Cmt
        '
        Me.chkKubun_A_Cmt.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_A_Cmt.Location = New System.Drawing.Point(77, 24)
        Me.chkKubun_A_Cmt.Name = "chkKubun_A_Cmt"
        Me.chkKubun_A_Cmt.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_A_Cmt.TabIndex = 1
        Me.chkKubun_A_Cmt.Text = "∫“›ƒ"
        Me.chkKubun_A_Cmt.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_A_Cmt.UseVisualStyleBackColor = True
        '
        'chkKubun_A_Evt
        '
        Me.chkKubun_A_Evt.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_A_Evt.Location = New System.Drawing.Point(293, 24)
        Me.chkKubun_A_Evt.Name = "chkKubun_A_Evt"
        Me.chkKubun_A_Evt.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_A_Evt.TabIndex = 1
        Me.chkKubun_A_Evt.Text = "≤Õﬁ›ƒ"
        Me.chkKubun_A_Evt.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_A_Evt.UseVisualStyleBackColor = True
        '
        'chkKubun_S_Cmt
        '
        Me.chkKubun_S_Cmt.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_S_Cmt.Location = New System.Drawing.Point(77, 48)
        Me.chkKubun_S_Cmt.Name = "chkKubun_S_Cmt"
        Me.chkKubun_S_Cmt.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_S_Cmt.TabIndex = 1
        Me.chkKubun_S_Cmt.Text = "∫“›ƒ"
        Me.chkKubun_S_Cmt.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_S_Cmt.UseVisualStyleBackColor = True
        '
        'chkKubun_S_Jyu
        '
        Me.chkKubun_S_Jyu.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_S_Jyu.Location = New System.Drawing.Point(149, 48)
        Me.chkKubun_S_Jyu.Name = "chkKubun_S_Jyu"
        Me.chkKubun_S_Jyu.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_S_Jyu.TabIndex = 1
        Me.chkKubun_S_Jyu.Text = "èdåÃè·"
        Me.chkKubun_S_Jyu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_S_Jyu.UseVisualStyleBackColor = True
        '
        'chkKubun_A_Kei
        '
        Me.chkKubun_A_Kei.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_A_Kei.Location = New System.Drawing.Point(221, 24)
        Me.chkKubun_A_Kei.Name = "chkKubun_A_Kei"
        Me.chkKubun_A_Kei.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_A_Kei.TabIndex = 1
        Me.chkKubun_A_Kei.Text = "åyåÃè·"
        Me.chkKubun_A_Kei.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_A_Kei.UseVisualStyleBackColor = True
        '
        'chkKubun_A_Jyu
        '
        Me.chkKubun_A_Jyu.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKubun_A_Jyu.Location = New System.Drawing.Point(149, 24)
        Me.chkKubun_A_Jyu.Name = "chkKubun_A_Jyu"
        Me.chkKubun_A_Jyu.Size = New System.Drawing.Size(68, 24)
        Me.chkKubun_A_Jyu.TabIndex = 1
        Me.chkKubun_A_Jyu.Text = "èdåÃè·"
        Me.chkKubun_A_Jyu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKubun_A_Jyu.UseVisualStyleBackColor = True
        '
        'dgvLog
        '
        Me.dgvLog.AllowUserToAddRows = False
        Me.dgvLog.AllowUserToDeleteRows = False
        Me.dgvLog.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvLog.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvLog.Location = New System.Drawing.Point(3, 39)
        Me.dgvLog.Name = "dgvLog"
        Me.dgvLog.ReadOnly = True
        Me.dgvLog.RowTemplate.Height = 21
        Me.dgvLog.Size = New System.Drawing.Size(1252, 599)
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
        Me.Label2.Location = New System.Drawing.Point(251, 5)
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
        Me.nudRowMaxCount.Location = New System.Drawing.Point(275, 1)
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
        Me.TableLayoutPanel2.Controls.Add(Me.dgvLog, 0, 1)
        Me.TableLayoutPanel2.Controls.Add(Me.TableLayoutPanel3, 0, 0)
        Me.TableLayoutPanel2.Controls.Add(Me.grpFilter, 0, 2)
        Me.TableLayoutPanel2.Location = New System.Drawing.Point(12, 1)
        Me.TableLayoutPanel2.Name = "TableLayoutPanel2"
        Me.TableLayoutPanel2.RowCount = 3
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 36.0!))
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 260.0!))
        Me.TableLayoutPanel2.Size = New System.Drawing.Size(1258, 901)
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
        Me.TableLayoutPanel3.Controls.Add(Me.Panel13, 2, 0)
        Me.TableLayoutPanel3.Location = New System.Drawing.Point(0, 3)
        Me.TableLayoutPanel3.Margin = New System.Windows.Forms.Padding(0, 3, 0, 3)
        Me.TableLayoutPanel3.Name = "TableLayoutPanel3"
        Me.TableLayoutPanel3.RowCount = 1
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Size = New System.Drawing.Size(1258, 30)
        Me.TableLayoutPanel3.TabIndex = 11
        '
        'Panel13
        '
        Me.Panel13.Controls.Add(Me.Label9)
        Me.Panel13.Controls.Add(Me.nudRowMaxCount)
        Me.Panel13.Controls.Add(Me.Label2)
        Me.Panel13.Controls.Add(Me.lblRowCount)
        Me.Panel13.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel13.Location = New System.Drawing.Point(888, 0)
        Me.Panel13.Margin = New System.Windows.Forms.Padding(0)
        Me.Panel13.Name = "Panel13"
        Me.Panel13.Size = New System.Drawing.Size(370, 30)
        Me.Panel13.TabIndex = 9
        '
        'frmMain_Log2
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.TableLayoutPanel2)
        Me.Name = "frmMain_Log2"
        Me.Text = "·råüópÉçÉOâÊñ ÇÃêeÉtÉHÅ[ÉÄ"
        Me.Controls.SetChildIndex(Me.TableLayoutPanel2, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.grpFilter.ResumeLayout(False)
        Me.Panel14.ResumeLayout(False)
        Me.grpFilter_Time.ResumeLayout(False)
        Me.grpFilter_Time.PerformLayout()
        Me.m_grpFilter_Code.ResumeLayout(False)
        Me.m_grpFilter_Code.PerformLayout()
        Me.grpFilter_Kubun.ResumeLayout(False)
        Me.grpFilter_Kubun.PerformLayout()
        CType(Me.dgvLog, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudRowMaxCount, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TableLayoutPanel2.ResumeLayout(False)
        Me.TableLayoutPanel3.ResumeLayout(False)
        Me.TableLayoutPanel3.PerformLayout()
        Me.Panel13.ResumeLayout(False)
        Me.Panel13.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Protected Friend WithEvents lblClock As System.Windows.Forms.Label
    Protected Friend WithEvents prbReadRow As System.Windows.Forms.ProgressBar
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Protected Friend WithEvents lblRowCount As System.Windows.Forms.Label
    Protected Friend WithEvents grpFilter As System.Windows.Forms.GroupBox
    Protected Friend WithEvents grpFilter_Kubun As System.Windows.Forms.GroupBox
    Protected Friend WithEvents chkKubun_A_Evt As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_A_Kei As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_A_Jyu As System.Windows.Forms.CheckBox
    Protected Friend WithEvents m_grpFilter_Code As System.Windows.Forms.GroupBox
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Protected Friend WithEvents grpFilter_PC As System.Windows.Forms.GroupBox
    Protected Friend WithEvents grpFilter_Time As System.Windows.Forms.GroupBox
    Protected Friend WithEvents dtpEndDay As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpEndTime As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpStartDay As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents dtpStartTime As System.Windows.Forms.DateTimePicker
    Protected Friend WithEvents timClock As System.Windows.Forms.Timer
    Protected Friend WithEvents txtCode_To As System.Windows.Forms.TextBox
    Protected Friend WithEvents txtCode_Not As System.Windows.Forms.TextBox
    Protected Friend WithEvents txtCode_Select As System.Windows.Forms.TextBox
    Protected Friend WithEvents txtCode_From As System.Windows.Forms.TextBox
    Protected Friend WithEvents chkEndDay As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkStartDay As System.Windows.Forms.CheckBox
    Protected Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents nudRowMaxCount As System.Windows.Forms.NumericUpDown
    Protected Friend WithEvents dgvLog As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Protected Friend WithEvents chkKubun_H_Evt As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_H_Kei As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_S_Evt As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_S_Kei As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_H_Jyu As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_S_Jyu As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_A_Cmt As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_H_Cmt As System.Windows.Forms.CheckBox
    Protected Friend WithEvents chkKubun_S_Cmt As System.Windows.Forms.CheckBox
    Friend WithEvents TableLayoutPanel2 As TableLayoutPanel
    Friend WithEvents TableLayoutPanel3 As TableLayoutPanel
    Friend WithEvents Panel13 As Panel
    Friend WithEvents Panel14 As Panel
    Protected Friend WithEvents grpFilter_PCKIND As GroupBox
End Class
