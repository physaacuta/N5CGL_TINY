<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmMain
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
        Me.txtDbName = New System.Windows.Forms.TextBox
        Me.txtDbWriteUser = New System.Windows.Forms.TextBox
        Me.txtDbWritePass = New System.Windows.Forms.TextBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.txtDbWriteSql = New System.Windows.Forms.TextBox
        Me.txtDBWriteAddless = New System.Windows.Forms.TextBox
        Me.Label4 = New System.Windows.Forms.Label
        Me.Label5 = New System.Windows.Forms.Label
        Me.btnDbWrite = New System.Windows.Forms.Button
        Me.btnInsert = New System.Windows.Forms.Button
        Me.btnUpdate = New System.Windows.Forms.Button
        Me.SqlGroup = New System.Windows.Forms.GroupBox
        Me.Label10 = New System.Windows.Forms.Label
        Me.Label9 = New System.Windows.Forms.Label
        Me.Label8 = New System.Windows.Forms.Label
        Me.Label7 = New System.Windows.Forms.Label
        Me.txtLoopEnd = New System.Windows.Forms.TextBox
        Me.txtLoopStart = New System.Windows.Forms.TextBox
        Me.txtTableName = New System.Windows.Forms.TextBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.BinDataGroup = New System.Windows.Forms.GroupBox
        Me.lblStatus = New System.Windows.Forms.Label
        Me.Label11 = New System.Windows.Forms.Label
        Me.SqlGroup.SuspendLayout()
        Me.SuspendLayout()
        '
        'txtDbName
        '
        Me.txtDbName.Location = New System.Drawing.Point(312, 36)
        Me.txtDbName.Name = "txtDbName"
        Me.txtDbName.Size = New System.Drawing.Size(104, 19)
        Me.txtDbName.TabIndex = 4
        Me.txtDbName.Text = "Database_"
        '
        'txtDbWriteUser
        '
        Me.txtDbWriteUser.Location = New System.Drawing.Point(308, 12)
        Me.txtDbWriteUser.Name = "txtDbWriteUser"
        Me.txtDbWriteUser.Size = New System.Drawing.Size(40, 19)
        Me.txtDbWriteUser.TabIndex = 2
        Me.txtDbWriteUser.Text = "sa"
        '
        'txtDbWritePass
        '
        Me.txtDbWritePass.Location = New System.Drawing.Point(376, 12)
        Me.txtDbWritePass.Name = "txtDbWritePass"
        Me.txtDbWritePass.Size = New System.Drawing.Size(40, 19)
        Me.txtDbWritePass.TabIndex = 3
        Me.txtDbWritePass.Text = "nsc"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(280, 40)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(30, 11)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "DB名"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(276, 16)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(27, 11)
        Me.Label2.TabIndex = 2
        Me.Label2.Text = "User"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(352, 16)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(22, 11)
        Me.Label3.TabIndex = 2
        Me.Label3.Text = "ﾊﾟｽ"
        '
        'txtDbWriteSql
        '
        Me.txtDbWriteSql.Location = New System.Drawing.Point(8, 56)
        Me.txtDbWriteSql.Multiline = True
        Me.txtDbWriteSql.Name = "txtDbWriteSql"
        Me.txtDbWriteSql.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.txtDbWriteSql.Size = New System.Drawing.Size(408, 124)
        Me.txtDbWriteSql.TabIndex = 5
        '
        'txtDBWriteAddless
        '
        Me.txtDBWriteAddless.Location = New System.Drawing.Point(156, 12)
        Me.txtDBWriteAddless.Name = "txtDBWriteAddless"
        Me.txtDBWriteAddless.Size = New System.Drawing.Size(116, 19)
        Me.txtDBWriteAddless.TabIndex = 1
        Me.txtDBWriteAddless.Text = "192.9.200.216\db2005"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(120, 16)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(33, 11)
        Me.Label4.TabIndex = 2
        Me.Label4.Text = "ｱﾄﾞﾚｽ"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(8, 40)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(37, 11)
        Me.Label5.TabIndex = 2
        Me.Label5.Text = "SQL文"
        '
        'btnDbWrite
        '
        Me.btnDbWrite.Location = New System.Drawing.Point(340, 208)
        Me.btnDbWrite.Name = "btnDbWrite"
        Me.btnDbWrite.Size = New System.Drawing.Size(76, 20)
        Me.btnDbWrite.TabIndex = 11
        Me.btnDbWrite.Text = "DB書き込み"
        Me.btnDbWrite.UseVisualStyleBackColor = True
        '
        'btnInsert
        '
        Me.btnInsert.Location = New System.Drawing.Point(172, 184)
        Me.btnInsert.Name = "btnInsert"
        Me.btnInsert.Size = New System.Drawing.Size(60, 20)
        Me.btnInsert.TabIndex = 7
        Me.btnInsert.Text = "INSERT"
        Me.btnInsert.UseVisualStyleBackColor = True
        '
        'btnUpdate
        '
        Me.btnUpdate.Location = New System.Drawing.Point(236, 184)
        Me.btnUpdate.Name = "btnUpdate"
        Me.btnUpdate.Size = New System.Drawing.Size(60, 20)
        Me.btnUpdate.TabIndex = 8
        Me.btnUpdate.Text = "UPDATE"
        Me.btnUpdate.UseVisualStyleBackColor = True
        '
        'SqlGroup
        '
        Me.SqlGroup.Controls.Add(Me.Label10)
        Me.SqlGroup.Controls.Add(Me.Label9)
        Me.SqlGroup.Controls.Add(Me.Label8)
        Me.SqlGroup.Controls.Add(Me.Label7)
        Me.SqlGroup.Controls.Add(Me.txtLoopEnd)
        Me.SqlGroup.Controls.Add(Me.txtLoopStart)
        Me.SqlGroup.Controls.Add(Me.txtDBWriteAddless)
        Me.SqlGroup.Controls.Add(Me.btnUpdate)
        Me.SqlGroup.Controls.Add(Me.Label4)
        Me.SqlGroup.Controls.Add(Me.txtDbWritePass)
        Me.SqlGroup.Controls.Add(Me.txtDbWriteUser)
        Me.SqlGroup.Controls.Add(Me.txtDbWriteSql)
        Me.SqlGroup.Controls.Add(Me.Label5)
        Me.SqlGroup.Controls.Add(Me.btnInsert)
        Me.SqlGroup.Controls.Add(Me.Label3)
        Me.SqlGroup.Controls.Add(Me.btnDbWrite)
        Me.SqlGroup.Controls.Add(Me.txtDbName)
        Me.SqlGroup.Controls.Add(Me.Label1)
        Me.SqlGroup.Controls.Add(Me.Label2)
        Me.SqlGroup.Controls.Add(Me.txtTableName)
        Me.SqlGroup.Controls.Add(Me.Label6)
        Me.SqlGroup.Location = New System.Drawing.Point(4, 48)
        Me.SqlGroup.Name = "SqlGroup"
        Me.SqlGroup.Size = New System.Drawing.Size(424, 232)
        Me.SqlGroup.TabIndex = 6
        Me.SqlGroup.TabStop = False
        Me.SqlGroup.Text = "SQL操作"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.Location = New System.Drawing.Point(188, 212)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(16, 11)
        Me.Label10.TabIndex = 12
        Me.Label10.Text = "～"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.Location = New System.Drawing.Point(212, 212)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(27, 11)
        Me.Label9.TabIndex = 12
        Me.Label9.Text = "終了"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(96, 212)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(27, 11)
        Me.Label8.TabIndex = 12
        Me.Label8.Text = "開始"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.Location = New System.Drawing.Point(8, 212)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(70, 11)
        Me.Label7.TabIndex = 12
        Me.Label7.Text = "SQLループ($n)"
        '
        'txtLoopEnd
        '
        Me.txtLoopEnd.Location = New System.Drawing.Point(240, 208)
        Me.txtLoopEnd.Name = "txtLoopEnd"
        Me.txtLoopEnd.Size = New System.Drawing.Size(48, 19)
        Me.txtLoopEnd.TabIndex = 10
        '
        'txtLoopStart
        '
        Me.txtLoopStart.Location = New System.Drawing.Point(128, 208)
        Me.txtLoopStart.Name = "txtLoopStart"
        Me.txtLoopStart.Size = New System.Drawing.Size(48, 19)
        Me.txtLoopStart.TabIndex = 9
        '
        'txtTableName
        '
        Me.txtTableName.Location = New System.Drawing.Point(60, 184)
        Me.txtTableName.Name = "txtTableName"
        Me.txtTableName.Size = New System.Drawing.Size(108, 19)
        Me.txtTableName.TabIndex = 6
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("MS UI Gothic", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(8, 188)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(46, 11)
        Me.Label6.TabIndex = 2
        Me.Label6.Text = "ﾃｰﾌﾞﾙ名"
        '
        'BinDataGroup
        '
        Me.BinDataGroup.Location = New System.Drawing.Point(4, 36)
        Me.BinDataGroup.Name = "BinDataGroup"
        Me.BinDataGroup.Size = New System.Drawing.Size(424, 8)
        Me.BinDataGroup.TabIndex = 7
        Me.BinDataGroup.TabStop = False
        Me.BinDataGroup.Text = "バイナリデータ読込"
        '
        'lblStatus
        '
        Me.lblStatus.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblStatus.Location = New System.Drawing.Point(60, 12)
        Me.lblStatus.Name = "lblStatus"
        Me.lblStatus.Size = New System.Drawing.Size(368, 16)
        Me.lblStatus.TabIndex = 1
        Me.lblStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Location = New System.Drawing.Point(4, 16)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(50, 12)
        Me.Label11.TabIndex = 0
        Me.Label11.Text = "ステータス"
        '
        'frmMain
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(431, 288)
        Me.Controls.Add(Me.BinDataGroup)
        Me.Controls.Add(Me.lblStatus)
        Me.Controls.Add(Me.SqlGroup)
        Me.Controls.Add(Me.Label11)
        Me.Name = "frmMain"
        Me.Tag = "0"
        Me.Text = "DB書込ツール(tBinaryInsert) ver 1.0"
        Me.SqlGroup.ResumeLayout(False)
        Me.SqlGroup.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txtDbName As System.Windows.Forms.TextBox
    Friend WithEvents txtDbWriteUser As System.Windows.Forms.TextBox
    Friend WithEvents txtDbWritePass As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents txtDbWriteSql As System.Windows.Forms.TextBox
    Friend WithEvents txtDBWriteAddless As System.Windows.Forms.TextBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents btnDbWrite As System.Windows.Forms.Button
    Friend WithEvents btnInsert As System.Windows.Forms.Button
    Friend WithEvents btnUpdate As System.Windows.Forms.Button
    Friend WithEvents SqlGroup As System.Windows.Forms.GroupBox
    Friend WithEvents BinDataGroup As System.Windows.Forms.GroupBox
    Friend WithEvents lblStatus As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents txtTableName As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents txtLoopStart As System.Windows.Forms.TextBox
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents txtLoopEnd As System.Windows.Forms.TextBox

End Class
