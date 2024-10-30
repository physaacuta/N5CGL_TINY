<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopKizuMap
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
        Me.Panel1 = New System.Windows.Forms.Panel
        Me.btnReflect = New System.Windows.Forms.Button
        Me.nudInterval = New System.Windows.Forms.NumericUpDown
        Me.nudHabaCol = New System.Windows.Forms.NumericUpDown
        Me.Label3 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label1 = New System.Windows.Forms.Label
        Me.dgvMap = New tClassLibrary.tCnt.tCnt_DataGridView
        Me.Panel1.SuspendLayout()
        CType(Me.nudInterval, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudHabaCol, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvMap, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.btnReflect)
        Me.Panel1.Controls.Add(Me.nudInterval)
        Me.Panel1.Controls.Add(Me.nudHabaCol)
        Me.Panel1.Controls.Add(Me.Label3)
        Me.Panel1.Controls.Add(Me.Label2)
        Me.Panel1.Controls.Add(Me.Label1)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.Panel1.Location = New System.Drawing.Point(0, 630)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(322, 26)
        Me.Panel1.TabIndex = 0
        '
        'btnReflect
        '
        Me.btnReflect.Location = New System.Drawing.Point(276, 4)
        Me.btnReflect.Name = "btnReflect"
        Me.btnReflect.Size = New System.Drawing.Size(40, 20)
        Me.btnReflect.TabIndex = 2
        Me.btnReflect.Text = "反映"
        Me.btnReflect.UseVisualStyleBackColor = True
        '
        'nudInterval
        '
        Me.nudInterval.Location = New System.Drawing.Point(188, 4)
        Me.nudInterval.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudInterval.Name = "nudInterval"
        Me.nudInterval.Size = New System.Drawing.Size(52, 19)
        Me.nudInterval.TabIndex = 1
        Me.nudInterval.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudInterval.Value = New Decimal(New Integer() {2, 0, 0, 0})
        '
        'nudHabaCol
        '
        Me.nudHabaCol.Location = New System.Drawing.Point(60, 4)
        Me.nudHabaCol.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudHabaCol.Name = "nudHabaCol"
        Me.nudHabaCol.Size = New System.Drawing.Size(52, 19)
        Me.nudHabaCol.TabIndex = 1
        Me.nudHabaCol.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudHabaCol.Value = New Decimal(New Integer() {10, 0, 0, 0})
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(244, 8)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(22, 12)
        Me.Label3.TabIndex = 0
        Me.Label3.Text = "[m]"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(124, 8)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(58, 12)
        Me.Label2.TabIndex = 0
        Me.Label2.Text = "長手サイズ"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(4, 8)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(53, 12)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "幅分割数"
        '
        'dgvMap
        '
        Me.dgvMap.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvMap.Location = New System.Drawing.Point(0, 0)
        Me.dgvMap.Name = "dgvMap"
        Me.dgvMap.RowTemplate.Height = 21
        Me.dgvMap.Size = New System.Drawing.Size(322, 630)
        Me.dgvMap.TabIndex = 1
        '
        'frmPopKizuMap
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(322, 656)
        Me.Controls.Add(Me.dgvMap)
        Me.Controls.Add(Me.Panel1)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopKizuMap"
        Me.Text = "疵マップ"
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        CType(Me.nudInterval, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudHabaCol, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvMap, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Panel1 As System.Windows.Forms.Panel
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents nudHabaCol As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnReflect As System.Windows.Forms.Button
    Friend WithEvents nudInterval As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents dgvMap As tClassLibrary.tCnt.tCnt_DataGridView
End Class
