<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSetFilter
    Inherits tClassForm.frmPop_Btn2

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
        Me.grpShousai = New System.Windows.Forms.GroupBox()
        Me.dgvShousaiEtc = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.dgvShousaiData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.cmbKaniGrade = New System.Windows.Forms.ComboBox()
        Me.radShousai = New System.Windows.Forms.RadioButton()
        Me.radKani = New System.Windows.Forms.RadioButton()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.grpShousai.SuspendLayout()
        CType(Me.dgvShousaiEtc, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvShousaiData, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(112, 672)
        '
        'btnF01
        '
        Me.btnF01.Tag = 0
        '
        'btnF02
        '
        Me.btnF02.Tag = 11
        '
        'grpShousai
        '
        Me.grpShousai.Controls.Add(Me.dgvShousaiEtc)
        Me.grpShousai.Controls.Add(Me.Label2)
        Me.grpShousai.Controls.Add(Me.dgvShousaiData)
        Me.grpShousai.Controls.Add(Me.cmbKaniGrade)
        Me.grpShousai.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpShousai.ForeColor = System.Drawing.Color.Navy
        Me.grpShousai.Location = New System.Drawing.Point(8, 64)
        Me.grpShousai.Name = "grpShousai"
        Me.grpShousai.Size = New System.Drawing.Size(416, 568)
        Me.grpShousai.TabIndex = 39
        Me.grpShousai.TabStop = False
        Me.grpShousai.Text = "è⁄ç◊ê›íË"
        '
        'dgvShousaiEtc
        '
        Me.dgvShousaiEtc.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvShousaiEtc.Location = New System.Drawing.Point(14, 536)
        Me.dgvShousaiEtc.Name = "dgvShousaiEtc"
        Me.dgvShousaiEtc.RowTemplate.Height = 21
        Me.dgvShousaiEtc.ScrollBars = System.Windows.Forms.ScrollBars.None
        Me.dgvShousaiEtc.Size = New System.Drawing.Size(372, 20)
        Me.dgvShousaiEtc.TabIndex = 31
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(121, 22)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(168, 16)
        Me.Label2.TabIndex = 47
        Me.Label2.Text = "ä»à’èoóÕÉOÉåÅ[Éhê›íË"
        '
        'dgvShousaiData
        '
        Me.dgvShousaiData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvShousaiData.Location = New System.Drawing.Point(14, 48)
        Me.dgvShousaiData.Name = "dgvShousaiData"
        Me.dgvShousaiData.RowTemplate.Height = 21
        Me.dgvShousaiData.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.ColumnHeaderSelect
        Me.dgvShousaiData.Size = New System.Drawing.Size(388, 479)
        Me.dgvShousaiData.TabIndex = 12
        '
        'cmbKaniGrade
        '
        Me.cmbKaniGrade.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbKaniGrade.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbKaniGrade.FormattingEnabled = True
        Me.cmbKaniGrade.Location = New System.Drawing.Point(291, 20)
        Me.cmbKaniGrade.Name = "cmbKaniGrade"
        Me.cmbKaniGrade.Size = New System.Drawing.Size(95, 21)
        Me.cmbKaniGrade.TabIndex = 46
        '
        'radShousai
        '
        Me.radShousai.Appearance = System.Windows.Forms.Appearance.Button
        Me.radShousai.AutoSize = True
        Me.radShousai.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.radShousai.Location = New System.Drawing.Point(16, 24)
        Me.radShousai.Name = "radShousai"
        Me.radShousai.Size = New System.Drawing.Size(66, 26)
        Me.radShousai.TabIndex = 41
        Me.radShousai.TabStop = True
        Me.radShousai.Text = "è⁄  ç◊"
        Me.radShousai.UseVisualStyleBackColor = True
        '
        'radKani
        '
        Me.radKani.Appearance = System.Windows.Forms.Appearance.Button
        Me.radKani.AutoSize = True
        Me.radKani.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.radKani.Location = New System.Drawing.Point(100, 24)
        Me.radKani.Name = "radKani"
        Me.radKani.Size = New System.Drawing.Size(66, 26)
        Me.radKani.TabIndex = 42
        Me.radKani.TabStop = True
        Me.radKani.Text = "ä»  à’"
        Me.radKani.UseVisualStyleBackColor = True
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(8, 636)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(256, 16)
        Me.Label4.TabIndex = 44
        Me.Label4.Text = "Å¶'Å\ÅfÇÕÅAèoóÕñ≥ÇµÇÃê›íËÇ≈Ç∑ÅB"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.radShousai)
        Me.GroupBox1.Controls.Add(Me.radKani)
        Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox1.Location = New System.Drawing.Point(8, 4)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(180, 56)
        Me.GroupBox1.TabIndex = 45
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ï\é¶ï˚ñ@ëIë"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label3.Location = New System.Drawing.Point(8, 656)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(424, 16)
        Me.Label3.TabIndex = 48
        Me.Label3.Text = "Å¶ä»à’èoóÕóLÇËÇÕ1å¬à»è„ÅA8å¬à»â∫Ç≈ê›íËÇµÇƒÇ≠ÇæÇ≥Ç¢ÅB"
        '
        'frmSetFilter
        '
        Me.ClientSize = New System.Drawing.Size(435, 762)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.grpShousai)
        Me.Name = "frmSetFilter"
        Me.Text = "ÉtÉBÉãÉ^Å[ê›íËâÊñ "
        Me.TopMost = False
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.grpShousai, 0)
        Me.Controls.SetChildIndex(Me.Label4, 0)
        Me.Controls.SetChildIndex(Me.GroupBox1, 0)
        Me.Controls.SetChildIndex(Me.Label3, 0)
        Me.grpShousai.ResumeLayout(False)
        Me.grpShousai.PerformLayout()
        CType(Me.dgvShousaiEtc, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvShousaiData, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents grpShousai As System.Windows.Forms.GroupBox
    Friend WithEvents dgvShousaiEtc As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvShousaiData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents radShousai As System.Windows.Forms.RadioButton
    Friend WithEvents radKani As System.Windows.Forms.RadioButton
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents cmbKaniGrade As System.Windows.Forms.ComboBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label

End Class
