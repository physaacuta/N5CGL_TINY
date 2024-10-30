<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmLovItem
    Inherits frmMain_ParSet_SysName

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.dlgColor = New System.Windows.Forms.ColorDialog()
        Me.dgvLov_Item = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvLov_Item, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.Label3)
        Me.grpBase.Controls.Add(Me.dgvLov_Item)
        Me.grpBase.Text = "選択項目アイテムマスター"
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "終了"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "設定"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "切替"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "画面保存"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "画面印刷"
        '
        'dgvLov_Item
        '
        Me.dgvLov_Item.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvLov_Item.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvLov_Item.Location = New System.Drawing.Point(16, 40)
        Me.dgvLov_Item.Name = "dgvLov_Item"
        Me.dgvLov_Item.RowTemplate.Height = 21
        Me.dgvLov_Item.Size = New System.Drawing.Size(720, 860)
        Me.dgvLov_Item.TabIndex = 9
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label3.ForeColor = System.Drawing.Color.Black
        Me.Label3.Location = New System.Drawing.Point(752, 40)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(72, 16)
        Me.Label3.TabIndex = 61
        Me.Label3.Text = "<注意点>"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label1.ForeColor = System.Drawing.Color.Black
        Me.Label1.Location = New System.Drawing.Point(752, 64)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(184, 16)
        Me.Label1.TabIndex = 62
        Me.Label1.Text = "lov_idを削除する場合、"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label2.ForeColor = System.Drawing.Color.Black
        Me.Label2.Location = New System.Drawing.Point(752, 88)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(296, 16)
        Me.Label2.TabIndex = 63
        Me.Label2.Text = "それに紐付くﾃﾞｰﾀｸﾞﾘｯﾄﾞ表示設定項目も"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label4.ForeColor = System.Drawing.Color.Black
        Me.Label4.Location = New System.Drawing.Point(752, 112)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(120, 16)
        Me.Label4.TabIndex = 64
        Me.Label4.Text = "削除されます。"
        '
        'frmLovItem
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmLovItem"
        Me.Text = "選択項目アイテムマスター画面"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        CType(Me.dgvLov_Item, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dlgColor As System.Windows.Forms.ColorDialog
    Friend WithEvents dgvLov_Item As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label

End Class
