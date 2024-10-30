'*********************************************************************************
'�@�r��������ʃN���X
'	[Ver]
'		Ver.01    2007/09/22  ����
'
'	[����]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Public Class frmMixedRateTool

    Private m_mode As Integer               '�v�����[�h(1:�I�����C���p�A0:�c�[���p�j
    Private m_strKizukenNo As String        '�r���Ǘ�No

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxRow_MRare As Integer = 11           '�f�[�^�sMAX
    Private Const DgvMaxCol_MRare As Integer = 7            '�f�[�^��MAX

    ' �O���b�h�@��ݒ�
    Private Enum EM_MRATE_COL_ID
        iTitle = 0              '���ڌ��o��
        iGrade_No01             'Grade1
        iGrade_No02             'Grade2
        iGrade_No03             'Grade3
        iGrade_No04             'Grade4
        iGrade_No05             'Grade5
        iGrade_No06             'Grade6
    End Enum

    ' �O���b�h �s�ݒ�
    Private Enum EM_MRATE_ROW_ID
        iRate = 0               '�r������
        iType_No01              '�r��No01
        iType_No02              '�r��No02
        iType_No03              '�r��No03
        iType_No04              '�r��No04
        iType_No05              '�r��No05
        iType_No06              '�r��No06
        iType_No07              '�r��No07
        iType_No08              '�r��No08
        iType_No09              '�r��No09
        iType_No10              '�r��No10
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
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        'Dim ii As Integer = 0
        With dgv
            .RowCount = DgvMaxRow_MRare

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
            .ScrollBars = ScrollBars.None


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
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' ���`
            .ColumnCount = DgvMaxCol_MRare           'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' ��w�b�_�T�C�Y

            '��P (�A�h���X) ���ڌ��o��
            .Columns(EM_MRATE_COL_ID.iTitle).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 120
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iTitle).Frozen = True                                       ' �����ړ��֎~

            '��Q (�A�h���X) �O���[�h�P
            .Columns(EM_MRATE_COL_ID.iGrade_No01).HeaderText = "�O���[�h�P"
            .Columns(EM_MRATE_COL_ID.iGrade_No01).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No01).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Frozen = True                                       ' �����ړ��֎~

            '��R (�A�h���X) �O���[�h�Q
            .Columns(EM_MRATE_COL_ID.iGrade_No02).HeaderText = "�O���[�h�Q"
            .Columns(EM_MRATE_COL_ID.iGrade_No02).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No02).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Frozen = True                                       ' �����ړ��֎~

            '��S (�A�h���X) �O���[�h�R
            .Columns(EM_MRATE_COL_ID.iGrade_No03).HeaderText = "�O���[�h�R"
            .Columns(EM_MRATE_COL_ID.iGrade_No03).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No03).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Frozen = True                                       ' �����ړ��֎~

            '��T (�A�h���X) �O���[�h�S
            .Columns(EM_MRATE_COL_ID.iGrade_No04).HeaderText = "�O���[�h�S"
            .Columns(EM_MRATE_COL_ID.iGrade_No04).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No04).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Frozen = True                                       ' �����ړ��֎~


            '��U (�A�h���X) �O���[�h�T
            .Columns(EM_MRATE_COL_ID.iGrade_No05).HeaderText = "�O���[�h�T"
            .Columns(EM_MRATE_COL_ID.iGrade_No05).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No05).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Frozen = True                                       ' �����ړ��֎~

            '��V (�A�h���X) �O���[�h�U
            .Columns(EM_MRATE_COL_ID.iGrade_No06).HeaderText = "�O���[�h�U"
            .Columns(EM_MRATE_COL_ID.iGrade_No06).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No06).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No06).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No06).Frozen = True                                       ' �����ړ��֎~

        End With
    End Sub

    ''' <summary>
    ''' ��ʃ��[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMixedRate_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Me.tProp_Moveable = True

        Call SetTitleName(Me)

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
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
            strSQL &= " WHERE �Ǘ�No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY �o�^���t DESC"
        Else                         '�c�[�����[�h
            'strSQL &= " SELECT TOP 100 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
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

        '����
        lblKoban.Text = ""

        '���ы敪
        lblZisekiDiv.Text = ""
        lblZisekiDiv.BackColor = Color.FromKnownColor(KnownColor.Control)

        '���W�敪
        lblColectDiv.Text = ""
        lblColectDiv.BackColor = Color.FromKnownColor(KnownColor.Control)

        '�O�����g�t���O
        lblGrant.Text = ""
        lblGrant.BackColor = Color.FromKnownColor(KnownColor.Control)

        '�r�������\ ������
        dgvInitialize(dgvMixedRate)

    End Sub


    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="strID">ID</param>
    ''' <param name="typZiseki">�r�������уf�[�^</param>
    ''' <param name="strKizuken">�Ǘ�No</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki As PRO_TYPE_ZISEKI, ByRef strKizuken As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData() As Byte        '���`���o�C�i���f�[�^

        '�r���Ǘ�No����
        strSQL = ""
        strSQL &= " SELECT ���`��,"
        strSQL &= "        �Ǘ�No"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
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
            bytData = CType(sqlRead.GetValue(0), Byte())

            '�r�����Ǘ�No
            strKizuken = sqlRead.GetValue(1).ToString

            tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

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
    ''' �f�[�^�Z�b�g
    ''' </summary>
    '''<param name="typZiseki">�v���R�����уf�[�^</param>
    ''' <param name="strKizuken">�r���Ǘ�No</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_ZISEKI, ByVal strKizuken As String)

        '�r���Ǘ�No
        lblKizuKen.Text = strKizuken

        ''����No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.cMeiNo)

        ''���YNO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.cCoilNo)

        '����
        lblKoban.Text = typZiseki.data.nKoban

        '�O�����g�t���O
        If 0 = typZiseki.data.Common.nGrandFlg Then
            lblGrant.Text = "����"
            lblGrant.BackColor = Color.Blue
            lblGrant.ForeColor = Color.Yellow
        Else
            lblGrant.Text = "�ُ�"
            lblGrant.BackColor = Color.Red
            lblGrant.ForeColor = Color.Yellow
        End If

        '���ы敪
        If 0 = typZiseki.data.nZiMode Then
            lblZisekiDiv.Text = "����"
            lblZisekiDiv.BackColor = Color.Blue
            lblZisekiDiv.ForeColor = Color.Yellow
        Else
            lblZisekiDiv.Text = "�ُ�_" & typZiseki.data.nZiMode.ToString
            lblZisekiDiv.BackColor = Color.Red
            lblZisekiDiv.ForeColor = Color.Yellow
        End If

        '���W�敪
        If 0 = typZiseki.data.nKikiMode Then
            lblColectDiv.Text = "����"
            lblColectDiv.BackColor = Color.Blue
            lblColectDiv.ForeColor = Color.Yellow
        Else
            lblColectDiv.Text = "�ُ�_" & typZiseki.data.nKikiMode.ToString
            lblColectDiv.BackColor = Color.Red
            lblColectDiv.ForeColor = Color.Yellow
        End If

        '���ڌ��o���ݒ�
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iRate).Value = "�r������ [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No01).Value = "�r��O�P [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No02).Value = "�r��O�Q [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No03).Value = "�r��O�R [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No04).Value = "�r��O�S [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No05).Value = "�r��O�T [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No06).Value = "�r��O�U [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No07).Value = "�r��O�V [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No08).Value = "�r��O�W [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No09).Value = "�r��O�X [��]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No10).Value = "�r��P�O [��]"


        '�����m�F�p
        For jj As Integer = EM_MRATE_COL_ID.iGrade_No01 To EM_MRATE_COL_ID.iGrade_No06
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Value = Format(typZiseki.data.grade_inf(jj - 1).nMixedRate / 100, "#0.00")
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Style.Alignment = DataGridViewContentAlignment.MiddleRight
            For ii As Integer = EM_MRATE_ROW_ID.iType_No01 To EM_MRATE_ROW_ID.iType_No10
                dgvMixedRate(jj, ii).Value = typZiseki.data.grade_inf(jj - 1).nTypeCnt(ii - 1)
                dgvMixedRate(jj, ii).Style.Alignment = DataGridViewContentAlignment.MiddleRight
            Next ii
        Next jj

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
    ''' �Z���I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMixedRate_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvMixedRate.SelectionChanged

        '�I��������
        dgvMixedRate.CurrentCell.Selected = False

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
        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '�r�������уf�[�^
        Dim strKizuken As String = ""       '�r���Ǘ�No

        '�f�[�^�\��
        If GetData(cmbID.Text, typZiseki, strKizuken) Then
            Call SetData(typZiseki, strKizuken)
        End If
    End Sub


End Class
