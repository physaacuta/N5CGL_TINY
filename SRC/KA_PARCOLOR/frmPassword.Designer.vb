<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPassword
    Inherits tClassForm.frmPop_Btn2
    'Inherits tClassForm.frmPop_Pass
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
        Me.lblMsg01 = New System.Windows.Forms.Label
        Me.lblMsg = New System.Windows.Forms.Label
        Me.txtPass = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.Panel3 = New System.Windows.Forms.Panel
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(128, 184)
        '
        'btnF01
        '
        Me.btnF01.Tag = 0
        '
        'btnF02
        '
        Me.btnF02.Tag = 11
        '
        'lblMsg01
        '
        Me.lblMsg01.BackColor = System.Drawing.Color.LemonChiffon
        Me.lblMsg01.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMsg01.Location = New System.Drawing.Point(11, 12)
        Me.lblMsg01.Name = "lblMsg01"
        Me.lblMsg01.Size = New System.Drawing.Size(453, 20)
        Me.lblMsg01.TabIndex = 13
        Me.lblMsg01.Text = "設定値の変更を行う為には、パスワード入力が必要です。"
        '
        'lblMsg
        '
        Me.lblMsg.BackColor = System.Drawing.Color.LemonChiffon
        Me.lblMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMsg.Location = New System.Drawing.Point(12, 32)
        Me.lblMsg.Name = "lblMsg"
        Me.lblMsg.Size = New System.Drawing.Size(452, 20)
        Me.lblMsg.TabIndex = 14
        Me.lblMsg.Text = "パスワードを入力して下さい。"
        '
        'txtPass
        '
        Me.txtPass.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtPass.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtPass.Location = New System.Drawing.Point(84, 156)
        Me.txtPass.Name = "txtPass"
        Me.txtPass.Size = New System.Drawing.Size(308, 23)
        Me.txtPass.TabIndex = 0
        '
        'Label1
        '
        Me.Label1.BackColor = System.Drawing.Color.LemonChiffon
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(12, 88)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(448, 20)
        Me.Label1.TabIndex = 15
        Me.Label1.Text = "入力成功  ：設定値の変更が可能です。"
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.Color.LemonChiffon
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(12, 108)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(448, 20)
        Me.Label2.TabIndex = 16
        Me.Label2.Text = "キャンセル：設定値の参照のみ可能です。"
        '
        'Label3
        '
        Me.Label3.BackColor = System.Drawing.Color.LemonChiffon
        Me.Label3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(12, 52)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(452, 20)
        Me.Label3.TabIndex = 17
        Me.Label3.Text = "パスワード未設定時は、キャンセルのみ有効になります。"
        '
        'Panel3
        '
        Me.Panel3.BackColor = System.Drawing.Color.LemonChiffon
        Me.Panel3.Location = New System.Drawing.Point(8, 4)
        Me.Panel3.Name = "Panel3"
        Me.Panel3.Size = New System.Drawing.Size(460, 144)
        Me.Panel3.TabIndex = 18
        '
        'frmPassword
        '
        Me.ClientSize = New System.Drawing.Size(476, 274)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.txtPass)
        Me.Controls.Add(Me.lblMsg)
        Me.Controls.Add(Me.lblMsg01)
        Me.Controls.Add(Me.Panel3)
        Me.Name = "frmPassword"
        Me.Text = "パラメータ設定画面 パスワード入力"
        Me.Controls.SetChildIndex(Me.Panel3, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.lblMsg01, 0)
        Me.Controls.SetChildIndex(Me.lblMsg, 0)
        Me.Controls.SetChildIndex(Me.txtPass, 0)
        Me.Controls.SetChildIndex(Me.Label1, 0)
        Me.Controls.SetChildIndex(Me.Label2, 0)
        Me.Controls.SetChildIndex(Me.Label3, 0)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents lblMsg01 As System.Windows.Forms.Label
    Friend WithEvents lblMsg As System.Windows.Forms.Label
    Friend WithEvents txtPass As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Panel3 As System.Windows.Forms.Panel

End Class
