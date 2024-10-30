<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSetting
    Inherits tClassForm.frmPop_Btn2

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
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.lblPopUpTani = New System.Windows.Forms.Label()
        Me.lblStopTani = New System.Windows.Forms.Label()
        Me.cmbPopUp = New System.Windows.Forms.ComboBox()
        Me.cmbStop = New System.Windows.Forms.ComboBox()
        Me.lblPopUp = New System.Windows.Forms.Label()
        Me.lblStop = New System.Windows.Forms.Label()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(76, 97)
        '
        'btnF01
        '
        Me.btnF01.Tag = 0
        '
        'btnF02
        '
        Me.btnF02.Tag = 11
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.lblPopUpTani)
        Me.GroupBox2.Controls.Add(Me.lblStopTani)
        Me.GroupBox2.Controls.Add(Me.cmbPopUp)
        Me.GroupBox2.Controls.Add(Me.cmbStop)
        Me.GroupBox2.Controls.Add(Me.lblPopUp)
        Me.GroupBox2.Controls.Add(Me.lblStop)
        Me.GroupBox2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(8, 4)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(372, 89)
        Me.GroupBox2.TabIndex = 221
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "表示設定"
        '
        'lblPopUpTani
        '
        Me.lblPopUpTani.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblPopUpTani.Location = New System.Drawing.Point(325, 52)
        Me.lblPopUpTani.Name = "lblPopUpTani"
        Me.lblPopUpTani.Size = New System.Drawing.Size(43, 16)
        Me.lblPopUpTani.TabIndex = 182
        Me.lblPopUpTani.Text = "[秒]"
        '
        'lblStopTani
        '
        Me.lblStopTani.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblStopTani.Location = New System.Drawing.Point(325, 24)
        Me.lblStopTani.Name = "lblStopTani"
        Me.lblStopTani.Size = New System.Drawing.Size(43, 16)
        Me.lblStopTani.TabIndex = 181
        Me.lblStopTani.Text = "[秒]"
        '
        'cmbPopUp
        '
        Me.cmbPopUp.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbPopUp.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbPopUp.FormattingEnabled = True
        Me.cmbPopUp.Items.AddRange(New Object() {"30"})
        Me.cmbPopUp.Location = New System.Drawing.Point(232, 49)
        Me.cmbPopUp.Name = "cmbPopUp"
        Me.cmbPopUp.Size = New System.Drawing.Size(92, 24)
        Me.cmbPopUp.TabIndex = 179
        '
        'cmbStop
        '
        Me.cmbStop.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbStop.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbStop.FormattingEnabled = True
        Me.cmbStop.Items.AddRange(New Object() {"30"})
        Me.cmbStop.Location = New System.Drawing.Point(231, 20)
        Me.cmbStop.Name = "cmbStop"
        Me.cmbStop.Size = New System.Drawing.Size(92, 24)
        Me.cmbStop.TabIndex = 178
        '
        'lblPopUp
        '
        Me.lblPopUp.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblPopUp.Location = New System.Drawing.Point(5, 52)
        Me.lblPopUp.Name = "lblPopUp"
        Me.lblPopUp.Size = New System.Drawing.Size(229, 17)
        Me.lblPopUp.TabIndex = 176
        Me.lblPopUp.Text = "ポップアップ画像のタイマー"
        '
        'lblStop
        '
        Me.lblStop.AutoSize = True
        Me.lblStop.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblStop.Location = New System.Drawing.Point(72, 24)
        Me.lblStop.Name = "lblStop"
        Me.lblStop.Size = New System.Drawing.Size(161, 16)
        Me.lblStop.TabIndex = 175
        Me.lblStop.Text = "一時停止のタイマー"
        '
        'frmSetting
        '
        Me.ClientSize = New System.Drawing.Size(392, 193)
        Me.Controls.Add(Me.GroupBox2)
        Me.Name = "frmSetting"
        Me.Text = "表示設定"
        Me.TopMost = False
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.GroupBox2, 0)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

    Friend WithEvents GroupBox2 As GroupBox
    Friend WithEvents lblPopUpTani As Label
    Friend WithEvents lblStopTani As Label
    Friend WithEvents cmbPopUp As ComboBox
    Friend WithEvents cmbStop As ComboBox
    Friend WithEvents lblPopUp As Label
    Friend WithEvents lblStop As Label
End Class
