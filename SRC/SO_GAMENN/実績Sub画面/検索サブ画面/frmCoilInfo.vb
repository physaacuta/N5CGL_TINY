'*********************************************************************************
'�@�R�C����񑗐M�`���N���X
'	[Ver]
'		Ver.01    2016/09/01  ����
'
'	[����]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary

Public Class frmCoilInfo

    ' �\�����ڃA�C�e��
    Structure ITEM
        Dim name As String
        Dim size As Integer
        Dim type As System.Type     ' �����p 0:byte 1:short 2:int 3:char �ȂǁE�E�E

        Public Overrides Function ToString() As String
            Return name
        End Function

        Public Sub New(ByVal name As String, ByVal size As Integer, ByVal type As System.Type)
            Me.name = name
            Me.size = size
            Me.type = type
        End Sub

    End Structure


    Private m_bRaw As Byte()                '���f�[�^
    Private m_lstName As New List(Of ITEM)  '���ږ��̈ꗗ
    Private m_mode As Integer               '�v�����[�h(1:�I�����C���p�A0:�c�[���p�j
    Private m_strKizukenNo As String        '�Ǘ�No
    Private m_nUpNum As Integer = 57        '�㉺�ؑֈʒu

    ''' <summary>
    ''' �p�����[�^�ɗv�����[�h(1:�I�����C���p�A0:�c�[���p�j
    ''' </summary>
    ''' <param name="mode">1:�I�����C���p�A0:�c�[���p</param>
    ''' <param name="strKizukenNo">�o���Ǘ�No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
        '�����o�ϐ��Z�b�g
        m_mode = mode                   '�p�����[�^�ɗv�����[�h(1:�I�����C���p�A0:�c�[���p�j
        m_strKizukenNo = strKizukenNo   '�o���Ǘ�No

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub


    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoil_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Me.tProp_Moveable = True
        Me.TopMost = False

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        SetNameList()

        'DGV������
        dgvInitialize(dgvCoilInfo, True)
        AddHandler dgvCoilInfo.ColumnHeaderMouseDoubleClick, AddressOf dgvColumnHeader_DoubleClick
        dgvInitialize(dgvNextCoilInfo, False)
        AddHandler dgvNextCoilInfo.ColumnHeaderMouseDoubleClick, AddressOf dgvColumnHeader_DoubleClick

        MakeIDList()

        If 1 = m_mode Then                  ' �I�����C�����[�h
            btnRead.Visible = False         ' �Ǎ��{�^���u��\���v
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count <> 0 Then
                cmbID.SelectedIndex = 0
            End If
        Else                                ' �c�[�����[�h
            btnRead.Visible = True          ' �Ǎ��{�^���u�\���v
        End If

    End Sub


    ''' <summary>
    ''' �\�����鍀�ږ����Z�b�g����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetNameList()
        m_lstName.Add(New ITEM("�T�u�w�b�_�[", 2, GetType(Byte)))
        m_lstName.Add(New ITEM("�f�[�^��", 2, GetType(Short)))
        m_lstName.Add(New ITEM("�s�b", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("�`���敪", 2, GetType(Short)))
        m_lstName.Add(New ITEM("���M����", 12, GetType(Short)))
        m_lstName.Add(New ITEM("�O�����g�t���O", 2, GetType(Short)))
        m_lstName.Add(New ITEM("�\��", 40, GetType(Char)))
        m_lstName.Add(New ITEM("����No", PRO_SIZE_ORDER_NO, GetType(Char)))
        m_lstName.Add(New ITEM("���YNo", PRO_SIZE_COIL_NO, GetType(Char)))

        m_lstName.Add(New ITEM("���M�ʒu", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("�R�C����", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("�ޗ���", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("�ޗ���", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("������", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("������", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("���H���g������", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("SPM�L�ї�", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("TLV�L�ї�", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("�����t�����g�[����", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("�����e�[���[����", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("SPM���[�N���[���a (��)", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("SPM���[�N���[���a (��)", 4, GetType(Int32)))

        m_lstName.Add(New ITEM("�\��", 124, GetType(Char)))
        m_lstName.Add(New ITEM("����� �\", 4, GetType(Char)))
        m_lstName.Add(New ITEM("����� ��", 4, GetType(Char)))
        m_lstName.Add(New ITEM("�N�����[�g���� ���", 2, GetType(Char)))
        m_lstName.Add(New ITEM("�N�����[�g���� �L��", 2, GetType(Char)))
        m_lstName.Add(New ITEM("�i��", 1, GetType(Char)))
        m_lstName.Add(New ITEM("���b�L���", 1, GetType(Char)))
        m_lstName.Add(New ITEM("��ގ�", 2, GetType(Char)))
        m_lstName.Add(New ITEM("�f�B�t�@�����V�����}�[�N", 1, GetType(Char)))
        m_lstName.Add(New ITEM("���敪", 1, GetType(Char)))
        m_lstName.Add(New ITEM("���b�L�T�C�N��", 1, GetType(Char)))
        m_lstName.Add(New ITEM("���x���[ �C�����C��SPM", 1, GetType(Char)))
        m_lstName.Add(New ITEM("���w�����h��", 1, GetType(Char)))
        m_lstName.Add(New ITEM("�I�t���C��SPM������", 1, GetType(Char)))
        m_lstName.Add(New ITEM("���b�L�敪", 1, GetType(Char)))
        m_lstName.Add(New ITEM("�\�ʏ���1����", 1, GetType(Char)))
        m_lstName.Add(New ITEM("�\�ʏ���2����", 1, GetType(Char)))
        m_lstName.Add(New ITEM("�h��", 1, GetType(Char)))
        m_lstName.Add(New ITEM("�H���L���\��", 1, GetType(Char)))
        m_lstName.Add(New ITEM("�o�E���_�����킹", 1, GetType(Char)))
        m_lstName.Add(New ITEM("CAPL��ގ�", 10, GetType(Char)))
        m_lstName.Add(New ITEM("�ڕt�K�i", 8, GetType(Char)))
        m_lstName.Add(New ITEM("Ni�t����", 2, GetType(Char)))
        m_lstName.Add(New ITEM("TCM���[���e�x�敪", 2, GetType(Char)))
        m_lstName.Add(New ITEM("SPM�����L��", 2, GetType(Char)))
        m_lstName.Add(New ITEM("�v�掟�H���R�[�h", 2, GetType(Char)))
        m_lstName.Add(New ITEM("����", 32, GetType(Char)))
        m_lstName.Add(New ITEM("�p�r", 4, GetType(Char)))
        m_lstName.Add(New ITEM("CAPL�\�ʎd�グ", 4, GetType(Char)))
        m_lstName.Add(New ITEM("CAPL�_���L��", 2, GetType(Char)))
        m_lstName.Add(New ITEM("�i��敪", 1, GetType(Char)))
        m_lstName.Add(New ITEM("GA/CR�敪", 1, GetType(Char)))
        m_lstName.Add(New ITEM("SPM�������їL��", 2, GetType(Char)))
        m_lstName.Add(New ITEM("�H���R�[�h", 2, GetType(Char)))
        m_lstName.Add(New ITEM("�\��", 42, GetType(Char)))

        m_lstName.Add(New ITEM("2�R�C����" & vbCrLf & "����No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("2�R�C����" & vbCrLf & "���YNo", 12, GetType(Char)))
        m_lstName.Add(New ITEM("2�R�C����" & vbCrLf & "�����t�����g�[��", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("2�R�C����" & vbCrLf & "�H���R�[�h", 2, GetType(Char)))
        m_lstName.Add(New ITEM("2�R�C����" & vbCrLf & "�\��", 2, GetType(Char)))

        m_lstName.Add(New ITEM("3�R�C����" & vbCrLf & "����No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("3�R�C����" & vbCrLf & "���YNo", 12, GetType(Char)))
        m_lstName.Add(New ITEM("3�R�C����" & vbCrLf & "�����t�����g�[��", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("3�R�C����" & vbCrLf & "�H���R�[�h", 2, GetType(Char)))
        m_lstName.Add(New ITEM("3�R�C����" & vbCrLf & "�\��", 2, GetType(Char)))

        m_lstName.Add(New ITEM("4�R�C����" & vbCrLf & "����No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("4�R�C����" & vbCrLf & "���YNo", 12, GetType(Char)))
        m_lstName.Add(New ITEM("4�R�C����" & vbCrLf & "�����t�����g�[��", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("4�R�C����" & vbCrLf & "�H���R�[�h", 2, GetType(Char)))
        m_lstName.Add(New ITEM("4�R�C����" & vbCrLf & "�\��", 2, GetType(Char)))

        m_lstName.Add(New ITEM("5�R�C����" & vbCrLf & "����No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("5�R�C����" & vbCrLf & "���YNo", 12, GetType(Char)))
        m_lstName.Add(New ITEM("5�R�C����" & vbCrLf & "�����t�����g�[��", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("5�R�C����" & vbCrLf & "�H���R�[�h", 2, GetType(Char)))
        m_lstName.Add(New ITEM("5�R�C����" & vbCrLf & "�\��", 2, GetType(Char)))

        m_lstName.Add(New ITEM("�\��", 16, GetType(Char)))

    End Sub

    ''' <summary>
    ''' ID���X�g�쐬
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        strSQL = ""

        If m_mode = 1 Then           '�I�����C�����[�h
            ' �I�����C�����[�h�̏ꍇ�́A�Ǘ�No.���猟������
            strSQL &= " SELECT A.ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_COILINF & " as A"
            strSQL &= " INNER JOIN " & DB_COIL_INF & " as B"
            strSQL &= " ON A.�L�[��� = B.�L�[���"
            strSQL &= " AND B.�Ǘ�No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY A.�o�^���t DESC"
        Else                         '�c�[�����[�h
            'strSQL &= " SELECT TOP 200 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_COILINF
            strSQL &= " ORDER BY �o�^���t DESC"
        End If

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Exit Sub
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            cmbID.Items.Clear()
            ' �f�[�^�擾
            While sqlRead.Read()

                cmbID.Items.Add(sqlRead.GetValue(0).ToString)

            End While

        Catch ex As Exception
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView, ByVal bUpper As Boolean)
        Dim ii As Integer = 0

        With dgv
            .Rows.Clear()
            ' �����`
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' �I�����[�h(�s�I��)
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells       ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnCount = 6                                                'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Columns(0).Width = 45
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).Resizable = DataGridViewTriState.False

            .Columns(1).Width = 190
            .Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(1).Resizable = DataGridViewTriState.False

            .Columns(2).Width = 100
            .Columns(2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(2).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(2).Visible = False
            .Columns(2).Resizable = DataGridViewTriState.False

            .Columns(3).Width = 45
            .Columns(3).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(3).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(3).Resizable = DataGridViewTriState.False

            .Columns(4).Width = 180
            .Columns(4).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(4).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(4).Resizable = DataGridViewTriState.True

            .Columns(5).Width = 435
            .Columns(5).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(5).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(5).Resizable = DataGridViewTriState.True

            .Columns(0).HeaderText = "Index"
            .Columns(1).HeaderText = "����"
            .Columns(2).HeaderText = "�^"
            .Columns(3).HeaderText = "Size"
            .Columns(4).HeaderText = "�l"
            .Columns(5).HeaderText = "HEX"

        End With

        Dim nStart As Integer
        If bUpper Then
            dgv.RowCount = m_nUpNum
            nStart = 0
        Else
            dgv.RowCount = m_lstName.Count - m_nUpNum
            nStart = m_nUpNum
        End If

        For ii = 0 To dgv.RowCount - 1
            dgv(0, ii).Value = GetPos(nStart + ii).ToString
            dgv(1, ii).Value = m_lstName(nStart + ii)
            dgv(2, ii).Value = m_lstName(nStart + ii).type.ToString
            dgv(3, ii).Value = m_lstName(nStart + ii).size.ToString
        Next ii

    End Sub

    ''' <summary>
    ''' �f�[�^(Hex)�擾
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetHex(ByVal idx As Integer) As String
        Dim sRet As String = ""

        If idx < 0 Or m_lstName.Count < idx Then Return ""

        Dim start As Integer = GetPos(idx)
        Dim size As Integer = m_lstName(idx).size

        If m_bRaw.Length < start + size Then Return ""

        For ii As Integer = 0 To size - 1
            sRet &= String.Format("{0:X2} ", m_bRaw(start + ii))
            If ii Mod 16 = 0 Then
                sRet &= vbCrLf
            End If
        Next

        Return sRet
    End Function


    ''' <summary>
    ''' �f�[�^(�l)�擾
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetValue(ByVal idx As Integer) As String
        Dim sRet As String = ""

        If idx < 0 Or m_lstName.Count < idx Then Return ""

        Dim start As Integer = GetPos(idx)
        Dim size As Integer = m_lstName(idx).size

        If m_bRaw.Length < start + size Then Return ""

        If m_lstName(idx).type Is GetType(Char) Then
            sRet = System.Text.ASCIIEncoding.ASCII.GetString(m_bRaw, start, size)
        ElseIf m_lstName(idx).type Is GetType(Short) Then
            Dim svalue As Short
            svalue = System.BitConverter.ToInt16(m_bRaw, start)
            sRet = svalue.ToString
        ElseIf m_lstName(idx).type Is GetType(Integer) Then
            Dim ivalue As Integer
            ivalue = System.BitConverter.ToInt32(m_bRaw, start)
            sRet = ivalue.ToString
        ElseIf m_lstName(idx).type Is GetType(Byte) Then
            sRet = GetHex(idx)
        End If

        Return sRet

    End Function


    ''' <summary>
    ''' �����擾
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetTimeValue(ByVal idx As Integer) As String


        If idx < 0 Or m_lstName.Count < idx Then Return ""

        Dim start As Integer = GetPos(idx)
        Dim size As Integer = m_lstName(idx).size
        Dim sRet(size / 2) As String

        If m_bRaw.Length < start + size Then Return ""

        For ii As Integer = 0 To size / 2
            Dim svalue As Short
            svalue = System.BitConverter.ToInt16(m_bRaw, start)
            sRet(ii) = svalue.ToString
            start = start + 2
        Next

        Return String.Format("{0}/{1}/{2} {3}:{4}:{5}", sRet(0), sRet(1), sRet(2), sRet(3), sRet(4), sRet(5))

    End Function

    ''' <summary>
    ''' �`���ʒu�擾
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetPos(ByVal idx As Integer) As Integer
        Dim ret As Integer = 0

        For ii As Integer = 0 To idx - 1
            ret += m_lstName(ii).size
        Next
        Return ret
    End Function

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' �I��
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' ����{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.Close()

    End Sub


    ''' <summary>
    ''' ID���X�g�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>

    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click(sender, e)
    End Sub


    ''' <summary>
    ''' �ǂݍ��݃{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click

        '�f�[�^�\��
        If GetData(cmbID.Text) Then
            Call DispData()
        End If

    End Sub

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispData()
        Dim dgv As DataGridView
        Dim row As Integer

        For ii As Integer = 0 To m_lstName.Count - 1
            If ii < m_nUpNum Then
                dgv = dgvCoilInfo
                row = ii
            Else
                dgv = dgvNextCoilInfo
                row = ii - m_nUpNum
            End If
            If "���M����" = m_lstName(ii).name Then
                dgv(4, row).Value = GetTimeValue(ii)
            Else
                dgv(4, row).Value = GetValue(ii)
            End If
            dgv(5, row).Value = GetHex(ii)
        Next ii

    End Sub

    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��

        '�r���Ǘ�No����
        strSQL = ""
        strSQL &= " Select ���`��"
        strSQL &= " FROM " & DB_BUF_PRO_R_COILINF
        strSQL &= " WHERE ID = '" & strID & "'"
        strSQL &= " ORDER BY �o�^���t DESC"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' �f�[�^�擾
            sqlRead.Read()

            '���`��
            m_bRaw = CType(sqlRead.GetValue(0), Byte())

        Catch ex As Exception
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function


    ''' <summary>
    ''' ��\���ؑ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvColumnHeader_DoubleClick(ByVal sender As System.Object, ByVal e As DataGridViewCellMouseEventArgs)
        Dim dgv As DataGridView = dgvCoilInfo
        With dgv.Columns(2)
            If .Visible Then
                .Visible = False
            Else
                .Visible = True
            End If
            dgvNextCoilInfo.Columns(2).Visible = .Visible
        End With
    End Sub


End Class
