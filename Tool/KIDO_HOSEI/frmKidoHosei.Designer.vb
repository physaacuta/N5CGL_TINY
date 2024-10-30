<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmKidoHosei
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
        Dim grbSetting As System.Windows.Forms.GroupBox
        Me.btnHoseitiRead = New System.Windows.Forms.Button()
        Me.radCor = New System.Windows.Forms.RadioButton()
        Me.lblCorPath = New System.Windows.Forms.Label()
        Me.txtCorPath = New System.Windows.Forms.TextBox()
        Me.radAfter = New System.Windows.Forms.RadioButton()
        Me.radBefore = New System.Windows.Forms.RadioButton()
        Me.btnPicSave = New System.Windows.Forms.Button()
        Me.txtBasePointKido = New System.Windows.Forms.TextBox()
        Me.txtMoveAveraveSize = New System.Windows.Forms.TextBox()
        Me.txtCameraSet = New System.Windows.Forms.TextBox()
        Me.btnHardCopy = New System.Windows.Forms.Button()
        Me.lblMvAve = New System.Windows.Forms.Label()
        Me.lblBP = New System.Windows.Forms.Label()
        Me.lblCamera = New System.Windows.Forms.Label()
        Me.btnExport = New System.Windows.Forms.Button()
        Me.btnKidoCheck = New System.Windows.Forms.Button()
        Me.chkBoxCorrected = New System.Windows.Forms.CheckBox()
        Me.chkBoxHProMvAve = New System.Windows.Forms.CheckBox()
        Me.chkBoxHProRow = New System.Windows.Forms.CheckBox()
        Me.pbxBitMap = New System.Windows.Forms.PictureBox()
        Me.pnlBitMap = New System.Windows.Forms.Panel()
        Me.VScrollBM = New tClassLibrary.tCnt.tCnt_VScrollBar()
        Me.HScrollBM = New tClassLibrary.tCnt.tCnt_HScrollBar()
        Me.lblKido = New System.Windows.Forms.Label()
        Me.lblFrame = New System.Windows.Forms.Label()
        Me.btnRateChg = New System.Windows.Forms.Button()
        Me.lblInform = New System.Windows.Forms.Label()
        Me.pgbProcess = New System.Windows.Forms.ProgressBar()
        Me.lblRateChg = New System.Windows.Forms.Label()
        Me.HScrollDgv = New tClassLibrary.tCnt.tCnt_HScrollBar()
        Me.dgvKido = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.btnRawMax = New System.Windows.Forms.Button()
        Me.btnAveMax = New System.Windows.Forms.Button()
        Me.ofdHoseitiRead = New System.Windows.Forms.OpenFileDialog()
        Me.chkBoxSelect = New System.Windows.Forms.CheckBox()
        grbSetting = New System.Windows.Forms.GroupBox()
        grbSetting.SuspendLayout()
        CType(Me.pbxBitMap, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlBitMap.SuspendLayout()
        CType(Me.dgvKido, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grbSetting
        '
        grbSetting.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        grbSetting.Controls.Add(Me.btnHoseitiRead)
        grbSetting.Controls.Add(Me.radCor)
        grbSetting.Controls.Add(Me.lblCorPath)
        grbSetting.Controls.Add(Me.txtCorPath)
        grbSetting.Controls.Add(Me.radAfter)
        grbSetting.Controls.Add(Me.radBefore)
        grbSetting.Controls.Add(Me.btnPicSave)
        grbSetting.Controls.Add(Me.txtBasePointKido)
        grbSetting.Controls.Add(Me.txtMoveAveraveSize)
        grbSetting.Controls.Add(Me.txtCameraSet)
        grbSetting.Controls.Add(Me.btnHardCopy)
        grbSetting.Controls.Add(Me.lblMvAve)
        grbSetting.Controls.Add(Me.lblBP)
        grbSetting.Controls.Add(Me.lblCamera)
        grbSetting.Controls.Add(Me.btnExport)
        grbSetting.Controls.Add(Me.btnKidoCheck)
        grbSetting.Controls.Add(Me.chkBoxSelect)
        grbSetting.Controls.Add(Me.chkBoxCorrected)
        grbSetting.Controls.Add(Me.chkBoxHProMvAve)
        grbSetting.Controls.Add(Me.chkBoxHProRow)
        grbSetting.ImeMode = System.Windows.Forms.ImeMode.NoControl
        grbSetting.Location = New System.Drawing.Point(480, 16)
        grbSetting.Name = "grbSetting"
        grbSetting.Size = New System.Drawing.Size(196, 501)
        grbSetting.TabIndex = 1
        grbSetting.TabStop = False
        grbSetting.Text = "輝度補正設定"
        '
        'btnHoseitiRead
        '
        Me.btnHoseitiRead.Enabled = False
        Me.btnHoseitiRead.Location = New System.Drawing.Point(12, 460)
        Me.btnHoseitiRead.Name = "btnHoseitiRead"
        Me.btnHoseitiRead.Size = New System.Drawing.Size(172, 32)
        Me.btnHoseitiRead.TabIndex = 21
        Me.btnHoseitiRead.Text = "補正値読込"
        Me.btnHoseitiRead.UseVisualStyleBackColor = True
        '
        'radCor
        '
        Me.radCor.Appearance = System.Windows.Forms.Appearance.Button
        Me.radCor.BackColor = System.Drawing.SystemColors.Control
        Me.radCor.Location = New System.Drawing.Point(132, 343)
        Me.radCor.Name = "radCor"
        Me.radCor.Size = New System.Drawing.Size(52, 32)
        Me.radCor.TabIndex = 20
        Me.radCor.TabStop = True
        Me.radCor.Text = "補正値"
        Me.radCor.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radCor.UseVisualStyleBackColor = False
        '
        'lblCorPath
        '
        Me.lblCorPath.AutoSize = True
        Me.lblCorPath.Location = New System.Drawing.Point(8, 126)
        Me.lblCorPath.Name = "lblCorPath"
        Me.lblCorPath.Size = New System.Drawing.Size(77, 12)
        Me.lblCorPath.TabIndex = 19
        Me.lblCorPath.Text = "補正値出力先"
        '
        'txtCorPath
        '
        Me.txtCorPath.Location = New System.Drawing.Point(8, 146)
        Me.txtCorPath.Name = "txtCorPath"
        Me.txtCorPath.Size = New System.Drawing.Size(180, 19)
        Me.txtCorPath.TabIndex = 18
        '
        'radAfter
        '
        Me.radAfter.Appearance = System.Windows.Forms.Appearance.Button
        Me.radAfter.BackColor = System.Drawing.SystemColors.Control
        Me.radAfter.Location = New System.Drawing.Point(72, 343)
        Me.radAfter.Name = "radAfter"
        Me.radAfter.Size = New System.Drawing.Size(52, 32)
        Me.radAfter.TabIndex = 17
        Me.radAfter.TabStop = True
        Me.radAfter.Text = "補正後"
        Me.radAfter.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radAfter.UseVisualStyleBackColor = False
        '
        'radBefore
        '
        Me.radBefore.Appearance = System.Windows.Forms.Appearance.Button
        Me.radBefore.BackColor = System.Drawing.SystemColors.Control
        Me.radBefore.Location = New System.Drawing.Point(12, 343)
        Me.radBefore.Name = "radBefore"
        Me.radBefore.Size = New System.Drawing.Size(52, 32)
        Me.radBefore.TabIndex = 16
        Me.radBefore.TabStop = True
        Me.radBefore.Text = "補正前"
        Me.radBefore.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radBefore.UseVisualStyleBackColor = False
        '
        'btnPicSave
        '
        Me.btnPicSave.Location = New System.Drawing.Point(12, 382)
        Me.btnPicSave.Name = "btnPicSave"
        Me.btnPicSave.Size = New System.Drawing.Size(172, 32)
        Me.btnPicSave.TabIndex = 15
        Me.btnPicSave.Text = "表示画像保存"
        Me.btnPicSave.UseVisualStyleBackColor = True
        '
        'txtBasePointKido
        '
        Me.txtBasePointKido.Location = New System.Drawing.Point(104, 63)
        Me.txtBasePointKido.Name = "txtBasePointKido"
        Me.txtBasePointKido.Size = New System.Drawing.Size(76, 19)
        Me.txtBasePointKido.TabIndex = 14
        '
        'txtMoveAveraveSize
        '
        Me.txtMoveAveraveSize.Location = New System.Drawing.Point(104, 96)
        Me.txtMoveAveraveSize.Name = "txtMoveAveraveSize"
        Me.txtMoveAveraveSize.Size = New System.Drawing.Size(76, 19)
        Me.txtMoveAveraveSize.TabIndex = 13
        '
        'txtCameraSet
        '
        Me.txtCameraSet.Location = New System.Drawing.Point(104, 32)
        Me.txtCameraSet.Name = "txtCameraSet"
        Me.txtCameraSet.Size = New System.Drawing.Size(76, 19)
        Me.txtCameraSet.TabIndex = 12
        '
        'btnHardCopy
        '
        Me.btnHardCopy.Location = New System.Drawing.Point(12, 421)
        Me.btnHardCopy.Name = "btnHardCopy"
        Me.btnHardCopy.Size = New System.Drawing.Size(172, 32)
        Me.btnHardCopy.TabIndex = 9
        Me.btnHardCopy.Text = "ハードコピー"
        Me.btnHardCopy.UseVisualStyleBackColor = True
        '
        'lblMvAve
        '
        Me.lblMvAve.AutoSize = True
        Me.lblMvAve.Location = New System.Drawing.Point(8, 100)
        Me.lblMvAve.Name = "lblMvAve"
        Me.lblMvAve.Size = New System.Drawing.Size(82, 12)
        Me.lblMvAve.TabIndex = 11
        Me.lblMvAve.Text = "移動平均サイズ"
        '
        'lblBP
        '
        Me.lblBP.AutoSize = True
        Me.lblBP.Location = New System.Drawing.Point(8, 67)
        Me.lblBP.Name = "lblBP"
        Me.lblBP.Size = New System.Drawing.Size(65, 12)
        Me.lblBP.TabIndex = 10
        Me.lblBP.Text = "基点輝度値"
        '
        'lblCamera
        '
        Me.lblCamera.AutoSize = True
        Me.lblCamera.Location = New System.Drawing.Point(8, 36)
        Me.lblCamera.Name = "lblCamera"
        Me.lblCamera.Size = New System.Drawing.Size(55, 12)
        Me.lblCamera.TabIndex = 4
        Me.lblCamera.Text = "カメラセット"
        '
        'btnExport
        '
        Me.btnExport.Location = New System.Drawing.Point(12, 304)
        Me.btnExport.Name = "btnExport"
        Me.btnExport.Size = New System.Drawing.Size(172, 32)
        Me.btnExport.TabIndex = 7
        Me.btnExport.Text = "補正値出力"
        Me.btnExport.UseVisualStyleBackColor = True
        '
        'btnKidoCheck
        '
        Me.btnKidoCheck.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.btnKidoCheck.Location = New System.Drawing.Point(12, 265)
        Me.btnKidoCheck.Name = "btnKidoCheck"
        Me.btnKidoCheck.Size = New System.Drawing.Size(172, 32)
        Me.btnKidoCheck.TabIndex = 6
        Me.btnKidoCheck.Text = "輝度計算"
        Me.btnKidoCheck.UseVisualStyleBackColor = True
        '
        'chkBoxCorrected
        '
        Me.chkBoxCorrected.AutoSize = True
        Me.chkBoxCorrected.Location = New System.Drawing.Point(12, 220)
        Me.chkBoxCorrected.Name = "chkBoxCorrected"
        Me.chkBoxCorrected.Size = New System.Drawing.Size(84, 16)
        Me.chkBoxCorrected.TabIndex = 5
        Me.chkBoxCorrected.Text = "補正輝度線"
        Me.chkBoxCorrected.UseVisualStyleBackColor = True
        '
        'chkBoxHProMvAve
        '
        Me.chkBoxHProMvAve.AutoSize = True
        Me.chkBoxHProMvAve.Location = New System.Drawing.Point(12, 200)
        Me.chkBoxHProMvAve.Name = "chkBoxHProMvAve"
        Me.chkBoxHProMvAve.Size = New System.Drawing.Size(176, 16)
        Me.chkBoxHProMvAve.TabIndex = 4
        Me.chkBoxHProMvAve.Text = "縦プロジェクション線（移動平均）"
        Me.chkBoxHProMvAve.UseVisualStyleBackColor = True
        '
        'chkBoxHProRow
        '
        Me.chkBoxHProRow.AutoSize = True
        Me.chkBoxHProRow.Location = New System.Drawing.Point(12, 180)
        Me.chkBoxHProRow.Name = "chkBoxHProRow"
        Me.chkBoxHProRow.Size = New System.Drawing.Size(140, 16)
        Me.chkBoxHProRow.TabIndex = 3
        Me.chkBoxHProRow.Text = "縦プロジェクション線（生）"
        Me.chkBoxHProRow.UseVisualStyleBackColor = True
        '
        'pbxBitMap
        '
        Me.pbxBitMap.BackColor = System.Drawing.SystemColors.ControlLightLight
        Me.pbxBitMap.Location = New System.Drawing.Point(3, 0)
        Me.pbxBitMap.Name = "pbxBitMap"
        Me.pbxBitMap.Size = New System.Drawing.Size(452, 332)
        Me.pbxBitMap.TabIndex = 2
        Me.pbxBitMap.TabStop = False
        '
        'pnlBitMap
        '
        Me.pnlBitMap.AllowDrop = True
        Me.pnlBitMap.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pnlBitMap.BackColor = System.Drawing.SystemColors.Control
        Me.pnlBitMap.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.pnlBitMap.Controls.Add(Me.VScrollBM)
        Me.pnlBitMap.Controls.Add(Me.HScrollBM)
        Me.pnlBitMap.Controls.Add(Me.pbxBitMap)
        Me.pnlBitMap.Location = New System.Drawing.Point(12, 60)
        Me.pnlBitMap.Name = "pnlBitMap"
        Me.pnlBitMap.Size = New System.Drawing.Size(454, 338)
        Me.pnlBitMap.TabIndex = 3
        '
        'VScrollBM
        '
        Me.VScrollBM.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.VScrollBM.LargeChange = 101
        Me.VScrollBM.Location = New System.Drawing.Point(436, 0)
        Me.VScrollBM.Name = "VScrollBM"
        Me.VScrollBM.Size = New System.Drawing.Size(16, 318)
        Me.VScrollBM.TabIndex = 4
        '
        'HScrollBM
        '
        Me.HScrollBM.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.HScrollBM.LargeChange = 101
        Me.HScrollBM.Location = New System.Drawing.Point(0, 318)
        Me.HScrollBM.Name = "HScrollBM"
        Me.HScrollBM.Size = New System.Drawing.Size(435, 16)
        Me.HScrollBM.TabIndex = 3
        '
        'lblKido
        '
        Me.lblKido.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.lblKido.AutoSize = True
        Me.lblKido.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblKido.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKido.Location = New System.Drawing.Point(12, 408)
        Me.lblKido.Name = "lblKido"
        Me.lblKido.Size = New System.Drawing.Size(78, 18)
        Me.lblKido.TabIndex = 6
        Me.lblKido.Text = "輝度一覧"
        '
        'lblFrame
        '
        Me.lblFrame.AutoSize = True
        Me.lblFrame.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblFrame.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblFrame.Location = New System.Drawing.Point(12, 36)
        Me.lblFrame.Name = "lblFrame"
        Me.lblFrame.Size = New System.Drawing.Size(112, 18)
        Me.lblFrame.TabIndex = 5
        Me.lblFrame.Text = "フレーム画像"
        '
        'btnRateChg
        '
        Me.btnRateChg.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.btnRateChg.Location = New System.Drawing.Point(392, 20)
        Me.btnRateChg.Name = "btnRateChg"
        Me.btnRateChg.Size = New System.Drawing.Size(75, 23)
        Me.btnRateChg.TabIndex = 6
        Me.btnRateChg.Text = "×1"
        Me.btnRateChg.UseVisualStyleBackColor = True
        '
        'lblInform
        '
        Me.lblInform.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblInform.Location = New System.Drawing.Point(484, 520)
        Me.lblInform.Name = "lblInform"
        Me.lblInform.Size = New System.Drawing.Size(192, 60)
        Me.lblInform.TabIndex = 10
        Me.lblInform.Visible = False
        '
        'pgbProcess
        '
        Me.pgbProcess.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pgbProcess.Location = New System.Drawing.Point(484, 585)
        Me.pgbProcess.Name = "pgbProcess"
        Me.pgbProcess.Size = New System.Drawing.Size(192, 15)
        Me.pgbProcess.TabIndex = 11
        Me.pgbProcess.Visible = False
        '
        'lblRateChg
        '
        Me.lblRateChg.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblRateChg.AutoSize = True
        Me.lblRateChg.Location = New System.Drawing.Point(332, 28)
        Me.lblRateChg.Name = "lblRateChg"
        Me.lblRateChg.Size = New System.Drawing.Size(53, 12)
        Me.lblRateChg.TabIndex = 13
        Me.lblRateChg.Text = "倍率表示"
        '
        'HScrollDgv
        '
        Me.HScrollDgv.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.HScrollDgv.LargeChange = 101
        Me.HScrollDgv.Location = New System.Drawing.Point(13, 570)
        Me.HScrollDgv.Name = "HScrollDgv"
        Me.HScrollDgv.Size = New System.Drawing.Size(454, 16)
        Me.HScrollDgv.TabIndex = 12
        '
        'dgvKido
        '
        Me.dgvKido.Anchor = CType(((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvKido.Location = New System.Drawing.Point(12, 432)
        Me.dgvKido.Name = "dgvKido"
        Me.dgvKido.RowTemplate.Height = 21
        Me.dgvKido.ScrollBars = System.Windows.Forms.ScrollBars.None
        Me.dgvKido.Size = New System.Drawing.Size(456, 155)
        Me.dgvKido.TabIndex = 7
        '
        'btnRawMax
        '
        Me.btnRawMax.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.btnRawMax.Location = New System.Drawing.Point(102, 404)
        Me.btnRawMax.Name = "btnRawMax"
        Me.btnRawMax.Size = New System.Drawing.Size(75, 23)
        Me.btnRawMax.TabIndex = 14
        Me.btnRawMax.Text = "生　最大"
        Me.btnRawMax.UseVisualStyleBackColor = True
        '
        'btnAveMax
        '
        Me.btnAveMax.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.btnAveMax.Location = New System.Drawing.Point(188, 404)
        Me.btnAveMax.Name = "btnAveMax"
        Me.btnAveMax.Size = New System.Drawing.Size(75, 23)
        Me.btnAveMax.TabIndex = 15
        Me.btnAveMax.Text = "平均最大"
        Me.btnAveMax.UseVisualStyleBackColor = True
        '
        'chkBoxSelect
        '
        Me.chkBoxSelect.AutoSize = True
        Me.chkBoxSelect.Location = New System.Drawing.Point(12, 240)
        Me.chkBoxSelect.Name = "chkBoxSelect"
        Me.chkBoxSelect.Size = New System.Drawing.Size(120, 16)
        Me.chkBoxSelect.TabIndex = 5
        Me.chkBoxSelect.Text = "幅方向位置選択線"
        Me.chkBoxSelect.UseVisualStyleBackColor = True
        '
        'frmKidoHosei
        '
        Me.AllowDrop = True
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(688, 602)
        Me.Controls.Add(Me.btnAveMax)
        Me.Controls.Add(Me.btnRawMax)
        Me.Controls.Add(Me.lblRateChg)
        Me.Controls.Add(Me.HScrollDgv)
        Me.Controls.Add(Me.lblKido)
        Me.Controls.Add(Me.dgvKido)
        Me.Controls.Add(Me.pnlBitMap)
        Me.Controls.Add(Me.lblFrame)
        Me.Controls.Add(Me.pgbProcess)
        Me.Controls.Add(Me.lblInform)
        Me.Controls.Add(Me.btnRateChg)
        Me.Controls.Add(grbSetting)
        Me.Name = "frmKidoHosei"
        Me.Text = "輝度補正画面"
        grbSetting.ResumeLayout(False)
        grbSetting.PerformLayout()
        CType(Me.pbxBitMap, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlBitMap.ResumeLayout(False)
        CType(Me.dgvKido, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btnHardCopy As System.Windows.Forms.Button
    Friend WithEvents btnExport As System.Windows.Forms.Button
    Friend WithEvents btnKidoCheck As System.Windows.Forms.Button
    Friend WithEvents chkBoxCorrected As System.Windows.Forms.CheckBox
    Friend WithEvents chkBoxHProMvAve As System.Windows.Forms.CheckBox
    Friend WithEvents chkBoxHProRow As System.Windows.Forms.CheckBox
    Friend WithEvents lblCamera As System.Windows.Forms.Label
    Friend WithEvents lblMvAve As System.Windows.Forms.Label
    Friend WithEvents lblBP As System.Windows.Forms.Label
    Friend WithEvents txtBasePointKido As System.Windows.Forms.TextBox
    Friend WithEvents txtMoveAveraveSize As System.Windows.Forms.TextBox
    Friend WithEvents txtCameraSet As System.Windows.Forms.TextBox
    Friend WithEvents pbxBitMap As System.Windows.Forms.PictureBox
    Friend WithEvents pnlBitMap As System.Windows.Forms.Panel
    Friend WithEvents lblKido As System.Windows.Forms.Label
    Friend WithEvents lblFrame As System.Windows.Forms.Label
    Friend WithEvents btnRateChg As System.Windows.Forms.Button
    Friend WithEvents dgvKido As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents lblInform As System.Windows.Forms.Label
    Friend WithEvents pgbProcess As System.Windows.Forms.ProgressBar
    Friend WithEvents VScrollBM As tClassLibrary.tCnt.tCnt_VScrollBar
    Friend WithEvents HScrollBM As tClassLibrary.tCnt.tCnt_HScrollBar
    Friend WithEvents btnPicSave As System.Windows.Forms.Button
    Friend WithEvents radAfter As System.Windows.Forms.RadioButton
    Friend WithEvents radBefore As System.Windows.Forms.RadioButton
    Friend WithEvents HScrollDgv As tClassLibrary.tCnt.tCnt_HScrollBar
    Friend WithEvents lblRateChg As System.Windows.Forms.Label
    Friend WithEvents lblCorPath As System.Windows.Forms.Label
    Friend WithEvents txtCorPath As System.Windows.Forms.TextBox
    Friend WithEvents btnRawMax As System.Windows.Forms.Button
    Friend WithEvents btnAveMax As System.Windows.Forms.Button
    Friend WithEvents radCor As System.Windows.Forms.RadioButton
    Friend WithEvents btnHoseitiRead As System.Windows.Forms.Button
    Friend WithEvents ofdHoseitiRead As System.Windows.Forms.OpenFileDialog
    Friend WithEvents chkBoxSelect As CheckBox
End Class
