<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmChat
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
		Me.components = New System.ComponentModel.Container
		Me.txtSendMsg = New System.Windows.Forms.TextBox
		Me.Label1 = New System.Windows.Forms.Label
		Me.btnSend = New System.Windows.Forms.Button
		Me.txtRecv = New System.Windows.Forms.TextBox
		Me.cmbSendIp = New System.Windows.Forms.ComboBox
		Me.timIc = New System.Windows.Forms.Timer(Me.components)
		Me.btnClear = New System.Windows.Forms.Button
		Me.btnClose = New System.Windows.Forms.Button
		Me.btnExplorer = New System.Windows.Forms.Button
		Me.btnRemoto = New System.Windows.Forms.Button
		Me.GroupBox1 = New System.Windows.Forms.GroupBox
		Me.btnExecCommand = New System.Windows.Forms.Button
		Me.btnExecFile = New System.Windows.Forms.Button
		Me.Label2 = New System.Windows.Forms.Label
		Me.txtExecItem = New System.Windows.Forms.TextBox
		Me.btnRemotoNon = New System.Windows.Forms.Button
		Me.btnRemoto2 = New System.Windows.Forms.Button
		Me.GroupBox1.SuspendLayout()
		Me.SuspendLayout()
		'
		'txtSendMsg
		'
		Me.txtSendMsg.AcceptsTab = True
		Me.txtSendMsg.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
					Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.txtSendMsg.ImeMode = System.Windows.Forms.ImeMode.[On]
		Me.txtSendMsg.Location = New System.Drawing.Point(4, 40)
		Me.txtSendMsg.Multiline = True
		Me.txtSendMsg.Name = "txtSendMsg"
		Me.txtSendMsg.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
		Me.txtSendMsg.Size = New System.Drawing.Size(348, 44)
		Me.txtSendMsg.TabIndex = 10
		Me.txtSendMsg.WordWrap = False
		'
		'Label1
		'
		Me.Label1.Location = New System.Drawing.Point(0, 4)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(44, 12)
		Me.Label1.TabIndex = 1
		Me.Label1.Text = "対象PC"
		'
		'btnSend
		'
		Me.btnSend.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnSend.Location = New System.Drawing.Point(352, 40)
		Me.btnSend.Name = "btnSend"
		Me.btnSend.Size = New System.Drawing.Size(40, 44)
		Me.btnSend.TabIndex = 3
		Me.btnSend.Text = "Msg送信"
		Me.btnSend.UseVisualStyleBackColor = True
		'
		'txtRecv
		'
		Me.txtRecv.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
					Or System.Windows.Forms.AnchorStyles.Left) _
					Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.txtRecv.Location = New System.Drawing.Point(0, 164)
		Me.txtRecv.Multiline = True
		Me.txtRecv.Name = "txtRecv"
		Me.txtRecv.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
		Me.txtRecv.Size = New System.Drawing.Size(400, 59)
		Me.txtRecv.TabIndex = 0
		Me.txtRecv.WordWrap = False
		'
		'cmbSendIp
		'
		Me.cmbSendIp.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.cmbSendIp.FormattingEnabled = True
		Me.cmbSendIp.ImeMode = System.Windows.Forms.ImeMode.Disable
		Me.cmbSendIp.Items.AddRange(New Object() {"192.9.200."})
		Me.cmbSendIp.Location = New System.Drawing.Point(44, 0)
		Me.cmbSendIp.Name = "cmbSendIp"
		Me.cmbSendIp.Size = New System.Drawing.Size(240, 20)
		Me.cmbSendIp.TabIndex = 6
		Me.cmbSendIp.Text = "192.9.200."
		'
		'timIc
		'
		Me.timIc.Interval = 600000
		'
		'btnClear
		'
		Me.btnClear.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnClear.Location = New System.Drawing.Point(288, 0)
		Me.btnClear.Name = "btnClear"
		Me.btnClear.Size = New System.Drawing.Size(84, 20)
		Me.btnClear.TabIndex = 3
		Me.btnClear.Text = "受信Msg消去"
		Me.btnClear.UseVisualStyleBackColor = True
		'
		'btnClose
		'
		Me.btnClose.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnClose.Location = New System.Drawing.Point(375, 0)
		Me.btnClose.Name = "btnClose"
		Me.btnClose.Size = New System.Drawing.Size(24, 20)
		Me.btnClose.TabIndex = 3
		Me.btnClose.Text = "×"
		Me.btnClose.UseVisualStyleBackColor = True
		'
		'btnExplorer
		'
		Me.btnExplorer.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnExplorer.Location = New System.Drawing.Point(4, 16)
		Me.btnExplorer.Name = "btnExplorer"
		Me.btnExplorer.Size = New System.Drawing.Size(104, 20)
		Me.btnExplorer.TabIndex = 3
		Me.btnExplorer.Text = "Explorer 実行"
		Me.btnExplorer.UseVisualStyleBackColor = True
		'
		'btnRemoto
		'
		Me.btnRemoto.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnRemoto.Location = New System.Drawing.Point(236, 16)
		Me.btnRemoto.Name = "btnRemoto"
		Me.btnRemoto.Size = New System.Drawing.Size(88, 20)
		Me.btnRemoto.TabIndex = 3
		Me.btnRemoto.Text = "旧ｺﾝｿｰﾙ接続"
		Me.btnRemoto.UseVisualStyleBackColor = True
		'
		'GroupBox1
		'
		Me.GroupBox1.Controls.Add(Me.btnExecCommand)
		Me.GroupBox1.Controls.Add(Me.btnExecFile)
		Me.GroupBox1.Controls.Add(Me.Label2)
		Me.GroupBox1.Controls.Add(Me.txtExecItem)
		Me.GroupBox1.Controls.Add(Me.btnExplorer)
		Me.GroupBox1.Controls.Add(Me.btnRemotoNon)
		Me.GroupBox1.Controls.Add(Me.btnRemoto2)
		Me.GroupBox1.Controls.Add(Me.btnRemoto)
		Me.GroupBox1.Controls.Add(Me.btnSend)
		Me.GroupBox1.Controls.Add(Me.txtSendMsg)
		Me.GroupBox1.Location = New System.Drawing.Point(0, 24)
		Me.GroupBox1.Name = "GroupBox1"
		Me.GroupBox1.Size = New System.Drawing.Size(396, 136)
		Me.GroupBox1.TabIndex = 12
		Me.GroupBox1.TabStop = False
		Me.GroupBox1.Text = "選択している対象PCに対して実行"
		'
		'btnExecCommand
		'
		Me.btnExecCommand.Location = New System.Drawing.Point(280, 88)
		Me.btnExecCommand.Name = "btnExecCommand"
		Me.btnExecCommand.Size = New System.Drawing.Size(112, 20)
		Me.btnExecCommand.TabIndex = 13
		Me.btnExecCommand.Text = "指定コマンドを実行"
		Me.btnExecCommand.UseVisualStyleBackColor = True
		'
		'btnExecFile
		'
		Me.btnExecFile.Location = New System.Drawing.Point(112, 88)
		Me.btnExecFile.Name = "btnExecFile"
		Me.btnExecFile.Size = New System.Drawing.Size(164, 20)
		Me.btnExecFile.TabIndex = 13
		Me.btnExecFile.Text = "指定ファイルを実行（フルパス）"
		Me.btnExecFile.UseVisualStyleBackColor = True
		'
		'Label2
		'
		Me.Label2.AutoSize = True
		Me.Label2.BackColor = System.Drawing.Color.Transparent
		Me.Label2.Location = New System.Drawing.Point(8, 88)
		Me.Label2.Name = "Label2"
		Me.Label2.Size = New System.Drawing.Size(90, 12)
		Me.Label2.TabIndex = 12
		Me.Label2.Text = "対象PC内で実行"
		'
		'txtExecItem
		'
		Me.txtExecItem.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.txtExecItem.Location = New System.Drawing.Point(4, 108)
		Me.txtExecItem.Name = "txtExecItem"
		Me.txtExecItem.Size = New System.Drawing.Size(388, 23)
		Me.txtExecItem.TabIndex = 11
		'
		'btnRemotoNon
		'
		Me.btnRemotoNon.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnRemotoNon.Location = New System.Drawing.Point(124, 16)
		Me.btnRemotoNon.Name = "btnRemotoNon"
		Me.btnRemotoNon.Size = New System.Drawing.Size(112, 20)
		Me.btnRemotoNon.TabIndex = 3
		Me.btnRemotoNon.Text = "ﾘﾓｰﾄﾃﾞｽｸﾄｯﾌﾟ接続"
		Me.btnRemotoNon.UseVisualStyleBackColor = True
		'
		'btnRemoto2
		'
		Me.btnRemoto2.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnRemoto2.Location = New System.Drawing.Point(324, 16)
		Me.btnRemoto2.Name = "btnRemoto2"
		Me.btnRemoto2.Size = New System.Drawing.Size(68, 20)
		Me.btnRemoto2.TabIndex = 3
		Me.btnRemoto2.Text = "新ｺﾝｿｰﾙ"
		Me.btnRemoto2.UseVisualStyleBackColor = True
		'
		'frmChat
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.ClientSize = New System.Drawing.Size(398, 223)
		Me.Controls.Add(Me.GroupBox1)
		Me.Controls.Add(Me.cmbSendIp)
		Me.Controls.Add(Me.txtRecv)
		Me.Controls.Add(Me.btnClose)
		Me.Controls.Add(Me.btnClear)
		Me.Controls.Add(Me.Label1)
		Me.MaximizeBox = False
		Me.MinimizeBox = False
		Me.Name = "frmChat"
		Me.ShowInTaskbar = False
		Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
		Me.Text = "簡易チャット （NOA回線経由可能） 使用ポート12346 Ver2.3"
		Me.GroupBox1.ResumeLayout(False)
		Me.GroupBox1.PerformLayout()
		Me.ResumeLayout(False)
		Me.PerformLayout()

	End Sub
	Friend WithEvents txtSendMsg As System.Windows.Forms.TextBox
	Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents btnSend As System.Windows.Forms.Button
	Friend WithEvents txtRecv As System.Windows.Forms.TextBox
	Friend WithEvents cmbSendIp As System.Windows.Forms.ComboBox
	Friend WithEvents timIc As System.Windows.Forms.Timer
	Friend WithEvents btnClear As System.Windows.Forms.Button
	Friend WithEvents btnClose As System.Windows.Forms.Button
	Friend WithEvents btnExplorer As System.Windows.Forms.Button
	Friend WithEvents btnRemoto As System.Windows.Forms.Button
	Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
	Friend WithEvents txtExecItem As System.Windows.Forms.TextBox
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents btnExecCommand As System.Windows.Forms.Button
	Friend WithEvents btnExecFile As System.Windows.Forms.Button
	Friend WithEvents btnRemotoNon As System.Windows.Forms.Button
	Friend WithEvents btnRemoto2 As System.Windows.Forms.Button
End Class
