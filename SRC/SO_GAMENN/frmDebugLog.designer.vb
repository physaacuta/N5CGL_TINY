<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmDebugLog
    Inherits tClassForm.frmBase_Popup

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
		Me.lstDebug = New System.Windows.Forms.ListBox
		Me.btnHide = New System.Windows.Forms.Button
		Me.SuspendLayout()
		'
		'lstDebug
		'
		Me.lstDebug.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
					Or System.Windows.Forms.AnchorStyles.Left) _
					Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lstDebug.FormattingEnabled = True
		Me.lstDebug.ItemHeight = 12
		Me.lstDebug.Location = New System.Drawing.Point(4, 36)
		Me.lstDebug.Name = "lstDebug"
		Me.lstDebug.Size = New System.Drawing.Size(400, 220)
		Me.lstDebug.TabIndex = 0
		'
		'btnHide
		'
		Me.btnHide.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnHide.Location = New System.Drawing.Point(328, 8)
		Me.btnHide.Name = "btnHide"
		Me.btnHide.Size = New System.Drawing.Size(75, 23)
		Me.btnHide.TabIndex = 1
		Me.btnHide.Text = "隠す"
		Me.btnHide.UseVisualStyleBackColor = True
		'
		'frmDebugLog
		'
		Me.ClientSize = New System.Drawing.Size(408, 257)
		Me.ControlBox = True
		Me.Controls.Add(Me.btnHide)
		Me.Controls.Add(Me.lstDebug)
		Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Sizable
		Me.MaximizeBox = True
		Me.MinimizeBox = True
		Me.Name = "frmDebugLog"
		Me.Text = "デバッグログ"
		Me.TopMost = False
		Me.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iOFFLINE
		Me.tProp_Moveable = True
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents lstDebug As System.Windows.Forms.ListBox
	Friend WithEvents btnHide As System.Windows.Forms.Button

End Class
