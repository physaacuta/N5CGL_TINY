<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmFFSearch
    Inherits tClassForm.frmBase_Main

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.grpFilter = New System.Windows.Forms.GroupBox()
        Me.grpFilter_KUBUN = New System.Windows.Forms.GroupBox()
        Me.chkKind_Cancel = New System.Windows.Forms.CheckBox()
        Me.chkKind_Failed = New System.Windows.Forms.CheckBox()
        Me.chkKind_NoSend = New System.Windows.Forms.CheckBox()
        Me.chkKind_Normal = New System.Windows.Forms.CheckBox()
        Me.grpFilter_Time = New System.Windows.Forms.GroupBox()
        Me.chkEndDay = New System.Windows.Forms.CheckBox()
        Me.chkStartDay = New System.Windows.Forms.CheckBox()
        Me.dtpEndDay = New System.Windows.Forms.DateTimePicker()
        Me.dtpEndTime = New System.Windows.Forms.DateTimePicker()
        Me.dtpStartDay = New System.Windows.Forms.DateTimePicker()
        Me.dtpStartTime = New System.Windows.Forms.DateTimePicker()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.Button1 = New System.Windows.Forms.Button()
        Me.TextBox1 = New System.Windows.Forms.TextBox()
        Me.prbReadRow = New System.Windows.Forms.ProgressBar()
        Me.dgvList = New SO_GAMENN.tCnt_DataGridView_turn()
        Me.grpFilter.SuspendLayout()
        Me.grpFilter_KUBUN.SuspendLayout()
        Me.grpFilter_Time.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        CType(Me.dgvList, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnF01
        '
        Me.btnF01.Text = "伝文出力"
        '
        'btnF12
        '
        Me.btnF12.Text = "メニュー"
        '
        'btnF11
        '
        Me.btnF11.Text = "画面保存"
        '
        'btnF10
        '
        Me.btnF10.Text = "画面印刷"
        '
        'btnF05
        '
        Me.btnF05.Text = "最新表示"
        '
        'grpFilter
        '
        Me.grpFilter.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.grpFilter.Controls.Add(Me.grpFilter_KUBUN)
        Me.grpFilter.Controls.Add(Me.grpFilter_Time)
        Me.grpFilter.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter.ForeColor = System.Drawing.Color.Navy
        Me.grpFilter.Location = New System.Drawing.Point(12, 6)
        Me.grpFilter.Name = "grpFilter"
        Me.grpFilter.Size = New System.Drawing.Size(860, 121)
        Me.grpFilter.TabIndex = 10
        Me.grpFilter.TabStop = False
        Me.grpFilter.Text = "フィルター項目"
        '
        'grpFilter_KUBUN
        '
        Me.grpFilter_KUBUN.Controls.Add(Me.chkKind_Cancel)
        Me.grpFilter_KUBUN.Controls.Add(Me.chkKind_Failed)
        Me.grpFilter_KUBUN.Controls.Add(Me.chkKind_NoSend)
        Me.grpFilter_KUBUN.Controls.Add(Me.chkKind_Normal)
        Me.grpFilter_KUBUN.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_KUBUN.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_KUBUN.Location = New System.Drawing.Point(392, 24)
        Me.grpFilter_KUBUN.Name = "grpFilter_KUBUN"
        Me.grpFilter_KUBUN.Size = New System.Drawing.Size(452, 88)
        Me.grpFilter_KUBUN.TabIndex = 3
        Me.grpFilter_KUBUN.TabStop = False
        Me.grpFilter_KUBUN.Text = "区分フィルター"
        '
        'chkKind_Cancel
        '
        Me.chkKind_Cancel.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKind_Cancel.Location = New System.Drawing.Point(330, 28)
        Me.chkKind_Cancel.Name = "chkKind_Cancel"
        Me.chkKind_Cancel.Size = New System.Drawing.Size(100, 50)
        Me.chkKind_Cancel.TabIndex = 6
        Me.chkKind_Cancel.Text = "再送止め"
        Me.chkKind_Cancel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKind_Cancel.UseVisualStyleBackColor = True
        '
        'chkKind_Failed
        '
        Me.chkKind_Failed.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKind_Failed.Location = New System.Drawing.Point(224, 28)
        Me.chkKind_Failed.Name = "chkKind_Failed"
        Me.chkKind_Failed.Size = New System.Drawing.Size(100, 50)
        Me.chkKind_Failed.TabIndex = 5
        Me.chkKind_Failed.Text = "送信失敗"
        Me.chkKind_Failed.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKind_Failed.UseVisualStyleBackColor = True
        '
        'chkKind_NoSend
        '
        Me.chkKind_NoSend.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKind_NoSend.Location = New System.Drawing.Point(118, 28)
        Me.chkKind_NoSend.Name = "chkKind_NoSend"
        Me.chkKind_NoSend.Size = New System.Drawing.Size(100, 50)
        Me.chkKind_NoSend.TabIndex = 4
        Me.chkKind_NoSend.Text = "未送信"
        Me.chkKind_NoSend.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKind_NoSend.UseVisualStyleBackColor = True
        '
        'chkKind_Normal
        '
        Me.chkKind_Normal.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkKind_Normal.Location = New System.Drawing.Point(11, 28)
        Me.chkKind_Normal.Name = "chkKind_Normal"
        Me.chkKind_Normal.Size = New System.Drawing.Size(100, 50)
        Me.chkKind_Normal.TabIndex = 3
        Me.chkKind_Normal.Text = "正常送信" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "完了"
        Me.chkKind_Normal.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkKind_Normal.UseVisualStyleBackColor = True
        '
        'grpFilter_Time
        '
        Me.grpFilter_Time.Controls.Add(Me.chkEndDay)
        Me.grpFilter_Time.Controls.Add(Me.chkStartDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpEndDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpEndTime)
        Me.grpFilter_Time.Controls.Add(Me.dtpStartDay)
        Me.grpFilter_Time.Controls.Add(Me.dtpStartTime)
        Me.grpFilter_Time.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFilter_Time.ForeColor = System.Drawing.Color.Black
        Me.grpFilter_Time.Location = New System.Drawing.Point(12, 24)
        Me.grpFilter_Time.Name = "grpFilter_Time"
        Me.grpFilter_Time.Size = New System.Drawing.Size(376, 88)
        Me.grpFilter_Time.TabIndex = 2
        Me.grpFilter_Time.TabStop = False
        Me.grpFilter_Time.Text = "時間フィルター"
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
        Me.chkEndDay.Text = "終了時間"
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
        Me.chkStartDay.Text = "開始時間"
        Me.chkStartDay.UseVisualStyleBackColor = True
        '
        'dtpEndDay
        '
        Me.dtpEndDay.CustomFormat = "yyyy/MM/dd"
        Me.dtpEndDay.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpEndDay.Format = System.Windows.Forms.DateTimePickerFormat.Custom
        Me.dtpEndDay.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpEndDay.Location = New System.Drawing.Point(108, 56)
        Me.dtpEndDay.Name = "dtpEndDay"
        Me.dtpEndDay.Size = New System.Drawing.Size(140, 26)
        Me.dtpEndDay.TabIndex = 5
        '
        'dtpEndTime
        '
        Me.dtpEndTime.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
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
        Me.dtpStartDay.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpStartDay.Format = System.Windows.Forms.DateTimePickerFormat.Custom
        Me.dtpStartDay.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpStartDay.Location = New System.Drawing.Point(108, 24)
        Me.dtpStartDay.Name = "dtpStartDay"
        Me.dtpStartDay.Size = New System.Drawing.Size(140, 26)
        Me.dtpStartDay.TabIndex = 3
        '
        'dtpStartTime
        '
        Me.dtpStartTime.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dtpStartTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpStartTime.ImeMode = System.Windows.Forms.ImeMode.Disable
        Me.dtpStartTime.Location = New System.Drawing.Point(252, 24)
        Me.dtpStartTime.Name = "dtpStartTime"
        Me.dtpStartTime.ShowUpDown = True
        Me.dtpStartTime.Size = New System.Drawing.Size(116, 26)
        Me.dtpStartTime.TabIndex = 2
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.Button1)
        Me.GroupBox1.Controls.Add(Me.TextBox1)
        Me.GroupBox1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.ForeColor = System.Drawing.Color.Black
        Me.GroupBox1.Location = New System.Drawing.Point(878, 6)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(379, 82)
        Me.GroupBox1.TabIndex = 12
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "保存先パス"
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(310, 37)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(60, 24)
        Me.Button1.TabIndex = 1
        Me.Button1.Text = "参照"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'TextBox1
        '
        Me.TextBox1.Location = New System.Drawing.Point(10, 38)
        Me.TextBox1.Name = "TextBox1"
        Me.TextBox1.Size = New System.Drawing.Size(294, 23)
        Me.TextBox1.TabIndex = 0
        '
        'prbReadRow
        '
        Me.prbReadRow.Location = New System.Drawing.Point(878, 96)
        Me.prbReadRow.Maximum = 10000
        Me.prbReadRow.Name = "prbReadRow"
        Me.prbReadRow.Size = New System.Drawing.Size(378, 24)
        Me.prbReadRow.Step = 1
        Me.prbReadRow.Style = System.Windows.Forms.ProgressBarStyle.Continuous
        Me.prbReadRow.TabIndex = 13
        Me.prbReadRow.Visible = False
        '
        'dgvList
        '
        Me.dgvList.bTanspose = False
        Me.dgvList.Location = New System.Drawing.Point(12, 133)
        Me.dgvList.Name = "dgvList"
        Me.dgvList.RowTemplate.Height = 21
        Me.dgvList.Size = New System.Drawing.Size(1245, 768)
        Me.dgvList.TabIndex = 14
        '
        'frmFFSearch
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.prbReadRow)
        Me.Controls.Add(Me.dgvList)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.grpFilter)
        Me.Name = "frmFFSearch"
        Me.Controls.SetChildIndex(Me.grpFilter, 0)
        Me.Controls.SetChildIndex(Me.GroupBox1, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.dgvList, 0)
        Me.Controls.SetChildIndex(Me.prbReadRow, 0)
        Me.grpFilter.ResumeLayout(False)
        Me.grpFilter_KUBUN.ResumeLayout(False)
        Me.grpFilter_Time.ResumeLayout(False)
        Me.grpFilter_Time.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.dgvList, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Protected Friend WithEvents grpFilter As GroupBox
    Protected Friend WithEvents grpFilter_KUBUN As GroupBox
    Protected Friend WithEvents grpFilter_Time As GroupBox
    Protected Friend WithEvents chkEndDay As CheckBox
    Protected Friend WithEvents chkStartDay As CheckBox
    Protected Friend WithEvents dtpEndDay As DateTimePicker
    Protected Friend WithEvents dtpEndTime As DateTimePicker
    Protected Friend WithEvents dtpStartDay As DateTimePicker
    Protected Friend WithEvents dtpStartTime As DateTimePicker
    Protected Friend WithEvents GroupBox1 As GroupBox
    Friend WithEvents Button1 As Button
    Friend WithEvents TextBox1 As TextBox
    Friend WithEvents chkKind_Failed As CheckBox
    Friend WithEvents chkKind_NoSend As CheckBox
    Friend WithEvents chkKind_Normal As CheckBox
    Friend WithEvents chkKind_Cancel As CheckBox
    Protected Friend WithEvents prbReadRow As ProgressBar
    Friend WithEvents dgvList As tCnt_DataGridView_turn
End Class
