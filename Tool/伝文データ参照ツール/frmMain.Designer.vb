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
        Me.btnPro = New System.Windows.Forms.Button()
        Me.lblTitle = New System.Windows.Forms.Label()
        Me.btnFF = New System.Windows.Forms.Button()
        Me.btnSaisun = New System.Windows.Forms.Button()
        Me.btnCut = New System.Windows.Forms.Button()
        Me.btnFFRecv = New System.Windows.Forms.Button()
        Me.btnCoilInfo = New System.Windows.Forms.Button()
        Me.btnFFStstus = New System.Windows.Forms.Button()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnPro
        '
        Me.btnPro.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnPro.Location = New System.Drawing.Point(416, 22)
        Me.btnPro.Name = "btnPro"
        Me.btnPro.Size = New System.Drawing.Size(128, 44)
        Me.btnPro.TabIndex = 0
        Me.btnPro.Text = "検査実績プロコン"
        Me.btnPro.UseVisualStyleBackColor = True
        '
        'lblTitle
        '
        Me.lblTitle.Font = New System.Drawing.Font("MS UI Gothic", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblTitle.Location = New System.Drawing.Point(28, 12)
        Me.lblTitle.Name = "lblTitle"
        Me.lblTitle.Size = New System.Drawing.Size(264, 40)
        Me.lblTitle.TabIndex = 1
        Me.lblTitle.Text = "伝文参照ツール"
        '
        'btnFF
        '
        Me.btnFF.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnFF.Location = New System.Drawing.Point(144, 22)
        Me.btnFF.Name = "btnFF"
        Me.btnFF.Size = New System.Drawing.Size(128, 44)
        Me.btnFF.TabIndex = 2
        Me.btnFF.Text = "検査実績FF"
        Me.btnFF.UseVisualStyleBackColor = True
        '
        'btnSaisun
        '
        Me.btnSaisun.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSaisun.Location = New System.Drawing.Point(144, 22)
        Me.btnSaisun.Name = "btnSaisun"
        Me.btnSaisun.Size = New System.Drawing.Size(128, 44)
        Me.btnSaisun.TabIndex = 3
        Me.btnSaisun.Text = "採寸実績伝文"
        Me.btnSaisun.UseVisualStyleBackColor = True
        '
        'btnCut
        '
        Me.btnCut.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnCut.Location = New System.Drawing.Point(280, 22)
        Me.btnCut.Name = "btnCut"
        Me.btnCut.Size = New System.Drawing.Size(128, 44)
        Me.btnCut.TabIndex = 4
        Me.btnCut.Text = "カット実績伝文"
        Me.btnCut.UseVisualStyleBackColor = True
        '
        'btnFFRecv
        '
        Me.btnFFRecv.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnFFRecv.Location = New System.Drawing.Point(8, 22)
        Me.btnFFRecv.Name = "btnFFRecv"
        Me.btnFFRecv.Size = New System.Drawing.Size(128, 44)
        Me.btnFFRecv.TabIndex = 7
        Me.btnFFRecv.Text = "前工程実績FF"
        Me.btnFFRecv.UseVisualStyleBackColor = True
        '
        'btnCoilInfo
        '
        Me.btnCoilInfo.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnCoilInfo.Location = New System.Drawing.Point(8, 22)
        Me.btnCoilInfo.Name = "btnCoilInfo"
        Me.btnCoilInfo.Size = New System.Drawing.Size(128, 44)
        Me.btnCoilInfo.TabIndex = 8
        Me.btnCoilInfo.Text = "コイル情報伝文"
        Me.btnCoilInfo.UseVisualStyleBackColor = True
        '
        'btnFFStstus
        '
        Me.btnFFStstus.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnFFStstus.Location = New System.Drawing.Point(280, 22)
        Me.btnFFStstus.Name = "btnFFStstus"
        Me.btnFFStstus.Size = New System.Drawing.Size(128, 44)
        Me.btnFFStstus.TabIndex = 9
        Me.btnFFStstus.Text = "検査状態FF"
        Me.btnFFStstus.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.btnPro)
        Me.GroupBox1.Controls.Add(Me.btnCut)
        Me.GroupBox1.Controls.Add(Me.btnCoilInfo)
        Me.GroupBox1.Controls.Add(Me.btnSaisun)
        Me.GroupBox1.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(24, 52)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(552, 76)
        Me.GroupBox1.TabIndex = 10
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "対プロコン"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.btnFFRecv)
        Me.GroupBox2.Controls.Add(Me.btnFF)
        Me.GroupBox2.Controls.Add(Me.btnFFStstus)
        Me.GroupBox2.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(24, 132)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(552, 76)
        Me.GroupBox2.TabIndex = 11
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "対ＦＦサーバ"
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(595, 228)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.lblTitle)
        Me.Name = "frmMain"
        Me.Text = "伝文参照ツール"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents btnPro As System.Windows.Forms.Button
    Friend WithEvents lblTitle As System.Windows.Forms.Label
    Friend WithEvents btnFF As System.Windows.Forms.Button
    Friend WithEvents btnSaisun As System.Windows.Forms.Button
    Friend WithEvents btnCut As System.Windows.Forms.Button
    Friend WithEvents btnFFRecv As System.Windows.Forms.Button
    Friend WithEvents btnCoilInfo As System.Windows.Forms.Button
    Friend WithEvents btnFFStstus As System.Windows.Forms.Button
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox

End Class
