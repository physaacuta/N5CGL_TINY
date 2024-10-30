<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain_Ras2
	Inherits tClassForm.frmBase_Main

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
		Me.timGetStatus = New System.Windows.Forms.Timer(Me.components)
		Me.SuspendLayout()
		'
		'btnF12
		'
		Me.btnF12.Text = "メニュー"
		'
		'btnF11
		'
		Me.btnF11.Text = "画面保存"
		'
		'btnF10
		'
		Me.btnF10.Text = "画面印刷"
		'
		'btnF08
		'
		Me.btnF08.Enabled = True
		Me.btnF08.Text = "ｼｽﾃﾑ終了"
		'
		'timGetStatus
		'
		'
		'frmMain_Ras
		'
		Me.ClientSize = New System.Drawing.Size(1272, 997)
		Me.Name = "frmMain_Ras"
		Me.Text = "疵検用Ras画面の親フォーム"
		Me.ResumeLayout(False)

	End Sub
	Protected Friend WithEvents timGetStatus As System.Windows.Forms.Timer

End Class
