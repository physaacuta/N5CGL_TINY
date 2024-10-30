<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcTajuuThresHold2
    Inherits KA_PARCOLOR.frmMain_AutoParSet_UpList_SysName

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
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
        Me.lblSCode = New System.Windows.Forms.Label()
        Me.cmbSCode = New System.Windows.Forms.ComboBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.dgvLabelImage = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.dgvSimpleBin = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.dgvFilterBin = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.camName = New KA_PARCOLOR.CamName()
        Me.pnlItem.SuspendLayout()
        Me.grpBase.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        CType(Me.dgvLabelImage, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        CType(Me.dgvSimpleBin, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        CType(Me.dgvFilterBin, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.GroupBox1)
        Me.grpBase.Controls.Add(Me.GroupBox2)
        Me.grpBase.Controls.Add(Me.GroupBox3)
        Me.grpBase.Controls.Add(Me.lblSCode)
        Me.grpBase.Controls.Add(Me.cmbSCode)
        Me.grpBase.Controls.Add(Me.Label6)
        Me.grpBase.Controls.Add(Me.Label7)
        Me.grpBase.Text = "多重分解能閾値設定"
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Text = "次頁"
        '
        'btnF01
        '
        Me.btnF01.AccessibleName = ""
        Me.btnF01.Enabled = True
        Me.btnF01.Text = "前頁"
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "終了"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "設定"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "切替"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "画面保存"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "画面印刷"
        '
        'lblSCode
        '
        Me.lblSCode.BackColor = System.Drawing.Color.LightSteelBlue
        Me.lblSCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSCode.ForeColor = System.Drawing.SystemColors.ControlText
        Me.lblSCode.Location = New System.Drawing.Point(166, 46)
        Me.lblSCode.Name = "lblSCode"
        Me.lblSCode.Size = New System.Drawing.Size(300, 24)
        Me.lblSCode.TabIndex = 52
        Me.lblSCode.Text = "12345678901234567890123456789012"
        Me.lblSCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCode
        '
        Me.cmbSCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.cmbSCode.FormattingEnabled = True
        Me.cmbSCode.Location = New System.Drawing.Point(6, 46)
        Me.cmbSCode.Name = "cmbSCode"
        Me.cmbSCode.Size = New System.Drawing.Size(156, 24)
        Me.cmbSCode.TabIndex = 49
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label6.Location = New System.Drawing.Point(166, 22)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(300, 24)
        Me.Label6.TabIndex = 51
        Me.Label6.Text = "検査表面状態名称"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(6, 22)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(154, 24)
        Me.Label7.TabIndex = 50
        Me.Label7.Text = "検査表面状態No"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.dgvLabelImage)
        Me.GroupBox3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox3.ForeColor = System.Drawing.Color.Black
        Me.GroupBox3.Location = New System.Drawing.Point(6, 73)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(1058, 274)
        Me.GroupBox3.TabIndex = 53
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "合成ラベル（検査表面状態別）"
        '
        'dgvLabelImage
        '
        Me.dgvLabelImage.Location = New System.Drawing.Point(6, 25)
        Me.dgvLabelImage.Name = "dgvLabelImage"
        Me.dgvLabelImage.RowTemplate.Height = 21
        Me.dgvLabelImage.Size = New System.Drawing.Size(1046, 243)
        Me.dgvLabelImage.TabIndex = 0
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(752, 348)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(240, 16)
        Me.Label1.TabIndex = 56
        Me.Label1.Text = "+:OR合成 *:AND合成 -:合成無し"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dgvSimpleBin)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.ForeColor = System.Drawing.Color.Black
        Me.GroupBox2.Location = New System.Drawing.Point(6, 362)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(1058, 274)
        Me.GroupBox2.TabIndex = 54
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "単純二値化（検査表面状態別）"
        '
        'dgvSimpleBin
        '
        Me.dgvSimpleBin.Location = New System.Drawing.Point(6, 25)
        Me.dgvSimpleBin.Name = "dgvSimpleBin"
        Me.dgvSimpleBin.RowTemplate.Height = 21
        Me.dgvSimpleBin.Size = New System.Drawing.Size(1046, 243)
        Me.dgvSimpleBin.TabIndex = 0
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.dgvFilterBin)
        Me.GroupBox1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.ForeColor = System.Drawing.Color.Black
        Me.GroupBox1.Location = New System.Drawing.Point(6, 640)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(1058, 274)
        Me.GroupBox1.TabIndex = 55
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "フィルタ二値化（検査表面状態別）"
        '
        'dgvFilterBin
        '
        Me.dgvFilterBin.Location = New System.Drawing.Point(6, 25)
        Me.dgvFilterBin.Name = "dgvFilterBin"
        Me.dgvFilterBin.RowTemplate.Height = 21
        Me.dgvFilterBin.Size = New System.Drawing.Size(1046, 243)
        Me.dgvFilterBin.TabIndex = 0
        '
        'camName
        '
        Me.camName.Location = New System.Drawing.Point(4, 412)
        Me.camName.Name = "camName"
        Me.camName.Size = New System.Drawing.Size(188, 224)
        Me.camName.TabIndex = 14
        '
        'frmEpcTajuuThresHold2
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.camName)
        Me.Name = "frmEpcTajuuThresHold2"
        Me.Text = "多重分解能閾値設定画面"
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.pnlItem, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.camName, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        CType(Me.dgvLabelImage, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        CType(Me.dgvSimpleBin, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        CType(Me.dgvFilterBin, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents lblSCode As Label
    Friend WithEvents cmbSCode As ComboBox
    Friend WithEvents Label6 As Label
    Friend WithEvents Label7 As Label
    Friend WithEvents GroupBox3 As GroupBox
    Friend WithEvents dgvLabelImage As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox2 As GroupBox
    Friend WithEvents dgvSimpleBin As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox1 As GroupBox
    Friend WithEvents dgvFilterBin As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents camName As CamName
    Friend WithEvents Label1 As Label
End Class
