<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmToCameraTbl
    Inherits System.Windows.Forms.Form

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
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
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.cmsHyDefectInf = New System.Windows.Forms.ContextMenuStrip(Me.components)
        Me.dspHyDefectInf = New System.Windows.Forms.ToolStripMenuItem()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.dgvCamInf = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.txtTitle = New System.Windows.Forms.Label()
        Me.cmsHyDefectInf.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        CType(Me.dgvCamInf, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
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
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(491, 90)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 20
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(491, 63)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 21
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dgvCamInf)
        Me.GroupBox2.Location = New System.Drawing.Point(16, 128)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(388, 242)
        Me.GroupBox2.TabIndex = 18
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "カメラ設定情報"
        '
        'dgvCamInf
        '
        Me.dgvCamInf.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCamInf.Location = New System.Drawing.Point(8, 19)
        Me.dgvCamInf.Name = "dgvCamInf"
        Me.dgvCamInf.RowTemplate.Height = 21
        Me.dgvCamInf.ScrollBars = System.Windows.Forms.ScrollBars.None
        Me.dgvCamInf.Size = New System.Drawing.Size(363, 210)
        Me.dgvCamInf.TabIndex = 19
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(491, 35)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 45
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.Color.White
        Me.txtInfo.Location = New System.Drawing.Point(16, 35)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(459, 78)
        Me.txtInfo.TabIndex = 46
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(12, 9)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(355, 21)
        Me.txtTitle.TabIndex = 47
        Me.txtTitle.Text = "カメラコントローラ情報テーブル"
        '
        'frmToCameraTbl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(7.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(623, 378)
        Me.Controls.Add(Me.txtTitle)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.GroupBox2)
        Me.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MaximizeBox = False
        Me.Name = "frmToCameraTbl"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "カメラコントローラ情報 [TBL_TO_CAMERA]"
        Me.cmsHyDefectInf.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        CType(Me.dgvCamInf, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents dgvCamInf As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents cmsHyDefectInf As System.Windows.Forms.ContextMenuStrip
    Friend WithEvents dspHyDefectInf As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents txtTitle As Label
End Class
