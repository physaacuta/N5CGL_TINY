<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_DbCheck
	Inherits tClassForm.frmPop_Btn1


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
		Me.components = New System.ComponentModel.Container
		Me.Panel4 = New System.Windows.Forms.Panel
		Me.lblMsg = New System.Windows.Forms.Label
		Me.ProgressBar1 = New System.Windows.Forms.ProgressBar
		Me.trmCycle = New System.Windows.Forms.Timer(Me.components)
		Me.Panel4.SuspendLayout()
		Me.SuspendLayout()
		'
		'grpFunction
		'
		Me.grpFunction.Location = New System.Drawing.Point(120, 144)
		'
		'btnF01
		'
		Me.btnF01.Tag = 0
		'
		'Panel4
		'
		Me.Panel4.BackColor = System.Drawing.Color.LemonChiffon
		Me.Panel4.Controls.Add(Me.lblMsg)
		Me.Panel4.Location = New System.Drawing.Point(4, 4)
		Me.Panel4.Name = "Panel4"
		Me.Panel4.Size = New System.Drawing.Size(380, 112)
		Me.Panel4.TabIndex = 13
		'
		'lblMsg
		'
		Me.lblMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblMsg.Location = New System.Drawing.Point(8, 8)
		Me.lblMsg.Name = "lblMsg"
		Me.lblMsg.Size = New System.Drawing.Size(364, 92)
		Me.lblMsg.TabIndex = 0
		Me.lblMsg.Text = "データベースに接続ができませんでした。" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "データベースとの接続が確立するまで、" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "しばらくお待ちください。"
		Me.lblMsg.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'ProgressBar1
		'
		Me.ProgressBar1.Location = New System.Drawing.Point(4, 120)
		Me.ProgressBar1.Name = "ProgressBar1"
		Me.ProgressBar1.Size = New System.Drawing.Size(380, 20)
		Me.ProgressBar1.Style = System.Windows.Forms.ProgressBarStyle.Continuous
		Me.ProgressBar1.TabIndex = 14
		'
		'trmCycle
		'
		Me.trmCycle.Enabled = True
		'
		'frmPop_DbCheck
		'
		Me.ClientSize = New System.Drawing.Size(390, 235)
		Me.Controls.Add(Me.ProgressBar1)
		Me.Controls.Add(Me.Panel4)
		Me.Name = "frmPop_DbCheck"
		Me.Text = "DB接続確認画面"
		Me.Controls.SetChildIndex(Me.Panel4, 0)
		Me.Controls.SetChildIndex(Me.grpFunction, 0)
		Me.Controls.SetChildIndex(Me.ProgressBar1, 0)
		Me.Panel4.ResumeLayout(False)
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents Panel4 As System.Windows.Forms.Panel
	Friend WithEvents ProgressBar1 As System.Windows.Forms.ProgressBar
	Private WithEvents lblMsg As System.Windows.Forms.Label
	Friend WithEvents trmCycle As System.Windows.Forms.Timer

End Class
