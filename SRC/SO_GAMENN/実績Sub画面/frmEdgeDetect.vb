'*********************************************************************************
'�@�G�b�W���o�󋵉�ʃN���X
'	[Ver]
'		Ver.01    2011/06/03  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Public Class frmEdgeDetect
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    '�G�b�W���o���[�h�ω����
    Private Enum EM_EDGE_CHANGEINF
        emNagate = 0                '����ʒu
        emKenKubun                  '���o�敪
        emLEdgeOut                  '�����ޔ͈͊O
        emREdgeOut                  '�E���ޔ͈͊O
        emWidthError                '���덷�͈͊O
        emBeforeFrame               '�O�ڰє͈͊O
        emLEdgeItiReal              '�G�b�W�ʒu����
        emREdgeItiReal              '�G�b�W�ʒu���E
        emLEdgeItiEnd               '�G�b�W�ʒu�ŏI��
        emREdgeItiEnd               '�G�b�W�ʒu�ŏI�E
    End Enum

    '�G�b�W�����o�t���[�����
    Private Enum EM_EDGE_FRAMEINF
        emNagate = 0                '����ʒu
        emCameraSet                 '�J�����Z�b�g
        emFrameNo                   '�t���[��No
        emGazoUmu                   '�摜�L��
        emKanriNo                   '�Ǘ�No
    End Enum

    '---------------------------------------------------------------------------
    ' �萔��`
    '---------------------------------------------------------------------------
    Private Const RSC_IMG_DEFAULT As String = "SO_GAMENN_SPM.�r����.bmp"       ' �f�t�H���g�摜�̃��[�X�p�X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                            '���O�Ǘ��N���X
    Private m_strKizuken As String                                          ' �o���Ǘ�No
    Private m_nCoilLen As Integer                                           ' �R�C�����ђ� [mm]
    Private m_bNagate As Boolean                                            ' ���щ�ʒ���������](True:���]����Afalse:���]�Ȃ��j

    Private m_typCoilInf As clsDataMap.TYP_COIL_INF                         ' �R�C�����

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="cls_Log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef cls_Log As tClass.tClass_LogManager, ByVal typCoilInf As clsDataMap.TYP_COIL_INF, ByVal bNagate As Boolean, Optional ByVal strKizuken As String = "")

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
        mcls_Log = cls_Log
        m_strKizuken = strKizuken
        m_typCoilInf = typCoilInf
        m_nCoilLen = m_typCoilInf.nCoilLen       ' �R�C�����ђ� [mm]
        m_bNagate = bNagate                     ' ���щ�ʒ���������](True:���]����Afalse:���]�Ȃ��j

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)

    End Sub

    ''' <summary>
    ''' ��ʃ��[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeDetect_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        dgvInitialize(dgvChangeInf_O)
        dgvInitChangeInf(dgvChangeInf_O)

        dgvInitialize(dgvChangeInf_U)
        dgvInitChangeInf(dgvChangeInf_U)


        Data_Disp_Change(dgvChangeInf_O, 0)
        Data_Disp_Change(dgvChangeInf_U, 1)
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
                    cmdEnd_Click(Nothing, Nothing)
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
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdEnd.Click
        Me.Close()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �������� �O���b�h������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �������� �O���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        'Call dgv.DataGridViewSelectionCancel(dgv)						' �I��F����

        With dgv
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect    ' �Z���I��

            .ScrollBars = ScrollBars.Both                               ' �X�N���[���o�[
            .ColumnHeadersVisible = True                                ' ��w�b�_�\��
            .ColumnHeadersHeight = 45                                   ' ��w�b�_��������
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

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
    ''' ���`(�G�b�W���o���[�h�ω����)
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitChangeInf(ByVal dgv As tCnt.tCnt_DataGridView)

        With dgv
            .ColumnHeadersHeight = 45         ' ��w�b�_��������

            ''''' ��ǉ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����ʒu
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���o�敪
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �����ޔ͈͊O
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �E���ޔ͈͊O
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���덷�͈͊O
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �O�ڰє͈͊O
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �G�b�W�ʒu���E
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �G�b�W�ʒu����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �G�b�W�ʒu�ŏI��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �G�b�W�ʒu�ŏI�E


            '''' ���`
            '����ʒu
            .Columns(EM_EDGE_CHANGEINF.emNagate).HeaderText = "����ʒu" & vbCrLf & "[m]"
            .Columns(EM_EDGE_CHANGEINF.emNagate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emNagate).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emNagate).Width = 90
            .Columns(EM_EDGE_CHANGEINF.emNagate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emNagate).ReadOnly = True

            '���o�敪
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).HeaderText = "���o�敪" & vbCrLf
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).Width = 138
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emKenKubun).ReadOnly = True

            '�����ޔ͈͊O
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).HeaderText = "������" & vbCrLf & "�͈͊O"
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emLEdgeOut).ReadOnly = True

            '�E���ޔ͈͊O
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).HeaderText = "�E����" & vbCrLf & "�͈͊O"
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emREdgeOut).ReadOnly = True

            '���덷�͈͊O
            .Columns(EM_EDGE_CHANGEINF.emWidthError).HeaderText = "���덷" & vbCrLf & "�͈͊O"
            .Columns(EM_EDGE_CHANGEINF.emWidthError).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emWidthError).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emWidthError).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emWidthError).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emWidthError).ReadOnly = True

            '�O�ڰє͈͊O
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).HeaderText = "�O�ڰ�" & vbCrLf & "�͈͊O"
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).Width = 65
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emBeforeFrame).ReadOnly = True

            '�G�b�W�ʒu����
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).HeaderText = "���ވʒu" & vbCrLf & "����[mm]"
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiReal).ReadOnly = True

            '�G�b�W�ʒu���E
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).HeaderText = "���ވʒu" & vbCrLf & "���E[mm]"
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiReal).ReadOnly = True

            '�G�b�W�ʒu�ŏI��
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).HeaderText = "���ވʒu" & vbCrLf & "�ŏI��[mm]"
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emLEdgeItiEnd).ReadOnly = True

            '�G�b�W�ʒu�ŏI�E
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).HeaderText = "���ވʒu" & vbCrLf & "�ŏI�E[mm]"
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).Width = 95
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_CHANGEINF.emREdgeItiEnd).ReadOnly = True


        End With

    End Sub

    ''' <summary>
    ''' ���`(�G�b�W�����o�t���[�����)
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitFrameInf(ByVal dgv As tCnt.tCnt_DataGridView)

        With dgv
            .ColumnHeadersHeight = 45         ' ��w�b�_��������

            ''''' ��ǉ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����ʒu
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �J�����Z�b�g
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �t���[��No
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �摜�L��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �Ǘ�No


            '''' ���`
            '����ʒu
            .Columns(EM_EDGE_FRAMEINF.emNagate).HeaderText = "����ʒu" & vbCrLf & "[m]"
            .Columns(EM_EDGE_FRAMEINF.emNagate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_FRAMEINF.emNagate).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emNagate).Width = 90
            .Columns(EM_EDGE_FRAMEINF.emNagate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emNagate).ReadOnly = True

            '�J����No
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).HeaderText = "���" & vbCrLf & "No"
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).Width = 50
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emCameraSet).ReadOnly = True

            '�t���[��No
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).HeaderText = "�ڰ�" & vbCrLf & "No"
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).Width = 80
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emFrameNo).ReadOnly = True

            '�摜�L��
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).HeaderText = "�摜�L��" & vbCrLf
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).Width = 50
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emGazoUmu).ReadOnly = True

            '�Ǘ�No
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).HeaderText = "�Ǘ�No" & vbCrLf
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).Width = 50
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).ReadOnly = True
            .Columns(EM_EDGE_FRAMEINF.emKanriNo).Visible = False        ' ��\��


        End With

    End Sub

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <returns>���A���(Ture:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp_Change(ByVal dgvData As tCnt.tCnt_DataGridView, ByVal nTB As Integer) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""
        Dim nCnt As Integer = 0         '�J�E���^
        Dim nWk As Integer

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_EDGE_CHANGE_INF & "]")
            Return False
        End If

        'SQL���쐬
        Call CreateEdgeChangeInfSql(strSql, nTB, True)

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSql)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_EDGE_CHANGE_INF & "]")
            Return False
        End If

        Try

            dgvData.RowCount = 0
            ' �f�[�^�\��
            While sqlRead.Read()
                dgvData.RowCount += 1

                Dim nPos As Integer = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("����ʒu")))

                If m_typCoilInf.bCorrect Then
                    '�擾�ʒu�ɕ␳������Z
                    nPos = CInt(nPos * m_typCoilInf.dCorrect)
                End If

                If m_bNagate = True Then
                    ' ���щ�ʒ���������]
                    nPos = m_nCoilLen - nPos
                    If nPos < 0 Then
                        nPos = 0
                    End If
                End If
                dgvData(EM_EDGE_CHANGEINF.emNagate, nCnt).Value = Format(nPos / 1000, "#0.000")
                nWk = sqlRead.GetInt32(sqlRead.GetOrdinal("���o�敪"))
                If nWk = 0 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "����"
                ElseIf nWk = 1 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "�д��ޗD��"
                ElseIf nWk = 2 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "����ײݐU�蕪��"
                ElseIf nWk = -1 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "�����"
                ElseIf nWk = -9 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "�ΏۊO"
                ElseIf nWk = -99 Then
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = "�������"
                Else
                    dgvData(EM_EDGE_CHANGEINF.emKenKubun, nCnt).Value = ""
                End If

                nWk = sqlRead.GetInt32(sqlRead.GetOrdinal("�G�b�W�s�����R"))
                If 1 = (nWk And &H1) Then
                    ' ���G�b�W���o�͈͊O
                    dgvData(EM_EDGE_CHANGEINF.emLEdgeOut, nCnt).Value = "��"
                End If
                If 2 = (nWk And &H2) Then
                    ' �E�G�b�W���o�͈͊O
                    dgvData(EM_EDGE_CHANGEINF.emREdgeOut, nCnt).Value = "��"
                End If
                If 4 = (nWk And &H4) Then
                    ' �����e�덷�͈͊O
                    dgvData(EM_EDGE_CHANGEINF.emWidthError, nCnt).Value = "��"
                End If
                If 8 = (nWk And &H8) Then
                    ' �O�ڰтƂ̋��e�͈͊O
                    dgvData(EM_EDGE_CHANGEINF.emBeforeFrame, nCnt).Value = "��"
                End If

                '�����_�ȉ��؂�̂�
                dgvData(EM_EDGE_CHANGEINF.emLEdgeItiReal, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("�G�b�W�ʒu����")))

                '�����_�ȉ��؂�̂�
                dgvData(EM_EDGE_CHANGEINF.emREdgeItiReal, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("�G�b�W�ʒu���E")))

                '�����_�ȉ��؂�̂�
                dgvData(EM_EDGE_CHANGEINF.emLEdgeItiEnd, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("�G�b�W�ʒu�ŏI��")))

                '�����_�ȉ��؂�̂�
                dgvData(EM_EDGE_CHANGEINF.emREdgeItiEnd, nCnt).Value = Math.Truncate(sqlRead.GetFloat(sqlRead.GetOrdinal("�G�b�W�ʒu�ŏI�E")))
                nCnt += 1
            End While

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�G�b�W���o���[�h�ω��\���G���[ [" & ex.Message & "]")
        Finally
            tcls_DB.Cancel()
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing

        End Try
        If 0 < nCnt Then
            ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪 �i�e�L�X�g�{�b�N�X)
            dgvData(EM_EDGE_CHANGEINF.emNagate, 0).Selected = True
        End If

        Return True
    End Function

    ''' <summary>
    ''' �G�b�W���o���[�h�ω��Ǘ��擾SQL�쐬
    ''' </summary>
    ''' <param name="strSql">SQL��</param>
    ''' <param name="nTB">�\���敪(0:�\,1:��)</param>
    ''' <param name="bMode">�擾���[�h(True:�ʏ�,False:��(COUNT(*)))</param>
    ''' <remarks></remarks>
    Private Sub CreateEdgeChangeInfSql(ByRef strSql As String, ByVal nTB As Integer, ByVal bMode As Boolean)

        strSql = ""

        If Not bMode Then
            strSql &= " SELECT COUNT(*) "
        Else
            strSql &= " SELECT * "
        End If

        strSql &= " FROM " & DB_EDGE_CHANGE_INF
        strSql &= " WHERE �Ǘ�No = '" & m_strKizuken & "'"
        strSql &= " AND �\���敪 = " & nTB.ToString
        If bMode Then
            If m_bNagate = True Then
                strSql &= " order by ����ʒu DESC"
            Else
                strSql &= " order by ����ʒu ASC"
            End If
        End If

    End Sub


    ''' <summary>
    ''' �f�[�^���ݗL���`�F�b�N(�G�b�W���o����)
    ''' </summary>
    ''' <param name="strKizuKen"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ChkData_CoilInf(ByVal strKizuKen As String) As Boolean
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String    'SQL��
        Dim strWK As String = ""
        Dim bRet As Boolean = False

        Try

            strSQL = ""
            strSQL &= " SELECT TOP 1 �Ǘ�No"
            strSQL &= " FROM " & DB_EDGE_COIL_INF
            strSQL &= " WHERE �Ǘ�No = '" & strKizuKen & "'"

            'DB�I�[�v��
            If Not tcls_DB.Open() Then
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_EDGE_COIL_INF & "]")
                Return False
            End If

            ' Select�����s
            bRet = tcls_DB.GetSelectKey(strSQL, strWK)

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("{0}�擾�Ɏ��s���܂����B[{1}]", DB_EDGE_COIL_INF, ex.Message))
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DB�ɐڑ�����������ꍇ
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try

        Return bRet
    End Function


End Class
