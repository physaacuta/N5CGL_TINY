<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMaeTypeGr
    Inherits frmMain_ParSet_SysName

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
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.dgvMaeType = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.dgvMaeMaster = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dlgColor = New System.Windows.Forms.ColorDialog()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.dgvMaeGrade = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.lblMaeProcessCode = New System.Windows.Forms.Label()
        Me.cmbMaeProcessCode = New System.Windows.Forms.ComboBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.grpBase.SuspendLayout()
        Me.GroupBox1.SuspendLayout()
        CType(Me.dgvMaeType, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox3.SuspendLayout()
        CType(Me.dgvMaeMaster, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        CType(Me.dgvMaeGrade, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.lblMaeProcessCode)
        Me.grpBase.Controls.Add(Me.cmbMaeProcessCode)
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label3)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.GroupBox2)
        Me.grpBase.Controls.Add(Me.GroupBox1)
        Me.grpBase.Controls.Add(Me.GroupBox3)
        Me.grpBase.Size = New System.Drawing.Size(1072, 912)
        Me.grpBase.Text = "前工程マスター登録　疵種・グレード設定"
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Text = "次項"
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.Text = "前項"
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
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.dgvMaeType)
        Me.GroupBox1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.GroupBox1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox1.Location = New System.Drawing.Point(8, 344)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(604, 520)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "疵種設定"
        '
        'dgvMaeType
        '
        Me.dgvMaeType.Location = New System.Drawing.Point(8, 24)
        Me.dgvMaeType.Name = "dgvMaeType"
        Me.dgvMaeType.RowTemplate.Height = 21
        Me.dgvMaeType.Size = New System.Drawing.Size(588, 485)
        Me.dgvMaeType.TabIndex = 0
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(12, 868)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(496, 16)
        Me.Label1.TabIndex = 10
        Me.Label1.Text = "※１ 優先度は１が最も高くなります。優先度の重複はできません。"
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.dgvMaeMaster)
        Me.GroupBox3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox3.Location = New System.Drawing.Point(8, 24)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(1056, 244)
        Me.GroupBox3.TabIndex = 11
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "マスター登録"
        '
        'dgvMaeMaster
        '
        Me.dgvMaeMaster.Location = New System.Drawing.Point(8, 24)
        Me.dgvMaeMaster.Name = "dgvMaeMaster"
        Me.dgvMaeMaster.RowTemplate.Height = 21
        Me.dgvMaeMaster.Size = New System.Drawing.Size(1040, 212)
        Me.dgvMaeMaster.TabIndex = 0
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dgvMaeGrade)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.GroupBox2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox2.Location = New System.Drawing.Point(616, 344)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(448, 520)
        Me.GroupBox2.TabIndex = 1
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "グレード設定"
        '
        'dgvMaeGrade
        '
        Me.dgvMaeGrade.Location = New System.Drawing.Point(8, 24)
        Me.dgvMaeGrade.Name = "dgvMaeGrade"
        Me.dgvMaeGrade.RowTemplate.Height = 21
        Me.dgvMaeGrade.Size = New System.Drawing.Size(432, 485)
        Me.dgvMaeGrade.TabIndex = 1
        '
        'lblMaeProcessCode
        '
        Me.lblMaeProcessCode.BackColor = System.Drawing.Color.LightSteelBlue
        Me.lblMaeProcessCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblMaeProcessCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.lblMaeProcessCode.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblMaeProcessCode.Location = New System.Drawing.Point(208, 312)
        Me.lblMaeProcessCode.Name = "lblMaeProcessCode"
        Me.lblMaeProcessCode.Size = New System.Drawing.Size(300, 24)
        Me.lblMaeProcessCode.TabIndex = 15
        Me.lblMaeProcessCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbMaeProcessCode
        '
        Me.cmbMaeProcessCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbMaeProcessCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbMaeProcessCode.FormattingEnabled = True
        Me.cmbMaeProcessCode.Location = New System.Drawing.Point(8, 312)
        Me.cmbMaeProcessCode.MaxDropDownItems = 16
        Me.cmbMaeProcessCode.Name = "cmbMaeProcessCode"
        Me.cmbMaeProcessCode.Size = New System.Drawing.Size(172, 24)
        Me.cmbMaeProcessCode.TabIndex = 14
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(208, 288)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(300, 24)
        Me.Label2.TabIndex = 13
        Me.Label2.Text = "前工程名称"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label3.Location = New System.Drawing.Point(8, 288)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(172, 24)
        Me.Label3.TabIndex = 12
        Me.Label3.Text = "前工程コード"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(12, 889)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(560, 16)
        Me.Label4.TabIndex = 16
        Me.Label4.Text = "※２ 疵種設定の表示有無は、グレード設定の確定後に選択可能となります。"
        '
        'frmMaeTypeGr
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Name = "frmMaeTypeGr"
        Me.Text = "前工程設定画面"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        Me.GroupBox1.ResumeLayout(False)
        CType(Me.dgvMaeType, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox3.ResumeLayout(False)
        CType(Me.dgvMaeMaster, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        CType(Me.dgvMaeGrade, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvMaeType As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvMaeMaster As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dlgColor As System.Windows.Forms.ColorDialog
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvMaeGrade As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents lblMaeProcessCode As System.Windows.Forms.Label
    Friend WithEvents cmbMaeProcessCode As System.Windows.Forms.ComboBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
End Class
