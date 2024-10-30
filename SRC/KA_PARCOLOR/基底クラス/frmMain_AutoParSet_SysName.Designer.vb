<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain_AutoParSet_SysName
    Inherits KA_PARCOLOR.frmMain_AutoParSet

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
        Me.lblSysName = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'lstVMenu
        '
        Me.lstVMenu.Location = New System.Drawing.Point(4, 44)
        Me.lstVMenu.Size = New System.Drawing.Size(188, 360)
        '
        'lblSysName
        '
        Me.lblSysName.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSysName.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSysName.ForeColor = System.Drawing.Color.Navy
        Me.lblSysName.Location = New System.Drawing.Point(4, 4)
        Me.lblSysName.Name = "lblSysName"
        Me.lblSysName.Size = New System.Drawing.Size(188, 40)
        Me.lblSysName.TabIndex = 0
        Me.lblSysName.Text = "ÉVÉXÉeÉÄñºèÃ"
        Me.lblSysName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'frmMain_AutoParSet_SysName
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.lblSysName)
        Me.Name = "frmMain_AutoParSet_SysName"
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.lblSysName, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents lblSysName As System.Windows.Forms.Label

End Class
