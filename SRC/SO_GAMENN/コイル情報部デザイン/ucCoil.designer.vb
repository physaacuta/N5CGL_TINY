<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class ucCoil
    Inherits System.Windows.Forms.UserControl

    'UserControl はコンポーネント一覧をクリーンアップするために dispose をオーバーライドします。
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
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.TCnt_DataGridView1 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.pnl1 = New System.Windows.Forms.Panel()
        Me.lblKensa = New System.Windows.Forms.Label()
        Me.lblKiki = New System.Windows.Forms.Label()
        Me.lblKensaName = New System.Windows.Forms.Label()
        Me.lblKikiName = New System.Windows.Forms.Label()
        Me.TCnt_DataGridView2 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.TableLayoutPanel1.SuspendLayout()
        CType(Me.TCnt_DataGridView1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnl1.SuspendLayout()
        CType(Me.TCnt_DataGridView2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 3
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 282.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 118.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.TCnt_DataGridView1, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.pnl1, 2, 0)
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
        Me.TCnt_DataGridView1.Size = New System.Drawing.Size(875, 70)
        Me.TCnt_DataGridView1.TabIndex = 0
        '
        'pnl1
        '
        Me.pnl1.Controls.Add(Me.lblKensa)
        Me.pnl1.Controls.Add(Me.lblKiki)
        Me.pnl1.Controls.Add(Me.lblKensaName)
        Me.pnl1.Controls.Add(Me.lblKikiName)
        Me.pnl1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.pnl1.Location = New System.Drawing.Point(1158, 0)
        Me.pnl1.Margin = New System.Windows.Forms.Padding(0)
        Me.pnl1.Name = "pnl1"
        Me.pnl1.Size = New System.Drawing.Size(118, 72)
        Me.pnl1.TabIndex = 254
        '
        'lblKensa
        '
        Me.lblKensa.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblKensa.BackColor = System.Drawing.Color.Blue
        Me.lblKensa.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKensa.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKensa.ForeColor = System.Drawing.Color.Yellow
        Me.lblKensa.Location = New System.Drawing.Point(34, 34)
        Me.lblKensa.Name = "lblKensa"
        Me.lblKensa.Size = New System.Drawing.Size(84, 28)
        Me.lblKensa.TabIndex = 362
        Me.lblKensa.Text = "正常"
        Me.lblKensa.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblKiki
        '
        Me.lblKiki.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblKiki.BackColor = System.Drawing.Color.Orange
        Me.lblKiki.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKiki.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKiki.ForeColor = System.Drawing.Color.Black
        Me.lblKiki.Location = New System.Drawing.Point(34, 2)
        Me.lblKiki.Name = "lblKiki"
        Me.lblKiki.Size = New System.Drawing.Size(84, 28)
        Me.lblKiki.TabIndex = 361
        Me.lblKiki.Text = "軽故障"
        Me.lblKiki.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblKensaName
        '
        Me.lblKensaName.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblKensaName.AutoSize = True
        Me.lblKensaName.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKensaName.Location = New System.Drawing.Point(2, 36)
        Me.lblKensaName.Name = "lblKensaName"
        Me.lblKensaName.Size = New System.Drawing.Size(31, 24)
        Me.lblKensaName.TabIndex = 363
        Me.lblKensaName.Text = "検査" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "状態"
        '
        'lblKikiName
        '
        Me.lblKikiName.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblKikiName.AutoSize = True
        Me.lblKikiName.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKikiName.Location = New System.Drawing.Point(2, 4)
        Me.lblKikiName.Name = "lblKikiName"
        Me.lblKikiName.Size = New System.Drawing.Size(31, 24)
        Me.lblKikiName.TabIndex = 360
        Me.lblKikiName.Text = "機器" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "状態"
        '
        'TCnt_DataGridView2
        '
        Me.TCnt_DataGridView2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TCnt_DataGridView2.Location = New System.Drawing.Point(877, 1)
        Me.TCnt_DataGridView2.Margin = New System.Windows.Forms.Padding(1, 1, 0, 1)
        Me.TCnt_DataGridView2.Name = "TCnt_DataGridView2"
        Me.TCnt_DataGridView2.RowTemplate.Height = 21
        Me.TCnt_DataGridView2.Size = New System.Drawing.Size(281, 70)
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
        Me.pnl1.ResumeLayout(False)
        Me.pnl1.PerformLayout()
        CType(Me.TCnt_DataGridView2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents TCnt_DataGridView1 As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents TCnt_DataGridView2 As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents pnl1 As Panel
    Friend WithEvents lblKensa As Label
    Friend WithEvents lblKiki As Label
    Friend WithEvents lblKensaName As Label
    Friend WithEvents lblKikiName As Label
End Class
