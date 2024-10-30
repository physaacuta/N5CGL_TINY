'*********************************************************************************
' �o���P���r������CSV�o�͉��
'	[Ver]
'		Ver.01    2007/08/29  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary

Public Class frmPop_Dekon

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////

    Private Const INI_TYPE_KEY As String = "Dekon_Type"
    Private Const INI_GRADE_KEY As String = "Dekon_Grade"

    '�R�C���������ʗ�No
    Private Enum EM_RESULT_COL
        nDATE = 0       ' �ʔ��t
        nCoilNo         ' ���YNo
        nOrderNo        ' ����No
        nKoban          ' �����q��
        nCutPos_F       ' �J�b�g�͈�(�e)
        nCutPos_T       ' �J�b�g�͈�(�s)
        nCoilLen        ' �R�C����
        nDspKizuCnt_O   ' �\���r���\
        nDspKizuCnt_U   ' �\���r����
        nMixedRate_O    ' ������ (�\)
        nMixedRate_U    ' ������ (��)
        nKizKanri       ' �r�Ǘ��ԍ�

    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���v���p�e�B
    '/ /////////////////////////////////////////////////////////////////////////////////



    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()


        '������
        txtCoil.Text = ""               ' ���YNo.
        txtSaveName.Text = ""           ' �Ǘ�No.
        lblGuide.Text = ""              ' n��������܂���

        '�f�t�H���g�œ��t�ƁA�������ʕ\�������~���ɂ���
        chkDateSet.Checked = True       ' ���t�t�B���^
        rdoSortDesc.Checked = True      ' ��������...�~��
        chkNagate.Checked = False       ' ����������]
        chkHaba.Checked = False         ' ���������]


        '�f�[�^�O���b�h������(���ʍ���)
        Call dgvInitialize(dgvResult)

        '�f�[�^�O���b�h������(���C���ŗL����)
        Call dgvInitCGL(dgvResult)


        '�r��E�O���[�h�t�B���^�[�̏������N���X�ɃZ�b�g
        MyBase.tProp_IniSection = SO_GAMENN
        MyBase.tProp_TypeKey = INI_TYPE_KEY
        MyBase.tProp_GradeKey = INI_GRADE_KEY
        MyBase.tProp_IniName = LOCALINI_NAME

        '�r��t�B���^�[�\��
        MyBase.SetFilterData(dgvDispType, DB_TYPE_NAME, SO_GAMENN, INI_TYPE_KEY)

        '�O���[�h�t�B���^�[�\��
        MyBase.SetFilterData(dgvDispGrade, DB_GRADE_NAME, SO_GAMENN, INI_GRADE_KEY)

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(btnSearch, 5 - 1)
        AddFunctionButton(btnEnd, 12 - 1)

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

    End Sub


    ''' <summary>
    ''' �t�H�[���ǂݍ��݊�������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPop_Dekon_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Me.Cursor = Cursors.WaitCursor

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        '�R�C������
        Call SearchCoil()
        Me.Cursor = Cursors.Arrow
    End Sub


    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����{�^��
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V�����{�^��No 0�I���W��</param>
    ''' <param name="onKey"></param>
    Private Sub frmSearch_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' �t�@���N�V�����{�^�� (0�I���W��)
                '---------------------------------------------------------------
                Case 5 - 1                                                      ' �R�C������
                    btnSearch_Click(Nothing, Nothing)
                    '-----------------------------------------------------------
                Case 12 - 1                                                     ' �I��
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("���ь�����ʃt�@���N�V�����C�x���g�ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        If Not CheckSearch() Then
            lblGuide.Text = "�����L�[����͂��Ă�������"
            Exit Sub
        End If

        Me.Cursor = Cursors.WaitCursor
        '�R�C������
        Call SearchCoil()
        Me.Cursor = Cursors.Arrow
    End Sub

    ''' <summary>
    ''' ����{�^������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.Hide()
    End Sub


    ''' <summary>
    ''' ���������`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckSearch() As Boolean

        '�����������w�肳��Ă��Ȃ���΁A�G���[
        If txtCoil.Text.Trim = "" And txtSaveName.Text.Trim = "" And
              Not chkDateSet.Checked And Not chkTimeSet.Checked Then
            Return False
        End If

        Return True

    End Function



    ''' <summary>
    ''' �R�C������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SearchCoil()

        Dim nCnt As Integer = 0             ' �J�E���^(����)
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""
        Dim strSqlKoban As String = ""

        Try
            dgvResult.RowCount = 0

            ' �������� DB�I�[�v��
            If Not tcls_DB.Open() Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_COIL_CUT_POS & "]")
                Exit Sub
            End If


            ' -----------------------------------------------------------
            ' ���ׂĂ̏����Ō���
            ' -----------------------------------------------------------
            Call CreateSql(strSql)                      ' SQL���쐬

            ' Select�����s
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[(1) [" & DB_COIL_CUT_POS & "] SQL = " & strSql)
                Exit Sub
            End If


            ' �f�[�^�\��
            While sqlRead.Read()
                dgvResult.RowCount += 1

                ' --------------------------------
                ' �R�C�����\��(�q�ԈȊO)
                ' --------------------------------
                dgvResult(EM_RESULT_COL.nDATE, nCnt).Value = sqlRead.GetDateTime(sqlRead.GetOrdinal("�o�^���t")).ToString("yyyy/MM/dd HH:mm:ss")
                dgvResult(EM_RESULT_COL.nCoilNo, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("���YNo")).ToString.Trim
                dgvResult(EM_RESULT_COL.nOrderNo, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("����No"))

                Dim CoilL As Integer = CInt(sqlRead.GetSqlValue(sqlRead.GetOrdinal("�R�C����")).ToString)
                Dim CoilF As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("�J�b�g�͈͂e"))).ToString)
                Dim CoilT As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("�J�b�g�͈͂s"))).ToString)
                Dim Kizu_o As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("�\���r���\"))).ToString)
                Dim Kizu_u As Integer = CInt((sqlRead.GetSqlValue(sqlRead.GetOrdinal("�\���r����"))).ToString)

                dgvResult(EM_RESULT_COL.nCoilLen, nCnt).Value = (CoilL / 1000).ToString("###0.0")       ' �R�C������
                dgvResult(EM_RESULT_COL.nCutPos_F, nCnt).Value = (CoilF / 1000).ToString("###0.0")      ' �J�b�g�͈͂e
                dgvResult(EM_RESULT_COL.nCutPos_T, nCnt).Value = (CoilT / 1000).ToString("###0.0")      ' �J�b�g�͈͂s
                dgvResult(EM_RESULT_COL.nDspKizuCnt_O, nCnt).Value = Kizu_o             ' �\���r���\
                dgvResult(EM_RESULT_COL.nDspKizuCnt_U, nCnt).Value = Kizu_u             ' �\���r����

                ' ������
                dgvResult(EM_RESULT_COL.nMixedRate_O, nCnt).Value = ""
                dgvResult(EM_RESULT_COL.nMixedRate_U, nCnt).Value = ""
                If CoilL > 0 Then
                    dgvResult(EM_RESULT_COL.nMixedRate_O, nCnt).Value = ((Kizu_o * 1000 / CoilL) * 100).ToString("##0.00")
                    dgvResult(EM_RESULT_COL.nMixedRate_U, nCnt).Value = ((Kizu_u * 1000 / CoilL) * 100).ToString("##0.00")
                End If


                ' �r�Ǘ��ԍ�
                dgvResult(EM_RESULT_COL.nKizKanri, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("�Ǘ�No"))

                ' �����q��
                dgvResult(EM_RESULT_COL.nKoban, nCnt).Value = sqlRead.GetSqlValue(sqlRead.GetOrdinal("�����q��"))

                nCnt += 1
            End While
            sqlRead.Close()
            sqlRead = Nothing


            If nCnt = 0 Then
                Exit Sub                             ' �R�C�����Ȃ��Ƃ��́A�I��
            End If


        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�R�C����񌟍��G���[ [" & ex.Message & "] SQL = " & strSql)
        Finally
            If Not sqlRead Is Nothing Then
                sqlRead.Close()
                sqlRead = Nothing
            End If

            If Not tcls_DB Is Nothing Then
                tcls_DB.Cancel()
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

        If 0 <> nCnt Then
            lblGuide.Text = CStr(nCnt) & "��������܂���"
        Else
            lblGuide.Text = "�f�[�^��������܂���ł����B"
        End If

    End Sub



    ''' <summary>
    ''' �R�C�������pSQL���쐬
    ''' </summary>
    ''' <param name="strSql"></param>
    ''' <remarks></remarks>
    Private Sub CreateSql(ByRef strSql As String)

        Dim ii As Integer
        Dim strSqlFilter As String


        '�r���t�B���^�[�pSQL�擾
        strSqlFilter = ""

        '���׏��t�B���^�[
        With dgvDispType
            For ii = 0 To .RowCount - 1
                If Not CType(dgvDispType(EM_FILTER_COL.nCheck, ii).Value, Boolean) Then
                    strSqlFilter &= " AND NOT �����r�� = " & dgvDispType(EM_FILTER_COL.nEdasysID, ii).Value.ToString
                End If
            Next ii
        End With

        '�O���[�h�t�B���^�[
        With dgvDispGrade
            For ii = 0 To .RowCount - 1
                If Not CType(dgvDispGrade(EM_FILTER_COL.nCheck, ii).Value, Boolean) Then
                    strSqlFilter &= " AND NOT ����O���[�h = " & dgvDispGrade(EM_FILTER_COL.nEdasysID, ii).Value.ToString
                End If
            Next ii
        End With

        strSql = ""
        strSql &= " SELECT A.�o�^���t as �o�^���t "
        strSql &= "      , ISNULL(B.���YNo,'') as ���YNo "
        strSql &= "      , ISNULL(B.����No,'') as ����No "
        strSql &= "      , ISNULL(A.�擪�ʒu, 0) as �J�b�g�͈͂e "
        strSql &= "      , ISNULL(A.�ۗL����, 0) as �R�C���� "
        strSql &= "      , ISNULL(A.�擪�ʒu, 0) + ISNULL(A.�ۗL����, 0) as �J�b�g�͈͂s "
        strSql &= "      , ISNULL(C.��, 0) as �\���r���\ "
        strSql &= "      , ISNULL(D.��, 0) as �\���r���� "
        strSql &= "      , ISNULL(A.�Ǘ�No,'') as �Ǘ�No "
        strSql &= "      , ISNULL(A.ID,0) as CUTPOS_ID    "
        strSql &= "      , ISNULL(A.No,0) as CUTPOS_No    "
        strSql &= "      , ISNULL(A.�����q��,0) as �����q��    "

        strSql &= " FROM  " & DB_COIL_CUT_POS & " A "
        strSql &= " LEFT OUTER JOIN " & DB_COIL_INF & " B "
        strSql &= " ON A.�Ǘ�No = B.�Ǘ�No "

        strSql &= " LEFT OUTER JOIN ( SELECT CA.ID, CA.No, count(*) as �� from "
        strSql &= "     " & DB_COIL_CUT_POS & " AS CA RIGHT OUTER JOIN " & DB_DEFECT_INF_T & " AS CB "
        strSql &= "     ON CA.�Ǘ�No = CB.�Ǘ�No "
        strSql &= "     AND ( CB.����ʒu >=  ISNULL(CA.�擪�ʒu, 0) )"
        strSql &= "     AND ( CB.����ʒu <= (CAST(ISNULL(CA.�擪�ʒu, 0) as bigint) + CAST(ISNULL(CA.�ۗL����, 0) as bigint))  ) "

        strSql &= "     LEFT OUTER JOIN " & DB_COIL_RESULT & " AS CC "
        strSql &= "     ON CB.�Ǘ�No = CC.�Ǘ�No "

        strSql &= "     where CB.�d������ <> 1 " & strSqlFilter
        strSql &= "     Group by CA.ID, CA.No ) C "
        strSql &= " ON A.ID = C.ID and A.No = C.No "

        strSql &= " LEFT OUTER JOIN ( SELECT DA.ID, DA.No, count(*) as �� from "
        strSql &= "     " & DB_COIL_CUT_POS & " AS DA RIGHT OUTER JOIN " & DB_DEFECT_INF_B & " AS DB "
        strSql &= "     ON DA.�Ǘ�No = DB.�Ǘ�No "
        strSql &= "     AND ( DB.����ʒu >=  ISNULL(DA.�擪�ʒu, 0) )"
        strSql &= "     AND ( DB.����ʒu <= (CAST(ISNULL(DA.�擪�ʒu, 0) as bigint) + CAST(ISNULL(DA.�ۗL����, 0) as bigint))  ) "

        strSql &= "     LEFT OUTER JOIN " & DB_COIL_RESULT & " AS DC "
        strSql &= "     ON DB.�Ǘ�No = DC.�Ǘ�No "

        strSql &= "     where DB.�d������ <> 1 " & strSqlFilter
        strSql &= "     Group by DA.ID, DA.No ) D "
        strSql &= " ON A.ID = D.ID and A.No = D.No "

        strSql &= " LEFT OUTER JOIN " & DB_COIL_RESULT & " E "
        strSql &= " ON A.�Ǘ�No = E.�Ǘ�No "

        strSql &= " WHERE 1 = 1"


        '�R�C��No
        If "" <> txtCoil.Text.Trim Then
            strSql = strSql & " and B.���YNo like '" & txtCoil.Text.Trim & "' "
        End If

        '�ۑ��t�@�C����
        If "" <> txtSaveName.Text.Trim Then
            strSql = strSql & " and A.�Ǘ�No like '" & txtSaveName.Text.Trim & "' "
        End If

        '�����i�J�n�j
        If chkDateSet.Checked Then
            '���w��
            strSql = strSql & " and A.�o�^���t >= '" & Format(dtpDateStart.Value, "yyyy/MM/dd ")
            '���w��
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeStart.Value, "HH:mm:ss") & "' ",
             strSql & "00:00:00' "))
        End If

        '�����i�I���j
        If chkDateSet.Checked Then
            '���w��
            strSql = strSql & " and A.�o�^���t <= '" & Format(dtpDateEnd.Value, "yyyy/MM/dd ")
            '���w��
            strSql = CStr(IIf(chkTimeSet.Checked,
             strSql & Format(dtpTimeEnd.Value, "HH:mm:ss") & "' ",
             strSql & "23:59:59' "))
        End If

        '' �I�[�_�[��
        strSql = CStr(IIf(rdoSortAsc.Checked,
         strSql & " ORDER BY A.ID ASC, A.No ASC",
         strSql & " ORDER BY A.ID DESC, A.No DESC"))


    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �������� �O���b�h������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �������� �O���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As DataGridView)

        With dgv

            .ColumnCount = 0

            '' �����`
            .AutoGenerateColumns = False            ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False             ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False          ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False        ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False       ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False          ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                 ' �Z���G���[��\�����Ȃ�
            .ShowCellToolTips = False               ' �c�[���q���g��\�����Ȃ�
            .ShowEditingIcon = False                ' 
            .ShowRowErrors = False                  '

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect    ' �Z���I��
            .MultiSelect = False                                        ' �����s�I��
            .ReadOnly = False                                           ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                ' �Z����������

            .ScrollBars = ScrollBars.Both                                                           '�X�N���[���o�[
            .ColumnHeadersVisible = True                                                            ' ��w�b�_�\��
            .ColumnHeadersHeight = 45         ' ��w�b�_��������
            .RowHeadersVisible = False                                                              ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None                             ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None                                   ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .RowCount = 0

        End With

    End Sub



    ''' <summary>
    ''' �������ʃO���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitCGL(ByVal dgv As DataGridView)

        With dgv
            ''''' ��ǉ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �ʔ���
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���YNO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �����q��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �J�b�g�͈�(�e)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �J�b�g�͈�(�s)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �R�C����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �\���r��(�\)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �\���r��(��)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ������(�\)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ������(��)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �_�~�[


            '''' ���`
            '�ʔ���
            .Columns(EM_RESULT_COL.nDATE).HeaderText = "�ʔ���" & vbCrLf
            .Columns(EM_RESULT_COL.nDATE).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDATE).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDATE).Width = 170
            .Columns(EM_RESULT_COL.nDATE).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nDATE).ReadOnly = True

            '���YNO
            .Columns(EM_RESULT_COL.nCoilNo).HeaderText = "���YNo" & vbCrLf
            .Columns(EM_RESULT_COL.nCoilNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_RESULT_COL.nCoilNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCoilNo).Width = 95
            .Columns(EM_RESULT_COL.nCoilNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCoilNo).ReadOnly = True
            .Columns(EM_RESULT_COL.nCoilNo).Frozen = True                                   ' ���X�N���[�����A�\����Œ�

            '����No
            .Columns(EM_RESULT_COL.nOrderNo).HeaderText = "����No" & vbCrLf
            .Columns(EM_RESULT_COL.nOrderNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nOrderNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nOrderNo).Width = 65
            .Columns(EM_RESULT_COL.nOrderNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nOrderNo).ReadOnly = True

            '�����q��
            .Columns(EM_RESULT_COL.nKoban).HeaderText = "����" & vbCrLf & "�q��"
            .Columns(EM_RESULT_COL.nKoban).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKoban).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKoban).Width = 50
            .Columns(EM_RESULT_COL.nKoban).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nKoban).ReadOnly = True

            '�J�b�g�͈�(�e)
            .Columns(EM_RESULT_COL.nCutPos_F).HeaderText = "�J�b�g" & vbCrLf & "�͈�(�e)"
            .Columns(EM_RESULT_COL.nCutPos_F).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nCutPos_F).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCutPos_F).Width = 80
            .Columns(EM_RESULT_COL.nCutPos_F).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCutPos_F).ReadOnly = True

            '�J�b�g�͈�(�s)
            .Columns(EM_RESULT_COL.nCutPos_T).HeaderText = "�J�b�g" & vbCrLf & "�͈�(�s)"
            .Columns(EM_RESULT_COL.nCutPos_T).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nCutPos_T).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCutPos_T).Width = 80
            .Columns(EM_RESULT_COL.nCutPos_T).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCutPos_T).ReadOnly = True

            '�R�C����
            .Columns(EM_RESULT_COL.nCoilLen).HeaderText = "�R�C����" & vbCrLf & "[m]"
            .Columns(EM_RESULT_COL.nCoilLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nCoilLen).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nCoilLen).Width = 80
            .Columns(EM_RESULT_COL.nCoilLen).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nCoilLen).ReadOnly = True

            '�\���r��
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).HeaderText = "�\���r" & vbCrLf & "��(�\)"
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).Width = 80
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nDspKizuCnt_O).ReadOnly = True

            '�\���r��
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).HeaderText = "�\���r" & vbCrLf & "��(��)"
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).Width = 80
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nDspKizuCnt_U).ReadOnly = True

            '������(�\)
            .Columns(EM_RESULT_COL.nMixedRate_O).HeaderText = "������" & vbCrLf & "�\[%]"
            .Columns(EM_RESULT_COL.nMixedRate_O).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nMixedRate_O).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nMixedRate_O).Width = 75
            .Columns(EM_RESULT_COL.nMixedRate_O).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nMixedRate_O).ReadOnly = True

            '������(��)
            .Columns(EM_RESULT_COL.nMixedRate_U).HeaderText = "������" & vbCrLf & "��[%]"
            .Columns(EM_RESULT_COL.nMixedRate_U).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_RESULT_COL.nMixedRate_U).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nMixedRate_U).Width = 75
            .Columns(EM_RESULT_COL.nMixedRate_U).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nMixedRate_U).ReadOnly = True

            '�Ǘ�No
            .Columns(EM_RESULT_COL.nKizKanri).HeaderText = "�Ǘ�No"
            .Columns(EM_RESULT_COL.nKizKanri).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKizKanri).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_RESULT_COL.nKizKanri).Width = 160
            .Columns(EM_RESULT_COL.nKizKanri).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_RESULT_COL.nKizKanri).ReadOnly = True

        End With

    End Sub



    ''' <summary>
    ''' CSV�{�^����������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCsv_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCsv.Click

        'CSV�o��
        Call CsvOut(CSV_PATH, "�o���������\�����.csv")

    End Sub



    ''' <summary>
    ''' ��������� �X�V
    ''' </summary>
    ''' <param name="nVal">�i���l</param>
    ''' <remarks></remarks>
    Private Sub DspWaitProg(ByVal nVal As Integer)

        '�i���l�Z�b�g
        prbMsg.Value = CInt(IIf(nVal < prbMsg.Maximum, nVal, prbMsg.Maximum))

    End Sub


    ''' <summary>
    ''' �I���{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSelect.Click

    End Sub


    ''' <summary>
    ''' �L�����Z���{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCancel.Click

    End Sub

End Class

