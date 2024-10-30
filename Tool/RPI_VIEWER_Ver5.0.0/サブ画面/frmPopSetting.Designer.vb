<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopSetting
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
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.btnChageX_Init = New System.Windows.Forms.Button()
        Me.btnChageX_Val = New System.Windows.Forms.Button()
        Me.btnChageY_Init = New System.Windows.Forms.Button()
        Me.nudResX_Val = New System.Windows.Forms.NumericUpDown()
        Me.btnChageY_Val = New System.Windows.Forms.Button()
        Me.btnChageX_Dev = New System.Windows.Forms.Button()
        Me.nudResY_Val = New System.Windows.Forms.NumericUpDown()
        Me.nudResX_Dev = New System.Windows.Forms.NumericUpDown()
        Me.btnChageY_Dev = New System.Windows.Forms.Button()
        Me.nudResY_Dev = New System.Windows.Forms.NumericUpDown()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.lblBaseResX = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.lblBaseResY = New System.Windows.Forms.Label()
        Me.lblNowResX = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.lblNowResY = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        CType(Me.nudResX_Val, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudResY_Val, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudResX_Dev, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudResY_Dev, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Location = New System.Drawing.Point(8, 36)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(36, 20)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "縦"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.Label7)
        Me.GroupBox1.Controls.Add(Me.btnChageX_Init)
        Me.GroupBox1.Controls.Add(Me.btnChageX_Val)
        Me.GroupBox1.Controls.Add(Me.btnChageY_Init)
        Me.GroupBox1.Controls.Add(Me.nudResX_Val)
        Me.GroupBox1.Controls.Add(Me.btnChageY_Val)
        Me.GroupBox1.Controls.Add(Me.btnChageX_Dev)
        Me.GroupBox1.Controls.Add(Me.nudResY_Val)
        Me.GroupBox1.Controls.Add(Me.nudResX_Dev)
        Me.GroupBox1.Controls.Add(Me.btnChageY_Dev)
        Me.GroupBox1.Controls.Add(Me.nudResY_Dev)
        Me.GroupBox1.Controls.Add(Me.Label4)
        Me.GroupBox1.Controls.Add(Me.Label3)
        Me.GroupBox1.Controls.Add(Me.lblBaseResX)
        Me.GroupBox1.Controls.Add(Me.Label6)
        Me.GroupBox1.Controls.Add(Me.lblBaseResY)
        Me.GroupBox1.Controls.Add(Me.lblNowResX)
        Me.GroupBox1.Controls.Add(Me.Label2)
        Me.GroupBox1.Controls.Add(Me.Label5)
        Me.GroupBox1.Controls.Add(Me.lblNowResY)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Location = New System.Drawing.Point(4, 4)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(488, 116)
        Me.GroupBox1.TabIndex = 1
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "分解能指定"
        '
        'btnChageX_Init
        '
        Me.btnChageX_Init.Location = New System.Drawing.Point(412, 60)
        Me.btnChageX_Init.Name = "btnChageX_Init"
        Me.btnChageX_Init.Size = New System.Drawing.Size(68, 20)
        Me.btnChageX_Init.TabIndex = 2
        Me.btnChageX_Init.Text = "ﾃﾞﾌｫﾙﾄ値"
        Me.btnChageX_Init.UseVisualStyleBackColor = True
        '
        'btnChageX_Val
        '
        Me.btnChageX_Val.Location = New System.Drawing.Point(368, 60)
        Me.btnChageX_Val.Name = "btnChageX_Val"
        Me.btnChageX_Val.Size = New System.Drawing.Size(40, 20)
        Me.btnChageX_Val.TabIndex = 2
        Me.btnChageX_Val.Text = "変更"
        Me.btnChageX_Val.UseVisualStyleBackColor = True
        '
        'btnChageY_Init
        '
        Me.btnChageY_Init.Location = New System.Drawing.Point(412, 36)
        Me.btnChageY_Init.Name = "btnChageY_Init"
        Me.btnChageY_Init.Size = New System.Drawing.Size(68, 20)
        Me.btnChageY_Init.TabIndex = 2
        Me.btnChageY_Init.Text = "ﾃﾞﾌｫﾙﾄ値"
        Me.btnChageY_Init.UseVisualStyleBackColor = True
        '
        'nudResX_Val
        '
        Me.nudResX_Val.DecimalPlaces = 6
        Me.nudResX_Val.Increment = New Decimal(New Integer() {1, 0, 0, 131072})
        Me.nudResX_Val.Location = New System.Drawing.Point(296, 60)
        Me.nudResX_Val.Name = "nudResX_Val"
        Me.nudResX_Val.Size = New System.Drawing.Size(72, 19)
        Me.nudResX_Val.TabIndex = 1
        Me.nudResX_Val.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudResX_Val.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'btnChageY_Val
        '
        Me.btnChageY_Val.Location = New System.Drawing.Point(368, 36)
        Me.btnChageY_Val.Name = "btnChageY_Val"
        Me.btnChageY_Val.Size = New System.Drawing.Size(40, 20)
        Me.btnChageY_Val.TabIndex = 2
        Me.btnChageY_Val.Text = "変更"
        Me.btnChageY_Val.UseVisualStyleBackColor = True
        '
        'btnChageX_Dev
        '
        Me.btnChageX_Dev.Location = New System.Drawing.Point(256, 60)
        Me.btnChageX_Dev.Name = "btnChageX_Dev"
        Me.btnChageX_Dev.Size = New System.Drawing.Size(40, 20)
        Me.btnChageX_Dev.TabIndex = 2
        Me.btnChageX_Dev.Text = "変更"
        Me.btnChageX_Dev.UseVisualStyleBackColor = True
        '
        'nudResY_Val
        '
        Me.nudResY_Val.DecimalPlaces = 6
        Me.nudResY_Val.Increment = New Decimal(New Integer() {1, 0, 0, 131072})
        Me.nudResY_Val.Location = New System.Drawing.Point(296, 36)
        Me.nudResY_Val.Name = "nudResY_Val"
        Me.nudResY_Val.Size = New System.Drawing.Size(72, 19)
        Me.nudResY_Val.TabIndex = 1
        Me.nudResY_Val.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudResY_Val.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'nudResX_Dev
        '
        Me.nudResX_Dev.DecimalPlaces = 2
        Me.nudResX_Dev.Increment = New Decimal(New Integer() {1, 0, 0, 131072})
        Me.nudResX_Dev.Location = New System.Drawing.Point(184, 60)
        Me.nudResX_Dev.Name = "nudResX_Dev"
        Me.nudResX_Dev.Size = New System.Drawing.Size(72, 19)
        Me.nudResX_Dev.TabIndex = 1
        Me.nudResX_Dev.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudResX_Dev.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'btnChageY_Dev
        '
        Me.btnChageY_Dev.Location = New System.Drawing.Point(256, 36)
        Me.btnChageY_Dev.Name = "btnChageY_Dev"
        Me.btnChageY_Dev.Size = New System.Drawing.Size(40, 20)
        Me.btnChageY_Dev.TabIndex = 2
        Me.btnChageY_Dev.Text = "変更"
        Me.btnChageY_Dev.UseVisualStyleBackColor = True
        '
        'nudResY_Dev
        '
        Me.nudResY_Dev.DecimalPlaces = 2
        Me.nudResY_Dev.Increment = New Decimal(New Integer() {1, 0, 0, 131072})
        Me.nudResY_Dev.Location = New System.Drawing.Point(184, 36)
        Me.nudResY_Dev.Name = "nudResY_Dev"
        Me.nudResY_Dev.Size = New System.Drawing.Size(72, 19)
        Me.nudResY_Dev.TabIndex = 1
        Me.nudResY_Dev.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudResY_Dev.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Location = New System.Drawing.Point(296, 20)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(112, 16)
        Me.Label4.TabIndex = 0
        Me.Label4.Text = "値指定 [mm/pix]"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Location = New System.Drawing.Point(184, 20)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(112, 16)
        Me.Label3.TabIndex = 0
        Me.Label3.Text = "倍率指定 [率]"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'lblBaseResX
        '
        Me.lblBaseResX.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblBaseResX.Location = New System.Drawing.Point(48, 60)
        Me.lblBaseResX.Name = "lblBaseResX"
        Me.lblBaseResX.Size = New System.Drawing.Size(68, 20)
        Me.lblBaseResX.TabIndex = 0
        Me.lblBaseResX.Text = "0.123456"
        Me.lblBaseResX.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Location = New System.Drawing.Point(48, 20)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(68, 16)
        Me.Label6.TabIndex = 0
        Me.Label6.Text = "元[mm/pix]"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'lblBaseResY
        '
        Me.lblBaseResY.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblBaseResY.Location = New System.Drawing.Point(48, 36)
        Me.lblBaseResY.Name = "lblBaseResY"
        Me.lblBaseResY.Size = New System.Drawing.Size(68, 20)
        Me.lblBaseResY.TabIndex = 0
        Me.lblBaseResY.Text = "0.123456"
        Me.lblBaseResY.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'lblNowResX
        '
        Me.lblNowResX.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblNowResX.Location = New System.Drawing.Point(116, 60)
        Me.lblNowResX.Name = "lblNowResX"
        Me.lblNowResX.Size = New System.Drawing.Size(68, 20)
        Me.lblNowResX.TabIndex = 0
        Me.lblNowResX.Text = "0.123456"
        Me.lblNowResX.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Location = New System.Drawing.Point(116, 20)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(68, 16)
        Me.Label2.TabIndex = 0
        Me.Label2.Text = "現状"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label5.Location = New System.Drawing.Point(8, 60)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(36, 20)
        Me.Label5.TabIndex = 0
        Me.Label5.Text = "横"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblNowResY
        '
        Me.lblNowResY.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblNowResY.Location = New System.Drawing.Point(116, 36)
        Me.lblNowResY.Name = "lblNowResY"
        Me.lblNowResY.Size = New System.Drawing.Size(68, 20)
        Me.lblNowResY.TabIndex = 0
        Me.lblNowResY.Text = "0.123456"
        Me.lblNowResY.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(12, 84)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(455, 24)
        Me.Label7.TabIndex = 3
        Me.Label7.Text = "※ 強制的に分解能を変更します。" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "　　そのため、疵情報を使った機能（散布図や疵マップのジャンプ機能等）は正常に機能しません。"
        '
        'frmPopSetting
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(494, 124)
        Me.Controls.Add(Me.GroupBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopSetting"
        Me.ShowIcon = False
        Me.Text = "各種設定"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.nudResX_Val, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudResY_Val, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudResX_Dev, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudResY_Dev, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents lblNowResY As System.Windows.Forms.Label
    Friend WithEvents btnChageY_Dev As System.Windows.Forms.Button
    Friend WithEvents nudResY_Dev As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents lblBaseResY As System.Windows.Forms.Label
    Friend WithEvents btnChageY_Val As System.Windows.Forms.Button
    Friend WithEvents nudResY_Val As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnChageY_Init As System.Windows.Forms.Button
    Friend WithEvents btnChageX_Init As System.Windows.Forms.Button
    Friend WithEvents btnChageX_Val As System.Windows.Forms.Button
    Friend WithEvents nudResX_Val As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnChageX_Dev As System.Windows.Forms.Button
    Friend WithEvents nudResX_Dev As System.Windows.Forms.NumericUpDown
    Friend WithEvents lblBaseResX As System.Windows.Forms.Label
    Friend WithEvents lblNowResX As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
End Class
