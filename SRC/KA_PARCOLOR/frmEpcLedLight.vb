'*********************************************************************************
' �Ɩ��ݒ��ʃN���X
'	[Ver]
'		Ver.01    2012/10/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcLedLight

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �萔
	'/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum EM_LEDLIGHT_GIRD_COL_ID
        emTxt_Title = 0         ' �e�L�X�g�{�b�N�X  �^�C�g��
        emTxt_M_rsw             ' �e�L�X�g�{�b�N�X  �S�̒���
        emTxt_Eq_data           ' �e�L�X�g�{�b�N�X  �z������
    End Enum

    ' �f�[�^�O���b�h�r���[�̗�o
    Private Enum EM_LEDLIGHT_GIRD_ROW_ID
        emTxt_Top1_In = 0       ' �e�L�X�g�{�b�N�X  �\��1����
        emTxt_Top1_LC           ' �e�L�X�g�{�b�N�X  �\��1�z������f�[�^
        emTxt_Top2_In           ' �e�L�X�g�{�b�N�X  �\��2����
        emTxt_Top2_LC           ' �e�L�X�g�{�b�N�X  �\��2�z������f�[�^
        emTxt_Top3_In           ' �e�L�X�g�{�b�N�X  �\��3����
        emTxt_Top3_LC           ' �e�L�X�g�{�b�N�X  �\��3�z������f�[�^
        emTxt_Top4_In           ' �e�L�X�g�{�b�N�X  �\��4����
        emTxt_Top4_LC           ' �e�L�X�g�{�b�N�X  �\��4�z������f�[�^
        emTxt_Top5_In           ' �e�L�X�g�{�b�N�X  �\��5����
        emTxt_Top5_LC           ' �e�L�X�g�{�b�N�X  �\��5�z������f�[�^
        emTxt_Bot1_In           ' �e�L�X�g�{�b�N�X  ����1����
        emTxt_Bot1_LC           ' �e�L�X�g�{�b�N�X  ����1�z������f�[�^
        emTxt_Bot2_In           ' �e�L�X�g�{�b�N�X  ����2����
        emTxt_Bot2_LC           ' �e�L�X�g�{�b�N�X  ����2�z������f�[�^
        emTxt_Bot3_In           ' �e�L�X�g�{�b�N�X  ����3����
        emTxt_Bot3_LC           ' �e�L�X�g�{�b�N�X  ����3�z������f�[�^
        emTxt_Bot4_In           ' �e�L�X�g�{�b�N�X  ����4����
        emTxt_Bot4_LC           ' �e�L�X�g�{�b�N�X  ����4�z������f�[�^
        emTxt_Bot5_In           ' �e�L�X�g�{�b�N�X  ����5����
        emTxt_Bot5_LC           ' �e�L�X�g�{�b�N�X  ����5�z������f�[�^


        em_END
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u��
    Private m_KeyCode As String = ""                                ' ��L�[

    'Private Const LED_ADJUST_CNT As Integer = 1
    Private Const LED_ADJUST_CNT As Integer = NUM_LED_BLOCK
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
	Private Sub frmLight_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
		If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 0
                ChangeCode(False)               ' F1 �O��
            Case 1
                ChangeCode(True)                ' F2 ����
			Case 6								' F07 ��ʈ��
				Call PrintOut()
			Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "LED�Ɩ��ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
				modMain.ChgProject()            ' F09 KA_PARSET�֐ؑ֏���
            Case 10
                ' subData_UpDate���ŃZ���ɒl��ݒ肷�邪�A���̍ۃZ���̒l���ύX����Ă��܂���
                ' �Z���ύX�C�x���g���Ă΂�Ă��܂��ׁA�Z���ύX�C�x���g����U�폜
                dgvLight.Enabled = False
                RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

                btnF11.Select()                 ' F11��I����Ԃɂ��� dgv���������ݒ��̎��ɁA���͊����ɂȂ�悤��
                subData_UpDate()                ' F11 �ݒ�

                ' �Z���f�[�^�ύX�C�x���g���ēo�^
                AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
                dgvLight.Enabled = True
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
    Private Sub frmLight_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList

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
    Private Sub frmEpcLedLight_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'm_bCellCheck = True
        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcLedLight           ' �Ɩ��ݒ���

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        If NOA_OFFLINE = g_NOA Or g_bPassFlg = False Then
            btnTopReset.Enabled = False
            btnBotReset.Enabled = False
        End If

        ' �A�b�v�R�[�h�̃R���{�{�b�N�X
        MyBase.SetComboBox_Scode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' �����̐ݒ�
        dgvInitialize(dgvLight)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        ' �R���{�{�b�N�X�Ɍ����p�^�[�����Z�b�g
        MyBase.SetComboBox_Scode(cmbSCode)

        If 0 < cmbSCode.Items.Count Then

            '�O�̉�ʂőI��������Ԃ��Z�b�g
            If cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                '�Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
                cmbSCode.SelectedIndex = 0
            Else
                cmbSCode.SelectedIndex = g_nScodeIndex
            End If
        Else
            ' ��ʕ\�����s�Ȃ̂ŁA�����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�Ƀf�[�^�������� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        cmbSCode.DropDownStyle = ComboBoxStyle.DropDownList

        ' �Z���I���C�x���g�ɂĎ��s�����ȉ��֐������{���Ȃ��ƃZ���Ƀf�[�^���ݒ肳��Ȃ�
        ' �����p�^�[����\��
        Call ShowSelectData()

        ' �t�H�[�����[�h���ɃZ���ύX�C�x���g���������Ȃ��悤�ɂ���ׁA�Z���ւ̃f�[�^�ݒ肪������ɃC�x���g��o�^
        ' �C�x���g�o�^
        RemoveHandler cmbSCode.SelectedIndexChanged, AddressOf cmbSCode_SelectedIndexChanged
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        AddHandler cmbSCode.SelectedIndexChanged, AddressOf cmbSCode_SelectedIndexChanged
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

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
    Private Sub frmEpcLedLight_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

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

        ' SQL������
        strSQL = " SELECT scode"
        strSQL &= ", �Ɩ��Ɠx_�\"
        strSQL &= ", �Ɩ��Ɠx2_�\"
        strSQL &= ", �Ɩ��Ɠx3_�\"
        strSQL &= ", �Ɩ��Ɠx4_�\"
        strSQL &= ", �Ɩ��Ɠx5_�\"
        strSQL &= ", �Ɩ��Ɠx_��"
        strSQL &= ", �Ɩ��Ɠx2_��"
        strSQL &= ", �Ɩ��Ɠx3_��"
        strSQL &= ", �Ɩ��Ɠx4_��"
        strSQL &= ", �Ɩ��Ɠx5_��"
        strSQL &= ", �Ɩ��Ɠx�␳�l_�\"
        strSQL &= ", �Ɩ��Ɠx�␳�l2_�\"
        strSQL &= ", �Ɩ��Ɠx�␳�l3_�\"
        strSQL &= ", �Ɩ��Ɠx�␳�l4_�\"
        strSQL &= ", �Ɩ��Ɠx�␳�l5_�\"
        strSQL &= ", �Ɩ��Ɠx�␳�l_��"
        strSQL &= ", �Ɩ��Ɠx�␳�l2_��"
        strSQL &= ", �Ɩ��Ɠx�␳�l3_��"
        strSQL &= ", �Ɩ��Ɠx�␳�l4_��"
        strSQL &= ", �Ɩ��Ɠx�␳�l5_��"
        strSQL &= " FROM " & DB_PARAM_SCODE
        strSQL &= " ORDER BY scode ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_SCODE & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function


    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <remarks></remarks>
    Private Function Data_Disp(ByVal key As String) As Boolean

        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�����p�^�[�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("scode='{0}'", key)          ' WHERE��ɑ����̏�������
        Dim drowScode() As DataRow = m_DS.Tables(0).Select(strWhere)
        Dim bytData_Top1() As Byte                                          ' �Ɩ��Ɠx�␳�l �\��1
        Dim bytData_Top2() As Byte                                          ' �Ɩ��Ɠx�␳�l �\��2
        Dim bytData_Top3() As Byte                                          ' �Ɩ��Ɠx�␳�l �\��3
        Dim bytData_Top4() As Byte                                          ' �Ɩ��Ɠx�␳�l �\��4
        Dim bytData_Top5() As Byte                                          ' �Ɩ��Ɠx�␳�l �\��5
        Dim bytData_Bot1() As Byte                                          ' �Ɩ��Ɠx�␳�l ����1
        Dim bytData_Bot2() As Byte                                          ' �Ɩ��Ɠx�␳�l ����2
        Dim bytData_Bot3() As Byte                                          ' �Ɩ��Ɠx�␳�l ����3
        Dim bytData_Bot4() As Byte                                          ' �Ɩ��Ɠx�␳�l ����4
        Dim bytData_Bot5() As Byte                                          ' �Ɩ��Ɠx�␳�l ����5
        Dim nLcData As Integer                                              ' �z������f�[�^
        Dim dResult As Double

        '�f�[�^���o�^����Ă��Ȃ��ꍇ�͋����I��������
        If MAX_SCODE < drowScode.Length Or 0 >= drowScode.Length Then
            Data_Disp = False
            Exit Function
        End If

        ' �e�s�̏����ݒ�
        dgvLight.RowCount = EM_LEDLIGHT_GIRD_ROW_ID.em_END

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = "�\�� ��(����)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = "�\�� ��(����:�c�r�`�b)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = "�\�� ��(����:�b�`�v�r)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = "�\�� ��(����:�c�r�`�b)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = "�\�� ��(����:�b�`�v�r)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = "���� ��(����)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = "���� ��(����:�c�r�`�b)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = "���� ��(����:�b�`�v�r)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = "���� ��(����:�v�r�`�b)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = "���� ��(����:�b�`�c�r)"
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Title, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = "�@�@ (�Q�ƁF�z������f�[�^)"

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Style.BackColor = Color.LightGray

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Style.BackColor = Color.LightGray
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).ReadOnly = True
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Style.BackColor = Color.LightGray

        For ii As Integer = 0 To LED_ADJUST_CNT - 1
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Style.BackColor = Color.LightGray

            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Style.BackColor = Color.LightGray
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).ReadOnly = True
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Style.BackColor = Color.LightGray
        Next ii

        '' ��ʂɕ\��
        ' �Ɩ��Ɠx�\��
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx_�\"))     ' �Ɩ��Ɠx_�\
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx2_�\"))    ' �Ɩ��Ɠx_�\
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx3_�\"))    ' �Ɩ��Ɠx_�\
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx4_�\"))    ' �Ɩ��Ɠx_�\
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx5_�\"))    ' �Ɩ��Ɠx_�\

        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx_��"))     ' �Ɩ��Ɠx_��
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx2_��"))    ' �Ɩ��Ɠx_��
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx3_��"))    ' �Ɩ��Ɠx_��
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx4_��"))    ' �Ɩ��Ɠx_��
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = CInt(drowScode(0).Item("�Ɩ��Ɠx5_��"))    ' �Ɩ��Ɠx_��

        ' �Ɩ��Ɠx�␳�l Byte�ϊ�
        '�\
        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l_�\") Then
            bytData_Top1 = Nothing
        Else
            bytData_Top1 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l_�\"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l2_�\") Then
            bytData_Top2 = Nothing
        Else
            bytData_Top2 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l2_�\"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l3_�\") Then
            bytData_Top3 = Nothing
        Else
            bytData_Top3 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l3_�\"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l4_�\") Then
            bytData_Top4 = Nothing
        Else
            bytData_Top4 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l4_�\"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l5_�\") Then
            bytData_Top5 = Nothing
        Else
            bytData_Top5 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l5_�\"), Byte())
        End If

        '��
        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l_��") Then
            bytData_Bot1 = Nothing
        Else
            bytData_Bot1 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l_��"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l2_��") Then
            bytData_Bot2 = Nothing
        Else
            bytData_Bot2 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l2_��"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l3_��") Then
            bytData_Bot3 = Nothing
        Else
            bytData_Bot3 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l3_��"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l4_��") Then
            bytData_Bot4 = Nothing
        Else
            bytData_Bot4 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l4_��"), Byte())
        End If

        If System.DBNull.Value Is drowScode(0).Item("�Ɩ��Ɠx�␳�l5_��") Then
            bytData_Bot5 = Nothing
        Else
            bytData_Bot5 = CType(drowScode(0).Item("�Ɩ��Ɠx�␳�l5_��"), Byte())
        End If

        ' �Ɩ��Ɠx�␳�l �z��w��
        ReDim Preserve bytData_Top1(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top2(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top3(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top4(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Top5(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot1(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot2(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot3(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot4(LED_ADJUST_CNT - 1)
        ReDim Preserve bytData_Bot5(LED_ADJUST_CNT - 1)

        ' �Ɩ��Ɠx�␳�l ��ʕ\��
        For ii As Integer = 0 To LED_ADJUST_CNT - 1
            '' �Ɩ��Ɠx�\��1
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = bytData_Top1(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)

            '' �Ɩ��Ɠx�\��2
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = bytData_Top2(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)

            '' �Ɩ��Ɠx�\��3
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = bytData_Top3(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)

            '' �Ɩ��Ɠx�\��4
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = bytData_Top4(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)

            '' �Ɩ��Ɠx�\��5
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = bytData_Top5(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)

            '' �Ɩ��Ɠx����1
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = bytData_Bot1(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)

            '' �Ɩ��Ɠx����2
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = bytData_Bot2(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)

            '' �Ɩ��Ɠx����3
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = bytData_Bot3(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)

            '' �Ɩ��Ɠx����4
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = bytData_Bot4(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)

            '' �Ɩ��Ɠx����5
            ' �z�������f�[�^
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = bytData_Bot5(ii)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value),
             CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value))
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = String.Format("Hex:{0:X}", dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)

        Next ii

        Data_Disp = True

    End Function

    ''' <summary>
    ''' �����p�^�[���ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' ShowSelectData���ŃZ���ɒl��ݒ肷�邪�A���̍ۃZ���̒l���ύX����Ă��܂���
        ' �Z���ύX�C�x���g���Ă΂�Ă��܂��ׁA�Z���ύX�C�x���g����U�폜
        dgvLight.Enabled = False
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

        ShowSelectData()

        ' �Z���f�[�^�ύX�C�x���g���ēo�^
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        dgvLight.Enabled = True

    End Sub

    ''' <summary>
    ''' �I�����������p�^�[���̃f�[�^��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ShowSelectData()
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂ŁA���ɖ߂����ꍇ�̑΍�)
        If m_KeyCode = cmbSCode.Text Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ�
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then
                ' ���ُ͈펞�́A�I���ʒu���߂�
                cmbSCode.Text = m_KeyCode
                Exit Sub
            End If

            ' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
            Data_Get(m_KeyCode)
        End If


        ' �I��ύX�ɔ��� ���x���l�ύX
        MyBase.SelectComboBoxItem(cmbSCode.SelectedIndex, cmbSCode, lblSCode)

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = SelectComboBoxKey(cmbSCode.SelectedIndex, cmbSCode)
        Data_Disp(m_KeyCode)

        '���̑I����Ԃ�ۑ�
        g_nScodeIndex = cmbSCode.SelectedIndex
    End Sub

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���̉�ʕ\�����̒l�Ǝ�荞��
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then Exit Sub

            Data_Get(m_KeyCode)
        End If

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

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
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String
        Dim blnNull As Boolean = False

        ' �Ɩ��Ɠx_�\1
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� �\��1") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = strWk

        ' �Ɩ��Ɠx_�\2
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� �\��2") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = strWk

        ' �Ɩ��Ɠx_�\3
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� �\��3") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = strWk

        ' �Ɩ��Ɠx_�\4
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� �\��4") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = strWk

        ' �Ɩ��Ɠx_�\5
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� �\��5") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = strWk

        ' �Ɩ��Ɠx_��1
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� ����1") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = strWk

        ' �Ɩ��Ɠx_��2
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� ����2") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = strWk

        ' �Ɩ��Ɠx_��3
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� ����3") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = strWk

        ' �Ɩ��Ɠx_��4
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� ����4") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = strWk

        ' �Ɩ��Ɠx_��5
        strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)
        If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                "�S�̒��� ����5") <> 0 Then Return False
        dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = strWk

        For ii As Integer = 0 To LED_ADJUST_CNT - 1
            ' �Ɩ��Ɠx�␳�l_�\1
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " �\��1") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_�\2
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " �\��2") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_�\3
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " �\��3") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_�\4
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " �\��4") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_�\5
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " �\��5") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_��1
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " ����1") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_��2
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " ����2") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_��3
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " ����3") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_��4
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " ����4") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value = strWk

            ' �Ɩ��Ɠx�␳�l_��5
            strWk = CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 255, blnNull,
                    "�z������" & CStr(ii + 1) & " ����5") <> 0 Then Return False
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value = strWk

        Next ii

        Return True

    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String                                  ' WHERE��ɑ����̏�������
        Dim drowScode() As DataRow
        Dim bytData_Top1(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l �\��1
        Dim bytData_Top2(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l �\��2
        Dim bytData_Top3(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l �\��3
        Dim bytData_Top4(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l �\��4
        Dim bytData_Top5(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l �\��5
        Dim bytData_Bot1(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l ����1
        Dim bytData_Bot2(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l ����2
        Dim bytData_Bot3(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l ����3
        Dim bytData_Bot4(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l ����4
        Dim bytData_Bot5(LED_ADJUST_CNT - 1) As Byte            ' �Ɩ��Ɠx�␳�l ����5
        Dim ii As Integer

        strWhere = String.Format("scode='{0}'", key)            ' WHERE��ɑ����̏�������
        drowScode = m_DS.Tables(0).Select(strWhere)             ' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

        '' �O�ׂ̈Ɍ����`�F�b�N
        If MAX_SCODE < drowScode.Length Or 0 >= drowScode.Length Then
            Exit Sub
        End If

        drowScode(0).Item("�Ɩ��Ɠx_�\") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)       ' �Ɩ��Ɠx_�\
        drowScode(0).Item("�Ɩ��Ɠx2_�\") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)      ' �Ɩ��Ɠx_�\
        drowScode(0).Item("�Ɩ��Ɠx3_�\") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)      ' �Ɩ��Ɠx_�\
        drowScode(0).Item("�Ɩ��Ɠx4_�\") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)      ' �Ɩ��Ɠx_�\
        drowScode(0).Item("�Ɩ��Ɠx5_�\") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)      ' �Ɩ��Ɠx_�\
        drowScode(0).Item("�Ɩ��Ɠx_��") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)       ' �Ɩ��Ɠx_��
        drowScode(0).Item("�Ɩ��Ɠx2_��") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)      ' �Ɩ��Ɠx_��
        drowScode(0).Item("�Ɩ��Ɠx3_��") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)      ' �Ɩ��Ɠx_��
        drowScode(0).Item("�Ɩ��Ɠx4_��") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)      ' �Ɩ��Ɠx_��
        drowScode(0).Item("�Ɩ��Ɠx5_��") = CInt(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)      ' �Ɩ��Ɠx_��
        For ii = 0 To LED_ADJUST_CNT - 1
            bytData_Top1(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value)
            bytData_Top2(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value)
            bytData_Top3(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value)
            bytData_Top4(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value)
            bytData_Top5(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value)
            bytData_Bot1(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value)
            bytData_Bot2(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value)
            bytData_Bot3(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value)
            bytData_Bot4(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value)
            bytData_Bot5(ii) = CByte(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value)
        Next ii
        drowScode(0).Item("�Ɩ��Ɠx�␳�l_�\") = bytData_Top1
        drowScode(0).Item("�Ɩ��Ɠx�␳�l2_�\") = bytData_Top2
        drowScode(0).Item("�Ɩ��Ɠx�␳�l3_�\") = bytData_Top3
        drowScode(0).Item("�Ɩ��Ɠx�␳�l4_�\") = bytData_Top4
        drowScode(0).Item("�Ɩ��Ɠx�␳�l5_�\") = bytData_Top5
        drowScode(0).Item("�Ɩ��Ɠx�␳�l_��") = bytData_Bot1
        drowScode(0).Item("�Ɩ��Ɠx�␳�l2_��") = bytData_Bot2
        drowScode(0).Item("�Ɩ��Ɠx�␳�l3_��") = bytData_Bot3
        drowScode(0).Item("�Ɩ��Ɠx�␳�l4_��") = bytData_Bot4
        drowScode(0).Item("�Ɩ��Ɠx�␳�l5_��") = bytData_Bot5

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

                '�A�_�v�^���Z�b�g���čX�V
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            '�R�~�b�g
            transact.Commit()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_LIGHT, CH_SCODE_ALL)        '�ǂ̃e�[�u�����ύX���ꂽ���킩��Ȃ��̂�ALL�ő��M
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

    ''' <summary>
    ''' LED�Ɩ��݌v����_�\���Z�b�g�{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnTopReset_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnTopReset.Click

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' ���Z�b�g���܂����H
            frmMsg.Text = "�m�F"
            frmMsg.ChangeMsg("LED�Ɩ��݌v����_�\�ʂ�����������܂��B���Z�b�g���܂����H", Color.Red.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_KT_LEDMGR_05
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' �f�[�^��
        que.data(0) = 0                         ' �\���敪 (�\:0   ��:1)
        que.data(1) = 0                         ' �ύX�����\�ʏ��No (�����\�ʏ��No:1�`  �S��:0)

        KizuLibCLI.KizuMod.Send_Mail(KT_LEDMGR, "*", que)

    End Sub

    ''' <summary>
    ''' LED�Ɩ��݌v����_�����Z�b�g�{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnBotReset_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnBotReset.Click

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' ���Z�b�g���܂����H
            frmMsg.Text = "�m�F"
            frmMsg.ChangeMsg("LED�Ɩ��݌v����_���ʂ�����������܂��B���Z�b�g���܂����H", Color.Red.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        Dim que As New KizuLibCLI.COMMON_QUE_L
        que.inisialize()
        que.nEventNo = FACT_KT_LEDMGR_05
        que.nLineNo = KizuLibCLI.KizuMod.GetLineID()

        ' �f�[�^��
        que.data(0) = 1                         ' �\���敪 (�\:0   ��:1)
        que.data(1) = 0                         ' �ύX�����\�ʏ��No (�����\�ʏ��No:1�`  �S��:0)

        KizuLibCLI.KizuMod.Send_Mail(KT_LEDMGR, "*", que)

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' dgv������
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����

        With dgv
            .RowCount = 0

            .AllowUserToResizeColumns = False                               ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
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
            .ColumnHeadersHeight = 24                                       ' �w�b�_�[�̍���

        End With

        ' �^�C�g��
        dgvColSetText(dgv, "", 240, True, True,
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �S�̒���
        dgvColSetText(dgv, "�S�̒���", 100, True, False,
                      DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' �z������
        For ii As Integer = 1 To LED_ADJUST_CNT
            dgvColSetText(dgv, "�z������" & CStr(ii), 100, False, False,
                              DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        Next ii

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ���̑����܂��܂������� �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' �O�� ����
    ''' </summary>
    ''' <param name="bln">�O��:false ����:true</param>
    ''' <remarks></remarks>
    Private Sub ChangeCode(ByVal bln As Boolean)
        Dim rec As Integer = CInt(IIf(bln, cmbSCode.SelectedIndex + 1, cmbSCode.SelectedIndex - 1))
        ' �㉺���`�F�b�N
        If rec < 0 OrElse rec >= cmbSCode.Items.Count Then Exit Sub
        ' �ؑ�
        cmbSCode.SelectedIndex = rec
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
        '��
        Dim strFromKey As String = ""                                    '�R�s�[���L�[ 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '��
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        '��
        Dim strToKey As String = MyBase.GetUpCodeItem_To(False)          '�R�s�[��L�[
        If strToKey.Length = 0 Then Exit Sub

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '''' DB�R�s�[
            If CopyDB(strFromKey, strToKey) Then
                '�����@�\�փp�����[�^�ύX�ʒm
                modMain.Send_Main(CH_PARAM_LIGHT, CH_SCODE_ALL)
                modMain.HardCopy(Me)

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


        ' Data_Disp���ŃZ���ɒl��ݒ肷�邪�A���̍ۃZ���̒l���ύX����Ă��܂���
        ' �Z���ύX�C�x���g���Ă΂�Ă��܂��ׁA�Z���ύX�C�x���g����U�폜
        dgvLight.Enabled = False
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

        '''' ��ʍĕ\��
        Data_Disp(m_KeyCode)

        ' �Z���f�[�^�ύX�C�x���g���ēo�^
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        dgvLight.Enabled = True
    End Sub

    ''' <summary>
    ''' �R�s�[����
    ''' </summary>
    ''' <param name="strFromKey">�R�s�[���L�[ </param>
    ''' <param name="strToKey">�R�s�[��L�[ </param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function CopyDB(ByVal strFromKey As String, ByVal strToKey As String) As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        '�\�ʏ�ԕ�(ParamScode)���X�V
        ' SQL������
        '''' SQL���̐���
        strSQL = "UPDATE " & DB_PARAM_SCODE & " SET "
        '�ϗ�
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx2_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx3_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx4_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx5_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx2_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx3_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx4_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx5_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l2_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l3_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l4_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l5_�\", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l2_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l3_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l4_��", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�Ɩ��Ɠx�␳�l5_��", "b")
        tClass_SQLServer.sqlColumnTrim(strSQL)                  ' ���܂��Ȃ��A�K���L�q

        'From��
        strSQL &= " FROM " & DB_PARAM_SCODE & " a "
        strSQL &= ", (SELECT * FROM " & DB_PARAM_SCODE & " WHERE scode='" & strFromKey & "') b "
        'WHERE��
        strSQL &= " WHERE a.scode IN(" & strToKey & ")"

        'SQL�����s
        If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_SCODE & " �X�V�G���[ [" & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' �ݒ蒲���l�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Get_LC_DATA(ByRef m_rsw As Integer, ByRef eq_data As Integer) As Integer
        Dim ans As Integer

        ans = CInt(Int(m_rsw / 2) + Int(m_rsw * eq_data / 512) + 1)

        'Dim dWK As Double
        ''�ݒ蒲���l�擾
        'dWK = (m_rsw / 2) + (m_rsw * eq_data / 512)
        ''�����_�؎̂�
        'If dWK < 0.5 Then
        '    dWK = 0
        'Else
        '    dWK = dWK - 0.5
        'End If
        ''�����_�؎̂Č�\��
        'ans = CInt(dWK)

        Return ans

    End Function

    ''' <summary>
    ''' �z������f�[�^�\���i�Z���f�[�^�ύX���p�j
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DISP_LC_DATA()
        Dim ii As Integer = 0
        Dim nMRsw As Integer                                               ' �S�̒����f�[�^
        Dim nEqData As Integer                                              ' �z�������f�[�^
        Dim nLcData As Integer                                              ' �z������f�[�^
        Dim dResult As Double

        For ii = 0 To LED_ADJUST_CNT - 1
            '' �Ɩ��Ɠx�\��1
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top1_LC).Value = String.Format("Hex:{0:X}", nMRsw)


            '' �Ɩ��Ɠx�\��2
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top2_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' �Ɩ��Ɠx�\��3
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top3_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' �Ɩ��Ɠx�\��4
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top4_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' �Ɩ��Ɠx�\��5
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Top5_LC).Value = String.Format("Hex:{0:X}", nMRsw)


            '' �Ɩ��Ɠx����1
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot1_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' �Ɩ��Ɠx����2
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot2_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' �Ɩ��Ɠx����3
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot3_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' �Ɩ��Ɠx����4
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot4_LC).Value = String.Format("Hex:{0:X}", nMRsw)

            '' �Ɩ��Ɠx����5
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value), nMRsw)
            tClassLibrary.tClass.tClass_ImputCheck.CheckData(CStr(dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_In).Value), nEqData)
            ' �z������f�[�^���Z
            nLcData = Get_LC_DATA(nMRsw, nEqData)
            ' �z������f�[�^�\��
            'dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & FormatPercent(nLcData / 256, 0) & ")"
            dResult = Math.Truncate((nLcData / 256) * 100)
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_Eq_data + ii, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = CStr(nLcData) & "(" & dResult.ToString & "%)"
            dgvLight(EM_LEDLIGHT_GIRD_COL_ID.emTxt_M_rsw, EM_LEDLIGHT_GIRD_ROW_ID.emTxt_Bot5_LC).Value = String.Format("Hex:{0:X}", nMRsw)


        Next ii
    End Sub

    ''' <summary>
    ''' �Z���f�[�^�ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvLight_CellValueChanged(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)

        ' DISP_LC_DATA���ŃZ���ɒl��ݒ肷�邪�A���̍ۃZ���̒l���ύX����Ă��܂���
        ' �Z���ύX�C�x���g���Ă΂�Ă��܂��ׁA�Z���ύX�C�x���g����U�폜
        dgvLight.Enabled = False
        RemoveHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged

        DISP_LC_DATA()

        ' �Z���f�[�^�ύX�C�x���g���ēo�^
        AddHandler dgvLight.CellValueChanged, AddressOf dgvLight_CellValueChanged
        dgvLight.Enabled = True

    End Sub

    '''' <summary>
    '''' �X�V�{�^���̗L��/����
    '''' </summary>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��
        pnlItem.Enabled = bPassResult                   ' �R�s�[�p���X�g�̔��f�{�^��

    End Sub
End Class
