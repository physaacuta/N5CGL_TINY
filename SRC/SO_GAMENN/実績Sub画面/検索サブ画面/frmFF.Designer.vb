<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmFF
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
        Me.lblKizuKen = New System.Windows.Forms.Label()
        Me.btnRead = New System.Windows.Forms.Button()
        Me.dgvFF1 = New System.Windows.Forms.DataGridView()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.cmbID = New System.Windows.Forms.ComboBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.btnEnd = New System.Windows.Forms.Button()
        Me.lblProductNo = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.lblMeiNo = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.dgvFF2 = New System.Windows.Forms.DataGridView()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.lblCoilLen1 = New System.Windows.Forms.Label()
        Me.lblMode1 = New System.Windows.Forms.Label()
        Me.lblOverCnt1 = New System.Windows.Forms.Label()
        Me.lblSetCnt1 = New System.Windows.Forms.Label()
        Me.lblSetCnt2 = New System.Windows.Forms.Label()
        Me.lblOverCnt2 = New System.Windows.Forms.Label()
        Me.lblMode2 = New System.Windows.Forms.Label()
        Me.lblCoilLen2 = New System.Windows.Forms.Label()
        Me.btnStatus = New System.Windows.Forms.Button()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.Panel2 = New System.Windows.Forms.Panel()
        Me.Panel1 = New System.Windows.Forms.Panel()
        CType(Me.dgvFF1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvFF2, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.TableLayoutPanel1.SuspendLayout()
        Me.Panel2.SuspendLayout()
        Me.Panel1.SuspendLayout()
        Me.SuspendLayout()
        '
        'lblKizuKen
        '
        Me.lblKizuKen.BackColor = System.Drawing.SystemColors.Control
        Me.lblKizuKen.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblKizuKen.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblKizuKen.ForeColor = System.Drawing.Color.Black
        Me.lblKizuKen.Location = New System.Drawing.Point(153, 6)
        Me.lblKizuKen.Name = "lblKizuKen"
        Me.lblKizuKen.Size = New System.Drawing.Size(400, 32)
        Me.lblKizuKen.TabIndex = 35
        Me.lblKizuKen.Text = "12345678901234567890"
        Me.lblKizuKen.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnRead
        '
        Me.btnRead.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnRead.Location = New System.Drawing.Point(275, 3)
        Me.btnRead.Name = "btnRead"
        Me.btnRead.Size = New System.Drawing.Size(104, 39)
        Me.btnRead.TabIndex = 32
        Me.btnRead.Text = "ì«Ç›çûÇ›"
        Me.btnRead.UseVisualStyleBackColor = True
        '
        'dgvFF1
        '
        Me.dgvFF1.AllowUserToAddRows = False
        Me.dgvFF1.AllowUserToDeleteRows = False
        Me.dgvFF1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvFF1.Location = New System.Drawing.Point(3, 303)
        Me.dgvFF1.Name = "dgvFF1"
        Me.dgvFF1.ReadOnly = True
        Me.dgvFF1.RowTemplate.Height = 21
        Me.dgvFF1.Size = New System.Drawing.Size(640, 228)
        Me.dgvFF1.TabIndex = 25
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.SystemColors.Control
        Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.Color.Black
        Me.Label2.Location = New System.Drawing.Point(9, 10)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(132, 28)
        Me.Label2.TabIndex = 34
        Me.Label2.Text = "·råüä«óùNo"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbID
        '
        Me.cmbID.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbID.FormattingEnabled = True
        Me.cmbID.Location = New System.Drawing.Point(51, 11)
        Me.cmbID.Name = "cmbID"
        Me.cmbID.Size = New System.Drawing.Size(204, 24)
        Me.cmbID.TabIndex = 31
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(7, 15)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(34, 21)
        Me.Label1.TabIndex = 30
        Me.Label1.Text = "ID"
        '
        'btnEnd
        '
        Me.btnEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnEnd.Location = New System.Drawing.Point(536, 5)
        Me.btnEnd.Name = "btnEnd"
        Me.btnEnd.Size = New System.Drawing.Size(104, 39)
        Me.btnEnd.TabIndex = 29
        Me.btnEnd.Text = "F12:ï¬Ç∂ÇÈ"
        Me.btnEnd.UseVisualStyleBackColor = True
        '
        'lblProductNo
        '
        Me.lblProductNo.BackColor = System.Drawing.SystemColors.Control
        Me.lblProductNo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblProductNo.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblProductNo.ForeColor = System.Drawing.Color.Black
        Me.lblProductNo.Location = New System.Drawing.Point(373, 42)
        Me.lblProductNo.Name = "lblProductNo"
        Me.lblProductNo.Size = New System.Drawing.Size(180, 28)
        Me.lblProductNo.TabIndex = 27
        Me.lblProductNo.Text = "123456789012"
        Me.lblProductNo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label9
        '
        Me.Label9.BackColor = System.Drawing.SystemColors.Control
        Me.Label9.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.ForeColor = System.Drawing.Color.Black
        Me.Label9.Location = New System.Drawing.Point(257, 42)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(104, 28)
        Me.Label9.TabIndex = 28
        Me.Label9.Text = "ê∂éYî‘çÜ"
        Me.Label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblMeiNo
        '
        Me.lblMeiNo.BackColor = System.Drawing.SystemColors.Control
        Me.lblMeiNo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMeiNo.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMeiNo.ForeColor = System.Drawing.Color.Black
        Me.lblMeiNo.Location = New System.Drawing.Point(153, 42)
        Me.lblMeiNo.Name = "lblMeiNo"
        Me.lblMeiNo.Size = New System.Drawing.Size(88, 28)
        Me.lblMeiNo.TabIndex = 24
        Me.lblMeiNo.Text = "1234"
        Me.lblMeiNo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label7
        '
        Me.Label7.BackColor = System.Drawing.SystemColors.Control
        Me.Label7.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.ForeColor = System.Drawing.Color.Black
        Me.Label7.Location = New System.Drawing.Point(9, 46)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(88, 28)
        Me.Label7.TabIndex = 26
        Me.Label7.Text = "ñΩóﬂNo"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'dgvFF2
        '
        Me.dgvFF2.AllowUserToAddRows = False
        Me.dgvFF2.AllowUserToDeleteRows = False
        Me.dgvFF2.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvFF2.Location = New System.Drawing.Point(3, 586)
        Me.dgvFF2.Name = "dgvFF2"
        Me.dgvFF2.ReadOnly = True
        Me.dgvFF2.RowTemplate.Height = 21
        Me.dgvFF2.Size = New System.Drawing.Size(640, 228)
        Me.dgvFF2.TabIndex = 36
        '
        'Label3
        '
        Me.Label3.BackColor = System.Drawing.SystemColors.Control
        Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.ForeColor = System.Drawing.Color.Black
        Me.Label3.Location = New System.Drawing.Point(3, 270)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(44, 28)
        Me.Label3.TabIndex = 37
        Me.Label3.Text = "ï\"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label4
        '
        Me.Label4.BackColor = System.Drawing.SystemColors.Control
        Me.Label4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.Color.Black
        Me.Label4.Location = New System.Drawing.Point(3, 553)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(44, 28)
        Me.Label4.TabIndex = 38
        Me.Label4.Text = "ó†"
        Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label6
        '
        Me.Label6.BackColor = System.Drawing.SystemColors.Control
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.ForeColor = System.Drawing.Color.Black
        Me.Label6.Location = New System.Drawing.Point(9, 86)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(188, 20)
        Me.Label6.TabIndex = 40
        Me.Label6.Text = "ÉRÉCÉãí∑Ç≥"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label8
        '
        Me.Label8.BackColor = System.Drawing.SystemColors.Control
        Me.Label8.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.ForeColor = System.Drawing.Color.Black
        Me.Label8.Location = New System.Drawing.Point(9, 114)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(188, 24)
        Me.Label8.TabIndex = 41
        Me.Label8.Text = "·råüèàóùÉÇÅ[Éh"
        Me.Label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label10
        '
        Me.Label10.BackColor = System.Drawing.SystemColors.Control
        Me.Label10.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.ForeColor = System.Drawing.Color.Black
        Me.Label10.Location = New System.Drawing.Point(9, 150)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(224, 28)
        Me.Label10.TabIndex = 42
        Me.Label10.Text = "·rå¬êîÉIÅ[ÉoÅ[ÉXÉeÅ[É^ÉX"
        Me.Label10.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label11
        '
        Me.Label11.BackColor = System.Drawing.SystemColors.Control
        Me.Label11.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label11.ForeColor = System.Drawing.Color.Black
        Me.Label11.Location = New System.Drawing.Point(9, 186)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(188, 24)
        Me.Label11.TabIndex = 43
        Me.Label11.Text = "ÉfÅ[É^ê›íËêî"
        Me.Label11.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblCoilLen1
        '
        Me.lblCoilLen1.BackColor = System.Drawing.SystemColors.Control
        Me.lblCoilLen1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblCoilLen1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblCoilLen1.ForeColor = System.Drawing.Color.Black
        Me.lblCoilLen1.Location = New System.Drawing.Point(253, 82)
        Me.lblCoilLen1.Name = "lblCoilLen1"
        Me.lblCoilLen1.Size = New System.Drawing.Size(124, 28)
        Me.lblCoilLen1.TabIndex = 44
        Me.lblCoilLen1.Text = "123456789012"
        Me.lblCoilLen1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblMode1
        '
        Me.lblMode1.BackColor = System.Drawing.SystemColors.Control
        Me.lblMode1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMode1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMode1.ForeColor = System.Drawing.Color.Black
        Me.lblMode1.Location = New System.Drawing.Point(253, 114)
        Me.lblMode1.Name = "lblMode1"
        Me.lblMode1.Size = New System.Drawing.Size(124, 28)
        Me.lblMode1.TabIndex = 45
        Me.lblMode1.Text = "123456789012"
        Me.lblMode1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblOverCnt1
        '
        Me.lblOverCnt1.BackColor = System.Drawing.SystemColors.Control
        Me.lblOverCnt1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblOverCnt1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblOverCnt1.ForeColor = System.Drawing.Color.Black
        Me.lblOverCnt1.Location = New System.Drawing.Point(253, 146)
        Me.lblOverCnt1.Name = "lblOverCnt1"
        Me.lblOverCnt1.Size = New System.Drawing.Size(124, 28)
        Me.lblOverCnt1.TabIndex = 47
        Me.lblOverCnt1.Text = "123456789012"
        Me.lblOverCnt1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblSetCnt1
        '
        Me.lblSetCnt1.BackColor = System.Drawing.SystemColors.Control
        Me.lblSetCnt1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblSetCnt1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSetCnt1.ForeColor = System.Drawing.Color.Black
        Me.lblSetCnt1.Location = New System.Drawing.Point(253, 178)
        Me.lblSetCnt1.Name = "lblSetCnt1"
        Me.lblSetCnt1.Size = New System.Drawing.Size(124, 28)
        Me.lblSetCnt1.TabIndex = 48
        Me.lblSetCnt1.Text = "123456789012"
        Me.lblSetCnt1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblSetCnt2
        '
        Me.lblSetCnt2.BackColor = System.Drawing.SystemColors.Control
        Me.lblSetCnt2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblSetCnt2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSetCnt2.ForeColor = System.Drawing.Color.Black
        Me.lblSetCnt2.Location = New System.Drawing.Point(389, 178)
        Me.lblSetCnt2.Name = "lblSetCnt2"
        Me.lblSetCnt2.Size = New System.Drawing.Size(124, 28)
        Me.lblSetCnt2.TabIndex = 53
        Me.lblSetCnt2.Text = "123456789012"
        Me.lblSetCnt2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblOverCnt2
        '
        Me.lblOverCnt2.BackColor = System.Drawing.SystemColors.Control
        Me.lblOverCnt2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblOverCnt2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblOverCnt2.ForeColor = System.Drawing.Color.Black
        Me.lblOverCnt2.Location = New System.Drawing.Point(389, 146)
        Me.lblOverCnt2.Name = "lblOverCnt2"
        Me.lblOverCnt2.Size = New System.Drawing.Size(124, 28)
        Me.lblOverCnt2.TabIndex = 52
        Me.lblOverCnt2.Text = "123456789012"
        Me.lblOverCnt2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblMode2
        '
        Me.lblMode2.BackColor = System.Drawing.SystemColors.Control
        Me.lblMode2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblMode2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMode2.ForeColor = System.Drawing.Color.Black
        Me.lblMode2.Location = New System.Drawing.Point(389, 114)
        Me.lblMode2.Name = "lblMode2"
        Me.lblMode2.Size = New System.Drawing.Size(124, 28)
        Me.lblMode2.TabIndex = 50
        Me.lblMode2.Text = "123456789012"
        Me.lblMode2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblCoilLen2
        '
        Me.lblCoilLen2.BackColor = System.Drawing.SystemColors.Control
        Me.lblCoilLen2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblCoilLen2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblCoilLen2.ForeColor = System.Drawing.Color.Black
        Me.lblCoilLen2.Location = New System.Drawing.Point(389, 82)
        Me.lblCoilLen2.Name = "lblCoilLen2"
        Me.lblCoilLen2.Size = New System.Drawing.Size(124, 28)
        Me.lblCoilLen2.TabIndex = 49
        Me.lblCoilLen2.Text = "123456789012"
        Me.lblCoilLen2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnStatus
        '
        Me.btnStatus.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnStatus.Location = New System.Drawing.Point(387, 3)
        Me.btnStatus.Name = "btnStatus"
        Me.btnStatus.Size = New System.Drawing.Size(104, 39)
        Me.btnStatus.TabIndex = 54
        Me.btnStatus.Text = "åüç∏èÛë‘ì`ï∂"
        Me.btnStatus.UseVisualStyleBackColor = True
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.AutoSize = True
        Me.TableLayoutPanel1.ColumnCount = 1
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.Panel2, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.Label4, 0, 4)
        Me.TableLayoutPanel1.Controls.Add(Me.dgvFF2, 0, 5)
        Me.TableLayoutPanel1.Controls.Add(Me.dgvFF1, 0, 3)
        Me.TableLayoutPanel1.Controls.Add(Me.Label3, 0, 2)
        Me.TableLayoutPanel1.Controls.Add(Me.Panel1, 0, 1)
        Me.TableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(0, 0)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 6
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 220.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(660, 837)
        Me.TableLayoutPanel1.TabIndex = 55
        '
        'Panel2
        '
        Me.Panel2.Controls.Add(Me.cmbID)
        Me.Panel2.Controls.Add(Me.btnStatus)
        Me.Panel2.Controls.Add(Me.btnEnd)
        Me.Panel2.Controls.Add(Me.Label1)
        Me.Panel2.Controls.Add(Me.btnRead)
        Me.Panel2.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel2.Location = New System.Drawing.Point(3, 3)
        Me.Panel2.Name = "Panel2"
        Me.Panel2.Size = New System.Drawing.Size(654, 44)
        Me.Panel2.TabIndex = 40
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.Label2)
        Me.Panel1.Controls.Add(Me.lblSetCnt2)
        Me.Panel1.Controls.Add(Me.Label7)
        Me.Panel1.Controls.Add(Me.lblOverCnt2)
        Me.Panel1.Controls.Add(Me.lblMeiNo)
        Me.Panel1.Controls.Add(Me.lblMode2)
        Me.Panel1.Controls.Add(Me.Label9)
        Me.Panel1.Controls.Add(Me.lblCoilLen2)
        Me.Panel1.Controls.Add(Me.lblProductNo)
        Me.Panel1.Controls.Add(Me.lblSetCnt1)
        Me.Panel1.Controls.Add(Me.lblKizuKen)
        Me.Panel1.Controls.Add(Me.lblOverCnt1)
        Me.Panel1.Controls.Add(Me.Label6)
        Me.Panel1.Controls.Add(Me.lblMode1)
        Me.Panel1.Controls.Add(Me.Label8)
        Me.Panel1.Controls.Add(Me.lblCoilLen1)
        Me.Panel1.Controls.Add(Me.Label10)
        Me.Panel1.Controls.Add(Me.Label11)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel1.Location = New System.Drawing.Point(3, 53)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(654, 214)
        Me.Panel1.TabIndex = 39
        '
        'frmFF
        '
        Me.ClientSize = New System.Drawing.Size(660, 837)
        Me.ControlBox = True
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.MaximizeBox = True
        Me.MinimizeBox = True
        Me.Name = "frmFF"
        Me.Text = "åüç∏é¿ê—FFâÊñ "
        Me.TopMost = False
        CType(Me.dgvFF1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvFF2, System.ComponentModel.ISupportInitialize).EndInit()
        Me.TableLayoutPanel1.ResumeLayout(False)
        Me.Panel2.ResumeLayout(False)
        Me.Panel2.PerformLayout()
        Me.Panel1.ResumeLayout(False)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents lblKizuKen As System.Windows.Forms.Label
	Friend WithEvents btnRead As System.Windows.Forms.Button
    Friend WithEvents dgvFF1 As System.Windows.Forms.DataGridView
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents cmbID As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents btnEnd As System.Windows.Forms.Button
    Friend WithEvents lblProductNo As System.Windows.Forms.Label
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents lblMeiNo As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents dgvFF2 As System.Windows.Forms.DataGridView
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label11 As System.Windows.Forms.Label
    Friend WithEvents lblCoilLen1 As System.Windows.Forms.Label
    Friend WithEvents lblMode1 As System.Windows.Forms.Label
    Friend WithEvents lblOverCnt1 As System.Windows.Forms.Label
    Friend WithEvents lblSetCnt1 As System.Windows.Forms.Label
    Friend WithEvents lblSetCnt2 As System.Windows.Forms.Label
    Friend WithEvents lblOverCnt2 As System.Windows.Forms.Label
    Friend WithEvents lblMode2 As System.Windows.Forms.Label
    Friend WithEvents lblCoilLen2 As System.Windows.Forms.Label
    Friend WithEvents lblCutCnt2 As System.Windows.Forms.Label
    Friend WithEvents lblCutCnt1 As System.Windows.Forms.Label
    Friend WithEvents btnStatus As System.Windows.Forms.Button
    Friend WithEvents TableLayoutPanel1 As TableLayoutPanel
    Friend WithEvents Panel2 As Panel
    Friend WithEvents Panel1 As Panel
End Class
