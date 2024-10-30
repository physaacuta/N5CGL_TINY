<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmCglDefList_Pattern01
    Inherits HY_DEFLIST.frmCglDefList_Base

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.tblLayoutPnl = New System.Windows.Forms.TableLayoutPanel()
        Me.UcDefect01_01 = New HY_DEFLIST.ucDefect01()
        Me.UcDefect01_02 = New HY_DEFLIST.ucDefect01()
        Me.UcDefect01_03 = New HY_DEFLIST.ucDefect01()
        Me.UcDefect01_04 = New HY_DEFLIST.ucDefect01()
        Me.UcDefect01_05 = New HY_DEFLIST.ucDefect01()
        Me.tblLayoutPnl.SuspendLayout()
        Me.SuspendLayout()
        '
        'tblLayoutPnl
        '
        Me.tblLayoutPnl.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tblLayoutPnl.ColumnCount = 5
        Me.tblLayoutPnl.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.tblLayoutPnl.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.tblLayoutPnl.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.tblLayoutPnl.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.tblLayoutPnl.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20.0!))
        Me.tblLayoutPnl.Controls.Add(Me.UcDefect01_01, 0, 0)
        Me.tblLayoutPnl.Controls.Add(Me.UcDefect01_02, 1, 0)
        Me.tblLayoutPnl.Controls.Add(Me.UcDefect01_03, 2, 0)
        Me.tblLayoutPnl.Controls.Add(Me.UcDefect01_04, 3, 0)
        Me.tblLayoutPnl.Controls.Add(Me.UcDefect01_05, 4, 0)
        Me.tblLayoutPnl.Location = New System.Drawing.Point(8, 8)
        Me.tblLayoutPnl.Name = "tblLayoutPnl"
        Me.tblLayoutPnl.RowCount = 1
        Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tblLayoutPnl.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.tblLayoutPnl.Size = New System.Drawing.Size(1256, 860)
        Me.tblLayoutPnl.TabIndex = 221
        '
        'UcDefect01_01
        '
        Me.UcDefect01_01.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcDefect01_01.Location = New System.Drawing.Point(0, 0)
        Me.UcDefect01_01.Margin = New System.Windows.Forms.Padding(0)
        Me.UcDefect01_01.Name = "UcDefect01_01"
        Me.UcDefect01_01.Size = New System.Drawing.Size(251, 860)
        Me.UcDefect01_01.TabIndex = 0
        '
        'UcDefect01_02
        '
        Me.UcDefect01_02.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcDefect01_02.Location = New System.Drawing.Point(251, 0)
        Me.UcDefect01_02.Margin = New System.Windows.Forms.Padding(0)
        Me.UcDefect01_02.Name = "UcDefect01_02"
        Me.UcDefect01_02.Size = New System.Drawing.Size(251, 860)
        Me.UcDefect01_02.TabIndex = 1
        '
        'UcDefect01_03
        '
        Me.UcDefect01_03.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcDefect01_03.Location = New System.Drawing.Point(502, 0)
        Me.UcDefect01_03.Margin = New System.Windows.Forms.Padding(0)
        Me.UcDefect01_03.Name = "UcDefect01_03"
        Me.UcDefect01_03.Size = New System.Drawing.Size(251, 860)
        Me.UcDefect01_03.TabIndex = 2
        '
        'UcDefect01_04
        '
        Me.UcDefect01_04.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcDefect01_04.Location = New System.Drawing.Point(753, 0)
        Me.UcDefect01_04.Margin = New System.Windows.Forms.Padding(0)
        Me.UcDefect01_04.Name = "UcDefect01_04"
        Me.UcDefect01_04.Size = New System.Drawing.Size(251, 860)
        Me.UcDefect01_04.TabIndex = 3
        '
        'UcDefect01_05
        '
        Me.UcDefect01_05.Dock = System.Windows.Forms.DockStyle.Fill
        Me.UcDefect01_05.Location = New System.Drawing.Point(1004, 0)
        Me.UcDefect01_05.Margin = New System.Windows.Forms.Padding(0)
        Me.UcDefect01_05.Name = "UcDefect01_05"
        Me.UcDefect01_05.Size = New System.Drawing.Size(252, 860)
        Me.UcDefect01_05.TabIndex = 4
        '
        'frmCaplDefList_Pattern01
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.tblLayoutPnl)
        Me.Name = "frmCaplDefList_Pattern01"
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.tblLayoutPnl, 0)
        Me.tblLayoutPnl.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents tblLayoutPnl As TableLayoutPanel
    Friend WithEvents UcDefect01_01 As ucDefect01
    Friend WithEvents UcDefect01_02 As ucDefect01
    Friend WithEvents UcDefect01_03 As ucDefect01
    Friend WithEvents UcDefect01_04 As ucDefect01
    Friend WithEvents UcDefect01_05 As ucDefect01
End Class
