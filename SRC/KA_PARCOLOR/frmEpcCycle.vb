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

Public Class frmEpcCycle
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�i�����r�ݒ�L��)
    Private Enum EM_DATACOLUMN_GRID_ID
        emTxt_No = 0                ' �e�L�X�g�{�b�N�X  No
        emTxt_Name                  ' �R���{�{�b�N�X    �r��
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
		'emChk_Grade1                ' �`�F�b�N�{�b�N�X  �O���[�h�P
		'emChk_Grade2                ' �`�F�b�N�{�b�N�X  �O���[�h�Q
		'emChk_Grade3                ' �`�F�b�N�{�b�N�X  �O���[�h�R
		'emChk_Grade4                ' �`�F�b�N�{�b�N�X  �O���[�h�S
		'emChk_Grade5                ' �`�F�b�N�{�b�N�X  �O���[�h�T
		'emChk_Grade6                ' �`�F�b�N�{�b�N�X  �O���[�h�U
		'emChk_Grade7                ' �`�F�b�N�{�b�N�X  �O���[�h�V
		'emChk_Grade8                ' �`�F�b�N�{�b�N�X  �O���[�h�W

		emChk_Grade                 ' �`�F�b�N�{�b�N�X  �O���[�h�W
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
    End Enum

    ' �f�[�^�O���b�h�r���[�̗�i�����r�ݒl)
    Private Enum EM_PERIOD_GRID_COL_ID
        emTxt_Title = 0             ' �e�L�X�g�{�b�N�X  �^�C�g��
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
		'emTxt_Grade1                ' �e�L�X�g�{�b�N�X  �O���[�h1
		'emTxt_Grade2                ' �e�L�X�g�{�b�N�X  �O���[�h2
		'emTxt_Grade3                ' �e�L�X�g�{�b�N�X  �O���[�h3
		'emTxt_Grade4                ' �e�L�X�g�{�b�N�X  �O���[�h4
		'emTxt_Grade5                ' �e�L�X�g�{�b�N�X  �O���[�h5
		'emTxt_Grade6                ' �e�L�X�g�{�b�N�X  �O���[�h6
		'emTxt_Grade7                ' �e�L�X�g�{�b�N�X  �O���[�h7
		'emTxt_Grade8                ' �e�L�X�g�{�b�N�X  �O���[�h8 

		emTxt_Grade                 ' �e�L�X�g�{�b�N�X  �O���[�h
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
    End Enum

    ' �f�[�^�O���b�h�r���[�̗�i�����r�ݒl)
    Private Enum EM_PERIOD_GRID_ROW_ID
        emTxt_HistThr = 0           ' �e�L�X�g�{�b�N�X  �r�Ԋu�q�X�g�O����臒l
        emTxt_Lpitch                ' �e�L�X�g�{�b�N�X  �r�Ԋu�v�Z�s�b�`
        emTxt_Nthr                  ' �e�L�X�g�{�b�N�X  �����臒l
        emTxt_Wpitch                ' �e�L�X�g�{�b�N�X  �������s�b�`
        emTxt_Min                   ' �e�L�X�g�{�b�N�X  �ŏ�����
        emTxt_Max                   ' �e�L�X�g�{�b�N�X  �ő����
        emTxt_KeepLen               ' �e�L�X�g�{�b�N�X  �ێ�����
        emTxt_Weight                ' �e�L�X�g�{�b�N�X  �������␳�W��
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
		emTxt_FrontEnableLen       '�e�L�X�g�{�b�N�X  F���L�����
		emTxt_TailEnableLen        '�e�L�X�g�{�b�N�X  T���L�����
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
    End Enum

	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
	'Private Const mc_TypeMsc_ColMax As Integer = 8 + 2                ' �����r�L���ݒ� �ő�� (8bit)
	'Private Const mc_Period_RowMax As Integer = 8                     ' �����r�ݒ�ő�s��
	'Private Const mc_Period_ColMax As Integer = 8                     ' �O���[�h�ő�ʐ� (8��Œ�)

	Private Const mc_TypeMsc_ColMax As Integer = MAX_CYCLE + 2        ' �����r�L���ݒ� �ő�� (16bit)
	Private Const mc_Period_RowMax As Integer = 10                     ' �����r�ݒ�ő�s��
	Private Const mc_Period_ColMax As Integer = MAX_CYCLE             ' �O���[�h�ő�ʐ� (16��Œ�)
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

    ' �I���{�^���̒萔
    Private Enum EM_SELECT_MODE
        emSelect = 0                     ' �S�I��
        emCancel                         ' �S����
    End Enum

    '����dgv�p�̍s�^�C�g���ݒ�A �\����2�s�ɂȂ�悤�ɉ��s���Ă܂�
'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
    'Private m_strPeriodRowHead() As String = {"�r�Ԋu�q�X�g�O����臒l" & vbNewLine & "(0 - 100) [%]",
    '   "�r�Ԋu�v�Z�s�b�`" & vbNewLine & "(5 - 1000) [mm]",
    '   "�����臒l" & vbNewLine & "(0 - 1000) [��]",
    '   "�������s�b�`" & vbNewLine & "(1 - 500) [pixel]",
    '   "�ŏ�����" & vbNewLine & "(0 - 2000) [mm]",
    '   "�ő����" & vbNewLine & "(0 - 10000000) [mm]",
    '   "�ێ�����" & vbNewLine & "(0 - 10000) [m]",
    '   "�������␳�W��" & vbNewLine & "(0 - 1.0)"}

	Private m_strPeriodRowHead() As String = {"�r�Ԋu�q�X�g�O����臒l" & vbNewLine & "(0 - 100) [%]",
	   "�r�Ԋu�v�Z�s�b�`" & vbNewLine & "(5 - 1000) [mm]",
	   "�����臒l" & vbNewLine & "(0 - 1000) [��]",
	   "�������s�b�`" & vbNewLine & "(1 - 500) [pixel]",
	   "�ŏ�����" & vbNewLine & "(0 - 10000000) [mm]",
	   "�ő����" & vbNewLine & "(0 - 10000000) [mm]",
	   "�ێ�����" & vbNewLine & "(0 - 10000) [m]",
	   "�������␳�W��" & vbNewLine & "(0 - 1.0)",
	   "F���L�����" & vbNewLine & "(0 - 20000)[m]",
	   "T���L�����" & vbNewLine & "(0 - 20000)[m]"}
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

    ''�{�^��dgv�p�̒萔�ݒ�{�^���p��dgv���r��dgv�Ɨ�𑵂���
    Private m_strBtnName() As String = {"�S�I��", "�S����"}

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter_TypeMsc As SqlClient.SqlDataAdapter = Nothing        ' �A�_�v�^�[
    Private m_Adapter_Period As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[

    Private m_DS_TypeMsc As New DataSet                                    ' ��ڑ��^�e�[�u��
    Private m_DS_Period As New DataSet                                     ' ��ڑ��^�e�[�u��

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
    Private Sub frmEpcCycle_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
			Case 6								' F07 ��ʈ��
				Call PrintOut()
			Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�����r�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
				modMain.ChgProject()			' F09 KA_PARSET�֐ؑ֏���
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
    Private Sub frmEpcCycle_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DS_TypeMsc.Dispose()
        m_DS_Period.Dispose()

        If Not m_Adapter_TypeMsc Is Nothing Then
            m_Adapter_TypeMsc.Dispose()
            m_Adapter_TypeMsc = Nothing
        End If

        If Not m_Adapter_Period Is Nothing Then
            m_Adapter_Period.Dispose()
            m_Adapter_Period = Nothing
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
    Private Sub frmEpcCycle_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcCycle              ' �����r�ݒ��ʌďo

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �����r�ݒ�L���ꗗ �ݒ�
        dgvInitialize_TypeMsc(dgvTypeMscData)

        ' �����r�ݒ�l�ꗗ �ݒ�
        dgvInitialize_Period(dgvPeriodData)

        ' �S�I���{�^��
        dgvInitialize_SelectBtn(dgvTypeMscBtnAll, EM_SELECT_MODE.emSelect)

        ' �S�I������
        dgvInitialize_SelectBtn(dgvTypeMscBtnClear, EM_SELECT_MODE.emCancel)

        DoubleBuffered = True

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        Else
            ' �f�[�^�\������
            Data_Disp()
        End If

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvTypeMscData.DataGridViewSelectionRowColorChange(g_ColorSelection)
        dgvTypeMscData.DataGridViewSelectionRowColorChange(g_ColorSelection)

        ' �X�V�{�^���̗L��/����
        Call UpdateEnabled(g_bPassFlg)

        frmMsg.Dispose()
		frmMsg = Nothing

		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>        
		' �c�X�N���[���o�[�\���L��Ȃ�΁Adgv�̕����L����
		ChangeDgvWidth()
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
    End Sub

    ''' <summary>
    ''' �f�B�X�|�[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEpcCycle_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

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

        ' �����r�ݒ�L���p�f�[�^�擾
        ' SQL������
        strSQL = "SELECT EdasysID"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", �D��x"
		strSQL &= ", PeriodProcMask"
		'strSQL &= ", ISNULL(PeriodProcMask,0) as PeriodProcMask"
		strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_TypeMsc, m_Adapter_TypeMsc, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' �����r�ݒ�l�f�[�^�擾
        '// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
		'strSQL = "SELECT [Key]"
		'      strSQL &= ", PeriodHistThr1, PeriodHistThr2"
		'      strSQL &= ", PeriodHistThr3, PeriodHistThr4"
		'      strSQL &= ", PeriodHistThr5, PeriodHistThr6"
		'      strSQL &= ", PeriodHistThr7, PeriodHistThr8"
		'      strSQL &= ", PeriodLpitch1, PeriodLpitch2"
		'      strSQL &= ", PeriodLpitch3, PeriodLpitch4"
		'      strSQL &= ", PeriodLpitch5, PeriodLpitch6"
		'      strSQL &= ", PeriodLpitch7, PeriodLpitch8"
		'      strSQL &= ", PeriodNthr1, PeriodNthr2"
		'      strSQL &= ", PeriodNthr3, PeriodNthr4"
		'      strSQL &= ", PeriodNthr5, PeriodNthr6"
		'      strSQL &= ", PeriodNthr7, PeriodNthr8"
		'      strSQL &= ", PeriodWpitch1, PeriodWpitch2"
		'      strSQL &= ", PeriodWpitch3, PeriodWpitch4"
		'      strSQL &= ", PeriodWpitch5, PeriodWpitch6"
		'      strSQL &= ", PeriodWpitch7, PeriodWpitch8"
		'      strSQL &= ", PeriodMin1, PeriodMin2"
		'      strSQL &= ", PeriodMin3, PeriodMin4"
		'      strSQL &= ", PeriodMin5, PeriodMin6"
		'      strSQL &= ", PeriodMin7, PeriodMin8"
		'      strSQL &= ", PeriodMax1, PeriodMax2"
		'      strSQL &= ", PeriodMax3, PeriodMax4"
		'      strSQL &= ", PeriodMax5, PeriodMax6"
		'      strSQL &= ", PeriodMax7, PeriodMax8"
		'      strSQL &= ", PeriodKeepLen1, PeriodKeepLen2"
		'      strSQL &= ", PeriodKeepLen3, PeriodKeepLen4"
		'      strSQL &= ", PeriodKeepLen5, PeriodKeepLen6"
		'      strSQL &= ", PeriodKeepLen7, PeriodKeepLen8"
		'      strSQL &= ", PeriodWeight1, PeriodWeight2"
		'      strSQL &= ", PeriodWeight3, PeriodWeight4"
		'      strSQL &= ", PeriodWeight5, PeriodWeight6"
		'      strSQL &= ", PeriodWeight7, PeriodWeight8"
		'      strSQL &= " FROM " & DB_PARAM_COMMON
		'      strSQL &= " WHERE [Key] = " & CStr(PARAM_COMMON_KEY)

		strSQL = " SELECT "
		strSQL &= "     id "
		strSQL &= "    ,PeriodHistThr "
		strSQL &= "    ,PeriodLpitch "
		strSQL &= "    ,PeriodNthr "
		strSQL &= "    ,PeriodWpitch "
		strSQL &= "    ,PeriodMin "
		strSQL &= "    ,PeriodMax "
		strSQL &= "    ,PeriodKeepLen "
		strSQL &= "    ,PeriodWeight "
		strSQL &= "    ,PeriodFrontEnableLen "
		strSQL &= "    ,PeriodTailEnableLen "
		strSQL &= " FROM " & DB_PARAM_CYCLE
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Period, m_Adapter_Period, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_COMMON & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' �擾�f�[�^����ʂɕ\������B
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        ' �S�I���A�S�����{�^���ݒ�
        Call Disp_SelectBtn(dgvTypeMscBtnAll)
        Call Disp_SelectBtn(dgvTypeMscBtnClear)

        ' �����r�ݒ�L���p�f�[�^�\��
        Call Db_Disp_TypeMsc()

        ' �����r�ݒ�l�f�[�^�\��
        Call Db_Disp_Period()

    End Sub

    ''' <summary>
    ''' �r��f�[�^dgv�̃`�F�b�N�{�b�N�X�̃`�F�b�N��ύX�������@�F��ς���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTypeMscData.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        '�^�C�g�����E�̗� �w�b�_�[�ȊO
        If e.ColumnIndex > EM_DATACOLUMN_GRID_ID.emTxt_Name And e.RowIndex >= 0 Then
            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                '�`�F�b�N��t������
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = g_ColorSetting
            Else
                '�`�F�b�N���O������
                dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = Color.Empty
            End If
        End If

    End Sub

    ''' <summary>
    ''' �S�I��dgv�{�^��������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscBtnAll_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTypeMscBtnAll.CellContentClick

        '�{�^����łȂ���΂Ȃɂ����Ȃ�
        If e.ColumnIndex <= 0 Then Return

        '��̃`�F�b�N��Ԃ�S�I���ɂ���
        ChangeTypeMscCheckBox(dgvTypeMscData, e.ColumnIndex, True)

    End Sub

    ''' <summary>
    ''' �S����dgv�{�^��������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscBtnClear_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvTypeMscBtnClear.CellContentClick

        '�{�^����łȂ���΂Ȃɂ����Ȃ�
        If e.ColumnIndex <= 0 Then Return

        '��̃`�F�b�N��Ԃ�S�����ɂ���
        ChangeTypeMscCheckBox(dgvTypeMscData, e.ColumnIndex, False)

    End Sub

    ''' <summary>
    ''' �Ώ�dgv�̑Ώۗ�̃`�F�b�N�{�b�N�X�̃`�F�b�N��Ԃ�ύX����
    ''' </summary>
    ''' <param name="dgv">�Ώۂ�dgv</param>
    ''' <param name="nIndex">��C���f�b�N�X</param>
    ''' <param name="bChecked">�`�F�b�N���</param>
    ''' <remarks></remarks>
    Private Sub ChangeTypeMscCheckBox(ByVal dgv As DataGridView, ByVal nIndex As Integer, ByVal bChecked As Boolean)
        Dim ii As Integer   '�J�E���^

        '�S�s�̃`�F�b�N��Ԃ�ύX
        For ii = 0 To dgv.RowCount - 1
            dgv(nIndex, ii).Value = bChecked
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
        Dim iColIx As Integer                                   ' ��INDEX

        ' �����r�L���ݒ�
        ' �`�F�b�N�{�b�N�X�݂̂Ȃ̂œ��Ƀ`�F�b�N�͂��Ȃ�

        ' �����r�ݒ�l
        ' �񐔕����[�v
        For ii As Integer = 1 To dgvPeriodData.ColumnCount - 1

            iColIx = ii

            ' �r�Ԋu�q�X�g�O����臒l
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 100, False, "�r�Ԋu�q�X�g�O����臒l") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value = strWk

            ' �r�Ԋu�v�Z�s�b�`
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 5, 1000, False, 10, "�r�Ԋu�v�Z�s�b�`", True) <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value = strWk

            ' �����臒l
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 1000, False, "�����臒l") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value = strWk

            ' �������s�b�`
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 500, False, "�������s�b�`") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value = strWk

            ' �ŏ�����
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value)
            '// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 2000, False, 10, "�ŏ�����", True) <> 0 Then Return False
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 10000000, False, 10, "�ŏ�����", True) <> 0 Then Return False
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value = strWk

            ' �ő����
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value)
            'If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 5000, False, 10, "�ő����", True) <> 0 Then Return False
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 10000000, False, 10, "�ő����", True) <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value = strWk

            ' �ێ�����
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 10000, False, "�ێ�����") <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value = strWk

            ' �������␳�W��
            strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 1.0, False, 10, "�������␳�W��", True) <> 0 Then Return False
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value = strWk

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			' F���L�����
			strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 20000, False, "F���L�����") <> 0 Then Return False
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value = strWk

			' T���L�����
			strWk = CStr(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value)
			If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 20000, False, "T���L�����") <> 0 Then Return False
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value = strWk
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
        Next ii

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

        Data_Get_TypeMsc()                  '�����r�ݒ�L�� �f�[�^�ݒ�
        Data_Get_Period()                   '�����r�ݒ�l   �f�[�^�ݒ�

    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g �����r�ݒ�L��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get_TypeMsc()
        Dim strWhere As String                          ' WHERE��ɑ����̏�������
        Dim drow As DataRow
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>> 
		'Dim BitFlag(0 To 7) As Integer                  ' BIT�ϊ���̔z��
		'Dim bFlag As Byte

		Dim BitFlag(0 To MAX_CYCLE - 1) As Integer                  ' BIT�ϊ���̔z��
		Dim nFlag As Integer
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
        Dim iStartIx As Integer

        ' ����No�����[�v
        For ii As Integer = 0 To dgvTypeMscData.Rows.Count - 1
            strWhere = String.Format("ID='{0}' ", ii)                     ' WHERE��ɑ����̏�������
            drow = m_DS_TypeMsc.Tables(0).Select(strWhere)(0)             ' ID����X�V�p�f�[�^�擾

            '''' �����`�F�b�N��Z�b�g

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'bFlag = 0
			'         iStartIx = EM_DATACOLUMN_GRID_ID.emChk_Grade1
			'         For jj As Integer = 0 To 7
			nFlag = 0
			iStartIx = EM_DATACOLUMN_GRID_ID.emChk_Grade
			For jj As Integer = 0 To MAX_CYCLE - 1
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

                If CInt(dgvTypeMscData(iStartIx, ii).Value) = 0 Then
                    BitFlag(jj) = 0      '����
                Else
                    BitFlag(jj) = 1      '�L��
                End If
                iStartIx = iStartIx + 1
            Next jj

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>> 
			'' �ݒ�t���O���m�F���āA�r�b�g�ϊ����s���B
			'IntArreyToBit(bFlag, UBound(BitFlag), BitFlag)
			'
			'' �Z�b�g
			'drow.Item("PeriodProcMask") = bFlag

            ' �ݒ�t���O���m�F���āA�r�b�g�ϊ����s���B
			IntArrayToBit(nFlag, UBound(BitFlag), BitFlag)

            ' �Z�b�g
			drow.Item("PeriodProcMask") = nFlag
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
        Next ii
    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g �����r�ݒ�l
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get_Period()
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>> 
		'Dim drow As DataRow
		'Dim strWhere As String                              ' WHERE��ɑ����̏�������
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
        Dim sDbItemName As String                           ' DB���ږ���
        Dim iColIx As Integer                               ' ��INDEX

		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
		'strWhere = String.Format("[Key]='{0}' ", 0)         ' WHERE��ɑ����̏�������
		'drow = m_DS_Period.Tables(0).Select(strWhere)(0)    ' KEY����X�V�p�f�[�^�擾
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

        For ii As Integer = 1 To mc_Period_ColMax
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			Dim strWhere As String = String.Format("[ID]='{0}' ", ii)       ' WHERE��ɑ����̏�������
			Dim drow As DataRow = m_DS_Period.Tables(0).Select(strWhere)(0) ' PCommon�e�[�u�����Q�Ƃ���̂�1�s�����Ȃ��͂�
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

            iColIx = ii

            ' �ݒ�l �r�Ԋu�q�X�g�O����臒l
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodHistThr" & iColIx
			sDbItemName = "PeriodHistThr"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------            
			drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value)           'PeriodHistThr

            ' �ݒ�l �r�Ԋu�v�Z�s�b�`
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodLpitch" & iColIx
			sDbItemName = "PeriodLpitch"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value)           'PeriodLpitch

            ' �ݒ�l �����臒l
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodNthr" & iColIx
			sDbItemName = "PeriodNthr"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value)              'PeriodNthr

            ' �ݒ�l �������s�b�`
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodWpitch" & iColIx
			sDbItemName = "PeriodWpitch"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value)            'PeriodWpitch

            ' �ݒ�l �ŏ�����
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodMin" & iColIx
			sDbItemName = "PeriodMin"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value)              'PeriodMin

            ' �ݒ�l �ő����
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodMax" & iColIx
			sDbItemName = "PeriodMax"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value)               'PeriodMax

            ' �ݒ�l �ێ�����
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodKeepLen" & iColIx
			sDbItemName = "PeriodKeepLen"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            drow.Item(sDbItemName) = CInt(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value)           'PeriodKeepLen

            ' �ݒ�l �������␳�W��
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodWeight" & iColIx
			sDbItemName = "PeriodWeight"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            drow.Item(sDbItemName) = CDbl(dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value)            'PeriodWeight

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			' �ݒ�l F���L�����
			sDbItemName = "PeriodFrontEnableLen"
			drow.Item(sDbItemName) = dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value    'PeriodFrontEnableLen

			' �ݒ�l T���L�����
			sDbItemName = "PeriodTailEnableLen"
			drow.Item(sDbItemName) = dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value     'PeriodTailEnableLen
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
        Next ii
    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim transactTypeMsc As SqlClient.SqlTransaction = Nothing   '�g�����U�N�V��������
        Dim transactPeriod As SqlClient.SqlTransaction = Nothing    '�g�����U�N�V��������

        Try
            ' DB�ڑ�
            m_Adapter_TypeMsc.SelectCommand.Connection.Open()
            m_Adapter_Period.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transactTypeMsc = m_Adapter_TypeMsc.SelectCommand.Connection.BeginTransaction()
            m_Adapter_TypeMsc.SelectCommand.Transaction = transactTypeMsc
            transactPeriod = m_Adapter_Period.SelectCommand.Connection.BeginTransaction()
            m_Adapter_Period.SelectCommand.Transaction = transactPeriod

            Using cbWk As New SqlClient.SqlCommandBuilder()

                'TypeName�̃A�_�v�^���Z�b�g���čX�V
                cbWk.DataAdapter = m_Adapter_TypeMsc
                m_Adapter_TypeMsc.Update(m_DS_TypeMsc)

                'ParamCommon�̃A�_�v�^���Z�b�g���čX�V
                cbWk.DataAdapter = m_Adapter_Period
                m_Adapter_Period.Update(m_DS_Period)

            End Using

            '�R�~�b�g
            transactTypeMsc.Commit()
            transactPeriod.Commit()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_CYCLE)                             '�����ւ̒ʒm��ύX���鎖
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[ [" & ex.Message & "] " & Me.Text)

            '' �G���[�Ȃ烍�[���o�b�N���ďI��
            If Not transactTypeMsc Is Nothing Then
                transactTypeMsc.Rollback()
            End If

            If Not transactPeriod Is Nothing Then
                transactPeriod.Rollback()
            End If

            Return False
        Finally
            ' ��n��
            If m_Adapter_TypeMsc.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter_TypeMsc.SelectCommand.Connection.Close()
            End If

            If m_Adapter_Period.SelectCommand.Connection.State = ConnectionState.Open Then
                m_Adapter_Period.SelectCommand.Connection.Close()
            End If

            If Not transactTypeMsc Is Nothing Then
                transactTypeMsc.Dispose()
            End If

            If Not transactPeriod Is Nothing Then
                transactPeriod.Dispose()
            End If
        End Try
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���̑��̊֐�
    '/ /////////////////////////////////////////////////////////////////////////////////
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
	'''' <summary>
	'''' BIT�ϊ����� BIT -> INT�^�̔z��
	'''' </summary>
	'''' <param name="bFlag">BIT�f�[�^</param>
	'''' <param name="IntFlag">INT�̔z��f�[�^</param>
	'''' <remarks></remarks>
	'Private Sub BitToIntArrey(ByVal bFlag As Byte, ByVal IntFlag() As Integer)
	'
	'
	'       If (bFlag And &H1) <> &H0 Then IntFlag(0) = 1
	'       If (bFlag And &H2) <> &H0 Then IntFlag(1) = 1
	'       If (bFlag And &H4) <> &H0 Then IntFlag(2) = 1
	'       If (bFlag And &H8) <> &H0 Then IntFlag(3) = 1
	'
	'       If (bFlag And &H10) <> &H0 Then IntFlag(4) = 1
	'       If (bFlag And &H20) <> &H0 Then IntFlag(5) = 1
	'       If (bFlag And &H40) <> &H0 Then IntFlag(6) = 1
	'       If (bFlag And &H80) <> &H0 Then IntFlag(7) = 1
	'
	'   End Sub
	'
	'   ''' <summary>
	'   ''' BIT�ϊ����� INT�^�̔z�� -> BIT
	'   ''' </summary>
	'   ''' <param name="bFlag">BIT�f�[�^</param>
	'   ''' <param name="iCnt">�z��</param>
	'   ''' <param name="IntFlag">INT�̔z��f�[�^</param>
	'   ''' <remarks></remarks>
	'   Private Sub IntArreyToBit(ByRef bFlag As Byte, ByVal iCnt As Integer, ByVal IntFlag() As Integer)
	'
	'       Dim bWork As UInteger
	'
	'       bWork = 0
	'       For ii As Integer = 0 To iCnt
	'           If (IntFlag(ii) = 1) Then bWork = bWork + CUInt(2 ^ ii) '
	'       Next ii
	'
	'       bFlag = CByte(bWork)
	'
	'   End Sub

    ''' <summary>
    ''' BIT�ϊ����� BIT -> INT�^�̔z��
    ''' </summary>
	''' <param name="nFlag">BIT�f�[�^(32bit)</param>
	''' <param name="nIntFlag">INT�̔z��f�[�^</param>
    ''' <remarks></remarks>
	Private Sub BitToIntArray(ByVal nFlag As Int32, ByVal nIntFlag() As Integer)
		If (nFlag And &H1) <> &H0 Then nIntFlag(0) = 1
		If (nFlag And &H2) <> &H0 Then nIntFlag(1) = 1
		If (nFlag And &H4) <> &H0 Then nIntFlag(2) = 1
		If (nFlag And &H8) <> &H0 Then nIntFlag(3) = 1

		If (nFlag And &H10) <> &H0 Then nIntFlag(4) = 1
		If (nFlag And &H20) <> &H0 Then nIntFlag(5) = 1
		If (nFlag And &H40) <> &H0 Then nIntFlag(6) = 1
		If (nFlag And &H80) <> &H0 Then nIntFlag(7) = 1

		If (nFlag And &H100) <> &H0 Then nIntFlag(8) = 1
		If (nFlag And &H200) <> &H0 Then nIntFlag(9) = 1
		If (nFlag And &H400) <> &H0 Then nIntFlag(10) = 1
		If (nFlag And &H800) <> &H0 Then nIntFlag(11) = 1

		If (nFlag And &H1000) <> &H0 Then nIntFlag(12) = 1
		If (nFlag And &H2000) <> &H0 Then nIntFlag(13) = 1
		If (nFlag And &H4000) <> &H0 Then nIntFlag(14) = 1
		If (nFlag And &H8000) <> &H0 Then nIntFlag(15) = 1
    End Sub

    ''' <summary>
    ''' BIT�ϊ����� INT�^�̔z�� -> BIT
    ''' </summary>
	''' <param name="nFlag">BIT�f�[�^(32bit)</param>
	''' <param name="nCnt">�z��</param>
	''' <param name="nIntFlag">INT�̔z��f�[�^</param>
    ''' <remarks></remarks>
	Private Sub IntArrayToBit(ByRef nFlag As Int32, ByVal nCnt As Integer, ByVal nIntFlag() As Integer)
		nFlag = 0
		For ii As Integer = 0 To nCnt
			If (nIntFlag(ii) = 1) Then nFlag = nFlag + CInt(2 ^ ii) '
        Next ii
    End Sub
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��ʕ\������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' DB�f�[�^��\������ �����r�ݒ�L��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Db_Disp_TypeMsc()

        Dim drow() As DataRow = m_DS_TypeMsc.Tables(0).Select()
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
		'Dim iMarkFlag(0 To 7) As Integer                       '�����r�ݒ� BIT�ϊ���̒l
		'Dim bFlags As Byte
		Dim iMarkFlag(0 To MAX_CYCLE - 1) As Integer                 '�����r�ݒ� BIT�ϊ���̒l        
		Dim nFlags As Integer
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

        ' ��ʂɕ\��
        dgvTypeMscData.RowCount = drow.Length                  ' ��{�I�Ƀ}�X�^�[�ɂ͐���ȃ��R�[�h��o�^�ς݂̂͂������A�ėp�I�ɂ����
        For ii As Integer = 0 To drow.Length - 1
            dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emTxt_No, ii).Value = CInt(drow(ii).Item("ID")) + 1    ' ID��0����Ȃ̂�1���Z����
            dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emTxt_Name, ii).Value = drow(ii).Item("Name")          ' �r��

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'For jj As Integer = 0 To 7
			For jj As Integer = 0 To MAX_CYCLE - 1
				'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

                iMarkFlag(jj) = 0
            Next jj
            ' �����r�ݒ� �iBIT�j�Őݒ肳��Ă���̂ŕϊ����Ă�邱��
			' BIT�ϊ��������쐬���邱��
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'If drow(ii).Item("PeriodProcMask") Is DBNull.Value Then
			'	bFlags = 0		'�r��
			'Else
			'	bFlags = CByte(drow(ii).Item("PeriodProcMask"))		'�r��
			'End If
			'
			'BitToIntArrey(bFlags, iMarkFlag)

			If drow(ii).Item("PeriodProcMask") Is DBNull.Value Then
				nFlags = 0      '�r��
			Else
				nFlags = CInt(drow(ii).Item("PeriodProcMask"))     '�r��
			End If

			BitToIntArray(nFlags, iMarkFlag)
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

			'�`�F�b�N�{�b�N�X�ݒ�
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade1, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade1 - 2)			' �`�F�b�N�{�b�N�X  �O���[�h�P
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade2, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade2 - 2)			' �`�F�b�N�{�b�N�X  �O���[�h�Q
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade3, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade3 - 2)			' �`�F�b�N�{�b�N�X  �O���[�h�R
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade4, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade4 - 2)			' �`�F�b�N�{�b�N�X  �O���[�h�S
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade5, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade5 - 2)			' �`�F�b�N�{�b�N�X  �O���[�h�T
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade6, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade6 - 2)			' �`�F�b�N�{�b�N�X  �O���[�h�U
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade7, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade7 - 2)			' �`�F�b�N�{�b�N�X  �O���[�h�V
			'dgvTypeMscData(EM_DATACOLUMN_GRID_ID.emChk_Grade8, ii).Value = iMarkFlag(EM_DATACOLUMN_GRID_ID.emChk_Grade8 - 2)            ' �`�F�b�N�{�b�N�X  �O���[�h�W

			For kk As Integer = 0 To MAX_CYCLE - 1
				Dim nColIndex As Integer = EM_DATACOLUMN_GRID_ID.emChk_Grade + kk
				Dim nFlagIndex As Integer = EM_DATACOLUMN_GRID_ID.emChk_Grade + kk - 2

				dgvTypeMscData(nColIndex, ii).Value = iMarkFlag(nFlagIndex)           ' �`�F�b�N�{�b�N�X  �O���[�h
			Next
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
		Next ii
    End Sub

    ''' <summary>
    ''' DB�f�[�^��\������ �����r�ݒ�l
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Db_Disp_Period()
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
		'Dim strWhere As String = String.Format("[Key]='{0}' ", 0)           ' WHERE��ɑ����̏�������
		'Dim drow() As DataRow = m_DS_Period.Tables(0).Select(strWhere)      ' PCommon�e�[�u�����Q�Ƃ���̂�1�s�����Ȃ��͂�
		'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
        Dim sDbItemName As String                                           ' DB���ږ���
        Dim iColIx As Integer                                               ' ��INDEX

        ' ��ʂɕ\��
        dgvPeriodData.RowCount = mc_Period_RowMax                           ' �s���ݒ�

        ' �s�^�C�g���ݒ�
        For ii As Integer = 0 To mc_Period_RowMax - 1
            dgvPeriodData(EM_PERIOD_GRID_COL_ID.emTxt_Title, ii).Value = m_strPeriodRowHead(ii)
        Next ii

        For ii As Integer = 0 To mc_Period_ColMax - 1
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			Dim strWhere As String = String.Format("[ID]='{0}' ", ii + 1)      ' WHERE��ɑ����̏�������
			Dim drow() As DataRow = m_DS_Period.Tables(0).Select(strWhere)  ' PCommon�e�[�u�����Q�Ƃ���̂�1�s�����Ȃ��͂�

			If drow Is Nothing OrElse drow.Length <= 0 Then
				Continue For
			End If
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

            iColIx = ii + 1
            ' �ݒ�l �r�Ԋu�q�X�g�O����臒l
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodHistThr" & iColIx
			sDbItemName = "PeriodHistThr"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_HistThr).Value = drow(0).Item(sDbItemName)            'PeriodHistThr

            ' �ݒ�l �r�Ԋu�v�Z�s�b�`
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodLpitch" & iColIx
			sDbItemName = "PeriodLpitch"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Lpitch).Value = drow(0).Item(sDbItemName)             'PeriodLpitch


            ' �ݒ�l �����臒l
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodNthr" & iColIx
			sDbItemName = "PeriodNthr"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Nthr).Value = drow(0).Item(sDbItemName)               'PeriodNthr

            ' �ݒ�l �������s�b�`
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodWpitch" & iColIx
			sDbItemName = "PeriodWpitch"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Wpitch).Value = drow(0).Item(sDbItemName)             'PeriodWpitch

            ' �ݒ�l �ŏ�����
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodMin" & iColIx
			sDbItemName = "PeriodMin"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Min).Value = drow(0).Item(sDbItemName)                'PeriodMin

            ' �ݒ�l �ő����
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodMax" & iColIx
			sDbItemName = "PeriodMax"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Max).Value = drow(0).Item(sDbItemName)                'PeriodMax


            ' �ݒ�l �ێ�����
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodKeepLen" & iColIx
			sDbItemName = "PeriodKeepLen"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_KeepLen).Value = drow(0).Item(sDbItemName)            'PeriodKeepLen

            ' �ݒ�l �������␳�W��
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'sDbItemName = "PeriodWeight" & iColIx
			sDbItemName = "PeriodWeight"
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------
            dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_Weight).Value = drow(0).Item(sDbItemName)             'PeriodWeight

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			sDbItemName = "PeriodFrontEnableLen"
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_FrontEnableLen).Value = drow(0).Item(sDbItemName)   'PeriodFrontEnableLen

			sDbItemName = "PeriodTailEnableLen"
			dgvPeriodData(iColIx, EM_PERIOD_GRID_ROW_ID.emTxt_TailEnableLen).Value = drow(0).Item(sDbItemName)   'PeriodTailEnableLen
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

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
    ''' �f�[�^�O���b�h�r���[�ݒ� �����r�ݒ�L�� 
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize_TypeMsc(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' �I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                       ' �w�b�_�[�̍���

        End With

        ' NO
        dgvColSetText(dgv, "NO", 45, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �r��
        dgvColSetText(dgv, "�r��", 145, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        For ii As Integer = 2 To mc_TypeMsc_ColMax - 1
            ' �O���[�h�ݒ�
            dgvColSetCheck(dgv, CStr(ii - 1), 100, False, False, _
                           DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        Next ii
    End Sub

    ''' <summary>
    ''' �^�C�g����̃Z�����I�����ꂽ�Ƃ��I���������s�� �����r�L���ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvTypeMscData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvTypeMscData.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        '�^�C�g����������Ă���Z���Ȃ�A�I����������
        If dgv.CurrentCell.ColumnIndex <= EM_DATACOLUMN_GRID_ID.emTxt_Name Then
            dgv.CurrentCell.Selected = False        ' �I������
        End If
    End Sub

    ''' <summary>
    ''' �Z���̓��e���ω������ꍇ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridView�̃`�F�b�N�{�b�N�X�������ꂽ�ꍇ �ʏ� �Z�����ړ������Ȃ��Ɗm��ɂȂ�Ȃ��ׂ̏��u</remarks>
    Private Sub dgvTypeMscData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvTypeMscData.CurrentCellDirtyStateChanged
        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ �R�~�b�g����
        CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�ݒ� �����r�ݒ�l 
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize_Period(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42                                                   ' �w�b�_�[�̍���
            .RowTemplate.Height = 38                                                    ' �e���v���[�g�̍s������ݒ�

        End With

        ' �s�^�C�g��
        dgvColSetText(dgv, "", 190, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
        dgv.Columns(EM_PERIOD_GRID_COL_ID.emTxt_Title).DefaultCellStyle.WrapMode = DataGridViewTriState.True

        For ii As Integer = 1 To mc_Period_ColMax
            ' �O���[�h�ݒ�
            dgvColSetText(dgv, CStr(ii), 100, False, False, _
                          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        Next ii
    End Sub

    ''' <summary>
    ''' �^�C�g����̃Z�����I�����ꂽ�Ƃ��I���������s�� �����r�ݒ�l
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPeriodData_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPeriodData.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)
        '�^�C�g����������Ă���Z���Ȃ�A�I����������
        If dgv.CurrentCell.ColumnIndex <= EM_PERIOD_GRID_COL_ID.emTxt_Title Then
            dgv.CurrentCell.Selected = False        ' �I������
        End If
    End Sub

    ''' <summary>
    ''' �Z���̓��e���ω������ꍇ �����r�ݒ�l
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>DataGridView�̃`�F�b�N�{�b�N�X�������ꂽ�ꍇ �ʏ� �Z�����ړ������Ȃ��Ɗm��ɂȂ�Ȃ��ׂ̏��u</remarks>
    Private Sub dgvPeriodData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPeriodData.CurrentCellDirtyStateChanged
        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ �R�~�b�g����
        CType(sender, tClassLibrary.tCnt.tCnt_DataGridView).dgvData_CurrentCellDirtyStateChanged(sender, e)
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�r���[�ݒ� �S�I�� Or �S����
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <param name="iMode">�\���ݒ� 0:�S�I�� 1:�S����</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize_SelectBtn(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal iMode As Integer)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                            ' �I��F����

        With dgv
            .RowCount = 0

			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>
			'.ScrollBars = ScrollBars.Both                                    ' �X�N���[���o�[�̕\��			            
			.ScrollBars = ScrollBars.None                                   ' �X�N���[���o�[�̕\��
			'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

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

        ' �f�[�^���Ɨ񐔂����킹��̂ŁA�擪�̂Q��Ƀ_�~�[���ǉ�����B
        ' �_�~�[1
        dgvColSetText(dgv, "Dummy1", 40, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

        ' �_�~�[2
        dgvColSetText(dgv, "Dummy2", 40, True, True, _
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

        '��������{���̃{�^���ݒ�
        For ii As Integer = 2 To mc_TypeMsc_ColMax - 1
            If EM_SELECT_MODE.emSelect = iMode Then                                         ' �S�I��
                ' �S�I���{�^��
                dgvColSetButton(dgv, m_strBtnName(EM_SELECT_MODE.emSelect), 100, False, False, _
                                DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                                New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, m_strBtnName(EM_SELECT_MODE.emSelect))
                dgv.Columns(ii).DefaultCellStyle().BackColor = Color.White                  ' �w�i
                dgv.Columns(ii).DefaultCellStyle().SelectionBackColor = Color.White         ' �I��

            Else                                                                            ' �S����
                ' �S�����{�^��
                dgvColSetButton(dgv, m_strBtnName(EM_SELECT_MODE.emCancel), 100, False, False, _
                                DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                                New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, m_strBtnName(EM_SELECT_MODE.emCancel))
                dgv.Columns(ii).DefaultCellStyle().BackColor = Color.AliceBlue              ' �w�i
                dgv.Columns(ii).DefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' �I��

            End If

        Next ii
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R -------------------->>>>>    
	''' <summary>
	''' dgvTypeMscData�̉��X�N���[�����ύX���ꂽ�Ƃ��� ���̑�dgv���X�N���[��������
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks>���ڂ��������Ƃ��p</remarks>
	Private Sub dgvTypeMscData_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvTypeMscData.Scroll, dgvPeriodData.Scroll
		' �X�N���[���C�x���g�������X�N���[���Ȃ�
		If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
			' �X�N���[���ʒu�����킹��
			Dim nScollingOffset As Integer = 0
			If sender Is dgvTypeMscData Then
				nScollingOffset = dgvTypeMscData.HorizontalScrollingOffset
				dgvPeriodData.HorizontalScrollingOffset = nScollingOffset
			Else
				nScollingOffset = dgvPeriodData.HorizontalScrollingOffset
				dgvTypeMscData.HorizontalScrollingOffset = nScollingOffset
			End If
			dgvTypeMscBtnAll.HorizontalScrollingOffset = nScollingOffset
			dgvTypeMscBtnClear.HorizontalScrollingOffset = nScollingOffset
		End If
	End Sub

	''' <summary>
	''' '�c�X�N���[���o�[�\���L��Ȃ�΁A�c�X�N���[���o�[���Adgv�̕����L����
	''' </summary>
	''' <remarks></remarks>
	Private Sub ChangeDgvWidth()
		Dim vsBar As VScrollBar

		For Each c As Control In dgvTypeMscData.Controls
			If TypeOf c Is VScrollBar Then
				vsBar = DirectCast(c, VScrollBar)

				If True = vsBar.Visible Then
					dgvTypeMscData.Width = dgvTypeMscData.Width + vsBar.Width
				End If
			End If
		Next

		For Each c As Control In dgvPeriodData.Controls
			If TypeOf c Is VScrollBar Then
				vsBar = DirectCast(c, VScrollBar)

				If True = vsBar.Visible Then
					dgvPeriodData.Width = dgvPeriodData.Width + vsBar.Width
				End If
			End If
		Next
	End Sub
	'// 2023.11.17 �ŏI�H���������u���@�\�����̂R <<<<<--------------------

End Class
