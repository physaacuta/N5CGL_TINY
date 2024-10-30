<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyCoilInf
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
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.txtTitle = New System.Windows.Forms.Label()
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbRecord = New System.Windows.Forms.ComboBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.dgvCoInfobase = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.dgvCoilBaseData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvProCoilInf = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label5 = New System.Windows.Forms.Label()
        CType(Me.dgvCoInfobase, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvCoilBaseData, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvProCoilInf, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(880, 4)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 32
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(880, 60)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 31
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(880, 32)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 30
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(0, 8)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(217, 21)
        Me.txtTitle.TabIndex = 29
        Me.txtTitle.Text = "コイル情報テーブル"
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.txtInfo.Location = New System.Drawing.Point(300, 4)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(568, 104)
        Me.txtInfo.TabIndex = 0
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(8, 40)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(76, 16)
        Me.Label1.TabIndex = 34
        Me.Label1.Text = "レコード"
        '
        'cmbRecord
        '
        Me.cmbRecord.FormattingEnabled = True
        Me.cmbRecord.Location = New System.Drawing.Point(156, 36)
        Me.cmbRecord.Name = "cmbRecord"
        Me.cmbRecord.Size = New System.Drawing.Size(121, 20)
        Me.cmbRecord.TabIndex = 33
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(4, 112)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(244, 16)
        Me.Label3.TabIndex = 36
        Me.Label3.Text = "コイル共通情報(COINFO_BASE)"
        '
        'dgvCoInfobase
        '
        Me.dgvCoInfobase.Location = New System.Drawing.Point(8, 132)
        Me.dgvCoInfobase.Name = "dgvCoInfobase"
        Me.dgvCoInfobase.RowTemplate.Height = 21
        Me.dgvCoInfobase.Size = New System.Drawing.Size(328, 171)
        Me.dgvCoInfobase.TabIndex = 35
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(8, 319)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(330, 16)
        Me.Label4.TabIndex = 38
        Me.Label4.Text = "システム共通コイル情報(COIL_BASEDATA)"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'dgvCoilBaseData
        '
        Me.dgvCoilBaseData.Location = New System.Drawing.Point(8, 340)
        Me.dgvCoilBaseData.Name = "dgvCoilBaseData"
        Me.dgvCoilBaseData.RowTemplate.Height = 21
        Me.dgvCoilBaseData.Size = New System.Drawing.Size(328, 276)
        Me.dgvCoilBaseData.TabIndex = 37
        '
        'dgvProCoilInf
        '
        Me.dgvProCoilInf.Location = New System.Drawing.Point(340, 132)
        Me.dgvProCoilInf.Name = "dgvProCoilInf"
        Me.dgvProCoilInf.RowTemplate.Height = 21
        Me.dgvProCoilInf.Size = New System.Drawing.Size(328, 484)
        Me.dgvProCoilInf.TabIndex = 39
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(344, 112)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(300, 16)
        Me.Label5.TabIndex = 40
        Me.Label5.Text = "コイル情報(PRO_TYPE_DATA_COILINF)"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'frmHyCoilInf
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1010, 630)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.dgvProCoilInf)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.dgvCoilBaseData)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.dgvCoInfobase)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbRecord)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.txtTitle)
        Me.Controls.Add(Me.txtInfo)
        Me.Name = "frmHyCoilInf"
        Me.Text = "コイル情報テーブル [TBL_HY_COILINF]"
        CType(Me.dgvCoInfobase, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvCoilBaseData, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvProCoilInf, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents txtTitle As System.Windows.Forms.Label
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents cmbRecord As System.Windows.Forms.ComboBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents dgvCoInfobase As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents dgvCoilBaseData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvProCoilInf As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label5 As System.Windows.Forms.Label
End Class
