<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcFilter
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
        Me.dgvCoef = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbId = New System.Windows.Forms.ComboBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.cmbSizeX = New System.Windows.Forms.ComboBox()
        Me.grpCoef = New System.Windows.Forms.GroupBox()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.txtName = New System.Windows.Forms.TextBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.txtOffset = New System.Windows.Forms.TextBox()
        Me.btnDel = New System.Windows.Forms.Button()
        Me.btnAdd = New System.Windows.Forms.Button()
        Me.lblSizeY = New System.Windows.Forms.Label()
        Me.lblSizeX = New System.Windows.Forms.Label()
        Me.txtNormalizeCoef = New System.Windows.Forms.TextBox()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvCoef, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpCoef.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.lblSizeX)
        Me.grpBase.Controls.Add(Me.lblSizeY)
        Me.grpBase.Controls.Add(Me.btnDel)
        Me.grpBase.Controls.Add(Me.btnAdd)
        Me.grpBase.Controls.Add(Me.txtNormalizeCoef)
        Me.grpBase.Controls.Add(Me.txtOffset)
        Me.grpBase.Controls.Add(Me.Label6)
        Me.grpBase.Controls.Add(Me.Label7)
        Me.grpBase.Controls.Add(Me.txtName)
        Me.grpBase.Controls.Add(Me.grpCoef)
        Me.grpBase.Controls.Add(Me.cmbSizeX)
        Me.grpBase.Controls.Add(Me.Label5)
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.Label3)
        Me.grpBase.Controls.Add(Me.cmbId)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Text = "ÉtÉBÉãÉ^ê›íËâÊñ "
        '
        'grpFunction
        '
        Me.grpFunction.Size = New System.Drawing.Size(1252, 68)
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "èIóπ"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "ê›íË"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "êÿë÷"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "âÊñ ï€ë∂"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "âÊñ àÛç¸"
        '
        'dgvCoef
        '
        Me.dgvCoef.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCoef.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCoef.Location = New System.Drawing.Point(37, 46)
        Me.dgvCoef.Name = "dgvCoef"
        Me.dgvCoef.RowTemplate.Height = 21
        Me.dgvCoef.Size = New System.Drawing.Size(740, 224)
        Me.dgvCoef.TabIndex = 68
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(12, 31)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(154, 24)
        Me.Label1.TabIndex = 69
        Me.Label1.Text = "ÉtÉBÉãÉ^Å[ID"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbId
        '
        Me.cmbId.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbId.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.cmbId.FormattingEnabled = True
        Me.cmbId.Location = New System.Drawing.Point(167, 31)
        Me.cmbId.Name = "cmbId"
        Me.cmbId.Size = New System.Drawing.Size(156, 24)
        Me.cmbId.TabIndex = 70
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label3.Location = New System.Drawing.Point(12, 79)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(154, 24)
        Me.Label3.TabIndex = 72
        Me.Label3.Text = "ñºèÃ"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(12, 120)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(154, 24)
        Me.Label4.TabIndex = 73
        Me.Label4.Text = "ÉJÅ[ÉlÉãÉTÉCÉY X"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label5.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label5.Location = New System.Drawing.Point(12, 144)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(154, 24)
        Me.Label5.TabIndex = 74
        Me.Label5.Text = "ÉJÅ[ÉlÉãÉTÉCÉY Y"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSizeX
        '
        Me.cmbSizeX.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSizeX.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.cmbSizeX.FormattingEnabled = True
        Me.cmbSizeX.Location = New System.Drawing.Point(167, 120)
        Me.cmbSizeX.Name = "cmbSizeX"
        Me.cmbSizeX.Size = New System.Drawing.Size(156, 24)
        Me.cmbSizeX.TabIndex = 76
        '
        'grpCoef
        '
        Me.grpCoef.Controls.Add(Me.Label9)
        Me.grpCoef.Controls.Add(Me.Label8)
        Me.grpCoef.Controls.Add(Me.dgvCoef)
        Me.grpCoef.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpCoef.ForeColor = System.Drawing.Color.Black
        Me.grpCoef.Location = New System.Drawing.Point(12, 184)
        Me.grpCoef.Name = "grpCoef"
        Me.grpCoef.Size = New System.Drawing.Size(790, 280)
        Me.grpCoef.TabIndex = 78
        Me.grpCoef.TabStop = False
        Me.grpCoef.Text = "â¡èdíl"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Location = New System.Drawing.Point(388, 19)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(19, 19)
        Me.Label9.TabIndex = 70
        Me.Label9.Text = "X"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(12, 162)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(19, 19)
        Me.Label8.TabIndex = 69
        Me.Label8.Text = "Y"
        '
        'txtName
        '
        Me.txtName.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtName.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtName.Location = New System.Drawing.Point(167, 79)
        Me.txtName.Name = "txtName"
        Me.txtName.Size = New System.Drawing.Size(562, 23)
        Me.txtName.TabIndex = 79
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label6.Location = New System.Drawing.Point(12, 536)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(154, 24)
        Me.Label6.TabIndex = 81
        Me.Label6.Text = "ÉIÉtÉZÉbÉg"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(12, 512)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(154, 24)
        Me.Label7.TabIndex = 80
        Me.Label7.Text = "ê≥ãKâªåWêî"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtOffset
        '
        Me.txtOffset.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtOffset.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtOffset.Location = New System.Drawing.Point(167, 536)
        Me.txtOffset.Name = "txtOffset"
        Me.txtOffset.Size = New System.Drawing.Size(156, 23)
        Me.txtOffset.TabIndex = 83
        '
        'btnDel
        '
        Me.btnDel.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnDel.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnDel.Location = New System.Drawing.Point(513, 31)
        Me.btnDel.Name = "btnDel"
        Me.btnDel.Size = New System.Drawing.Size(92, 32)
        Me.btnDel.TabIndex = 85
        Me.btnDel.Text = "çÌèú"
        Me.btnDel.UseVisualStyleBackColor = True
        '
        'btnAdd
        '
        Me.btnAdd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnAdd.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnAdd.Location = New System.Drawing.Point(413, 31)
        Me.btnAdd.Name = "btnAdd"
        Me.btnAdd.Size = New System.Drawing.Size(92, 32)
        Me.btnAdd.TabIndex = 84
        Me.btnAdd.Text = "êVãKí«â¡"
        Me.btnAdd.UseVisualStyleBackColor = True
        '
        'lblSizeY
        '
        Me.lblSizeY.BackColor = System.Drawing.Color.White
        Me.lblSizeY.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSizeY.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.lblSizeY.ForeColor = System.Drawing.SystemColors.ControlText
        Me.lblSizeY.Location = New System.Drawing.Point(167, 144)
        Me.lblSizeY.Name = "lblSizeY"
        Me.lblSizeY.Size = New System.Drawing.Size(154, 24)
        Me.lblSizeY.TabIndex = 86
        Me.lblSizeY.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblSizeX
        '
        Me.lblSizeX.BackColor = System.Drawing.Color.White
        Me.lblSizeX.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSizeX.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.lblSizeX.ForeColor = System.Drawing.SystemColors.ControlText
        Me.lblSizeX.Location = New System.Drawing.Point(167, 120)
        Me.lblSizeX.Name = "lblSizeX"
        Me.lblSizeX.Size = New System.Drawing.Size(136, 24)
        Me.lblSizeX.TabIndex = 88
        Me.lblSizeX.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'txtNormalizeCoef
        '
        Me.txtNormalizeCoef.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtNormalizeCoef.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtNormalizeCoef.Location = New System.Drawing.Point(168, 512)
        Me.txtNormalizeCoef.Name = "txtNormalizeCoef"
        Me.txtNormalizeCoef.Size = New System.Drawing.Size(156, 23)
        Me.txtNormalizeCoef.TabIndex = 83
        '
        'frmEpcFilter
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmEpcFilter"
        Me.Text = "ÉtÉBÉãÉ^ê›íË"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        CType(Me.dgvCoef, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpCoef.ResumeLayout(False)
        Me.grpCoef.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvCoef As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents cmbId As System.Windows.Forms.ComboBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents cmbSizeX As System.Windows.Forms.ComboBox
    Friend WithEvents grpCoef As System.Windows.Forms.GroupBox
    Friend WithEvents txtName As System.Windows.Forms.TextBox
    Friend WithEvents txtOffset As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents btnDel As System.Windows.Forms.Button
    Friend WithEvents btnAdd As System.Windows.Forms.Button
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents lblSizeY As System.Windows.Forms.Label
    Friend WithEvents lblSizeX As System.Windows.Forms.Label
    Friend WithEvents txtNormalizeCoef As TextBox
End Class
