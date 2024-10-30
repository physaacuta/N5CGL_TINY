<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPlDspLen
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

    'Windows フォーム デザイナで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
    'Windows フォーム デザイナを使用して変更できます。  
    'コード エディタを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmPlDspLen))
        Me.lblY = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.lblL = New System.Windows.Forms.Label()
        Me.lblX = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.btnSetCalc = New System.Windows.Forms.Button()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.ckbRes = New System.Windows.Forms.CheckBox()
        Me.LabelIcon = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        'lblY
        '
        Me.lblY.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblY.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblY.Location = New System.Drawing.Point(40, 0)
        Me.lblY.Name = "lblY"
        Me.lblY.Size = New System.Drawing.Size(76, 24)
        Me.lblY.TabIndex = 3
        Me.lblY.Text = "1234567"
        Me.lblY.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label5
        '
        Me.Label5.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(120, 4)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(36, 20)
        Me.Label5.TabIndex = 2
        Me.Label5.Text = "mm"
        '
        'Label1
        '
        Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(0, 4)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(32, 16)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "長さ"
        '
        'Label7
        '
        Me.Label7.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.Location = New System.Drawing.Point(0, 60)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(40, 20)
        Me.Label7.TabIndex = 7
        Me.Label7.Text = "斜辺"
        '
        'Label2
        '
        Me.Label2.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(0, 32)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(32, 16)
        Me.Label2.TabIndex = 8
        Me.Label2.Text = "幅"
        '
        'lblL
        '
        Me.lblL.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblL.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblL.Location = New System.Drawing.Point(40, 56)
        Me.lblL.Name = "lblL"
        Me.lblL.Size = New System.Drawing.Size(76, 24)
        Me.lblL.TabIndex = 9
        Me.lblL.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'lblX
        '
        Me.lblX.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblX.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblX.Location = New System.Drawing.Point(40, 28)
        Me.lblX.Name = "lblX"
        Me.lblX.Size = New System.Drawing.Size(76, 24)
        Me.lblX.TabIndex = 4
        Me.lblX.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label9
        '
        Me.Label9.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.Location = New System.Drawing.Point(120, 60)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(36, 20)
        Me.Label9.TabIndex = 5
        Me.Label9.Text = "mm"
        '
        'Label8
        '
        Me.Label8.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(120, 32)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(36, 20)
        Me.Label8.TabIndex = 6
        Me.Label8.Text = "mm"
        '
        'btnSetCalc
        '
        Me.btnSetCalc.Location = New System.Drawing.Point(156, 0)
        Me.btnSetCalc.Name = "btnSetCalc"
        Me.btnSetCalc.Size = New System.Drawing.Size(48, 48)
        Me.btnSetCalc.TabIndex = 10
        Me.btnSetCalc.Text = "長さ" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ｼﾞｬﾝﾌﾟ" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "反映"
        Me.btnSetCalc.UseVisualStyleBackColor = True
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(148, 52)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(60, 11)
        Me.Label3.TabIndex = 12
        Me.Label3.Text = "分解能補正"
        '
        'ckbRes
        '
        Me.ckbRes.Location = New System.Drawing.Point(148, 64)
        Me.ckbRes.Name = "ckbRes"
        Me.ckbRes.Size = New System.Drawing.Size(60, 16)
        Me.ckbRes.TabIndex = 13
        Me.ckbRes.Text = "反映前"
        Me.ckbRes.UseVisualStyleBackColor = True
        '
        'LabelIcon
        '
        Me.LabelIcon.AutoSize = True
        Me.LabelIcon.Image = CType(resources.GetObject("LabelIcon.Image"), System.Drawing.Image)
        Me.LabelIcon.Location = New System.Drawing.Point(0, 0)
        Me.LabelIcon.Name = "LabelIcon"
        Me.LabelIcon.Size = New System.Drawing.Size(0, 12)
        Me.LabelIcon.Visible = False
        Me.LabelIcon.TabIndex = 14
        '
        'frmPlDspLen
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(206, 82)
        Me.ControlBox = False
        Me.Controls.Add(Me.LabelIcon)
        Me.Controls.Add(Me.ckbRes)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.btnSetCalc)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.lblL)
        Me.Controls.Add(Me.lblX)
        Me.Controls.Add(Me.Label9)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.lblY)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.Label1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Name = "frmPlDspLen"
        Me.Text = "長さ計測"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents lblY As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents lblL As System.Windows.Forms.Label
    Friend WithEvents lblX As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents btnSetCalc As System.Windows.Forms.Button
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents ckbRes As System.Windows.Forms.CheckBox
    Friend WithEvents LabelIcon As Label
End Class
