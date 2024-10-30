'*********************************************************************************
'�@��������FF�N���X
'	[Ver]
'		Ver.01    2007/09/22  ����
'
'	[����]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary

Public Class frmFF

    Private m_mode As Integer               '�v�����[�h(1:�I�����C���p�A0:�c�[���p�j
    Private m_strKizukenNo As String        '�r���Ǘ�No

    Private Const DgvMaxCol_MRare As Integer = 11            '�f�[�^��MAX

    ' �O���b�h�@��ݒ�
    Private Enum EM_MRATE_COL_ID
        iNO = 0             'NO
        iKizuOut            '���o�敪
        iTypeID             '�r��
        iGradeID            '�r�O���[�h
        iHaba               '�r�Е����ʒu
        iLen                '�r����
        iTotalLen           '�r�����������ʒu
        iKizuDiv            '�r�敪
        iKizuNo             '�rNo
        iOpeDsp             '�I�y�K�G�\���t���O
        iSysKbn             '�V�X�e���敪
    End Enum

    ''' <summary>
    ''' �p�����[�^�ɗv�����[�h(1:�I�����C���p�A0:�c�[���p�j
    ''' </summary>
    ''' <param name="mode">1:�I�����C���p�A0:�c�[���p</param>
    ''' <param name="strKizukenNo">�Ǘ�No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
        '�����o�ϐ��Z�b�g
        m_mode = mode                   '�p�����[�^�ɗv�����[�h(1:�I�����C���p�A0:�c�[���p�j
        m_strKizukenNo = strKizukenNo   '�Ǘ�No

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
    Private Sub frmFF_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Me.tProp_Moveable = True

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        '��ʏ�����
        Call InitDsp()

        'ID���X�g�쐬
        Call MakeIDList()

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
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
            strSQL &= " WHERE �Ǘ�No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY �o�^���t DESC"
        Else                         '�c�[�����[�h
            'strSQL &= " SELECT TOP 200 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
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
    ''' ��ʏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '�r�����Ǘ�No
        lblKizuKen.Text = ""

        '����No
        lblMeiNo.Text = ""

        '���YNO
        lblProductNo.Text = ""

        '�R�C����
        lblCoilLen1.Text = ""
        lblCoilLen2.Text = ""

        '�r���������[�h
        lblMode1.Text = ""
        lblMode2.Text = ""

        ' 0.1m�� �Ή� --->>>
        ''�J�b�g�M����
        'lblCutCnt1.Text = ""
        'lblCutCnt2.Text = ""
        ' 0.1m�� �Ή� <<<---

        '�r���I�[�o�[�X�e�[�^�X
        lblOverCnt1.Text = ""
        lblOverCnt2.Text = ""

        '�f�[�^�ݒ萔
        lblSetCnt1.Text = ""
        lblSetCnt2.Text = ""

        '�r�������\ ������
        dgvInitialize(dgvFF1)
        dgvInitialize(dgvFF2)

    End Sub

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
    ''' ��ʏI��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '��ʏI��
        Me.Close()

    End Sub


    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="typZiseki1">FF���уf�[�^</param>
    ''' <param name="typZiseki2">FF���уf�[�^</param>
    ''' <param name="strKizuken">�r���Ǘ�No</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki1 As FF_JISSEKI,
                             ByRef typZiseki2 As FF_JISSEKI, ByRef strKizuken As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData1() As Byte  '���`���o�C�i���f�[�^
        Dim bytData2() As Byte  '���`���o�C�i���f�[�^


        '�r���Ǘ�No����
        strSQL = ""
        strSQL &= " SELECT ���`��1,"
        strSQL &= "        ���`��2,"
        strSQL &= "        �Ǘ�No"
        strSQL &= " FROM " & DB_BUF_FF_S_ZISEKI
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
            bytData1 = CType(sqlRead.GetValue(0), Byte())

            '���`��
            bytData2 = CType(sqlRead.GetValue(1), Byte())

            '�r�����Ǘ�No
            strKizuken = sqlRead.GetValue(2).ToString

            tMod.MemSet(bytData1, 0, typZiseki1, tMod.SizeOf(typZiseki1))
            tMod.MemSet(bytData2, 0, typZiseki2, tMod.SizeOf(typZiseki2))


        Catch ex As Exception
            Return False
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
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        'Dim ii As Integer = 0
        With dgv
            .RowCount = MAX_FF_DFCT_CNT

            '' �����`
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


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = True                                                ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' �I���Z���F��`
            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                 ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = DgvMaxCol_MRare           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 50                                       ' ��w�b�_�T�C�Y

            '��0 (�A�h���X) �r���o�敪
            .Columns(EM_MRATE_COL_ID.iNO).HeaderText = "NO"
            .Columns(EM_MRATE_COL_ID.iNO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iNO).Width = 40
            .Columns(EM_MRATE_COL_ID.iNO).SortMode = DataGridViewColumnSortMode.NotSortable



            '��P (�A�h���X) �r���o�敪
            .Columns(EM_MRATE_COL_ID.iKizuOut).HeaderText = "�r���o�敪"
            .Columns(EM_MRATE_COL_ID.iKizuOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iKizuOut).Width = 60
            .Columns(EM_MRATE_COL_ID.iKizuOut).SortMode = DataGridViewColumnSortMode.NotSortable


            '��Q (�A�h���X) �r��
            .Columns(EM_MRATE_COL_ID.iTypeID).HeaderText = "�r��"
            .Columns(EM_MRATE_COL_ID.iTypeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTypeID).Width = 50
            .Columns(EM_MRATE_COL_ID.iTypeID).SortMode = DataGridViewColumnSortMode.NotSortable


            '��R (�A�h���X) �r�O���[�h
            .Columns(EM_MRATE_COL_ID.iGradeID).HeaderText = "�r��ڰ��"
            .Columns(EM_MRATE_COL_ID.iGradeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGradeID).Width = 80
            .Columns(EM_MRATE_COL_ID.iGradeID).SortMode = DataGridViewColumnSortMode.NotSortable


            '��S (�A�h���X) �r�Е����ʒu
            .Columns(EM_MRATE_COL_ID.iHaba).HeaderText = "�r��" & vbNewLine & "�����ʒu"
            .Columns(EM_MRATE_COL_ID.iHaba).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iHaba).Width = 80
            .Columns(EM_MRATE_COL_ID.iHaba).SortMode = DataGridViewColumnSortMode.NotSortable


            '��T (�A�h���X) �r����
            .Columns(EM_MRATE_COL_ID.iLen).HeaderText = "�r����"
            .Columns(EM_MRATE_COL_ID.iLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iLen).Width = 60
            .Columns(EM_MRATE_COL_ID.iLen).SortMode = DataGridViewColumnSortMode.NotSortable



            '��U (�A�h���X) �r�����������ʒu
            .Columns(EM_MRATE_COL_ID.iTotalLen).HeaderText = "�r������" & vbNewLine & "�����ʒu"
            .Columns(EM_MRATE_COL_ID.iTotalLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTotalLen).Width = 80
            .Columns(EM_MRATE_COL_ID.iTotalLen).SortMode = DataGridViewColumnSortMode.NotSortable


            '��V (�A�h���X) �r�敪
            .Columns(EM_MRATE_COL_ID.iKizuDiv).HeaderText = "�r�敪"
            .Columns(EM_MRATE_COL_ID.iKizuDiv).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iKizuDiv).Width = 60
            .Columns(EM_MRATE_COL_ID.iKizuDiv).SortMode = DataGridViewColumnSortMode.NotSortable


            '��8 (�A�h���X) �rNo
            .Columns(EM_MRATE_COL_ID.iKizuNo).HeaderText = "�rNO"
            .Columns(EM_MRATE_COL_ID.iKizuNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iKizuNo).Width = 110
            .Columns(EM_MRATE_COL_ID.iKizuNo).SortMode = DataGridViewColumnSortMode.NotSortable

            '��9 (�A�h���X) �I�y�K�G
            .Columns(EM_MRATE_COL_ID.iOpeDsp).HeaderText = "�I�y�K�G"
            .Columns(EM_MRATE_COL_ID.iOpeDsp).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iOpeDsp).Width = 80
            .Columns(EM_MRATE_COL_ID.iOpeDsp).SortMode = DataGridViewColumnSortMode.NotSortable

            '��10 (�A�h���X) �V�X�e���敪
            .Columns(EM_MRATE_COL_ID.iSysKbn).HeaderText = "���ы敪"
            .Columns(EM_MRATE_COL_ID.iSysKbn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iSysKbn).Width = 80
            .Columns(EM_MRATE_COL_ID.iSysKbn).SortMode = DataGridViewColumnSortMode.NotSortable
        End With
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

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typZiseki1 As FF_JISSEKI = Nothing  '�r�������уf�[�^
        Dim typZiseki2 As FF_JISSEKI = Nothing  '�r�������уf�[�^
        Dim strKizuken As String = ""       '�r���Ǘ�No

        '�f�[�^�\��
        If GetData(cmbID.Text, typZiseki1, typZiseki2, strKizuken) Then
            Call SetData(typZiseki1, typZiseki2, strKizuken)
        End If
    End Sub
    ''' <summary>
    ''' �f�[�^�Z�b�g
    ''' </summary>
    '''<param name="typZiseki1">FF���уf�[�^1</param>
    ''' <param name="typZiseki2">FF���уf�[�^2</param>
    ''' <param name="strKizuken">�r���Ǘ�No</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki1 As FF_JISSEKI, ByVal typZiseki2 As FF_JISSEKI, ByVal strKizuken As String)

        Dim ii As Integer   '���[�v�J�E���^
        ' 0.1m�� �Ή� --->>>
        Dim strBuff As String
        ' 0.1m�� �Ή� <<<---

        '�r���Ǘ�No
        lblKizuKen.Text = tMod.ByteToString(typZiseki1.Id)

        ''����No
        lblMeiNo.Text = tMod.ByteToString(typZiseki1.Hd.Meino)

        ''���YNO
        lblProductNo.Text = tMod.ByteToString(typZiseki1.Hd.Coilno)

        '�R�C����
        ' 0.1m�� �Ή� --->>>
        'lblCoilLen1.Text = tMod.ByteToString(typZiseki1.CoilLen)
        'lblCoilLen2.Text = tMod.ByteToString(typZiseki2.CoilLen)
        'lblCoilLen1.Text = Format(CSng(IIf(IsNumeric(tMod.ByteToString(typZiseki1.CoilLen)), tMod.ByteToString(typZiseki1.CoilLen).Trim, "0")) / 10, "0.0")
        lblCoilLen1.Text = Format(IIf(IsNumeric(tMod.ByteToString(typZiseki1.CoilLen)), CDbl(tMod.ByteToString(typZiseki1.CoilLen)) / 10.0, 0.0), "0.0")
        'lblCoilLen2.Text = Format(CSng(IIf(IsNumeric(tMod.ByteToString(typZiseki2.CoilLen)), tMod.ByteToString(typZiseki2.CoilLen).Trim, "0")) / 10, "0.0")
        lblCoilLen2.Text = Format(IIf(IsNumeric(tMod.ByteToString(typZiseki2.CoilLen)), CDbl(tMod.ByteToString(typZiseki2.CoilLen)) / 10.0, 0.0), "0.0")
        ' 0.1m�� �Ή� <<<---

        '�r���������[�h
        lblMode1.Text = tMod.ByteToString(typZiseki1.Mode)
        lblMode2.Text = tMod.ByteToString(typZiseki2.Mode)

        ' 0.1m�� �Ή� --->>>
        ''�J�b�g�M����
        'lblCutCnt1.Text = tMod.ByteToString(typZiseki1.CutCount)
        'lblCutCnt2.Text = tMod.ByteToString(typZiseki2.CutCount)
        ' 0.1m�� �Ή� <<<---

        '�r���I�[�o�[�X�e�[�^�X
        lblOverCnt1.Text = tMod.ByteToString(typZiseki1.OverStatus)
        lblOverCnt2.Text = tMod.ByteToString(typZiseki2.OverStatus)

        '�f�[�^�ݒ萔
        lblSetCnt1.Text = tMod.ByteToString(typZiseki1.DataDefault)
        lblSetCnt2.Text = tMod.ByteToString(typZiseki2.DataDefault)

        ''�\
        For ii = 0 To MAX_FF_DFCT_CNT - 1
            dgvFF1(EM_MRATE_COL_ID.iNO, ii).Value = ii + 1
            dgvFF1(EM_MRATE_COL_ID.iKizuOut, ii).Value = Chr(typZiseki1.Dt(ii).Kizukbn(0))
            dgvFF1(EM_MRATE_COL_ID.iTypeID, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).Kind)
            dgvFF1(EM_MRATE_COL_ID.iGradeID, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).Grade)
            dgvFF1(EM_MRATE_COL_ID.iHaba, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).WidPos)
            ' 0.1m�� �Ή� --->>>
            '�r����
            ''dgvFF1(EM_MRATE_COL_ID.iLen, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).Len)
            strBuff = tMod.ByteToString(typZiseki1.Dt(ii).Len)
            If strBuff.Trim().Length <> 0 Then
                dgvFF1(EM_MRATE_COL_ID.iLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF1(EM_MRATE_COL_ID.iLen, ii).Value = ""
            End If

            '�r�����������ʒu
            ''dgvFF1(EM_MRATE_COL_ID.iTotalLen, ii).Value = tMod.ByteToString(typZiseki1.Dt(ii).DefectPos)
            strBuff = tMod.ByteToString(typZiseki1.Dt(ii).DefectPos)
            If strBuff.Trim().Length <> 0 Then
                dgvFF1(EM_MRATE_COL_ID.iTotalLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF1(EM_MRATE_COL_ID.iTotalLen, ii).Value = ""
            End If
            ' 0.1m�� �Ή� <<<---

            dgvFF1(EM_MRATE_COL_ID.iKizuDiv, ii).Value = Chr(typZiseki1.Dt(ii).kbn(0))

            Dim sNoHex As String = tMod.ByteToString(typZiseki1.Dt(ii).No)
            Dim nNo As Integer
            Dim sNo As String = sNoHex

            '// 20200520 �r�ۑ�������g�� --->>>
            ''�rNo 16�i��(10�i��)
            'If Integer.TryParse(sNoHex, Globalization.NumberStyles.HexNumber, Nothing, nNo) = True Then
            '    sNo &= String.Format(" ({0,5})", nNo)
            'End If

            ''�rNo 32�i��(10�i��)
            If Base32Hex_Decord(typZiseki1.Dt(ii).No, nNo) = True Then
                sNo &= String.Format(" ({0,7})", nNo)
            End If
            '// 20200520 �r�ۑ�������g�� ---<<<
            dgvFF1(EM_MRATE_COL_ID.iKizuNo, ii).Value = sNo

            dgvFF1(EM_MRATE_COL_ID.iOpeDsp, ii).Value = Chr(typZiseki1.Dt(ii).OpeDsp(0))
            dgvFF1(EM_MRATE_COL_ID.iSysKbn, ii).Value = Chr(typZiseki1.Dt(ii).SysKbn(0))
        Next ii


        ''��
        For ii = 0 To MAX_FF_DFCT_CNT - 1
            dgvFF2(EM_MRATE_COL_ID.iNO, ii).Value = ii + 1
            dgvFF2(EM_MRATE_COL_ID.iKizuOut, ii).Value = Chr(typZiseki2.Dt(ii).Kizukbn(0))
            dgvFF2(EM_MRATE_COL_ID.iTypeID, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).Kind)
            dgvFF2(EM_MRATE_COL_ID.iGradeID, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).Grade)
            dgvFF2(EM_MRATE_COL_ID.iHaba, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).WidPos)

            ' 0.1m�� �Ή� --->>>
            '�r����
            ''dgvFF2(EM_MRATE_COL_ID.iLen, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).Len)
            strBuff = tMod.ByteToString(typZiseki2.Dt(ii).Len)
            If strBuff.Trim().Length <> 0 Then
                dgvFF2(EM_MRATE_COL_ID.iLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF2(EM_MRATE_COL_ID.iLen, ii).Value = ""
            End If

            '�r�����������ʒu
            ''dgvFF2(EM_MRATE_COL_ID.iTotalLen, ii).Value = tMod.ByteToString(typZiseki2.Dt(ii).DefectPos)
            strBuff = tMod.ByteToString(typZiseki2.Dt(ii).DefectPos)
            If strBuff.Trim().Length <> 0 Then
                dgvFF2(EM_MRATE_COL_ID.iTotalLen, ii).Value = Format(CSng(IIf(IsNumeric(strBuff), strBuff, "0")) / 10, "0.0")
            Else
                dgvFF2(EM_MRATE_COL_ID.iTotalLen, ii).Value = ""
            End If

            ' 0.1m�� �Ή� <<<---

            dgvFF2(EM_MRATE_COL_ID.iKizuDiv, ii).Value = Chr(typZiseki2.Dt(ii).kbn(0))

            Dim sNoHex As String = tMod.ByteToString(typZiseki2.Dt(ii).No)
            Dim nNo As Integer
            Dim sNo As String = sNoHex

            '// 20200520 �r�ۑ�������g�� --->>>
            '�rNo 16�i��(10�i��)
            'If Integer.TryParse(sNoHex, Globalization.NumberStyles.HexNumber, Nothing, nNo) = True Then
            '    sNo &= String.Format(" ({0,5})", nNo)
            'End If

            ''�rNo 32�i��(10�i��)
            If Base32Hex_Decord(typZiseki2.Dt(ii).No, nNo) = True Then
                sNo &= String.Format(" ({0,7})", nNo)
            End If
            '// 20200520 �r�ۑ�������g�� ---<<<
            dgvFF2(EM_MRATE_COL_ID.iKizuNo, ii).Value = sNo

            dgvFF2(EM_MRATE_COL_ID.iOpeDsp, ii).Value = Chr(typZiseki2.Dt(ii).OpeDsp(0))
            dgvFF2(EM_MRATE_COL_ID.iSysKbn, ii).Value = Chr(typZiseki2.Dt(ii).SysKbn(0))
        Next ii

    End Sub

    '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) ----->>>
    ''' <summary>
    ''' ������ԓ`���{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnStatus_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStatus.Click
        If "" = cmbID.Text Then Exit Sub

        Dim frm As New frmKensaStatus(1, cmbID.Text)

        frm.Show()
        'frm.Dispose()
        'frm = Nothing
    End Sub
    '// 2015.03.02 T.Mizoguchi �I�y���[�^�K�C�_���X����(������ԓ`���ǉ�) <<<-----

    '// 20200520 �r�ۑ�������g�� --->>>
    ''' <summary>
    ''' Base32HEX�f�R�[�h
    ''' </summary>
    ''' <param name="val"></param>
    ''' <param name="nResult"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Base32Hex_Decord(ByVal val As Byte(), ByRef nResult As Integer) As Boolean

        Dim nDec As Integer = 0
        Dim nTotal As Integer = 0

        For ii As Integer = 0 To val.Length - 1
            Dim bt As Byte = val(val.Length - (1 + ii))
            If &H30 <= bt And &H39 >= bt Then
                nDec = CInt(bt - &H30)
            ElseIf &H41 <= bt And &H56 >= bt Then
                nDec = CInt((bt - &H41) + 10)
            ElseIf &H61 <= bt And &H76 >= bt Then
                nDec = CInt((bt - &H61) + 10)
            Else
                Return False
            End If

            nTotal = nTotal + nDec * CInt(Math.Pow(32, ii))
        Next

        nResult = nTotal
        Return True

    End Function
    '// 20200520 �r�ۑ�������g�� ---<<<


End Class
