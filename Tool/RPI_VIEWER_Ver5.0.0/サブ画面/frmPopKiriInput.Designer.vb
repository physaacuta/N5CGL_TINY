<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopKiriInput
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

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.cmdClipCopy = New System.Windows.Forms.Button()
        Me.picMain = New System.Windows.Forms.PictureBox()
        Me.cmdPosSave = New System.Windows.Forms.Button()
        Me.Panel1 = New System.Windows.Forms.Panel()
        CType(Me.picMain, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.Panel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'cmdClipCopy
        '
        Me.cmdClipCopy.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cmdClipCopy.Location = New System.Drawing.Point(60, 0)
        Me.cmdClipCopy.MinimumSize = New System.Drawing.Size(30, 20)
        Me.cmdClipCopy.Name = "cmdClipCopy"
        Me.cmdClipCopy.Size = New System.Drawing.Size(92, 20)
        Me.cmdClipCopy.TabIndex = 1
        Me.cmdClipCopy.Text = "ｸﾘｯﾌﾟﾎﾞｰﾄﾞｺﾋﾟｰ"
        Me.cmdClipCopy.UseVisualStyleBackColor = True
        '
        'picMain
        '
        Me.picMain.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                  Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.picMain.Location = New System.Drawing.Point(0, 20)
        Me.picMain.Name = "picMain"
        Me.picMain.Size = New System.Drawing.Size(156, 124)
        Me.picMain.TabIndex = 2
        Me.picMain.TabStop = False
        '
        'cmdPosSave
        '
        Me.cmdPosSave.Location = New System.Drawing.Point(0, 0)
        Me.cmdPosSave.MinimumSize = New System.Drawing.Size(30, 20)
        Me.cmdPosSave.Name = "cmdPosSave"
        Me.cmdPosSave.Size = New System.Drawing.Size(62, 20)
        Me.cmdPosSave.TabIndex = 3
        Me.cmdPosSave.Text = "位置保存"
        Me.cmdPosSave.UseVisualStyleBackColor = True
        '
        'Panel1
        '
        Me.Panel1.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                  Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.Panel1.Controls.Add(Me.cmdPosSave)
        Me.Panel1.Controls.Add(Me.cmdClipCopy)
        Me.Panel1.Location = New System.Drawing.Point(0, 0)
        Me.Panel1.Margin = New System.Windows.Forms.Padding(0)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(157, 20)
        Me.Panel1.TabIndex = 1
        '
        'frmPopKiriInput
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(154, 144)
        Me.Controls.Add(Me.Panel1)
        Me.Controls.Add(Me.picMain)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.MinimumSize = New System.Drawing.Size(60, 27)
        Me.Name = "frmPopKiriInput"
        Me.ShowIcon = False
        Me.Text = "任意選択画面"
        CType(Me.picMain, System.ComponentModel.ISupportInitialize).EndInit()
        Me.Panel1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents cmdClipCopy As System.Windows.Forms.Button
    Friend WithEvents picMain As System.Windows.Forms.PictureBox
    Friend WithEvents cmdPosSave As System.Windows.Forms.Button
    Friend WithEvents Panel1 As System.Windows.Forms.Panel
End Class
