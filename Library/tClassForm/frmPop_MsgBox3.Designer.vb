<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_MsgBox3
    Inherits tClassForm.frmPop_Btn3

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
		Me.Panel4 = New System.Windows.Forms.Panel
		Me.lblMsg = New System.Windows.Forms.Label
		Me.Panel4.SuspendLayout()
		Me.SuspendLayout()
		'
		'grpFunction
		'
		Me.grpFunction.Location = New System.Drawing.Point(24, 164)
		'
		'btnF01
		'
		Me.btnF01.Tag = 0
		'
		'btnF02
		'
		Me.btnF02.Tag = 4
		'
		'btnF03
		'
		Me.btnF03.Tag = 11
		'
		'Panel4
		'
		Me.Panel4.BackColor = System.Drawing.Color.LemonChiffon
		Me.Panel4.Controls.Add(Me.lblMsg)
		Me.Panel4.Location = New System.Drawing.Point(12, 4)
		Me.Panel4.Name = "Panel4"
		Me.Panel4.Size = New System.Drawing.Size(360, 156)
		Me.Panel4.TabIndex = 13
		'
		'lblMsg
		'
		Me.lblMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblMsg.Location = New System.Drawing.Point(8, 8)
		Me.lblMsg.Name = "lblMsg"
		Me.lblMsg.Size = New System.Drawing.Size(344, 140)
		Me.lblMsg.TabIndex = 0
		Me.lblMsg.Text = "aaaaあああaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
		'
		'frmPop_MsgBox3
		'
		Me.ClientSize = New System.Drawing.Size(385, 249)
		Me.Controls.Add(Me.Panel4)
		Me.Name = "frmPop_MsgBox3"
		Me.Text = "メッセージボックス用ポップアップ画面の親フォーム"
		Me.TopMost = False
		Me.Controls.SetChildIndex(Me.grpFunction, 0)
		Me.Controls.SetChildIndex(Me.Panel4, 0)
		Me.Panel4.ResumeLayout(False)
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents Panel4 As System.Windows.Forms.Panel
	Public WithEvents lblMsg As System.Windows.Forms.Label

End Class
