'*********************************************************************************
'�@�̐����уN���X
'	[Ver]
'		Ver.01    2007/09/22  ����
'
'	[����]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Public Class frmSaisun

    Private m_mode As Integer               '�v�����[�h(1:�I�����C���p�A0:�c�[���p�j
    Private m_strKizukenNo As String        '�r���Ǘ�No

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxRow_MRare As Integer = 16           '�f�[�^�sMAX
    Private Const DgvMaxCol_MRare As Integer = 3            '�f�[�^��MAX

    ' �O���b�h�@��ݒ�
    Private Enum EM_MRATE_COL_ID
        iTitle = 0          '���ڌ��o��
        iStartPos           '�J�n�ʒu
        iEndPos             '�I���ʒu
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
    Private Sub frmSaisun_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

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
            strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
            strSQL &= " WHERE �Ǘ�No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY �o�^���t DESC"
        Else                         '�c�[�����[�h
            'strSQL &= " SELECT TOP 100 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
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

        '����No
        lblMeiNo.Text = ""

        '���YNO
        lblProductNo.Text = ""

        '���уR�C����
        lblCoilLen.Text = ""

        '�t�����g�������܂꒷��
        lblFLen.Text = ""

        '�Z�b�g�����̐���
        lblSetCnt.Text = ""

        '�O�����g�t���O
        lblGrant.Text = ""
        lblGrant.BackColor = Color.FromKnownColor(KnownColor.Control)

        ''�r���I�[�o�[�X�e�[�^�X

        ''�f�[�^�ݒ萔


        '�J�b�g�f�[�^
        dgvInitialize(dgvCut)

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
    ''' <param name="strID">ID</param>
    ''' <param name="typZiseki">�J�b�g���ѓ`��</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki As PRO_TYPE_SAISUN) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String        'SQL��
        Dim bytData() As Byte       '���`���o�C�i���f�[�^

        '�r���Ǘ�No����
        strSQL = ""
        strSQL &= " SELECT ���`��"
        strSQL &= " FROM " & DB_BUF_PRO_R_SAISUN
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

            ' ���`��
            bytData = CType(sqlRead.GetValue(0), Byte())
            Dim ii As Integer
            ii = tMod.SizeOf(typZiseki)
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
    ''' ID���X�g�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>

    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click(sender, e)
    End Sub

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typZiseki As PRO_TYPE_SAISUN = Nothing  '�r�������уf�[�^

        '�f�[�^�\��
        If GetData(cmbID.Text, typZiseki) Then
            Call SetData(typZiseki)
        End If
    End Sub

    ''' <summary>
    ''' �f�[�^�Z�b�g
    ''' </summary>
    '''<param name="typZiseki">�J�b�g����</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_SAISUN)

        ''����No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.KeyInf.cMeiNo)

        ''���YNO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.KeyInf.cCoilNo)

        '���уR�C����
        lblCoilLen.Text = Format(CSng(typZiseki.data.nLen) / 10, "0.0")
        'lblCoilLen.Text = typZiseki.data.nLen

        '�t�����g�������܂꒷��
        lblFLen.Text = Format(CSng(typZiseki.data.nMakiLenF) / 10, "0.0")
        'lblFLen.Text = typZiseki.data.nMakiLenF

        '�Z�b�g�����̐���
        lblSetCnt.Text = typZiseki.data.nSetCnt

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

        ' �̐��J�n�ʒu�A�̐��I���ʒu
        For ii As Integer = 0 To DgvMaxRow_MRare - 1
            dgvCut(0, ii).Value = ii + 1
            'dgvCut(1, ii).Value = typZiseki.data.saisun(ii).nStart
            dgvCut(1, ii).Value = Format(CSng(typZiseki.data.inf(ii).nStart) / 10, "0.0")

            'dgvCut(2, ii).Value = typZiseki.data.saisun(ii).nEnd
            dgvCut(2, ii).Value = Format(CSng(typZiseki.data.inf(ii).nEnd) / 10, "0.0")
        Next ii

    End Sub

    ''' <summary>
    ''' �O���b�h�̏�����
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
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
            .Columns(EM_MRATE_COL_ID.iTitle).HeaderText = "NO"
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 40
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable



            '��P (�A�h���X) �r���o�敪
            .Columns(EM_MRATE_COL_ID.iStartPos).HeaderText = "�J�n�ʒu"
            .Columns(EM_MRATE_COL_ID.iStartPos).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iStartPos).Width = 100
            .Columns(EM_MRATE_COL_ID.iStartPos).SortMode = DataGridViewColumnSortMode.NotSortable


            '��Q (�A�h���X) �r��
            .Columns(EM_MRATE_COL_ID.iEndPos).HeaderText = "�I���ʒu"
            .Columns(EM_MRATE_COL_ID.iEndPos).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iEndPos).Width = 108
            .Columns(EM_MRATE_COL_ID.iEndPos).SortMode = DataGridViewColumnSortMode.NotSortable
        End With
    End Sub
End Class
