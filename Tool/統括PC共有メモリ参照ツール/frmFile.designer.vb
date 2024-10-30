<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmFile
    Inherits System.Windows.Forms.Form

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

    'Windows フォーム デザイナで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
    'Windows フォーム デザイナを使用して変更できます。  
    'コード エディタを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
		Me.btnRead = New System.Windows.Forms.Button
		Me.btnWrite = New System.Windows.Forms.Button
		Me.dgvSharedMemory = New System.Windows.Forms.DataGridView
		Me.lblBaseFolder = New System.Windows.Forms.Label
		Me.Label1 = New System.Windows.Forms.Label
		Me.btnBaseFolder = New System.Windows.Forms.Button
		CType(Me.dgvSharedMemory, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.SuspendLayout()
		'
		'btnRead
		'
		Me.btnRead.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnRead.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnRead.Location = New System.Drawing.Point(408, 312)
		Me.btnRead.Name = "btnRead"
		Me.btnRead.Size = New System.Drawing.Size(108, 32)
		Me.btnRead.TabIndex = 5
		Me.btnRead.Text = "バイナリ読込"
		Me.btnRead.UseVisualStyleBackColor = True
		'
		'btnWrite
		'
		Me.btnWrite.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnWrite.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnWrite.Location = New System.Drawing.Point(520, 312)
		Me.btnWrite.Name = "btnWrite"
		Me.btnWrite.Size = New System.Drawing.Size(108, 32)
		Me.btnWrite.TabIndex = 6
		Me.btnWrite.Text = "バイナリ書込"
		Me.btnWrite.UseVisualStyleBackColor = True
		'
		'dgvSharedMemory
		'
		Me.dgvSharedMemory.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
					Or System.Windows.Forms.AnchorStyles.Left) _
					Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.dgvSharedMemory.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvSharedMemory.Location = New System.Drawing.Point(12, 56)
		Me.dgvSharedMemory.Name = "dgvSharedMemory"
		Me.dgvSharedMemory.RowTemplate.Height = 21
		Me.dgvSharedMemory.Size = New System.Drawing.Size(616, 252)
		Me.dgvSharedMemory.TabIndex = 8
		'
		'lblBaseFolder
		'
		Me.lblBaseFolder.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
					Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.lblBaseFolder.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
		Me.lblBaseFolder.Location = New System.Drawing.Point(12, 28)
		Me.lblBaseFolder.Name = "lblBaseFolder"
		Me.lblBaseFolder.Size = New System.Drawing.Size(616, 23)
		Me.lblBaseFolder.TabIndex = 9
		Me.lblBaseFolder.Text = "c:\"
		Me.lblBaseFolder.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'Label1
		'
		Me.Label1.AutoSize = True
		Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label1.Location = New System.Drawing.Point(12, 8)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(191, 16)
		Me.Label1.TabIndex = 10
		Me.Label1.Text = "共有メモリ保存・読込フォルダ"
		'
		'btnBaseFolder
		'
		Me.btnBaseFolder.Location = New System.Drawing.Point(208, 4)
		Me.btnBaseFolder.Name = "btnBaseFolder"
		Me.btnBaseFolder.Size = New System.Drawing.Size(75, 23)
		Me.btnBaseFolder.TabIndex = 11
		Me.btnBaseFolder.Text = "フォルダ指定"
		Me.btnBaseFolder.UseVisualStyleBackColor = True
		'
		'frmFile
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.ClientSize = New System.Drawing.Size(635, 349)
		Me.Controls.Add(Me.btnBaseFolder)
		Me.Controls.Add(Me.Label1)
		Me.Controls.Add(Me.lblBaseFolder)
		Me.Controls.Add(Me.dgvSharedMemory)
		Me.Controls.Add(Me.btnWrite)
		Me.Controls.Add(Me.btnRead)
		Me.Name = "frmFile"
		Me.Text = "ファイル操作"
		CType(Me.dgvSharedMemory, System.ComponentModel.ISupportInitialize).EndInit()
		Me.ResumeLayout(False)
		Me.PerformLayout()

	End Sub
	Friend WithEvents btnRead As System.Windows.Forms.Button
	Friend WithEvents btnWrite As System.Windows.Forms.Button
	Friend WithEvents dgvSharedMemory As System.Windows.Forms.DataGridView
	Friend WithEvents lblBaseFolder As System.Windows.Forms.Label
	Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents btnBaseFolder As System.Windows.Forms.Button
End Class
