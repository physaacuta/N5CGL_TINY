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
Public Class frmMixedRate

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

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager    '���O�Ǘ��N���X
    Private m_strKizuken As String                  '�Ǘ�No
    Private m_dctID As New Dictionary(Of Integer, String)   '�Y���Ǘ�No�̃J�b�g���т�ID
    Private m_nIDCnt As Integer                     '�J�b�g���т�ID�̃J�E���^

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="cls_Log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef cls_Log As tClass.tClass_LogManager, Optional ByVal strKizuken As String = "")

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Me.Text = String.Format("[{0}] �r�������\�����", SYSTEM_TITLE)

        mcls_Log = cls_Log
        m_strKizuken = strKizuken

        '�T����ʂ̏ꍇ�́A�O���A�����{�^���͖���
        If "" = m_strKizuken Then
            btnPre.Visible = False
            btnNext.Visible = False
            lblPage.Visible = False
        End If

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
        Dim ii As Integer = 0
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
            .Columns(EM_MRATE_COL_ID.iGrade_No01).HeaderText = "Gr��ٰ��1"
            .Columns(EM_MRATE_COL_ID.iGrade_No01).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No01).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Frozen = True                                       ' �����ړ��֎~

            '��R (�A�h���X) �O���[�h�Q
            .Columns(EM_MRATE_COL_ID.iGrade_No02).HeaderText = "Gr��ٰ��2"
            .Columns(EM_MRATE_COL_ID.iGrade_No02).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No02).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Frozen = True                                       ' �����ړ��֎~

            '��S (�A�h���X) �O���[�h�R
            .Columns(EM_MRATE_COL_ID.iGrade_No03).HeaderText = "Gr��ٰ��3"
            .Columns(EM_MRATE_COL_ID.iGrade_No03).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No03).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Frozen = True                                       ' �����ړ��֎~

            '��T (�A�h���X) �O���[�h�S
            .Columns(EM_MRATE_COL_ID.iGrade_No04).HeaderText = "Gr��ٰ��4"
            .Columns(EM_MRATE_COL_ID.iGrade_No04).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No04).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Frozen = True                                       ' �����ړ��֎~


            '��U (�A�h���X) �O���[�h�T
            .Columns(EM_MRATE_COL_ID.iGrade_No05).HeaderText = "Gr��ٰ��5"
            .Columns(EM_MRATE_COL_ID.iGrade_No05).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No05).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Frozen = True                                       ' �����ړ��֎~

            '��V (�A�h���X) �O���[�h�U
            .Columns(EM_MRATE_COL_ID.iGrade_No06).HeaderText = "Gr��ٰ��6"
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

        Dim strCutID As String = ""      '�J�b�g����ID

        '��ʏ�����
        Call InitDsp()

        '���щ�ʂ̏ꍇ�A�Y���Ǘ�No�̃J�b�g���т�ID��S�Ď擾
        If m_strKizuken <> "" Then
            If Not GetCutID() Or 0 = m_dctID.Count Then Exit Sub
            m_nIDCnt = 0
            strCutID = m_dctID(m_nIDCnt)
        End If

        '�f�[�^�\��
        Call DspData(strCutID)

        '�{�^�L���A����
        Call SetPreNextBotton()

    End Sub

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <param name="strCutID"></param>
    ''' <remarks></remarks>
    Private Sub DspData(ByVal strCutID)

        Dim nOutCutNo As Integer = 0                '����No
        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '�r�������уf�[�^

        '�f�[�^�擾
        If Not GetData(nOutCutNo, typZiseki, strCutID) Then
            Exit Sub
        End If

        '�f�[�^�Z�b�g
        SetData(nOutCutNo, typZiseki)

    End Sub


    ''' <summary>
    ''' �f�[�^���ݗL���`�F�b�N
    ''' </summary>
    ''' <param name="strKizuKen"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ChkData(ByVal strKizuKen As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String    'SQL��
        Dim strWK As String = ""

        strSQL = ""
        strSQL &= " SELECT TOP 1 �Ǘ�No"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE �Ǘ�No = '" & strKizuKen & "'"
        strSQL &= " ORDER BY �o�^���t DESC"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_BUF_PRO_S_ZISEKI & "]")
            Return False
        End If

        ' Select�����s
        ChkData = tcls_DB.GetSelectKey(strSQL, strWK)
        tcls_DB.Dispose()
        tcls_DB = Nothing


    End Function



    ''' <summary>
    ''' ��ʏ�����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '����No
        lblMeiNo.Text = ""

        '���YNO
        lblProductNo.Text = ""

        '�o���J�b�g���ԁi5CGL�ł͔�\���j
        lblOutCutNo.Text = ""

        '���ы敪
        lblZisekiDiv.Text = ""
        lblZisekiDiv.BackColor = Color.FromKnownColor(KnownColor.Control)

        '���W�敪
        lblColectDiv.Text = ""
        lblColectDiv.BackColor = Color.FromKnownColor(KnownColor.Control)


        '�r�������\ ������
        dgvInitialize(dgvMixedRate)

    End Sub


    ''' <summary>
    ''' �f�[�^�擾
    ''' </summary>
    ''' <param name="nOutCutNo">����No</param>
    ''' <param name="typZiseki">�r�������уf�[�^</param>
    ''' <remarks></remarks>
    Private Function GetData(ByRef nOutCutNo As Integer, ByRef typZiseki As PRO_TYPE_ZISEKI, Optional ByVal strCutID As String = "") As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim bytData() As Byte        '���`���o�C�i���f�[�^
        Dim bRet As Boolean = False

        If "" = m_strKizuken Then

            '�Ǘ�No�Ȃ�()
            strSQL = ""
            strSQL &= " SELECT TOP 1 "
            strSQL &= " ����No"
            strSQL &= " ,���`��"
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
            strSQL &= " ORDER BY �o�^���t DESC"

        Else

            '�Ǘ�No����
            strSQL = ""
            strSQL &= " SELECT "
            strSQL &= " ����No"
            strSQL &= " ,���`��"
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
            strSQL &= " WHERE �Ǘ�No = '" & m_strKizuken & "'"
            strSQL &= " AND   ID = '" & strCutID & "'"

        End If

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_BUF_PRO_S_ZISEKI & "]")
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' �f�[�^�擾
            sqlRead.Read()

            If sqlRead.HasRows Then

                '����No
                nOutCutNo = CInt(sqlRead.GetValue(0).ToString)

                '���`��
                bytData = CType(sqlRead.GetValue(1), Byte())

                tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

                '����I��
                bRet = True

            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r�������\���G���[ [" & ex.Message & "]")
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return bRet

    End Function


    ''' <summary>
    ''' �f�[�^�Z�b�g
    ''' </summary>
    '''<param name="nOutCutNo">����No</param>
    '''<param name="typZiseki">�v���R�����уf�[�^</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal nOutCutNo As Integer, ByVal typZiseki As PRO_TYPE_ZISEKI)

        Dim ii As Integer   '���[�v�J�E���^
        Dim jj As Integer   '���[�v�J�E���^

        '����No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.cMeiNo)

        '���YNO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.cCoilNo)

        '�o���J�b�g���ԁi5CGL�ł͔�\���j
        lblOutCutNo.Text = nOutCutNo

        '���ы敪
        If 0 = typZiseki.data.nZiMode Then
            lblZisekiDiv.Text = "����"
            lblZisekiDiv.BackColor = Color.Blue
            lblZisekiDiv.ForeColor = Color.Yellow
        Else
            lblZisekiDiv.Text = "�ُ�"
            lblZisekiDiv.BackColor = Color.Red
            lblZisekiDiv.ForeColor = Color.Yellow
        End If

        '���W�敪
        If 0 = typZiseki.data.nKikiMode Then
            lblColectDiv.Text = "����"
            lblColectDiv.BackColor = Color.Blue
            lblColectDiv.ForeColor = Color.Yellow
        Else
            lblColectDiv.Text = "�ُ�"
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
        For jj = EM_MRATE_COL_ID.iGrade_No01 To EM_MRATE_COL_ID.iGrade_No06
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Value = Format(typZiseki.data.grade_inf(jj - 1).nMixedRate / 100, "#0.00")
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Style.Alignment = DataGridViewContentAlignment.MiddleRight
            For ii = EM_MRATE_ROW_ID.iType_No01 To EM_MRATE_ROW_ID.iType_No10
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
    ''' �Z���I���Ǘ�No
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMixedRate_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvMixedRate.SelectionChanged

        '�I��������
        dgvMixedRate.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' �Y���Ǘ�No�̃J�b�g���т�ID��S�Ď擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetCutID() As Boolean


        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim nCnt As Integer = 0    '�J�E���^

        strSQL = ""
        strSQL &= " SELECT ID"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
        strSQL &= " WHERE �Ǘ�No = '" & m_strKizuken & "'"
        strSQL &= " ORDER BY �o�^���t ASC"

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_STATUS_MASTER & "]")
            Return False
        End If

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False


        Try

            m_dctID.Clear()
            ' �f�[�^�擾
            While sqlRead.Read()

                m_dctID.Add(nCnt, sqlRead.GetString(0))
                nCnt += 1
            End While

            Return True
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r�������\���G���[ [" & ex.Message & "]")
            Return False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try



    End Function

    ''' <summary>
    ''' �O���{�^����������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnPre_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPre.Click

        m_nIDCnt -= 1

        '�f�[�^�\��
        Call DspData(m_dctID(m_nIDCnt))
        SetPreNextBotton()

    End Sub

    ''' <summary>
    ''' �����{�^����������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnNext_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnNext.Click

        m_nIDCnt += 1

        '�f�[�^�\��
        Call DspData(m_dctID(m_nIDCnt))
        SetPreNextBotton()
    End Sub


    ''' <summary>
    ''' �O���A�����{�^���̗L���A������ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetPreNextBotton()

        If m_nIDCnt = 0 Then

            btnPre.Enabled = False
        Else
            btnPre.Enabled = True
        End If

        If m_nIDCnt = m_dctID.Count - 1 Then
            btnNext.Enabled = False
        Else
            btnNext.Enabled = True
        End If

        lblPage.Text = CStr(m_nIDCnt + 1) & "/ " & CStr(m_dctID.Count)
    End Sub

End Class
