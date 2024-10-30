<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmStatusLog
    Inherits SO_GAMENN.frmMain_StatusLog

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
        Dim DataGridViewCellStyle15 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle16 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle17 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle18 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle19 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle20 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim DataGridViewCellStyle21 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Me.tabFilter_KUBUN = New tClassLibrary.tCnt.tCnt_TabControl()
        Me.PCPage1 = New System.Windows.Forms.TabPage()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.chkKen_StpSys_U = New System.Windows.Forms.CheckBox()
        Me.chkKen_NotChk_U = New System.Windows.Forms.CheckBox()
        Me.chkKen_NoSave_U = New System.Windows.Forms.CheckBox()
        Me.chkKen_Edge_U = New System.Windows.Forms.CheckBox()
        Me.chkKen_Syu_U = New System.Windows.Forms.CheckBox()
        Me.chkKen_Ok_U = New System.Windows.Forms.CheckBox()
        Me.chkKen_Stp_U = New System.Windows.Forms.CheckBox()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.chkKik_Ok_U = New System.Windows.Forms.CheckBox()
        Me.chkKik_Kei_U = New System.Windows.Forms.CheckBox()
        Me.chkKik_Jyu_U = New System.Windows.Forms.CheckBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.chkKen_StpSys_O = New System.Windows.Forms.CheckBox()
        Me.chkKen_NotChk_O = New System.Windows.Forms.CheckBox()
        Me.chkKad_Init = New System.Windows.Forms.CheckBox()
        Me.chkKen_NoSave_O = New System.Windows.Forms.CheckBox()
        Me.chkKen_Edge_O = New System.Windows.Forms.CheckBox()
        Me.chkKen_Syu_O = New System.Windows.Forms.CheckBox()
        Me.chkKen_Ok_O = New System.Windows.Forms.CheckBox()
        Me.chkKen_Stp_O = New System.Windows.Forms.CheckBox()
        Me.chkKik_Ok_O = New System.Windows.Forms.CheckBox()
        Me.chkKik_Kei_O = New System.Windows.Forms.CheckBox()
        Me.chkKik_Jyu_O = New System.Windows.Forms.CheckBox()
        Me.chkKad_Stp = New System.Windows.Forms.CheckBox()
        Me.chkKad_Kad = New System.Windows.Forms.CheckBox()
        Me.chkKad_Ret = New System.Windows.Forms.CheckBox()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.PCPage2 = New System.Windows.Forms.TabPage()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.DataGridViewTextBoxColumn1 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn2 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn3 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn4 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn5 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn6 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.DataGridViewTextBoxColumn7 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.grpFilter_KUBUN.SuspendLayout()
        Me.grpFilter_Time.SuspendLayout()
        Me.tabFilter_KUBUN.SuspendLayout()
        Me.PCPage1.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpFilter_KUBUN
        '
        Me.grpFilter_KUBUN.Controls.Add(Me.tabFilter_KUBUN)
        Me.grpFilter_KUBUN.Location = New System.Drawing.Point(392, 0)
        Me.grpFilter_KUBUN.Size = New System.Drawing.Size(825, 175)
        '
        'grpFilter_Time
        '
        Me.grpFilter_Time.Location = New System.Drawing.Point(10, 0)
        '
        'dtpEndDay
        '
        Me.dtpEndDay.Value = New Date(2013, 9, 18, 23, 59, 59, 0)
        '
        'dtpEndTime
        '
        Me.dtpEndTime.Value = New Date(2013, 9, 18, 23, 59, 59, 0)
        '
        'dtpStartDay
        '
        Me.dtpStartDay.Value = New Date(2013, 9, 18, 0, 0, 0, 0)
        '
        'dtpStartTime
        '
        Me.dtpStartTime.Value = New Date(2013, 9, 18, 0, 0, 0, 0)
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(0, 908)
        Me.grpFunction.Size = New System.Drawing.Size(1270, 84)
        '
        'btnF02
        '
        Me.btnF02.Size = New System.Drawing.Size(81, 48)
        '
        'lblF02_b
        '
        Me.lblF02_b.Size = New System.Drawing.Size(81, 48)
        '
        'btnF03
        '
        Me.btnF03.Size = New System.Drawing.Size(81, 48)
        '
        'lblF03_b
        '
        Me.lblF03_b.Size = New System.Drawing.Size(81, 48)
        '
        'btnF01
        '
        Me.btnF01.Size = New System.Drawing.Size(81, 48)
        '
        'lblF01_b
        '
        Me.lblF01_b.Size = New System.Drawing.Size(81, 48)
        '
        'btnF12
        '
        Me.btnF12.Size = New System.Drawing.Size(81, 48)
        '
        'btnF11
        '
        Me.btnF11.Size = New System.Drawing.Size(81, 48)
        '
        'btnF10
        '
        Me.btnF10.Size = New System.Drawing.Size(81, 48)
        '
        'btnF09
        '
        Me.btnF09.Size = New System.Drawing.Size(81, 48)
        '
        'btnF08
        '
        Me.btnF08.Size = New System.Drawing.Size(81, 48)
        '
        'btnF07
        '
        Me.btnF07.Size = New System.Drawing.Size(81, 48)
        '
        'btnF06
        '
        Me.btnF06.Size = New System.Drawing.Size(81, 48)
        '
        'btnF05
        '
        Me.btnF05.Size = New System.Drawing.Size(81, 48)
        '
        'btnF04
        '
        Me.btnF04.Size = New System.Drawing.Size(81, 48)
        '
        'lblF12_b
        '
        Me.lblF12_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF11_b
        '
        Me.lblF11_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF10_b
        '
        Me.lblF10_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF09_b
        '
        Me.lblF09_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF08_b
        '
        Me.lblF08_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF07_b
        '
        Me.lblF07_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF06_b
        '
        Me.lblF06_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF05_b
        '
        Me.lblF05_b.Size = New System.Drawing.Size(81, 48)
        '
        'lblF04_b
        '
        Me.lblF04_b.Size = New System.Drawing.Size(81, 48)
        '
        'tabFilter_KUBUN
        '
        Me.tabFilter_KUBUN.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tabFilter_KUBUN.Controls.Add(Me.PCPage1)
        Me.tabFilter_KUBUN.Controls.Add(Me.PCPage2)
        Me.tabFilter_KUBUN.ItemSize = New System.Drawing.Size(54, 5)
        Me.tabFilter_KUBUN.Location = New System.Drawing.Point(6, 16)
        Me.tabFilter_KUBUN.Name = "tabFilter_KUBUN"
        Me.tabFilter_KUBUN.SelectedIndex = 0
        Me.tabFilter_KUBUN.Size = New System.Drawing.Size(805, 156)
        Me.tabFilter_KUBUN.TabIndex = 21
        '
        'PCPage1
        '
        Me.PCPage1.Controls.Add(Me.Label5)
        Me.PCPage1.Controls.Add(Me.Label6)
        Me.PCPage1.Controls.Add(Me.chkKen_StpSys_U)
        Me.PCPage1.Controls.Add(Me.chkKen_NotChk_U)
        Me.PCPage1.Controls.Add(Me.chkKen_NoSave_U)
        Me.PCPage1.Controls.Add(Me.chkKen_Edge_U)
        Me.PCPage1.Controls.Add(Me.chkKen_Syu_U)
        Me.PCPage1.Controls.Add(Me.chkKen_Ok_U)
        Me.PCPage1.Controls.Add(Me.chkKen_Stp_U)
        Me.PCPage1.Controls.Add(Me.Label4)
        Me.PCPage1.Controls.Add(Me.chkKik_Ok_U)
        Me.PCPage1.Controls.Add(Me.chkKik_Kei_U)
        Me.PCPage1.Controls.Add(Me.chkKik_Jyu_U)
        Me.PCPage1.Controls.Add(Me.Label3)
        Me.PCPage1.Controls.Add(Me.chkKen_StpSys_O)
        Me.PCPage1.Controls.Add(Me.chkKen_NotChk_O)
        Me.PCPage1.Controls.Add(Me.chkKad_Init)
        Me.PCPage1.Controls.Add(Me.chkKen_NoSave_O)
        Me.PCPage1.Controls.Add(Me.chkKen_Edge_O)
        Me.PCPage1.Controls.Add(Me.chkKen_Syu_O)
        Me.PCPage1.Controls.Add(Me.chkKen_Ok_O)
        Me.PCPage1.Controls.Add(Me.chkKen_Stp_O)
        Me.PCPage1.Controls.Add(Me.chkKik_Ok_O)
        Me.PCPage1.Controls.Add(Me.chkKik_Kei_O)
        Me.PCPage1.Controls.Add(Me.chkKik_Jyu_O)
        Me.PCPage1.Controls.Add(Me.chkKad_Stp)
        Me.PCPage1.Controls.Add(Me.chkKad_Kad)
        Me.PCPage1.Controls.Add(Me.chkKad_Ret)
        Me.PCPage1.Controls.Add(Me.Label8)
        Me.PCPage1.Controls.Add(Me.Label12)
        Me.PCPage1.Controls.Add(Me.Label13)
        Me.PCPage1.Location = New System.Drawing.Point(4, 9)
        Me.PCPage1.Name = "PCPage1"
        Me.PCPage1.Padding = New System.Windows.Forms.Padding(3)
        Me.PCPage1.Size = New System.Drawing.Size(797, 143)
        Me.PCPage1.TabIndex = 0
        Me.PCPage1.UseVisualStyleBackColor = True
        '
        'Label5
        '
        Me.Label5.Location = New System.Drawing.Point(82, 114)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(48, 24)
        Me.Label5.TabIndex = 76
        Me.Label5.Text = "裏面"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label6
        '
        Me.Label6.Location = New System.Drawing.Point(82, 90)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(48, 24)
        Me.Label6.TabIndex = 75
        Me.Label6.Text = "表面"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'chkKen_StpSys_U
        '
        Me.chkKen_StpSys_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_StpSys_U.Location = New System.Drawing.Point(648, 114)
        Me.chkKen_StpSys_U.Name = "chkKen_StpSys_U"
        Me.chkKen_StpSys_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_StpSys_U.TabIndex = 74
        Me.chkKen_StpSys_U.Text = "検査停止"
        Me.chkKen_StpSys_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_StpSys_U.UseVisualStyleBackColor = True
        '
        'chkKen_NotChk_U
        '
        Me.chkKen_NotChk_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_NotChk_U.Location = New System.Drawing.Point(562, 114)
        Me.chkKen_NotChk_U.Name = "chkKen_NotChk_U"
        Me.chkKen_NotChk_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_NotChk_U.TabIndex = 73
        Me.chkKen_NotChk_U.Text = "未検査"
        Me.chkKen_NotChk_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_NotChk_U.UseVisualStyleBackColor = True
        '
        'chkKen_NoSave_U
        '
        Me.chkKen_NoSave_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_NoSave_U.Location = New System.Drawing.Point(476, 114)
        Me.chkKen_NoSave_U.Name = "chkKen_NoSave_U"
        Me.chkKen_NoSave_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_NoSave_U.TabIndex = 72
        Me.chkKen_NoSave_U.Text = "未保存"
        Me.chkKen_NoSave_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_NoSave_U.UseVisualStyleBackColor = True
        '
        'chkKen_Edge_U
        '
        Me.chkKen_Edge_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Edge_U.Location = New System.Drawing.Point(390, 114)
        Me.chkKen_Edge_U.Name = "chkKen_Edge_U"
        Me.chkKen_Edge_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Edge_U.TabIndex = 71
        Me.chkKen_Edge_U.Text = "ｴｯｼﾞ不正"
        Me.chkKen_Edge_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Edge_U.UseVisualStyleBackColor = True
        '
        'chkKen_Syu_U
        '
        Me.chkKen_Syu_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Syu_U.Location = New System.Drawing.Point(304, 114)
        Me.chkKen_Syu_U.Name = "chkKen_Syu_U"
        Me.chkKen_Syu_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Syu_U.TabIndex = 70
        Me.chkKen_Syu_U.Text = "縮退"
        Me.chkKen_Syu_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Syu_U.UseVisualStyleBackColor = True
        '
        'chkKen_Ok_U
        '
        Me.chkKen_Ok_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Ok_U.Location = New System.Drawing.Point(218, 114)
        Me.chkKen_Ok_U.Name = "chkKen_Ok_U"
        Me.chkKen_Ok_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Ok_U.TabIndex = 69
        Me.chkKen_Ok_U.Text = "正常"
        Me.chkKen_Ok_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Ok_U.UseVisualStyleBackColor = True
        '
        'chkKen_Stp_U
        '
        Me.chkKen_Stp_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Stp_U.Location = New System.Drawing.Point(132, 114)
        Me.chkKen_Stp_U.Name = "chkKen_Stp_U"
        Me.chkKen_Stp_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Stp_U.TabIndex = 68
        Me.chkKen_Stp_U.Text = "停止"
        Me.chkKen_Stp_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Stp_U.UseVisualStyleBackColor = True
        '
        'Label4
        '
        Me.Label4.Location = New System.Drawing.Point(82, 60)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(48, 24)
        Me.Label4.TabIndex = 67
        Me.Label4.Text = "裏面"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'chkKik_Ok_U
        '
        Me.chkKik_Ok_U.AccessibleRole = System.Windows.Forms.AccessibleRole.None
        Me.chkKik_Ok_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKik_Ok_U.Location = New System.Drawing.Point(132, 60)
        Me.chkKik_Ok_U.Name = "chkKik_Ok_U"
        Me.chkKik_Ok_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKik_Ok_U.TabIndex = 64
        Me.chkKik_Ok_U.Text = "正常"
        Me.chkKik_Ok_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKik_Ok_U.UseVisualStyleBackColor = True
        '
        'chkKik_Kei_U
        '
        Me.chkKik_Kei_U.AllowDrop = True
        Me.chkKik_Kei_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKik_Kei_U.Location = New System.Drawing.Point(218, 60)
        Me.chkKik_Kei_U.Name = "chkKik_Kei_U"
        Me.chkKik_Kei_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKik_Kei_U.TabIndex = 65
        Me.chkKik_Kei_U.Text = "軽故障"
        Me.chkKik_Kei_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKik_Kei_U.UseVisualStyleBackColor = True
        '
        'chkKik_Jyu_U
        '
        Me.chkKik_Jyu_U.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKik_Jyu_U.Location = New System.Drawing.Point(304, 60)
        Me.chkKik_Jyu_U.Name = "chkKik_Jyu_U"
        Me.chkKik_Jyu_U.Size = New System.Drawing.Size(86, 24)
        Me.chkKik_Jyu_U.TabIndex = 66
        Me.chkKik_Jyu_U.Text = "重故障"
        Me.chkKik_Jyu_U.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKik_Jyu_U.UseVisualStyleBackColor = True
        '
        'Label3
        '
        Me.Label3.Location = New System.Drawing.Point(82, 36)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(48, 24)
        Me.Label3.TabIndex = 63
        Me.Label3.Text = "表面"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'chkKen_StpSys_O
        '
        Me.chkKen_StpSys_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_StpSys_O.Location = New System.Drawing.Point(648, 90)
        Me.chkKen_StpSys_O.Name = "chkKen_StpSys_O"
        Me.chkKen_StpSys_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_StpSys_O.TabIndex = 62
        Me.chkKen_StpSys_O.Text = "検査停止"
        Me.chkKen_StpSys_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_StpSys_O.UseVisualStyleBackColor = True
        '
        'chkKen_NotChk_O
        '
        Me.chkKen_NotChk_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_NotChk_O.Location = New System.Drawing.Point(562, 90)
        Me.chkKen_NotChk_O.Name = "chkKen_NotChk_O"
        Me.chkKen_NotChk_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_NotChk_O.TabIndex = 60
        Me.chkKen_NotChk_O.Text = "未検査"
        Me.chkKen_NotChk_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_NotChk_O.UseVisualStyleBackColor = True
        '
        'chkKad_Init
        '
        Me.chkKad_Init.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKad_Init.Location = New System.Drawing.Point(132, 6)
        Me.chkKad_Init.Name = "chkKad_Init"
        Me.chkKad_Init.Size = New System.Drawing.Size(86, 24)
        Me.chkKad_Init.TabIndex = 36
        Me.chkKad_Init.Text = "初期化中"
        Me.chkKad_Init.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKad_Init.UseVisualStyleBackColor = True
        '
        'chkKen_NoSave_O
        '
        Me.chkKen_NoSave_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_NoSave_O.Location = New System.Drawing.Point(476, 90)
        Me.chkKen_NoSave_O.Name = "chkKen_NoSave_O"
        Me.chkKen_NoSave_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_NoSave_O.TabIndex = 58
        Me.chkKen_NoSave_O.Text = "未保存"
        Me.chkKen_NoSave_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_NoSave_O.UseVisualStyleBackColor = True
        '
        'chkKen_Edge_O
        '
        Me.chkKen_Edge_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Edge_O.Location = New System.Drawing.Point(390, 90)
        Me.chkKen_Edge_O.Name = "chkKen_Edge_O"
        Me.chkKen_Edge_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Edge_O.TabIndex = 56
        Me.chkKen_Edge_O.Text = "ｴｯｼﾞ不正"
        Me.chkKen_Edge_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Edge_O.UseVisualStyleBackColor = True
        '
        'chkKen_Syu_O
        '
        Me.chkKen_Syu_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Syu_O.Location = New System.Drawing.Point(304, 90)
        Me.chkKen_Syu_O.Name = "chkKen_Syu_O"
        Me.chkKen_Syu_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Syu_O.TabIndex = 54
        Me.chkKen_Syu_O.Text = "縮退"
        Me.chkKen_Syu_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Syu_O.UseVisualStyleBackColor = True
        '
        'chkKen_Ok_O
        '
        Me.chkKen_Ok_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Ok_O.Location = New System.Drawing.Point(218, 90)
        Me.chkKen_Ok_O.Name = "chkKen_Ok_O"
        Me.chkKen_Ok_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Ok_O.TabIndex = 52
        Me.chkKen_Ok_O.Text = "正常"
        Me.chkKen_Ok_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Ok_O.UseVisualStyleBackColor = True
        '
        'chkKen_Stp_O
        '
        Me.chkKen_Stp_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKen_Stp_O.Location = New System.Drawing.Point(132, 90)
        Me.chkKen_Stp_O.Name = "chkKen_Stp_O"
        Me.chkKen_Stp_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKen_Stp_O.TabIndex = 49
        Me.chkKen_Stp_O.Text = "停止"
        Me.chkKen_Stp_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKen_Stp_O.UseVisualStyleBackColor = True
        '
        'chkKik_Ok_O
        '
        Me.chkKik_Ok_O.AccessibleRole = System.Windows.Forms.AccessibleRole.None
        Me.chkKik_Ok_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKik_Ok_O.Location = New System.Drawing.Point(132, 36)
        Me.chkKik_Ok_O.Name = "chkKik_Ok_O"
        Me.chkKik_Ok_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKik_Ok_O.TabIndex = 43
        Me.chkKik_Ok_O.Text = "正常"
        Me.chkKik_Ok_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKik_Ok_O.UseVisualStyleBackColor = True
        '
        'chkKik_Kei_O
        '
        Me.chkKik_Kei_O.AllowDrop = True
        Me.chkKik_Kei_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKik_Kei_O.Location = New System.Drawing.Point(218, 36)
        Me.chkKik_Kei_O.Name = "chkKik_Kei_O"
        Me.chkKik_Kei_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKik_Kei_O.TabIndex = 46
        Me.chkKik_Kei_O.Text = "軽故障"
        Me.chkKik_Kei_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKik_Kei_O.UseVisualStyleBackColor = True
        '
        'chkKik_Jyu_O
        '
        Me.chkKik_Jyu_O.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKik_Jyu_O.Location = New System.Drawing.Point(304, 36)
        Me.chkKik_Jyu_O.Name = "chkKik_Jyu_O"
        Me.chkKik_Jyu_O.Size = New System.Drawing.Size(86, 24)
        Me.chkKik_Jyu_O.TabIndex = 48
        Me.chkKik_Jyu_O.Text = "重故障"
        Me.chkKik_Jyu_O.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKik_Jyu_O.UseVisualStyleBackColor = True
        '
        'chkKad_Stp
        '
        Me.chkKad_Stp.AllowDrop = True
        Me.chkKad_Stp.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKad_Stp.Location = New System.Drawing.Point(218, 6)
        Me.chkKad_Stp.Name = "chkKad_Stp"
        Me.chkKad_Stp.Size = New System.Drawing.Size(86, 24)
        Me.chkKad_Stp.TabIndex = 37
        Me.chkKad_Stp.Text = "停止中"
        Me.chkKad_Stp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKad_Stp.UseVisualStyleBackColor = True
        '
        'chkKad_Kad
        '
        Me.chkKad_Kad.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKad_Kad.Location = New System.Drawing.Point(304, 6)
        Me.chkKad_Kad.Name = "chkKad_Kad"
        Me.chkKad_Kad.Size = New System.Drawing.Size(86, 24)
        Me.chkKad_Kad.TabIndex = 40
        Me.chkKad_Kad.Text = "稼動中"
        Me.chkKad_Kad.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKad_Kad.UseVisualStyleBackColor = True
        '
        'chkKad_Ret
        '
        Me.chkKad_Ret.AllowDrop = True
        Me.chkKad_Ret.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKad_Ret.Location = New System.Drawing.Point(390, 6)
        Me.chkKad_Ret.Name = "chkKad_Ret"
        Me.chkKad_Ret.Size = New System.Drawing.Size(86, 24)
        Me.chkKad_Ret.TabIndex = 42
        Me.chkKad_Ret.Text = "ﾘﾄﾗｲ中"
        Me.chkKad_Ret.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKad_Ret.UseVisualStyleBackColor = True
        '
        'Label8
        '
        Me.Label8.Location = New System.Drawing.Point(4, 8)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(76, 24)
        Me.Label8.TabIndex = 26
        Me.Label8.Text = "稼動状態"
        Me.Label8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label12
        '
        Me.Label12.Location = New System.Drawing.Point(4, 98)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(76, 24)
        Me.Label12.TabIndex = 28
        Me.Label12.Text = "検査状態"
        Me.Label12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label13
        '
        Me.Label13.Location = New System.Drawing.Point(4, 48)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(76, 24)
        Me.Label13.TabIndex = 30
        Me.Label13.Text = "機器状態"
        Me.Label13.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'PCPage2
        '
        Me.PCPage2.Location = New System.Drawing.Point(4, 9)
        Me.PCPage2.Name = "PCPage2"
        Me.PCPage2.Padding = New System.Windows.Forms.Padding(3)
        Me.PCPage2.Size = New System.Drawing.Size(797, 143)
        Me.PCPage2.TabIndex = 1
        Me.PCPage2.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(224, 3)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(176, 12)
        Me.Label1.TabIndex = 4
        Me.Label1.Text = "HANTEI"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'Label10
        '
        Me.Label10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label10.Font = New System.Drawing.Font("ＭＳ 明朝", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.Location = New System.Drawing.Point(408, 3)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(176, 12)
        Me.Label10.TabIndex = 4
        Me.Label10.Text = "CYCLE"
        Me.Label10.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'DataGridViewTextBoxColumn1
        '
        DataGridViewCellStyle15.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        DataGridViewCellStyle15.Format = "yyyy/MM/dd HH:mm:ss"
        Me.DataGridViewTextBoxColumn1.DefaultCellStyle = DataGridViewCellStyle15
        Me.DataGridViewTextBoxColumn1.HeaderText = "変更日"
        Me.DataGridViewTextBoxColumn1.Name = "DataGridViewTextBoxColumn1"
        Me.DataGridViewTextBoxColumn1.ReadOnly = True
        Me.DataGridViewTextBoxColumn1.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn1.Width = 160
        '
        'DataGridViewTextBoxColumn2
        '
        DataGridViewCellStyle16.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter
        Me.DataGridViewTextBoxColumn2.DefaultCellStyle = DataGridViewCellStyle16
        Me.DataGridViewTextBoxColumn2.HeaderText = "管理No"
        Me.DataGridViewTextBoxColumn2.Name = "DataGridViewTextBoxColumn2"
        Me.DataGridViewTextBoxColumn2.ReadOnly = True
        Me.DataGridViewTextBoxColumn2.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn2.Width = 160
        '
        'DataGridViewTextBoxColumn3
        '
        DataGridViewCellStyle17.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter
        Me.DataGridViewTextBoxColumn3.DefaultCellStyle = DataGridViewCellStyle17
        Me.DataGridViewTextBoxColumn3.HeaderText = "コイルNo"
        Me.DataGridViewTextBoxColumn3.Name = "DataGridViewTextBoxColumn3"
        Me.DataGridViewTextBoxColumn3.ReadOnly = True
        Me.DataGridViewTextBoxColumn3.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn3.Width = 110
        '
        'DataGridViewTextBoxColumn4
        '
        DataGridViewCellStyle18.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter
        Me.DataGridViewTextBoxColumn4.DefaultCellStyle = DataGridViewCellStyle18
        Me.DataGridViewTextBoxColumn4.HeaderText = "稼動状態"
        Me.DataGridViewTextBoxColumn4.Name = "DataGridViewTextBoxColumn4"
        Me.DataGridViewTextBoxColumn4.ReadOnly = True
        Me.DataGridViewTextBoxColumn4.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn4.Width = 120
        '
        'DataGridViewTextBoxColumn5
        '
        DataGridViewCellStyle19.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter
        Me.DataGridViewTextBoxColumn5.DefaultCellStyle = DataGridViewCellStyle19
        Me.DataGridViewTextBoxColumn5.HeaderText = "機器状態"
        Me.DataGridViewTextBoxColumn5.Name = "DataGridViewTextBoxColumn5"
        Me.DataGridViewTextBoxColumn5.ReadOnly = True
        Me.DataGridViewTextBoxColumn5.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn5.Width = 120
        '
        'DataGridViewTextBoxColumn6
        '
        DataGridViewCellStyle20.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter
        Me.DataGridViewTextBoxColumn6.DefaultCellStyle = DataGridViewCellStyle20
        Me.DataGridViewTextBoxColumn6.HeaderText = "検査状態"
        Me.DataGridViewTextBoxColumn6.Name = "DataGridViewTextBoxColumn6"
        Me.DataGridViewTextBoxColumn6.ReadOnly = True
        Me.DataGridViewTextBoxColumn6.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn6.Width = 120
        '
        'DataGridViewTextBoxColumn7
        '
        DataGridViewCellStyle21.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft
        Me.DataGridViewTextBoxColumn7.DefaultCellStyle = DataGridViewCellStyle21
        Me.DataGridViewTextBoxColumn7.HeaderText = "ガイダンスメッセージ"
        Me.DataGridViewTextBoxColumn7.Name = "DataGridViewTextBoxColumn7"
        Me.DataGridViewTextBoxColumn7.ReadOnly = True
        Me.DataGridViewTextBoxColumn7.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.DataGridViewTextBoxColumn7.Width = 685
        '
        'frmStatusLog
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Name = "frmStatusLog"
        Me.Text = "検査状態ログ画面"
        Me.grpFilter_KUBUN.ResumeLayout(False)
        Me.grpFilter_Time.ResumeLayout(False)
        Me.grpFilter_Time.PerformLayout()
        Me.tabFilter_KUBUN.ResumeLayout(False)
        Me.PCPage1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents tabFilter_KUBUN As tClassLibrary.tCnt.tCnt_TabControl
    Friend WithEvents PCPage1 As System.Windows.Forms.TabPage
    ' Friend WithEvents Label1 As System.Windows.Forms.Label
    'Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents PCPage2 As System.Windows.Forms.TabPage
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents chkKen_StpSys_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKen_NotChk_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKad_Init As System.Windows.Forms.CheckBox
    Friend WithEvents chkKen_NoSave_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKen_Edge_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKen_Syu_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKen_Ok_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKen_Stp_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKik_Ok_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKik_Kei_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKik_Jyu_O As System.Windows.Forms.CheckBox
    Friend WithEvents chkKad_Stp As System.Windows.Forms.CheckBox
    Friend WithEvents chkKad_Kad As System.Windows.Forms.CheckBox
    Friend WithEvents chkKad_Ret As System.Windows.Forms.CheckBox
    Friend WithEvents DataGridViewTextBoxColumn1 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn2 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn3 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn4 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn5 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn6 As DataGridViewTextBoxColumn
    Friend WithEvents DataGridViewTextBoxColumn7 As DataGridViewTextBoxColumn
    Friend WithEvents Label5 As Label
    Friend WithEvents Label6 As Label
    Friend WithEvents chkKen_StpSys_U As CheckBox
    Friend WithEvents chkKen_NotChk_U As CheckBox
    Friend WithEvents chkKen_NoSave_U As CheckBox
    Friend WithEvents chkKen_Edge_U As CheckBox
    Friend WithEvents chkKen_Syu_U As CheckBox
    Friend WithEvents chkKen_Ok_U As CheckBox
    Friend WithEvents chkKen_Stp_U As CheckBox
    Friend WithEvents Label4 As Label
    Friend WithEvents chkKik_Ok_U As CheckBox
    Friend WithEvents chkKik_Kei_U As CheckBox
    Friend WithEvents chkKik_Jyu_U As CheckBox
    Friend WithEvents Label3 As Label
End Class
