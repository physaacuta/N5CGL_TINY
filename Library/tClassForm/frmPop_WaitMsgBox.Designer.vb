<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_WaitMsgBox
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
		Me.ProgressBar1 = New System.Windows.Forms.ProgressBar
		Me.Panel4 = New System.Windows.Forms.Panel
		Me.lblMsg2 = New System.Windows.Forms.Label
		Me.lblMsg1 = New System.Windows.Forms.Label
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
		Me.btnF01.Tag = 11
		'
		'ProgressBar1
		'
		Me.ProgressBar1.Location = New System.Drawing.Point(4, 120)
		Me.ProgressBar1.Name = "ProgressBar1"
		Me.ProgressBar1.Size = New System.Drawing.Size(380, 20)
		Me.ProgressBar1.Style = System.Windows.Forms.ProgressBarStyle.Continuous
		Me.ProgressBar1.TabIndex = 16
		'
		'Panel4
		'
		Me.Panel4.BackColor = System.Drawing.Color.LemonChiffon
		Me.Panel4.Controls.Add(Me.lblMsg2)
		Me.Panel4.Controls.Add(Me.lblMsg1)
		Me.Panel4.Location = New System.Drawing.Point(4, 4)
		Me.Panel4.Name = "Panel4"
		Me.Panel4.Size = New System.Drawing.Size(380, 112)
		Me.Panel4.TabIndex = 15
		'
		'lblMsg2
		'
		Me.lblMsg2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblMsg2.Location = New System.Drawing.Point(8, 60)
		Me.lblMsg2.Name = "lblMsg2"
		Me.lblMsg2.Size = New System.Drawing.Size(364, 48)
		Me.lblMsg2.TabIndex = 0
		Me.lblMsg2.Text = "ÇµÇŒÇÁÇ≠Ç®ë“ÇøÇ≠ÇæÇ≥Ç¢ÅB"
		Me.lblMsg2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'lblMsg1
		'
		Me.lblMsg1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblMsg1.Location = New System.Drawing.Point(8, 8)
		Me.lblMsg1.Name = "lblMsg1"
		Me.lblMsg1.Size = New System.Drawing.Size(364, 48)
		Me.lblMsg1.TabIndex = 0
		Me.lblMsg1.Text = "ÉfÅ[É^èàóùíÜÇ≈Ç∑ÅB"
		Me.lblMsg1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'trmCycle
		'
		'
		'frmPop_WaitMsgBox
		'
		Me.ClientSize = New System.Drawing.Size(390, 235)
		Me.Controls.Add(Me.ProgressBar1)
		Me.Controls.Add(Me.Panel4)
		Me.Name = "frmPop_WaitMsgBox"
		Me.Controls.SetChildIndex(Me.grpFunction, 0)
		Me.Controls.SetChildIndex(Me.Panel4, 0)
		Me.Controls.SetChildIndex(Me.ProgressBar1, 0)
		Me.Panel4.ResumeLayout(False)
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents ProgressBar1 As System.Windows.Forms.ProgressBar
	Friend WithEvents Panel4 As System.Windows.Forms.Panel
	Friend WithEvents trmCycle As System.Windows.Forms.Timer
	Public WithEvents lblMsg1 As System.Windows.Forms.Label
	Public WithEvents lblMsg2 As System.Windows.Forms.Label

End Class
