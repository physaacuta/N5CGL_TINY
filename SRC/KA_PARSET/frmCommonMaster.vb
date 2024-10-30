'*********************************************************************************
' �r���萔�ݒ��ʃN���X
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmCommonMaster

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o�[�ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
	Private m_AdapterGrade As New SqlClient.SqlDataAdapter          ' �A�_�v�^�[ �O���[�h�}�X�^�[

	Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u��
	Private m_DsGrade As New DataSet                                ' ��ڑ��^�e�[�u�� �O���[�h�}�X�^�[ �R���{�{�b�N�X�p

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �萔
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Const m_PtnNsu As Integer = MAX_PCODE

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
	Private Sub frmCommonMaster_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' �t�@���N�V�����{�^���L�����ڂ̂�
		If Not enable Then Exit Sub

		'' �t�@���N�V�����ؑ�
		Select Case FncNo                       ' ��0�I���W��
			Case 6                              ' F07 ��ʈ��
				Call PrintOut()
			Case 7                              ' F08 ��ʕۑ�
				Call HardCopy(IMG_PATH, "�萔�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
	Private Sub frmCommonMaster_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
		' ���낢��J������
		m_DS.Dispose()
		If Not m_Adapter Is Nothing Then
			m_Adapter.Dispose()
			m_Adapter = Nothing
		End If

		m_DsGrade.Dispose()
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
	''' <remarks></remarks>
	Private Sub frmCommonMaster_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
		modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
		MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmCommonMaster          ' �r���萔�ݒ��ʌďo

		'�^�C�g���ݒ�
		Call SetTitleName(Me)

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

		' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
		If Not DB_Select() Then
			' DB ���s�Ȃ̂ŁA��ʋ����I��
			frmMsg.SetSelectedFNo(0)

			frmMsg.Text = "�ُ�"
			frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
			frmMsg.ShowDialog()
			modMain.TaskEnd()                       '�I��
		End If

		' �����̐ݒ�
		nudInitialize()

		' ��ʂɒl��\��        
		If Not Data_Disp() Then
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
	Private Sub frmCommonMaster_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

		'���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
		If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

	End Sub

	''' <summary>
	''' �e�L�X�g�{�b�N�X�Ȃǂ̏�����
	''' </summary>
	''' <remarks></remarks>
	Private Sub nudInitialize()
		' �O���[�v�{�b�N�X�̕\�����̂�ύX
		gupDispCount.Text = "�f�t�H���g�\������"

		' ���O�\�������̏����ݒ�
		nudDispCount.ImeMode = Windows.Forms.ImeMode.Disable        ' IME���[�h�g�p�s��
		nudDispCount.Increment = 1                                  ' ������
		nudDispCount.Maximum = 999999                               ' �ő�l
		nudDispCount.Minimum = 1                                    ' �ŏ��l

		' �T�����
		' �T����ʕ\����ԏ�
		nudDispSmall.ImeMode = Windows.Forms.ImeMode.Disable        ' IME���[�h�g�p�s��
		nudDispSmall.Increment = MAP_REPRES_MIN                     ' ������
		nudDispSmall.Maximum = 1000                                 ' �ő�l
		nudDispSmall.Minimum = MAP_REPRES_MIN                       ' �ŏ��l

		' �T����ʕ\����ԑ�
		nudDispBig.ImeMode = Windows.Forms.ImeMode.Disable          ' IME���[�h�g�p�s��
		nudDispBig.Increment = MAP_REPRES_MIN                       ' ������
		nudDispBig.Maximum = 1000                                   ' �ő�l
		nudDispBig.Minimum = MAP_REPRES_MIN                         ' �ŏ��l

		' �E�H�[�}�ގw��ʒu�̏����ݒ�
		For ii As Integer = 0 To m_PtnNsu - 1
			cmbWormPos.Items.Add(ii + 1)
		Next ii
		cmbWormPos.DropDownStyle = ComboBoxStyle.DropDownList

		' Jpeg�؂�o���摜�i���̏����ݒ�
		nudJpegGazo.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��
		nudJpegGazo.Increment = 1                                   ' ������
		nudJpegGazo.Maximum = 90                                    ' �ő�l
		nudJpegGazo.Minimum = 1                                     ' �ŏ��l

		' �J�������x臒l�̏����ݒ�
		nudCamTemp.ImeMode = Windows.Forms.ImeMode.Disable          ' IME���[�h�g�p�s��
		nudCamTemp.Increment = 1                                    ' ������
		nudCamTemp.Maximum = 99                                     ' �ő�l
		nudCamTemp.Minimum = 1                                      ' �ŏ��l

		'�G�b�W������_���{���
		cmbEdgType.Items.Clear()
		cmbEdgType.Items.Add("����")
		cmbEdgType.Items.Add("����������̂�")
		'cmbEdgType.Items.Add("����������{�G�b�W�͈͎w��")

		' ���o�͈�
		txtEdgeKenHan.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��
		' ���J�o���[���o�͈�
		txtEdgeReCov.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��
		' �����e�덷
		txtEdgeHabaGoso.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��
		' �O�t���[���Ƃ̋��e�덷
		txtEdgeFrmGoso.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��

		' �n�ڌ�s���т̏����ݒ�
		nudEdgeFukantai.ImeMode = Windows.Forms.ImeMode.Disable     ' IME���[�h�g�p�s��
		nudEdgeFukantai.Increment = 1                               ' ������
		nudEdgeFukantai.Maximum = 1000                              ' �ő�l
		nudEdgeFukantai.Minimum = 0                                 ' �ŏ��l

		' �G�b�W���o�ُ�FF���M�̏����ݒ�
		nudEdgeFF.ImeMode = Windows.Forms.ImeMode.Disable           ' IME���[�h�g�p�s��
		nudEdgeFF.Increment = 1                                     ' ������
		nudEdgeFF.Maximum = 9999                                    ' �ő�l
		nudEdgeFF.Minimum = 0                                       ' �ŏ��l

		'�G�b�W���o_���{���
		cmbEdgOutType.Items.Clear()
		cmbEdgOutType.Items.Add("����")
		cmbEdgOutType.Items.Add("�����G�b�W�͈͎w��")

		'�G�b�W���o_���o�͈�
		txtEdgeOutKenHan.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��

		' �r�}�b�v�r��臒l�̏����ݒ� 
		' �y
		nudKizumapTempKei.ImeMode = Windows.Forms.ImeMode.Disable        ' IME���[�h�g�p�s��
		nudKizumapTempKei.Increment = 1                                  ' ������
		nudKizumapTempKei.Maximum = 99                                   ' �ő�l
		nudKizumapTempKei.Minimum = 1                                    ' �ŏ��l
		' ��
		nudKizumapTempChu.ImeMode = Windows.Forms.ImeMode.Disable        ' IME���[�h�g�p�s��
		nudKizumapTempChu.Increment = 1                                  ' ������
		nudKizumapTempChu.Maximum = 99                                   ' �ő�l
		nudKizumapTempChu.Minimum = 1                                    ' �ŏ��l
		' �d
		nudKizumapTempZyu.ImeMode = Windows.Forms.ImeMode.Disable        ' IME���[�h�g�p�s��
		nudKizumapTempZyu.Increment = 1                                  ' ������
		nudKizumapTempZyu.Maximum = 99                                   ' �ő�l
		nudKizumapTempZyu.Minimum = 1                                    ' �ŏ��l

		' ������__�㗬�J�����̏����ݒ�
		nudCamera.ImeMode = Windows.Forms.ImeMode.Disable                ' IME���[�h�g�p�s��
		nudCamera.Increment = 1                                          ' ������
		nudCamera.Maximum = 999999                                       ' �ő�l
		nudCamera.Minimum = 1                                            ' �ŏ��l

		' ������__SPM�̏����ݒ�
		nudSpm.ImeMode = Windows.Forms.ImeMode.Disable                   ' IME���[�h�g�p�s��
		nudSpm.Increment = 1                                             ' ������
		nudSpm.Maximum = 999999                                          ' �ő�l
		nudSpm.Minimum = 1                                               ' �ŏ��l

		' �\����__��������̏����ݒ�
		nudExaminingTableB.ImeMode = Windows.Forms.ImeMode.Disable       ' IME���[�h�g�p�s��
		nudExaminingTableB.Increment = 1                                 ' ������
		nudExaminingTableB.Maximum = 999999                              ' �ő�l
		nudExaminingTableB.Minimum = 1                                   ' �ŏ��l

		' �\����__�\������̏����ݒ�
		nudExaminingTableT.ImeMode = Windows.Forms.ImeMode.Disable       ' IME���[�h�g�p�s��
		nudExaminingTableT.Increment = 1                                 ' ������
		nudExaminingTableT.Maximum = 999999                              ' �ő�l
		nudExaminingTableT.Minimum = 1                                   ' �ŏ��l

		' �\����__�V���[�̏����ݒ�
		nudCutPos.ImeMode = Windows.Forms.ImeMode.Disable                ' IME���[�h�g�p�s��
		nudCutPos.Increment = 1                                          ' ������
		nudCutPos.Maximum = 999999                                       ' �ő�l
		nudCutPos.Minimum = 1                                            ' �ŏ��l

		' �S���摜_�ۑ�_R
		cmbImageSaveR.Items.Add("����")
		cmbImageSaveR.Items.Add("�L��")
		cmbImageSaveR.DropDownStyle = ComboBoxStyle.DropDownList
		' �S���摜_�ۑ�_G
		cmbImageSaveG.Items.Add("����")
		cmbImageSaveG.Items.Add("�L��")
		cmbImageSaveG.DropDownStyle = ComboBoxStyle.DropDownList
		' �S���摜_�ۑ�_B
		cmbImageSaveB.Items.Add("����")
		cmbImageSaveB.Items.Add("�L��")
		cmbImageSaveB.DropDownStyle = ComboBoxStyle.DropDownList
		' �S���摜_�ۑ�_��
		cmbImageSaveRB.Items.Add("����")
		cmbImageSaveRB.Items.Add("�L��")
		cmbImageSaveRB.DropDownStyle = ComboBoxStyle.DropDownList

		' PLG������r�ُ����l
		txtHikakuPlgHigh.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��
		' PLG�����\���ُ����l
		txtHyoziPlgHigh.ImeMode = Windows.Forms.ImeMode.Disable          ' IME���[�h�g�p�s��
		' �v���R���R�C������r�ُ����l
		txtProcomHigh.ImeMode = Windows.Forms.ImeMode.Disable            ' IME���[�h�g�p�s��

		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		'-----------------------
		' ���������׃K�C�_���X
		'-----------------------

		' �K�C�_���X�L��
		cmbLongPd_GuidanceAvailable.Items.Clear()
		cmbLongPd_GuidanceAvailable.Items.Add("����")
		cmbLongPd_GuidanceAvailable.Items.Add("�L��")

		' �m�F���{�Ԋu
		nudLongPd_GuidanceCheckInterval.ImeMode = Windows.Forms.ImeMode.Disable       ' IME���[�h�g�p�s��
		nudLongPd_GuidanceCheckInterval.Increment = 1                                 ' ������
		nudLongPd_GuidanceCheckInterval.Maximum = 999999                              ' �ő�l
		nudLongPd_GuidanceCheckInterval.Minimum = 1                                   ' �ŏ��l

		' �F�`�J����͈� From
		nudLongPd_NearFurnaceRangeFrom.ImeMode = Windows.Forms.ImeMode.Disable       ' IME���[�h�g�p�s��
		nudLongPd_NearFurnaceRangeFrom.Increment = 1                                 ' ������
		nudLongPd_NearFurnaceRangeFrom.Maximum = COIL_LEN_MAX * 1000                 ' �ő�l
		nudLongPd_NearFurnaceRangeFrom.Minimum = 1                                   ' �ŏ��l

		' �F�`�J����͈� To
		nudLongPd_NearFurnaceRangeTo.ImeMode = Windows.Forms.ImeMode.Disable         ' IME���[�h�g�p�s��
		nudLongPd_NearFurnaceRangeTo.Increment = 1                                   ' ������
		nudLongPd_NearFurnaceRangeTo.Maximum = COIL_LEN_MAX * 1000                   ' �ő�l
		nudLongPd_NearFurnaceRangeTo.Minimum = 1                                     ' �ŏ��l

		' �����s�b�`��ʕ\���p����
		nudLongPd_PeriodicPitchTolerance.ImeMode = Windows.Forms.ImeMode.Disable     ' IME���[�h�g�p�s��
		nudLongPd_PeriodicPitchTolerance.Increment = 1                               ' ������
		nudLongPd_PeriodicPitchTolerance.Maximum = COIL_LEN_MAX * 1000               ' �ő�l
		nudLongPd_PeriodicPitchTolerance.Minimum = 1                                 ' �ŏ��l
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	End Sub

	''' <summary>
	''' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
	''' </summary>
	''' <returns>���A���(True:����,False:�ُ�)</returns>
	''' <remarks></remarks>
	Private Function DB_Select() As Boolean
		Dim strSQL As String = ""
		Dim strErrMsg As String = ""

		' SQL������
		strSQL = "SELECT LINEID"
		strSQL &= ",RAS����F"
		strSQL &= ",RAS�ُ�F"
		strSQL &= ",RAS���ӐF"
		strSQL &= ",LOG�\������"
		strSQL &= ",LOG�C�x���g�F"
		strSQL &= ",LOG�d�̏�F"
		strSQL &= ",LOG�y�̏�F"
		strSQL &= ",�T����ʕ\����ԏ�"
		strSQL &= ",�T����ʕ\����ԑ�"
		strSQL &= ",�E�H�[�}�ގw��ʒu"
		strSQL &= ",Jpeg�؂�o���摜�i��"
		strSQL &= ",�J�������x臒l"
		strSQL &= ",�G�b�W������_���{���"
		strSQL &= ",�G�b�W������_���o�͈�"
		strSQL &= ",�G�b�W������_���J�o���[���o�͈�"
		strSQL &= ",�G�b�W������_�����e�덷"
		strSQL &= ",�G�b�W������_�O�t���[���Ƃ̋��e�덷"
		strSQL &= ",�G�b�W������_�n�ڌ�s����"
		strSQL &= ",�G�b�W������_�G�b�W���o�ُ�FF���M"
		strSQL &= ",�G�b�W���o_���{���"
		strSQL &= ",�G�b�W���o_���o�͈�"
		strSQL &= ",�d���r�}�b�v�w�i�F_�d"
		strSQL &= ",�d���r�}�b�v�w�i�F_��"
		strSQL &= ",�d���r�}�b�v�w�i�F_�y"
		strSQL &= ",�d���r�}�b�v�r��臒l_�d"
		strSQL &= ",�d���r�}�b�v�r��臒l_��"
		strSQL &= ",�d���r�}�b�v�r��臒l_�y"
		strSQL &= ",�ݔ���_������__�㗬�J����"
		strSQL &= ",�ݔ���_������__SPM"
		strSQL &= ",�ݔ���_�\����__��������"
		strSQL &= ",�ݔ���_�\����__�\������"
		strSQL &= ",�ݔ���_�\����__�V���["
		strSQL &= ",�S���摜_�ۑ�_R"
		strSQL &= ",�S���摜_�ۑ�_G"
		strSQL &= ",�S���摜_�ۑ�_B"
		strSQL &= ",�S���摜_�ۑ�_��"
		strSQL &= ",PLG������r�ُ����l"
		strSQL &= ",PLG�����\���ُ����l"
		strSQL &= ",�v���R���R�C������r�ُ����l"
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		strSQL &= ",LongPd_GuidanceAvailable"
		strSQL &= ",LongPd_GuidanceCheckInterval"
		strSQL &= ",LongPd_NearFurnaceRangeFrom"
		strSQL &= ",LongPd_NearFurnaceRangeTo"
		strSQL &= ",LongPd_PeriodicPitchTolerance"
		strSQL &= ",LongPd_GuidanceForeColor"
		strSQL &= ",LongPd_GuidanceBackColor"
		strSQL &= ",LongPd_GuidanceMessage"
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

		strSQL &= " FROM " & DB_COMMON_MASTER

		'SQL�����s
		If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
			gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_COMMON_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
			Return False
		End If

		' SQL������ �O���[�h�͗D��x�ŏ���
		strSQL = "SELECT EdasysID, Name ,�}�b�v�\���F "
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
	''' DB�̏�����ʂɕ\��
	''' </summary>
	''' <returns>���A���(True:����,False:�ُ�)</returns>
	''' <remarks></remarks>
	Private Function Data_Disp() As Boolean
		Dim drow() As DataRow     ' �f�[�^�s

		' ��L�[���w�肵�Ď��o���Ă�̂ŁA1�������Ȃ��͂�
		drow = m_DS.Tables(0).Select()

		' �擾�s��1�s�łȂ��Ȃ�G���[�I��
		If drow.Length <> 1 Then Return False

		' ��ʂɕ\��
		lblNormalColor.BackColor = Color.FromArgb(CInt(drow(0).Item("RAS����F")))              ' RAS����F
		lblAbnormalColor.BackColor = Color.FromArgb(CInt(drow(0).Item("RAS�ُ�F")))            ' RAS�ُ�F
		lblCautionColor.BackColor = Color.FromArgb(CInt(drow(0).Item("RAS���ӐF")))             ' RAS���ӐF
		nudDispCount.Text = CStr(drow(0).Item("LOG�\������"))                                   ' LOG�\������
		lblEventColor.ForeColor = Color.FromArgb(CInt(drow(0).Item("LOG�C�x���g�F")))           ' LOG�C�x���g�F
		lblZyuColor.ForeColor = Color.FromArgb(CInt(drow(0).Item("LOG�d�̏�F")))               ' LOG�d�̏�F
		lblKeiColor.ForeColor = Color.FromArgb(CInt(drow(0).Item("LOG�y�̏�F")))               ' LOG�y�̏�F
		nudDispSmall.Text = CStr(drow(0).Item("�T����ʕ\����ԏ�"))                            ' �T����ʕ\����ԏ�
		nudDispBig.Text = CStr(drow(0).Item("�T����ʕ\����ԑ�"))                              ' �T����ʕ\����ԑ�

		If CInt(drow(0).Item("�E�H�[�}�ގw��ʒu")) >= 1 And
		   CInt(drow(0).Item("�E�H�[�}�ގw��ʒu")) <= m_PtnNsu Then
			cmbWormPos.Text = CStr(drow(0).Item("�E�H�[�}�ގw��ʒu"))                          ' �E�H�[�}�ގw��ʒu
		End If

		nudJpegGazo.Text = CStr(drow(0).Item("Jpeg�؂�o���摜�i��"))                           ' Jpeg�؂�o���摜�i��
		nudCamTemp.Text = CStr(drow(0).Item("�J�������x臒l"))                                  ' �J�������x臒l
		Dim nEdgType As Integer = CInt(drow(0).Item("�G�b�W������_���{���"))                   ' �G�b�W������_���{���
		Select Case nEdgType
			Case 0 To 1
				cmbEdgType.SelectedIndex = nEdgType
			Case Else
				cmbEdgType.SelectedIndex = 0
		End Select
		txtEdgeKenHan.Text = CStr(drow(0).Item("�G�b�W������_���o�͈�"))                        ' �G�b�W������_���o�͈�
		txtEdgeReCov.Text = CStr(drow(0).Item("�G�b�W������_���J�o���[���o�͈�"))               ' �G�b�W������_���J�o���[���o�͈�
		txtEdgeHabaGoso.Text = CStr(drow(0).Item("�G�b�W������_�����e�덷"))                  ' �G�b�W������_�����e�덷
		txtEdgeFrmGoso.Text = CStr(drow(0).Item("�G�b�W������_�O�t���[���Ƃ̋��e�덷"))         ' �G�b�W������_�O�t���[���Ƃ̋��e�덷
		nudEdgeFukantai.Text = CStr(drow(0).Item("�G�b�W������_�n�ڌ�s����"))                  ' �G�b�W������_�n�ڌ�s����
		nudEdgeFF.Text = CStr(drow(0).Item("�G�b�W������_�G�b�W���o�ُ�FF���M"))                ' �G�b�W������_�G�b�W���o�ُ�FF���M

		Dim nEdgOutType As Integer = CInt(drow(0).Item("�G�b�W���o_���{���"))                   ' �G�b�W���o_���{���
		Select Case nEdgOutType
			Case 0 To 1
				cmbEdgOutType.SelectedIndex = nEdgOutType
			Case Else
				cmbEdgOutType.SelectedIndex = 0
		End Select
		txtEdgeOutKenHan.Text = CStr(drow(0).Item("�G�b�W���o_���o�͈�"))                        ' �G�b�W���o_���o�͈�

		lblMapZyuColor.BackColor = Color.FromArgb(CInt(drow(0).Item("�d���r�}�b�v�w�i�F_�d")))  ' �d���r�}�b�v�w�i�F_�d
		lblMapChuColor.BackColor = Color.FromArgb(CInt(drow(0).Item("�d���r�}�b�v�w�i�F_��")))  ' �d���r�}�b�v�w�i�F_��
		lblMapKeiColor.BackColor = Color.FromArgb(CInt(drow(0).Item("�d���r�}�b�v�w�i�F_�y")))  ' �d���r�}�b�v�w�i�F_�y

		nudKizumapTempZyu.Text = CStr(drow(0).Item("�d���r�}�b�v�r��臒l_�d"))                ' �d���r�}�b�v�r��臒l_�d
		nudKizumapTempChu.Text = CStr(drow(0).Item("�d���r�}�b�v�r��臒l_��"))                ' �d���r�}�b�v�r��臒l_��
		nudKizumapTempKei.Text = CStr(drow(0).Item("�d���r�}�b�v�r��臒l_�y"))                ' �d���r�}�b�v�r��臒l_�y
		lblKzcntMiddleH.Text = nudKizumapTempZyu.Text                                           ' �d���r�}�b�v�r��臒l_�� ���
		lblKzcntLightH.Text = nudKizumapTempChu.Text                                            ' �d���r�}�b�v�r��臒l_�y ���

		'��_�����ݒ�
		nudCamera.Text = CStr(drow(0).Item("�ݔ���_������__�㗬�J����"))                       ' �ݔ���_������__�㗬�J����
		nudSpm.Text = CStr(drow(0).Item("�ݔ���_������__SPM"))                                 ' �ݔ���_������__SPM
		nudExaminingTableB.Text = CStr(drow(0).Item("�ݔ���_�\����__��������"))                ' �ݔ���_�\����__��������
		nudExaminingTableT.Text = CStr(drow(0).Item("�ݔ���_�\����__�\������"))                ' �ݔ���_�\����__�\������
		nudCutPos.Text = CStr(drow(0).Item("�ݔ���_�\����__�V���["))                           ' �ݔ���_�\����__�V���[

		' �S���摜
		If 1 = CInt(drow(0).Item("�S���摜_�ۑ�_R")) Then
			cmbImageSaveR.Text = "�L��"
		Else
			cmbImageSaveR.Text = "����"
		End If
		If 1 = CInt(drow(0).Item("�S���摜_�ۑ�_G")) Then
			cmbImageSaveG.Text = "�L��"
		Else
			cmbImageSaveG.Text = "����"
		End If
		If 1 = CInt(drow(0).Item("�S���摜_�ۑ�_B")) Then
			cmbImageSaveB.Text = "�L��"
		Else
			cmbImageSaveB.Text = "����"
		End If
		If 1 = CInt(drow(0).Item("�S���摜_�ۑ�_��")) Then
			cmbImageSaveRB.Text = "�L��"
		Else
			cmbImageSaveRB.Text = "����"
		End If

		txtHikakuPlgHigh.Text = CStr(drow(0).Item("PLG������r�ُ����l"))                            ' PLG������r�ُ����l
		txtHyoziPlgHigh.Text = CStr(drow(0).Item("PLG�����\���ُ����l"))                             ' PLG�����\���ُ����l
		txtProcomHigh.Text = CStr(drow(0).Item("�v���R���R�C������r�ُ����l"))                      ' �v���R���R�C������r�ُ����l

		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		' ���������׃K�C�_���X
		If 1 = CInt(drow(0).Item("LongPd_GuidanceAvailable")) Then
			cmbLongPd_GuidanceAvailable.Text = "�L��"
		Else
			cmbLongPd_GuidanceAvailable.Text = "����"
		End If

		nudLongPd_GuidanceCheckInterval.Text = CStr(drow(0).Item("LongPd_GuidanceCheckInterval"))
		nudLongPd_NearFurnaceRangeFrom.Text = CStr(drow(0).Item("LongPd_NearFurnaceRangeFrom"))
		nudLongPd_NearFurnaceRangeTo.Text = CStr(drow(0).Item("LongPd_NearFurnaceRangeTo"))
		nudLongPd_PeriodicPitchTolerance.Text = CStr(drow(0).Item("LongPd_PeriodicPitchTolerance"))


		txtLongPd_GuidanceMessage.Text = CStr(drow(0).Item("LongPd_GuidanceMessage"))
		lblLongPd_GuidanceMessagePreview.ForeColor = Color.FromArgb(CInt(drow(0).Item("LongPd_GuidanceForeColor")))
		lblLongPd_GuidanceMessagePreview.BackColor = Color.FromArgb(CInt(drow(0).Item("LongPd_GuidanceBackColor")))
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

		Return True
	End Function

	''' <summary>
	''' �m�菈��
	''' </summary>
	''' <remarks></remarks>
	Private Sub subData_UpDate()

		' ���͒l�`�F�b�N
		If Not Data_Check() Then Exit Sub

		Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
		frmMsg.SetSelectedFNo(0)

		Try

			' �o�^���܂����H
			frmMsg.Text = "�����m�F"
			frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
			If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

			'��ʂ̃f�[�^���擾
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

			' ���݂̓��e�ōĕ\��
			Data_Disp()

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
		Dim strWk As String         ' �����񃏁[�N
		Dim strMsg As String        ' ���b�Z�[�W���[�N
		Dim nWk As Integer          ' int ���[�N
		Dim nWkH As Integer         ' �r��臒l �d ���[�N
		Dim nWkM As Integer         ' �r��臒l �� ���[�N
		Dim nWkL As Integer         ' �r��臒l �y ���[�N

		' ���O�\������
		strWk = nudDispCount.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "�f�t�H���g�\������") <> 0 Then Return False
		nudDispCount.Text = strWk

		' �T����ʕ\����ԏ�
		strWk = nudDispSmall.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 5, 1000, False, "�T����ʕ\����ԏ�") <> 0 Then Return False
		nudDispSmall.Text = strWk

		' �\����Ԃ��ŏ���\�r�Ԋu�̔{�����`�F�b�N����B�{���łȂ���΃��b�Z�[�W���o���ďI��
		If MAP_REPRES_MIN > 0 Then
			nWk = CInt(strWk) Mod MAP_REPRES_MIN
			If nWk > 0 Then
				strMsg = "�y �T����ʕ\����ԏ� �z" & vbCrLf
				strMsg &= "�K�؂Ȓl�ł͂���܂���B" & vbCrLf
				strMsg &= "���͒l�� " & MAP_REPRES_MIN & " �̔{���ɂ��ĉ������B"
				MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
				Return False
			End If
		End If

		' �T����ʕ\����ԑ�
		strWk = nudDispBig.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 5, 1000, False, "�T����ʕ\����ԑ�") <> 0 Then Return False
		nudDispBig.Text = strWk

		' �\����Ԃ��ŏ���\�r�Ԋu�̔{�����`�F�b�N����B�{���łȂ���΃��b�Z�[�W���o���ďI��
		If MAP_REPRES_MIN > 0 Then
			nWk = CInt(strWk) Mod MAP_REPRES_MIN
			If nWk > 0 Then
				strMsg = "�y �T����ʕ\����ԑ� �z" & vbCrLf
				strMsg &= "�K�؂Ȓl�ł͂���܂���B" & vbCrLf
				strMsg &= "���͒l�� " & MAP_REPRES_MIN & " �̔{���ɂ��ĉ������B"
				MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
				Return False
			End If
		End If

		' �E�H�[�}�ގw��ʒu
		' �R���{�{�b�N�X�Ȃ̂Ń`�F�b�N����

		' Jpeg�؂�o���摜�i��
		strWk = nudJpegGazo.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 90, False, "Jpeg�؂�o���摜�i��") <> 0 Then Return False
		nudJpegGazo.Text = strWk

		' �J�������x臒l
		strWk = nudCamTemp.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "�J�������x臒l") <> 0 Then Return False
		nudCamTemp.Text = strWk

		' �G�b�W������_���o�͈�
		strWk = txtEdgeKenHan.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 1, 999.99, False, 2, "�G�b�W������_���o�͈�") <> 0 Then Return False
		txtEdgeKenHan.Text = strWk

		' �G�b�W������_���J�o���[���o�͈�
		strWk = txtEdgeReCov.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 1, 999.99, False, 2, "�G�b�W������_���J�o���[���o�͈�") <> 0 Then Return False
		txtEdgeReCov.Text = strWk

		' �G�b�W������_�����e�덷
		strWk = txtEdgeHabaGoso.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 999.99, False, 2, "�G�b�W������_�����e�덷") <> 0 Then Return False
		txtEdgeHabaGoso.Text = strWk

		' �G�b�W������_�O�t���[���Ƃ̋��e�덷
		strWk = txtEdgeFrmGoso.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 1, 999.99, False, 2, "�G�b�W������_�O�t���[���Ƃ̋��e�덷") <> 0 Then Return False
		txtEdgeFrmGoso.Text = strWk

		' �G�b�W������_�n�ڌ�s����
		strWk = nudEdgeFukantai.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 1000, False, "�G�b�W������_�n�ڌ�s����") <> 0 Then Return False
		nudEdgeFukantai.Text = strWk

		' �G�b�W������_�G�b�W���o�ُ�FF���M
		strWk = nudEdgeFF.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 9999, False, "�G�b�W������_�G�b�W���o�ُ�FF���M") <> 0 Then Return False
		nudEdgeFF.Text = strWk

		' �G�b�W���o_���o�͈�
		strWk = txtEdgeOutKenHan.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 1800.0, False, 6, "�G�b�W���o_���o�͈�") <> 0 Then Return False
		txtEdgeOutKenHan.Text = strWk

		' �d���r�}�b�v�r��臒l_�d
		strWk = nudKizumapTempZyu.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "�d���r�}�b�v�r��臒l_�d") <> 0 Then Return False
		nudKizumapTempZyu.Text = strWk

		' �d���r�}�b�v�r��臒l_��
		strWk = nudKizumapTempChu.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "�d���r�}�b�v�r��臒l_��") <> 0 Then Return False
		nudKizumapTempChu.Text = strWk

		' �d���r�}�b�v�r��臒l_�y
		strWk = nudKizumapTempKei.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 99, False, "�d���r�}�b�v�r��臒l_�y") <> 0 Then Return False
		nudKizumapTempKei.Text = strWk

		' ����臒l�́u �d > �� > �y �v�_���`�F�b�N
		nWkH = CInt(nudKizumapTempZyu.Text)                 ' �r��臒l �d
		nWkM = CInt(nudKizumapTempChu.Text)                 ' �r��臒l ��
		nWkL = CInt(nudKizumapTempKei.Text)                 ' �r��臒l �y
		If nWkH > nWkM AndAlso nWkM > nWkL Then
			' ����臒l�́u �d > �� > �y �v�_���`�F�b�N����
		Else
			' ���ُ͔͈͈�
			strMsg = "�y �d���r�}�b�v�\���ݒ� �r��臒l �z" & vbCrLf
			strMsg &= "�K�؂Ȓl�ł͂���܂���B" & vbCrLf
			strMsg &= "�r���̊֌W���u �d �� �� �� �y �v�ɂ��Ă��������B"
			MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
			Return False
		End If

		' �ݔ���_������__�㗬�J����
		strWk = nudCamera.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "�ݔ���_������__�㗬�J����") <> 0 Then Return False
		nudCamera.Text = strWk

		' �ݔ���_������__SPM
		strWk = nudSpm.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "�ݔ���_������__SPM") <> 0 Then Return False
		nudSpm.Text = strWk

		' �ݔ���_�\����__��������
		strWk = nudExaminingTableB.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "�ݔ���_�\����__��������") <> 0 Then Return False
		nudExaminingTableB.Text = strWk

		' �ݔ���_�\����__�\������
		strWk = nudExaminingTableT.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "�ݔ���_�\����__�\������") <> 0 Then Return False
		nudExaminingTableT.Text = strWk

		' �ݔ���_�\����__�V���[
		strWk = nudCutPos.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 999999, False, "�ݔ���_�\����__�V���[") <> 0 Then Return False
		nudCutPos.Text = strWk

		' PLG������r�ُ����l
		strWk = txtHikakuPlgHigh.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 100, False, 5, "�R�C������r�ُ����l_��r�pPLG") <> 0 Then Return False
		txtHikakuPlgHigh.Text = strWk

		' PLG�����\���ُ����l
		strWk = txtHyoziPlgHigh.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 100, False, 5, "�R�C������r�ُ����l_�\���pPLG") <> 0 Then Return False
		txtHyoziPlgHigh.Text = strWk

		' �v���R���R�C������r�ُ����l
		strWk = txtProcomHigh.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 100, False, 5, "�R�C������r�ُ����l_�v���R��") <> 0 Then Return False
		txtProcomHigh.Text = strWk

		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>

		' ���������׃K�C�_���X �m�F���{�Ԋu
		strWk = nudLongPd_GuidanceCheckInterval.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_GuidanceCheckInterval.Minimum), CLng(nudLongPd_GuidanceCheckInterval.Maximum), False, "���������׃K�C�_���X_�m�F���{�Ԋu") <> 0 Then Return False
		nudLongPd_GuidanceCheckInterval.Text = strWk

		' ���������׃K�C�_���X �F�`�J����͈� From
		strWk = nudLongPd_NearFurnaceRangeFrom.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_NearFurnaceRangeFrom.Minimum), CLng(nudLongPd_NearFurnaceRangeFrom.Maximum), False, "���������׃K�C�_���X_�F�`�J����͈� From") <> 0 Then Return False
		nudLongPd_NearFurnaceRangeFrom.Text = strWk

		' ���������׃K�C�_���X �F�`�J����͈� To
		strWk = nudLongPd_NearFurnaceRangeTo.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_NearFurnaceRangeTo.Minimum), CLng(nudLongPd_NearFurnaceRangeTo.Maximum), False, "���������׃K�C�_���X_�F�`�J����͈� To") <> 0 Then Return False
		nudLongPd_NearFurnaceRangeTo.Text = strWk

		' ���������׃K�C�_���X �����s�b�`��ʕ\���p����
		strWk = nudLongPd_PeriodicPitchTolerance.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, CLng(nudLongPd_PeriodicPitchTolerance.Minimum), CLng(nudLongPd_PeriodicPitchTolerance.Maximum), False, "���������׃K�C�_���X_�����s�b�`��ʕ\���p����") <> 0 Then Return False
		nudLongPd_PeriodicPitchTolerance.Text = strWk

		' ���������׃K�C�_���X �K�C�_���X���b�Z�[�W
		strWk = txtLongPd_GuidanceMessage.Text
		If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "���������׃K�C�_���X_�K�C�_���X���b�Z�[�W") <> 0 Then Return False
		txtLongPd_GuidanceMessage.Text = strWk
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

		Return True
	End Function

	''' <summary>
	''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
	''' </summary>
	''' <remarks></remarks>
	Private Sub Data_Get()
		Dim drow As DataRow     ' �f�[�^�s

		' ��L�[���w�肵�Ď��o����Ă�̂ňꌏ�����Ȃ�
		drow = m_DS.Tables(0).Select()(0)

		' �Z�b�g
		drow.Item("RAS����F") = lblNormalColor.BackColor.ToArgb                        ' RAS����F
		drow.Item("RAS�ُ�F") = lblAbnormalColor.BackColor.ToArgb                      ' RAS�ُ�F
		drow.Item("RAS���ӐF") = lblCautionColor.BackColor.ToArgb                       ' RAS���ӐF
		drow.Item("LOG�\������") = CInt(nudDispCount.Text)                              ' LOG�\������
		drow.Item("LOG�C�x���g�F") = lblEventColor.ForeColor.ToArgb                     ' LOG�C�x���g�F
		drow.Item("LOG�d�̏�F") = lblZyuColor.ForeColor.ToArgb                         ' LOG�d�̏�F
		drow.Item("LOG�y�̏�F") = lblKeiColor.ForeColor.ToArgb                         ' LOG�y�̏�F
		drow.Item("�T����ʕ\����ԏ�") = CInt(nudDispSmall.Text)                       ' �T����ʕ\����ԏ�
		drow.Item("�T����ʕ\����ԑ�") = CInt(nudDispBig.Text)                         ' �T����ʕ\����ԑ�
		drow.Item("�E�H�[�}�ގw��ʒu") = CInt(cmbWormPos.Text)                         ' �E�H�[�}�ގw��ʒu
		drow.Item("Jpeg�؂�o���摜�i��") = CInt(nudJpegGazo.Text)                      ' Jpeg�؂�o���摜�i��
		drow.Item("�J�������x臒l") = CInt(nudCamTemp.Text)                             ' �J�������x臒l

		drow.Item("�G�b�W������_���{���") = cmbEdgType.SelectedIndex                   ' �G�b�W������_���{���
		drow.Item("�G�b�W������_���o�͈�") = CDbl(txtEdgeKenHan.Text)                   ' �G�b�W������_���o�͈�
		drow.Item("�G�b�W������_���J�o���[���o�͈�") = CDbl(txtEdgeReCov.Text)          ' �G�b�W������_���J�o���[���o�͈�
		drow.Item("�G�b�W������_�����e�덷") = CDbl(txtEdgeHabaGoso.Text)             ' �G�b�W������_�����e�덷
		drow.Item("�G�b�W������_�O�t���[���Ƃ̋��e�덷") = CDbl(txtEdgeFrmGoso.Text)    ' �G�b�W������_�O�t���[���Ƃ̋��e�덷
		drow.Item("�G�b�W������_�n�ڌ�s����") = CInt(nudEdgeFukantai.Text)             ' �G�b�W������_�n�ڌ�s����
		drow.Item("�G�b�W������_�G�b�W���o�ُ�FF���M") = CInt(nudEdgeFF.Text)           ' �G�b�W������_�G�b�W���o�ُ�FF���M

		drow.Item("�G�b�W���o_���{���") = cmbEdgOutType.SelectedIndex                   ' �G�b�W���o_���{���
		drow.Item("�G�b�W���o_���o�͈�") = CDbl(txtEdgeOutKenHan.Text)                   ' �G�b�W���o_���o�͈�

		drow.Item("�d���r�}�b�v�w�i�F_�d") = lblMapZyuColor.BackColor.ToArgb            ' �d���r�}�b�v�w�i�F_�d
		drow.Item("�d���r�}�b�v�w�i�F_��") = lblMapChuColor.BackColor.ToArgb            ' �d���r�}�b�v�w�i�F_��
		drow.Item("�d���r�}�b�v�w�i�F_�y") = lblMapKeiColor.BackColor.ToArgb            ' �d���r�}�b�v�w�i�F_�y

		drow.Item("�d���r�}�b�v�r��臒l_�d") = CInt(nudKizumapTempZyu.Text)           ' �d���r�}�b�v�r��臒l_�d
		drow.Item("�d���r�}�b�v�r��臒l_��") = CInt(nudKizumapTempChu.Text)           ' �d���r�}�b�v�r��臒l_��
		drow.Item("�d���r�}�b�v�r��臒l_�y") = CInt(nudKizumapTempKei.Text)           ' �d���r�}�b�v�r��臒l_�y

		drow.Item("�ݔ���_������__�㗬�J����") = CInt(nudCamera.Text)                  ' �ݔ���_������__�㗬�J����
		drow.Item("�ݔ���_������__SPM") = CInt(nudSpm.Text)                            ' �ݔ���_������__SPM
		drow.Item("�ݔ���_�\����__��������") = CInt(nudExaminingTableB.Text)           ' �ݔ���_�\����__��������
		drow.Item("�ݔ���_�\����__�\������") = CInt(nudExaminingTableT.Text)           ' �ݔ���_�\����__�\������
		drow.Item("�ݔ���_�\����__�V���[") = CInt(nudCutPos.Text)                      ' �ݔ���_�\����__�V���[

		'�S���摜
		If cmbImageSaveR.Text = "�L��" Then
			drow.Item("�S���摜_�ۑ�_R") = 1
		Else
			drow.Item("�S���摜_�ۑ�_R") = 0
		End If
		If cmbImageSaveG.Text = "�L��" Then
			drow.Item("�S���摜_�ۑ�_G") = 1
		Else
			drow.Item("�S���摜_�ۑ�_G") = 0
		End If
		If cmbImageSaveB.Text = "�L��" Then
			drow.Item("�S���摜_�ۑ�_B") = 1
		Else
			drow.Item("�S���摜_�ۑ�_B") = 0
		End If
		If cmbImageSaveRB.Text = "�L��" Then
			drow.Item("�S���摜_�ۑ�_��") = 1
		Else
			drow.Item("�S���摜_�ۑ�_��") = 0
		End If


		drow.Item("PLG������r�ُ����l") = CDbl(txtHikakuPlgHigh.Text)                     ' PLG������r�ُ����l
		drow.Item("PLG�����\���ُ����l") = CDbl(txtHyoziPlgHigh.Text)                      ' PLG�����\���ُ����l
		drow.Item("�v���R���R�C������r�ُ����l") = CDbl(txtProcomHigh.Text)               ' �v���R���R�C������r�ُ����l

		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		' ���������׃K�C�_���X
		If cmbLongPd_GuidanceAvailable.Text = "�L��" Then
			drow.Item("LongPd_GuidanceAvailable") = 1
		Else
			drow.Item("LongPd_GuidanceAvailable") = 0
		End If

		drow.Item("LongPd_GuidanceCheckInterval") = CInt(nudLongPd_GuidanceCheckInterval.Text)
		drow.Item("LongPd_NearFurnaceRangeFrom") = CInt(nudLongPd_NearFurnaceRangeFrom.Text)
		drow.Item("LongPd_NearFurnaceRangeTo") = CInt(nudLongPd_NearFurnaceRangeTo.Text)
		drow.Item("LongPd_PeriodicPitchTolerance") = CInt(nudLongPd_PeriodicPitchTolerance.Text)

		drow.Item("LongPd_GuidanceForeColor") = CInt(lblLongPd_GuidanceMessagePreview.ForeColor.ToArgb)
		drow.Item("LongPd_GuidanceBackColor") = CInt(lblLongPd_GuidanceMessagePreview.BackColor.ToArgb)
		drow.Item("LongPd_GuidanceMessage") = CStr(txtLongPd_GuidanceMessage.Text)
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
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

			Using cbWk As New SqlClient.SqlCommandBuilder()

				'�R�}���h�A�_�v�^���Z�b�g���āA�X�V
				cbWk.DataAdapter = m_Adapter
				m_Adapter.Update(m_DS)

			End Using

			'�R�~�b�g
			transact.Commit()

			'����ɓo�^���ꂽ�ꍇ�̓��[�����M
			modMain.Send_Main(CH_PARAM_SETTIG_COMMON)
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
			If m_Adapter.SelectCommand.Connection.State = ConnectionState.Open Then
				m_Adapter.SelectCommand.Connection.Close()
			End If

			If Not transact Is Nothing Then
				transact.Dispose()
			End If
		End Try
	End Function


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
		Next

		Return retColor
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �F�ύX����
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �F�I���_�C�A���O��\�����A�{�^���̐F��ύX
	''' </summary>
	''' <param name="objColor">�F�ύX�Ώۂ̃��x���܂��̓{�^��</param>
	''' <param name="bFont">�F�ύX�Ώۂ��������w�i�� True:���� False:�w�i</param>
	''' <remarks></remarks>
	Private Sub subGetDlgColor(ByVal objColor As Control, ByVal bFont As Boolean)

		'�F�I���_�C�A���O�̐ݒ�
		'�F�̍쐬���g�p���Ȃ�
		dlgColor.AllowFullOpen = False

		If bFont Then
			'�F�I���_�C�A���O�̑I��F���A�{�^���̕����F�i�O�i�F�j�ɂ���
			dlgColor.Color = objColor.ForeColor
		Else
			'�F�I���_�C�A���O�̑I��F���A�{�^���̔w�i�F�ɂ���
			dlgColor.Color = objColor.BackColor
		End If

		' �F�I���_�C�A���O��ok�{�^���������ꂽ��A�{�^���̐F��ύX���� 
		If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
			If bFont Then
				'�I��F���{�^���̕����F��
				objColor.ForeColor = dlgColor.Color
			Else
				'�I��F���{�^���̔w�i�F��
				objColor.BackColor = dlgColor.Color
			End If
		End If

		dlgColor.Dispose()
	End Sub

	''' <summary>
	''' �C�x���g�����F�ύX�{�^���������Ƀ��x���̕����F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnEventColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEventColor.Click
		'�F�I���_�C�A���O��\�����A���x���̕����F��ύX
		subGetDlgColor(lblEventColor, True)
	End Sub

	''' <summary>
	''' �y�̏ᕶ���F�ύX�{�^���������Ƀ��x���̕����F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnKeiColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKeiColor.Click
		'�F�I���_�C�A���O��\�����A���x���̕����F��ύX
		subGetDlgColor(lblKeiColor, True)
	End Sub

	''' <summary>
	''' �d�̏ᕶ���F�ύX�{�^���������Ƀ��x���̕����F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnZyuColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnZyuColor.Click
		'�F�I���_�C�A���O��\�����A���x���̕����F��ύX
		subGetDlgColor(lblZyuColor, True)
	End Sub

	''' <summary>
	''' ����F�ύX�{�^���������Ƀ��x���̔w�i�F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnNormalColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnNormalColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblNormalColor, False)
	End Sub

	''' <summary>
	''' ���ӐF�ύX�{�^���������Ƀ��x���̔w�i�F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnCautionColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCautionColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblCautionColor, False)
	End Sub

	''' <summary>
	''' �ُ�F�ύX�{�^���������Ƀ��x���̔w�i�F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAbnormalColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAbnormalColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblAbnormalColor, False)
	End Sub

	''' <summary>
	''' �d���r�}�b�v�w�i�F_�y �ύX�{�^���������Ƀ��x���̔w�i�F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMapKeiColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMapKeiColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblMapKeiColor, False)
	End Sub

	''' <summary>
	''' �d���r�}�b�v�w�i�F_�� �ύX�{�^���������Ƀ��x���̔w�i�F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMapChuColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMapChuColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblMapChuColor, False)
	End Sub

	''' <summary>
	''' �d���r�}�b�v�w�i�F_�d �ύX�{�^���������Ƀ��x���̔w�i�F��ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMapZyuColor_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMapZyuColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblMapZyuColor, False)
	End Sub

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	''' <summary>
	''' ���������׃K�C�_���X���b�Z�[�W �O�i�F
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnLongPd_GuidanceFrontColor_Click(sender As Object, e As EventArgs) Handles btnLongPd_GuidanceForeColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblLongPd_GuidanceMessagePreview, True)
	End Sub

	''' <summary>
	''' ���������׃K�C�_���X���b�Z�[�W �w�i�F
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnLongPd_GuidanceBackColor_Click(sender As Object, e As EventArgs) Handles btnLongPd_GuidanceBackColor.Click
		'�F�I���_�C�A���O��\�����A���x���̔w�i�F��ύX
		subGetDlgColor(lblLongPd_GuidanceMessagePreview, False)
	End Sub

	''' <summary>
	''' ���������׃K�C�_���X���b�Z�[�W ���e�ύX
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub txtLongPd_GuidanceMessage_TextChanged(sender As Object, e As EventArgs) Handles txtLongPd_GuidanceMessage.TextChanged
		lblLongPd_GuidanceMessagePreview.Text = txtLongPd_GuidanceMessage.Text
	End Sub
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	''' <summary>
	''' �X�V�{�^���̗L��/����
	''' </summary>
	''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
	''' <remarks></remarks>
	Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

		btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

	End Sub
End Class

