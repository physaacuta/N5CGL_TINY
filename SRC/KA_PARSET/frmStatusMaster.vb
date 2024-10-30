'*********************************************************************************
' �@��}�X�^�[�ݒ���
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

''' <summary>
''' �@��}�X�^�[�ݒ���
''' </summary>
''' <remarks></remarks>
Public Class frmStatusMaster
	'---------------------------------------------------------------------------
	' �񋓑̒�`
	'---------------------------------------------------------------------------
	''' <summary>
	''' �@��}�X�^�[�ݒ��
	''' </summary>
	''' <remarks></remarks>
	Private Enum EM_COL_DGVMST
        emTxt_ID = 0                                  ' �e�L�X�g�{�b�N�X�@�@��ID
        emTxt_Name                                    ' �e�L�X�g�{�b�N�X�@�@�햼
        emTxt_Inival                                  ' �e�L�X�g�{�b�N�X�@�����l
        emCmb_Eerrscale                               ' �R���{�{�b�N�X�@  �ُ�͈�
        emCmb_Err_Kensa                               ' �R���{�{�b�N�X�@  �����ُ�x
        emCmb_Err_Kiki                                ' �R���{�{�b�N�X�@  �@��ُ�x
        emTxt_Priority                                ' �e�L�X�g�{�b�N�X�@�D��x
		emTxt_Err_Msg								  ' �e�L�X�g�{�b�N�X�@�ُ�MSG
	End Enum

	'---------------------------------------------------------------------------
	' �\���̒�`
	'---------------------------------------------------------------------------
	''' <summary>
	''' �@��}�X�^�[�\���f�[�^
	''' </summary>
	''' <remarks></remarks>
	Private Structure STATUS_DISP_VALUE
		Dim strName As String
		Dim oDBValue As Object
		Dim nForeClr As Color
		Dim nBackClr As Color

		''' <summary>
		''' �f�[�^�Z�b�g
		''' </summary>
		''' <param name="name">�\���f�[�^��</param>
		''' <param name="value">DB��̒l</param>
		''' <param name="fore">�\�������F</param>
		''' <param name="back">�\���w�i�F</param>
		''' <remarks></remarks>
		Public Sub SetValue(ByVal name As String, ByVal value As Object, ByVal fore As Color, ByVal back As Color)
			strName = name
			oDBValue = value
			nForeClr = fore
			nBackClr = back
		End Sub
	End Structure

	'---------------------------------------------------------------------------
	' �萔��`
	'---------------------------------------------------------------------------
    Private m_stInitVal() As STATUS_DISP_VALUE                     ' �����l
    Private m_stErrScale() As STATUS_DISP_VALUE                    ' �ُ�͈�
    Private m_stKensaErr() As STATUS_DISP_VALUE                    ' �����ُ�x
    Private m_stKikiErr() As STATUS_DISP_VALUE                     ' �@��ُ�x

	'---------------------------------------------------------------------------
	' �����o�ϐ�
    '---------------------------------------------------------------------------
    Private m_Adapter As New SqlClient.SqlDataAdapter              ' �A�_�v�^�[Type
    Private m_DataSet As New DataSet                               ' ��ڑ��^�e�[�u��TYPE_NAME

	'---------------------------------------------------------------------------
	' ���\�b�h
	'---------------------------------------------------------------------------

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
	Private Sub frmStatusMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' �t�@���N�V�����{�^���L�����ڂ̂�
		If Not enable Then Exit Sub

		'' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�@��}�X�^�[�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
	Private Sub frmStatusMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
		' ���낢��J������
		m_DataSet.Dispose()
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
	Private Sub frmStatusMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmStatusMaster          ' �@��}�X�^�[�ݒ���

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        Data_Init()                                                             ' �\���f�[�^������
		dgvInitialize(dgvData)													' �r��dgv������

        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       ' �I��
        End If

        Data_Disp()                                                             ' ��ʂɒl��\��

        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)           ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�

        Call UpdateEnabled(g_bPassFlg)                                          ' �X�V�{�^���̗L��/����

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
	''' �\���f�[�^�̏�����
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Init()
		'-----------------------------------------------------------------------
		' �����l�̕\�����쐬
		'-----------------------------------------------------------------------
		ReDim m_stInitVal(3 - 1)
		m_stInitVal(0).SetValue("����", 0, Color.Empty, Color.Empty)
		m_stInitVal(1).SetValue("�ُ�", 1, Color.Empty, Color.Empty)
		m_stInitVal(2).SetValue("�ύX����", DBNull.Value, Color.Empty, Color.Empty)

		'-----------------------------------------------------------------------
		' �ُ�͈͂̕\�����쐬
		'-----------------------------------------------------------------------
		ReDim m_stErrScale(4 - 1)
		m_stErrScale(0).SetValue("RAS��ʂ̂ݕ\��", -1, Color.Empty, Color.Empty)
		m_stErrScale(1).SetValue("�S��", 0, Color.Empty, Color.Empty)
		m_stErrScale(2).SetValue("�\��", 1, Color.Empty, Color.Empty)
		m_stErrScale(3).SetValue("����", 2, Color.Empty, Color.Empty)

		'-----------------------------------------------------------------------
		' �����ُ�x�̕\�����쐬
		'-----------------------------------------------------------------------
		ReDim m_stKensaErr(6 - 1)
		m_stKensaErr(0).SetValue("����", 0, Color.Yellow, g_ColorKensaOk)
		m_stKensaErr(1).SetValue("�k��", 1, Color.Yellow, g_ColorKensaNg)
		m_stKensaErr(2).SetValue("�G�b�W�s��", 4, Color.Yellow, g_ColorKensaEdgeNg)
		m_stKensaErr(3).SetValue("���ۑ�", 5, Color.Yellow, g_ColorKensaDbNon)
		m_stKensaErr(4).SetValue("������", 6, Color.Yellow, g_ColorKensaSkip)
        m_stKensaErr(5).SetValue("��~", 9, Color.White, g_ColorKensaStop)

        '-----------------------------------------------------------------------
        ' �@��ُ�x�̕\�����쐬
        '-----------------------------------------------------------------------
        ReDim m_stKikiErr(3 - 1)
        m_stKikiErr(0).SetValue("����", 0, Color.Yellow, g_ColorKikiOk)
		m_stKikiErr(1).SetValue("�y�̏�", 1, Color.Black, g_ColorKikiKei)
		m_stKikiErr(2).SetValue("�d�̏�", 3, Color.Black, g_ColorKikiJyu)

    End Sub

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        Dim rows() As DataRow = m_DataSet.Tables(0).Select()
        dgvData.RowCount = rows.Length
        For ii As Integer = 0 To rows.Length - 1
            Dim row As DataRow = rows(ii)
            '-------------------------------------------------------------------
            ' �@��ID�Ƌ@�햼
            '-------------------------------------------------------------------
            dgvData(EM_COL_DGVMST.emTxt_ID, ii).Value = row.Item("�@��ID")
            dgvData(EM_COL_DGVMST.emTxt_Name, ii).Value = row.Item("�@�햼")

            '-------------------------------------------------------------------
            ' �����l
            '-------------------------------------------------------------------
            If row.Item("�����l") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value = row.Item("�����l")
            End If

            '-------------------------------------------------------------------
            ' �ُ�͈�
            '-------------------------------------------------------------------
            For Each dsp As STATUS_DISP_VALUE In m_stErrScale
                If dsp.oDBValue.Equals(row.Item("�ُ�͈�")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Eerrscale, ii).Value = dsp.strName
                End If
            Next dsp

            '-------------------------------------------------------------------
            ' �����ُ�x
            '-------------------------------------------------------------------
			For Each dsp As STATUS_DISP_VALUE In m_stKensaErr
				If dsp.oDBValue.Equals(row.Item("�����ُ�x")) = True Then
					dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, ii).Value = dsp.strName
				End If
			Next dsp


            '-------------------------------------------------------------------
            ' �@��ُ�x
            '-------------------------------------------------------------------
            For Each dsp As STATUS_DISP_VALUE In m_stKikiErr
                If dsp.oDBValue.Equals(row.Item("�@��ُ�x")) = True Then
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, ii).Value = dsp.strName
                End If
            Next dsp

            '-------------------------------------------------------------------
            ' �D��x
            '-------------------------------------------------------------------
            If row.Item("�D��x") Is DBNull.Value = True Then
                dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value = row.Item("�D��x")
            End If

			'-------------------------------------------------------------------
            ' �ُ�MSG
            '-------------------------------------------------------------------
            If row.Item("�ُ�MSG") Is DBNull.Value Then
                dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value = ""
            Else
                dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value = row.Item("�ُ�MSG")
            End If

        Next ii
    End Sub

    ''' <summary>
    ''' �f�[�^�̎擾
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean
        Dim strSQL As String = ""

        '-----------------------------------------------------------------------
        ' �@��}�X�^�[�擾���쐬
        '-----------------------------------------------------------------------
        strSQL = "SELECT �@��ID"
        strSQL &= ", �@�햼"
        strSQL &= ", �����l"
        strSQL &= ", �ُ�͈�"
        strSQL &= ", �����ُ�x"
        strSQL &= ", �@��ُ�x"
        strSQL &= ", �D��x"
		strSQL &= ", �ُ�MSG"
		strSQL &= " FROM " & DB_STATUS_MASTER
        strSQL &= " ORDER BY �@��ID ASC"

        '-----------------------------------------------------------------------
        ' �@��}�X�^�[�e�[�u���擾
        '-----------------------------------------------------------------------
        Dim strErrMsg As String = ""
        If 0 > MyBase.Get_SelectData(strSQL, m_DataSet, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

	''' <summary>
	''' ��ʎ����̔j���C�x���g
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmStatusMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		If g_nDspfrmID = MyBase.tProp_MyFrmID Then
			modMain.TaskEnd()													'���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
		End If
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

            ' ��ʂ̃f�[�^���擾
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

            ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
            If Not DB_Select() Then
                ' DB ���s�Ȃ̂ŁA��ʋ����I��
                frmMsg.SetSelectedFNo(0)

                frmMsg.Text = "�ُ�"
                frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
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

        ' ��ʂɒl��\��
        Data_Disp()

    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim nKikiId As Integer = 0

        ' ���O�\�����������[�v
        For ii As Integer = 0 To dgvData.RowCount - 1
            nKikiId = CInt(dgvData(EM_COL_DGVMST.emTxt_ID, ii).Value)

            Dim strValIni As String = CStr(dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strValIni, -999, 999, True, "�@��ID=" & nKikiId & " �����l") <> 0 Then
                Return False
            End If
            dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value = strValIni

            Dim strVal As String = CStr(dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strVal, 1, 999, True, "�@��ID=" & nKikiId & " �D��x") <> 0 Then
                Return False
            End If
            dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value = strVal

            If Not Nothing Is dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value Then
                Dim strWk As String = CStr(dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "�@��ID=" & nKikiId & " �ُ�MSG") <> 0 Then Return False
                dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value = strWk
            End If
        Next ii

        Return True
    End Function

    ''' <summary>
    ''' ���͏����f�[�^�Z�b�g�ɐݒ肷��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim rows() As DataRow = m_DataSet.Tables(0).Select()
        For ii As Integer = 0 To rows.Length - 1
            Dim row As DataRow = rows(ii)
            Dim strVal As String = ""

            '-----------------------------------------------------------------------------
            ' �����l
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Inival, ii).Value)
            If strVal.Length > 0 Then
                row.Item("�����l") = CInt(strVal)
            Else
                row.Item("�����l") = DBNull.Value
            End If

            '-----------------------------------------------------------------------------
            ' �ُ�͈�
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Eerrscale, ii).Value)
            For Each dsp As STATUS_DISP_VALUE In m_stErrScale
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("�ُ�͈�") = dsp.oDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' �����ُ�x
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, ii).Value)
            For Each dsp As STATUS_DISP_VALUE In m_stKensaErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("�����ُ�x") = dsp.oDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' �@��ُ�x
            '-----------------------------------------------------------------------------
            strVal = CStr(dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, ii).Value)
            For Each dsp As STATUS_DISP_VALUE In m_stKikiErr
                If dsp.strName.CompareTo(strVal) = 0 Then
                    row.Item("�@��ُ�x") = dsp.oDBValue
                    Exit For
                End If
            Next dsp

            '-----------------------------------------------------------------------------
            ' �D��x
            '-----------------------------------------------------------------------------
            If dgvData(EM_COL_DGVMST.emCmb_Eerrscale, ii).Value Is Nothing = True Then
                row.Item("�D��x") = DBNull.Value
            Else
                strVal = CStr(dgvData(EM_COL_DGVMST.emTxt_Priority, ii).Value)
                If strVal.Length > 0 Then
                    row.Item("�D��x") = CInt(strVal)
                Else
                    row.Item("�D��x") = DBNull.Value
                End If
            End If

			'-----------------------------------------------------------------------------
            ' �ُ�MSG
            '-----------------------------------------------------------------------------
            If dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value Is Nothing Then
                row.Item("�ُ�MSG") = DBNull.Value
            Else
                If dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value.ToString = "" Then
                    row.Item("�ُ�MSG") = DBNull.Value
                Else
                    row.Item("�ُ�MSG") = dgvData(EM_COL_DGVMST.emTxt_Err_Msg, ii).Value.ToString
                End If
            End If
		Next ii
    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transact As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������

        Try
            ' DB�ڑ�
            m_Adapter.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transact = m_Adapter.SelectCommand.Connection.BeginTransaction()
            m_Adapter.SelectCommand.Transaction = transact

            Using builder As New SqlClient.SqlCommandBuilder()
                builder.DataAdapter = m_Adapter
                m_Adapter.Update(m_DataSet)                             ' �ύX�����A�b�v�f�[�g����
            End Using

            '�R�~�b�g
            transact.Commit()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_STATUS)                   ' �@����

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
	Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)							' �I��F����

		With dgv
			'-------------------------------------------------------------------
			' ��{�����`
			'-------------------------------------------------------------------
			.ColumnHeadersHeight = 42										' �w�b�_�[�̍���
			.ScrollBars = ScrollBars.Both									' �X�N���[���o�[�̕\��

			' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		'�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		'��s�����ɑI��F�ύX
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			.AllowUserToResizeColumns = True											' ���[�U�[ ��T�C�Y�ύX �\�Ƃ���

        End With

        '-------------------------------------------------------------------
        ' ���`
        '-------------------------------------------------------------------
        ' �@��ID
		dgvColSetText(dgv, "�@��ID", 65, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, "")

        ' �@�햼
		dgvColSetText(dgv, "�@�햼", 280, False, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' �����l
        dgvColSetText(dgv, "�����l" & vbNewLine & "��1", 65, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' �ُ�͈�
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�ُ�͈�"
            clm.Width = 150
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As STATUS_DISP_VALUE In m_stErrScale
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' �����ُ�x
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "����" & vbNewLine & "�ُ�x"
			clm.Width = 120
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As STATUS_DISP_VALUE In m_stKensaErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' �@��ُ�x
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�@��" & vbNewLine & "�ُ�x"
            clm.Width = 86
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            For Each dsp As STATUS_DISP_VALUE In m_stKikiErr
                lstItem.Items.Add(dsp.strName)
            Next dsp
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' �D��x
        dgvColSetText(dgv, "�D��x" & vbNewLine & "��2", 65, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

		' �ُ�MSG
        Using clm As New DataGridViewTextBoxColumn
            clm.HeaderText = "�ُ�MSG"
            clm.Width = 512
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.ReadOnly = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.MaxInputLength = 256
            dgv.Columns.Add(clm)
        End Using

    End Sub

	''' <summary>
	''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
	Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)                      ' �Ώۂ�dgv
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then    ' ��̃^�C�v���R���{�{�b�N�X���H
            SendKeys.Send("%{DOWN}")                                               ' �h���b�v�_�E���\��
        End If
    End Sub

    ''' <summary>
    ''' �Z���̒l���ύX���ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged
        Dim nRow As Integer = e.RowIndex
        Dim nCol As Integer = e.ColumnIndex
        Dim strVal As String = CStr(dgvData(nCol, nRow).Value)

        If nCol = EM_COL_DGVMST.emCmb_Err_Kensa Then                               ' �����ُ�x��̏ꍇ
            For ii As Integer = 0 To m_stKensaErr.Length - 1
                Dim dsp As STATUS_DISP_VALUE = m_stKensaErr(ii)
                If dsp.strName.CompareTo(strVal) = 0 Then
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, nRow).Style.ForeColor = m_stKensaErr(ii).nForeClr
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kensa, nRow).Style.BackColor = m_stKensaErr(ii).nBackClr
                End If
            Next ii
        End If

        If nCol = EM_COL_DGVMST.emCmb_Err_Kiki Then                                ' �@��ُ�x��̏ꍇ
            For ii As Integer = 0 To m_stKikiErr.Length - 1
                Dim dsp As STATUS_DISP_VALUE = m_stKikiErr(ii)
                If dsp.strName.CompareTo(strVal) = 0 Then
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, nRow).Style.ForeColor = m_stKikiErr(ii).nForeClr
                    dgvData(EM_COL_DGVMST.emCmb_Err_Kiki, nRow).Style.BackColor = m_stKikiErr(ii).nBackClr
                End If
            Next ii
        End If
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)
        btnF11.Enabled = bPassResult                                            ' �t�@���N�V�����L�[�̐ݒ�{�^��
    End Sub

End Class
