<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_Btn1
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
        Me.grpFunction = New System.Windows.Forms.GroupBox
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel
        Me.Panel2 = New System.Windows.Forms.Panel
        Me.btnF01 = New System.Windows.Forms.Button
        Me.lblF01_b = New System.Windows.Forms.Label
        Me.lblF01 = New System.Windows.Forms.Label
        Me.grpFunction.SuspendLayout()
        Me.TableLayoutPanel1.SuspendLayout()
        Me.Panel2.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Controls.Add(Me.TableLayoutPanel1)
        Me.grpFunction.Font = New System.Drawing.Font("ＭＳ 明朝", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpFunction.Location = New System.Drawing.Point(120, 132)
        Me.grpFunction.Name = "grpFunction"
        Me.grpFunction.Padding = New System.Windows.Forms.Padding(3, 0, 3, 3)
        Me.grpFunction.Size = New System.Drawing.Size(128, 84)
        Me.grpFunction.TabIndex = 13
        Me.grpFunction.TabStop = False
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 3
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 8.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 8.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.Panel2, 1, 1)
        Me.TableLayoutPanel1.Controls.Add(Me.lblF01, 1, 0)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(3, 15)
        Me.TableLayoutPanel1.Margin = New System.Windows.Forms.Padding(0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 2
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 16.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(122, 66)
        Me.TableLayoutPanel1.TabIndex = 0
        '
        'Panel2
        '
        Me.Panel2.Controls.Add(Me.btnF01)
        Me.Panel2.Controls.Add(Me.lblF01_b)
        Me.Panel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel2.Location = New System.Drawing.Point(8, 16)
        Me.Panel2.Margin = New System.Windows.Forms.Padding(0)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(106, 50)
        Me.Panel2.TabIndex = 4
        '
        'btnF01
        '
        Me.btnF01.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnF01.Font = New System.Drawing.Font("ＭＳ 明朝", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF01.Location = New System.Drawing.Point(0, 0)
        Me.btnF01.Margin = New System.Windows.Forms.Padding(0)
        Me.btnF01.Name = "btnF01"
        Me.btnF01.Size = New System.Drawing.Size(100, 43)
        Me.btnF01.TabIndex = 6
        Me.btnF01.Tag = "1"
        Me.btnF01.Text = "終了"
        Me.btnF01.UseVisualStyleBackColor = True
        '
        'lblF01_b
        '
        Me.lblF01_b.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblF01_b.BackColor = System.Drawing.Color.Black
        Me.lblF01_b.Enabled = False
        Me.lblF01_b.Location = New System.Drawing.Point(4, 4)
        Me.lblF01_b.Name = "lblF01_b"
        Me.lblF01_b.Size = New System.Drawing.Size(100, 43)
        Me.lblF01_b.TabIndex = 7
        '
        'lblF01
        '
        Me.lblF01.AutoSize = True
        Me.lblF01.Location = New System.Drawing.Point(8, 0)
        Me.lblF01.Margin = New System.Windows.Forms.Padding(0)
        Me.lblF01.Name = "lblF01"
        Me.lblF01.Size = New System.Drawing.Size(34, 15)
        Me.lblF01.TabIndex = 4
        Me.lblF01.Text = "F12"
        '
        'frmPop_Btn1
        '
        Me.ClientSize = New System.Drawing.Size(371, 219)
        Me.Controls.Add(Me.grpFunction)
        Me.Name = "frmPop_Btn1"
        Me.Text = "1ボタン用ポップアップ画面の親フォーム"
        Me.grpFunction.ResumeLayout(False)
        Me.TableLayoutPanel1.ResumeLayout(False)
        Me.TableLayoutPanel1.PerformLayout()
        Me.Panel2.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
	Protected Friend WithEvents grpFunction As System.Windows.Forms.GroupBox
	Friend WithEvents TableLayoutPanel1 As System.Windows.Forms.TableLayoutPanel
	Friend WithEvents Panel2 As System.Windows.Forms.Panel
	Protected Friend WithEvents btnF01 As System.Windows.Forms.Button
	Protected Friend WithEvents lblF01_b As System.Windows.Forms.Label
	Protected Friend WithEvents lblF01 As System.Windows.Forms.Label

End Class
