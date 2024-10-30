<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ucCoil
    Inherits System.Windows.Forms.UserControl

    'UserControl はコンポーネント一覧をクリーンアップするために dispose をオーバーライドします。
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
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.TCnt_DataGridView1 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.TCnt_DataGridView2 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.TableLayoutPanel1.SuspendLayout()
        CType(Me.TCnt_DataGridView1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.TCnt_DataGridView2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 2
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 262.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.TCnt_DataGridView1, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.TCnt_DataGridView2, 1, 0)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel1.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 1
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(1276, 72)
        Me.TableLayoutPanel1.TabIndex = 0
        '
        'TCnt_DataGridView1
        '
        Me.TCnt_DataGridView1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TCnt_DataGridView1.Location = New System.Drawing.Point(1, 1)
        Me.TCnt_DataGridView1.Margin = New System.Windows.Forms.Padding(1, 1, 0, 1)
        Me.TCnt_DataGridView1.Name = "TCnt_DataGridView1"
        Me.TCnt_DataGridView1.RowTemplate.Height = 21
        Me.TCnt_DataGridView1.Size = New System.Drawing.Size(1013, 70)
        Me.TCnt_DataGridView1.TabIndex = 0
        '
        'TCnt_DataGridView2
        '
        Me.TCnt_DataGridView2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TCnt_DataGridView2.Location = New System.Drawing.Point(1015, 1)
        Me.TCnt_DataGridView2.Margin = New System.Windows.Forms.Padding(1, 1, 0, 1)
        Me.TCnt_DataGridView2.Name = "TCnt_DataGridView2"
        Me.TCnt_DataGridView2.RowTemplate.Height = 21
        Me.TCnt_DataGridView2.Size = New System.Drawing.Size(261, 70)
        Me.TCnt_DataGridView2.TabIndex = 1
        '
        'ucCoil
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Name = "ucCoil"
        Me.Size = New System.Drawing.Size(1276, 72)
        Me.TableLayoutPanel1.ResumeLayout(False)
        CType(Me.TCnt_DataGridView1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.TCnt_DataGridView2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents TCnt_DataGridView1 As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents TCnt_DataGridView2 As tClassLibrary.tCnt.tCnt_DataGridView
End Class
