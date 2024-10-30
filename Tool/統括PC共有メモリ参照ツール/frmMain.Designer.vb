<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
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
        Me.Label1 = New System.Windows.Forms.Label()
        Me.btnCoil = New System.Windows.Forms.Button()
        Me.btnCamera = New System.Windows.Forms.Button()
        Me.btnSeqmem = New System.Windows.Forms.Button()
        Me.btnFile = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 24.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(16, 8)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(412, 40)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "統括PC共有メモリ参照ツール"
        '
        'btnCoil
        '
        Me.btnCoil.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnCoil.Location = New System.Drawing.Point(16, 72)
        Me.btnCoil.Name = "btnCoil"
        Me.btnCoil.Size = New System.Drawing.Size(152, 52)
        Me.btnCoil.TabIndex = 2
        Me.btnCoil.Text = "コイル情報受け渡し"
        Me.btnCoil.UseVisualStyleBackColor = True
        '
        'btnCamera
        '
        Me.btnCamera.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnCamera.Location = New System.Drawing.Point(336, 72)
        Me.btnCamera.Name = "btnCamera"
        Me.btnCamera.Size = New System.Drawing.Size(152, 52)
        Me.btnCamera.TabIndex = 7
        Me.btnCamera.Text = "カメラコントローラ情報"
        Me.btnCamera.UseVisualStyleBackColor = True
        '
        'btnSeqmem
        '
        Me.btnSeqmem.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSeqmem.Location = New System.Drawing.Point(176, 72)
        Me.btnSeqmem.Name = "btnSeqmem"
        Me.btnSeqmem.Size = New System.Drawing.Size(152, 52)
        Me.btnSeqmem.TabIndex = 6
        Me.btnSeqmem.Text = "データメモリ情報"
        Me.btnSeqmem.UseVisualStyleBackColor = True
        '
        'btnFile
        '
        Me.btnFile.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnFile.Location = New System.Drawing.Point(336, 130)
        Me.btnFile.Name = "btnFile"
        Me.btnFile.Size = New System.Drawing.Size(152, 52)
        Me.btnFile.TabIndex = 27
        Me.btnFile.Text = "ファイル操作"
        Me.btnFile.UseVisualStyleBackColor = True
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(498, 355)
        Me.Controls.Add(Me.btnFile)
        Me.Controls.Add(Me.btnCamera)
        Me.Controls.Add(Me.btnSeqmem)
        Me.Controls.Add(Me.btnCoil)
        Me.Controls.Add(Me.Label1)
        Me.Name = "frmMain"
        Me.Text = "統括PC共有メモリ参照ツール"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents btnCoil As System.Windows.Forms.Button
	Friend WithEvents btnCamera As System.Windows.Forms.Button
	Friend WithEvents btnSeqmem As System.Windows.Forms.Button
    Friend WithEvents btnFile As Button
End Class
