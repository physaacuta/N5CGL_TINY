<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmDenbun
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
        Me.btnPro = New System.Windows.Forms.Button
        Me.lblTitle = New System.Windows.Forms.Label
        Me.btnFF = New System.Windows.Forms.Button
        Me.btnSaisun = New System.Windows.Forms.Button
        Me.btnCut = New System.Windows.Forms.Button
        Me.btnProZi = New System.Windows.Forms.Button
        Me.btnFFMake = New System.Windows.Forms.Button
        Me.btnFFRecv = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'btnPro
        '
        Me.btnPro.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnPro.Location = New System.Drawing.Point(24, 76)
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
        Me.btnFF.Location = New System.Drawing.Point(164, 76)
        Me.btnFF.Name = "btnFF"
        Me.btnFF.Size = New System.Drawing.Size(128, 44)
        Me.btnFF.TabIndex = 2
        Me.btnFF.Text = "検査実績FF"
        Me.btnFF.UseVisualStyleBackColor = True
        '
        'btnSaisun
        '
        Me.btnSaisun.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSaisun.Location = New System.Drawing.Point(24, 132)
        Me.btnSaisun.Name = "btnSaisun"
        Me.btnSaisun.Size = New System.Drawing.Size(128, 44)
        Me.btnSaisun.TabIndex = 3
        Me.btnSaisun.Text = "採寸実績伝文"
        Me.btnSaisun.UseVisualStyleBackColor = True
        '
        'btnCut
        '
        Me.btnCut.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnCut.Location = New System.Drawing.Point(164, 132)
        Me.btnCut.Name = "btnCut"
        Me.btnCut.Size = New System.Drawing.Size(128, 44)
        Me.btnCut.TabIndex = 4
        Me.btnCut.Text = "カット実績伝文"
        Me.btnCut.UseVisualStyleBackColor = True
        '
        'btnProZi
        '
        Me.btnProZi.Location = New System.Drawing.Point(24, 184)
        Me.btnProZi.Name = "btnProZi"
        Me.btnProZi.Size = New System.Drawing.Size(128, 44)
        Me.btnProZi.TabIndex = 5
        Me.btnProZi.Text = "プロコンテスト伝文作成"
        Me.btnProZi.UseVisualStyleBackColor = True
        '
        'btnFFMake
        '
        Me.btnFFMake.Location = New System.Drawing.Point(168, 184)
        Me.btnFFMake.Name = "btnFFMake"
        Me.btnFFMake.Size = New System.Drawing.Size(124, 44)
        Me.btnFFMake.TabIndex = 6
        Me.btnFFMake.Text = "FFテスト伝文作成"
        Me.btnFFMake.UseVisualStyleBackColor = True
        '
        'btnFFRecv
        '
        Me.btnFFRecv.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnFFRecv.Location = New System.Drawing.Point(300, 76)
        Me.btnFFRecv.Name = "btnFFRecv"
        Me.btnFFRecv.Size = New System.Drawing.Size(128, 44)
        Me.btnFFRecv.TabIndex = 7
        Me.btnFFRecv.Text = "前工程実績FF"
        Me.btnFFRecv.UseVisualStyleBackColor = True
        '
        'frmDenbun
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(463, 267)
        Me.Controls.Add(Me.btnFFRecv)
        Me.Controls.Add(Me.btnFFMake)
        Me.Controls.Add(Me.btnProZi)
        Me.Controls.Add(Me.btnCut)
        Me.Controls.Add(Me.btnSaisun)
        Me.Controls.Add(Me.btnFF)
        Me.Controls.Add(Me.lblTitle)
        Me.Controls.Add(Me.btnPro)
        Me.Name = "frmDenbun"
        Me.Text = "伝文参照ツール"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents btnPro As System.Windows.Forms.Button
    Friend WithEvents lblTitle As System.Windows.Forms.Label
    Friend WithEvents btnFF As System.Windows.Forms.Button
    Friend WithEvents btnSaisun As System.Windows.Forms.Button
    Friend WithEvents btnCut As System.Windows.Forms.Button
    Friend WithEvents btnProZi As System.Windows.Forms.Button
    Friend WithEvents btnFFMake As System.Windows.Forms.Button
    Friend WithEvents btnFFRecv As System.Windows.Forms.Button

End Class
