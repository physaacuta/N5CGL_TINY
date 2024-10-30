'*********************************************************************************
' �����p�^�[���}�X�^�[�o�^��ʃN���X
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Class frmPCodeMaster
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum EM_GRID_ID
        emTxt_Code = 0     ' �e�L�X�g�{�b�N�X�@�����p�^�[���R�[�h
        emChk_Check        ' �`�F�b�N�{�b�N�X�@�L������
        emTxt_Priority     ' �e�L�X�g�{�b�N�X�@�D��x
        emTxt_Name         ' �e�L�X�g�{�b�N�X�@�����\�ʏ�Ԗ���
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u��

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���j���[�{�^��
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�@���N�V�����ؑ�
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
    ''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
    ''' <param name="onKey">�����ɉ�����Ă���L�[</param>
    ''' <remarks></remarks>
    Private Sub frmPatternMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�����p�^�[���}�X�^�[�o�^���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLOR�֐ؑ֏���
            Case 10
                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' �ݒ�
            Case 11
                modMain.TaskEnd()               ' F12 �I��
        End Select

    End Sub

    ''' <summary>
    ''' ��ʕύX�v�����ꂽ�Ƃ��̏��� (�p�����N���X���R�[�������)
    ''' </summary>
    ''' <param name="frmID">�I�����ID</param>
    ''' <param name="cancel">�I��������߂�ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmPatternMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If

        ' ��ʐؑ�
        ShowForm(frmID)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���\�b�h��`
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPCodeMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCodeMaster           ' �����p�^�[���}�X�^�[�ݒ���

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        'dgvData������
        dgvInitialize(dgvData, True)

        'dgvEtc������
		dgvInitialize(dgvEtc, False)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ' ��ʂɒl��\��
        If Not Data_Disp() Then                                       '  ��ʍ��ڕ\������
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ' �X�V�{�^���̗L��/����
        Call UpdateEnabled(g_bPassFlg)

        '�v�����^���Ȃ���΁A��ʈ������
        If Not tClass_HeadCopy.IsCheckPrinter Then
            btnF07.Enabled = False
        Else
            btnF07.Enabled = True
        End If

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub

    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPCodeMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL������
        strSQL = " SELECT pcode"
        strSQL &= ", �L������"
        strSQL &= ", �D��x"
        strSQL &= ", �����p�^�[������"
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"

		'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <returns>����FTrue �ُ�FFalse</returns>
    ''' <remarks></remarks>
    Private Function Data_Disp() As Boolean
        Dim drowPcode() As DataRow = m_DS.Tables(0).Select()           ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        '�f�[�^���������p�^�[��MAX(64)�Ɠ����łȂ��ꍇ�ُ͈�Ȃ̂ŋ����I��������
        'HACK 2015/02/04�@��͂�K�v
        If MAX_PCODE <> drowPcode.Length Then
            Return False
        End If

        ' �f�[�^�O���b�h�r���[�̍s���ݒ�
        dgvData.RowCount = drowPcode.Length - 1

        ' ��ʂɕ\��
        For ii As Integer = 0 To drowPcode.Length - 1                 ' �擾������ LOOP����
            ' �ŏI�s���ǂ���
            If ii < drowPcode.Length - 1 Then
                ' �ŏI�s�łȂ����dgvData�ɏo��
                dgvData(EM_GRID_ID.emTxt_Code, ii).Value = drowPcode(ii).Item("pcode")                      ' �����p�^�[���R�[�h 1�`64
                ' �L������`�F�b�N�{�b�N�X
                If CInt(drowPcode(ii).Item("�L������")) = 0 Then
                    dgvData(EM_GRID_ID.emChk_Check, ii).Value = False                                       ' 0�Ȃ�`�F�b�NOFF
                Else
                    dgvData(EM_GRID_ID.emChk_Check, ii).Value = True                                        ' 0�ȊO�̓`�F�b�NON
                End If
                dgvData(EM_GRID_ID.emTxt_Priority, ii).Value = drowPcode(ii).Item("�D��x")                 ' �D��x
                dgvData(EM_GRID_ID.emTxt_Name, ii).Value = drowPcode(ii).Item("�����p�^�[������")           ' �����\�p�^�[������

            Else
                ' �ŏI�s�͂��̑�dgv�ɏo��
                dgvEtc(EM_GRID_ID.emTxt_Code, 0).Value = drowPcode(ii).Item("pcode")                        ' �����p�^�[���R�[�h 64
                dgvEtc(EM_GRID_ID.emTxt_Priority, 0).Value = drowPcode(ii).Item("�D��x")                  ' �D��x
                dgvEtc(EM_GRID_ID.emTxt_Name, 0).Value = drowPcode(ii).Item("�����p�^�[������")            ' �����\�p�^�[������

            End If
        Next ii

        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪 �i�e�L�X�g�{�b�N�X)
        dgvData(EM_GRID_ID.emTxt_Name, 0).Selected = True
        Return True
    End Function

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���͒l�`�F�b�N
        If Not Data_Check() Then Exit Sub

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' �\���f�[�^�擾
            Data_Get()

            ' �z���g�ɓo�^        
            If DB_UpDate() Then
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String

        ' �����p�^�[���R�[�h�����[�v
        For ii As Integer = 0 To dgvData.RowCount - 1
            ' �����p�^�[������
            strWk = dgvData(EM_GRID_ID.emTxt_Name, ii).Value.ToString
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, _
                "�����p�^�[���R�[�h=" & dgvData(EM_GRID_ID.emTxt_Code, ii).Value.ToString & " �����p�^�[������") <> 0 Then Return False
            dgvData(EM_GRID_ID.emTxt_Name, ii).Value = strWk

            ' �L������擾
            Dim bCheck As Boolean = CBool(dgvData(EM_GRID_ID.emChk_Check, ii).Value)

            ' �D��x�͈̔͊m�F
            strWk = dgvData(EM_GRID_ID.emTxt_Priority, ii).Value.ToString
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, Not bCheck, "�����p�^�[���R�[�h=" & dgvData(EM_GRID_ID.emTxt_Code, ii).Value.ToString & " �D��x") <> 0 Then
                ' �L�����肪�L���̏ꍇ��NULL�֎~�A�����̏ꍇ��NULL����
                Return False
            End If
            dgvData(EM_GRID_ID.emTxt_Priority, ii).Value = strWk

            ' �D��x�̏d���`�F�b�N
            If bCheck = True Then
                ' �L���ȍ��ڂ��m�F����
                For jj As Integer = ii + 1 To dgvData.Rows.Count - 1
                    If Trim(CStr(dgvData(EM_GRID_ID.emTxt_Code, jj).Value)) = "" Then
                        Continue For                                                ' PCode����Ȃ玟��
                    End If
                    If Not CBool(dgvData(EM_GRID_ID.emChk_Check, jj).Value) Then
                        Continue For                                                ' �����Ȃ玟��
                    End If
                    If IsNumeric(dgvData(EM_GRID_ID.emTxt_Priority, jj).Value) Then
                        If CInt(dgvData(EM_GRID_ID.emTxt_Priority, ii).Value) = CInt(dgvData(EM_GRID_ID.emTxt_Priority, jj).Value) Then
                            Dim strMsg As String = ""
                            strMsg = "�y �����p�^�[���}�X�^�[�o�^ �z" & vbNewLine
                            strMsg &= "�����D��x�͓o�^�ł��܂���B" & vbNewLine
                            strMsg &= "- - - �����p�^�[���R�[�h�u" & CStr(dgvData(EM_GRID_ID.emTxt_Code, ii).Value) & "�v �� �u" & CStr(dgvData(EM_GRID_ID.emTxt_Code, jj).Value) & "�v"
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj
            End If

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim strWhere As String              ' WHERE��ɑ����̏�������
        Dim drow As DataRow                 ' �f�[�^���R�[�h 1�s
        Dim nCheck As Integer               ' �L������

        ' �����p�^�[���R�[�h�����[�v ���̑��ȊO�̃f�[�^���i�[
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            strWhere = String.Format("pcode='{0}'", dgvData(EM_GRID_ID.emTxt_Code, ii).Value.ToString)  ' WHERE��ɑ����̏�������
            drow = m_DS.Tables(0).Select(strWhere)(0)                                                   ' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

            ' �L�������`�F�b�N�{�b�N�X�̃`�F�b�N��Ԃɂ��A�L��������Z�b�g
            If CBool(dgvData(EM_GRID_ID.emChk_Check, ii).Value) Then
                nCheck = 1                                          ' �L��
            Else
                nCheck = 0                                          ' ����
            End If

            ' �擾�����l���Z�b�g
            drow.Item("�L������") = nCheck                          ' �L������

            ' �D��x
            If Nothing Is dgvData(EM_GRID_ID.emTxt_Priority, ii).Value OrElse _
            "" = dgvData(EM_GRID_ID.emTxt_Priority, ii).Value.ToString.Trim Then
                drow.Item("�D��x") = System.DBNull.Value
            Else
                drow.Item("�D��x") = CInt(dgvData(EM_GRID_ID.emTxt_Priority, ii).Value)
            End If

            drow.Item("�����p�^�[������") = dgvData(EM_GRID_ID.emTxt_Name, ii).Value.ToString  '�����p�^�[������

        Next ii
        For ii As Integer = 0 To dgvEtc.Rows.Count - 1
            strWhere = String.Format("pcode='{0}'", dgvEtc(EM_GRID_ID.emTxt_Code, ii).Value.ToString)  ' WHERE��ɑ����̏�������
            drow = m_DS.Tables(0).Select(strWhere)(0)                                                   ' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

        Next ii
    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������

        Try
            ' DB�ڑ�
            m_Adapter.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()

                '�R�}���h�A�_�v�^���Z�b�g���āA�X�V
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            '�R�~�b�g
            transact.Commit()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)

            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            If Not transact Is Nothing Then
                transact.Rollback()
            End If

            Return False
        Finally
            ' ��n��
            If m_Adapter.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter.SelectCommand.Connection.Close()
            End If

            If Not transact Is Nothing Then
                transact.Dispose()
            End If
        End Try
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal bDisptype As Boolean)
		Dim bLineVisible As Boolean = True									' ���C���ʕ\����\������(true:�\��,false:��\��)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                      ' �w�b�_�[�̍���

        End With

        '' ���`

        ' ����No
        dgvColSetText(dgv, "�����p�^�[��" & vbNewLine & "�R�[�h", 130, False, True,
           DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        If bDisptype = True Then
            ' dgvData���i�v���p�e�B�Z�b�g
            ' �L������
            dgvColSetCheck(dgv, "�L������" & vbNewLine & "(�����F�L��)", 120, False, False,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        Else
            ' dgvEtc���i�v���p�e�B�Z�b�g
            ' �L������
            dgvColSetText(dgv, "", 120, False, True,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
        End If

		If bDisptype = True Then
            ' dgvData���i�v���p�e�B�Z�b�g
            ' �D��x
            dgvColSetText(dgv, "�D��x" & vbNewLine & "��1", 80, False, False,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, "")
        Else
            ' dgvEtc���i�v���p�e�B�Z�b�g
            ' �D��x
            dgvColSetText(dgv, "�D��x" & vbNewLine & "��1", 80, False, True,
             DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, "")
        End If

		If bDisptype = True Then
            ' �����p�^�[������
            dgvColSetText(dgv, "�����p�^�[������" & vbNewLine & "�i���p�R�Q�����j", 300, False, False,
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, Nothing, "")
        Else
            ' �����p�^�[������
            dgvColSetText(dgv, "�����p�^�[������" & vbNewLine & "�i���p�R�Q�����j", 300, False, True,
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, Nothing, "")
        End If

        If bDisptype = False Then
            ' dgvEtc�̌ʐݒ� ���ɓK���ȂƂ��낪�����̂ł����֋L��
            dgv.ColumnHeadersVisible = False                           ' �w�b�_�[��\��
            dgv.Enabled = True                                         ' dgv�g�p�s��
            dgv.RowCount = 1                                           ' �K��1�s
            dgv.Rows(0).DefaultCellStyle.BackColor = g_ColorSetting    ' �w�i�F���K��F�ɂ���
        End If

	End Sub

    ''' <summary>
    ''' �L��������̃`�F�b�N��Ԃ��ς�����Ƃ��ɁA�w�i�F��ύX����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        '�w�b�_�͏������Ȃ�
        If e.RowIndex = -1 Then Exit Sub

        ' �L��������
        If e.ColumnIndex = EM_GRID_ID.emChk_Check Then
            If CBool(dgvData(e.ColumnIndex, e.RowIndex).Value) Then
                '�`�F�b�N��t������ �w�i�F�ƑI��F���K��F�ɂ���
                dgvData.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSetting
                dgvData.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = g_ColorSetting
            Else
                '�`�F�b�N���O������ �w�i�F�ƑI��F�����f�t�H���g�ɖ߂�
                dgvData.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgvData.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

End Class
