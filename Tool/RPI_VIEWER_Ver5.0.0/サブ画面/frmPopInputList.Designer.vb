<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopInputList
    Inherits PopBaseForm

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.dgvList = New tClassLibrary.tCnt.tCnt_DataGridView
        Me.btnSave = New System.Windows.Forms.Button
        Me.btnReRead = New System.Windows.Forms.Button
        Me.Label2 = New System.Windows.Forms.Label
        Me.btnOpenFolder = New System.Windows.Forms.Button
        Me.chkDspDisable = New System.Windows.Forms.CheckBox
        Me.btnAddAll = New System.Windows.Forms.Button
        Me.Label1 = New System.Windows.Forms.Label
        Me.txtUserName = New System.Windows.Forms.TextBox
        CType(Me.dgvList, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'dgvList
        '
        Me.dgvList.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvList.Location = New System.Drawing.Point(4, 32)
        Me.dgvList.Name = "dgvList"
        Me.dgvList.RowTemplate.Height = 21
        Me.dgvList.Size = New System.Drawing.Size(561, 156)
        Me.dgvList.TabIndex = 4
        '
        'btnSave
        '
        Me.btnSave.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnSave.Location = New System.Drawing.Point(464, 4)
        Me.btnSave.Name = "btnSave"
        Me.btnSave.Size = New System.Drawing.Size(100, 20)
        Me.btnSave.TabIndex = 5
        Me.btnSave.Text = "結果保存"
        Me.btnSave.UseVisualStyleBackColor = True
        '
        'btnReRead
        '
        Me.btnReRead.Location = New System.Drawing.Point(4, 4)
        Me.btnReRead.Name = "btnReRead"
        Me.btnReRead.Size = New System.Drawing.Size(56, 20)
        Me.btnReRead.TabIndex = 5
        Me.btnReRead.Text = "再読込"
        Me.btnReRead.UseVisualStyleBackColor = True
        '
        'Label2
        '
        Me.Label2.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.Label2.Location = New System.Drawing.Point(4, 196)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(480, 12)
        Me.Label2.TabIndex = 6
        Me.Label2.Text = "※画像変更（分解能変更など）を行った場合、ジャンプできません。"
        '
        'btnOpenFolder
        '
        Me.btnOpenFolder.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnOpenFolder.Location = New System.Drawing.Point(355, 4)
        Me.btnOpenFolder.Name = "btnOpenFolder"
        Me.btnOpenFolder.Size = New System.Drawing.Size(100, 20)
        Me.btnOpenFolder.TabIndex = 5
        Me.btnOpenFolder.Text = "保存フォルダ表示"
        Me.btnOpenFolder.UseVisualStyleBackColor = True
        '
        'chkDspDisable
        '
        Me.chkDspDisable.Appearance = System.Windows.Forms.Appearance.Button
        Me.chkDspDisable.Location = New System.Drawing.Point(68, 4)
        Me.chkDspDisable.Name = "chkDspDisable"
        Me.chkDspDisable.Size = New System.Drawing.Size(80, 20)
        Me.chkDspDisable.TabIndex = 7
        Me.chkDspDisable.Text = "凹：表示無効"
        Me.chkDspDisable.UseVisualStyleBackColor = True
        '
        'btnAddAll
        '
        Me.btnAddAll.Location = New System.Drawing.Point(156, 4)
        Me.btnAddAll.Name = "btnAddAll"
        Me.btnAddAll.Size = New System.Drawing.Size(88, 20)
        Me.btnAddAll.TabIndex = 8
        Me.btnAddAll.Text = "疵位置全登録"
        Me.btnAddAll.UseVisualStyleBackColor = True
        Me.btnAddAll.Visible = False
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(256, 8)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(17, 12)
        Me.Label1.TabIndex = 9
        Me.Label1.Text = "名"
        '
        'txtUserName
        '
        Me.txtUserName.Location = New System.Drawing.Point(272, 4)
        Me.txtUserName.Name = "txtUserName"
        Me.txtUserName.Size = New System.Drawing.Size(72, 19)
        Me.txtUserName.TabIndex = 10
        '
        'frmPopInputList
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(567, 211)
        Me.Controls.Add(Me.txtUserName)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.btnAddAll)
        Me.Controls.Add(Me.chkDspDisable)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.btnOpenFolder)
        Me.Controls.Add(Me.btnReRead)
        Me.Controls.Add(Me.btnSave)
        Me.Controls.Add(Me.dgvList)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopInputList"
        Me.ShowIcon = False
        Me.Text = "選択位置一覧画面"
        CType(Me.dgvList, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents dgvList As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents btnSave As System.Windows.Forms.Button
    Friend WithEvents btnReRead As System.Windows.Forms.Button
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents btnOpenFolder As System.Windows.Forms.Button
    Friend WithEvents chkDspDisable As System.Windows.Forms.CheckBox
    Friend WithEvents btnAddAll As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents txtUserName As System.Windows.Forms.TextBox
End Class
