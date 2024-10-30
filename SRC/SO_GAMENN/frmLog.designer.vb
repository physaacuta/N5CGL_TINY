<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmLog
    Inherits frmMain_Log2

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
        Me.dgvPcName = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvPcKind = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpFilter_Kubun.SuspendLayout()
        Me.m_grpFilter_Code.SuspendLayout()
        Me.grpFilter_PC.SuspendLayout()
        Me.grpFilter_Time.SuspendLayout()
        Me.grpFilter_PCKIND.SuspendLayout()
        CType(Me.dgvPcName, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvPcKind, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpFilter_Kubun
        '
        Me.grpFilter_Kubun.Location = New System.Drawing.Point(4, 96)
        Me.grpFilter_Kubun.Size = New System.Drawing.Size(368, 111)
        Me.grpFilter_Kubun.TabIndex = 3
        '
        'chkKubun_A_Evt
        '
        Me.chkKubun_A_Evt.BackColor = System.Drawing.SystemColors.Control
        Me.chkKubun_A_Evt.TabIndex = 4
        Me.chkKubun_A_Evt.UseVisualStyleBackColor = False
        '
        'chkKubun_A_Kei
        '
        Me.chkKubun_A_Kei.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_A_Kei.Checked = True
        Me.chkKubun_A_Kei.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_A_Kei.TabIndex = 3
        Me.chkKubun_A_Kei.UseVisualStyleBackColor = False
        '
        'chkKubun_A_Jyu
        '
        Me.chkKubun_A_Jyu.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_A_Jyu.Checked = True
        Me.chkKubun_A_Jyu.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_A_Jyu.TabIndex = 2
        Me.chkKubun_A_Jyu.UseVisualStyleBackColor = False
        '
        'm_grpFilter_Code
        '
        Me.m_grpFilter_Code.Location = New System.Drawing.Point(375, 3)
        Me.m_grpFilter_Code.Size = New System.Drawing.Size(208, 204)
        Me.m_grpFilter_Code.TabIndex = 4
        '
        'grpFilter_PC
        '
        Me.grpFilter_PC.Controls.Add(Me.dgvPcName)
        Me.grpFilter_PC.Location = New System.Drawing.Point(586, 107)
        Me.grpFilter_PC.Size = New System.Drawing.Size(649, 100)
        Me.grpFilter_PC.TabIndex = 6
        '
        'grpFilter_Time
        '
        Me.grpFilter_Time.Location = New System.Drawing.Point(4, 2)
        Me.grpFilter_Time.Size = New System.Drawing.Size(368, 92)
        '
        'dtpEndDay
        '
        Me.dtpEndDay.Value = New Date(2010, 9, 22, 23, 59, 59, 0)
        '
        'dtpEndTime
        '
        Me.dtpEndTime.TabIndex = 6
        Me.dtpEndTime.Value = New Date(2010, 9, 22, 23, 59, 59, 0)
        '
        'dtpStartDay
        '
        Me.dtpStartDay.Value = New Date(2010, 9, 22, 0, 0, 0, 0)
        '
        'dtpStartTime
        '
        Me.dtpStartTime.TabIndex = 4
        Me.dtpStartTime.Value = New Date(2010, 9, 22, 0, 0, 0, 0)
        '
        'txtCode_To
        '
        Me.txtCode_To.TabIndex = 2
        '
        'txtCode_Not
        '
        Me.txtCode_Not.TabIndex = 4
        '
        'txtCode_Select
        '
        Me.txtCode_Select.TabIndex = 3
        '
        'txtCode_From
        '
        Me.txtCode_From.TabIndex = 1
        '
        'chkKubun_H_Evt
        '
        Me.chkKubun_H_Evt.BackColor = System.Drawing.SystemColors.Control
        Me.chkKubun_H_Evt.TabIndex = 13
        Me.chkKubun_H_Evt.UseVisualStyleBackColor = False
        '
        'chkKubun_H_Kei
        '
        Me.chkKubun_H_Kei.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_H_Kei.Checked = True
        Me.chkKubun_H_Kei.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_H_Kei.TabIndex = 12
        Me.chkKubun_H_Kei.UseVisualStyleBackColor = False
        '
        'chkKubun_S_Evt
        '
        Me.chkKubun_S_Evt.BackColor = System.Drawing.SystemColors.Control
        Me.chkKubun_S_Evt.TabIndex = 8
        Me.chkKubun_S_Evt.UseVisualStyleBackColor = False
        '
        'chkKubun_S_Kei
        '
        Me.chkKubun_S_Kei.BackColor = System.Drawing.SystemColors.Control
        Me.chkKubun_S_Kei.TabIndex = 7
        Me.chkKubun_S_Kei.UseVisualStyleBackColor = False
        '
        'chkKubun_H_Jyu
        '
        Me.chkKubun_H_Jyu.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_H_Jyu.Checked = True
        Me.chkKubun_H_Jyu.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_H_Jyu.TabIndex = 10
        Me.chkKubun_H_Jyu.UseVisualStyleBackColor = False
        '
        'chkKubun_S_Jyu
        '
        Me.chkKubun_S_Jyu.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_S_Jyu.Checked = True
        Me.chkKubun_S_Jyu.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_S_Jyu.TabIndex = 6
        Me.chkKubun_S_Jyu.UseVisualStyleBackColor = False
        '
        'chkKubun_A_Cmt
        '
        Me.chkKubun_A_Cmt.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_A_Cmt.Checked = True
        Me.chkKubun_A_Cmt.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_A_Cmt.UseVisualStyleBackColor = False
        '
        'chkKubun_H_Cmt
        '
        Me.chkKubun_H_Cmt.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_H_Cmt.Checked = True
        Me.chkKubun_H_Cmt.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_H_Cmt.TabIndex = 9
        Me.chkKubun_H_Cmt.UseVisualStyleBackColor = False
        '
        'chkKubun_S_Cmt
        '
        Me.chkKubun_S_Cmt.BackColor = System.Drawing.Color.Cyan
        Me.chkKubun_S_Cmt.Checked = True
        Me.chkKubun_S_Cmt.CheckState = System.Windows.Forms.CheckState.Checked
        Me.chkKubun_S_Cmt.TabIndex = 5
        Me.chkKubun_S_Cmt.UseVisualStyleBackColor = False
        '
        'grpFilter_PCKIND
        '
        Me.grpFilter_PCKIND.Controls.Add(Me.dgvPcKind)
        Me.grpFilter_PCKIND.Location = New System.Drawing.Point(586, 2)
        Me.grpFilter_PCKIND.Size = New System.Drawing.Size(649, 100)
        '
        'btnF02
        '
        Me.btnF02.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF03
        '
        Me.btnF03.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnF01.Text = "RAS" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ï\é¶âÊñ "
        '
        'btnF12
        '
        Me.btnF12.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF11
        '
        Me.btnF11.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF10
        '
        Me.btnF10.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF09
        '
        Me.btnF09.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF08
        '
        Me.btnF08.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF07
        '
        Me.btnF07.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF06
        '
        Me.btnF06.Enabled = True
        Me.btnF06.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF05
        '
        Me.btnF05.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'btnF04
        '
        Me.btnF04.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        '
        'dgvPcName
        '
        Me.dgvPcName.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvPcName.Location = New System.Drawing.Point(3, 19)
        Me.dgvPcName.Name = "dgvPcName"
        Me.dgvPcName.RowTemplate.Height = 21
        Me.dgvPcName.Size = New System.Drawing.Size(643, 78)
        Me.dgvPcName.TabIndex = 1
        '
        'dgvPcKind
        '
        Me.dgvPcKind.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvPcKind.Location = New System.Drawing.Point(3, 19)
        Me.dgvPcKind.Name = "dgvPcKind"
        Me.dgvPcKind.RowTemplate.Height = 21
        Me.dgvPcKind.Size = New System.Drawing.Size(643, 78)
        Me.dgvPcKind.TabIndex = 2
        '
        'frmLog
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Name = "frmLog"
        Me.Text = "åÃè·ÉçÉOâÊñ "
        Me.tProp_IsComment = True
        Me.grpFilter_Kubun.ResumeLayout(False)
        Me.grpFilter_Kubun.PerformLayout()
        Me.m_grpFilter_Code.ResumeLayout(False)
        Me.m_grpFilter_Code.PerformLayout()
        Me.grpFilter_PC.ResumeLayout(False)
        Me.grpFilter_Time.ResumeLayout(False)
        Me.grpFilter_Time.PerformLayout()
        Me.grpFilter_PCKIND.ResumeLayout(False)
        CType(Me.dgvPcName, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvPcKind, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvPcName As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvPcKind As tClassLibrary.tCnt.tCnt_DataGridView
End Class
