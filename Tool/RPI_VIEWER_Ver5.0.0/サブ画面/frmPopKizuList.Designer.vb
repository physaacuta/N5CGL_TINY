<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopKizuList
    Inherits PopBaseForm

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
        Me.dgvDefect = New tClassLibrary.tCnt.tCnt_DataGridView
        Me.dgvAttr = New tClassLibrary.tCnt.tCnt_DataGridView
        Me.clbType = New System.Windows.Forms.CheckedListBox
        Me.clbGr = New System.Windows.Forms.CheckedListBox
        Me.btnTypeAllOk = New System.Windows.Forms.Button
        Me.btnTypeAllNg = New System.Windows.Forms.Button
        Me.btnGrAllOk = New System.Windows.Forms.Button
        Me.btnGrAllNg = New System.Windows.Forms.Button
        Me.btnExec = New System.Windows.Forms.Button
        Me.btnCsvKizu = New System.Windows.Forms.Button
        Me.btnCsvAttr = New System.Windows.Forms.Button
        CType(Me.dgvDefect, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvAttr, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'dgvDefect
        '
        Me.dgvDefect.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.dgvDefect.Location = New System.Drawing.Point(0, 136)
        Me.dgvDefect.Name = "dgvDefect"
        Me.dgvDefect.RowTemplate.Height = 21
        Me.dgvDefect.Size = New System.Drawing.Size(388, 377)
        Me.dgvDefect.TabIndex = 2
        '
        'dgvAttr
        '
        Me.dgvAttr.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvAttr.Location = New System.Drawing.Point(392, 136)
        Me.dgvAttr.Name = "dgvAttr"
        Me.dgvAttr.RowTemplate.Height = 21
        Me.dgvAttr.Size = New System.Drawing.Size(396, 377)
        Me.dgvAttr.TabIndex = 3
        '
        'clbType
        '
        Me.clbType.CheckOnClick = True
        Me.clbType.FormattingEnabled = True
        Me.clbType.Location = New System.Drawing.Point(8, 0)
        Me.clbType.Name = "clbType"
        Me.clbType.Size = New System.Drawing.Size(212, 130)
        Me.clbType.TabIndex = 4
        '
        'clbGr
        '
        Me.clbGr.CheckOnClick = True
        Me.clbGr.FormattingEnabled = True
        Me.clbGr.Location = New System.Drawing.Point(284, 0)
        Me.clbGr.Name = "clbGr"
        Me.clbGr.Size = New System.Drawing.Size(212, 130)
        Me.clbGr.TabIndex = 4
        '
        'btnTypeAllOk
        '
        Me.btnTypeAllOk.Location = New System.Drawing.Point(224, 68)
        Me.btnTypeAllOk.Name = "btnTypeAllOk"
        Me.btnTypeAllOk.Size = New System.Drawing.Size(56, 28)
        Me.btnTypeAllOk.TabIndex = 5
        Me.btnTypeAllOk.Text = "全選択"
        Me.btnTypeAllOk.UseVisualStyleBackColor = True
        '
        'btnTypeAllNg
        '
        Me.btnTypeAllNg.Location = New System.Drawing.Point(224, 100)
        Me.btnTypeAllNg.Name = "btnTypeAllNg"
        Me.btnTypeAllNg.Size = New System.Drawing.Size(56, 28)
        Me.btnTypeAllNg.TabIndex = 5
        Me.btnTypeAllNg.Text = "全解除"
        Me.btnTypeAllNg.UseVisualStyleBackColor = True
        '
        'btnGrAllOk
        '
        Me.btnGrAllOk.Location = New System.Drawing.Point(500, 72)
        Me.btnGrAllOk.Name = "btnGrAllOk"
        Me.btnGrAllOk.Size = New System.Drawing.Size(56, 28)
        Me.btnGrAllOk.TabIndex = 5
        Me.btnGrAllOk.Text = "全選択"
        Me.btnGrAllOk.UseVisualStyleBackColor = True
        '
        'btnGrAllNg
        '
        Me.btnGrAllNg.Location = New System.Drawing.Point(500, 100)
        Me.btnGrAllNg.Name = "btnGrAllNg"
        Me.btnGrAllNg.Size = New System.Drawing.Size(56, 28)
        Me.btnGrAllNg.TabIndex = 5
        Me.btnGrAllNg.Text = "全解除"
        Me.btnGrAllNg.UseVisualStyleBackColor = True
        '
        'btnExec
        '
        Me.btnExec.Location = New System.Drawing.Point(500, 0)
        Me.btnExec.Name = "btnExec"
        Me.btnExec.Size = New System.Drawing.Size(56, 56)
        Me.btnExec.TabIndex = 5
        Me.btnExec.Text = "反映"
        Me.btnExec.UseVisualStyleBackColor = True
        '
        'btnCsvKizu
        '
        Me.btnCsvKizu.Location = New System.Drawing.Point(640, 100)
        Me.btnCsvKizu.Name = "btnCsvKizu"
        Me.btnCsvKizu.Size = New System.Drawing.Size(56, 32)
        Me.btnCsvKizu.TabIndex = 5
        Me.btnCsvKizu.Text = "疵一覧" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "CSV出力"
        Me.btnCsvKizu.UseVisualStyleBackColor = True
        '
        'btnCsvAttr
        '
        Me.btnCsvAttr.Location = New System.Drawing.Point(700, 100)
        Me.btnCsvAttr.Name = "btnCsvAttr"
        Me.btnCsvAttr.Size = New System.Drawing.Size(56, 32)
        Me.btnCsvAttr.TabIndex = 5
        Me.btnCsvAttr.Text = "特徴量" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "CSV出力"
        Me.btnCsvAttr.UseVisualStyleBackColor = True
        '
        'frmPopKizuList
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(789, 512)
        Me.Controls.Add(Me.btnTypeAllNg)
        Me.Controls.Add(Me.btnGrAllNg)
        Me.Controls.Add(Me.btnExec)
        Me.Controls.Add(Me.btnCsvAttr)
        Me.Controls.Add(Me.btnCsvKizu)
        Me.Controls.Add(Me.btnGrAllOk)
        Me.Controls.Add(Me.btnTypeAllOk)
        Me.Controls.Add(Me.clbGr)
        Me.Controls.Add(Me.clbType)
        Me.Controls.Add(Me.dgvAttr)
        Me.Controls.Add(Me.dgvDefect)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopKizuList"
        Me.Text = "疵一覧"
        CType(Me.dgvDefect, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvAttr, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvDefect As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvAttr As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents clbType As System.Windows.Forms.CheckedListBox
    Friend WithEvents clbGr As System.Windows.Forms.CheckedListBox
    Friend WithEvents btnTypeAllOk As System.Windows.Forms.Button
    Friend WithEvents btnTypeAllNg As System.Windows.Forms.Button
    Friend WithEvents btnGrAllOk As System.Windows.Forms.Button
    Friend WithEvents btnGrAllNg As System.Windows.Forms.Button
    Friend WithEvents btnExec As System.Windows.Forms.Button
    Friend WithEvents btnCsvKizu As System.Windows.Forms.Button
    Friend WithEvents btnCsvAttr As System.Windows.Forms.Button
End Class
