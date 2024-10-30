<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmGridSetting
    Inherits frmMain_ParSet_SysName

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.dlgColor = New System.Windows.Forms.ColorDialog()
        Me.dgvGrid = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.cmbGID = New System.Windows.Forms.ComboBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.cmbGridID = New System.Windows.Forms.ComboBox()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvGrid, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label10)
        Me.grpBase.Controls.Add(Me.Label7)
        Me.grpBase.Controls.Add(Me.Label9)
        Me.grpBase.Controls.Add(Me.Label3)
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label6)
        Me.grpBase.Controls.Add(Me.Label14)
        Me.grpBase.Controls.Add(Me.Label13)
        Me.grpBase.Controls.Add(Me.Label12)
        Me.grpBase.Controls.Add(Me.Label8)
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.cmbGridID)
        Me.grpBase.Controls.Add(Me.Label5)
        Me.grpBase.Controls.Add(Me.Label11)
        Me.grpBase.Controls.Add(Me.cmbGID)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.dgvGrid)
        Me.grpBase.Text = "データグリッド表示設定"
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
        'dgvGrid
        '
        Me.dgvGrid.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCellsExceptHeader
        Me.dgvGrid.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvGrid.Location = New System.Drawing.Point(16, 76)
        Me.dgvGrid.Name = "dgvGrid"
        Me.dgvGrid.RowTemplate.Height = 21
        Me.dgvGrid.Size = New System.Drawing.Size(1040, 480)
        Me.dgvGrid.TabIndex = 9
        '
        'cmbGID
        '
        Me.cmbGID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbGID.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.cmbGID.FormattingEnabled = True
        Me.cmbGID.Location = New System.Drawing.Point(16, 48)
        Me.cmbGID.Name = "cmbGID"
        Me.cmbGID.Size = New System.Drawing.Size(185, 24)
        Me.cmbGID.TabIndex = 22
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(16, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(185, 24)
        Me.Label1.TabIndex = 21
        Me.Label1.Text = "画面ID"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label11.ForeColor = System.Drawing.Color.Black
        Me.Label11.Location = New System.Drawing.Point(24, 572)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(104, 16)
        Me.Label11.TabIndex = 40
        Me.Label11.Text = "＜操作説明＞"
        '
        'cmbGridID
        '
        Me.cmbGridID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbGridID.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.cmbGridID.FormattingEnabled = True
        Me.cmbGridID.Location = New System.Drawing.Point(221, 48)
        Me.cmbGridID.Name = "cmbGridID"
        Me.cmbGridID.Size = New System.Drawing.Size(284, 24)
        Me.cmbGridID.TabIndex = 53
        '
        'Label5
        '
        Me.Label5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label5.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label5.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label5.Location = New System.Drawing.Point(221, 24)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(284, 24)
        Me.Label5.TabIndex = 52
        Me.Label5.Text = "グリッドID"
        Me.Label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label4.ForeColor = System.Drawing.Color.Black
        Me.Label4.Location = New System.Drawing.Point(20, 596)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(376, 16)
        Me.Label4.TabIndex = 54
        Me.Label4.Text = "１）「画面ID」にて対象となる画面を選択します。"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label8.ForeColor = System.Drawing.Color.Black
        Me.Label8.Location = New System.Drawing.Point(20, 620)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(808, 16)
        Me.Label8.TabIndex = 55
        Me.Label8.Text = "２）「グリッドID」にて修正対象のデータグリッドを選択します。(対象画面上のデータグリッド見出しに対応)"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label12.ForeColor = System.Drawing.Color.Black
        Me.Label12.Location = New System.Drawing.Point(20, 644)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(808, 16)
        Me.Label12.TabIndex = 56
        Me.Label12.Text = "３）指定したデータグリッドに対応する項目が表示されますので、修正を実施し「設定」ボタンを押下します。"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label13.ForeColor = System.Drawing.Color.Black
        Me.Label13.Location = New System.Drawing.Point(20, 668)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(576, 16)
        Me.Label13.TabIndex = 57
        Me.Label13.Text = "　※１ 項目を削除したい場合は「項目No」をクリアして設定を行って下さい。"
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label14.ForeColor = System.Drawing.Color.Black
        Me.Label14.Location = New System.Drawing.Point(20, 692)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(688, 16)
        Me.Label14.TabIndex = 58
        Me.Label14.Text = "　※２ 項目を新規に追加したい場合は、最右列の空白行に情報を登録し設定を行って下さい。"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label2.ForeColor = System.Drawing.Color.Black
        Me.Label2.Location = New System.Drawing.Point(20, 788)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(744, 16)
        Me.Label2.TabIndex = 59
        Me.Label2.Text = "４）設定確認のポップアップメッセージが表示されますので、問題なければ「OK」を押下して下さい。"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label3.ForeColor = System.Drawing.Color.Black
        Me.Label3.Location = New System.Drawing.Point(20, 812)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(648, 16)
        Me.Label3.TabIndex = 60
        Me.Label3.Text = "５）設定完了すると「正常に登録しました」とポップアップメッセージが表示されます。"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label6.ForeColor = System.Drawing.Color.Black
        Me.Label6.Location = New System.Drawing.Point(20, 740)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(848, 16)
        Me.Label6.TabIndex = 61
        Me.Label6.Text = "　※４ 「主キー」、「フィールドタイプ」、「NULL入力可否」は「フィールド名」を選択すると自動で取得します。"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label9.ForeColor = System.Drawing.Color.Black
        Me.Label9.Location = New System.Drawing.Point(20, 888)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(0, 16)
        Me.Label9.TabIndex = 63
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label7.ForeColor = System.Drawing.Color.Black
        Me.Label7.Location = New System.Drawing.Point(20, 716)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(1008, 16)
        Me.Label7.TabIndex = 64
        Me.Label7.Text = "　※３ 新規表示パターンのコンボボックスを追加する場合は、事前に選択項目ｱｲﾃﾑﾏｽﾀｰ画面上で「lov_id」の定義をする必要があります。"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label10.ForeColor = System.Drawing.Color.Black
        Me.Label10.Location = New System.Drawing.Point(20, 764)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(768, 16)
        Me.Label10.TabIndex = 65
        Me.Label10.Text = "　※５ 修正内容が異常な場合は、ポップアップメッセージが表示されますので内容を確認してください。"
        '
        'frmGridSetting
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmGridSetting"
        Me.Text = "データグリッド表示設定画面"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        CType(Me.dgvGrid, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dlgColor As System.Windows.Forms.ColorDialog
    Friend WithEvents dgvGrid As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents cmbGID As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents cmbGridID As System.Windows.Forms.ComboBox
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label

End Class
