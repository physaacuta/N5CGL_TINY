<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSCodeMaster
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
        Me.Label1 = New System.Windows.Forms.Label()
        Me.dgvData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvEtc = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvEtc, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.dgvEtc)
        Me.grpBase.Controls.Add(Me.dgvData)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Text = "検査表面状態マスター登録"
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
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(672, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(224, 48)
        Me.Label1.TabIndex = 10
        Me.Label1.Text = "※１ 優先度は１が最も高く、" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "9999が最も低くなります。" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "優先度の重複はできません。" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        '
        'dgvData
        '
        Me.dgvData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvData.Location = New System.Drawing.Point(12, 24)
        Me.dgvData.Name = "dgvData"
        Me.dgvData.RowTemplate.Height = 21
        Me.dgvData.Size = New System.Drawing.Size(648, 840)
        Me.dgvData.TabIndex = 11
        '
        'dgvEtc
        '
        Me.dgvEtc.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvEtc.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvEtc.Location = New System.Drawing.Point(12, 868)
        Me.dgvEtc.Name = "dgvEtc"
        Me.dgvEtc.RowTemplate.Height = 21
        Me.dgvEtc.Size = New System.Drawing.Size(632, 22)
        Me.dgvEtc.TabIndex = 12
        '
        'frmSCodeMaster
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmSCodeMaster"
        Me.Text = "検査表面状態マスター登録画面"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvEtc, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents dgvData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvEtc As tClassLibrary.tCnt.tCnt_DataGridView

End Class
