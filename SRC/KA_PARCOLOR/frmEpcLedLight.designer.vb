<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcLedLight
    Inherits frmMain_ParSet_UplList_SysName

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
        Me.Label1 = New System.Windows.Forms.Label()
        Me.cmbSCode = New System.Windows.Forms.ComboBox()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.lblSCode = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.grpReset = New System.Windows.Forms.GroupBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.btnBotReset = New System.Windows.Forms.Button()
        Me.btnTopReset = New System.Windows.Forms.Button()
        Me.Label25 = New System.Windows.Forms.Label()
        Me.grpLightControl = New System.Windows.Forms.GroupBox()
        Me.dgvLight = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.pnlItem.SuspendLayout()
        Me.grpBase.SuspendLayout()
        Me.grpReset.SuspendLayout()
        Me.grpLightControl.SuspendLayout()
        CType(Me.dgvLight, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'lblTitolCopySend
        '
        Me.lblTitolCopySend.Text = "�R�s�[���̌����\�ʏ��NO"
        '
        'lblTitolCopyTo
        '
        Me.lblTitolCopyTo.Text = "�R�s�[��̌����\�ʏ��NO"
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.grpLightControl)
        Me.grpBase.Controls.Add(Me.grpReset)
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.cmbSCode)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.lblSCode)
        Me.grpBase.Text = "LED�Ɩ��ݒ�"
        '
        'grpFunction
        '
        Me.grpFunction.Size = New System.Drawing.Size(1240, 68)
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Size = New System.Drawing.Size(93, 32)
        Me.btnF02.Text = "����"
        '
        'lblF02_b
        '
        Me.lblF02_b.Size = New System.Drawing.Size(93, 32)
        '
        'btnF03
        '
        Me.btnF03.Size = New System.Drawing.Size(93, 32)
        '
        'lblF03_b
        '
        Me.lblF03_b.Size = New System.Drawing.Size(93, 32)
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.Size = New System.Drawing.Size(93, 32)
        Me.btnF01.Text = "�O��"
        '
        'lblF01_b
        '
        Me.lblF01_b.Size = New System.Drawing.Size(93, 32)
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Size = New System.Drawing.Size(93, 32)
        Me.btnF12.Text = "�I��"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Size = New System.Drawing.Size(93, 32)
        Me.btnF11.Text = "�ݒ�"
        '
        'btnF10
        '
        Me.btnF10.Size = New System.Drawing.Size(93, 32)
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Size = New System.Drawing.Size(93, 32)
        Me.btnF09.Text = "�ؑ�"
        '
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Size = New System.Drawing.Size(93, 32)
        Me.btnF08.Text = "��ʕۑ�"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Size = New System.Drawing.Size(93, 32)
        Me.btnF07.Text = "��ʈ��"
        '
        'btnF06
        '
        Me.btnF06.Size = New System.Drawing.Size(93, 32)
        '
        'btnF05
        '
        Me.btnF05.Size = New System.Drawing.Size(93, 32)
        '
        'btnF04
        '
        Me.btnF04.Size = New System.Drawing.Size(93, 32)
        '
        'lblF12_b
        '
        Me.lblF12_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF11_b
        '
        Me.lblF11_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF10_b
        '
        Me.lblF10_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF09_b
        '
        Me.lblF09_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF08_b
        '
        Me.lblF08_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF07_b
        '
        Me.lblF07_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF06_b
        '
        Me.lblF06_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF05_b
        '
        Me.lblF05_b.Size = New System.Drawing.Size(93, 32)
        '
        'lblF04_b
        '
        Me.lblF04_b.Size = New System.Drawing.Size(93, 32)
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.ImageAlign = System.Drawing.ContentAlignment.TopLeft
        Me.Label1.Location = New System.Drawing.Point(12, 28)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(154, 24)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "�����\�ʏ��No"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCode
        '
        Me.cmbSCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCode.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbSCode.FormattingEnabled = True
        Me.cmbSCode.Location = New System.Drawing.Point(12, 52)
        Me.cmbSCode.MaxDropDownItems = 16
        Me.cmbSCode.Name = "cmbSCode"
        Me.cmbSCode.Size = New System.Drawing.Size(154, 24)
        Me.cmbSCode.TabIndex = 4
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.ImageAlign = System.Drawing.ContentAlignment.TopLeft
        Me.Label4.Location = New System.Drawing.Point(171, 28)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(299, 24)
        Me.Label4.TabIndex = 8
        Me.Label4.Text = "�����\�ʏ�Ԗ���"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblSCode
        '
        Me.lblSCode.BackColor = System.Drawing.Color.LightSteelBlue
        Me.lblSCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSCode.Font = New System.Drawing.Font("�l�r ����", 12.0!)
        Me.lblSCode.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblSCode.Location = New System.Drawing.Point(172, 52)
        Me.lblSCode.Name = "lblSCode"
        Me.lblSCode.Size = New System.Drawing.Size(300, 24)
        Me.lblSCode.TabIndex = 9
        Me.lblSCode.Text = "12345679801234567890123456789012"
        Me.lblSCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label5
        '
        Me.Label5.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(12, 528)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(496, 100)
        Me.Label5.TabIndex = 28
        Me.Label5.Text = "���z������f�[�^�͈ȉ��̌v�Z���ŋ��߂��l��\�����܂��B" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "�@�@�E�S�̒����f�[�^�iM_RSW�j" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "�@�@�E�z�������f�[�^�iEQ_DATA�j" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "�@�@�E�z������f�[�^�iL" &
    "C_DATA�j" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & " " & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "     LC_DATA = (M_RSW / 2) + (M_RSW * EQ_DATA / 512) + 1"
        '
        'grpReset
        '
        Me.grpReset.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.grpReset.Controls.Add(Me.Label2)
        Me.grpReset.Controls.Add(Me.Label3)
        Me.grpReset.Controls.Add(Me.Label14)
        Me.grpReset.Controls.Add(Me.btnBotReset)
        Me.grpReset.Controls.Add(Me.btnTopReset)
        Me.grpReset.Controls.Add(Me.Label25)
        Me.grpReset.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpReset.ForeColor = System.Drawing.Color.Black
        Me.grpReset.Location = New System.Drawing.Point(12, 748)
        Me.grpReset.Name = "grpReset"
        Me.grpReset.Size = New System.Drawing.Size(620, 160)
        Me.grpReset.TabIndex = 6
        Me.grpReset.TabStop = False
        Me.grpReset.Text = "���ʐݒ�"
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.Color.Black
        Me.Label2.Location = New System.Drawing.Point(328, 32)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(92, 24)
        Me.Label2.TabIndex = 29
        Me.Label2.Text = "����"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.ForeColor = System.Drawing.Color.Black
        Me.Label3.Location = New System.Drawing.Point(224, 32)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(92, 24)
        Me.Label3.TabIndex = 28
        Me.Label3.Text = "�\��"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label14
        '
        Me.Label14.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label14.Location = New System.Drawing.Point(112, 120)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(496, 24)
        Me.Label14.TabIndex = 27
        Me.Label14.Text = "�����Z�b�g���s����LED�Ɩ��ғ��݌v���Ԃ�0�N���A����܂��B"
        '
        'btnBotReset
        '
        Me.btnBotReset.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnBotReset.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnBotReset.Location = New System.Drawing.Point(328, 68)
        Me.btnBotReset.Name = "btnBotReset"
        Me.btnBotReset.Size = New System.Drawing.Size(92, 32)
        Me.btnBotReset.TabIndex = 2
        Me.btnBotReset.Text = "���Z�b�g"
        Me.btnBotReset.UseVisualStyleBackColor = True
        '
        'btnTopReset
        '
        Me.btnTopReset.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnTopReset.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnTopReset.Location = New System.Drawing.Point(224, 68)
        Me.btnTopReset.Name = "btnTopReset"
        Me.btnTopReset.Size = New System.Drawing.Size(92, 32)
        Me.btnTopReset.TabIndex = 1
        Me.btnTopReset.Text = "���Z�b�g"
        Me.btnTopReset.UseVisualStyleBackColor = True
        '
        'Label25
        '
        Me.Label25.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label25.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label25.ForeColor = System.Drawing.Color.Black
        Me.Label25.Location = New System.Drawing.Point(36, 68)
        Me.Label25.Name = "Label25"
        Me.Label25.Size = New System.Drawing.Size(172, 32)
        Me.Label25.TabIndex = 7
        Me.Label25.Text = "LED�Ɩ��ғ��݌v����"
        Me.Label25.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'grpLightControl
        '
        Me.grpLightControl.Controls.Add(Me.dgvLight)
        Me.grpLightControl.Controls.Add(Me.Label5)
        Me.grpLightControl.Font = New System.Drawing.Font("�l�r ����", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpLightControl.ForeColor = System.Drawing.Color.Black
        Me.grpLightControl.Location = New System.Drawing.Point(12, 88)
        Me.grpLightControl.Name = "grpLightControl"
        Me.grpLightControl.Size = New System.Drawing.Size(1052, 640)
        Me.grpLightControl.TabIndex = 10
        Me.grpLightControl.TabStop = False
        Me.grpLightControl.Text = "�����ݒ�"
        '
        'dgvLight
        '
        Me.dgvLight.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvLight.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvLight.Location = New System.Drawing.Point(8, 32)
        Me.dgvLight.Name = "dgvLight"
        Me.dgvLight.RowTemplate.Height = 21
        Me.dgvLight.Size = New System.Drawing.Size(1032, 480)
        Me.dgvLight.TabIndex = 29
        '
        'frmEpcLedLight
        '
        Me.ClientSize = New System.Drawing.Size(1268, 989)
        Me.Name = "frmEpcLedLight"
        Me.Text = "LED�Ɩ��ݒ���"
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        Me.grpReset.ResumeLayout(False)
        Me.grpLightControl.ResumeLayout(False)
        CType(Me.dgvLight, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents cmbSCode As System.Windows.Forms.ComboBox
	Friend WithEvents Label4 As System.Windows.Forms.Label
	Friend WithEvents lblSCode As System.Windows.Forms.Label
	Friend WithEvents grpReset As System.Windows.Forms.GroupBox
	Friend WithEvents Label25 As System.Windows.Forms.Label
	Friend WithEvents btnBotReset As System.Windows.Forms.Button
	Friend WithEvents btnTopReset As System.Windows.Forms.Button
	Friend WithEvents Label14 As System.Windows.Forms.Label
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents Label3 As System.Windows.Forms.Label
	Friend WithEvents Label5 As System.Windows.Forms.Label
	Friend WithEvents grpLightControl As System.Windows.Forms.GroupBox
	Friend WithEvents dgvLight As tClassLibrary.tCnt.tCnt_DataGridView

End Class
