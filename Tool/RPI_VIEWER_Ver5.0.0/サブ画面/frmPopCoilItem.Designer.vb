<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopCoilItem
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
        Me.dgvCoil = New tClassLibrary.tCnt.tCnt_DataGridView
        CType(Me.dgvCoil, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'dgvCoil
        '
        Me.dgvCoil.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvCoil.Location = New System.Drawing.Point(0, 0)
        Me.dgvCoil.Name = "dgvCoil"
        Me.dgvCoil.RowTemplate.Height = 21
        Me.dgvCoil.Size = New System.Drawing.Size(266, 543)
        Me.dgvCoil.TabIndex = 0
        '
        'frmPopCoilItem
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(266, 543)
        Me.Controls.Add(Me.dgvCoil)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopCoilItem"
        Me.Text = "コイル情報一覧"
        CType(Me.dgvCoil, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvCoil As tClassLibrary.tCnt.tCnt_DataGridView
End Class
