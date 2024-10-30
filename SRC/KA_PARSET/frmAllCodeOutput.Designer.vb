<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmAllCodeOutput
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
        Me.grpShousai = New System.Windows.Forms.GroupBox()
        Me.dgvShousaiEtc = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvShousaiData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.grpBase.SuspendLayout()
        Me.grpShousai.SuspendLayout()
        CType(Me.dgvShousaiEtc, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvShousaiData, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.grpShousai)
        Me.grpBase.Text = "重大欠陥表示設定"
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "終了"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "設定"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "切替"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "画面保存"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "画面印刷"
        '
        'grpShousai
        '
        Me.grpShousai.Controls.Add(Me.dgvShousaiEtc)
        Me.grpShousai.Controls.Add(Me.dgvShousaiData)
        Me.grpShousai.Location = New System.Drawing.Point(24, 45)
        Me.grpShousai.Name = "grpShousai"
        Me.grpShousai.Size = New System.Drawing.Size(500, 816)
        Me.grpShousai.TabIndex = 38
        Me.grpShousai.TabStop = False
        Me.grpShousai.Text = "詳細設定"
        '
        'dgvShousaiEtc
        '
        Me.dgvShousaiEtc.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvShousaiEtc.Location = New System.Drawing.Point(10, 788)
        Me.dgvShousaiEtc.Name = "dgvShousaiEtc"
        Me.dgvShousaiEtc.RowTemplate.Height = 21
        Me.dgvShousaiEtc.ScrollBars = System.Windows.Forms.ScrollBars.None
        Me.dgvShousaiEtc.Size = New System.Drawing.Size(465, 20)
        Me.dgvShousaiEtc.TabIndex = 31
        '
        'dgvShousaiData
        '
        Me.dgvShousaiData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvShousaiData.Location = New System.Drawing.Point(10, 24)
        Me.dgvShousaiData.Name = "dgvShousaiData"
        Me.dgvShousaiData.RowTemplate.Height = 21
        Me.dgvShousaiData.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.ColumnHeaderSelect
        Me.dgvShousaiData.Size = New System.Drawing.Size(480, 758)
        Me.dgvShousaiData.TabIndex = 12
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(236, 28)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(256, 16)
        Me.Label4.TabIndex = 42
        Me.Label4.Text = "※'―’は、出力無しの設定です。"
        '
        'frmAllCodeOutput
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Name = "frmAllCodeOutput"
        Me.Text = "重大欠陥表示設定画面"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        Me.grpShousai.ResumeLayout(False)
        CType(Me.dgvShousaiEtc, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvShousaiData, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents grpShousai As System.Windows.Forms.GroupBox
    Friend WithEvents dgvShousaiEtc As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvShousaiData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label4 As System.Windows.Forms.Label
End Class
