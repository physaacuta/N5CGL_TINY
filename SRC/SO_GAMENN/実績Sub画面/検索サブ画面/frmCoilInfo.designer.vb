<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmCoilInfo
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
        Me.btnEnd = New System.Windows.Forms.Button()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.cmbID = New System.Windows.Forms.ComboBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.dgvCoilInfo = New System.Windows.Forms.DataGridView()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label100 = New System.Windows.Forms.Label()
        Me.dgvNextCoilInfo = New System.Windows.Forms.DataGridView()
        Me.SplitContainer1 = New System.Windows.Forms.SplitContainer()
        CType(Me.dgvCoilInfo, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvNextCoilInfo, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.SplitContainer1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SplitContainer1.Panel1.SuspendLayout()
        Me.SplitContainer1.Panel2.SuspendLayout()
        Me.SplitContainer1.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnEnd
        '
        Me.btnEnd.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnEnd.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnEnd.Location = New System.Drawing.Point(793, 7)
        Me.btnEnd.Name = "btnEnd"
        Me.btnEnd.Size = New System.Drawing.Size(104, 39)
        Me.btnEnd.TabIndex = 42
        Me.btnEnd.Text = "F12:閉じる"
        Me.btnEnd.UseVisualStyleBackColor = True
        '
        'btnRead
        '
        Me.btnRead.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRead.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRead.Location = New System.Drawing.Point(680, 8)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(104, 39)
        Me.btnRead.TabIndex = 41
        Me.btnRead.Text = "読み込み"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'cmbID
        '
        Me.cmbID.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbID.FormattingEnabled = True
        Me.cmbID.Location = New System.Drawing.Point(64, 16)
        Me.cmbID.Name = "cmbID"
        Me.cmbID.Size = New System.Drawing.Size(204, 24)
        Me.cmbID.TabIndex = 40
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(20, 20)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(34, 21)
        Me.Label1.TabIndex = 39
        Me.Label1.Text = "ID"
        '
        'dgvCoilInfo
        '
        Me.dgvCoilInfo.AllowUserToAddRows = False
        Me.dgvCoilInfo.AllowUserToDeleteRows = False
        Me.dgvCoilInfo.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvCoilInfo.BackgroundColor = System.Drawing.SystemColors.Control
        Me.dgvCoilInfo.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCoilInfo.Location = New System.Drawing.Point(0, 0)
        Me.dgvCoilInfo.Name = "dgvCoilInfo"
        Me.dgvCoilInfo.ReadOnly = True
        Me.dgvCoilInfo.RowTemplate.Height = 21
        Me.dgvCoilInfo.Size = New System.Drawing.Size(900, 595)
        Me.dgvCoilInfo.TabIndex = 500
        '
        'Label12
        '
        Me.Label12.BackColor = System.Drawing.SystemColors.Control
        Me.Label12.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label12.ForeColor = System.Drawing.Color.Black
        Me.Label12.Location = New System.Drawing.Point(12, 52)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(148, 28)
        Me.Label12.TabIndex = 501
        Me.Label12.Text = "上段：コイル情報"
        Me.Label12.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label100
        '
        Me.Label100.BackColor = System.Drawing.SystemColors.Control
        Me.Label100.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label100.ForeColor = System.Drawing.Color.Black
        Me.Label100.Location = New System.Drawing.Point(188, 52)
        Me.Label100.Name = "Label100"
        Me.Label100.Size = New System.Drawing.Size(208, 28)
        Me.Label100.TabIndex = 502
        Me.Label100.Text = "下段：2～5コイル目情報"
        Me.Label100.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'dgvNextCoilInfo
        '
        Me.dgvNextCoilInfo.AllowUserToAddRows = False
        Me.dgvNextCoilInfo.AllowUserToDeleteRows = False
        Me.dgvNextCoilInfo.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvNextCoilInfo.BackgroundColor = System.Drawing.SystemColors.Control
        Me.dgvNextCoilInfo.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvNextCoilInfo.Location = New System.Drawing.Point(0, 0)
        Me.dgvNextCoilInfo.Name = "dgvNextCoilInfo"
        Me.dgvNextCoilInfo.ReadOnly = True
        Me.dgvNextCoilInfo.RowTemplate.Height = 21
        Me.dgvNextCoilInfo.Size = New System.Drawing.Size(900, 117)
        Me.dgvNextCoilInfo.TabIndex = 503
        '
        'SplitContainer1
        '
        Me.SplitContainer1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.SplitContainer1.Location = New System.Drawing.Point(4, 84)
        Me.SplitContainer1.Name = "SplitContainer1"
        Me.SplitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal
        '
        'SplitContainer1.Panel1
        '
        Me.SplitContainer1.Panel1.Controls.Add(Me.dgvCoilInfo)
        '
        'SplitContainer1.Panel2
        '
        Me.SplitContainer1.Panel2.Controls.Add(Me.dgvNextCoilInfo)
        Me.SplitContainer1.Size = New System.Drawing.Size(900, 720)
        Me.SplitContainer1.SplitterDistance = 597
        Me.SplitContainer1.TabIndex = 504
        '
        'frmCoilInfo
        '
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None
        Me.ClientSize = New System.Drawing.Size(906, 806)
        Me.ControlBox = True
        Me.Controls.Add(Me.SplitContainer1)
        Me.Controls.Add(Me.Label100)
        Me.Controls.Add(Me.Label12)
        Me.Controls.Add(Me.btnEnd)
        Me.Controls.Add(Me.btnRead)
        Me.Controls.Add(Me.cmbID)
        Me.Controls.Add(Me.Label1)
        Me.MaximizeBox = True
        Me.MinimizeBox = True
        Me.Name = "frmCoilInfo"
        Me.Text = "コイル情報伝文画面"
        Me.TopMost = False
        CType(Me.dgvCoilInfo, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvNextCoilInfo, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer1.Panel1.ResumeLayout(False)
        Me.SplitContainer1.Panel2.ResumeLayout(False)
        CType(Me.SplitContainer1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.SplitContainer1.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btnEnd As System.Windows.Forms.Button
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents cmbID As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents dgvCoilInfo As System.Windows.Forms.DataGridView
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Label100 As System.Windows.Forms.Label
    Friend WithEvents dgvNextCoilInfo As System.Windows.Forms.DataGridView
    Friend WithEvents SplitContainer1 As System.Windows.Forms.SplitContainer

End Class
