'*********************************************************************************
' �ُ��ԕ\�����
'	[Ver]
'		Ver.01    2010/11/15 ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuForm
Imports tClassLibrary.tClass.tClass_SQLServer

Public Class frmStart
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��`
    '/ /////////////////////////////////////////////////////////////////////////////////
    '���W�I�{�^���敪
    Private Enum EM_RAD_KUBUN
        nErr = 0      '�ُ�̂ݕ\��
        nDefult = 1   '����{�ُ�
        nAll = 2      '�S��
    End Enum
    '// �f�[�^�O���b�h�萔
    Public Enum en_KikiList                 ' �k�ޏ�ԕ\��
        iNo = 0                             ' NO
        iKiKiName                           ' �@�햼��
        iIjouHani                           ' �ُ�͈�
        iKensa                              ' �������
        iKiki                               ' �@����
        iData                               ' ���l
        iMsg                                ' �ُ탁�b�Z�[�W
        iBikou                              ' ���l
    End Enum
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nRadKubun As EM_RAD_KUBUN            '���W�I�{�^���I��(0:�ُ�̂ݕ\��,1:����{�ُ�,2:�S��)

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub

    ''' <summary>
    ''' �t�H�[���A�N�e�B�u
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmSyukutai_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Activated

        '�ړ��\
        Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.tProp_EndButtonEnable = False

        '-------------------------------------------------------------------
        ' �t�@���N�V�����L�[�ݒ�
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)
    End Sub

    ''' <summary>
    ''' ��ʃ��[�h�� �����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmSyukutai_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        '�^�C�g���ݒ�
        modMain.SetTitleName(Me)

        '�|�b�v�A�b�v��� ��𑜓x�Ή�
        Call SetPcHeight(Me)

        ' �f�[�^�O���b�h �����ݒ�
        ' �@���ԕ\��
        dgvInitialize(dgvKiki)

        '�����ݒ�
        Call InitDsp()

        '�f�t�H���g�ُ���̂ݕ\��
        m_nRadKubun = EM_RAD_KUBUN.nErr
        radErr.Checked = True

        '�^�C�}�[�����ݒ�
        tmrDsp.Enabled = True
        tmrDsp.Interval = tMod.tMod_IniFile.ReadIniInt(SO_GAMENN, "SYUKUTAI_TIMER", TASKINI_NAME, 10000)

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �@���ԕ\�� �O���b�h������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �@���ԕ\�� �O���b�h������
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As DataGridView)

        With dgv
            .RowCount = 0
            .ColumnCount = 0

            '' �����`
            .AutoGenerateColumns = False            ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False             ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False          ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False        ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = True        ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False          ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                 ' �Z���G���[��\�����Ȃ�
            .ShowCellToolTips = False               ' �c�[���q���g��\�����Ȃ�
            .ShowEditingIcon = False                ' 
            .ShowRowErrors = False                  '

            .SelectionMode = DataGridViewSelectionMode.CellSelect       ' �Z���I��
            .MultiSelect = False                                         ' �����s�I��
            .ReadOnly = True                                             ' �ǂݎ���p
            .EditMode = DataGridViewEditMode.EditOnEnter                 ' �Z����������


            .ScrollBars = ScrollBars.Both                                                           '�X�N���[���o�[�L��
            .ColumnHeadersVisible = True                                                            ' ��w�b�_�\��
            .RowHeadersVisible = False                                                              ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None                             ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None                                   ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1�s�����ɐF�ւ�

            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' ��ǉ�
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �@�햼��
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �������
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �������
            .Columns.Add(New DataGridViewTextBoxColumn)   ' �@����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���l
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ���b�Z�[�W

            '''' ���`
            '1���:NO
            .Columns(en_KikiList.iNo).HeaderText = "NO" & vbCrLf
            .Columns(en_KikiList.iNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).Width = 50
            .Columns(en_KikiList.iNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_KikiList.iNo).Frozen = True


            '2���:�@�햼��
            .Columns(en_KikiList.iKiKiName).HeaderText = "�@�햼��" & vbCrLf
            .Columns(en_KikiList.iKiKiName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiKiName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiKiName).Width = 200
            .Columns(en_KikiList.iKiKiName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3���:�ُ�͈�
            .Columns(en_KikiList.iIjouHani).HeaderText = "�ُ�͈�" & vbCrLf
            .Columns(en_KikiList.iIjouHani).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iIjouHani).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iIjouHani).Width = 90
            .Columns(en_KikiList.iIjouHani).SortMode = DataGridViewColumnSortMode.NotSortable


            '4���:�������
            .Columns(en_KikiList.iKensa).HeaderText = "�������" & vbCrLf
            .Columns(en_KikiList.iKensa).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKensa).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKensa).Width = 90
            .Columns(en_KikiList.iKensa).SortMode = DataGridViewColumnSortMode.NotSortable


            '5���:�@����
            .Columns(en_KikiList.iKiki).HeaderText = "�@����" & vbCrLf
            .Columns(en_KikiList.iKiki).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKiki).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiki).Width = 90
            .Columns(en_KikiList.iKiki).SortMode = DataGridViewColumnSortMode.NotSortable


            '6���:���l
            .Columns(en_KikiList.iData).HeaderText = "���l" & vbCrLf
            .Columns(en_KikiList.iData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).Width = 50
            .Columns(en_KikiList.iData).SortMode = DataGridViewColumnSortMode.NotSortable


            '7���:�ُ�MSG
            .Columns(en_KikiList.iMsg).HeaderText = "�ُ�MSG" & vbCrLf
            .Columns(en_KikiList.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).Width = 1000
            .Columns(en_KikiList.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
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
    ''' �����I��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Me.Close()

    End Sub

    ''' <summary>
    ''' ������^�C�}�[
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrDsp_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrDsp.Tick

        'DB�ڑ��`�F�b�N
        If Not OpenCheck(TASKINI_NAME, INI_DB) Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '�f�[�^�\��
        Call DspData()


    End Sub

    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL��
        Dim nMaxRowCnt As Integer  '�ő�s��
        Dim nRowCnt As Integer = 0

        'DB�I�[�v��
        If Not tcls_DB.Open() Then
            'gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '�s���擾SQL�쐬
        strSQL = GetSQL(False)

        '���R�[�h�ǂݍ���
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            'gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�f�[�^�擾�G���[ [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '�ő�s���Z�b�g
        dgvKiki.RowCount = nMaxRowCnt

        '�\���f�[�^�̍��ڎ擾
        strSQL = ""
        strSQL = GetSQL(True)

        ' Select�����s
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' �f�[�^�\��
            While sqlRead.Read()

                ' �f�[�^���o��
                dgvKiki(en_KikiList.iNo, nRowCnt).Value = sqlRead.GetInt32(0)           '�@��ID
                dgvKiki(en_KikiList.iKiKiName, nRowCnt).Value = sqlRead.GetString(1)    '�@�햼

                ' �ُ�͈�
                If 0 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "�S��"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 1 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "�\��"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 2 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "����"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                ElseIf -1 = sqlRead.GetInt32(2) Then
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = "��ײ݂̂�"
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty
                Else
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = ""
                    dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty

                End If

                '������ԕ\��
                If 0 = sqlRead.GetInt32(3) Then
                    '�����ُ�x�Ȃ�
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then

                    '�@���Ԃ��ُ�̏ꍇ�͌����ُ�x�����ĕ\������
                    Select Case sqlRead.GetInt32(3)
						Case EM_DIV_KENSA.DIV_KENSA_NG
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "�k��"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
						Case EM_DIV_KENSA.DIV_KENSA_DBNON
							dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "���ۑ�"
							dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
						Case EM_DIV_KENSA.DIV_KENSA_SKIP
							dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "������"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KENSA.DIV_KENSA_STOP
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "��~"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case CType(-1, EM_DIV_KENSA)
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "��~"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KENSA.DIV_KENSA_MENTE
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "��Ò�"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KENSA.DIV_KENSA_EDGENG
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "���ޕs��"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                        Case Else
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "�s��"
                            dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                    End Select

                Else

                    '�����ُ�x����A�@���Ԑ���
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "����"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                End If

                '�@���ԕ\��
                If 0 = sqlRead.GetInt32(4) Then

                    '�@��ُ�x�Ȃ�
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then

                    '�@���Ԃ��ُ�̏ꍇ�́A�@��ُ�x�����ĕ\������
                    Select Case sqlRead.GetInt32(4)
                        Case EM_DIV_KIKI.DIV_KIKI_KEI
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "�y�̏�"
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Pink
                        Case EM_DIV_KIKI.DIV_KIKI_JYU
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "�d�̏�"
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Pink
                        Case Else
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "�s��"
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Pink
                    End Select

                Else

                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "����"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                End If

                '���l�Ƃ��ċ@���Ԃ�\������B
                dgvKiki(en_KikiList.iData, nRowCnt).Value = CStr(sqlRead.GetInt32(6))    '���l

                dgvKiki(en_KikiList.iMsg, nRowCnt).Value = sqlRead.GetString(5)          '�ُ�MSG

                '''' ���s����
                nRowCnt += 1

            End While

        Catch ex As Exception
            'gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�k�ޏ�ԕ\���G���[")
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' SQL���擾
    ''' </summary>
    ''' <param name="bMode">���ڎ擾�敪(True:�ʏ�,False:��(count(*)))</param>
    ''' <returns>�쐬����SQL��</returns>
    ''' <remarks></remarks>
    Private Function GetSQL(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '�s���擾
            strSQL &= " SELECT COUNT(*) "

        Else

            '���ڎ擾
            strSQL &= " SELECT a.�@��ID,"
            strSQL &= "        a.�@�햼,"
            strSQL &= "        a.�ُ�͈�,"
            strSQL &= "        a.�����ُ�x,"
            strSQL &= "        a.�@��ُ�x,"
            strSQL &= "        ISNULL(a.�ُ�MSG,''),"
            strSQL &= "        b.�@����,"

        End If

        '�Ō��","���폜
        sqlColumnTrim(strSQL)

        '// �ُ�̂ݕ\���͌���̂܂�
        '// ���ׂĕ\�� ��� �@��ID���ݒ肳��Ă���f�[�^���ׂĕ\��
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.�@��ID = b.�@��ID "

        ' �I���{�^���ɂ�镪��
        If m_nRadKubun = EM_RAD_KUBUN.nErr Then
            ' �ُ���̂ݕ\��
            strSQL &= " WHERE (a.�����ُ�x <> 0 or a.�@��ُ�x <> 0) "
            strSQL &= " AND b.�@���� = 1"
            'strSQL &= " AND b.�@���� <> 0"
        ElseIf m_nRadKubun = EM_RAD_KUBUN.nDefult Then
            ' ����{�ُ� ���\��
            strSQL &= " WHERE (a.�����ُ�x <> 0 or a.�@��ُ�x <> 0) "
        Else
            ' �S�ĕ\�� ��� �@��ID���ݒ肳��Ă���f�[�^���ׂĕ\��
        End If

        '�쐬����SQL��Ԃ�
        Return strSQL

    End Function
    ''' <summary>
    ''' �I���s�`�F���W
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvKiki.SelectionChanged

        '�I��������
        dgvKiki.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' �s�I��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowEnter

        '�s�̔w�i��ύX
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    End Sub
    ''' <summary>
    ''' �s�ړ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowLeave

        '�s�̔w�i���f�t�H���g�F�ɖ߂�
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' ���W�I�{�^���`�F�b�N�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radbtn_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim radData As RadioButton = CType(sender, RadioButton)

        '�`�F�b�N���O�ꂽ�ꍇ�́A������
        If Not radData.Checked Then
            'radData.BackColor = Color.FromKnownColor(KnownColor.Control)
            radData.BackColor = Color.Empty
            Exit Sub
        End If

        Select Case CInt(radData.Tag)
            Case EM_RAD_KUBUN.nErr
                radData.BackColor = Color.Cyan
            Case EM_RAD_KUBUN.nDefult
                radData.BackColor = Color.Cyan
            Case EM_RAD_KUBUN.nAll
                radData.BackColor = Color.Cyan
        End Select

        '�X�e�[�^�X�Z�b�g
        m_nRadKubun = CType(radData.Tag, EM_RAD_KUBUN)

        '�ĕ\��
        Call DspData()

    End Sub

    ''' <summary>
    ''' �����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '���W�I�{�^�������ݒ�
        radErr.Tag = EM_RAD_KUBUN.nErr
        radDefult.Tag = EM_RAD_KUBUN.nDefult
        radAll.Tag = EM_RAD_KUBUN.nAll

        '�C�x���g�ǉ�
        AddHandler radErr.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radDefult.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radAll.CheckedChanged, AddressOf radbtn_CheckedChanged

    End Sub


End Class
