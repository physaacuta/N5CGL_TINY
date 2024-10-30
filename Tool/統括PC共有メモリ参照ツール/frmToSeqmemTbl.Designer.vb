<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmToSeqmemTbl
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
        Me.Label3 = New System.Windows.Forms.Label()
        Me.dgvSeqRecv = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvSeqSend = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label1 = New System.Windows.Forms.Label()
        CType(Me.dgvSeqRecv, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvSeqSend, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'chkUpdate
        '
        Me.chkUpdate.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkUpdate.Location = New System.Drawing.Point(664, 4)
        Me.chkUpdate.Name = "chkUpdate"
        Me.chkUpdate.Size = New System.Drawing.Size(120, 24)
        Me.chkUpdate.TabIndex = 30
        Me.chkUpdate.Text = "自動更新"
        Me.chkUpdate.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.chkUpdate.UseVisualStyleBackColor = True
        '
        'btnWrite
        '
        Me.btnWrite.Location = New System.Drawing.Point(664, 60)
        Me.btnWrite.Name = "btnWrite"
        Me.btnWrite.Size = New System.Drawing.Size(120, 24)
        Me.btnWrite.TabIndex = 29
        Me.btnWrite.Text = "書き込み"
        Me.btnWrite.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Location = New System.Drawing.Point(664, 32)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(120, 24)
        Me.btnRead.TabIndex = 28
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'txtTitle
        '
        Me.txtTitle.AutoSize = True
        Me.txtTitle.Font = New System.Drawing.Font("ＭＳ ゴシック", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtTitle.Location = New System.Drawing.Point(4, 4)
        Me.txtTitle.Name = "txtTitle"
        Me.txtTitle.Size = New System.Drawing.Size(263, 21)
        Me.txtTitle.TabIndex = 31
        Me.txtTitle.Text = "シーケンサ情報テーブル"
        '
        'txtInfo
        '
        Me.txtInfo.BackColor = System.Drawing.Color.White
        Me.txtInfo.Location = New System.Drawing.Point(276, 0)
        Me.txtInfo.Multiline = True
        Me.txtInfo.Name = "txtInfo"
        Me.txtInfo.ReadOnly = True
        Me.txtInfo.Size = New System.Drawing.Size(380, 88)
        Me.txtInfo.TabIndex = 32
        '
        'Label3
        '
        Me.Label3.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(10, 95)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(93, 16)
        Me.Label3.TabIndex = 33
        Me.Label3.Text = "受信エリア"
        '
        'dgvSeqRecv
        '
        Me.dgvSeqRecv.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.dgvSeqRecv.Location = New System.Drawing.Point(6, 114)
        Me.dgvSeqRecv.Name = "dgvSeqRecv"
        Me.dgvSeqRecv.RowTemplate.Height = 21
        Me.dgvSeqRecv.Size = New System.Drawing.Size(544, 610)
        Me.dgvSeqRecv.TabIndex = 34
        '
        'dgvSeqSend
        '
        Me.dgvSeqSend.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.dgvSeqSend.Location = New System.Drawing.Point(562, 114)
        Me.dgvSeqSend.Name = "dgvSeqSend"
        Me.dgvSeqSend.RowTemplate.Height = 21
        Me.dgvSeqSend.Size = New System.Drawing.Size(544, 610)
        Me.dgvSeqSend.TabIndex = 35
        '
        'Label1
        '
        Me.Label1.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(563, 95)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(93, 16)
        Me.Label1.TabIndex = 36
        Me.Label1.Text = "送信エリア"
        '
        'frmToSeqmemTbl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1113, 740)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.dgvSeqSend)
        Me.Controls.Add(Me.dgvSeqRecv)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.txtInfo)
        Me.Controls.Add(Me.txtTitle)
        Me.Controls.Add(Me.chkUpdate)
        Me.Controls.Add(Me.btnWrite)
        Me.Controls.Add(Me.btnRead)
        Me.Name = "frmToSeqmemTbl"
        Me.Text = "シーケンサ情報テーブル [TO_SEQMEM_TB]"
        CType(Me.dgvSeqRecv, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvSeqSend, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents chkUpdate As System.Windows.Forms.CheckBox
    Friend WithEvents btnWrite As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents txtTitle As System.Windows.Forms.Label
    Friend WithEvents txtInfo As System.Windows.Forms.TextBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents dgvSeqRecv As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvSeqSend As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label1 As System.Windows.Forms.Label
End Class
