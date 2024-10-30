
'*********************************************************************************
' �����p�^�[���ŗL�o�͐ݒ��ʃN���X
'	[Ver]
'		Ver.01    2009/07/29  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmPCommonOutPut
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' PCOMMON_OUTPUT�p �f�[�^�O���b�h�r���[�̗�
    Private Enum em_PComOutDtColumn
        iText_Code = 0          ' �e�L�X�g�{�b�N�X �R�[�h
        iText_Name              ' �e�L�X�g�{�b�N�X �����p�^�[������
        iCheck_StopMark         ' �`�F�b�N�{�b�N�X �X�g�b�v�}�[�NFF���M�L��
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterPComOut As New SqlClient.SqlDataAdapter        ' �A�_�v�^�[ PCOMMON_OUTPUT
    Private m_AdapterPCode As New SqlClient.SqlDataAdapter          ' �A�_�v�^�[ PARAM_MASTER(�p�^�[�����̎擾�p)

    Private m_DsPCode As New DataSet                                ' ��ڑ��^�e�[�u�� PARAM_MASTER
    Private m_DsPComOut As New DataSet                              ' ��ڑ��^�e�[�u�� �����p�^�[���ŗL�o�͐ݒ� PCOMMON_OUTPUT

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
    Private Sub frmPCommonOutPut_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�����p�^�[���ŗL�o�͐ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    ''' <param name="cancel">�I��������߂����ꍇ�́ATrue���Z�b�g</param>
    ''' <remarks></remarks>
    Private Sub frmPCommonOutPut_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DsPCode.Dispose()
        If Not m_AdapterPCode Is Nothing Then
            m_AdapterPCode.Dispose()
            m_AdapterPCode = Nothing
        End If

        m_DsPComOut.Dispose()
        If Not m_AdapterPComOut Is Nothing Then
            m_AdapterPComOut.Dispose()
            m_AdapterPComOut = Nothing
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
    Private Sub frmPCommonOutPut_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        modMain.SetListMenu(Me)                                         ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                     ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCommonOutPut             ' �����p�^�[���ŗL�o�͐ݒ�

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        '�i�R���{�{�b�N�X���r��A�O���[�h��ݒ肷���,Grid�ݒ�̑O�Ŏ��{�j
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        '�����p�^�[���ŗL�o�͐ݒ�
        'dgvData������
        dgvInitialize(dgvPComOut)
        dgvColunmDefData(dgvPComOut, True)
        MyBase.DataGridViewSelectionCancel(dgvPComOut)

        'dgvEtc������()
        dgvInitialize(dgvPComOutEtc)
        dgvColunmDefData(dgvPComOutEtc, False)
        MyBase.DataGridViewSelectionCancel(dgvPComOutEtc)

        '// �����p�^�[���ŗL�o�͐ݒ� ��ʂɒl��\��
        If Not Data_PComOutDisp() Then                                       '  ��ʍ��ڕ\������
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
    ''' ���̃t�H�[���ŕK�v�ȃf�[�^���擾���� DB�Ǎ�
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        '// ��������ݓǍ� ��������݂�pcode�ŏ���
        ' SQL������
        strSQL = " SELECT "
        strSQL &= " pcode, "
        strSQL &= " �L������ , "
        strSQL &= " �����p�^�[������ "
        strSQL &= " FROM " & DB_PCODE_MASTER
        strSQL &= " ORDER BY pcode ASC"


        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPCode, m_AdapterPCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        '// �\���A�X�V�p
        '�����p�^�[���ŗL�o�͐ݒ�
        strSQL = " SELECT "
        strSQL &= " pcode, "
        strSQL &= " �X�g�b�v�}�[�NFF���M�L�� "
        strSQL &= " FROM " & DB_PCOMMON_OUTPUT
        strSQL &= " ORDER BY pcode ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPComOut, m_AdapterPComOut, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCOMMON_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\�� �����p�^�[���ŗL�o�͐ݒ�
    ''' </summary>
    ''' <returns>����FTrue �ُ�FFalse</returns>
    ''' <remarks></remarks>
    Private Function Data_PComOutDisp() As Boolean
        Dim strWhere As String              ' WHERE��ɑ����̏�������
        Dim drowPcode() As DataRow = m_DsPComOut.Tables(0).Select()         ' �S���R�[�h��\�����邽��WHERE��Ȃ�
        Dim drow As DataRow                                                 ' �����p�^�[���擾�p �f�[�^���R�[�h 1�s

        '�f�[�^���������p�^�[��MAX�Ɠ����łȂ��ꍇ�ُ͈�Ȃ̂ŋ����I��������
        If MAX_PCODE <> drowPcode.Length Then
            Return False
        End If

        Try

            ' �f�[�^�O���b�h�r���[�̍s���ݒ�
            dgvPComOut.RowCount = drowPcode.Length - 1

            ' ��ʂɕ\��
            For ii As Integer = 0 To drowPcode.Length - 1                         ' �擾������ LOOP����
                ' �ŏI�s���ǂ���
                If ii < drowPcode.Length - 1 Then
                    ' �ŏI�s�o�Ȃ����dgvPComOut�ɏo��
                    dgvPComOut(em_PComOutDtColumn.iText_Code, ii).Value = drowPcode(ii).Item("pcode")


                    ' �X�g�b�v�}�[�NFF���M�L���`�F�b�N�{�b�N�X
                    If CInt(drowPcode(ii).Item("�X�g�b�v�}�[�NFF���M�L��")) = 0 Then
						dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, ii).Value = False								   ' 0�Ȃ�`�F�b�NOFF
					Else
						dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, ii).Value = True									   ' 0�ȊO�̓`�F�b�NON
					End If


                    '�p�^�[�����̐ݒ�
                    strWhere = String.Format("pcode='{0}'", dgvPComOut(em_PComOutDtColumn.iText_Code, ii).Value.ToString)   ' WHERE��ɑ����̏�������

                    drow = m_DsPCode.Tables(0).Select(strWhere)(0)											' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�
					If drow.Item("�����p�^�[������") Is System.DBNull.Value Then
						dgvPComOut(em_PComOutDtColumn.iText_Name, ii).Value = ""										  ' �����p�^�[������
					Else
						dgvPComOut(em_PComOutDtColumn.iText_Name, ii).Value = drow.Item("�����p�^�[������").ToString	  ' �����p�^�[������
					End If


				Else
                    ' �ŏI�s��dgvPComOutEtc�ɏo��
                    dgvPComOutEtc(em_PComOutDtColumn.iText_Code, 0).Value = drowPcode(ii).Item("pcode")                       ' �����p�^�[���R�[�h Z

                    ' �X�g�b�v�}�[�NFF���M�L���`�F�b�N�{�b�N�X
                    If CInt(drowPcode(ii).Item("�X�g�b�v�}�[�NFF���M�L��")) = 0 Then
						dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, 0).Value = False									  ' 0�Ȃ�`�F�b�NOFF
					Else
						dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, 0).Value = True									  ' 0�ȊO�̓`�F�b�NON
					End If

                    '�p�^�[�����̐ݒ�
                    strWhere = String.Format("pcode='{0}'", dgvPComOutEtc(em_PComOutDtColumn.iText_Code, 0).Value.ToString) ' WHERE��ɑ����̏�������
                    drow = m_DsPCode.Tables(0).Select(strWhere)(0)											' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�
					dgvPComOutEtc(em_PComOutDtColumn.iText_Name, 0).Value = drow.Item("�����p�^�[������").ToString			' �����\�p�^�[������

				End If
			Next ii

            ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪 �i�`�F�b�N�{�b�N�X)

            dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, 0).Selected = True
            dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, 0).Selected = True

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�����p�^�[���ŗL�o�͐ݒ� �\���ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        End Try

    End Function


    ''' <summary>
    ''' ���͒l�̃`�F�b�N �����p�^�[���ŗL�o�͐ݒ�
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_CheckPComOut() As Boolean

        '���͉ӏ����X�g�b�v�}�[�NFF���M�L���ׁ݈̂A�`�F�b�N�����͂Ȃ�

        Return True
    End Function


    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������

        Try
            ' DB�ڑ�
            m_AdapterPComOut.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transact = m_AdapterPComOut.SelectCommand.Connection.BeginTransaction()
            m_AdapterPComOut.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()
                '�R�}���h�A�_�v�^���Z�b�g���āA�X�V
                '�����p�^�[���ŗL�o�͐ݒ�
                cbWk.DataAdapter = m_AdapterPComOut
                m_AdapterPComOut.Update(m_DsPComOut)
            End Using

            '�R�~�b�g
            transact.Commit()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_PCOMMON)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �ݒ�G���[ [" & ex.Message & "] " & Me.Text)

            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            If Not transact Is Nothing Then
                transact.Rollback()
            End If

            Return False
        Finally
            ' ��n��
            If m_AdapterPComOut.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterPComOut.SelectCommand.Connection.Close()
            End If

            If Not transact Is Nothing Then
                transact.Dispose()
            End If
        End Try
    End Function



    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g �����p�^�[���ŗL�o�͐ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get_PComOut()
        Dim strWhere As String              ' WHERE��ɑ����̏�������
        Dim drow As DataRow                 ' �f�[�^���R�[�h 1�s
        Dim nCheck As Integer               ' �L������

        Try
            ' �����p�^�[���R�[�h�����[�v ���̑��ȊO�̃f�[�^���i�[
			For ii As Integer = 0 To dgvPComOut.Rows.Count - 1
                strWhere = String.Format("pcode='{0}'", dgvPComOut(em_PComOutDtColumn.iText_Code, ii).Value.ToString)     ' WHERE��ɑ����̏�������

                drow = m_DsPComOut.Tables(0).Select(strWhere)(0)											' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

				' �L�������`�F�b�N�{�b�N�X�̃`�F�b�N��Ԃɂ��Z�b�g
				If CBool(dgvPComOut(em_PComOutDtColumn.iCheck_StopMark, ii).Value) Then
					nCheck = 1											' �L��
				Else
					nCheck = 0											' ����
				End If

				' �擾�����l���Z�b�g
				drow.Item("�X�g�b�v�}�[�NFF���M�L��") = nCheck													 ' �L������

			Next ii

            '�f�t�H���g�X�V ���ۂɂ͂P�������Ȃ�
			For ii As Integer = 0 To dgvPComOutEtc.Rows.Count - 1
                strWhere = String.Format("pcode='{0}'", dgvPComOutEtc(em_PComOutDtColumn.iText_Code, ii).Value.ToString)     ' WHERE��ɑ����̏�������
                drow = m_DsPComOut.Tables(0).Select(strWhere)(0)                                            ' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

                ' �L�������`�F�b�N�{�b�N�X�̃`�F�b�N��Ԃɂ��Z�b�g
                If CBool(dgvPComOutEtc(em_PComOutDtColumn.iCheck_StopMark, ii).Value) Then
					nCheck = 1											' �L��
				Else
					nCheck = 0											' ����
				End If

				' �擾�����l���Z�b�g
				drow.Item("�X�g�b�v�}�[�NFF���M�L��") = nCheck													 ' �L������
			Next


        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�����p�^�[���ŗL�o�͐ݒ� ��ڑ��^�e�[�u�� �f�[�^�Z�b�g�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub


    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()
        ' ���͒l�`�F�b�N
        'If Not Data_CheckPComOut() Then Exit Sub               '�`�F�b�N���ڂ������̂ŃR�����g��

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' �\���f�[�^�擾
            Call Data_Get_PComOut()

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

            ' ��ʍX�V
            ' �f�[�^�̍Ď擾
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.SetSelectedFNo(0)

                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '�I��
            End If

            ' �ĕ\��
            '// �����p�^�[���ŗL�o�͐ݒ� ��ʂɒl��\��
            If Not Data_PComOutDisp() Then                                       '  ��ʍ��ڕ\������
                ' ��ʕ\�����s�Ȃ̂ŁA�����I��
                frmMsg.SetSelectedFNo(0)

                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '�I��
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As DataGridView)

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                                     ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                                  ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                                ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                                  ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                                         ' �Z���G���[�̕\��
            .ShowCellToolTips = False                                       ' �c�[���q���g�̕\��
            .ShowEditingIcon = False                                        ' �ҏW���̃Z���̖���
            .ShowRowErrors = False                                          ' �G���[�s�̖���
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' �Z���I��
            .MultiSelect = False                                            ' �����s�I��
            .ReadOnly = False                                               ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' �Z����������

            .ColumnHeadersVisible = True                                    ' ��w�b�_�\��
            .RowHeadersVisible = False                                      ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' �I���Z���F��`
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With
    End Sub

    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPComOut.SelectionChanged,
                                                                                                      dgvPComOutEtc.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv
        dgv.CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        ' ��̃^�C�v���R���{�{�b�N�X���H
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' �h���b�v�_�E���\��
            SendKeys.Send("%{DOWN}")
        End If
    End Sub

    ''' <summary>
    ''' �Z���̏�Ԃ��ω��������̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPComOut.CurrentCellDirtyStateChanged, dgvPComOutEtc.CurrentCellDirtyStateChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ�R�~�b�g����
        If dgv.IsCurrentCellDirty Then dgv.CommitEdit(DataGridViewDataErrorContexts.Commit)
    End Sub

    ''' <summary>
    ''' �L��������̃`�F�b�N��Ԃ��ς�����Ƃ��ɁA�w�i�F��ύX���� (�����p�^�[���ŗL�o�͐ݒ�)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPComOut.CellValueChanged, dgvPComOutEtc.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        '�w�b�_�͏������Ȃ�
        If e.RowIndex = -1 Then Exit Sub

        ' �L��������
        If e.ColumnIndex = em_PComOutDtColumn.iCheck_StopMark Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                '�`�F�b�N��t������ �w�i�F�ƑI��F���K��F�ɂ���
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSetting
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = g_ColorSetting
            Else
                '�`�F�b�N���O������ �w�i�F�ƑI��F�����f�t�H���g�ɖ߂�
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If
    End Sub


    ''' <summary>
    ''' dgvPComOut�̉��X�N���[�����ύX���ꂽ�Ƃ��� ���̑���dgvPComOutEtc���X�N���[��������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>���ڂ��������Ƃ��p</remarks>
    Private Sub dgvPComOut_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvPComOut.Scroll
        ' �X�N���[���C�x���g�������X�N���[���Ȃ�
        If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
            ' dgvPComOutEtc��dgvPComOut�̃X�N���[���ʒu�����킹��
            dgvPComOutEtc.HorizontalScrollingOffset = dgvPComOut.HorizontalScrollingOffset
        End If
    End Sub


    ''' <summary>
    ''' �����p�^�[���ŗL�o�͐ݒ� ���`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <param name="bDisptype">dgv�̃^�C�v ture = dgvData, false = dgvEtc</param>
    ''' <remarks></remarks>
    Private Sub dgvColunmDefData(ByVal dgv As DataGridView, ByVal bDisptype As Boolean)

        With dgv
            ''=================================
            '' ���`

            ' ����No
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "��������ݺ���"
                clm.Width = 120
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

            ' �����p�^�[������
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "��������ݖ���"
                'clm.Width = 300
                clm.Width = 260
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DefaultCellStyle.DataSourceNullValue = ""
                .Columns.Add(clm)
            End Using

            ' dgvData���i�v���p�e�B�Z�b�g
            ' �į��ϰ�FF���M�L��
            Using clm As New DataGridViewCheckBoxColumn
                clm.HeaderText = "�į��ϰ�FF���M�L��" & vbNewLine & "(�����F�L��)"
                clm.Width = 170
                clm.Frozen = False
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

            If bDisptype = False Then

                ' dgvEtc�̌ʐݒ� ���ɓK���ȂƂ��낪�����̂ł����֋L��
                .ColumnHeadersVisible = False                           ' �w�b�_�[��\��
                .RowCount = 1                                           ' �K��1�s
                '.Rows(0).DefaultCellStyle.BackColor = g_ColorSetting    ' �w�i�F���K��F�ɂ���

            End If

        End With
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
