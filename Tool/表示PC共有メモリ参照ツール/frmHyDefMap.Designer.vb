<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmHyDefMap
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
        Me.Label4 = New System.Windows.Forms.Label()
        Me.dgvMapInf = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbRecord = New System.Windows.Forms.ComboBox()
        Me.chkUpdate = New System.Windows.Forms.CheckBox()
        Me.btnWrite = New System.Windows.Forms.Button()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.txtTitle = New System.Windows.Forms.Label()
        Me.txtInfo = New System.Windows.Forms.TextBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.txtRec = New System.Windows.Forms.TextBox()
        CType(Me.dgvMapInf, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(19, 208)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(110, 16)
        Me.Label4.TabIndex = 56
        Me.Label4.Text = "疵マップ情報"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'dgvMapInf
        '
        Me.dgvMapInf.Location = New System.Drawing.Point(15, 230)
        Me.dgvMapInf.Name = "dgvMapInf"
        Me.dgvMapInf.RowTemplate.Height = 21
        Me.dgvMapInf.Size = New System.Drawing.Size(303, 190)
        Me.dgvMapInf.TabIndex = 55
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(12, 148)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(76, 16)
        Me.Label1.TabIndex = 52
        Me.Label1.Text = "レコード"
        '
        'cmbRecord
        '
        Me.cmbRecord.FormattingEnabled = True
        Me.cmbRecord.Location = New System.Drawing.Point(100, 145)
        Me.cmbRecord.Name = "cmbRecord"
        Me.cmbRecord.Size = New System.Drawing.Size(121, 20)
        Me.cmbRecord.TabIndex = 51
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(322, 31)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 50
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(322, 87)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 49
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(322, 59)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 48
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(4, 7)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(286, 21)
        Me.txtTitle.TabIndex = 47
        Me.txtTitle.Text = "疵マップ表示情報テーブル"
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.txtInfo.Location = New System.Drawing.Point(4, 32)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(301, 104)
        Me.txtInfo.TabIndex = 46
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(15, 176)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(146, 18)
        Me.Label2.TabIndex = 58
        Me.Label2.Text = "最新レコード位置"
        '
        'txtRec
        '
        Me.txtRec.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtRec.Location = New System.Drawing.Point(163, 174)
        Me.txtRec.Name = "txtRec"
        Me.txtRec.Size = New System.Drawing.Size(80, 23)
        Me.txtRec.TabIndex = 57
        Me.txtRec.Text = "123456"
        Me.txtRec.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'frmHyDefMap
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(453, 428)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.txtRec)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.dgvMapInf)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbRecord)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.txtTitle)
        Me.Controls.Add(Me.txtInfo)
        Me.Name = "frmHyDefMap"
        Me.Text = "疵マップ表示情報[TBL_HY_DEFMAP]"
        CType(Me.dgvMapInf, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents dgvMapInf As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents cmbRecord As System.Windows.Forms.ComboBox
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents txtTitle As System.Windows.Forms.Label
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents Label2 As Label
    Friend WithEvents txtRec As TextBox
End Class
