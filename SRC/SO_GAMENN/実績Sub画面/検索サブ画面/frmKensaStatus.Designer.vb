<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmKensaStatus
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
        Me.btnRead = New System.Windows.Forms.Button()
        Me.cmbID = New System.Windows.Forms.ComboBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.btnEnd = New System.Windows.Forms.Button()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.TableLayoutPanel2 = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel2 = New System.Windows.Forms.Panel()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.lbldataSize1 = New System.Windows.Forms.Label()
        Me.lblDate1 = New System.Windows.Forms.Label()
        Me.Label18 = New System.Windows.Forms.Label()
        Me.lblHeader_Code1 = New System.Windows.Forms.Label()
        Me.lblId1 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.Label17 = New System.Windows.Forms.Label()
        Me.lblRec_Code1 = New System.Windows.Forms.Label()
        Me.lblCoilno1 = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label15 = New System.Windows.Forms.Label()
        Me.lblLine_Code1 = New System.Windows.Forms.Label()
        Me.lblMeino1 = New System.Windows.Forms.Label()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.dgvStatus1 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.TableLayoutPanel3 = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel3 = New System.Windows.Forms.Panel()
        Me.Label25 = New System.Windows.Forms.Label()
        Me.Label27 = New System.Windows.Forms.Label()
        Me.lbldataSize2 = New System.Windows.Forms.Label()
        Me.lblDate2 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.lblHeader_Code2 = New System.Windows.Forms.Label()
        Me.lblId2 = New System.Windows.Forms.Label()
        Me.Label23 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.lblRec_Code2 = New System.Windows.Forms.Label()
        Me.lblCoilno2 = New System.Windows.Forms.Label()
        Me.Label21 = New System.Windows.Forms.Label()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.lblLine_Code2 = New System.Windows.Forms.Label()
        Me.lblMeino2 = New System.Windows.Forms.Label()
        Me.Label16 = New System.Windows.Forms.Label()
        Me.dgvStatus2 = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel1 = New System.Windows.Forms.Panel()
        Me.GroupBox1.SuspendLayout()
        Me.TableLayoutPanel2.SuspendLayout()
        Me.Panel2.SuspendLayout()
        CType(Me.dgvStatus1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        Me.TableLayoutPanel3.SuspendLayout()
        Me.Panel3.SuspendLayout()
        CType(Me.dgvStatus2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TableLayoutPanel1.SuspendLayout()
        Me.Panel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'btnRead
        '
        Me.btnRead.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRead.Location = New System.Drawing.Point(271, 3)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(104, 39)
        Me.btnRead.TabIndex = 43
        Me.btnRead.Text = "ì«Ç›çûÇ›"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'cmbID
        '
        Me.cmbID.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbID.FormattingEnabled = True
        Me.cmbID.Location = New System.Drawing.Point(47, 11)
        Me.cmbID.Name = "cmbID"
        Me.cmbID.Size = New System.Drawing.Size(204, 24)
        Me.cmbID.TabIndex = 42
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(3, 15)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(34, 21)
        Me.Label1.TabIndex = 41
        Me.Label1.Text = "ID"
        '
        'btnEnd
        '
        Me.btnEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnEnd.Location = New System.Drawing.Point(515, 3)
        Me.btnEnd.Name = "btnEnd"
        Me.btnEnd.Size = New System.Drawing.Size(104, 39)
        Me.btnEnd.TabIndex = 40
        Me.btnEnd.Text = "F12:ï¬Ç∂ÇÈ"
        Me.btnEnd.UseVisualStyleBackColor = True
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(213, 10)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(109, 15)
        Me.Label2.TabIndex = 50
        Me.Label2.Text = "ì`ï∂î≠ê∂éûçè"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.TableLayoutPanel2)
        Me.GroupBox1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox1.Location = New System.Drawing.Point(3, 58)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(630, 376)
        Me.GroupBox1.TabIndex = 51
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ï\"
        '
        'TableLayoutPanel2
        '
        Me.TableLayoutPanel2.ColumnCount = 1
        Me.TableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel2.Controls.Add(Me.Panel2, 0, 0)
        Me.TableLayoutPanel2.Controls.Add(Me.dgvStatus1, 0, 1)
        Me.TableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel2.Location = New System.Drawing.Point(3, 19)
        Me.TableLayoutPanel2.Name = "TableLayoutPanel2"
        Me.TableLayoutPanel2.RowCount = 2
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 120.0!))
        Me.TableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel2.Size = New System.Drawing.Size(624, 354)
        Me.TableLayoutPanel2.TabIndex = 68
        '
        'Panel2
        '
        Me.Panel2.Controls.Add(Me.Label6)
        Me.Panel2.Controls.Add(Me.Label2)
        Me.Panel2.Controls.Add(Me.lbldataSize1)
        Me.Panel2.Controls.Add(Me.lblDate1)
        Me.Panel2.Controls.Add(Me.Label18)
        Me.Panel2.Controls.Add(Me.lblHeader_Code1)
        Me.Panel2.Controls.Add(Me.lblId1)
        Me.Panel2.Controls.Add(Me.Label9)
        Me.Panel2.Controls.Add(Me.Label17)
        Me.Panel2.Controls.Add(Me.lblRec_Code1)
        Me.Panel2.Controls.Add(Me.lblCoilno1)
        Me.Panel2.Controls.Add(Me.Label11)
        Me.Panel2.Controls.Add(Me.Label15)
        Me.Panel2.Controls.Add(Me.lblLine_Code1)
        Me.Panel2.Controls.Add(Me.lblMeino1)
        Me.Panel2.Controls.Add(Me.Label13)
        Me.Panel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel2.Location = New System.Drawing.Point(3, 3)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(618, 114)
        Me.Panel2.TabIndex = 0
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(3, 10)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(92, 15)
        Me.Label6.TabIndex = 52
        Me.Label6.Text = "ì`ï∂ÉRÅ[Éh"
        '
        'lbldataSize1
        '
        Me.lbldataSize1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lbldataSize1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lbldataSize1.Location = New System.Drawing.Point(151, 84)
        Me.lbldataSize1.Name = "lbldataSize1"
        Me.lbldataSize1.Size = New System.Drawing.Size(60, 24)
        Me.lbldataSize1.TabIndex = 65
        Me.lbldataSize1.Text = "12345"
        Me.lbldataSize1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblDate1
        '
        Me.lblDate1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblDate1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblDate1.Location = New System.Drawing.Point(325, 4)
        Me.lblDate1.Name = "lblDate1"
        Me.lblDate1.Size = New System.Drawing.Size(140, 24)
        Me.lblDate1.TabIndex = 51
        Me.lblDate1.Text = "12345678901234"
        Me.lblDate1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label18
        '
        Me.Label18.AutoSize = True
        Me.Label18.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label18.Location = New System.Drawing.Point(3, 90)
        Me.Label18.Name = "Label18"
        Me.Label18.Size = New System.Drawing.Size(143, 15)
        Me.Label18.TabIndex = 64
        Me.Label18.Text = "åüç∏èÛë‘ÉZÉbÉgêî"
        '
        'lblHeader_Code1
        '
        Me.lblHeader_Code1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblHeader_Code1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblHeader_Code1.Location = New System.Drawing.Point(97, 4)
        Me.lblHeader_Code1.Name = "lblHeader_Code1"
        Me.lblHeader_Code1.Size = New System.Drawing.Size(32, 24)
        Me.lblHeader_Code1.TabIndex = 53
        Me.lblHeader_Code1.Text = "12"
        Me.lblHeader_Code1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblId1
        '
        Me.lblId1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblId1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblId1.Location = New System.Drawing.Point(81, 56)
        Me.lblId1.Name = "lblId1"
        Me.lblId1.Size = New System.Drawing.Size(192, 24)
        Me.lblId1.TabIndex = 63
        Me.lblId1.Text = "12345678901234567890"
        Me.lblId1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.Location = New System.Drawing.Point(133, 10)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(41, 15)
        Me.Label9.TabIndex = 54
        Me.Label9.Text = "òAî‘"
        '
        'Label17
        '
        Me.Label17.AutoSize = True
        Me.Label17.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label17.Location = New System.Drawing.Point(3, 62)
        Me.Label17.Name = "Label17"
        Me.Label17.Size = New System.Drawing.Size(75, 15)
        Me.Label17.TabIndex = 62
        Me.Label17.Text = "ä«óùî‘çÜ"
        '
        'lblRec_Code1
        '
        Me.lblRec_Code1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblRec_Code1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblRec_Code1.Location = New System.Drawing.Point(177, 4)
        Me.lblRec_Code1.Name = "lblRec_Code1"
        Me.lblRec_Code1.Size = New System.Drawing.Size(32, 24)
        Me.lblRec_Code1.TabIndex = 55
        Me.lblRec_Code1.Text = "12"
        Me.lblRec_Code1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblCoilno1
        '
        Me.lblCoilno1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblCoilno1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblCoilno1.Location = New System.Drawing.Point(261, 30)
        Me.lblCoilno1.Name = "lblCoilno1"
        Me.lblCoilno1.Size = New System.Drawing.Size(100, 24)
        Me.lblCoilno1.TabIndex = 61
        Me.lblCoilno1.Text = "1234567890"
        Me.lblCoilno1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label11.Location = New System.Drawing.Point(469, 10)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(92, 15)
        Me.Label11.TabIndex = 56
        Me.Label11.Text = "çHíˆÉRÅ[Éh"
        '
        'Label15
        '
        Me.Label15.AutoSize = True
        Me.Label15.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label15.Location = New System.Drawing.Point(133, 36)
        Me.Label15.Name = "Label15"
        Me.Label15.Size = New System.Drawing.Size(126, 15)
        Me.Label15.TabIndex = 60
        Me.Label15.Text = "ó‚âÑÉRÉCÉãî‘çÜ"
        '
        'lblLine_Code1
        '
        Me.lblLine_Code1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblLine_Code1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblLine_Code1.Location = New System.Drawing.Point(563, 4)
        Me.lblLine_Code1.Name = "lblLine_Code1"
        Me.lblLine_Code1.Size = New System.Drawing.Size(32, 24)
        Me.lblLine_Code1.TabIndex = 57
        Me.lblLine_Code1.Text = "12"
        Me.lblLine_Code1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblMeino1
        '
        Me.lblMeino1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMeino1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMeino1.Location = New System.Drawing.Point(81, 30)
        Me.lblMeino1.Name = "lblMeino1"
        Me.lblMeino1.Size = New System.Drawing.Size(48, 24)
        Me.lblMeino1.TabIndex = 59
        Me.lblMeino1.Text = "1234"
        Me.lblMeino1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label13.Location = New System.Drawing.Point(3, 36)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(75, 15)
        Me.Label13.TabIndex = 58
        Me.Label13.Text = "ñΩóﬂî‘çÜ"
        '
        'dgvStatus1
        '
        Me.dgvStatus1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvStatus1.Location = New System.Drawing.Point(3, 123)
        Me.dgvStatus1.Name = "dgvStatus1"
        Me.dgvStatus1.RowTemplate.Height = 21
        Me.dgvStatus1.Size = New System.Drawing.Size(618, 228)
        Me.dgvStatus1.TabIndex = 67
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.TableLayoutPanel3)
        Me.GroupBox2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.GroupBox2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.Location = New System.Drawing.Point(3, 440)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(630, 376)
        Me.GroupBox2.TabIndex = 52
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "ó†"
        '
        'TableLayoutPanel3
        '
        Me.TableLayoutPanel3.ColumnCount = 1
        Me.TableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Controls.Add(Me.Panel3, 0, 0)
        Me.TableLayoutPanel3.Controls.Add(Me.dgvStatus2, 0, 1)
        Me.TableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel3.Location = New System.Drawing.Point(3, 19)
        Me.TableLayoutPanel3.Name = "TableLayoutPanel3"
        Me.TableLayoutPanel3.RowCount = 2
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 120.0!))
        Me.TableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel3.Size = New System.Drawing.Size(624, 354)
        Me.TableLayoutPanel3.TabIndex = 68
        '
        'Panel3
        '
        Me.Panel3.Controls.Add(Me.Label25)
        Me.Panel3.Controls.Add(Me.Label27)
        Me.Panel3.Controls.Add(Me.lbldataSize2)
        Me.Panel3.Controls.Add(Me.lblDate2)
        Me.Panel3.Controls.Add(Me.Label5)
        Me.Panel3.Controls.Add(Me.lblHeader_Code2)
        Me.Panel3.Controls.Add(Me.lblId2)
        Me.Panel3.Controls.Add(Me.Label23)
        Me.Panel3.Controls.Add(Me.Label8)
        Me.Panel3.Controls.Add(Me.lblRec_Code2)
        Me.Panel3.Controls.Add(Me.lblCoilno2)
        Me.Panel3.Controls.Add(Me.Label21)
        Me.Panel3.Controls.Add(Me.Label12)
        Me.Panel3.Controls.Add(Me.lblLine_Code2)
        Me.Panel3.Controls.Add(Me.lblMeino2)
        Me.Panel3.Controls.Add(Me.Label16)
        Me.Panel3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel3.Location = New System.Drawing.Point(3, 3)
        Me.Panel3.Name = "Panel3"
        Me.Panel3.Size = New System.Drawing.Size(618, 114)
        Me.Panel3.TabIndex = 0
        '
        'Label25
        '
        Me.Label25.AutoSize = True
        Me.Label25.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label25.Location = New System.Drawing.Point(3, 10)
        Me.Label25.Name = "Label25"
        Me.Label25.Size = New System.Drawing.Size(92, 15)
        Me.Label25.TabIndex = 52
        Me.Label25.Text = "ì`ï∂ÉRÅ[Éh"
        '
        'Label27
        '
        Me.Label27.AutoSize = True
        Me.Label27.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label27.Location = New System.Drawing.Point(213, 10)
        Me.Label27.Name = "Label27"
        Me.Label27.Size = New System.Drawing.Size(109, 15)
        Me.Label27.TabIndex = 50
        Me.Label27.Text = "ì`ï∂î≠ê∂éûçè"
        '
        'lbldataSize2
        '
        Me.lbldataSize2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lbldataSize2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lbldataSize2.Location = New System.Drawing.Point(151, 84)
        Me.lbldataSize2.Name = "lbldataSize2"
        Me.lbldataSize2.Size = New System.Drawing.Size(60, 24)
        Me.lbldataSize2.TabIndex = 65
        Me.lbldataSize2.Text = "12345"
        Me.lbldataSize2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblDate2
        '
        Me.lblDate2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblDate2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblDate2.Location = New System.Drawing.Point(325, 4)
        Me.lblDate2.Name = "lblDate2"
        Me.lblDate2.Size = New System.Drawing.Size(140, 24)
        Me.lblDate2.TabIndex = 51
        Me.lblDate2.Text = "12345678901234"
        Me.lblDate2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(3, 90)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(143, 15)
        Me.Label5.TabIndex = 64
        Me.Label5.Text = "åüç∏èÛë‘ÉZÉbÉgêî"
        '
        'lblHeader_Code2
        '
        Me.lblHeader_Code2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblHeader_Code2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblHeader_Code2.Location = New System.Drawing.Point(97, 4)
        Me.lblHeader_Code2.Name = "lblHeader_Code2"
        Me.lblHeader_Code2.Size = New System.Drawing.Size(32, 24)
        Me.lblHeader_Code2.TabIndex = 53
        Me.lblHeader_Code2.Text = "12"
        Me.lblHeader_Code2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblId2
        '
        Me.lblId2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblId2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblId2.Location = New System.Drawing.Point(81, 56)
        Me.lblId2.Name = "lblId2"
        Me.lblId2.Size = New System.Drawing.Size(192, 24)
        Me.lblId2.TabIndex = 63
        Me.lblId2.Text = "12345678901234567890"
        Me.lblId2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label23
        '
        Me.Label23.AutoSize = True
        Me.Label23.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label23.Location = New System.Drawing.Point(133, 10)
        Me.Label23.Name = "Label23"
        Me.Label23.Size = New System.Drawing.Size(41, 15)
        Me.Label23.TabIndex = 54
        Me.Label23.Text = "òAî‘"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(3, 62)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(75, 15)
        Me.Label8.TabIndex = 62
        Me.Label8.Text = "ä«óùî‘çÜ"
        '
        'lblRec_Code2
        '
        Me.lblRec_Code2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblRec_Code2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblRec_Code2.Location = New System.Drawing.Point(177, 4)
        Me.lblRec_Code2.Name = "lblRec_Code2"
        Me.lblRec_Code2.Size = New System.Drawing.Size(32, 24)
        Me.lblRec_Code2.TabIndex = 55
        Me.lblRec_Code2.Text = "12"
        Me.lblRec_Code2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblCoilno2
        '
        Me.lblCoilno2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblCoilno2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblCoilno2.Location = New System.Drawing.Point(261, 30)
        Me.lblCoilno2.Name = "lblCoilno2"
        Me.lblCoilno2.Size = New System.Drawing.Size(100, 24)
        Me.lblCoilno2.TabIndex = 61
        Me.lblCoilno2.Text = "1234567890"
        Me.lblCoilno2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label21
        '
        Me.Label21.AutoSize = True
        Me.Label21.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label21.Location = New System.Drawing.Point(469, 10)
        Me.Label21.Name = "Label21"
        Me.Label21.Size = New System.Drawing.Size(92, 15)
        Me.Label21.TabIndex = 56
        Me.Label21.Text = "çHíˆÉRÅ[Éh"
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label12.Location = New System.Drawing.Point(133, 36)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(126, 15)
        Me.Label12.TabIndex = 60
        Me.Label12.Text = "ó‚âÑÉRÉCÉãî‘çÜ"
        '
        'lblLine_Code2
        '
        Me.lblLine_Code2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblLine_Code2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblLine_Code2.Location = New System.Drawing.Point(563, 4)
        Me.lblLine_Code2.Name = "lblLine_Code2"
        Me.lblLine_Code2.Size = New System.Drawing.Size(32, 24)
        Me.lblLine_Code2.TabIndex = 57
        Me.lblLine_Code2.Text = "12"
        Me.lblLine_Code2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblMeino2
        '
        Me.lblMeino2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMeino2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMeino2.Location = New System.Drawing.Point(81, 30)
        Me.lblMeino2.Name = "lblMeino2"
        Me.lblMeino2.Size = New System.Drawing.Size(48, 24)
        Me.lblMeino2.TabIndex = 59
        Me.lblMeino2.Text = "1234"
        Me.lblMeino2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label16
        '
        Me.Label16.AutoSize = True
        Me.Label16.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label16.Location = New System.Drawing.Point(3, 36)
        Me.Label16.Name = "Label16"
        Me.Label16.Size = New System.Drawing.Size(75, 15)
        Me.Label16.TabIndex = 58
        Me.Label16.Text = "ñΩóﬂî‘çÜ"
        '
        'dgvStatus2
        '
        Me.dgvStatus2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.dgvStatus2.Location = New System.Drawing.Point(3, 123)
        Me.dgvStatus2.Name = "dgvStatus2"
        Me.dgvStatus2.RowTemplate.Height = 21
        Me.dgvStatus2.Size = New System.Drawing.Size(618, 228)
        Me.dgvStatus2.TabIndex = 67
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.ColumnCount = 1
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.Panel1, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.GroupBox2, 0, 2)
        Me.TableLayoutPanel1.Controls.Add(Me.GroupBox1, 0, 1)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 3
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 55.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(636, 819)
        Me.TableLayoutPanel1.TabIndex = 53
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.Label1)
        Me.Panel1.Controls.Add(Me.btnEnd)
        Me.Panel1.Controls.Add(Me.btnRead)
        Me.Panel1.Controls.Add(Me.cmbID)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel1.Location = New System.Drawing.Point(3, 3)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(630, 49)
        Me.Panel1.TabIndex = 0
        '
        'frmKensaStatus
        '
        Me.ClientSize = New System.Drawing.Size(636, 819)
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.Name = "frmKensaStatus"
        Me.Text = "åüç∏èÛë‘âÊñ "
        Me.TopMost = False
        Me.tProp_FromFormatMode = tClassForm.frmBase.gem_FromFormatMode.iNON
        Me.GroupBox1.ResumeLayout(False)
        Me.TableLayoutPanel2.ResumeLayout(False)
        Me.Panel2.ResumeLayout(False)
        Me.Panel2.PerformLayout()
        CType(Me.dgvStatus1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        Me.TableLayoutPanel3.ResumeLayout(False)
        Me.Panel3.ResumeLayout(False)
        Me.Panel3.PerformLayout()
        CType(Me.dgvStatus2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TableLayoutPanel1.ResumeLayout(False)
        Me.Panel1.ResumeLayout(False)
        Me.Panel1.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents cmbID As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents btnEnd As System.Windows.Forms.Button
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents lblDate1 As System.Windows.Forms.Label
    Friend WithEvents lblLine_Code1 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents lblRec_Code1 As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents lblHeader_Code1 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents lblId1 As System.Windows.Forms.Label
    Friend WithEvents Label17 As System.Windows.Forms.Label
    Friend WithEvents lblCoilno1 As System.Windows.Forms.Label
    Friend WithEvents Label15 As System.Windows.Forms.Label
    Friend WithEvents lblMeino1 As System.Windows.Forms.Label
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents lbldataSize1 As System.Windows.Forms.Label
    Friend WithEvents Label18 As System.Windows.Forms.Label
    Friend WithEvents dgvStatus1 As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvStatus2 As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents lbldataSize2 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents lblId2 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents lblCoilno2 As System.Windows.Forms.Label
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents lblMeino2 As System.Windows.Forms.Label
    Friend WithEvents Label16 As System.Windows.Forms.Label
    Friend WithEvents lblLine_Code2 As System.Windows.Forms.Label
    Friend WithEvents Label21 As System.Windows.Forms.Label
    Friend WithEvents lblRec_Code2 As System.Windows.Forms.Label
    Friend WithEvents Label23 As System.Windows.Forms.Label
    Friend WithEvents lblHeader_Code2 As System.Windows.Forms.Label
    Friend WithEvents Label25 As System.Windows.Forms.Label
    Friend WithEvents lblDate2 As System.Windows.Forms.Label
    Friend WithEvents Label27 As System.Windows.Forms.Label
    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents Panel1 As Panel
    Friend WithEvents TableLayoutPanel2 As TableLayoutPanel
    Friend WithEvents Panel2 As Panel
    Friend WithEvents TableLayoutPanel3 As TableLayoutPanel
    Friend WithEvents Panel3 As Panel
End Class
