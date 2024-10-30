<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmToCoilTbl
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
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.cmbRecord = New System.Windows.Forms.ComboBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.txtcID = New System.Windows.Forms.TextBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.txtTitle = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.dgvVal = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvCoilBaseData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvCoilSetting = New tClassLibrary.tCnt.tCnt_DataGridView()
        CType(Me.dgvVal, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvCoilBaseData, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvCoilSetting, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.Color.White
        Me.txtInfo.Location = New System.Drawing.Point(320, 4)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(728, 104)
        Me.txtInfo.TabIndex = 0
        '
        'cmbRecord
        '
        Me.cmbRecord.FormattingEnabled = True
        Me.cmbRecord.Location = New System.Drawing.Point(168, 56)
        Me.cmbRecord.Name = "cmbRecord"
        Me.cmbRecord.Size = New System.Drawing.Size(121, 20)
        Me.cmbRecord.TabIndex = 4
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(20, 60)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(76, 16)
        Me.Label1.TabIndex = 5
        Me.Label1.Text = "レコード"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(20, 88)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(76, 16)
        Me.Label2.TabIndex = 6
        Me.Label2.Text = "キー情報"
        '
        'txtcID
        '
        Me.txtcID.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtcID.Location = New System.Drawing.Point(96, 84)
        Me.txtcID.Name = "txtcID"
        Me.txtcID.Size = New System.Drawing.Size(192, 23)
        Me.txtcID.TabIndex = 7
        Me.txtcID.Text = "12345678901234567890"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(4, 128)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(253, 16)
        Me.Label3.TabIndex = 8
        Me.Label3.Text = "コイル基本設定(COIL_SETTING)"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(400, 128)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(262, 16)
        Me.Label4.TabIndex = 8
        Me.Label4.Text = "コイル基本情報(COIL_BASEDATA)"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(4, 8)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(309, 21)
        Me.txtTitle.TabIndex = 9
        Me.txtTitle.Text = "コイル情報受け渡しテーブル"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(788, 128)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(110, 16)
        Me.Label6.TabIndex = 11
        Me.Label6.Text = "受信データ部"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(1060, 36)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 25
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(1060, 64)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 26
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(1060, 8)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 27
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'dgvVal
        '
        Me.dgvVal.Location = New System.Drawing.Point(784, 148)
        Me.dgvVal.Name = "dgvVal"
        Me.dgvVal.RowTemplate.Height = 21
        Me.dgvVal.Size = New System.Drawing.Size(400, 440)
        Me.dgvVal.TabIndex = 10
        '
        'dgvCoilBaseData
        '
        Me.dgvCoilBaseData.Location = New System.Drawing.Point(396, 148)
        Me.dgvCoilBaseData.Name = "dgvCoilBaseData"
        Me.dgvCoilBaseData.RowTemplate.Height = 21
        Me.dgvCoilBaseData.Size = New System.Drawing.Size(384, 440)
        Me.dgvCoilBaseData.TabIndex = 2
        '
        'dgvCoilSetting
        '
        Me.dgvCoilSetting.Location = New System.Drawing.Point(8, 148)
        Me.dgvCoilSetting.Name = "dgvCoilSetting"
        Me.dgvCoilSetting.RowTemplate.Height = 21
        Me.dgvCoilSetting.Size = New System.Drawing.Size(384, 440)
        Me.dgvCoilSetting.TabIndex = 1
        '
        'frmToCoilTbl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1191, 598)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.dgvVal)
        Me.Controls.Add(Me.txtTitle)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.txtcID)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbRecord)
        Me.Controls.Add(Me.dgvCoilBaseData)
        Me.Controls.Add(Me.dgvCoilSetting)
        Me.Controls.Add(Me.txtInfo)
        Me.Name = "frmToCoilTbl"
        Me.Text = "コイル情報受け渡しテーブル"
        CType(Me.dgvVal, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvCoilBaseData, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvCoilSetting, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
	Friend WithEvents dgvCoilSetting As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents dgvCoilBaseData As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents cmbRecord As System.Windows.Forms.ComboBox
	Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents txtcID As System.Windows.Forms.TextBox
	Friend WithEvents Label3 As System.Windows.Forms.Label
	Friend WithEvents Label4 As System.Windows.Forms.Label
	Friend WithEvents txtTitle As System.Windows.Forms.Label
	Friend WithEvents dgvVal As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents Label6 As System.Windows.Forms.Label
	Friend WithEvents btnRead As System.Windows.Forms.Button
	Friend WithEvents btnWrite As System.Windows.Forms.Button
	Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
End Class
