<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
	Inherits System.Windows.Forms.Form

	'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
	<System.Diagnostics.DebuggerNonUserCode()> _
	Protected Overrides Sub Dispose(ByVal disposing As Boolean)
		If disposing AndAlso components IsNot Nothing Then
			components.Dispose()
		End If
		MyBase.Dispose(disposing)
	End Sub

	'Windows フォーム デザイナで必要です。
	Private components As System.ComponentModel.IContainer

	'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
	'Windows フォーム デザイナを使用して変更できます。  
	'コード エディタを使って変更しないでください。
	<System.Diagnostics.DebuggerStepThrough()> _
	Private Sub InitializeComponent()
		Me.components = New System.ComponentModel.Container
		Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
		Me.imgList = New System.Windows.Forms.ImageList(Me.components)
		Me.trmCycle = New System.Windows.Forms.Timer(Me.components)
		Me.picTikuseki = New System.Windows.Forms.PictureBox
		Me.picTokatu = New System.Windows.Forms.PictureBox
		Me.Label4 = New System.Windows.Forms.Label
		Me.Label2 = New System.Windows.Forms.Label
		Me.Label1 = New System.Windows.Forms.Label
		Me.ProgressBar1 = New System.Windows.Forms.ProgressBar
		Me.Label3 = New System.Windows.Forms.Label
		CType(Me.picTikuseki, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.picTokatu, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.SuspendLayout()
		'
		'imgList
		'
		Me.imgList.ImageStream = CType(resources.GetObject("imgList.ImageStream"), System.Windows.Forms.ImageListStreamer)
		Me.imgList.TransparentColor = System.Drawing.Color.Transparent
		Me.imgList.Images.SetKeyName(0, "OFF.ICO")
		Me.imgList.Images.SetKeyName(1, "ON.ICO")
		'
		'trmCycle
		'
		Me.trmCycle.Enabled = True
		'
		'picTikuseki
		'
		Me.picTikuseki.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.picTikuseki.Location = New System.Drawing.Point(760, 788)
		Me.picTikuseki.Name = "picTikuseki"
		Me.picTikuseki.Size = New System.Drawing.Size(48, 48)
		Me.picTikuseki.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage
		Me.picTikuseki.TabIndex = 8
		Me.picTikuseki.TabStop = False
		'
		'picTokatu
		'
		Me.picTokatu.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.picTokatu.Location = New System.Drawing.Point(760, 712)
		Me.picTokatu.Name = "picTokatu"
		Me.picTokatu.Size = New System.Drawing.Size(48, 48)
		Me.picTokatu.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage
		Me.picTokatu.TabIndex = 9
		Me.picTokatu.TabStop = False
		'
		'Label4
		'
		Me.Label4.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.Label4.AutoSize = True
		Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 27.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label4.ForeColor = System.Drawing.Color.Green
		Me.Label4.Location = New System.Drawing.Point(836, 792)
		Me.Label4.Name = "Label4"
		Me.Label4.Size = New System.Drawing.Size(407, 37)
		Me.Label4.TabIndex = 7
		Me.Label4.Text = "データベース起動完了"
		'
		'Label2
		'
		Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 96.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label2.ForeColor = System.Drawing.Color.Green
		Me.Label2.Location = New System.Drawing.Point(164, 269)
		Me.Label2.Name = "Label2"
		Me.Label2.Size = New System.Drawing.Size(900, 140)
		Me.Label2.TabIndex = 5
		Me.Label2.Text = "イニシャル中"
		'
		'Label1
		'
		Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 96.0!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label1.ForeColor = System.Drawing.Color.Green
		Me.Label1.Location = New System.Drawing.Point(15, 93)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(685, 140)
		Me.Label1.TabIndex = 6
		Me.Label1.Text = "システム"
		'
		'ProgressBar1
		'
		Me.ProgressBar1.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.ProgressBar1.BackColor = System.Drawing.SystemColors.Control
		Me.ProgressBar1.Location = New System.Drawing.Point(740, 872)
		Me.ProgressBar1.Name = "ProgressBar1"
		Me.ProgressBar1.Size = New System.Drawing.Size(504, 32)
		Me.ProgressBar1.Step = 1
		Me.ProgressBar1.Style = System.Windows.Forms.ProgressBarStyle.Continuous
		Me.ProgressBar1.TabIndex = 4
		'
		'Label3
		'
		Me.Label3.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.Label3.AutoSize = True
		Me.Label3.Font = New System.Drawing.Font("ＭＳ 明朝", 27.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label3.ForeColor = System.Drawing.Color.Green
		Me.Label3.Location = New System.Drawing.Point(837, 715)
		Me.Label3.Name = "Label3"
		Me.Label3.Size = New System.Drawing.Size(329, 37)
		Me.Label3.TabIndex = 10
		Me.Label3.Text = "統括機能準備完了"
		'
		'frmMain
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.BackColor = System.Drawing.Color.Black
		Me.ClientSize = New System.Drawing.Size(1274, 999)
		Me.ControlBox = False
		Me.Controls.Add(Me.Label3)
		Me.Controls.Add(Me.picTikuseki)
		Me.Controls.Add(Me.picTokatu)
		Me.Controls.Add(Me.Label4)
		Me.Controls.Add(Me.Label2)
		Me.Controls.Add(Me.Label1)
		Me.Controls.Add(Me.ProgressBar1)
		Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
		Me.Name = "frmMain"
		Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
		Me.Text = "イニシャル待機中画面"
		Me.WindowState = System.Windows.Forms.FormWindowState.Maximized
		CType(Me.picTikuseki, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.picTokatu, System.ComponentModel.ISupportInitialize).EndInit()
		Me.ResumeLayout(False)
		Me.PerformLayout()

	End Sub
	Friend WithEvents imgList As System.Windows.Forms.ImageList
	Friend WithEvents trmCycle As System.Windows.Forms.Timer
	Friend WithEvents picTikuseki As System.Windows.Forms.PictureBox
	Friend WithEvents picTokatu As System.Windows.Forms.PictureBox
	Friend WithEvents Label4 As System.Windows.Forms.Label
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents ProgressBar1 As System.Windows.Forms.ProgressBar
	Friend WithEvents Label3 As System.Windows.Forms.Label

End Class
