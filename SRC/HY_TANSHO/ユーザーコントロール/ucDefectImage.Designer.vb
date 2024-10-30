<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ucDefectImage
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

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.picImg = New System.Windows.Forms.PictureBox()
        Me.TableLayoutPanel2 = New System.Windows.Forms.TableLayoutPanel()
        Me.lblImgSideL_H = New System.Windows.Forms.Label()
        Me.lblImgSideR_H = New System.Windows.Forms.Label()
        Me.TableLayoutPanel3 = New System.Windows.Forms.TableLayoutPanel()
        Me.lblTitleName_H = New System.Windows.Forms.Label()
        Me.lblImgSideR_V = New System.Windows.Forms.Label()
        Me.lblImgSideL_V = New System.Windows.Forms.Label()
        Me.lblTitleName_V = New System.Windows.Forms.Label()
        Me.TableLayoutPanel1.SuspendLayout()
        CType(Me.picImg, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TableLayoutPanel2.SuspendLayout()
        Me.TableLayoutPanel3.SuspendLayout()
        Me.SuspendLayout()
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel1.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.[Single]
        Me.TableLayoutPanel1.ColumnCount = 2
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.picImg, 1, 1)
        Me.TableLayoutPanel1.Controls.Add(Me.TableLayoutPanel2, 1, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.TableLayoutPanel3, 0, 1)
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel1.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 2
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(200, 400)
        Me.TableLayoutPanel1.TabIndex = 0
        '
        'picImg
        '
        Me.picImg.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.picImg.Dock = System.Windows.Forms.DockStyle.Fill
        Me.picImg.Location = New System.Drawing.Point(22, 22)
        Me.picImg.Margin = New System.Windows.Forms.Padding(0)
        Me.picImg.Name = "picImg"
        Me.picImg.Size = New System.Drawing.Size(177, 377)
        Me.picImg.TabIndex = 1
        Me.picImg.TabStop = False
        '
        'TableLayoutPanel2
        '
        Me.TableLayoutPanel2.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel2.BackColor = System.Drawing.Color.SkyBlue
        Me.TableLayoutPanel2.ColumnCount = 3
        Me.TableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
        Me.TableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
        Me.TableLayoutPanel2.Controls.Add(Me.lblTitleName_H, 0, 0)
        Me.TableLayoutPanel2.Controls.Add(Me.lblImgSideL_H, 0, 0)
        Me.TableLayoutPanel2.Controls.Add(Me.lblImgSideR_H, 2, 0)
        Me.TableLayoutPanel2.Location = New System.Drawing.Point(22, 1)
        Me.TableLayoutPanel2.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel2.Name = "TableLayoutPanel2"
        Me.TableLayoutPanel2.RowCount = 1
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel2.Size = New System.Drawing.Size(177, 20)
        Me.TableLayoutPanel2.TabIndex = 0
        '
        'lblImgSideL_H
        '
        Me.lblImgSideL_H.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.lblImgSideL_H.BackColor = System.Drawing.Color.Transparent
        Me.lblImgSideL_H.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblImgSideL_H.Location = New System.Drawing.Point(3, 0)
        Me.lblImgSideL_H.Name = "lblImgSideL_H"
        Me.lblImgSideL_H.Size = New System.Drawing.Size(26, 20)
        Me.lblImgSideL_H.TabIndex = 104
        Me.lblImgSideL_H.Text = "DS"
        Me.lblImgSideL_H.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblImgSideR_H
        '
        Me.lblImgSideR_H.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblImgSideR_H.BackColor = System.Drawing.Color.SkyBlue
        Me.lblImgSideR_H.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblImgSideR_H.Location = New System.Drawing.Point(148, 0)
        Me.lblImgSideR_H.Name = "lblImgSideR_H"
        Me.lblImgSideR_H.Size = New System.Drawing.Size(26, 20)
        Me.lblImgSideR_H.TabIndex = 105
        Me.lblImgSideR_H.Text = "WS"
        Me.lblImgSideR_H.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'TableLayoutPanel3
        '
        Me.TableLayoutPanel3.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel3.BackColor = System.Drawing.Color.SkyBlue
        Me.TableLayoutPanel3.ColumnCount = 1
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Controls.Add(Me.lblTitleName_V, 0, 1)
        Me.TableLayoutPanel3.Controls.Add(Me.lblImgSideL_V, 0, 2)
        Me.TableLayoutPanel3.Controls.Add(Me.lblImgSideR_V, 0, 0)
        Me.TableLayoutPanel3.Location = New System.Drawing.Point(1, 22)
        Me.TableLayoutPanel3.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel3.Name = "TableLayoutPanel3"
        Me.TableLayoutPanel3.RowCount = 3
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.TableLayoutPanel3.Size = New System.Drawing.Size(20, 377)
        Me.TableLayoutPanel3.TabIndex = 2
        '
        'lblTitleName_H
        '
        Me.lblTitleName_H.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblTitleName_H.BackColor = System.Drawing.Color.SkyBlue
        Me.lblTitleName_H.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTitleName_H.Location = New System.Drawing.Point(35, 0)
        Me.lblTitleName_H.Name = "lblTitleName_H"
        Me.lblTitleName_H.Size = New System.Drawing.Size(107, 20)
        Me.lblTitleName_H.TabIndex = 106
        Me.lblTitleName_H.Text = "画像"
        Me.lblTitleName_H.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblImgSideR_V
        '
        Me.lblImgSideR_V.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.lblImgSideR_V.BackColor = System.Drawing.Color.Transparent
        Me.lblImgSideR_V.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblImgSideR_V.Location = New System.Drawing.Point(3, 0)
        Me.lblImgSideR_V.Name = "lblImgSideR_V"
        Me.lblImgSideR_V.Size = New System.Drawing.Size(14, 32)
        Me.lblImgSideR_V.TabIndex = 105
        Me.lblImgSideR_V.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblImgSideL_V
        '
        Me.lblImgSideL_V.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblImgSideL_V.BackColor = System.Drawing.Color.SkyBlue
        Me.lblImgSideL_V.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblImgSideL_V.Location = New System.Drawing.Point(3, 345)
        Me.lblImgSideL_V.Name = "lblImgSideL_V"
        Me.lblImgSideL_V.Size = New System.Drawing.Size(14, 32)
        Me.lblImgSideL_V.TabIndex = 106
        Me.lblImgSideL_V.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblTitleName_V
        '
        Me.lblTitleName_V.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblTitleName_V.BackColor = System.Drawing.Color.SkyBlue
        Me.lblTitleName_V.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTitleName_V.Location = New System.Drawing.Point(3, 32)
        Me.lblTitleName_V.Name = "lblTitleName_V"
        Me.lblTitleName_V.Size = New System.Drawing.Size(14, 313)
        Me.lblTitleName_V.TabIndex = 107
        Me.lblTitleName_V.Text = "画像"
        Me.lblTitleName_V.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'ucDefectImage
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Name = "ucDefectImage"
        Me.Size = New System.Drawing.Size(200, 400)
        Me.TableLayoutPanel1.ResumeLayout(False)
        CType(Me.picImg, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TableLayoutPanel2.ResumeLayout(False)
        Me.TableLayoutPanel3.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents TableLayoutPanel2 As TableLayoutPanel
    Friend WithEvents lblImgSideL_H As Label
    Friend WithEvents lblImgSideR_H As Label
    Friend WithEvents picImg As PictureBox
    Friend WithEvents lblTitleName_H As Label
    Friend WithEvents TableLayoutPanel3 As TableLayoutPanel
    Friend WithEvents lblTitleName_V As Label
    Friend WithEvents lblImgSideL_V As Label
    Friend WithEvents lblImgSideR_V As Label
End Class
