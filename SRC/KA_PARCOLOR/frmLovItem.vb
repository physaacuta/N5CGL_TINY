'*********************************************************************************
' ����������ʑI�����ڃA�C�e���}�X�^�[��ʃN���X
'	[Ver]
'		Ver.01    2015/07/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmLovItem
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �萔
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Enum EM_LOV_ITEM
		emNo = 0							' No
		emLov_id							' lov_id
		emSeq								' seq
		emDisp_member						' �\�����ް
		emVal_member_i						' �l���ް(���l)
		emVal_member_c						' �l���ް(����)
		emVal_member_b						' �l���ް(bit)
		emInsButton							' �}���{�^��
		emDelButton							' �폜�{�^��
	End Enum

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_AdapterGrid As New SqlClient.SqlDataAdapter		 ' �A�_�v�^�[ �f�[�^�O���b�h�\���ݒ�
	Private m_Grid As New DataSet								 ' ��ڑ��^�e�[�u�� �f�[�^�O���b�h�\���ݒ�
	Private m_AdapterLov_Item As New SqlClient.SqlDataAdapter	' �A�_�v�^�[ PAR_LOV_ITEM
	Private m_Lov_Item As New DataSet							' ��ڑ��^�e�[�u�� PAR_LOV_ITEM

	Private m_nSendScode As Integer								' ���[���ő��M����Scode
	Private m_nDelLov_id(10000 - 1) As Integer					' �폜�Ώۂ�Lov_id
	Private m_nDelLov_id_Count As Integer						' �폜�Ώی���
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
	Private Sub frmTypeGr_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' �t�@���N�V�����{�^���L�����ڂ̂�
		If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�I�����ڃA�C�e���}�X�^�[���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
	Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
		' ���낢��J������
		m_Grid.Dispose()
		If Not m_AdapterGrid Is Nothing Then
			m_AdapterGrid.Dispose()
			m_AdapterGrid = Nothing
		End If

		m_Lov_Item.Dispose()
		If Not m_AdapterLov_Item Is Nothing Then
			m_AdapterLov_Item.Dispose()
			m_AdapterLov_Item = Nothing
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
	Private Sub frmTypeGr_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                 ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                             ' ���j���[���x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmLovItem			' ��ʌďo

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		'dgv������
		dgvInitializeDefType(dgvLov_Item)

		' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
		If Not DB_Select() Then
			' DB ���s�Ȃ̂ŁA��ʋ����I��
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "�ُ�"
			frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()						'�I��
		End If

		' �f�[�^�̃��R�[�h����������
		Data_Init()

		' ��ʂɒl��\��
		Data_Disp()

		' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
		dgvLov_Item.DataGridViewSelectionRowColorChange(g_ColorSelection)	  ' �r��dgv

		' �X�V�{�^���̗L��/����
		Call UpdateEnabled(g_bPassFlg)

		frmMsg.Dispose()
		frmMsg = Nothing

	End Sub
	''' <summary>
	''' �f�B�X�|�[�Y
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmTypeGr_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

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
		strSQL = "SELECT ���ID"
		strSQL &= ", �O���b�hID"
		strSQL &= ", ����No"
		strSQL &= ", ���ڃw�b�_�[1"
		strSQL &= ", ���ڃw�b�_�[2"
		strSQL &= ", ���l"
		strSQL &= ", ���ڃZ���^�C�v"
		strSQL &= ", lov_id"
		strSQL &= ", �\��_���ڕ\���L��"
		strSQL &= ", �\��_�ǂݎ���p"
		strSQL &= ", �\��_���ڕ�"
		strSQL &= ", �\��_���ڔz�u"
		strSQL &= ", �\��_�t�H�[�}�b�g"
		strSQL &= ", �\��_��Œ�"
		strSQL &= ", DB_�e�[�u����"
		strSQL &= ", DB_�t�B�[���h��"
		strSQL &= ", DB_��L�["
		strSQL &= ", DB_�t�B�[���h�^�C�v"
		strSQL &= ", ����_���͉\�ŏ��l"
		strSQL &= ", ����_���͉\�ő�l"
		strSQL &= ", ����_NULL���͉�"
		strSQL &= ", ����_���͉\����"
		strSQL &= ", ����_�����_�ȉ�����"
		strSQL &= ", ����_�\���_�C�A���O"
		strSQL &= " FROM " & DB_PAR_GRID_SETTING

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_Grid, m_AdapterGrid, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PAR_GRID_SETTING & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		' SQL������
		strSQL = "SELECT lov_id"
		strSQL &= ", seq"
		strSQL &= ", disp_member"
		strSQL &= ", val_member_i"
		strSQL &= ", val_member_c"
		strSQL &= ", val_member_b"
		strSQL &= " FROM " & DB_PAR_LOV_ITEM
		strSQL &= " ORDER BY lov_id, seq ASC"

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_Lov_Item, m_AdapterLov_Item, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PAR_LOV_ITEM & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		Return True

	End Function

    ''' <summary>
    ''' �e�[�u���X�V���s��
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function DB_Delete() As Boolean
        Dim transactGrid As SqlClient.SqlTransaction = Nothing  '�g�����U�N�V��������
        Dim transactLov As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������
        Dim strSql As String                                    'Sql��

        Try
            ' Delete�p SqlCommand�쐬
            strSql = "DELETE FROM " & DB_PAR_LOV_ITEM
            m_AdapterLov_Item.DeleteCommand = New SqlClient.SqlCommand
            m_AdapterLov_Item.DeleteCommand.CommandText = strSql
            m_AdapterLov_Item.DeleteCommand.Connection = m_AdapterLov_Item.SelectCommand.Connection

            ' DB�ڑ�
            m_AdapterGrid.SelectCommand.Connection.Open()
            m_AdapterLov_Item.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transactGrid = m_AdapterGrid.SelectCommand.Connection.BeginTransaction()
            m_AdapterGrid.SelectCommand.Transaction = transactGrid

            transactLov = m_AdapterLov_Item.SelectCommand.Connection.BeginTransaction()
            m_AdapterLov_Item.SelectCommand.Transaction = transactLov
            m_AdapterLov_Item.DeleteCommand.Transaction = transactLov

            ' �I�����ڃA�C�e���}�X�^�[�̑S���ڂ��폜
            If 0 > m_AdapterLov_Item.DeleteCommand.ExecuteNonQuery() Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                transactLov.Rollback()
                Return False
            End If

            ' �I�����ڃA�C�e���}�X�^�[�̓o�^
            Using cbWk As New SqlClient.SqlCommandBuilder()

                '�R�}���h�A�_�v�^���Z�b�g���āADB���X�V
                cbWk.DataAdapter = m_AdapterGrid
                m_AdapterGrid.Update(m_Grid)

                cbWk.DataAdapter = m_AdapterLov_Item
                m_AdapterLov_Item.Update(m_Lov_Item)

            End Using

            '�R�~�b�g
            transactGrid.Commit()
            transactLov.Commit()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �I�����ڃA�C�e���}�X�^�[�o�^���s [" & ex.Message & "] " & Me.Text)

            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            If Not transactGrid Is Nothing Then
                transactGrid.Rollback()
            End If

            If Not transactLov Is Nothing Then
                transactLov.Rollback()
            End If

            Return False
        Finally

            ' ��n��
            If m_AdapterGrid.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterGrid.SelectCommand.Connection.Close()
            End If

            If m_AdapterLov_Item.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterLov_Item.SelectCommand.Connection.Close()
            End If

            If Not transactGrid Is Nothing Then
                transactGrid.Dispose()
            End If

            If Not transactLov Is Nothing Then
                transactLov.Dispose()
            End If
        End Try
    End Function
    ''' <summary>
    ''' �����o�ϐ��̏����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()

		' �폜�Ώۂ��r��EdasysID�z��̏�����
		For ii As Integer = 0 To m_nDelLov_id.Length - 1
			m_nDelLov_id(ii) = 0
		Next ii

	End Sub

	''' <summary>
	''' �f�[�^�s��}��
	''' </summary>
	''' <param name="nRowIndex">�s�̑}���ʒu</param>
	''' <remarks></remarks>
	Private Sub InsertAt_Lov_Item(ByVal nRowIndex As Integer)
		' dgvLov_Item�Ƀ��R�[�h��}��
		dgvLov_Item.Rows.Insert(nRowIndex)

		' �f�[�^������
		dgvLov_Item(EM_LOV_ITEM.emNo, nRowIndex).Value = ""								' No
		dgvLov_Item(EM_LOV_ITEM.emLov_id, nRowIndex).Value = ""							' lov_id
		dgvLov_Item(EM_LOV_ITEM.emSeq, nRowIndex).Value = ""							' seq
		dgvLov_Item(EM_LOV_ITEM.emDisp_member, nRowIndex).Value = ""					' �\�����ް
		dgvLov_Item(EM_LOV_ITEM.emVal_member_i, nRowIndex).Value = ""					' �l���ް(���l)
		dgvLov_Item(EM_LOV_ITEM.emVal_member_c, nRowIndex).Value = ""					' �l���ް(����)
		dgvLov_Item(EM_LOV_ITEM.emVal_member_b, nRowIndex).Value = False				' �l���ް(bit)

	End Sub

	''' <summary>
	''' DB�̏�����ʂɕ\��
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Disp()
		'�r��̃f�[�^��\��
		Data_DispdgvLov_Item(dgvLov_Item)
		'�r��̃C���f�b�N�X�ƃ{�^���̕\���ݒ�
		InitIndexdgvLov_Item(dgvLov_Item)

		' �s�̐F��ς��� �i�����\���ŐF���ς��΍�j
		dgvLov_Item.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

	End Sub

	''' <summary>
	''' �r��dgv�Ƀf�[�^��\��
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_DispdgvLov_Item(ByVal dgv As DataGridView)
		Dim drowLov_Item() As DataRow = m_Lov_Item.Tables(0).Select()			' �S���R�[�h��\�����邽��WHERE��Ȃ�
		'�s��������
		dgv.Rows.Clear()
		' ��ʂɕ\��
		dgv.RowCount = drowLov_Item.Length			  ' 
		For ii As Integer = 0 To dgv.RowCount - 1
			'No
			dgv(EM_LOV_ITEM.emNo, ii).Value = ii + 1
			'lov_id
			dgv(EM_LOV_ITEM.emLov_id, ii).Value = drowLov_Item(ii).Item("lov_id")
			'Seq
			dgv(EM_LOV_ITEM.emSeq, ii).Value = drowLov_Item(ii).Item("seq")
			'�\�����ް
			dgv(EM_LOV_ITEM.emDisp_member, ii).Value = drowLov_Item(ii).Item("disp_member").ToString.Trim()
			'�l���ް(���l)
			dgv(EM_LOV_ITEM.emVal_member_i, ii).Value = drowLov_Item(ii).Item("val_member_i").ToString.Trim()
			'�l���ް(����)
			If System.DBNull.Value Is drowLov_Item(ii).Item("val_member_c").ToString Then
				dgv(EM_LOV_ITEM.emVal_member_c, ii).Value = ""
			Else
				dgv(EM_LOV_ITEM.emVal_member_c, ii).Value = drowLov_Item(ii).Item("val_member_c").ToString.Trim()
			End If
			'�l���ް(bit)
			dgv(EM_LOV_ITEM.emVal_member_b, ii).Value = drowLov_Item(ii).Item("val_member_b").ToString.Trim()
		Next ii
        ''�󔒍s��ǉ�
        'InsertAt_Lov_Item(dgv.RowCount)
        ''No��U��Ȃ����A�{�^����\�����Ȃ���
        'InitIndexdgvLov_Item(dgv)

        If 1 > dgvLov_Item.RowCount Then ' �f�[�^���Ȃ��ꍇ
            ' �󔒍s��ǉ�
            InsertAt_Lov_Item(dgv.RowCount)
        End If

        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
        dgv(1, 0).Selected = True
	End Sub

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

			'��ʂ���폜���ꂽEdasysID���擾
			Data_CheckDelete()

			' ��ʂ̃f�[�^���擾
			Data_Get()

            ' �z���g�ɓo�^        
            If DB_UpDate() Then
                modMain.HardCopy(Me)

                frmMsg.Text = "�m�菈��"
                frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "�m�菈��"
				frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
				frmMsg.ShowDialog()
			End If

			' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
			If Not DB_Select() Then
				' DB ���s�Ȃ̂ŁA��ʋ����I��
				frmMsg.Text = "�ُ�"
				frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Black.ToArgb, True, False)
				frmMsg.ShowDialog()
				modMain.TaskEnd()						'�I��
			End If

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

		' �ݒ�̏�����
		Data_Init()

		' ��ʂɒl��\��
		Data_Disp()
	End Sub

	''' <summary>
	''' ���͒l�̃`�F�b�N
	''' </summary>
	''' <returns>���A���</returns>
	''' <remarks></remarks>
	Private Function Data_Check() As Boolean
		Dim strWk As String		' dgv�̃f�[�^�i�[�p
		Dim strMsg As String	' �d���p�̃G���[���b�Z�[�W
		Dim bVal_member_i As Boolean	' �l���ް(���l) �L������ true:�L��
		Dim bVal_member_c As Boolean	' �l���ް(����) �L������ true:�L��
		Dim bVal_member_b As Boolean	' �l���ް(bit)  �L������ true:�L��
		' dgv�s�������[�v
		For ii As Integer = 0 To dgvLov_Item.Rows.Count - 1
			'lov_id
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99999, True, "No=" & ii + 1 & " lov_id") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value = strWk
			' lov_id���󔒂̍s�͖���
			If Trim(strWk) = "" Then Continue For

			'seq
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999, False, "No=" & ii + 1 & " Seq") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value = strWk

			'�\�����ް
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emDisp_member, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "No=" & ii + 1 & " �\�����ް") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emDisp_member, ii).Value = strWk

			'�l���ް(���l)
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, True, "No=" & ii + 1 & " �l���ް(���l)") <> 0 Then Return False
            dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value = strWk

			'�l���ް(����)
			strWk = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value)
			If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "No=" & ii + 1 & " �l���ް(����)") <> 0 Then Return False
			dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value = strWk

			For jj As Integer = ii + 1 To dgvLov_Item.Rows.Count - 1
				If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, jj).Value) Then
					' lov_id����v���Ă���̂�ΏۂƂ��`�F�b�N���s��
					If IsNumeric(dgvLov_Item(EM_LOV_ITEM.emSeq, jj).Value) Then
						' lov_id��Seq�������Ȃ烁�b�Z�[�W���o���ďI��
						If CInt(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value) = CInt(dgvLov_Item(EM_LOV_ITEM.emSeq, jj).Value) Then
							strMsg = "lov_id�y��Seq���d�����Ă��܂��B" & vbNewLine
							strMsg &= "No =[" & CStr(ii + 1) & "] �� No =[" & CStr(jj + 1) & "]"
							MsgBox(strMsg)
							Return False
						End If
					End If
					'lov_id���œ���̒l���ް���o�^����Ă���ꍇ�A�ُ�Ƃ���
					' �l���ް(���l)
					If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
						If CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, jj).Value) Then
							strMsg = "�l���ް(���l)���d�����Ă��܂��B" & vbNewLine
							strMsg &= "No =[" & CStr(ii + 1) & "] �� No =[" & CStr(jj + 1) & "]"
							MsgBox(strMsg)
							Return False
						End If
					End If
					' �l���ް(����)
					If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
						If CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, jj).Value) Then
							strMsg = "�l���ް(����)���d�����Ă��܂��B" & vbNewLine
							strMsg &= "No =[" & CStr(ii + 1) & "] �� No =[" & CStr(jj + 1) & "]"
							MsgBox(strMsg)
							Return False
						End If
					End If
				End If
			Next jj
			bVal_member_i = False
			bVal_member_c = False
			bVal_member_b = False
			' �l���ް���o�^����Ă��邩���肷��
			' �l���ް(���l)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
				bVal_member_i = True
			End If
			' �l���ް(����)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
				bVal_member_c = True
			End If
			' �l���ް(bit)
			If CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, ii).Value) Then
				bVal_member_b = True
			End If
			' �l���ް�������o�^���Ă���ꍇ�A�ُ�Ƃ���
			If ((bVal_member_i And bVal_member_c) Or (bVal_member_i And bVal_member_b) Or (bVal_member_c And bVal_member_b)) Then
				strMsg = "�l���ް�𕡐��o�^���Ă��܂��B" & vbNewLine
				strMsg &= "No =[" & CStr(ii + 1) & "]"
				MsgBox(strMsg)
				Return False
			End If

		Next ii
		'�l���ް(bit)�`�F�b�N
		If Not ChkVal_member_b() Then
			Return False
		End If
		Return True
	End Function

	''' <summary>
	''' �l���ް(bit)�`�F�b�N
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function ChkVal_member_b() As Boolean
		Dim strMsg As String	' �d���p�̃G���[���b�Z�[�W
		Dim bVal_member_i As Boolean	' �l���ް(���l) �L������ true:�L��
		Dim bVal_member_c As Boolean	' �l���ް(����) �L������ true:�L��
		Dim nCount As Integer			' �J�E���g�l
		' dgv�s�������[�v
		For ii As Integer = 0 To dgvLov_Item.Rows.Count - 1
			If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) Then Continue For
			' �l���ް�������o�^���Ă���ꍇ�ȏ�Ƃ���
			bVal_member_i = False
			bVal_member_c = False
			' �l���ް���o�^����Ă��邩���肷��
			' �l���ް(���l)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
				bVal_member_i = True
			End If
			' �l���ް(����)
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
				bVal_member_c = True
			End If
			nCount = 0
			' �l���ް(���l)����(����)���o�^����Ă��Ȃ��ꍇ�A�l���ް(bit)���L��
			If Not bVal_member_i And Not bVal_member_c Then
				For jj As Integer = 0 To dgvLov_Item.Rows.Count - 1
					If ii = jj Then Continue For '�����̏ꍇ�̓X�L�b�v
					'lov_id����v���A�l���ް(bit)���L���Ȃ��̂�ΏۂƂ��A�l����v���邩�`�F�b�N���s���A��v����ꍇ�A�ُ�Ƃ���
					If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) = CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, jj).Value) Then
						'If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, jj).Value) Then
						'	If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, jj).Value) Then
						If CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, ii).Value) = CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, jj).Value) Then
							strMsg = "�l���ް(bit)���d�����Ă��܂��B" & vbNewLine
							strMsg &= "No =[" & ii + 1 & "]" & " �� No =[" & jj + 1 & "]"
							MsgBox(strMsg)
							Return False
						End If
						nCount = nCount + 1
						'End If
						'	End If
					End If
				Next jj
				' �l���ް(bit)�ɑ΂�true,false�̂Q�p�^�[���݂̂̓o�^�łȂ��ꍇ�A�ُ�Ƃ���
				If nCount <> 1 Then
					strMsg = "�l���ް(bit)�́A" & vbNewLine & "ON�^OFF��2�p�^�[����o�^���Ă��������B" & vbNewLine
					strMsg &= "No =[" & ii + 1 & "] lov_id =[" & CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) & "]"
					MsgBox(strMsg)
					Return False
				End If
			End If
		Next ii
		Return True
	End Function

	''' <summary>
	''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Get()
		Dim nCountNo As Integer	  'No�J�E���g�p

		' �f���[�g�C���T�[�g����ׁA�������Ă���s���N���A����
		m_Lov_Item.Tables(0).Rows.Clear()

		'No������
		nCountNo = 0

		' dgv�̒l���ڑ��e�[�u���֊i�[
		For ii As Integer = 0 To dgvLov_Item.RowCount - 1
			'lov_id��seq ���󔒈ȊO�̍s�����擾
			If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value) = "" Then Continue For
			If CStr(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value) = "" Then Continue For
            Dim drowType As DataRow = m_Lov_Item.Tables(0).NewRow()                                             ' �V�K�s

            drowType.Item("lov_id") = CInt(dgvLov_Item(EM_LOV_ITEM.emLov_id, ii).Value)                         ' lov_id
            drowType.Item("seq") = CInt(dgvLov_Item(EM_LOV_ITEM.emSeq, ii).Value)                               ' seq
            drowType.Item("disp_member") = CStr(dgvLov_Item(EM_LOV_ITEM.emDisp_member, ii).Value)               ' �\�����ް
            If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value) Then
				drowType.Item("val_member_i") = System.DBNull.Value
			Else
                drowType.Item("val_member_i") = CInt(dgvLov_Item(EM_LOV_ITEM.emVal_member_i, ii).Value)         ' �l���ް(���l)
            End If

			If "" = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value) Then
				drowType.Item("val_member_c") = System.DBNull.Value
			Else
                drowType.Item("val_member_c") = CStr(dgvLov_Item(EM_LOV_ITEM.emVal_member_c, ii).Value)         ' �l���ް(����)
            End If

			If CBool(dgvLov_Item(EM_LOV_ITEM.emVal_member_b, ii).Value) Then
				drowType.Item("val_member_b") = 1		  ' �l���ް(bit)
			Else
				drowType.Item("val_member_b") = 0		  ' �l���ް(bit)
			End If

			m_Lov_Item.Tables(0).Rows.Add(drowType)		'�s��ǉ�
		Next ii

	End Sub

	''' <summary>
	''' �A�b�v�f�[�g
	''' </summary>
	''' <returns>���A���(True:����,False:�ُ�)</returns>
	''' <remarks></remarks>
	Private Function DB_UpDate() As Boolean
		Try
			'�l��o�^
			If Not DB_Delete() Then
				Return False
			End If

			Return True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)
			Return False
		End Try
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ DataGridView �֘A
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' dgv�̗��`
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
	''' <remarks></remarks>
	Private Sub dgvInitializeDefType(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)						' �I��F����

		With dgv
			.ColumnHeadersHeight = 42									' �w�b�_�[�̍���
			.AllowUserToResizeColumns = True							' ���[�U�[ ��T�C�Y�ύX
			.ScrollBars = ScrollBars.Both								' �X�N���[���o�[�̕\��

			' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		' �I��F
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		' ��s�����ɑI��F�ύX
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black
		End With

		'' ���`
		' No
		dgvColSetText(dgv, "No", 40, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		' lov_id
		dgvColSetText(dgv, "lov_id", 84, True, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		' seq
		dgvColSetText(dgv, "seq", 42, True, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
        ' �\�����ް
        dgvColSetText(dgv, "�\�����ް", 180, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
        ' �l���ް(���l)
        dgvColSetText(dgv, "�l���ް" & vbNewLine & "(���l)", 84, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		' �l���ް(����)
		dgvColSetText(dgv, "�l���ް" & vbNewLine & "(����)", 84, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		' �l���ް(bit)
		dgvColSetCheck(dgv, "�l���ް" & vbNewLine & "(bit)", 84, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' ��֑}��
		dgvColSetButton(dgv, "���" & vbNewLine & "�}��", 50, False, False, _
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
			New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�}��")

		' �폜
		dgvColSetButton(dgv, "�폜", 50, False, False, _
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
			New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�폜")

	End Sub

	''' <summary>
	''' dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLov_Item_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvLov_Item.CellContentClick
		Dim dgv As DataGridView = CType(sender, DataGridView)

		' �s���N���b�N����Ă��Ȃ���ΏI��
		If e.RowIndex < 0 Then Exit Sub

		' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
		Select Case e.ColumnIndex
			Case EM_LOV_ITEM.emInsButton	 '��֑}���{�^��
				InsBtnClick_Lov_Item(dgv, e.RowIndex)
			Case EM_LOV_ITEM.emDelButton	 '�폜�{�^��
				DelBtnClick_Lov_Item(dgv, e.RowIndex)
		End Select
	End Sub

	''' <summary>
	''' �}�b�v�\���F�{�^���������ꂽ��A�F�I���_�C�A���O��\�����A�{�^���̐F��ύX
	''' </summary>
	''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
	''' <param name="nColIndex">�{�^���̗�C���f�b�N�X</param>
	''' <param name="nRowIndex">�{�^���̍s�C���f�b�N�X</param>
	''' <remarks></remarks>
	Private Sub GetDlgColor(ByVal dgv As DataGridView, ByVal nColIndex As Integer, ByVal nRowIndex As Integer)
		'�F�I���_�C�A���O�̐ݒ�
		'�F�̍쐬���g�p���Ȃ�
		dlgColor.AllowFullOpen = False

		'�F�I���_�C�A���O�̑I��F���A�{�^���̔w�i�F�ɂ���
		dlgColor.Color = dgv(nColIndex, nRowIndex).Style.BackColor

		' �F�I���_�C�A���O��ok�{�^���������ꂽ��A�{�^���̐F��ύX���� 
		If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
			'�I��F���{�^���̔w�i�F��
			dgv(nColIndex, nRowIndex).Style.BackColor = dlgColor.Color
		End If

		dlgColor.Dispose()
	End Sub

	''' <summary>
	''' �}���{�^�������ꂽ�Ƃ��̏���
	''' </summary>
	''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
	''' <param name="nRowIndex">���R�[�h��}������ʒu</param>
	''' <remarks></remarks>
	Private Sub InsBtnClick_Lov_Item(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

		' �}���{�^�����g�p�\���`�F�b�N
		If (dgv.Rows.Count - 1 = nRowIndex) Then
			' �ŏI�s�͎g�p�s�Ȃ̂Ŕ�����
			Exit Sub
		End If

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		frmMsg.SetSelectedFNo(0)

		Try
			'�}�����邩���b�Z�[�W���o��
			frmMsg.Text = "�����m�F"
			frmMsg.ChangeMsg("�}�����܂����H", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

		' dgv�Ƀ��R�[�h��}��
		InsertAt_Lov_Item(nRowIndex)

		'No��U��Ȃ����A�{�^����\�����Ȃ���
		InitIndexdgvLov_Item(dgv)

	End Sub

	''' <summary>
	''' �폜�{�^���������ꂽ�Ƃ��̏���
	''' </summary>
	''' <param name="dgv"></param>
	''' <param name="nRowIndex"></param>
	''' <remarks></remarks>
	Private Sub DelBtnClick_Lov_Item(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		' �{�^���g�p�\���`�F�b�N
		If (dgv.Rows.Count - 1 = nRowIndex) Then
			' �ŏI�s�͎g�p�s�Ȃ̂Ŕ�����
			Exit Sub
		End If

		frmMsg.SetSelectedFNo(0)

		Try

			' �폜���邩���b�Z�[�W���o��
			frmMsg.Text = "�����m�F"
			frmMsg.ChangeMsg("�폜���܂����H", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

		Catch ex As Exception

		Finally
			If Not frmMsg Is Nothing Then
				frmMsg.Dispose()
				frmMsg = Nothing
			End If
		End Try

		' �s���폜
		dgv.Rows.RemoveAt(nRowIndex)

		' No��U��Ȃ����A�{�^����\�����Ȃ���
		InitIndexdgvLov_Item(dgv)

	End Sub

	''' <summary>
	''' dgvLov_Item�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
	''' </summary>
	''' <param name="dgv">�r��dgv</param>
	''' <remarks></remarks>
	Private Sub InitIndexdgvLov_Item(ByVal dgv As DataGridView)
		For ii As Integer = 0 To dgv.Rows.Count - 1
			dgv(EM_LOV_ITEM.emNo, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�

			' �}���{�^���̎g�p�\��Ԃ�ݒ�    (�{�^�����g�p�s�ɂ͂ł��Ȃ��̂ŁA�F��ύX�j
			If ii = dgv.RowCount - 1 Then
				' �ŏI�s�̓{�^�����g�p�s��
				dgv(EM_LOV_ITEM.emInsButton, ii).Style.ForeColor = Color.Gray
				dgv(EM_LOV_ITEM.emDelButton, ii).Style.ForeColor = Color.Gray
			Else
				' ����ȊO�͎g�p��
				dgv(EM_LOV_ITEM.emInsButton, ii).Style.ForeColor = Color.Black
				dgv(EM_LOV_ITEM.emDelButton, ii).Style.ForeColor = Color.Black
			End If
		Next ii
	End Sub

	''' <summary>
	''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim dgv As DataGridView = CType(sender, DataGridView)	' �Ώۂ�dgv
		dgv.CurrentCell.Selected = False
	End Sub

	''' <summary>
	''' �f�[�^�O���b�h�\�����ڕύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvLov_Item_CellValueChanged(ByVal sender As Object, ByVal e As DataGridViewCellEventArgs) Handles dgvLov_Item.CellValueChanged
		Dim nWkx As Integer = 0
		If 0 > e.ColumnIndex Then Exit Sub
		'�ŏI�s��lov_id��ǉ������ꍇ�A���̓��̓G���A��ǉ�����
		If EM_LOV_ITEM.emLov_id = e.ColumnIndex And (dgvLov_Item.Rows.Count - 1) = e.RowIndex Then
			If "" <> CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, e.RowIndex).Value) Then
				'�󔒍s��ǉ�
				InsertAt_Lov_Item(dgvLov_Item.RowCount)
				'No��U��Ȃ����A�{�^����\�����Ȃ���
				InitIndexdgvLov_Item(dgvLov_Item)
			End If
		End If
	End Sub

	''' <summary>
	''' �X�V�{�^���̗L��/����
	''' </summary>
	''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
	''' <remarks></remarks>
	Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

		btnF11.Enabled = bPassResult					' �t�@���N�V�����L�[�̐ݒ�{�^��

	End Sub


	''' <summary>
	''' ��ʂɕ\������lov_id��DB��lov_id���ׂāA�폜���ꂽ���̂����邩�`�F�b�N����
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_CheckDelete()
		Dim bCheck As Boolean	'�`�F�b�N�t���O True�Ȃ�lov_id������
		Dim bDelGrid(10000 - 1) As Boolean		'�폜�ΏۃO���b�h
		Dim nChangeGrid(10000 - 1) As Integer	'�ύX�Ώۂ̃J�E���g��
		Dim strDelGID(10000 - 1) As String		'�폜�Ώۂ̉�ʂh�c
		Dim nDelGridID(10000 - 1) As Integer	'�폜�Ώۂ̃O���b�h�h�c
		Dim nDelPos(10000 - 1) As Integer		'�폜�Ώۂ�Row�ʒu
		Dim nDelKNo(10000 - 1) As Integer		'�폜�Ώۂ̍��ڂm��
		Dim nDelCnt As Integer					'�폜���z�̌���

		m_nDelLov_id_Count = 0
		' �e�[�u���őS�s���[�v
		For ii As Integer = 0 To m_Lov_Item.Tables(0).Rows.Count - 1
			' �`�F�b�N�t���O�̏�����
			bCheck = False
			' �r��dgv�őS�s���[�v
			For kk As Integer = 0 To dgvLov_Item.Rows.Count - 1
				' �󔒍s�ȊO�ŁA��r
				If CStr(dgvLov_Item(EM_LOV_ITEM.emLov_id, kk).Value) <> "" AndAlso _
				 CInt(dgvLov_Item(EM_LOV_ITEM.emLov_id, kk).Value) = CInt(m_Lov_Item.Tables(0).Rows(ii).Item("lov_id")) Then
					' lov_id����v�����Ƃ��A�`�F�b�NOK
					bCheck = True
					Exit For
				End If
			Next kk
			' �`�F�b�N�t���O��False�̂Ƃ������ꂽlov_id�Ȃ̂ŁA�o�^����
			If bCheck = False Then
				m_nDelLov_id(m_nDelLov_id_Count) = CInt(m_Lov_Item.Tables(0).Rows(ii).Item("lov_id"))
				m_nDelLov_id_Count = m_nDelLov_id_Count + 1
			End If
		Next ii
		For ii As Integer = 0 To m_nDelLov_id_Count - 1
			For jj As Integer = ii + 1 To m_nDelLov_id_Count - 1
				If m_nDelLov_id(ii) = m_nDelLov_id(jj) Then
					'lov_id �d�����Ă�����̂̓N���A����
					m_nDelLov_id(jj) = 0
				End If
			Next jj
		Next ii
		For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
			bDelGrid(ii) = False
			nChangeGrid(ii) = 0
			strDelGID(ii) = ""
			nDelGridID(ii) = 0
			nDelPos(ii) = 0
			nDelKNo(ii) = 0
		Next ii
		nDelCnt = 0
		'�폜�ΏۂƂȂ�lov_id�ɕR�t���O���b�h�̏����擾����
		For ii As Integer = 0 To m_nDelLov_id_Count - 1
			If 0 = m_nDelLov_id(ii) Then Continue For
			For jj As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
				If bDelGrid(jj) Then Continue For
				If m_Grid.Tables(0).Rows(jj).Item("lov_id") Is System.DBNull.Value Then Continue For
				If CInt(m_Grid.Tables(0).Rows(jj).Item("lov_id")) = m_nDelLov_id(ii) Then
					bDelGrid(jj) = True
					nDelPos(nDelCnt) = jj
					strDelGID(jj) = CStr(m_Grid.Tables(0).Rows(jj).Item("���ID"))
					nDelGridID(jj) = CInt(m_Grid.Tables(0).Rows(jj).Item("�O���b�hID"))
					nDelKNo(jj) = CInt(m_Grid.Tables(0).Rows(jj).Item("����No"))
					nDelCnt = nDelCnt + 1
				End If
			Next jj
		Next ii
        '�폜�����悤�ƂȂ����O���b�h�ɕR�t�������擾����
        For ii As Integer = 0 To nDelCnt - 1
			For jj As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
				If strDelGID(nDelPos(ii)) = CStr(m_Grid.Tables(0).Rows(jj).Item("���ID")) Then
					If nDelGridID(nDelPos(ii)) = CInt(m_Grid.Tables(0).Rows(jj).Item("�O���b�hID")) Then
						If nDelKNo(nDelPos(ii)) < CInt(m_Grid.Tables(0).Rows(jj).Item("����No")) Then
							'���ڂm�����폜�Ώۂ��傫�����̂��L������
							nChangeGrid(jj) = nChangeGrid(jj) + 1
						End If
					End If
				End If
			Next jj
		Next ii

		'�O���b�h�̍폜�y�сA���̍X�V���s��
		For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
			If nChangeGrid(ii) <> 0 And False = bDelGrid(ii) Then
				Dim drowGrid As DataRow = m_Grid.Tables(0).NewRow()
				''''''''''''''''''''''''''''''''''''''''''''
				Dim drowGridBase As DataRow = m_Grid.Tables(0).Rows(ii)												  ' �V�K�s
				drowGrid.Item("���ID") = drowGridBase.Item("���ID")
				drowGrid.Item("�O���b�hID") = drowGridBase.Item("�O���b�hID")
				drowGrid.Item("���ڃw�b�_�[1") = drowGridBase.Item("���ڃw�b�_�[1")
				drowGrid.Item("���ڃw�b�_�[2") = drowGridBase.Item("���ڃw�b�_�[2")
				drowGrid.Item("���l") = drowGridBase.Item("���l")
				drowGrid.Item("���ڃZ���^�C�v") = drowGridBase.Item("���ڃZ���^�C�v")
				drowGrid.Item("lov_id") = drowGridBase.Item("lov_id")
				drowGrid.Item("�\��_���ڕ\���L��") = drowGridBase.Item("�\��_���ڕ\���L��")
				drowGrid.Item("�\��_�ǂݎ���p") = drowGridBase.Item("�\��_�ǂݎ���p")
				drowGrid.Item("�\��_���ڕ�") = drowGridBase.Item("�\��_���ڕ�")
				drowGrid.Item("�\��_���ڔz�u") = drowGridBase.Item("�\��_���ڔz�u")
				drowGrid.Item("�\��_�t�H�[�}�b�g") = drowGridBase.Item("�\��_�t�H�[�}�b�g")
				drowGrid.Item("�\��_��Œ�") = drowGridBase.Item("�\��_��Œ�")
				drowGrid.Item("DB_�e�[�u����") = drowGridBase.Item("DB_�e�[�u����")
				drowGrid.Item("DB_�t�B�[���h��") = drowGridBase.Item("DB_�t�B�[���h��")
				drowGrid.Item("DB_��L�[") = drowGridBase.Item("DB_��L�[")
				drowGrid.Item("DB_�t�B�[���h�^�C�v") = drowGridBase.Item("DB_�t�B�[���h�^�C�v")
				drowGrid.Item("����_���͉\�ŏ��l") = drowGridBase.Item("����_���͉\�ŏ��l")
				drowGrid.Item("����_���͉\�ő�l") = drowGridBase.Item("����_���͉\�ő�l")
				drowGrid.Item("����_NULL���͉�") = drowGridBase.Item("����_NULL���͉�")
				drowGrid.Item("����_���͉\����") = drowGridBase.Item("����_���͉\����")
				drowGrid.Item("����_�����_�ȉ�����") = drowGridBase.Item("����_�����_�ȉ�����")
				drowGrid.Item("����_�\���_�C�A���O") = drowGridBase.Item("����_�\���_�C�A���O")
				''''''''''''''''''''''''''''''''''''''''''''
				drowGrid.Item("����No") = CInt(drowGridBase.Item("����No")) - nChangeGrid(ii)
				m_Grid.Tables(0).Rows(ii).Delete()
				m_Grid.Tables(0).Rows.Add(drowGrid)		'�s��ǉ�
			End If
			If bDelGrid(ii) Then
				m_Grid.Tables(0).Rows(ii).Delete()
			End If
		Next ii
	End Sub

End Class
