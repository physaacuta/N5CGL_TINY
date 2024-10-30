<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSystemStatus
    Inherits tClassForm.frmBase_Popup

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
        Me.components = New System.ComponentModel.Container()
        Me.cmdEnd = New System.Windows.Forms.Button()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.btnKensaEnd = New System.Windows.Forms.Button()
        Me.btnKensaStart = New System.Windows.Forms.Button()
        Me.lblSystemStatus = New System.Windows.Forms.Label()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.rdoSmyu = New System.Windows.Forms.RadioButton()
        Me.rdoCamera = New System.Windows.Forms.RadioButton()
        Me.rdoSamp = New System.Windows.Forms.RadioButton()
        Me.rdoNormal = New System.Windows.Forms.RadioButton()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.rdoPlgBot = New System.Windows.Forms.RadioButton()
        Me.rdoPlgTop = New System.Windows.Forms.RadioButton()
        Me.rdoPlgTest = New System.Windows.Forms.RadioButton()
        Me.rdoPlgLine = New System.Windows.Forms.RadioButton()
        Me.timCheck = New System.Windows.Forms.Timer(Me.components)
        Me.timTimeOut = New System.Windows.Forms.Timer(Me.components)
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.rdoKatamenYes = New System.Windows.Forms.RadioButton()
        Me.rdoKatamenNo = New System.Windows.Forms.RadioButton()
        Me.prbTimeOut = New System.Windows.Forms.ProgressBar()
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        Me.btnHTEnable = New System.Windows.Forms.Button()
        Me.btnSystemEnd = New System.Windows.Forms.Button()
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.lblPCodeBot = New System.Windows.Forms.Label()
        Me.cmbPCodeBot = New System.Windows.Forms.ComboBox()
        Me.lblSCodeBot = New System.Windows.Forms.Label()
        Me.cmbSCodeBot = New System.Windows.Forms.ComboBox()
        Me.lblPCodeTop = New System.Windows.Forms.Label()
        Me.cmbPCodeTop = New System.Windows.Forms.ComboBox()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.lblSCodeTop = New System.Windows.Forms.Label()
        Me.cmbSCodeTop = New System.Windows.Forms.ComboBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox7 = New System.Windows.Forms.GroupBox()
        Me.GroupBox8 = New System.Windows.Forms.GroupBox()
        Me.lblErrMsg = New System.Windows.Forms.Label()
        Me.btnLPEndPC2 = New System.Windows.Forms.Button()
        Me.btnLPStartPC2 = New System.Windows.Forms.Button()
        Me.btnLPEndPC1 = New System.Windows.Forms.Button()
        Me.btnLPStartPC1 = New System.Windows.Forms.Button()
        Me.cmbCamera2PC2 = New System.Windows.Forms.ComboBox()
        Me.cmbCamera1PC2 = New System.Windows.Forms.ComboBox()
        Me.cmbCamera2PC1 = New System.Windows.Forms.ComboBox()
        Me.cmbCamera1PC1 = New System.Windows.Forms.ComboBox()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.lblLPStatusPC2 = New System.Windows.Forms.Label()
        Me.lblLPStatusPC1 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox4.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        Me.GroupBox6.SuspendLayout()
        Me.GroupBox7.SuspendLayout()
        Me.GroupBox8.SuspendLayout()
        Me.SuspendLayout()
        '
        'cmdEnd
        '
        Me.cmdEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmdEnd.Location = New System.Drawing.Point(524, 700)
        Me.cmdEnd.Name = "cmdEnd"
        Me.cmdEnd.Size = New System.Drawing.Size(120, 32)
        Me.cmdEnd.TabIndex = 4
        Me.cmdEnd.Text = "F12ÅFï¬Ç∂ÇÈ"
        Me.cmdEnd.UseVisualStyleBackColor = True
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.btnKensaEnd)
        Me.GroupBox1.Controls.Add(Me.btnKensaStart)
        Me.GroupBox1.Controls.Add(Me.lblSystemStatus)
        Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox1.Location = New System.Drawing.Point(8, 8)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(412, 108)
        Me.GroupBox1.TabIndex = 5
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "åüç∏èÛë‘"
        '
        'btnKensaEnd
        '
        Me.btnKensaEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnKensaEnd.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnKensaEnd.Location = New System.Drawing.Point(228, 64)
        Me.btnKensaEnd.Name = "btnKensaEnd"
        Me.btnKensaEnd.Size = New System.Drawing.Size(165, 34)
        Me.btnKensaEnd.TabIndex = 2
        Me.btnKensaEnd.Text = "åüç∏í‚é~"
        Me.btnKensaEnd.UseVisualStyleBackColor = True
        '
        'btnKensaStart
        '
        Me.btnKensaStart.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnKensaStart.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnKensaStart.Location = New System.Drawing.Point(228, 24)
        Me.btnKensaStart.Name = "btnKensaStart"
        Me.btnKensaStart.Size = New System.Drawing.Size(165, 34)
        Me.btnKensaStart.TabIndex = 1
        Me.btnKensaStart.Text = "åüç∏äJén"
        Me.btnKensaStart.UseVisualStyleBackColor = True
        '
        'lblSystemStatus
        '
        Me.lblSystemStatus.BackColor = System.Drawing.Color.Blue
        Me.lblSystemStatus.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblSystemStatus.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblSystemStatus.ForeColor = System.Drawing.SystemColors.ControlLightLight
        Me.lblSystemStatus.Location = New System.Drawing.Point(16, 36)
        Me.lblSystemStatus.Name = "lblSystemStatus"
        Me.lblSystemStatus.Size = New System.Drawing.Size(196, 52)
        Me.lblSystemStatus.TabIndex = 0
        Me.lblSystemStatus.Text = "ÉVÉXÉeÉÄâ“ìÆíÜ"
        Me.lblSystemStatus.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.rdoSmyu)
        Me.GroupBox2.Controls.Add(Me.rdoCamera)
        Me.GroupBox2.Controls.Add(Me.rdoSamp)
        Me.GroupBox2.Controls.Add(Me.rdoNormal)
        Me.GroupBox2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox2.Location = New System.Drawing.Point(8, 216)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(204, 200)
        Me.GroupBox2.TabIndex = 6
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "â^ì]èÛë‘"
        '
        'rdoSmyu
        '
        Me.rdoSmyu.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoSmyu.Location = New System.Drawing.Point(20, 156)
        Me.rdoSmyu.Name = "rdoSmyu"
        Me.rdoSmyu.Size = New System.Drawing.Size(164, 36)
        Me.rdoSmyu.TabIndex = 5
        Me.rdoSmyu.TabStop = True
        Me.rdoSmyu.Text = "ÉVÉ~ÉÖÉåÅ[ÉVÉáÉì"
        Me.rdoSmyu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoSmyu.UseVisualStyleBackColor = True
        '
        'rdoCamera
        '
        Me.rdoCamera.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoCamera.Location = New System.Drawing.Point(20, 112)
        Me.rdoCamera.Name = "rdoCamera"
        Me.rdoCamera.Size = New System.Drawing.Size(164, 36)
        Me.rdoCamera.TabIndex = 4
        Me.rdoCamera.TabStop = True
        Me.rdoCamera.Text = "ÉJÉÅÉâí≤êÆ"
        Me.rdoCamera.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoCamera.UseVisualStyleBackColor = True
        '
        'rdoSamp
        '
        Me.rdoSamp.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoSamp.Enabled = False
        Me.rdoSamp.Location = New System.Drawing.Point(20, 68)
        Me.rdoSamp.Name = "rdoSamp"
        Me.rdoSamp.Size = New System.Drawing.Size(164, 36)
        Me.rdoSamp.TabIndex = 3
        Me.rdoSamp.TabStop = True
        Me.rdoSamp.Text = "ÉTÉìÉvÉãéBâe"
        Me.rdoSamp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoSamp.UseVisualStyleBackColor = True
        '
        'rdoNormal
        '
        Me.rdoNormal.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoNormal.BackColor = System.Drawing.SystemColors.Control
        Me.rdoNormal.Location = New System.Drawing.Point(20, 24)
        Me.rdoNormal.Name = "rdoNormal"
        Me.rdoNormal.Size = New System.Drawing.Size(164, 36)
        Me.rdoNormal.TabIndex = 2
        Me.rdoNormal.TabStop = True
        Me.rdoNormal.Text = "í èÌâ^ì]"
        Me.rdoNormal.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoNormal.UseVisualStyleBackColor = False
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.rdoPlgBot)
        Me.GroupBox3.Controls.Add(Me.rdoPlgTop)
        Me.GroupBox3.Controls.Add(Me.rdoPlgTest)
        Me.GroupBox3.Controls.Add(Me.rdoPlgLine)
        Me.GroupBox3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox3.Location = New System.Drawing.Point(216, 216)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(204, 200)
        Me.GroupBox3.TabIndex = 7
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "PLGãÊï™"
        '
        'rdoPlgBot
        '
        Me.rdoPlgBot.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoPlgBot.BackColor = System.Drawing.SystemColors.Control
        Me.rdoPlgBot.Enabled = False
        Me.rdoPlgBot.Location = New System.Drawing.Point(20, 112)
        Me.rdoPlgBot.Name = "rdoPlgBot"
        Me.rdoPlgBot.Size = New System.Drawing.Size(165, 36)
        Me.rdoPlgBot.TabIndex = 3
        Me.rdoPlgBot.TabStop = True
        Me.rdoPlgBot.Text = "ó†çZê≥"
        Me.rdoPlgBot.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoPlgBot.UseVisualStyleBackColor = False
        '
        'rdoPlgTop
        '
        Me.rdoPlgTop.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoPlgTop.BackColor = System.Drawing.SystemColors.Control
        Me.rdoPlgTop.Enabled = False
        Me.rdoPlgTop.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.rdoPlgTop.Location = New System.Drawing.Point(20, 68)
        Me.rdoPlgTop.Name = "rdoPlgTop"
        Me.rdoPlgTop.Size = New System.Drawing.Size(165, 36)
        Me.rdoPlgTop.TabIndex = 2
        Me.rdoPlgTop.TabStop = True
        Me.rdoPlgTop.Text = "ï\çZê≥"
        Me.rdoPlgTop.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoPlgTop.UseVisualStyleBackColor = False
        '
        'rdoPlgTest
        '
        Me.rdoPlgTest.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoPlgTest.Location = New System.Drawing.Point(20, 156)
        Me.rdoPlgTest.Name = "rdoPlgTest"
        Me.rdoPlgTest.Size = New System.Drawing.Size(164, 36)
        Me.rdoPlgTest.TabIndex = 1
        Me.rdoPlgTest.TabStop = True
        Me.rdoPlgTest.Text = "TESTópÉpÉãÉX"
        Me.rdoPlgTest.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoPlgTest.UseVisualStyleBackColor = True
        '
        'rdoPlgLine
        '
        Me.rdoPlgLine.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoPlgLine.BackColor = System.Drawing.SystemColors.Control
        Me.rdoPlgLine.Location = New System.Drawing.Point(20, 24)
        Me.rdoPlgLine.Name = "rdoPlgLine"
        Me.rdoPlgLine.Size = New System.Drawing.Size(164, 36)
        Me.rdoPlgLine.TabIndex = 0
        Me.rdoPlgLine.TabStop = True
        Me.rdoPlgLine.Text = "ÉâÉCÉìPLG"
        Me.rdoPlgLine.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoPlgLine.UseVisualStyleBackColor = False
        '
        'timCheck
        '
        '
        'timTimeOut
        '
        Me.timTimeOut.Interval = 60000
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.Label6)
        Me.GroupBox4.Controls.Add(Me.Label5)
        Me.GroupBox4.Controls.Add(Me.rdoKatamenYes)
        Me.GroupBox4.Controls.Add(Me.rdoKatamenNo)
        Me.GroupBox4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox4.Location = New System.Drawing.Point(424, 216)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(204, 200)
        Me.GroupBox4.TabIndex = 8
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "ï–ñ åüç∏èÛë‘"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(8, 152)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(189, 39)
        Me.Label6.TabIndex = 3
        Me.Label6.Text = "Å¶ óºñ Ç™" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "àŸèÌí‚é~ÇµÇΩÉ^ÉCÉ~ÉìÉOÇ≈ÅA" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ã≠êßçƒãNìÆÇé¿é{ÇµÇ‹Ç∑ÅB"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(8, 64)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(189, 39)
        Me.Label5.TabIndex = 3
        Me.Label5.Text = "Å¶ Ç«ÇøÇÁÇ©ÇÃñ Ç™" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "àŸèÌí‚é~ÇµÇΩÉ^ÉCÉ~ÉìÉOÇ≈ÅA" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ã≠êßçƒãNìÆÇé¿é{ÇµÇ‹Ç∑ÅB"
        '
        'rdoKatamenYes
        '
        Me.rdoKatamenYes.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoKatamenYes.BackColor = System.Drawing.SystemColors.Control
        Me.rdoKatamenYes.Location = New System.Drawing.Point(20, 112)
        Me.rdoKatamenYes.Name = "rdoKatamenYes"
        Me.rdoKatamenYes.Size = New System.Drawing.Size(165, 34)
        Me.rdoKatamenYes.TabIndex = 2
        Me.rdoKatamenYes.TabStop = True
        Me.rdoKatamenYes.Text = "ï–ñ åüç∏Å@ãñâ¬"
        Me.rdoKatamenYes.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoKatamenYes.UseVisualStyleBackColor = False
        '
        'rdoKatamenNo
        '
        Me.rdoKatamenNo.Appearance = System.Windows.Forms.Appearance.Button
        Me.rdoKatamenNo.BackColor = System.Drawing.SystemColors.Control
        Me.rdoKatamenNo.Location = New System.Drawing.Point(20, 24)
        Me.rdoKatamenNo.Name = "rdoKatamenNo"
        Me.rdoKatamenNo.Size = New System.Drawing.Size(165, 34)
        Me.rdoKatamenNo.TabIndex = 0
        Me.rdoKatamenNo.TabStop = True
        Me.rdoKatamenNo.Text = "ï–ñ åüç∏Å@ïsãñâ¬"
        Me.rdoKatamenNo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.rdoKatamenNo.UseVisualStyleBackColor = False
        '
        'prbTimeOut
        '
        Me.prbTimeOut.Location = New System.Drawing.Point(8, 124)
        Me.prbTimeOut.Name = "prbTimeOut"
        Me.prbTimeOut.Size = New System.Drawing.Size(636, 28)
        Me.prbTimeOut.Style = System.Windows.Forms.ProgressBarStyle.Continuous
        Me.prbTimeOut.TabIndex = 9
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.btnHTEnable)
        Me.GroupBox5.Controls.Add(Me.btnSystemEnd)
        Me.GroupBox5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle), System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox5.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox5.Location = New System.Drawing.Point(424, 8)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(220, 108)
        Me.GroupBox5.TabIndex = 10
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "ÉVÉXÉeÉÄèÛë‘"
        '
        'btnHTEnable
        '
        Me.btnHTEnable.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnHTEnable.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnHTEnable.Location = New System.Drawing.Point(28, 100)
        Me.btnHTEnable.Name = "btnHTEnable"
        Me.btnHTEnable.Size = New System.Drawing.Size(165, 34)
        Me.btnHTEnable.TabIndex = 13
        Me.btnHTEnable.Text = "ÉJÉÅÉâîÒégópê›íË"
        Me.btnHTEnable.UseVisualStyleBackColor = True
        Me.btnHTEnable.Visible = False
        '
        'btnSystemEnd
        '
        Me.btnSystemEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSystemEnd.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnSystemEnd.Location = New System.Drawing.Point(28, 44)
        Me.btnSystemEnd.Name = "btnSystemEnd"
        Me.btnSystemEnd.Size = New System.Drawing.Size(165, 34)
        Me.btnSystemEnd.TabIndex = 3
        Me.btnSystemEnd.Text = "ÉVÉXÉeÉÄèIóπ"
        Me.btnSystemEnd.UseVisualStyleBackColor = True
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.Label11)
        Me.GroupBox6.Controls.Add(Me.Label12)
        Me.GroupBox6.Controls.Add(Me.Label10)
        Me.GroupBox6.Controls.Add(Me.Label9)
        Me.GroupBox6.Controls.Add(Me.lblPCodeBot)
        Me.GroupBox6.Controls.Add(Me.cmbPCodeBot)
        Me.GroupBox6.Controls.Add(Me.lblSCodeBot)
        Me.GroupBox6.Controls.Add(Me.cmbSCodeBot)
        Me.GroupBox6.Controls.Add(Me.lblPCodeTop)
        Me.GroupBox6.Controls.Add(Me.cmbPCodeTop)
        Me.GroupBox6.Controls.Add(Me.Label3)
        Me.GroupBox6.Controls.Add(Me.Label4)
        Me.GroupBox6.Controls.Add(Me.lblSCodeTop)
        Me.GroupBox6.Controls.Add(Me.cmbSCodeTop)
        Me.GroupBox6.Controls.Add(Me.Label2)
        Me.GroupBox6.Controls.Add(Me.Label1)
        Me.GroupBox6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox6.Location = New System.Drawing.Point(8, 24)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(620, 188)
        Me.GroupBox6.TabIndex = 11
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "åüç∏èåèèâä˙íl"
        '
        'Label11
        '
        Me.Label11.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label11.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label11.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label11.Location = New System.Drawing.Point(20, 156)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(28, 24)
        Me.Label11.TabIndex = 29
        Me.Label11.Text = "ó†"
        Me.Label11.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label12
        '
        Me.Label12.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label12.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label12.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label12.Location = New System.Drawing.Point(20, 128)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(28, 24)
        Me.Label12.TabIndex = 28
        Me.Label12.Text = "ï\"
        Me.Label12.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label10
        '
        Me.Label10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label10.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label10.Location = New System.Drawing.Point(20, 76)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(28, 24)
        Me.Label10.TabIndex = 27
        Me.Label10.Text = "ó†"
        Me.Label10.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label9
        '
        Me.Label9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label9.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label9.Location = New System.Drawing.Point(20, 48)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(28, 24)
        Me.Label9.TabIndex = 26
        Me.Label9.Text = "ï\"
        Me.Label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblPCodeBot
        '
        Me.lblPCodeBot.BackColor = System.Drawing.Color.PeachPuff
        Me.lblPCodeBot.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblPCodeBot.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.lblPCodeBot.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblPCodeBot.Location = New System.Drawing.Point(260, 156)
        Me.lblPCodeBot.Name = "lblPCodeBot"
        Me.lblPCodeBot.Size = New System.Drawing.Size(300, 24)
        Me.lblPCodeBot.TabIndex = 25
        Me.lblPCodeBot.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbPCodeBot
        '
        Me.cmbPCodeBot.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbPCodeBot.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbPCodeBot.FormattingEnabled = True
        Me.cmbPCodeBot.Location = New System.Drawing.Point(52, 156)
        Me.cmbPCodeBot.MaxDropDownItems = 16
        Me.cmbPCodeBot.Name = "cmbPCodeBot"
        Me.cmbPCodeBot.Size = New System.Drawing.Size(192, 24)
        Me.cmbPCodeBot.TabIndex = 19
        '
        'lblSCodeBot
        '
        Me.lblSCodeBot.BackColor = System.Drawing.Color.PeachPuff
        Me.lblSCodeBot.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSCodeBot.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.lblSCodeBot.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblSCodeBot.Location = New System.Drawing.Point(260, 76)
        Me.lblSCodeBot.Name = "lblSCodeBot"
        Me.lblSCodeBot.Size = New System.Drawing.Size(300, 24)
        Me.lblSCodeBot.TabIndex = 23
        Me.lblSCodeBot.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCodeBot
        '
        Me.cmbSCodeBot.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCodeBot.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbSCodeBot.FormattingEnabled = True
        Me.cmbSCodeBot.Location = New System.Drawing.Point(52, 76)
        Me.cmbSCodeBot.MaxDropDownItems = 16
        Me.cmbSCodeBot.Name = "cmbSCodeBot"
        Me.cmbSCodeBot.Size = New System.Drawing.Size(192, 24)
        Me.cmbSCodeBot.TabIndex = 17
        '
        'lblPCodeTop
        '
        Me.lblPCodeTop.BackColor = System.Drawing.Color.PeachPuff
        Me.lblPCodeTop.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblPCodeTop.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.lblPCodeTop.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblPCodeTop.Location = New System.Drawing.Point(260, 128)
        Me.lblPCodeTop.Name = "lblPCodeTop"
        Me.lblPCodeTop.Size = New System.Drawing.Size(300, 24)
        Me.lblPCodeTop.TabIndex = 21
        Me.lblPCodeTop.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbPCodeTop
        '
        Me.cmbPCodeTop.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbPCodeTop.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbPCodeTop.FormattingEnabled = True
        Me.cmbPCodeTop.Location = New System.Drawing.Point(52, 128)
        Me.cmbPCodeTop.MaxDropDownItems = 16
        Me.cmbPCodeTop.Name = "cmbPCodeTop"
        Me.cmbPCodeTop.Size = New System.Drawing.Size(192, 24)
        Me.cmbPCodeTop.TabIndex = 18
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label3.Location = New System.Drawing.Point(260, 104)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(300, 24)
        Me.Label3.TabIndex = 19
        Me.Label3.Text = "åüç∏ÉpÉ^Å[ÉìñºèÃ"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label4
        '
        Me.Label4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(52, 104)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(192, 24)
        Me.Label4.TabIndex = 18
        Me.Label4.Text = "åüç∏ÉpÉ^Å[ÉìÉRÅ[Éh"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblSCodeTop
        '
        Me.lblSCodeTop.BackColor = System.Drawing.Color.PeachPuff
        Me.lblSCodeTop.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSCodeTop.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.lblSCodeTop.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblSCodeTop.Location = New System.Drawing.Point(260, 48)
        Me.lblSCodeTop.Name = "lblSCodeTop"
        Me.lblSCodeTop.Size = New System.Drawing.Size(300, 24)
        Me.lblSCodeTop.TabIndex = 17
        Me.lblSCodeTop.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCodeTop
        '
        Me.cmbSCodeTop.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCodeTop.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbSCodeTop.FormattingEnabled = True
        Me.cmbSCodeTop.Location = New System.Drawing.Point(52, 48)
        Me.cmbSCodeTop.MaxDropDownItems = 16
        Me.cmbSCodeTop.Name = "cmbSCodeTop"
        Me.cmbSCodeTop.Size = New System.Drawing.Size(192, 24)
        Me.cmbSCodeTop.TabIndex = 16
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(260, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(300, 24)
        Me.Label2.TabIndex = 15
        Me.Label2.Text = "åüç∏ï\ñ èÛë‘ñºèÃ"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(52, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(192, 24)
        Me.Label1.TabIndex = 14
        Me.Label1.Text = "åüç∏ï\ñ èÛë‘No"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'GroupBox7
        '
        Me.GroupBox7.Controls.Add(Me.GroupBox6)
        Me.GroupBox7.Controls.Add(Me.GroupBox2)
        Me.GroupBox7.Controls.Add(Me.GroupBox3)
        Me.GroupBox7.Controls.Add(Me.GroupBox4)
        Me.GroupBox7.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle))
        Me.GroupBox7.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox7.Location = New System.Drawing.Point(8, 160)
        Me.GroupBox7.Name = "GroupBox7"
        Me.GroupBox7.Size = New System.Drawing.Size(636, 420)
        Me.GroupBox7.TabIndex = 12
        Me.GroupBox7.TabStop = False
        Me.GroupBox7.Text = "åüç∏äJénèåè"
        '
        'GroupBox8
        '
        Me.GroupBox8.Controls.Add(Me.lblErrMsg)
        Me.GroupBox8.Controls.Add(Me.btnLPEndPC2)
        Me.GroupBox8.Controls.Add(Me.btnLPStartPC2)
        Me.GroupBox8.Controls.Add(Me.btnLPEndPC1)
        Me.GroupBox8.Controls.Add(Me.btnLPStartPC1)
        Me.GroupBox8.Controls.Add(Me.cmbCamera2PC2)
        Me.GroupBox8.Controls.Add(Me.cmbCamera1PC2)
        Me.GroupBox8.Controls.Add(Me.cmbCamera2PC1)
        Me.GroupBox8.Controls.Add(Me.cmbCamera1PC1)
        Me.GroupBox8.Controls.Add(Me.Label15)
        Me.GroupBox8.Controls.Add(Me.lblLPStatusPC2)
        Me.GroupBox8.Controls.Add(Me.lblLPStatusPC1)
        Me.GroupBox8.Controls.Add(Me.Label7)
        Me.GroupBox8.Controls.Add(Me.Label8)
        Me.GroupBox8.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox8.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox8.Location = New System.Drawing.Point(8, 583)
        Me.GroupBox8.Name = "GroupBox8"
        Me.GroupBox8.Size = New System.Drawing.Size(636, 112)
        Me.GroupBox8.TabIndex = 13
        Me.GroupBox8.TabStop = False
        Me.GroupBox8.Text = "ÉçÉWÉbÉNÉpÉâÉâÉìã@î\"
        '
        'lblErrMsg
        '
        Me.lblErrMsg.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblErrMsg.ForeColor = System.Drawing.SystemColors.ControlText
        Me.lblErrMsg.Location = New System.Drawing.Point(8, 18)
        Me.lblErrMsg.Name = "lblErrMsg"
        Me.lblErrMsg.Size = New System.Drawing.Size(279, 24)
        Me.lblErrMsg.TabIndex = 45
        Me.lblErrMsg.Text = "DBçXêVé∏îsÅ@ÉJÉÅÉâï€éùÇ…é∏îsÇµÇ‹ÇµÇΩÅB"
        Me.lblErrMsg.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnLPEndPC2
        '
        Me.btnLPEndPC2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnLPEndPC2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnLPEndPC2.Location = New System.Drawing.Point(534, 78)
        Me.btnLPEndPC2.Name = "btnLPEndPC2"
        Me.btnLPEndPC2.Size = New System.Drawing.Size(87, 24)
        Me.btnLPEndPC2.TabIndex = 44
        Me.btnLPEndPC2.Text = "åüç∏í‚é~"
        Me.btnLPEndPC2.UseVisualStyleBackColor = True
        '
        'btnLPStartPC2
        '
        Me.btnLPStartPC2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnLPStartPC2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnLPStartPC2.Location = New System.Drawing.Point(441, 78)
        Me.btnLPStartPC2.Name = "btnLPStartPC2"
        Me.btnLPStartPC2.Size = New System.Drawing.Size(87, 24)
        Me.btnLPStartPC2.TabIndex = 43
        Me.btnLPStartPC2.Text = "åüç∏äJén"
        Me.btnLPStartPC2.UseVisualStyleBackColor = True
        '
        'btnLPEndPC1
        '
        Me.btnLPEndPC1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnLPEndPC1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnLPEndPC1.Location = New System.Drawing.Point(534, 46)
        Me.btnLPEndPC1.Name = "btnLPEndPC1"
        Me.btnLPEndPC1.Size = New System.Drawing.Size(87, 24)
        Me.btnLPEndPC1.TabIndex = 42
        Me.btnLPEndPC1.Text = "åüç∏í‚é~"
        Me.btnLPEndPC1.UseVisualStyleBackColor = True
        '
        'btnLPStartPC1
        '
        Me.btnLPStartPC1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.btnLPStartPC1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.btnLPStartPC1.Location = New System.Drawing.Point(441, 46)
        Me.btnLPStartPC1.Name = "btnLPStartPC1"
        Me.btnLPStartPC1.Size = New System.Drawing.Size(87, 24)
        Me.btnLPStartPC1.TabIndex = 41
        Me.btnLPStartPC1.Text = "åüç∏äJén"
        Me.btnLPStartPC1.UseVisualStyleBackColor = True
        '
        'cmbCamera2PC2
        '
        Me.cmbCamera2PC2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbCamera2PC2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbCamera2PC2.FormattingEnabled = True
        Me.cmbCamera2PC2.Location = New System.Drawing.Point(368, 79)
        Me.cmbCamera2PC2.MaxDropDownItems = 16
        Me.cmbCamera2PC2.Name = "cmbCamera2PC2"
        Me.cmbCamera2PC2.Size = New System.Drawing.Size(68, 24)
        Me.cmbCamera2PC2.TabIndex = 38
        '
        'cmbCamera1PC2
        '
        Me.cmbCamera1PC2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbCamera1PC2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbCamera1PC2.FormattingEnabled = True
        Me.cmbCamera1PC2.Location = New System.Drawing.Point(294, 79)
        Me.cmbCamera1PC2.MaxDropDownItems = 16
        Me.cmbCamera1PC2.Name = "cmbCamera1PC2"
        Me.cmbCamera1PC2.Size = New System.Drawing.Size(68, 24)
        Me.cmbCamera1PC2.TabIndex = 37
        '
        'cmbCamera2PC1
        '
        Me.cmbCamera2PC1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbCamera2PC1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbCamera2PC1.FormattingEnabled = True
        Me.cmbCamera2PC1.Location = New System.Drawing.Point(368, 46)
        Me.cmbCamera2PC1.MaxDropDownItems = 16
        Me.cmbCamera2PC1.Name = "cmbCamera2PC1"
        Me.cmbCamera2PC1.Size = New System.Drawing.Size(68, 24)
        Me.cmbCamera2PC1.TabIndex = 34
        '
        'cmbCamera1PC1
        '
        Me.cmbCamera1PC1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbCamera1PC1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbCamera1PC1.FormattingEnabled = True
        Me.cmbCamera1PC1.Location = New System.Drawing.Point(294, 46)
        Me.cmbCamera1PC1.MaxDropDownItems = 16
        Me.cmbCamera1PC1.Name = "cmbCamera1PC1"
        Me.cmbCamera1PC1.Size = New System.Drawing.Size(68, 24)
        Me.cmbCamera1PC1.TabIndex = 33
        '
        'Label15
        '
        Me.Label15.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label15.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label15.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label15.Location = New System.Drawing.Point(294, 18)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(142, 24)
        Me.Label15.TabIndex = 32
        Me.Label15.Text = "ÉJÉÅÉâÉZÉbÉg"
        Me.Label15.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblLPStatusPC2
        '
        Me.lblLPStatusPC2.BackColor = System.Drawing.Color.Blue
        Me.lblLPStatusPC2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblLPStatusPC2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblLPStatusPC2.ForeColor = System.Drawing.Color.Yellow
        Me.lblLPStatusPC2.Location = New System.Drawing.Point(189, 79)
        Me.lblLPStatusPC2.Name = "lblLPStatusPC2"
        Me.lblLPStatusPC2.Size = New System.Drawing.Size(99, 22)
        Me.lblLPStatusPC2.TabIndex = 31
        Me.lblLPStatusPC2.Text = "åüç∏íÜ"
        Me.lblLPStatusPC2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblLPStatusPC1
        '
        Me.lblLPStatusPC1.BackColor = System.Drawing.Color.Blue
        Me.lblLPStatusPC1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblLPStatusPC1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.lblLPStatusPC1.ForeColor = System.Drawing.Color.Yellow
        Me.lblLPStatusPC1.Location = New System.Drawing.Point(189, 46)
        Me.lblLPStatusPC1.Name = "lblLPStatusPC1"
        Me.lblLPStatusPC1.Size = New System.Drawing.Size(99, 24)
        Me.lblLPStatusPC1.TabIndex = 30
        Me.lblLPStatusPC1.Text = "åüç∏íÜ"
        Me.lblLPStatusPC1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(9, 79)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(174, 22)
        Me.Label7.TabIndex = 29
        Me.Label7.Text = "ÉçÉWÉbÉNÉpÉâÉâÉìPC02"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label8
        '
        Me.Label8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label8.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label8.Location = New System.Drawing.Point(9, 46)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(174, 24)
        Me.Label8.TabIndex = 28
        Me.Label8.Text = "ÉçÉWÉbÉNÉpÉâÉâÉìPC01"
        Me.Label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'frmSystemStatus
        '
        Me.ClientSize = New System.Drawing.Size(655, 735)
        Me.Controls.Add(Me.GroupBox8)
        Me.Controls.Add(Me.GroupBox7)
        Me.Controls.Add(Me.GroupBox5)
        Me.Controls.Add(Me.prbTimeOut)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.cmdEnd)
        Me.Name = "frmSystemStatus"
        Me.Text = "åüç∏äJéní‚é~ê›íËâÊñ "
        Me.TopMost = False
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox4.PerformLayout()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox6.ResumeLayout(False)
        Me.GroupBox7.ResumeLayout(False)
        Me.GroupBox8.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents cmdEnd As System.Windows.Forms.Button
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents lblSystemStatus As System.Windows.Forms.Label
    Friend WithEvents btnKensaEnd As System.Windows.Forms.Button
    Friend WithEvents btnKensaStart As System.Windows.Forms.Button
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents rdoSmyu As System.Windows.Forms.RadioButton
    Friend WithEvents rdoCamera As System.Windows.Forms.RadioButton
    Friend WithEvents rdoSamp As System.Windows.Forms.RadioButton
    Friend WithEvents rdoNormal As System.Windows.Forms.RadioButton
    Friend WithEvents rdoPlgTest As System.Windows.Forms.RadioButton
    Friend WithEvents rdoPlgLine As System.Windows.Forms.RadioButton
    Friend WithEvents timCheck As System.Windows.Forms.Timer
    Friend WithEvents timTimeOut As System.Windows.Forms.Timer
    Friend WithEvents rdoPlgBot As System.Windows.Forms.RadioButton
    Friend WithEvents rdoPlgTop As System.Windows.Forms.RadioButton
    Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
    Friend WithEvents rdoKatamenYes As System.Windows.Forms.RadioButton
    Friend WithEvents rdoKatamenNo As System.Windows.Forms.RadioButton
    Friend WithEvents prbTimeOut As System.Windows.Forms.ProgressBar
    Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
    Friend WithEvents btnSystemEnd As System.Windows.Forms.Button
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents lblPCodeTop As System.Windows.Forms.Label
    Friend WithEvents cmbPCodeTop As System.Windows.Forms.ComboBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents lblSCodeTop As System.Windows.Forms.Label
    Friend WithEvents cmbSCodeTop As System.Windows.Forms.ComboBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents GroupBox7 As System.Windows.Forms.GroupBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents lblPCodeBot As System.Windows.Forms.Label
    Friend WithEvents cmbPCodeBot As System.Windows.Forms.ComboBox
    Friend WithEvents lblSCodeBot As System.Windows.Forms.Label
    Friend WithEvents cmbSCodeBot As System.Windows.Forms.ComboBox
    Friend WithEvents btnHTEnable As System.Windows.Forms.Button
    Friend WithEvents GroupBox8 As GroupBox
    Friend WithEvents cmbCamera2PC2 As ComboBox
    Friend WithEvents cmbCamera1PC2 As ComboBox
    Friend WithEvents cmbCamera2PC1 As ComboBox
    Friend WithEvents cmbCamera1PC1 As ComboBox
    Friend WithEvents Label15 As Label
    Friend WithEvents lblLPStatusPC2 As Label
    Friend WithEvents lblLPStatusPC1 As Label
    Friend WithEvents Label7 As Label
    Friend WithEvents Label8 As Label
    Friend WithEvents btnLPEndPC2 As Button
    Friend WithEvents btnLPStartPC2 As Button
    Friend WithEvents btnLPEndPC1 As Button
    Friend WithEvents btnLPStartPC1 As Button
    Friend WithEvents lblErrMsg As Label
End Class
