<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ucStatus
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
        Me.lblMsgGuaid = New System.Windows.Forms.Label()
        Me.lblKadou = New System.Windows.Forms.Label()
        Me.Label103 = New System.Windows.Forms.Label()
        Me.lblKensa = New System.Windows.Forms.Label()
        Me.lblKiki = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.TableLayoutPanel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'lblMsgGuaid
        '
        Me.lblMsgGuaid.AutoEllipsis = True
        Me.lblMsgGuaid.BackColor = System.Drawing.SystemColors.Control
        Me.lblMsgGuaid.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMsgGuaid.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblMsgGuaid.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMsgGuaid.ForeColor = System.Drawing.Color.Black
        Me.lblMsgGuaid.Location = New System.Drawing.Point(3, 0)
        Me.lblMsgGuaid.Name = "lblMsgGuaid"
        Me.lblMsgGuaid.Size = New System.Drawing.Size(850, 32)
        Me.lblMsgGuaid.TabIndex = 49
        Me.lblMsgGuaid.Text = "照明装置 表1異常"
        Me.lblMsgGuaid.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblKadou
        '
        Me.lblKadou.BackColor = System.Drawing.Color.Blue
        Me.lblKadou.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKadou.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblKadou.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKadou.ForeColor = System.Drawing.Color.Yellow
        Me.lblKadou.Location = New System.Drawing.Point(896, 0)
        Me.lblKadou.Margin = New System.Windows.Forms.Padding(0)
        Me.lblKadou.Name = "lblKadou"
        Me.lblKadou.Size = New System.Drawing.Size(100, 32)
        Me.lblKadou.TabIndex = 55
        Me.lblKadou.Text = "稼動中"
        Me.lblKadou.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label103
        '
        Me.Label103.BackColor = System.Drawing.SystemColors.Control
        Me.Label103.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Label103.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label103.ForeColor = System.Drawing.Color.Black
        Me.Label103.Location = New System.Drawing.Point(856, 0)
        Me.Label103.Margin = New System.Windows.Forms.Padding(0)
        Me.Label103.Name = "Label103"
        Me.Label103.Size = New System.Drawing.Size(40, 32)
        Me.Label103.TabIndex = 48
        Me.Label103.Text = "稼動" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "状態"
        Me.Label103.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblKensa
        '
        Me.lblKensa.BackColor = System.Drawing.Color.Magenta
        Me.lblKensa.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKensa.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblKensa.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKensa.ForeColor = System.Drawing.Color.Yellow
        Me.lblKensa.Location = New System.Drawing.Point(1176, 0)
        Me.lblKensa.Margin = New System.Windows.Forms.Padding(0)
        Me.lblKensa.Name = "lblKensa"
        Me.lblKensa.Size = New System.Drawing.Size(100, 32)
        Me.lblKensa.TabIndex = 52
        Me.lblKensa.Text = "縮退"
        Me.lblKensa.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblKiki
        '
        Me.lblKiki.BackColor = System.Drawing.Color.Red
        Me.lblKiki.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKiki.Dock = System.Windows.Forms.DockStyle.Fill
        Me.lblKiki.Font = New System.Drawing.Font("ＭＳ 明朝", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKiki.ForeColor = System.Drawing.Color.Yellow
        Me.lblKiki.Location = New System.Drawing.Point(1036, 0)
        Me.lblKiki.Margin = New System.Windows.Forms.Padding(0)
        Me.lblKiki.Name = "lblKiki"
        Me.lblKiki.Size = New System.Drawing.Size(100, 32)
        Me.lblKiki.TabIndex = 53
        Me.lblKiki.Text = "重故障"
        Me.lblKiki.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label4
        '
        Me.Label4.BackColor = System.Drawing.SystemColors.Control
        Me.Label4.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.Color.Black
        Me.Label4.Location = New System.Drawing.Point(996, 0)
        Me.Label4.Margin = New System.Windows.Forms.Padding(0)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(40, 32)
        Me.Label4.TabIndex = 54
        Me.Label4.Text = "機器" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "状態"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label6
        '
        Me.Label6.BackColor = System.Drawing.SystemColors.Control
        Me.Label6.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.ForeColor = System.Drawing.Color.Black
        Me.Label6.Location = New System.Drawing.Point(1136, 0)
        Me.Label6.Margin = New System.Windows.Forms.Padding(0)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(40, 32)
        Me.Label6.TabIndex = 51
        Me.Label6.Text = "検査" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "状態"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 7
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 40.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.lblKensa, 6, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.lblKadou, 2, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.Label6, 5, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.lblKiki, 4, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.lblMsgGuaid, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.Label4, 3, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.Label103, 1, 0)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 1
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(1276, 32)
        Me.TableLayoutPanel1.TabIndex = 87
        '
        'ucStatus
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Name = "ucStatus"
        Me.Size = New System.Drawing.Size(1276, 32)
        Me.TableLayoutPanel1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label103 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Protected Friend WithEvents lblMsgGuaid As System.Windows.Forms.Label
    Protected Friend WithEvents lblKadou As System.Windows.Forms.Label
    Protected Friend WithEvents lblKensa As System.Windows.Forms.Label
    Protected Friend WithEvents lblKiki As System.Windows.Forms.Label
    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
End Class
