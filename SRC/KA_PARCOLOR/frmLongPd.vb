'*********************************************************************************
' �d�o�b�����r�ݒ��ʃN���X
'	[Ver]
'		Ver.01    2012/10/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmLongPd
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �萔
	'/ /////////////////////////////////////////////////////////////////////////////////

	' �f�[�^�O���b�h�r���[�́u��v�i���������׌��o�}�X�^�[�ݒ�)
	Private Enum EM_COMMON_GRID_COL_ID
		emTxt_Name = 0               ' ����
		emTxt_Filter_Start           ' �t�B���^�[1�`16�̐ݒ�
	End Enum

	' �f�[�^�O���b�h�r���[�́u�s�v�i���������׌��o�}�X�^�[�ݒ�)
	Private Enum EM_COMMON_GRID_ROW_ID
		emTxt_ExecInterval = 0                        ' ��������s�Ԋu int
		emTxt_InitialExec_OldLimit                    ' ��������s���ёΏ۔͈� int
		emTxt_MaxDefectLen                            ' �yFFT�z�ő��r���� int
		emTxt_FFTFreq                                 ' �yFFT�z�T���v�����O���� int
		emTxt_FFTPeakThr                              ' �yFFT�z�s�[�N���o臒l float 
		emTxt_HoughConvertAccumulatorRasio            ' �y�n�t�ϊ��zFFT�����{�� float
		emTxt_HoughConvertMinLineLengthRasio          ' �y�n�t�ϊ��z�ŏ��������{�� float
		emTxt_HoughConvertMaxLineGap                  ' �y�n�t�ϊ��z���C���M���b�v int
		emTxt_HoughConvertOnlineThr                   ' �y�n�t�ϊ��z�����󔻒f�]�T�� float
		emTxt_HoughConvertTangentThr                  ' �y�n�t�ϊ��z�Tangent�{�� float
		emCmb_ExpandImage                             ' �y�n�t�ϊ��z�摜�A�������L��(0:�u�A���Ȃ��v 1:�u�A������v)
		emTxt_CsvDir                                  ' CSV�o�̓��[�h�o�͐�t�H���_ varchar(max)
		emTxt_CsvSaveDate                             ' CSV�o�̓��[�h�o�͐�t�@�C���ۑ����� int
		emTxt_PeriodicExecInterval                    ' ����������s�b�` int

		ROW_MAX
	End Enum

	' �f�[�^�O���b�h�r���[�́u��v�i�����������r��ݒ�)
	Private Enum EM_OUTPUT_GRID_COL_ID
		emTxt_No = 0
		emTxt_Name                  ' �r��
		emTxt_EdasysId              ' �r��EdasysId (��\��)
		emCmb_Grade_Start           ' �t�B���^�[1�`16�̃O���[�h�ݒ�J�n��
	End Enum


	' �f�[�^�O���b�h�r���[�́u�s�v�i���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�)
	Private Enum EM_FILTER_GRID_ROW_ID
		emTxt_Name = 0              ' �t�B���^�[����
		emCmb_Enable                ' �L���^����
		emTxt_MaskFront             ' �R�C����[�}�X�N��
		emTxt_MaskTail              ' �R�C�����[�}�X�N��
		emTxt_MaskLeft              ' ���G�b�W�}�X�N��
		emTxt_MaskRight             ' �E�G�b�W�}�X�N��
		emTxt_DetectPdMin           ' ���o��������
		emTxt_DetectPdMax           ' ���o�������
		emTxt_DefectNumMin          ' ���o������
		emTxt_DefectNumMax          ' ���o�����
		emTxt_WidthDiv              ' �������o�����P�ʋ�

		ROW_MAX
	End Enum

	' �f�[�^�O���b�h�r���[�́u��v�i���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�)
	Private Enum EM_FILTER_GRID_COL_ID
		emTxt_Name = 0               ' ����
		emTxt_Filter_Start           ' �t�B���^�[1�`16�̐ݒ�
	End Enum

	'MAX_LONGPD_FILTER
	Private Const MX_COMMON_COLMAX As Integer = MAX_LONGPD_FILTER + 1        ' ���������׌��o�}�X�^�[�ݒ� �ő��
	Private Const MX_FILTER_COLMAX As Integer = MAX_LONGPD_FILTER + 1        ' ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ� �ő��
	Private Const MX_OUTPUT_COLMAX As Integer = MAX_LONGPD_FILTER + 2        ' �������r�L���ݒ� �ő��
	Private Const MC_OUTPUT_ROWMAX As Integer = MAX_TYPE                     ' �������r�ݒ�ő�s��

	' �I���{�^���̒萔
	Private Enum EM_SELECT_MODE
		emCancel                         ' �S����
	End Enum

	'���������׌��o�}�X�^�[�ݒ� DGV�p�̍s�^�C�g���ݒ�
	Private m_strCommonRowHead() As String = {
	   "��������s�Ԋu" & vbNewLine & "(0 - ) [�b]",
	   "��������s���ёΏ۔͈�" & vbNewLine & "(0 - ) [��]",
	   "�yFFT�z�ő��r����" & vbNewLine & "[mm]",
	   "�yFFT�z�T���v�����O����" & vbNewLine & "(1 - ) [mm]",
	   "�yFFT�z�s�[�N���o臒l" & vbNewLine & "",
	   "�y�n�t�ϊ��zFFT�����{��" & vbNewLine & "",
	   "�y�n�t�ϊ��z�ŏ��������{��" & vbNewLine & "",
	   "�y�n�t�ϊ��z���C���M���b�v" & vbNewLine & "",
	   "�y�n�t�ϊ��z�����󔻒f�]�T��" & vbNewLine & "",
	   "�y�n�t�ϊ��z�Tangent�{��" & vbNewLine & "",
	   "�y�n�t�ϊ��z�摜�A�������L��" & vbNewLine & "",
	   "CSV�o�̓��[�h�o�͐�t�H���_" & vbNewLine & "",
	   "CSV�o�̓��[�h�o�͐�t�@�C��" & vbNewLine & "�ۑ�����[��]",
	   "����������s�b�`" & vbNewLine & "(0 - ) [mm]"
		}

	'���������׌��o_�����p�^�[���~�t�B���^�[�ݒ� DGV�p�̍s�^�C�g���ݒ�
	Private m_strFilterRowHead() As String = {
	   "�t�B���^�[����" & vbNewLine & "(�S�p32����)",
	   "�L���^����" & vbNewLine & "",
	   "�R�C����[�}�X�N��" & vbNewLine & "(0 - ) [mm]",
	   "�R�C�����[�}�X�N��" & vbNewLine & "(0 - ) [mm]",
	   "���G�b�W�}�X�N��" & vbNewLine & "(0 - ) [mm]",
	   "�E�G�b�W�}�X�N��" & vbNewLine & "(0 - ) [mm]",
	   "���o��������" & vbNewLine & "(-1 - ) [mm]",
	   "���o�������" & vbNewLine & "(-1 - ) [mm]",
	   "���o������" & vbNewLine & "(-1 - ) [��]",
	   "���o�����" & vbNewLine & "(-1 - ) [��]",
	   "�������o�����P�ʋ�" & vbNewLine & "[mm]"
		}

	''�{�^��dgv�p�̒萔�ݒ�{�^���p��dgv���r��dgv�Ɨ�𑵂���
	Private m_strBtnName() As String = {"�S����"}

	' �������菈���L�� �R���{�p
	Private m_strCmbLongPcodeEnable() As String = {"�Ȃ�", "����"}

	' �y�n�t�ϊ��z�摜�A�������L���@�R���{�p
	Private m_strCmbExpandImageValue() As String = {"�A���Ȃ�", "�A������"}

	' �L���^���� �R���{�p
	Private m_strCmbFilterEnableValue() As String = {"����", "�L��"}

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_Adapter_LongPd_Common As SqlClient.SqlDataAdapter = Nothing               ' �A�_�v�^�[
	Private m_Adapter_LongPd_PCode As SqlClient.SqlDataAdapter = Nothing                ' �A�_�v�^�[
	Private m_Adapter_LongPd_PCodeFilter As SqlClient.SqlDataAdapter = Nothing          ' �A�_�v�^�[
	Private m_Adapter_LongPd_PCodeFilterOutput As SqlClient.SqlDataAdapter = Nothing    ' �A�_�v�^�[
	Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing                         ' �A�_�v�^�[
	Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing                        ' �A�_�v�^�[

	Private m_DS_LongPd_Common As New DataSet                                           ' ��ڑ��^�e�[�u��
	Private m_DS_LongPd_PCode As New DataSet                                            ' ��ڑ��^�e�[�u��
	Private m_DS_LongPd_PCodeFilter As New DataSet                                      ' ��ڑ��^�e�[�u��
	Private m_DS_LongPd_PCodeFilterOutput As New DataSet                                ' ��ڑ��^�e�[�u��
	Private m_DsType As New DataSet                                                     ' ��ڑ��`�e�[�u��
	Private m_DsGrade As New DataSet                                                    ' ��ڑ��^�e�[�u��

	Private m_KeyCode As String = ""                                                    ' ��L�[(PCode)

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���j���[�֘A
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�@���N�V�����ؑ�
	''' </summary>
	''' <param name="FncNo">�t�@���N�V����No (0�I���W��)</param>
	''' <param name="enable">�t�@���N�V�����{�^���̗L���������</param>
	''' <param name="onKey">�����ɉ�����Ă���L�[</param>
	''' <remarks></remarks>
	Private Sub frmLongPd_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' �t�@���N�V�����{�^���L�����ڂ̂�
		If Not enable Then Exit Sub

		'' �t�@���N�V�����ؑ�
		Select Case FncNo                       ' ��0�I���W��
			Case 6                              ' F07 ��ʈ��
				Call PrintOut()
			Case 7                              ' F08 ��ʕۑ�
				Call HardCopy(IMG_PATH, "���������׌��o�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
			Case 8
				modMain.ChgProject()            ' F09 KA_PARSET�֐ؑ֏���
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
	Private Sub frmLongPd_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
		' ���낢��J������
		m_DS_LongPd_Common.Dispose()
		m_DS_LongPd_PCode.Dispose()
		m_DS_LongPd_PCodeFilter.Dispose()
		m_DS_LongPd_PCodeFilterOutput.Dispose()
		m_DsType.Dispose()
		m_DsGrade.Dispose()

		If Not m_DS_LongPd_Common Is Nothing Then
			m_DS_LongPd_Common.Dispose()
			m_DS_LongPd_Common = Nothing
		End If

		If Not m_Adapter_LongPd_PCode Is Nothing Then
			m_Adapter_LongPd_PCode.Dispose()
			m_Adapter_LongPd_PCode = Nothing
		End If

		If Not m_Adapter_LongPd_PCodeFilter Is Nothing Then
			m_Adapter_LongPd_PCodeFilter.Dispose()
			m_Adapter_LongPd_PCodeFilter = Nothing
		End If

		If Not m_Adapter_LongPd_PCodeFilterOutput Is Nothing Then
			m_Adapter_LongPd_PCodeFilterOutput.Dispose()
			m_Adapter_LongPd_PCodeFilterOutput = Nothing
		End If

		If Not m_AdapterType Is Nothing Then
			m_AdapterType.Dispose()
			m_AdapterType = Nothing
		End If

		If Not m_AdapterGrade Is Nothing Then
			m_AdapterGrade.Dispose()
			m_AdapterGrade = Nothing
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
	Private Sub frmLongPd_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

		'�^�C�g���ݒ�
		Call SetTitleName(Me)

		modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
		modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
		MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmLongPd                ' ���������׌��o�ݒ��ʌďo

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		' �A�b�v�R�[�h�̃R���{�{�b�N�X
		MyBase.SetComboBox_Pcode(cmbUpCodeItem)
		MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

		' �������菈���R���{			
		cmbEnable.Items.Clear()
		cmbEnable.Items.AddRange(m_strCmbLongPcodeEnable)

		' ���̃t�H�[���ŕK�v�ȃf�[�^���擾���� �R���{�{�b�N�X�̒l���Ƃ��Ă���̂ŁAdgv�ݒ����
		If Not DB_Select() Then
			' DB ���s�Ȃ̂ŁA��ʋ����I��
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "�ُ�"
			frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()                       '�I��
		End If

		' �R���{�{�b�N�X�̕\���f�[�^��������
		Data_Init()

		' �i���ʁj���������׌��o_�}�X�^�[�ݒ�
		dgvInitialize_Common(dgvCommon)

		' �i�r��ݒ�j����������_�r��ݒ�
		dgvInitialize_Output(dgvOutput, True)
		dgvInitialize_Output(dgvOutputEtc, False)

		' �i�����p�^�[���ʁj���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�
		dgvInitialize_Filter(dgvFilter)

		' �S�I������
		dgvInitialize_SelectBtn(dgvOutputBtnClear)

		' �R���{�{�b�N�X�Ɍ����p�^�[�����Z�b�g
		MyBase.SetComboBox_Pcode(cmbPCode)

		DoubleBuffered = True

		If 0 < cmbPCode.Items.Count Then
			'�O�̉�ʂőI��������Ԃ��Z�b�g
			If cmbPCode.Items.Count - 1 < g_nPaternIndex Then
				'�Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
				cmbPCode.SelectedIndex = 0
			Else
				cmbPCode.SelectedIndex = g_nPaternIndex
			End If
		Else
			' ��ʕ\�����s�Ȃ̂ŁA�����I��
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "�ُ�"
			frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()                       '�I��
		End If

		' ��ʂɃp�^�[���ʂ̒l��\��		
		m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
		Data_Disp()

		' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
		dgvCommon.DataGridViewSelectionRowColorChange(g_ColorSelection)
		dgvOutput.DataGridViewSelectionRowColorChange(g_ColorSelection)
		dgvOutputEtc.DataGridViewSelectionRowColorChange(g_ColorSelection)
		dgvFilter.DataGridViewSelectionRowColorChange(g_ColorSelection)

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

		' �c�X�N���[���o�[�\���L��Ȃ�΁Adgv�̕����L����
		'ChangeDgvWidth()
	End Sub

	''' <summary>
	''' �f�B�X�|�[�Y
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmLongPd_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

		'���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
		If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

	End Sub

	''' <summary>
	''' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
	''' </summary>
	''' <returns>���A���(True:����,False:�ُ�)</returns>
	''' <remarks></remarks>
	Private Function DB_Select() As Boolean

		Dim strSQL As String = ""
		Dim strErrMsg As String = ""
		Dim sbSQL As New System.Text.StringBuilder

		'--------------------------------------------------------------
		' ���������׌��o_�}�X�^�[�ݒ�
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     ID")
		sbSQL.AppendLine("    ,ExecInterval")
		sbSQL.AppendLine("    ,InitialExec_OldLimit")
		sbSQL.AppendLine("    ,MaxDefectLen")
		sbSQL.AppendLine("    ,FFTFreq")
		sbSQL.AppendLine("    ,FFTPeakThr")
		sbSQL.AppendLine("    ,HoughConvertAccumulatorRasio")
		sbSQL.AppendLine("    ,HoughConvertMinLineLengthRasio")
		sbSQL.AppendLine("    ,HoughConvertMaxLineGap")
		sbSQL.AppendLine("    ,HoughConvertOnlineThr")
		sbSQL.AppendLine("    ,HoughConvertTangentThr")
		sbSQL.AppendLine("    ,ExpandImage")
		sbSQL.AppendLine("    ,CsvDir")
		sbSQL.AppendLine("    ,CsvSaveDate")
		sbSQL.AppendLine("    ,PeriodicExecInterval")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0}", DB_LONGPD_COMMON)
		sbSQL.AppendLine()
		sbSQL.AppendLine("ORDER BY ID ASC")

		strSQL = sbSQL.ToString()

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_Common, m_Adapter_LongPd_Common, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_COMMON & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' ���������׌��o_�����p�^�[���R�[�h�ʐݒ�
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     Pcode")
		sbSQL.AppendLine("    ,Enable")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0}", DB_LONGPD_PCODE)
		sbSQL.AppendLine()
		sbSQL.AppendLine("ORDER BY Pcode ASC")

		strSQL = sbSQL.ToString()

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCode, m_Adapter_LongPd_PCode, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     Pcode")
		sbSQL.AppendLine("    ,Filter")
		sbSQL.AppendLine("    ,Name")
		sbSQL.AppendLine("    ,Enable")
		sbSQL.AppendLine("    ,MaskFront")
		sbSQL.AppendLine("    ,MaskTail")
		sbSQL.AppendLine("    ,MaskLeft")
		sbSQL.AppendLine("    ,MaskRight")
		sbSQL.AppendLine("    ,DetectPdMin")
		sbSQL.AppendLine("    ,DetectPdMax")
		sbSQL.AppendLine("    ,DefectNumMin")
		sbSQL.AppendLine("    ,DefectNumMax")
		sbSQL.AppendLine("    ,WidthDiv")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0}", DB_LONGPD_PCODE_FILTER)
		sbSQL.AppendLine()
		sbSQL.AppendLine("ORDER BY")
		sbSQL.AppendLine("    Pcode ASC,")
		sbSQL.AppendLine("    Filter ASC")

		strSQL = sbSQL.ToString()

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCodeFilter, m_Adapter_LongPd_PCodeFilter, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' ����������_�r��ݒ�
		'--------------------------------------------------------------
		sbSQL.Clear()
		sbSQL.AppendLine("SELECT")
		sbSQL.AppendLine("     Pcode")
		sbSQL.AppendLine("    ,Filter")
		sbSQL.AppendLine("    ,TypeEdasysID")
		sbSQL.AppendLine("    ,GrEdasysID")
		sbSQL.AppendLine("FROM")
		sbSQL.AppendFormat("    {0} A ", DB_LONGPD_PCODE_FILTER_OUTPUT)
		sbSQL.AppendLine()
		sbSQL.AppendFormat("    LEFT OUTER JOIN {0} B ", DB_TYPE_NAME)
		sbSQL.AppendLine()
		sbSQL.AppendLine(" ON A.TypeEdasysID = B.EdasysID ")
		sbSQL.AppendLine("ORDER BY B.ID ")

		strSQL = sbSQL.ToString()

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCodeFilterOutput, m_Adapter_LongPd_PCodeFilterOutput, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' �r�� ���R���{�{�b�N�X�p
		'--------------------------------------------------------------
		' SQL������ �r���ID�ŏ���
		strSQL = "SELECT EdasysID"
		strSQL &= ", Name"
		strSQL &= " FROM " & DB_TYPE_NAME
		strSQL &= " ORDER BY ID ASC"

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'--------------------------------------------------------------
		' �O���[�h ���R���{�{�b�N�X�p
		'--------------------------------------------------------------
		' SQL������ �O���[�h�͗D��x�ŏ���
		strSQL = "SELECT EdasysID"
		strSQL &= ", Name"
		strSQL &= ", �}�b�v�\���F"
		strSQL &= ", �D��x"
		strSQL &= " FROM " & DB_GRADE_NAME
		strSQL &= " ORDER BY �D��x ASC"

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		Return True
	End Function

	''' <summary>
	''' DB����Ƃ��Ă����R���{�{�b�N�X�p�̒l��\���p�ɉ��H
	''' </summary>
	''' <remarks>�O���[�h���̂Ɉȏ������A�����̍s��ǉ��A</remarks>
	Private Sub Data_Init()
		Dim drowNewType As DataRow      ' �r��}�X�^�[�̐V�K�s
		Dim drowNewGrade As DataRow     ' �r��O���[�h�̐V�K�s

		Try
			'�O���[�h��ڑ��e�[�u���̐ݒ�
			' �S�s���[�v �O���[�h�̖��̂Ɉȏ������
			For ii As Integer = 0 To m_DsGrade.Tables(0).Rows.Count - 1
				m_DsGrade.Tables(0).Rows(ii).Item("Name") = m_DsGrade.Tables(0).Rows(ii).Item("Name").ToString & "�ȏ�"
			Next ii
			' �O���[�h�̐V�K�s��ݒ� EdasysID�A�O���[�h���́A�}�b�v�\���F
			drowNewGrade = m_DsGrade.Tables(0).NewRow()
			drowNewGrade.Item("EdasysID") = 0
			drowNewGrade.Item("Name") = "-"
			drowNewGrade.Item("�}�b�v�\���F") = -1      ' �}�b�v�\���F�A�g��Ȃ����ǈꉞ�ݒ�
			' �e�[�u���̈�ԏ�ɑ}��
			m_DsGrade.Tables(0).Rows.InsertAt(drowNewGrade, 0)

		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R���{�{�b�N�X�p�f�[�^ �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
		End Try

	End Sub

	''' <summary>
	''' �擾�f�[�^����ʂɕ\������B
	''' </summary>
	''' <param name="bDispOnlyPcode">True:�R���{�őI�����ꂽPCode�̃f�[�^�����\�� False:�S�\��</param>
	''' <remarks></remarks>
	Private Sub Data_Disp(Optional ByVal bDispOnlyPcode As Boolean = False)

		If False = bDispOnlyPcode Then
			' �S�I���A�S�����{�^���ݒ�
			Call Disp_SelectBtn(dgvOutputBtnClear)

			' ���������׌��o_�}�X�^�[�ݒ�
			Call Db_Disp_Common()
		End If

		' ����������_�r��ݒ�
		Call Db_Disp_PcodeFilterOutput()

		' ���������׌��o_�����p�^�[���R�[�h�ʐݒ�
		Call Db_Disp_Pcode()

		' ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�
		Call Db_Disp_PcodeFilter()

	End Sub

	''' <summary>
	''' �r��f�[�^dgv�̃`�F�b�N�{�b�N�X�̃`�F�b�N��ύX�������@�F��ς���
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvOutput_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvOutput.CellValueChanged, dgvOutputEtc.CellValueChanged
		Dim dgv As DataGridView = CType(sender, DataGridView)

		' �s�ȊO�̓X���[
		If e.RowIndex < 0 Then Exit Sub

		If EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start <= e.ColumnIndex And
		   EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + MAX_LONGPD_FILTER >= e.ColumnIndex Then

			' �Z���̒l���擾
			Dim strEdasysID As String = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

			' �Z���̒l�iEdasysID�j�ŐF���擾���A�w�i�F��ύX���� 
			dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strEdasysID)
		End If
	End Sub

	''' <summary>
	''' �F�擾
	''' </summary>
	''' <param name="key">GrEdasysID</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetCmbColor(ByVal key As String) As Color
		' EdasysID�̒l�����o��
		Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE��ɑ����̏�������
		Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select(strWhere)       ' 1�s����͂�

		Dim retColor As Color = Color.Empty   ' �߂�l �����l��NULL�F

		' Key��0�Ȃ�NULL�F��Ԃ�
		If 0 = CInt(key) Then Return retColor

		' 1�s�����Ȃ��Ǝv������
		For ii As Integer = 0 To drowGrade.Length - 1
			' �J���[���擾
			retColor = Color.FromArgb(CInt(drowGrade(0).Item("�}�b�v�\���F").ToString))
		Next ii

		Return retColor
	End Function

	''' <summary>
	''' �S����dgv�{�^��������
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvTypeMscBtnClear_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvOutputBtnClear.CellContentClick

		'�{�^����łȂ���΂Ȃɂ����Ȃ�
		If e.ColumnIndex <= 0 Then Return

		'��̃R���{�𖢑I���ɂ���B
		ClearOutputComboBox(dgvOutput, e.ColumnIndex)
		ClearOutputComboBox(dgvOutputEtc, e.ColumnIndex)
	End Sub

	''' <summary>
	''' �Ώ�dgv�̑Ώۗ�̃R���{�{�b�N�X�𖢑I���ɂ���B
	''' </summary>
	''' <param name="dgv">�Ώۂ�dgv</param>
	''' <param name="nIndex">��C���f�b�N�X</param>
	''' <remarks></remarks>
	Private Sub ClearOutputComboBox(ByVal dgv As DataGridView, ByVal nIndex As Integer)
		Dim ii As Integer   '�J�E���^

		'�S�s�̃R���{�{�b�N�X�𖢑I���ɁB
		For ii = 0 To dgv.RowCount - 1
			dgv(nIndex, ii).Value = 0
		Next ii
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

			'��ʐݒ�l��ǂݍ���
			Data_Get()

			' �z���g�ɓo�^        
			If DB_UpDate() Then
				frmMsg.Text = "�m�菈��"
				frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
				If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
				'DB�f�[�^�ēǍ�
				Data_Disp()
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

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �ݒ�{�^��������� ���̓`�F�b�N�ADB�X�V�����֘A DB�X�V�������L�����邱��
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' ���͒l�̃`�F�b�N
	''' </summary>
	''' <returns>���A���</returns>
	''' <remarks></remarks>
	Private Function Data_Check() As Boolean
		Dim strWk As String
		Dim nColIx As Integer                                   ' ��INDEX


		' ����(���������׌��o_�}�X�^�[�ݒ�)
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_COMMON_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strFilterMsg As String = String.Format("�t�B���^�[{0}��", ii + 1)

			' ��������s�Ԋu
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "��������s�Ԋu") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value = strWk

			' ��������s���ёΏ۔͈�
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "��������s���ёΏ۔͈�") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value = strWk

			' �yFFT�z�ő��r����
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "�yFFT�z�ő��r����") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value = strWk

			' �yFFT�z�T���v�����O����
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, Int32.MaxValue, False, strFilterMsg & "�yFFT�z�T���v�����O����") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value = strWk

			' �yFFT�z�s�[�N���o臒l
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "�yFFT�z�s�[�N���o臒l") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value = strWk

			' �y�n�t�ϊ��zFFT�����{��
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "�y�n�t�ϊ��zFFT�����{��") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value = strWk

			' �y�n�t�ϊ��z�ŏ��������{��
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "�y�n�t�ϊ��z�ŏ��������{��") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value = strWk

			' �y�n�t�ϊ��z���C���M���b�v
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "�y�n�t�ϊ��z���C���M���b�v") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value = strWk

			' �y�n�t�ϊ��z�����󔻒f�]�T��
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "�y�n�t�ϊ��z�����󔻒f�]�T��") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value = strWk

			' �y�n�t�ϊ��z�Tangent�{�� 
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, , , False, 4, strFilterMsg & "�y�n�t�ϊ��z�Tangent�{�� ") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value = strWk

			' �y�n�t�ϊ��z�摜�A�������L��
			' �R���{�Ȃ̂Ń`�F�b�N�Ȃ�

			' CSV�o�̓��[�h�o�͐�t�H���_
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, , , strFilterMsg & "CSV�o�̓��[�h�o�͐�t�H���_ ") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value = strWk

			' CSV�o�̓��[�h�o�͐�t�@�C���ۑ�����
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "CSV�o�̓��[�h�o�͐�t�@�C���ۑ�����") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value = strWk

			' ����������s�b�`
			strWk = CStr(dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "����������s�b�`") <> 0 Then Return False
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value = strWk
		Next ii

		' ���������׌��o_�����p�^�[���R�[�h�ʐݒ�
		' �R���{�Ȃ̂Ń`�F�b�N�Ȃ�

		' ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_FILTER_GRID_COL_ID.emTxt_Filter_Start + ii
			Dim strFilterMsg As String = String.Format("�t�B���^�[{0}��", ii + 1)

			' �t�B���^�[����
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, , 64, strFilterMsg & "�t�B���^�[����") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value = strWk

			' �L���^����
			' �R���{�Ȃ̂Ń`�F�b�N�Ȃ�

			' �R�C����[�}�X�N��
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "�R�C����[�}�X�N��") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value = strWk

			' �R�C�����[�}�X�N��
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "�R�C�����[�}�X�N��") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value = strWk

			' ���G�b�W�}�X�N��
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "���G�b�W�}�X�N��") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value = strWk

			' �E�G�b�W�}�X�N��
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, False, strFilterMsg & "�E�G�b�W�}�X�N��") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value = strWk

			' ���o��������
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "���o��������") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value = strWk

			' ���o�������
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, strFilterMsg & "���o�������") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value = strWk

			' ���o������
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, -1, Int32.MaxValue, False, strFilterMsg & "���o������") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value = strWk

			' ���o�����
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, -1, Int32.MaxValue, False, strFilterMsg & "���o�����") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value = strWk

			' �������o�����P�ʋ�
			strWk = CStr(dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, False, "�������o�����P�ʋ�") <> 0 Then Return False
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value = strWk
		Next ii

		' ����������_�r��ݒ�
		' �R���{�Ȃ̂Ń`�F�b�N�Ȃ�

		Return True
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��ʂɕ\�����̃f�[�^�擾
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Get()
		Data_Get_Common()
		Data_Get_Output()
		Data_Get_Pcode()
		Data_Get_PcodeFilter()
	End Sub

	''' <summary>
	'''  ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g (���������׌��o_�}�X�^�[�ݒ�)
	''' </summary>
	Private Sub Data_Get_Common()
		Dim nColIx As Integer                                               ' ��INDEX

		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_COMMON_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("ID='{0}' ", ii + 1)            ' WHERE��ɑ����̏�������
			Dim drow() As DataRow = m_DS_LongPd_Common.Tables(0).Select(strWhere)

			'' �f�[�^��������Βǉ�
			'If drow Is Nothing OrElse drow.Length <= 0 Then

			'	Dim temp As DataRow = m_DS_LongPd_Common.Tables(0).NewRow
			'	temp.Item("ID") = (ii + 1).ToString()
			'	m_DS_LongPd_Common.Tables(0).Rows.Add(temp)

			'	drow = m_DS_LongPd_Common.Tables(0).Select(strWhere)
			'End If

			If drow Is Nothing OrElse drow.Length <= 0 Then
				' �S���L�锤
				Continue For
			End If

			' ��������s�Ԋu
			drow(0).Item("ExecInterval") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value

			' ��������s���ёΏ۔͈�
			drow(0).Item("InitialExec_OldLimit") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value

			' �yFFT�z�ő��r����
			drow(0).Item("MaxDefectLen") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value

			' �yFFT�z�T���v�����O����
			drow(0).Item("FFTFreq") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value

			' �yFFT�z�s�[�N���o臒l
			drow(0).Item("FFTPeakThr") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value

			' �y�n�t�ϊ��zFFT�����{��
			drow(0).Item("HoughConvertAccumulatorRasio") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value

			' �y�n�t�ϊ��z�ŏ��������{��
			drow(0).Item("HoughConvertMinLineLengthRasio") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value

			' �y�n�t�ϊ��z���C���M���b�v
			drow(0).Item("HoughConvertMaxLineGap") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value

			' �y�n�t�ϊ��z�����󔻒f�]�T��
			drow(0).Item("HoughConvertOnlineThr") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value

			' �y�n�t�ϊ��z�Tangent�{�� 
			drow(0).Item("HoughConvertTangentThr") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value

			' �y�n�t�ϊ��z�摜�A�������L��
			For nIdx As Integer = 0 To m_strCmbExpandImageValue.Length - 1
				If dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage).Value.ToString = m_strCmbExpandImageValue(nIdx) Then
					drow(0).Item("ExpandImage") = nIdx
				End If
			Next nIdx

			' CSV�o�̓��[�h�o�͐�t�H���_
			drow(0).Item("CsvDir") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value

			' CSV�o�̓��[�h�o�͐�t�@�C���ۑ�����
			drow(0).Item("CsvSaveDate") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value

			' ����������s�b�`
			drow(0).Item("PeriodicExecInterval") = dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value
		Next ii

	End Sub

	''' <summary>
	''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g (����������_�r��ݒ�)
	''' </summary>
	Private Sub Data_Get_Output()

		Try
			' �f�[�^���擾
			For ii As Integer = 0 To dgvOutput.RowCount - 1
				Dim nTypeEdasysId As Integer = CInt(dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_EdasysId, ii).Value)

				For nFilter As Integer = 1 To MAX_LONGPD_FILTER
					Dim strWhere As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID={2}", m_KeyCode, nFilter, nTypeEdasysId)
					Dim drow() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhere)                ' EdasysID��1�ȏ�̃f�[�^��    0�`64�s

					'���݉�ʂɕ\�����Ă���pcode�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
					Dim nRowCount As Integer = drow.Length
					For nRowIndex As Integer = 1 To nRowCount
						m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Remove(drow(nRowCount - nRowIndex))
					Next

					' ���\�����Ă���f�[�^���i�[
					Dim drowNew As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).NewRow()
					drowNew.Item("pcode") = m_KeyCode
					drowNew.Item("filter") = nFilter
					drowNew.Item("TypeEdasysID") = nTypeEdasysId
					drowNew.Item("GrEdasysID") = dgvOutput(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, ii).Value

					' �e�[�u���ɒǉ�
					m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.InsertAt(drowNew, m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count)
				Next nFilter
			Next ii

			' ���̑�
			For nFilter As Integer = 1 To MAX_LONGPD_FILTER
				Dim strWhereEtc As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID=-1", m_KeyCode, nFilter)
				Dim drowEtc() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhereEtc)

				'���݉�ʂɕ\�����Ă���pcode�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
				Dim nRowCount As Integer = drowEtc.Length
				For nRowIndex As Integer = 1 To nRowCount
					m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Remove(drowEtc(nRowCount - nRowIndex))
				Next

				' ���\�����Ă���f�[�^���i�[
				Dim drowNew As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).NewRow()
				drowNew.Item("pcode") = m_KeyCode
				drowNew.Item("filter") = nFilter
				drowNew.Item("TypeEdasysID") = -1
				drowNew.Item("GrEdasysID") = dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, 0).Value


				' �e�[�u���ɒǉ�
				m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.InsertAt(drowNew, m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count)
			Next nFilter

		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, " �擾�G���[ [" & ex.Message & "] " & Me.Text)
			Return
		End Try
	End Sub

	''' <summary>
	'''  ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g (���������׌��o_�����p�^�[���R�[�h�ʐݒ�)
	''' </summary>
	Private Sub Data_Get_Pcode()
		Dim strWhere As String = String.Format("Pcode='{0}' ", m_KeyCode)            ' WHERE��ɑ����̏�������
		Dim drow() As DataRow = m_DS_LongPd_PCode.Tables(0).Select(strWhere)

		If drow Is Nothing OrElse drow.Length <= 0 Then
			' �f�[�^�͗L�锤
		Else
			drow(0).Item("Enable") = cmbEnable.SelectedIndex
		End If
	End Sub

	''' <summary>
	'''  ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g(���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�)
	''' </summary>
	Private Sub Data_Get_PcodeFilter()
		Dim nColIx As Integer                                               ' ��INDEX

		' �f�[�^�\��
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_FILTER_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("Pcode='{0}' and Filter={1} ", m_KeyCode, ii + 1)            ' WHERE��ɑ����̏�������
			Dim drow() As DataRow = m_DS_LongPd_PCodeFilter.Tables(0).Select(strWhere)

			' �f�[�^��������΃X�L�b�v
			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If

			' �t�B���^�[����
			If dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value Is Nothing OrElse
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value.ToString = "" Then

				drow(0).Item("Name") = DBNull.Value
			Else
				drow(0).Item("Name") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value
			End If

			' �L���^����
			For nIdx As Integer = 0 To m_strCmbFilterEnableValue.Length - 1
				If dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable).Value.ToString = m_strCmbFilterEnableValue(nIdx) Then
					drow(0).Item("Enable") = nIdx
				End If
			Next nIdx

			' �R�C����[�}�X�N��
			drow(0).Item("MaskFront") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value

			' �R�C�����[�}�X�N��
			drow(0).Item("MaskTail") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value

			' ���G�b�W�}�X�N��
			drow(0).Item("MaskLeft") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value

			' �E�G�b�W�}�X�N��
			drow(0).Item("MaskRight") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value

			' ���o��������
			drow(0).Item("DetectPdMin") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value

			' ���o�������
			drow(0).Item("DetectPdMax") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value

			' ���o������
			drow(0).Item("DefectNumMin") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value

			' ���o�����
			drow(0).Item("DefectNumMax") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value

			' �������o�����P�ʋ�
			drow(0).Item("WidthDiv") = dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value

		Next ii
	End Sub


	''' <summary>
	''' �A�b�v�f�[�g
	''' </summary>
	''' <returns>���A���(True:����,False:�ُ�)</returns>
	''' <remarks></remarks>
	Private Function DB_UpDate() As Boolean
		Dim transactCommon As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������
		Dim transactPCode As SqlClient.SqlTransaction = Nothing    '�g�����U�N�V��������
		Dim transactFilter As SqlClient.SqlTransaction = Nothing    '�g�����U�N�V��������
		Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
		tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim sbSql As New Text.StringBuilder
		Dim bProcState As Boolean = False

		Try
			' DB�ڑ�
			'''' �f�[�^�x�[�X�I�[�v��
			If Not tcls_DB.Open() Then
				bProcState = False
				Exit Try
			End If

			m_Adapter_LongPd_Common.SelectCommand.Connection.Open()
			m_Adapter_LongPd_PCode.SelectCommand.Connection.Open()
			m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.Open()

			'�g�����U�N�V�����J�n
			transactCommon = m_Adapter_LongPd_Common.SelectCommand.Connection.BeginTransaction()
			m_Adapter_LongPd_Common.SelectCommand.Transaction = transactCommon

			transactPCode = m_Adapter_LongPd_PCode.SelectCommand.Connection.BeginTransaction()
			m_Adapter_LongPd_PCode.SelectCommand.Transaction = transactPCode

			transactFilter = m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.BeginTransaction()
			m_Adapter_LongPd_PCodeFilter.SelectCommand.Transaction = transactFilter

			tcls_DB.TransactStart()

			' �X�V����
			Using cbWk As New SqlClient.SqlCommandBuilder()
				' �A�_�v�^���Z�b�g���čX�V
				cbWk.DataAdapter = m_Adapter_LongPd_Common
				m_Adapter_LongPd_Common.Update(m_DS_LongPd_Common)

				cbWk.DataAdapter = m_Adapter_LongPd_PCode
				m_Adapter_LongPd_PCode.Update(m_DS_LongPd_PCode)

				cbWk.DataAdapter = m_Adapter_LongPd_PCodeFilter
				m_Adapter_LongPd_PCodeFilter.Update(m_DS_LongPd_PCodeFilter)
			End Using

			' ����������_�r��ݒ��DELETE INSERT����̂ō폜
			sbSql.Clear()
			sbSql.AppendLine("DELETE FROM " & DB_LONGPD_PCODE_FILTER_OUTPUT)
			If 0 > tcls_DB.DMLExecute(sbSql.ToString) Then
				' �G���[�Ȃ烍�[���o�b�N���ďI��
				tcls_DB.TransactRollback()
				bProcState = False
				Exit Try
			End If

			'��������_�r��ݒ��e�[�u���փf�[�^��o�^ 
			For ii As Integer = 0 To m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count - 1
				With m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows(ii)
					If CInt(.Item("TypeEdasysID").ToString) = -1 Or
					   (CInt(.Item("TypeEdasysID").ToString) > 0 And CInt(.Item("GrEdasysID").ToString) > 0) Then

						'TYPEOUT�o�^SQL���쐬						
						sbSql.Clear()
						sbSql.AppendFormat("INSERT INTO {0} VALUES (", DB_LONGPD_PCODE_FILTER_OUTPUT)
						sbSql.AppendLine()
						sbSql.AppendLine(String.Format(" {0}", .Item("pcode")))
						sbSql.AppendLine(String.Format(",{0}", .Item("filter")))
						sbSql.AppendLine(String.Format(",{0}", .Item("TypeEdasysID")))
						sbSql.AppendLine(String.Format(",{0}", .Item("GrEdasysID")))
						sbSql.AppendLine(")")

						'SQL���s
						If 0 > tcls_DB.DMLExecute(sbSql.ToString) Then
							' �G���[�Ȃ烍�[���o�b�N���ďI��
							tcls_DB.TransactRollback()
							Return False
						End If
					End If
				End With
			Next ii

			'�R�~�b�g
			transactCommon.Commit()
			transactPCode.Commit()
			transactFilter.Commit()
			tcls_DB.TransactCommint()

			'�����@�\�փp�����[�^�ύX�ʒm
			modMain.Send_Main(CH_PARAM_LONGPD)                             '�����ւ̒ʒm��ύX���鎖
			modMain.HardCopy(Me)

			bProcState = True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)

			'' �G���[�Ȃ烍�[���o�b�N���ďI��
			If Not transactCommon Is Nothing Then
				transactCommon.Rollback()
			End If

			If Not transactPCode Is Nothing Then
				transactPCode.Rollback()
			End If

			If Not transactFilter Is Nothing Then
				transactFilter.Rollback()
			End If

			If Not tcls_DB Is Nothing Then
				tcls_DB.TransactRollback()
			End If


			bProcState = False
		Finally
			'' ��n��
			If m_Adapter_LongPd_Common.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter_LongPd_Common.SelectCommand.Connection.Close()
			End If

			If m_Adapter_LongPd_PCode.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter_LongPd_PCode.SelectCommand.Connection.Close()
			End If

			If m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter_LongPd_PCodeFilter.SelectCommand.Connection.Close()
			End If

			If Not transactCommon Is Nothing Then
				transactCommon.Dispose()
			End If

			If Not transactPCode Is Nothing Then
				transactPCode.Dispose()
			End If

			If Not transactFilter Is Nothing Then
				transactFilter.Dispose()
			End If

			If Not tcls_DB Is Nothing Then
				tcls_DB.Close()
				tcls_DB.Dispose()
			End If
		End Try

		Return bProcState
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���̑��̊֐�
	'/ /////////////////////////////////////////////////////////////////////////////////

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��ʕ\������
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' DB�f�[�^��\������ (���������׌��o_�}�X�^�[�ݒ�)
	''' </summary>
	''' <remarks></remarks>
	Private Sub Db_Disp_Common()
		Dim nColIx As Integer                                               ' ��INDEX

		' �s�\��
		dgvCommon.RowCount = EM_COMMON_GRID_ROW_ID.ROW_MAX                  ' �s���ݒ�

		' �s�^�C�g���ݒ�
		For ii As Integer = 0 To EM_COMMON_GRID_ROW_ID.ROW_MAX - 1
			dgvCommon(EM_COMMON_GRID_COL_ID.emTxt_Name, ii).Value = m_strCommonRowHead(ii)
			dgvCommon(EM_COMMON_GRID_COL_ID.emTxt_Name, ii).Style.Font = New System.Drawing.Font("�l�r ����", 9, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
		Next ii

		' �f�[�^�\��
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_COMMON_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("ID='{0}' ", ii + 1)            ' WHERE��ɑ����̏�������
			Dim drow() As DataRow = m_DS_LongPd_Common.Tables(0).Select(strWhere)

			' �s���̏����ݒ�
			' �y�n�t�ϊ��z�摜�A�������L��
			Using clm As New DataGridViewComboBoxCell
				Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
				listItem.Items.AddRange(m_strCmbExpandImageValue)
				listItem.MaxDropDownItems = listItem.Items.Count
				dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage) = clm
			End Using

			' CSV�o�̓��[�h�o�͐�t�H���_
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Style.Alignment = DataGridViewContentAlignment.MiddleLeft

			' �f�[�^��������΃X�L�b�v
			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If


			' ��������s�Ԋu
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_ExecInterval).Value = drow(0).Item("ExecInterval")

			' ��������s���ёΏ۔͈�
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_InitialExec_OldLimit).Value = drow(0).Item("InitialExec_OldLimit")

			' �yFFT�z�ő��r����
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_MaxDefectLen).Value = drow(0).Item("MaxDefectLen")

			' �yFFT�z�T���v�����O����
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTFreq).Value = drow(0).Item("FFTFreq")

			' �yFFT�z�s�[�N���o臒l
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_FFTPeakThr).Value = drow(0).Item("FFTPeakThr")

			' �y�n�t�ϊ��zFFT�����{��
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertAccumulatorRasio).Value = drow(0).Item("HoughConvertAccumulatorRasio")

			' �y�n�t�ϊ��z�ŏ��������{��
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMinLineLengthRasio).Value = drow(0).Item("HoughConvertMinLineLengthRasio")

			' �y�n�t�ϊ��z���C���M���b�v
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertMaxLineGap).Value = drow(0).Item("HoughConvertMaxLineGap")

			' �y�n�t�ϊ��z�����󔻒f�]�T��
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertOnlineThr).Value = drow(0).Item("HoughConvertOnlineThr")

			' �y�n�t�ϊ��z�Tangent�{�� 
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_HoughConvertTangentThr).Value = drow(0).Item("HoughConvertTangentThr")

			' �y�n�t�ϊ��z�摜�A�������L��
			If m_strCmbExpandImageValue.Length - 1 < CInt(drow(0).Item("ExpandImage")) OrElse 0 > CInt(drow(0).Item("ExpandImage")) Then
				' �͈͊O��0�̒l
				dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage).Value = m_strCmbExpandImageValue(0)
			Else
				dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emCmb_ExpandImage).Value = m_strCmbExpandImageValue(CInt(drow(0).Item("ExpandImage")))
			End If

			' CSV�o�̓��[�h�o�͐�t�H���_
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvDir).Value = drow(0).Item("CsvDir")

			' CSV�o�̓��[�h�o�͐�t�@�C���ۑ�����
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_CsvSaveDate).Value = drow(0).Item("CsvSaveDate")

			' ����������s�b�`
			dgvCommon(nColIx, EM_COMMON_GRID_ROW_ID.emTxt_PeriodicExecInterval).Value = drow(0).Item("PeriodicExecInterval")
		Next ii

	End Sub


	''' <summary>
	''' DB�f�[�^��\������ (���������׌��o_�����p�^�[���R�[�h�ʐݒ�)
	''' </summary>
	''' <remarks></remarks>
	Private Sub Db_Disp_Pcode()
		Dim strWhere As String = String.Format("Pcode='{0}' ", m_KeyCode)            ' WHERE��ɑ����̏�������
		Dim drow() As DataRow = m_DS_LongPd_PCode.Tables(0).Select(strWhere)

		If drow Is Nothing OrElse drow.Length <= 0 Then
			cmbEnable.SelectedIndex = 0
		Else
			Dim nEnable As Integer = CInt(drow(0).Item("Enable"))
			If 0 > nEnable Or m_strCmbLongPcodeEnable.Length - 1 < nEnable Then
				cmbEnable.SelectedIndex = 0
			Else
				cmbEnable.SelectedIndex = nEnable
			End If
		End If
	End Sub

	''' <summary>
	''' DB�f�[�^��\������ (����������_�r��ݒ�)
	''' </summary>
	Private Sub Db_Disp_PcodeFilterOutput()
		' �ꊇ�̃e�[�u���Ȃ̂ŁA�����p�^�[�����̒l�̂ݎ��o��
		Try
			' ��ʂɕ\��
			dgvOutput.RowCount = m_DsType.Tables(0).Rows.Count

			' �f�[�^���Z�b�g
			For ii As Integer = 0 To dgvOutput.RowCount - 1
				dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_No, ii).Value = ii + 1                              ' No
				dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_Name, ii).Value = m_DsType.Tables(0).Rows(ii).Item("Name").ToString
				Dim nTypeEdasysId As Integer = CInt(m_DsType.Tables(0).Rows(ii).Item("EdasysID"))
				dgvOutput(EM_OUTPUT_GRID_COL_ID.emTxt_EdasysId, ii).Value = nTypeEdasysId

				For nFilter As Integer = 1 To MAX_LONGPD_FILTER
					Dim strWhere As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID={2}", m_KeyCode, nFilter, nTypeEdasysId)
					Dim drow() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhere)                ' EdasysID��1�ȏ�̃f�[�^��    0�`64�s

					If 0 < drow.Length Then
						dgvOutput(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, ii).Value = CInt(drow(0).Item("GrEdasysID"))
					Else
						' �f�[�^������						
						dgvOutput(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, ii).Value = 0
					End If
				Next nFilter
			Next ii

			'���̑�dgv�̐ݒ�
			dgvOutputEtc.RowCount = 1                                                                             ' 1�s�Œ�
			dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emTxt_No, 0).Value = ""                                            ' No ��
			dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emTxt_Name, 0).Value = "��L�ȊO"                                  ' �r��

			For nFilter As Integer = 1 To MAX_LONGPD_FILTER
				Dim strWhereEtc As String = String.Format("pcode='{0}' and filter={1} and TypeEdasysID=-1", m_KeyCode, nFilter)
				Dim drowEtc() As DataRow = m_DS_LongPd_PCodeFilterOutput.Tables(0).Select(strWhereEtc)

				' ���̑��s�̕\���f�[�^�����邩
				If drowEtc.Length > 0 Then
					dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, 0).Value = CInt(drowEtc(0).Item("GrEdasysID"))
				Else
					' �\���f�[�^�������s�͏���������
					dgvOutputEtc(EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start + nFilter - 1, 0).Value = 0
				End If
			Next nFilter

			' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
			dgvOutput(1, 0).Selected = True
			dgvOutputEtc(1, 0).Selected = True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, " �\���G���[ [" & ex.Message & "] " & Me.Text)
			Return
		End Try
	End Sub

	''' <summary>
	''' DB�f�[�^��\������ (���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�)
	''' </summary>
	''' <remarks></remarks>
	Private Sub Db_Disp_PcodeFilter()
		Dim nColIx As Integer                                               ' ��INDEX

		' �s�\��
		dgvFilter.RowCount = EM_FILTER_GRID_ROW_ID.ROW_MAX                  ' �s���ݒ�

		' �s�^�C�g���ݒ�
		For ii As Integer = 0 To EM_FILTER_GRID_ROW_ID.ROW_MAX - 1
			dgvFilter(EM_FILTER_GRID_COL_ID.emTxt_Name, ii).Value = m_strFilterRowHead(ii)
			'dgvFilter(EM_FILTER_GRID_COL_ID.emTxt_Name, ii).Style.Font = New System.Drawing.Font("�l�r ����", 9, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
		Next ii

		' �f�[�^�\��
		For ii As Integer = 0 To MAX_LONGPD_FILTER - 1
			nColIx = EM_FILTER_GRID_COL_ID.emTxt_Filter_Start + ii

			Dim strWhere As String = String.Format("Pcode='{0}' and Filter={1} ", m_KeyCode, ii + 1)            ' WHERE��ɑ����̏�������
			Dim drow() As DataRow = m_DS_LongPd_PCodeFilter.Tables(0).Select(strWhere)

			' �s���̏����ݒ�

			' �t�B���^�[����
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Style.Alignment = DataGridViewContentAlignment.MiddleLeft

			' �L���^����
			Using clm As New DataGridViewComboBoxCell
				Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
				listItem.Items.AddRange(m_strCmbFilterEnableValue)
				listItem.MaxDropDownItems = listItem.Items.Count
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable) = clm
			End Using

			' �f�[�^��������΃X�L�b�v
			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If

			' �t�B���^�[����
			If drow(0).Item("Name") Is DBNull.Value Then
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value = ""
			Else
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_Name).Value = drow(0).Item("Name")
			End If


			' �L���^����
			If m_strCmbFilterEnableValue.Length - 1 < CInt(drow(0).Item("Enable")) OrElse 0 > CInt(drow(0).Item("Enable")) Then
				' �͈͊O��0�̒l
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable).Value = m_strCmbFilterEnableValue(0)
			Else
				dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emCmb_Enable).Value = m_strCmbFilterEnableValue(CInt(drow(0).Item("Enable")))
			End If

			' �R�C����[�}�X�N��
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskFront).Value = drow(0).Item("MaskFront")

			' �R�C�����[�}�X�N��
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskTail).Value = drow(0).Item("MaskTail")

			' ���G�b�W�}�X�N��
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskLeft).Value = drow(0).Item("MaskLeft")

			' �E�G�b�W�}�X�N��
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_MaskRight).Value = drow(0).Item("MaskRight")

			' ���o��������
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMin).Value = drow(0).Item("DetectPdMin")

			' ���o�������
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DetectPdMax).Value = drow(0).Item("DetectPdMax")

			' ���o������
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMin).Value = drow(0).Item("DefectNumMin")

			' ���o�����
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_DefectNumMax).Value = drow(0).Item("DefectNumMax")

			' �������o�����P�ʋ�
			dgvFilter(nColIx, EM_FILTER_GRID_ROW_ID.emTxt_WidthDiv).Value = drow(0).Item("WidthDiv")

		Next ii

	End Sub


	''' <summary>
	''' �S�I���@�S�����{�^���ݒ�
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
	''' <remarks></remarks>
	Private Sub Disp_SelectBtn(ByVal dgv As DataGridView)

        '�s��������
        dgv.Rows.Clear()

        ' ��ʂɕ\��
        dgv.RowCount = 1                  ' �{�^���ݒ�̃��R�[�h����"1"�ŌŒ�

    End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ DataGridView �֘A            
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �^�C�g����̃Z�����I�����ꂽ�Ƃ��I���������s�� �����r�L���ݒ�
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvTypeMscData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvOutput.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
		''�^�C�g����������Ă���Z���Ȃ�A�I����������
		'If dgv.CurrentCell.ColumnIndex <= EM_DATACOLUMN_GRID_ID.emTxt_Name Then
		'    dgv.CurrentCell.Selected = False        ' �I������
		'End If
	End Sub

    ''' <summary>
    ''' �Z���̓��e���ω������ꍇ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridView�̃`�F�b�N�{�b�N�X�������ꂽ�ꍇ �ʏ� �Z�����ړ������Ȃ��Ɗm��ɂȂ�Ȃ��ׂ̏��u</remarks>
    Private Sub dgvTypeMscData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvOutput.CurrentCellDirtyStateChanged
        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ �R�~�b�g����
        CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
    End Sub

	''' <summary>
	''' �f�[�^�O���b�h�r���[�ݒ� ���������׌��o�}�X�^�[�ݒ�
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
	''' <remarks></remarks>
	Private Overloads Sub dgvInitialize_Common(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

		With dgv
			.RowCount = 0

			.ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

			'''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 11, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			' ���`
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'.ColumnHeadersHeight = 42                                                   ' �w�b�_�[�̍���
			.RowTemplate.Height = 38                                                    ' �e���v���[�g�̍s������ݒ�

		End With

		' ��^�C�g��
		dgvColSetText(dgv, "", 190, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
		dgv.Columns(EM_COMMON_GRID_COL_ID.emTxt_Name).DefaultCellStyle.WrapMode = DataGridViewTriState.True

		For ii As Integer = 1 To MX_COMMON_COLMAX - 1
			dgvColSetText(dgv, CStr(ii), 100, False, False,
						  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
		Next ii
	End Sub

	''' <summary>
	''' �f�[�^�O���b�h�r���[�ݒ� ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�ݒ�
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
	''' <remarks></remarks>
	Private Overloads Sub dgvInitialize_Filter(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

		With dgv
			.RowCount = 0

			.ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

			'''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 11, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			' ���`
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'.ColumnHeadersHeight = 42                                                   ' �w�b�_�[�̍���
			.RowTemplate.Height = 38                                                    ' �e���v���[�g�̍s������ݒ�

		End With

		' ��^�C�g��
		dgvColSetText(dgv, "", 190, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
		dgv.Columns(EM_COMMON_GRID_COL_ID.emTxt_Name).DefaultCellStyle.WrapMode = DataGridViewTriState.True

		For ii As Integer = 1 To MX_FILTER_COLMAX - 1
			' �O���[�h�ݒ�
			dgvColSetText(dgv, CStr(ii), 100, False, False,
						  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

		Next ii
	End Sub

	Private Overloads Sub dgvInitialize_Output(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal bDisptype As Boolean)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)                         ' �I��F����
		Call dgv.DataGridViewDataExec(dgv)

		With dgv
			.ScrollBars = ScrollBars.Both                                 ' �X�N���[���o�[�̕\��

			'''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black
			.DefaultCellStyle.WrapMode = DataGridViewTriState.True
		End With

		'' ���`

		'�u��L�ȊO�v�̃O���b�h�̐ݒ�
		If bDisptype = False Then
			dgv.ColumnHeadersVisible = False        ' ��w�b�_�[��\��
			dgv.ScrollBars = ScrollBars.None        ' �X�N���[���o�[��\��
		End If

		' NO
		dgvColSetText(dgv, "NO", 45, True, True,
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' �r��
		If bDisptype = True Then

			' dgvEtc���i�v���p�e�B�Z�b�g
			dgvColSetText(dgv, "�r��", 145, True, True,
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		Else
			' dgvEtc���i�v���p�e�B�Z�b�g
			dgvColSetText(dgv, "", 145, True, True,
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		End If

		' �r��EdasysID (��\��)
		dgvColSetText(dgv, "", 0, False, True,
			DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, ,, False)

		' �O���[�h�ݒ�
		For ii As Integer = 1 To MAX_LONGPD_FILTER
			Using clm As New DataGridViewComboBoxColumn
				clm.HeaderText = CStr(ii)
				clm.Width = 100
				clm.SortMode = DataGridViewColumnSortMode.NotSortable
				clm.Frozen = False
				clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
				clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
				clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
				clm.ValueMember = "EdasysID"              ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
				clm.DisplayMember = "Name"                ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
				clm.MaxDropDownItems = 10                 ' �h���b�v�_�E�����X�g�̕\����
				clm.DropDownWidth = 150                   ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
				dgv.Columns.Add(clm)
			End Using
		Next ii

	End Sub


	''' <summary>
	''' �^�C�g����̃Z�����I�����ꂽ�Ƃ��I���������s�� �����r�ݒ�l
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgvPeriodData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvFilter.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
		''�^�C�g����������Ă���Z���Ȃ�A�I����������
		'If dgv.CurrentCell.ColumnIndex <= EM_PERIOD_GRID_COL_ID.emTxt_Title Then
		'    dgv.CurrentCell.Selected = False        ' �I������
		'End If
	End Sub

    ''' <summary>
    ''' �Z���̓��e���ω������ꍇ �����r�ݒ�l
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridView�̃`�F�b�N�{�b�N�X�������ꂽ�ꍇ �ʏ� �Z�����ړ������Ȃ��Ɗm��ɂȂ�Ȃ��ׂ̏��u</remarks>
    Private Sub dgvPeriodData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvFilter.CurrentCellDirtyStateChanged
		' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ �R�~�b�g����
		' CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
	End Sub

	''' <summary>
	''' �f�[�^�O���b�h�r���[�ݒ� �S����
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
	''' <remarks></remarks>
	Private Overloads Sub dgvInitialize_SelectBtn(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)                            ' �I��F����

		With dgv
			.RowCount = 0

			.ScrollBars = ScrollBars.None                                   ' �X�N���[���o�[�̕\��

			'''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' �I���Z���F��`
			'.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' �I��
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue   ' 1�s�����ɐF�ւ�
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black

			' ���`
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersHeight = 42                                        ' �w�b�_�[�̍���
			.ColumnHeadersVisible = False                                    ' ��w�b�_�\��

		End With

		' �f�[�^���Ɨ񐔂����킹��̂ŁA�擪�Ƀ_�~�[���ǉ�����B
		' �_�~�[1
		dgvColSetText(dgv, "Dummy1", 40, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

		' �_�~�[2
		dgvColSetText(dgv, "Dummy2", 40, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

		' �_�~�[3
		dgvColSetText(dgv, "Dummy3", 40, True, True,
					  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)


		'��������{���̃{�^���ݒ�
		For ii As Integer = EM_OUTPUT_GRID_COL_ID.emCmb_Grade_Start To MX_OUTPUT_COLMAX - 1
			' �S����
			' �S�����{�^��
			dgvColSetButton(dgv, m_strBtnName(EM_SELECT_MODE.emCancel), 100, False, False,
								DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
								New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, m_strBtnName(EM_SELECT_MODE.emCancel))
			dgv.Columns(ii).DefaultCellStyle().BackColor = Color.AliceBlue              ' �w�i
			dgv.Columns(ii).DefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' �I��

		Next ii
	End Sub

	''' <summary>
	''' �X�V�{�^���̗L��/����
	''' </summary>
	''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
	''' <remarks></remarks>
	Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

		btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��
		pnlItem.Enabled = bPassResult                   ' �R�s�[�p���X�g�̔��f�{�^��
	End Sub

	''' <summary>
	''' dgvTypeMscData�̉��X�N���[�����ύX���ꂽ�Ƃ��� ���̑�dgv���X�N���[��������
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks>���ڂ��������Ƃ��p</remarks>
	Private Sub dgvCommon_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvCommon.Scroll, dgvOutput.Scroll, dgvFilter.Scroll
		' �X�N���[���C�x���g�������X�N���[���Ȃ�
		If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
			' �X�N���[���ʒu�����킹��
			Dim nScollingOffset As Integer = 0
			If sender Is dgvCommon Then
				nScollingOffset = dgvCommon.HorizontalScrollingOffset
				dgvOutput.HorizontalScrollingOffset = nScollingOffset
				dgvFilter.HorizontalScrollingOffset = nScollingOffset
			ElseIf sender Is dgvOutput Then
				nScollingOffset = dgvOutput.HorizontalScrollingOffset
				dgvCommon.HorizontalScrollingOffset = nScollingOffset
				dgvFilter.HorizontalScrollingOffset = nScollingOffset
			Else
				nScollingOffset = dgvFilter.HorizontalScrollingOffset
				dgvCommon.HorizontalScrollingOffset = nScollingOffset
				dgvOutput.HorizontalScrollingOffset = nScollingOffset
			End If
			dgvOutputEtc.HorizontalScrollingOffset = nScollingOffset
			dgvOutputBtnClear.HorizontalScrollingOffset = nScollingOffset
		End If
	End Sub


	''' <summary>
	''' �����p�^�[���R�[�h�̑I��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub cmbPCode_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbPCode.SelectedIndexChanged
		' ���̑I���Ǝ�L�[�������ꍇ�͖���
		If m_KeyCode = cmbPCode.Text Then Exit Sub

		'' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
		' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ���
		If m_KeyCode <> "" Then
			' ���͒l�`�F�b�N
			If Not Data_Check() Then
				' ���ُ͈펞�́A�I���ʒu���߂�
				cmbPCode.Text = m_KeyCode
				Exit Sub
			End If

			' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
			Data_Get()
		End If

		' �I��ύX�ɔ��� ���x���l�ύX
		MyBase.SelectComboBoxItem(cmbPCode.SelectedIndex, cmbPCode, lblPCode)

		' ��ʂɃp�^�[���ʂ̒l��\��
		m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
		Data_Disp()
	End Sub

	''' <summary>
	''' �������菈���R���{�̑I��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub cmbEnable_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cmbEnable.SelectedIndexChanged
		If cmbEnable.Items.Count = 0 Then
			Exit Sub
		End If

		If cmbEnable.Text = m_strCmbLongPcodeEnable(0) Then
			cmbEnable.BackColor = Color.LightGray
		Else
			cmbEnable.BackColor = Color.PeachPuff
		End If
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �A�b�v�R�[�h �֘A
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���f�{�^���N���b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnUpCodeItem_UpDate_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnUpCodeItem_UpDate.Click
		'''' �����`�F�b�N
		'�R�s�[��
		Dim strFromKey As String = ""                                    '�R�s�[���L�[ 
		If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '��
		strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
		'�R�s�[��
		Dim strToKey As String = MyBase.GetUpCodeItem_To(False)              '�R�s�[��L�[
		If strToKey.Length = 0 Then Exit Sub

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		frmMsg.SetSelectedFNo(0)

		Try

			' �o�^���܂����H
			frmMsg.Text = "�����m�F"
			frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

			'''' �A�b�v�f�[�g �ύs�̂���UPDATE���͎g���Ȃ��̂ŁA�f���[�g�C���T�[�g���Ă���
			If DB_UPDATE_UpCodeItem(strFromKey, strToKey) Then
				frmMsg.Text = "���f����"
				frmMsg.ChangeMsg("����ɓo�^���܂���", Color.Black.ToArgb, True, False)
				frmMsg.ShowDialog()
			Else
				frmMsg.Text = "���f����"
				frmMsg.ChangeMsg("�o�^�Ɏ��s", Color.Black.ToArgb, True, False)
				frmMsg.ShowDialog()
			End If

			'''' �f�[�^�̍Ď擾
			If Not DB_Select() Then
				' DB ���s�Ȃ̂ŁA��ʋ����I��
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

		' �擾�f�[�^������
		Data_Init()

		'''' ��ʍĕ\��
		Data_Disp()
	End Sub

	''' <summary>
	''' �R�s�[����pcode����R�s�[���pcode�փf�[�^���f���[�g�C���T�[�g����
	''' </summary>
	''' <param name="strFromkey">�R�s�[���L�[</param>
	''' <param name="strToKey">�R�s�[��̃L�[��������������</param>
	''' <returns>����/�ُ�</returns>
	''' <remarks></remarks>
	Private Function DB_UPDATE_UpCodeItem(ByVal strFromkey As String, ByVal strToKey As String) As Boolean

		Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim strSQL As String = ""
		Dim strPcode() As String                                ' �R�s�[���pcode
		Dim strErrMsg As String = ""

		' ���������׌��o_�����p�^�[���R�[�h�ʐݒ�
		'''' SQL���̐���
		strSQL = "UPDATE " & DB_LONGPD_PCODE & " SET "
		' �ϗ�
		strSQL &= tClass_SQLServer.sqlColumnUpdate("Enable", "b")

		tClass_SQLServer.sqlColumnTrim(strSQL)      ' ���܂��Ȃ� �K�������Ă�

		' From��
		strSQL &= "FROM " & DB_LONGPD_PCODE & " a "
		strSQL &= ", (SELECT * FROM " & DB_LONGPD_PCODE
		strSQL &= " WHERE pcode='" & strFromkey & "') b "

		' WHERE��
		strSQL &= "WHERE  a.pcode IN(" & strToKey & ")"

		' SQL�����s
		If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE & " �X�V�G���[ [" & strErrMsg & "] " & Me.Text)
			Return False
		End If

		' ���������׌��o_�����p�^�[���~�t�B���^�[�ݒ�
		'''' SQL���̐���
		strSQL = "UPDATE " & DB_LONGPD_PCODE_FILTER & " SET "
		' �ϗ�
		strSQL &= tClass_SQLServer.sqlColumnUpdate("Name", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("Enable", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskFront", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskTail", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskLeft", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("MaskRight", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DetectPdMin", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DetectPdMax", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DefectNumMin", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("DefectNumMax", "b")
		strSQL &= tClass_SQLServer.sqlColumnUpdate("WidthDiv", "b")

		tClass_SQLServer.sqlColumnTrim(strSQL)      ' ���܂��Ȃ� �K�������Ă�

		' From��
		strSQL &= "FROM " & DB_LONGPD_PCODE_FILTER & " a "
		strSQL &= ", (SELECT * FROM " & DB_LONGPD_PCODE_FILTER
		strSQL &= " WHERE pcode='" & strFromkey & "') b "

		' WHERE��
		strSQL &= "WHERE a.Filter = b.Filter AND a.pcode IN(" & strToKey & ")"

		' SQL�����s
		If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER & " �X�V�G���[ [" & strErrMsg & "] " & Me.Text)
			Return False
		End If

		'�J���}��؂��pcode��z��ɕϊ�
		strPcode = Split(strToKey, ",")

		'�R�s�[���̃f�[�^���擾
		strSQL = "SELECT pcode"
		strSQL &= ", Filter"
		strSQL &= ", TypeEdasysID"
		strSQL &= ", GrEdasysID"
		strSQL &= " FROM " & DB_LONGPD_PCODE_FILTER_OUTPUT
		strSQL &= " WHERE pcode='" & strFromkey & "'"

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DS_LongPd_PCodeFilterOutput, m_Adapter_LongPd_PCodeFilterOutput, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_LONGPD_PCODE_FILTER_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		Try
			'''' �f���[�g�C���T�[�g
			If Not tcls_DB.Open() Then Return False

			'�g�����U�N�V�����J�n
			tcls_DB.TransactStart()

			' ���R�[�h�̍폜
			Dim strSqlDel As String     ' ���R�[�h�폜�psql
			strSqlDel = "DELETE FROM " & DB_LONGPD_PCODE_FILTER_OUTPUT & " WHERE pcode IN(" & strToKey & ")"

			If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False

			' ���R�[�h�̑}�� �R�s�[��pcode�̌������J��Ԃ�
			For ii As Integer = 0 To strPcode.Length - 1
				' �R�s�[��pcode�̃��R�[�h�������J��Ԃ��}��
				For kk As Integer = 0 To m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows.Count - 1
					With m_DS_LongPd_PCodeFilterOutput.Tables(0).Rows(kk)

						'OUTPUT_PCODE�o�^SQL���쐬
						strSQL = ""
						strSQL &= "INSERT INTO " & DB_LONGPD_PCODE_FILTER_OUTPUT & " VALUES ("
						strSQL &= strPcode(ii) & ","
						strSQL &= .Item("filter").ToString & ","
						strSQL &= .Item("TypeEdasysID").ToString & ","
						strSQL &= .Item("GrEdasysID").ToString & ")"

						'SQL���s
						If 0 > tcls_DB.DMLExecute(strSQL) Then
							' �G���[�Ȃ烍�[���o�b�N���ďI��
							tcls_DB.TransactRollback()
							Return False
						End If

					End With
				Next kk
			Next ii

			'�R�~�b�g
			tcls_DB.TransactCommint()

			'�����@�\�փp�����[�^�ύX�ʒm
			modMain.Send_Main(CH_PARAM_LONGPD)
			modMain.HardCopy(Me)
			Return True
		Catch ex As Exception
			' �G���[�Ȃ烍�[���o�b�N���ďI��
			tcls_DB.TransactRollback()
			gcls_Log.Write(tClass_LogManager.LNO.ERR, " �f�[�^�R�s�[�G���[ [" & ex.Message & "] " & Me.Text)
			Return False
		Finally
			tcls_DB.Close()
			tcls_DB.Dispose()
		End Try
	End Function



End Class
