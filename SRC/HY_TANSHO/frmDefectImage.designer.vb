<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmDefectImage
    Inherits tClassForm.frmBase_Popup

    'Form は、コンポーネント一覧に後処理を実行するために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.cmdEnd = New System.Windows.Forms.Button()
        Me.cmbPicRate = New System.Windows.Forms.ComboBox()
        Me.tlpImage = New System.Windows.Forms.TableLayoutPanel()
        Me.lblRateName = New System.Windows.Forms.Label()
        Me.dgvKizuInfo = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvKizuInfo2 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.tlpMain = New System.Windows.Forms.TableLayoutPanel()
        Me.hsbImg = New tClassLibrary.tCnt.tCnt_HScrollBar()
        Me.vsbImg = New tClassLibrary.tCnt.tCnt_VScrollBar()
        Me.cbDetail = New System.Windows.Forms.CheckBox()
        Me.tlpTitle = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel1 = New System.Windows.Forms.Panel()
        CType(Me.dgvKizuInfo, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvKizuInfo2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tlpMain.SuspendLayout()
        Me.tlpTitle.SuspendLayout()
        Me.Panel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'cmdEnd
        '
        Me.cmdEnd.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cmdEnd.Font = New System.Drawing.Font("ＭＳ 明朝", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmdEnd.Location = New System.Drawing.Point(746, 0)
        Me.cmdEnd.Margin = New System.Windows.Forms.Padding(0)
        Me.cmdEnd.Name = "cmdEnd"
        Me.cmdEnd.Size = New System.Drawing.Size(106, 39)
        Me.cmdEnd.TabIndex = 101
        Me.cmdEnd.Text = "F12:閉じる"
        Me.cmdEnd.UseVisualStyleBackColor = True
        '
        'cmbPicRate
        '
        Me.cmbPicRate.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cmbPicRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbPicRate.Font = New System.Drawing.Font("ＭＳ 明朝", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbPicRate.FormattingEnabled = True
        Me.cmbPicRate.Location = New System.Drawing.Point(2, 16)
        Me.cmbPicRate.Name = "cmbPicRate"
        Me.cmbPicRate.Size = New System.Drawing.Size(80, 21)
        Me.cmbPicRate.TabIndex = 100
        '
        'tlpImage
        '
        Me.tlpImage.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tlpImage.ColumnCount = 1
        Me.tlpImage.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpImage.Location = New System.Drawing.Point(3, 3)
        Me.tlpImage.Name = "tlpImage"
        Me.tlpImage.RowCount = 1
        Me.tlpImage.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpImage.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.tlpImage.Size = New System.Drawing.Size(830, 530)
        Me.tlpImage.TabIndex = 102
        '
        'lblRateName
        '
        Me.lblRateName.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.lblRateName.AutoSize = True
        Me.lblRateName.Font = New System.Drawing.Font("ＭＳ 明朝", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblRateName.Location = New System.Drawing.Point(6, 2)
        Me.lblRateName.Name = "lblRateName"
        Me.lblRateName.Size = New System.Drawing.Size(67, 13)
        Me.lblRateName.TabIndex = 103
        Me.lblRateName.Text = "画像倍率"
        '
        'dgvKizuInfo
        '
        Me.dgvKizuInfo.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvKizuInfo.Location = New System.Drawing.Point(672, 42)
        Me.dgvKizuInfo.Name = "dgvKizuInfo"
        Me.dgvKizuInfo.RowTemplate.Height = 21
        Me.dgvKizuInfo.Size = New System.Drawing.Size(180, 548)
        Me.dgvKizuInfo.TabIndex = 104
        '
        'dgvKizuInfo2
        '
        Me.dgvKizuInfo2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvKizuInfo2.Location = New System.Drawing.Point(32, 0)
        Me.dgvKizuInfo2.Margin = New System.Windows.Forms.Padding(0)
        Me.dgvKizuInfo2.Name = "dgvKizuInfo2"
        Me.dgvKizuInfo2.RowTemplate.Height = 21
        Me.dgvKizuInfo2.Size = New System.Drawing.Size(630, 39)
        Me.dgvKizuInfo2.TabIndex = 105
        '
        'tlpMain
        '
        Me.tlpMain.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tlpMain.ColumnCount = 2
        Me.tlpMain.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpMain.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 16.0!))
        Me.tlpMain.Controls.Add(Me.hsbImg, 0, 1)
        Me.tlpMain.Controls.Add(Me.vsbImg, 1, 0)
        Me.tlpMain.Controls.Add(Me.tlpImage, 0, 0)
        Me.tlpMain.Location = New System.Drawing.Point(0, 38)
        Me.tlpMain.Name = "tlpMain"
        Me.tlpMain.RowCount = 2
        Me.tlpMain.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpMain.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 16.0!))
        Me.tlpMain.Size = New System.Drawing.Size(852, 552)
        Me.tlpMain.TabIndex = 106
        '
        'hsbImg
        '
        Me.hsbImg.Dock = System.Windows.Forms.DockStyle.Fill
        Me.hsbImg.Location = New System.Drawing.Point(0, 536)
        Me.hsbImg.Name = "hsbImg"
        Me.hsbImg.Size = New System.Drawing.Size(836, 16)
        Me.hsbImg.TabIndex = 108
        '
        'vsbImg
        '
        Me.vsbImg.Dock = System.Windows.Forms.DockStyle.Fill
        Me.vsbImg.Location = New System.Drawing.Point(836, 0)
        Me.vsbImg.Name = "vsbImg"
        Me.vsbImg.Size = New System.Drawing.Size(16, 536)
        Me.vsbImg.TabIndex = 104
        '
        'cbDetail
        '
        Me.cbDetail.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.cbDetail.Appearance = System.Windows.Forms.Appearance.Button
        Me.cbDetail.BackColor = System.Drawing.SystemColors.Control
        Me.cbDetail.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cbDetail.Location = New System.Drawing.Point(0, 0)
        Me.cbDetail.Margin = New System.Windows.Forms.Padding(0)
        Me.cbDetail.Name = "cbDetail"
        Me.cbDetail.Size = New System.Drawing.Size(32, 39)
        Me.cbDetail.TabIndex = 107
        Me.cbDetail.Text = "詳細"
        Me.cbDetail.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.cbDetail.UseVisualStyleBackColor = False
        '
        'tlpTitle
        '
        Me.tlpTitle.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tlpTitle.ColumnCount = 4
        Me.tlpTitle.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 32.0!))
        Me.tlpTitle.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpTitle.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 84.0!))
        Me.tlpTitle.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 106.0!))
        Me.tlpTitle.Controls.Add(Me.cmdEnd, 3, 0)
        Me.tlpTitle.Controls.Add(Me.Panel1, 2, 0)
        Me.tlpTitle.Controls.Add(Me.cbDetail, 0, 0)
        Me.tlpTitle.Controls.Add(Me.dgvKizuInfo2, 1, 0)
        Me.tlpTitle.Location = New System.Drawing.Point(0, 0)
        Me.tlpTitle.Margin = New System.Windows.Forms.Padding(0)
        Me.tlpTitle.Name = "tlpTitle"
        Me.tlpTitle.RowCount = 1
        Me.tlpTitle.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tlpTitle.Size = New System.Drawing.Size(852, 39)
        Me.tlpTitle.TabIndex = 108
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.cmbPicRate)
        Me.Panel1.Controls.Add(Me.lblRateName)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel1.Location = New System.Drawing.Point(662, 0)
        Me.Panel1.Margin = New System.Windows.Forms.Padding(0)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(84, 39)
        Me.Panel1.TabIndex = 108
        '
        'frmDefectImage
        '
        Me.ClientSize = New System.Drawing.Size(853, 590)
        Me.Controls.Add(Me.tlpTitle)
        Me.Controls.Add(Me.tlpMain)
        Me.Controls.Add(Me.dgvKizuInfo)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Sizable
        Me.Name = "frmDefectImage"
        Me.Text = "疵拡大画像"
        Me.TopMost = False
        Me.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iNON
        Me.tProp_Moveable = True
        CType(Me.dgvKizuInfo, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvKizuInfo2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tlpMain.ResumeLayout(False)
        Me.tlpTitle.ResumeLayout(False)
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents cmdEnd As System.Windows.Forms.Button
    Friend WithEvents cmbPicRate As System.Windows.Forms.ComboBox
    Friend WithEvents tlpImage As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents lblRateName As System.Windows.Forms.Label
    Friend WithEvents dgvKizuInfo As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvKizuInfo2 As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents tlpMain As TableLayoutPanel
    Friend WithEvents hsbImg As tClassLibrary.tCnt.tCnt_HScrollBar
    Friend WithEvents vsbImg As tClassLibrary.tCnt.tCnt_VScrollBar
    Friend WithEvents cbDetail As CheckBox
    Friend WithEvents tlpTitle As TableLayoutPanel
    Friend WithEvents Panel1 As Panel
End Class
