<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyMap
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
        Me.components = New System.ComponentModel.Container()
        Me.cmbRecord = New System.Windows.Forms.ComboBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.cmbMen = New System.Windows.Forms.ComboBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.cmbStage = New System.Windows.Forms.ComboBox()
        Me.cmsHyDefectInf = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.dspHyDefectInf = New System.Windows.Forms.ToolStripMenuItem()
        Me.btnWriteHyMapRow = New System.Windows.Forms.Button()
        Me.btnReadHyMapRow = New System.Windows.Forms.Button()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.dgvHyMapRow = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.cmsHyDefectInf.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        CType(Me.dgvHyMapRow, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'cmbRecord
        '
        Me.cmbRecord.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbRecord.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbRecord.FormattingEnabled = True
        Me.cmbRecord.Location = New System.Drawing.Point(108, 4)
        Me.cmbRecord.Name = "cmbRecord"
        Me.cmbRecord.Size = New System.Drawing.Size(120, 24)
        Me.cmbRecord.TabIndex = 2
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(8, 4)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(100, 24)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "レコード"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(8, 60)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(100, 24)
        Me.Label2.TabIndex = 5
        Me.Label2.Text = "表裏"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbMen
        '
        Me.cmbMen.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbMen.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbMen.FormattingEnabled = True
        Me.cmbMen.Location = New System.Drawing.Point(108, 60)
        Me.cmbMen.Name = "cmbMen"
        Me.cmbMen.Size = New System.Drawing.Size(120, 24)
        Me.cmbMen.TabIndex = 6
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(8, 32)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(100, 24)
        Me.Label3.TabIndex = 3
        Me.Label3.Text = "マップ切替"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbStage
        '
        Me.cmbStage.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbStage.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbStage.FormattingEnabled = True
        Me.cmbStage.Location = New System.Drawing.Point(108, 32)
        Me.cmbStage.Name = "cmbStage"
        Me.cmbStage.Size = New System.Drawing.Size(120, 24)
        Me.cmbStage.TabIndex = 4
        '
        'cmsHyDefectInf
        '
        Me.cmsHyDefectInf.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmsHyDefectInf.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.dspHyDefectInf})
        Me.cmsHyDefectInf.Name = "cmsKizuInf"
        Me.cmsHyDefectInf.Size = New System.Drawing.Size(131, 26)
        '
        'dspHyDefectInf
        '
        Me.dspHyDefectInf.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.dspHyDefectInf.Name = "dspHyDefectInf"
        Me.dspHyDefectInf.Size = New System.Drawing.Size(130, 22)
        Me.dspHyDefectInf.Text = "疵情報表示"
        '
        'btnWriteHyMapRow
        '
        Me.btnWriteHyMapRow.Location = New System.Drawing.Point(952, 59)
        Me.btnWriteHyMapRow.Name = "btnWriteHyMapRow"
        Me.btnWriteHyMapRow.Size = New System.Drawing.Size(120, 24)
        Me.btnWriteHyMapRow.TabIndex = 20
        Me.btnWriteHyMapRow.Text = "書き込み"
        Me.btnWriteHyMapRow.UseVisualStyleBackColor = True
        '
        'btnReadHyMapRow
        '
        Me.btnReadHyMapRow.Location = New System.Drawing.Point(952, 32)
        Me.btnReadHyMapRow.Name = "btnReadHyMapRow"
        Me.btnReadHyMapRow.Size = New System.Drawing.Size(120, 24)
        Me.btnReadHyMapRow.TabIndex = 21
        Me.btnReadHyMapRow.Text = "読み込み"
        Me.btnReadHyMapRow.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dgvHyMapRow)
        Me.GroupBox2.Location = New System.Drawing.Point(8, 116)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(1068, 429)
        Me.GroupBox2.TabIndex = 18
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "マップ位置"
        '
        'dgvHyMapRow
        '
        Me.dgvHyMapRow.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvHyMapRow.Location = New System.Drawing.Point(8, 20)
        Me.dgvHyMapRow.Name = "dgvHyMapRow"
        Me.dgvHyMapRow.RowTemplate.Height = 21
        Me.dgvHyMapRow.ScrollBars = System.Windows.Forms.ScrollBars.None
        Me.dgvHyMapRow.Size = New System.Drawing.Size(1056, 400)
        Me.dgvHyMapRow.TabIndex = 19
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(952, 4)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 45
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.txtInfo.Location = New System.Drawing.Point(236, 4)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(704, 107)
        Me.txtInfo.TabIndex = 46
        '
        'frmHyMap
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(7.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1084, 547)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.btnWriteHyMapRow)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbStage)
        Me.Controls.Add(Me.btnReadHyMapRow)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.cmbMen)
        Me.Controls.Add(Me.cmbRecord)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label3)
        Me.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "frmHyMap"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "マップ位置 [TBL_HY_MAP_ROW]"
        Me.cmsHyDefectInf.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        CType(Me.dgvHyMapRow, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents cmbRecord As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents cmbMen As System.Windows.Forms.ComboBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents cmbStage As System.Windows.Forms.ComboBox
    Friend WithEvents dgvHyMapRow As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents btnWriteHyMapRow As System.Windows.Forms.Button
    Friend WithEvents btnReadHyMapRow As System.Windows.Forms.Button
    Friend WithEvents cmsHyDefectInf As System.Windows.Forms.ContextMenuStrip
    Friend WithEvents dspHyDefectInf As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
End Class
