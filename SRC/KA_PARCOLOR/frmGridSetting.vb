'*********************************************************************************
' ����������ʃf�[�^�O���b�h�\���ݒ�N���X
'	[Ver]
'		Ver.01    2015/07/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmGridSetting
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�P��
    Private Enum EM_GRID
        emTxt_KNo = 0                           ' ����No
        emTxt_Hedder1                           ' ���ڃw�b�_�[�P
        emTxt_Hedder2                           ' ���ڃw�b�_�[�Q
        emTxt_Bikou                             ' ���l
        emCmb_CellType                          ' ���ڃZ���^�C�v
        emCmb_Lov_id                            ' lov_id
        emChk_DispUmu                           ' ���ڕ\���L��
        emChk_ReadOnly                          ' �ǂݎ���p
        emTxt_KHaba                             ' ���ڕ�
        emCmb_KHaiti                            ' ���ڔz�u
        emTxt_Format                            ' �t�H�[�}�b�g
        emChk_Frozen                            ' ��Œ�
        emCmb_TableName                         ' �e�[�u����
        emCmb_Field                             ' �t�B�[���h��
        emChk_Key                               ' ��L�[
        emCmb_FieldType                         ' �t�B�[���h�^�C�v
        emTxt_InputMin                          ' ���͉\�ŏ��l
        emTxt_InputMax                          ' ���͉\�ő�l
        emChk_NullOK                            ' NULL���͉�
        emTxt_InputKeta                         ' ���͉\����
        emTxt_DecKeta                           ' �����_�ȉ�����
        emCmb_DispDialog                        ' �\���_�C�A���O
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterGrid As New SqlClient.SqlDataAdapter        ' �A�_�v�^�[ �f�[�^�O���b�h�\���ݒ�
    Private m_Grid As New DataSet                                ' ��ڑ��^�e�[�u�� �f�[�^�O���b�h�\���ݒ�
    Private m_AdapterLov_id As New SqlClient.SqlDataAdapter      ' �A�_�v�^�[ Lov_id
    Private m_Lov_id As New DataSet                              ' ��ڑ��^�e�[�u�� Lov_id
    Private m_AdapterField As New SqlClient.SqlDataAdapter       ' �A�_�v�^�[DB�t�B�[���h��
    Private m_Field As New DataSet                               ' ��ڑ��^�e�[�u�� DB�t�B�[���h��
    Private m_AdapterDBKey As New SqlClient.SqlDataAdapter       ' �A�_�v�^�[��L�[�擾
    Private m_DBKey As New DataSet                               ' ��ڑ��^�e�[�u�� ��L�[�擾
    Private m_AdapterDBAttr As New SqlClient.SqlDataAdapter      ' �A�_�v�^�[DB�����擾
    Private m_DBAttr As New DataSet                              ' ��ڑ��^�e�[�u�� DB�����擾

    Private Const ITEM_MAX_COUNT As Integer = 10000              ' �O���b�hROW���ڐ�
    Private m_nDelItem(ITEM_MAX_COUNT - 1) As Integer            ' �폜�Ώ� ����No
    Private m_nDispItem(ITEM_MAX_COUNT - 1) As Integer           ' �\���� ����No
    Private Const GRID_ROW_COUNT As Integer = 22                 ' �O���b�hROW���ڐ�
    Private m_KeyGID As String = ""                              ' �I�𒆉��ID
    Private m_KeyGridID As String = ""                           ' �I�𒆃O���b�hID

    Private m_bDispFlg As Boolean = False                        ' �\������������p�t���O true:�\�������� false:�������Ŗ���
    '//////////////////////////////////////////////////////////////////////////////////
    '���ID�ɕR�t�����\����
    '//////////////////////////////////////////////////////////////////////////////////
    Public Structure GID_INFO
        Dim strDBGID As String                                   ' DB��̃O���b�hID
        Dim nGridCnt As Integer                                  ' �O���b�h��
        Dim listGrid As Dictionary(Of Integer, String)           ' �O���b�hID���L�[�Ƃ�����ʕ\���O���b�hID
        Dim listGridID As Dictionary(Of String, Integer)         ' ��ʕ\���O���b�hID���L�[�Ƃ����O���b�hID
        Public Sub initialize()
            listGrid.Clear()
            listGridID.Clear()
        End Sub
    End Structure
    Public m_DBlst As New Dictionary(Of String, GID_INFO)      ' ��ʕ\���O���b�hID�擾�p���

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
                Call HardCopy(IMG_PATH, "�f�[�^�O���b�h�\���ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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

        m_Lov_id.Dispose()
        If Not m_AdapterLov_id Is Nothing Then
            m_AdapterLov_id.Dispose()
            m_AdapterLov_id = Nothing
        End If

        m_Field.Dispose()
        If Not m_AdapterField Is Nothing Then
            m_AdapterField.Dispose()
            m_AdapterField = Nothing
        End If

        m_DBKey.Dispose()
        If Not m_AdapterField Is Nothing Then
            m_AdapterField.Dispose()
            m_AdapterField = Nothing
        End If

        m_DBAttr.Dispose()
        If Not m_AdapterDBAttr Is Nothing Then
            m_AdapterDBAttr.Dispose()
            m_AdapterDBAttr = Nothing
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
        modMain.SetLabelSysName(Me)                             ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmGridSetting       ' ��ʌďo

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        'dgv������
        dgvInitializeGrid(dgvGrid)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '�I��
        End If

        '���ID�R���{�{�b�N�X�p��������
        Call SetComboBox_GID()
        Call SetComboBox_GridID()

        ' ��ʂɒl��\��
        Data_Disp()

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvGrid.DataGridViewSelectionRowColorChange(g_ColorSelection)     ' dgv

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
        strSQL = "SELECT DISTINCT lov_id"
        strSQL &= " FROM " & DB_PAR_LOV_ITEM
        strSQL &= " ORDER BY lov_id ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_Lov_id, m_AdapterLov_id, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PAR_LOV_ITEM & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        m_bDispFlg = True
        '�f�[�^��\��
        Data_DispdgvGrid(dgvGrid)
        m_bDispFlg = False

        ' �s�̐F��ς��� �i�����\���ŐF���ς��΍�j
        dgvGrid.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' dgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispdgvGrid(ByVal dgv As DataGridView)
        ' �\�������ID�y�уO���b�hID�ɕR�t������ΏۂƂ���
        Dim strGridWhere As String = String.Format("���ID='{0}' and �O���b�hID = '{1}'", m_DBlst(cmbGID.Text).strDBGID, m_DBlst(cmbGID.Text).listGridID(cmbGridID.Text))          ' �\�������ID �Ώ�
        Dim drowGrid() As DataRow = m_Grid.Tables(0).Select(strGridWhere)           ' �f�[�^�O���b�h�\���ݒ�
        Dim drowLov_id() As DataRow = m_Lov_id.Tables(0).Select()                   ' Lov_id
        '������
        dgv.Rows.Clear()
        dgv.Columns.Clear()

        dgv.RowCount = GRID_ROW_COUNT                   ' ROW���ڐ�
        Call SetDgvTitle()                              ' �f�[�^�O���b�h���o���ݒ�

        For ii As Integer = 0 To drowGrid.Length - 1
            '�\�����ڗp�̗��ǉ�
            Call AddDgvRetu(ii)
            '�\�������Z�b�g
            dgv(ii, EM_GRID.emTxt_KNo).Value = drowGrid(ii).Item("����No").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_Hedder1).Value = drowGrid(ii).Item("���ڃw�b�_�[1").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_Hedder2).Value = drowGrid(ii).Item("���ڃw�b�_�[2").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_Bikou).Value = drowGrid(ii).Item("���l").ToString.Trim()

            If "0" = drowGrid(ii).Item("���ڃZ���^�C�v").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "TextBox"
            ElseIf "1" = drowGrid(ii).Item("���ڃZ���^�C�v").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "ComboBox"
            ElseIf "2" = drowGrid(ii).Item("���ڃZ���^�C�v").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "CheckBox"
            ElseIf "3" = drowGrid(ii).Item("���ڃZ���^�C�v").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_CellType).Value = "Button"
            Else
                dgv(ii, EM_GRID.emCmb_CellType).Value = "TextBox"
            End If

            If "" = drowGrid(ii).Item("lov_id").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_Lov_id).Value = "�ΏۊO"
            ElseIf "0" = drowGrid(ii).Item("lov_id").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_Lov_id).Value = "�ΏۊO"
            Else
                dgv(ii, EM_GRID.emCmb_Lov_id).Value = drowGrid(ii).Item("lov_id").ToString.Trim()
            End If

            dgv(ii, EM_GRID.emChk_DispUmu).Value = drowGrid(ii).Item("�\��_���ڕ\���L��").ToString.Trim()
            dgv(ii, EM_GRID.emChk_ReadOnly).Value = drowGrid(ii).Item("�\��_�ǂݎ���p").ToString.Trim()
            dgv(ii, EM_GRID.emTxt_KHaba).Value = drowGrid(ii).Item("�\��_���ڕ�").ToString.Trim()

            If "1" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "�㍶"
            ElseIf "2" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "�㒆"
            ElseIf "4" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "��E"
            ElseIf "16" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "����"
            ElseIf "32" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "����"
            ElseIf "64" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "���E"
            ElseIf "256" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "����"
            ElseIf "512" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "����"
            ElseIf "1024" = drowGrid(ii).Item("�\��_���ڔz�u").ToString.Trim() Then
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "���E"
            Else
                dgv(ii, EM_GRID.emCmb_KHaiti).Value = "�㍶"
            End If

			If drowGrid(ii).Item("�\��_�t�H�[�}�b�g") Is DBNull.Value Then
				dgv(ii, EM_GRID.emTxt_Format).Value = ""
			Else
				dgv(ii, EM_GRID.emTxt_Format).Value = drowGrid(ii).Item("�\��_�t�H�[�}�b�g").ToString.Trim()
			End If
			dgv(ii, EM_GRID.emChk_Frozen).Value = drowGrid(ii).Item("�\��_��Œ�").ToString.Trim()
			dgv(ii, EM_GRID.emCmb_TableName).Value = drowGrid(ii).Item("DB_�e�[�u����").ToString.Trim()

			'�t�B�[���h���擾
			Call GetCmbFieldName(ii)
			dgv(ii, EM_GRID.emCmb_Field).Value = drowGrid(ii).Item("DB_�t�B�[���h��").ToString.Trim()

			dgv(ii, EM_GRID.emChk_Key).Value = drowGrid(ii).Item("DB_��L�[").ToString.Trim()
			If "0" = drowGrid(ii).Item("DB_�t�B�[���h�^�C�v").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "Int32"
			ElseIf "1" = drowGrid(ii).Item("DB_�t�B�[���h�^�C�v").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "Float"
			ElseIf "2" = drowGrid(ii).Item("DB_�t�B�[���h�^�C�v").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "����"
			ElseIf "3" = drowGrid(ii).Item("DB_�t�B�[���h�^�C�v").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "bit"
			Else
				dgv(ii, EM_GRID.emCmb_FieldType).Value = "Int32"
			End If

			dgv(ii, EM_GRID.emTxt_InputMin).Value = drowGrid(ii).Item("����_���͉\�ŏ��l").ToString.Trim()
			dgv(ii, EM_GRID.emTxt_InputMax).Value = drowGrid(ii).Item("����_���͉\�ő�l").ToString.Trim()
			dgv(ii, EM_GRID.emChk_NullOK).Value = drowGrid(ii).Item("����_NULL���͉�").ToString.Trim()
			If drowGrid(ii).Item("����_���͉\����") Is DBNull.Value Then
				dgv(ii, EM_GRID.emTxt_InputKeta).Value = ""
			Else
				dgv(ii, EM_GRID.emTxt_InputKeta).Value = drowGrid(ii).Item("����_���͉\����").ToString.Trim()
			End If
			dgv(ii, EM_GRID.emTxt_DecKeta).Value = drowGrid(ii).Item("����_�����_�ȉ�����").ToString.Trim()

			If "1" = drowGrid(ii).Item("����_�\���_�C�A���O").ToString.Trim() Then
				dgv(ii, EM_GRID.emCmb_DispDialog).Value = "�\��"
			Else
				dgv(ii, EM_GRID.emCmb_DispDialog).Value = "�\������"
			End If
			'��̕��𒲐����܂�
			dgv.AutoResizeColumn(ii)
		Next ii

        '�V�K�o�^�p�̋󔒗��ǉ�
        Call AddDgvRetu(drowGrid.Length)
        '�_�~�[����폜
        dgv.Columns.RemoveAt(drowGrid.Length + 1)
        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
        dgv(0, 0).Selected = True

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

        ''��ʂ���폜���ꂽ����No���擾���A�ŏI�I��DB�o�^�p����No�̕ҏW���s��
        If Not Data_CheckDelete() Then Exit Sub

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

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
        Dim strWk As String     ' dgv�̃f�[�^�i�[�p
        Dim strWk2 As String    ' dgv�̃f�[�^�i�[�p
        Dim strMsg As String    ' �d���p�̃G���[���b�Z�[�W

        For ii As Integer = 0 To dgvGrid.ColumnCount - 1

            '����No
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_KNo).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, ITEM_MAX_COUNT, True, "��ʒu =[" & ii + 1 & "]") <> 0 Then Return False

            ' ����No���󔒂̍s�͖���
            If Trim(strWk) = "" Then Continue For

            ' ����No�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvGrid.ColumnCount - 1
                ' ��r��̍���No�����l�Ȃ��r
                If IsNumeric(dgvGrid(jj, EM_GRID.emTxt_KNo).Value) Then
                    ' ����No�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) = CInt(dgvGrid(jj, EM_GRID.emTxt_KNo).Value) Then
                        strMsg = "����No�d��" & vbNewLine
                        strMsg &= "��ʒu =[" & CStr(ii + 1) & "] �� ��ʒu=[" & CStr(jj + 1) & "]"
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            '���ڃw�b�_�[1
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder1).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "���ڃw�b�_�[1 ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            '���ڃw�b�_�[2
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder2).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "���ڃw�b�_�[2 ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            '���l
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Bikou).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 256, "���l ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            '���ڃZ���^�C�v�y�сAlov_id�̑g�ݍ��킹���픻��
            strWk = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value)
            strWk2 = CStr(dgvGrid(ii, EM_GRID.emCmb_Lov_id).Value)
            If "ComboBox" = strWk Then
                'ComboBox�I�𒆂̏ꍇ�Alov_id���I������Ă��Ȃ��ꍇ�A�ُ�Ƃ���
                If "�ΏۊO" = strWk2 Then
                    strMsg = "lov_id���I������Ă��܂���B" & vbNewLine
                    strMsg &= "����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            Else
                'ComboBox�I�𒆂łȂ��ꍇ�Alov_id���u�ΏۊO�v�łȂ��ꍇ�A�ُ�Ƃ���
                If "�ΏۊO" <> strWk2 Then
                    strMsg = "ComboBox�ȊO�ł�lov_id�̑I���͖����ł��B" & vbNewLine & "�u�ΏۊO�v��I�����Ă��������B" & vbNewLine
                    strMsg &= "����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            End If

            '���ڕ�
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_KHaba).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, True, "���ڕ� ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            '�t�H�[�}�b�g
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_Format).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "�t�H�[�}�b�g ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]", True) <> 0 Then Return False

            '�t�B�[���h�^�C�v
            strWk = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value)
            '���͉\����
            strWk2 = CStr(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value)
            If "����" <> strWk Then
                If "0" <> strWk2 And "" <> Trim(strWk2) Then
                    strMsg = "���͉\�����́A�t�B�[���h�^�C�v�u�����v�̏ꍇ�̂ݗL���ł��B" & vbNewLine
                    strMsg &= "����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            End If
            '�����_�ȉ�����
            strWk2 = CStr(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value)
            If "Float" <> strWk Then
                If "0" <> strWk2 And "" <> Trim(strWk2) Then
                    strMsg = "�����_�ȉ������́A�t�B�[���h�^�C�v�uFloat�v�̏ꍇ�̂ݗL���ł��B" & vbNewLine
                    strMsg &= "����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                    MsgBox(strMsg)
                    Return False
                End If
            End If

            '�t�B�[���h��
            strWk = CStr(dgvGrid(ii, EM_GRID.emCmb_Field).Value)
            If "" = CStr(dgvGrid(ii, EM_GRID.emCmb_Field).Value) Then
                strMsg = "�t�B�[���h�����I������Ă��܂���B" & vbNewLine
                strMsg &= "����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]"
                MsgBox(strMsg)
                Return False
            End If

            '���͉\�ŏ��l
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMin).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, True, "���͉\�ŏ��l ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            '���͉\�ő�l
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMax).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, Int32.MinValue, Int32.MaxValue, True, "���͉\�ő�l ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            '���͉\����
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, True, "���͉\���� ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False

            '�����_�ȉ�����
            strWk = CStr(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Int32.MaxValue, True, "�����_�ȉ����� ����No =[" & CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) & "]") <> 0 Then Return False


        Next ii

        Return True
    End Function

    ''' <summary>
    ''' �폜���ꂽ���ڂ����邩�`�F�b�N����
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_CheckDelete() As Boolean
        Dim bCheck As Boolean   '�`�F�b�N�t���O True�Ȃ�EdasysID������
        Dim nCount As Integer   '�J�E���^
        Dim nPos As Integer     '�ʒu
        Dim nMaxItem As Integer '�ő區��No
        Dim nMinItem As Integer '�ŏ�����No
        Dim bChk As Boolean     '�`�F�b�N����

        '�\�����L�����ڂ�1���������ꍇ�A�`�F�b�N���s��Ȃ�
        nCount = 0
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            If IsNullOrEmptyOrSpace(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) = False _
               AndAlso "" <> CStr(dgvGrid(ii, EM_GRID.emTxt_KNo).Value).Trim Then
                nCount = nCount + 1
            End If
        Next ii
        If 0 = nCount Then Return True

        nCount = 0
        ' �e�[�u���őS�s���[�v
        For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
            If m_Grid.Tables(0).Rows(ii).Item("���ID").ToString = m_DBlst(m_KeyGID).strDBGID Then
                If CInt(m_Grid.Tables(0).Rows(ii).Item("�O���b�hID")) = m_DBlst(m_KeyGID).listGridID(cmbGridID.Text) Then
                    '���ID�y�уO���b�hID����v����ӏ���ΏۂƂ���
                    bCheck = False
                    m_nDelItem(nCount) = 0
                    ' dgv�őS�s���[�v
                    For kk As Integer = 0 To dgvGrid.ColumnCount - 1
                        ' ����No���󔒍s�ȊO�ŁA��r
                        If IsNullOrEmptyOrSpace(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) Then Continue For
                        If CInt(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) = CInt(m_Grid.Tables(0).Rows(ii).Item("����No")) Then
                            ' ����No����v�����Ƃ��A�`�F�b�NOK�Ƃ���
                            bCheck = True
                            Exit For
                        End If
                    Next kk
                    ' ��v���鍀��No�����݂��Ȃ��ꍇ�A�폜���ꂽ�Ɣ��肷��
                    If bCheck = False Then
                        m_nDelItem(nCount) = CInt(m_Grid.Tables(0).Rows(ii).Item("����No"))
                        nCount = nCount + 1
                    End If
                End If
            End If
        Next ii

        For ii As Integer = 0 To dgvGrid.ColumnCount - 1

            ' ����No���󔒍s�ł���Ζ��ݒ�
            If IsNullOrEmptyOrSpace(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) Then
                m_nDispItem(ii) = 0
            Else
                m_nDispItem(ii) = CInt(dgvGrid(ii, EM_GRID.emTxt_KNo).Value)
            End If

        Next ii
        For ii As Integer = 0 To nCount - 1
            '�傫������No����O�ɂ߂�
            nPos = nCount - 1 - ii
            For kk As Integer = 0 To dgvGrid.ColumnCount - 1
                If IsNullOrEmptyOrSpace(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) = False _
                    AndAlso CInt(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) > m_nDelItem(nPos) Then
                    '���ёւ������ʂ����[�N�ɋL�����Ă���
                    'm_nDispItem(ii) = CInt(dgvGrid(kk, EM_GRID.emTxt_KNo).Value) - 1
                    m_nDispItem(kk) = m_nDispItem(kk) - 1
                End If
            Next kk
        Next ii

        'nMinItem = m_nDispItem(0)
        nMinItem = ITEM_MAX_COUNT
        nMaxItem = 0

        '���ёւ������ʂ̍���No�ŏ��l�ƍő�l���擾
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            If 0 >= m_nDispItem(ii) Then Continue For
            If nMinItem > m_nDispItem(ii) Then
                nMinItem = m_nDispItem(ii)
            End If
            If nMaxItem < m_nDispItem(ii) Then
                nMaxItem = m_nDispItem(ii)
            End If
        Next ii

        '���ёւ������ʂ̍���No�ŏ��l��1�łȂ��ꍇ�A�ُ�Ƃ���
        If nMinItem <> 1 Then
            MsgBox("�u����No�v��1����w�肵�Ă��������B")
            Return False
        End If

        '������No���A�����鐔�l���`�F�b�N���s��
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            bChk = False
            '���ёւ������ʂ̍���No�ő�l���Ώۂ̏ꍇ�A������No�̃`�F�b�N���s��Ȃ�
            If m_nDispItem(ii) = nMaxItem Then Continue For
            For kk As Integer = 0 To dgvGrid.ColumnCount - 1
                If m_nDispItem(ii) + 1 = m_nDispItem(kk) Then
                    bChk = True
                    Exit For
                End If
            Next kk
            '����No���A�����Ȃ��ꍇ�A�ُ�Ƃ���
            If False = bChk Then
                MsgBox("�u����No�v�͘A�����鐔�l�Ŏw�肵�Ă��������B")
                Return False
            End If
        Next ii

        Return True

    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        ' ��ڑ��e�[�u���őS�s���[�v
        For ii As Integer = 0 To m_Grid.Tables(0).Rows.Count - 1
            If m_Grid.Tables(0).Rows(ii).Item("���ID").ToString = m_DBlst(m_KeyGID).strDBGID Then
                If CInt(m_Grid.Tables(0).Rows(ii).Item("�O���b�hID")) = m_DBlst(m_KeyGID).listGridID(m_KeyGridID) Then
                    '���ID�y�уO���b�hID����v����ӏ���ΏۂƂ���
                    ' �f���[�g�C���T�[�g����ׁA�������Ă���s���N���A����
                    m_Grid.Tables(0).Rows(ii).Delete()
                End If
            End If
        Next ii

        ' dgv�őS�s���[�v
        For ii As Integer = 0 To dgvGrid.ColumnCount - 1
            ' ����No���󔒂̏ꍇ�A�X�L�b�v����
            If IsNullOrEmptyOrSpace(dgvGrid(ii, EM_GRID.emTxt_KNo).Value) Then Continue For
            Dim drowGrid As DataRow = m_Grid.Tables(0).NewRow()                                                ' �V�K�s

            drowGrid.Item("���ID") = m_DBlst(m_KeyGID).strDBGID
            drowGrid.Item("�O���b�hID") = m_DBlst(cmbGID.Text).listGridID(m_KeyGridID)

            '�\���������Z�b�g
            drowGrid.Item("����No") = m_nDispItem(ii)
            drowGrid.Item("���ڃw�b�_�[1") = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder1).Value)
            drowGrid.Item("���ڃw�b�_�[2") = CStr(dgvGrid(ii, EM_GRID.emTxt_Hedder2).Value)
            drowGrid.Item("���l") = CStr(dgvGrid(ii, EM_GRID.emTxt_Bikou).Value)

            If "TextBox" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("���ڃZ���^�C�v") = 0
            ElseIf "ComboBox" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("���ڃZ���^�C�v") = 1
            ElseIf "CheckBox" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("���ڃZ���^�C�v") = 2
            ElseIf "Button" = CStr(dgvGrid(ii, EM_GRID.emCmb_CellType).Value) Then
                drowGrid.Item("���ڃZ���^�C�v") = 3
            Else
                drowGrid.Item("���ڃZ���^�C�v") = 0
            End If

            If "�ΏۊO" = CStr(dgvGrid(ii, EM_GRID.emCmb_Lov_id).Value) Then
				drowGrid.Item("lov_id") = System.DBNull.Value
            Else
                drowGrid.Item("lov_id") = CInt(dgvGrid(ii, EM_GRID.emCmb_Lov_id).Value)
            End If

            If CBool(dgvGrid(ii, EM_GRID.emChk_DispUmu).Value) Then
                drowGrid.Item("�\��_���ڕ\���L��") = 1
            Else
                drowGrid.Item("�\��_���ڕ\���L��") = 0
            End If

            If CBool(dgvGrid(ii, EM_GRID.emChk_ReadOnly).Value) Then
                drowGrid.Item("�\��_�ǂݎ���p") = 1
            Else
                drowGrid.Item("�\��_�ǂݎ���p") = 0
            End If
			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_KHaba).Value) Then
				drowGrid.Item("�\��_���ڕ�") = System.DBNull.Value
			Else
				drowGrid.Item("�\��_���ڕ�") = CInt(dgvGrid(ii, EM_GRID.emTxt_KHaba).Value)
			End If

            If "�㍶" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 1
            ElseIf "�㒆" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 2
            ElseIf "��E" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 4
            ElseIf "����" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 16
            ElseIf "����" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 32
            ElseIf "���E" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 64
            ElseIf "����" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 256
            ElseIf "����" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 512
            ElseIf "���E" = CStr(dgvGrid(ii, EM_GRID.emCmb_KHaiti).Value) Then
                drowGrid.Item("�\��_���ڔz�u") = 1024
            Else
                drowGrid.Item("�\��_���ڔz�u") = 1
            End If

			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_Format).Value) Then
				drowGrid.Item("�\��_�t�H�[�}�b�g") = System.DBNull.Value
			Else
				drowGrid.Item("�\��_�t�H�[�}�b�g") = CStr(dgvGrid(ii, EM_GRID.emTxt_Format).Value)
			End If

			If CBool(dgvGrid(ii, EM_GRID.emChk_Frozen).Value) Then
				drowGrid.Item("�\��_��Œ�") = 1
			Else
				drowGrid.Item("�\��_��Œ�") = 0
			End If

			drowGrid.Item("DB_�e�[�u����") = CStr(dgvGrid(ii, EM_GRID.emCmb_TableName).Value)
			drowGrid.Item("DB_�t�B�[���h��") = CStr(dgvGrid(ii, EM_GRID.emCmb_Field).Value)

			If CBool(dgvGrid(ii, EM_GRID.emChk_Key).Value) Then
				drowGrid.Item("DB_��L�[") = 1
			Else
				drowGrid.Item("DB_��L�[") = 0
			End If

			If "Int32" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_�t�B�[���h�^�C�v") = 0
			ElseIf "Float" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_�t�B�[���h�^�C�v") = 1
			ElseIf "����" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_�t�B�[���h�^�C�v") = 2
			ElseIf "bit" = CStr(dgvGrid(ii, EM_GRID.emCmb_FieldType).Value) Then
				drowGrid.Item("DB_�t�B�[���h�^�C�v") = 3
			Else
				drowGrid.Item("DB_�t�B�[���h�^�C�v") = 0
			End If

			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMin).Value) Then
				drowGrid.Item("����_���͉\�ŏ��l") = System.DBNull.Value
			Else
				drowGrid.Item("����_���͉\�ŏ��l") = CDbl(dgvGrid(ii, EM_GRID.emTxt_InputMin).Value)
			End If
			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_InputMax).Value) Then
				drowGrid.Item("����_���͉\�ő�l") = System.DBNull.Value
			Else
				drowGrid.Item("����_���͉\�ő�l") = CDbl(dgvGrid(ii, EM_GRID.emTxt_InputMax).Value)
			End If

			If CBool(dgvGrid(ii, EM_GRID.emChk_NullOK).Value) Then
				drowGrid.Item("����_NULL���͉�") = 1
			Else
				drowGrid.Item("����_NULL���͉�") = 0
			End If

			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value) Then
				drowGrid.Item("����_���͉\����") = System.DBNull.Value
			Else
				drowGrid.Item("����_���͉\����") = CInt(dgvGrid(ii, EM_GRID.emTxt_InputKeta).Value)
			End If
			If "" = CStr(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value) Then
				drowGrid.Item("����_�����_�ȉ�����") = System.DBNull.Value
			Else
				drowGrid.Item("����_�����_�ȉ�����") = CInt(dgvGrid(ii, EM_GRID.emTxt_DecKeta).Value)
			End If

			If "�\��" = CStr(dgvGrid(ii, EM_GRID.emCmb_DispDialog).Value) Then
				drowGrid.Item("����_�\���_�C�A���O") = 1
			Else
				drowGrid.Item("����_�\���_�C�A���O") = 0
			End If

			m_Grid.Tables(0).Rows.Add(drowGrid)		'�s��ǉ�
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
            m_AdapterGrid.SelectCommand.Connection.Open()

            '�g�����U�N�V�����J�n
            transact = m_AdapterGrid.SelectCommand.Connection.BeginTransaction()
            m_AdapterGrid.SelectCommand.Transaction = transact

            Using cbWk As New SqlClient.SqlCommandBuilder()

                '�R�}���h�A�_�v�^���Z�b�g���āA�X�V
                cbWk.DataAdapter = m_AdapterGrid
                m_AdapterGrid.Update(m_Grid)

            End Using

            '�R�~�b�g
            transact.Commit()

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
            If m_AdapterGrid.SelectCommand.Connection.State = ConnectionState.Open Then
                m_AdapterGrid.SelectCommand.Connection.Close()
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
    ''' dgv�̗��`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeGrid(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                       ' �I��F����

        With dgv
            .ColumnHeadersHeight = 42                                   ' �w�b�_�[�̍���
            .AllowUserToResizeColumns = True                            ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Both                               ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            '�s�w�b�_�[�ݒ�
            .RowHeadersWidth = 140                                   ' �w�b�_�[�̉���
            .RowHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            '.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' �I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' ��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            .ColumnHeadersVisible = False
            .RowHeadersVisible = True

        End With

        ''���`
        ' �_�~�[��
		dgvColSetText(dgv, "", 140, True, False, _
  DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

    End Sub

    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv
        dgv.CurrentCell.Selected = False
    End Sub

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��

    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h �`�F�b�N�{�b�N�X�Z���o�^
    ''' </summary>
    ''' <param name="nPos">��ʒu</param>
    ''' <param name="nIndex">�s�ʒu</param>
    ''' <remarks></remarks>
    Private Sub CellSetDataGridViewCheckBox(ByVal nPos As Integer, ByVal nIndex As Integer)

        Using clm As New DataGridViewCheckBoxCell
            Dim wkCell As DataGridViewCheckBoxCell = CType(clm, DataGridViewCheckBoxCell)
            wkCell.Style.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            dgvGrid(nPos, nIndex) = clm
            If EM_GRID.emChk_Key = nIndex Then
                dgvGrid(nPos, nIndex).ReadOnly = True
            ElseIf EM_GRID.emChk_NullOK = nIndex Then
                dgvGrid(nPos, nIndex).ReadOnly = True
            End If
        End Using
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h �e�L�X�g�{�b�N�X�Z���o�^
    ''' </summary>
    ''' <param name="nPos">��ʒu</param>
    ''' <param name="nIndex">�s�ʒu</param>
    ''' <param name="nType">�\���^�w��(1:���l �ȊO:����)</param>
    ''' <remarks></remarks>
    Private Sub CellSetDataGridViewTextBox(ByVal nPos As Integer, ByVal nIndex As Integer, Optional ByVal nType As Integer = Nothing)

        Using clm As New DataGridViewTextBoxCell
            Dim wkCell As DataGridViewTextBoxCell = CType(clm, DataGridViewTextBoxCell)
            wkCell.Style.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            If 1 = nType Then
                '���l�̏ꍇ�A�E�񂹕\��
                wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleRight
            Else
                '�����̏ꍇ�A���񂹕\��
                wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            End If
            dgvGrid(nPos, nIndex) = clm
        End Using
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h �R���{�{�b�N�X�Z���o�^
    ''' </summary>
    ''' <param name="nPos">��ʒu</param>
    ''' <param name="nIndex">�s�ʒu</param>
    ''' <remarks></remarks>
    Private Sub CellSetDataGridViewComboBox(ByVal nPos As Integer, ByVal nIndex As Integer)

        Using clm As New DataGridViewComboBoxCell
            Dim wkCell As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
            Dim ii As Integer
            wkCell.Style.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            Dim listItemFF As DataGridViewComboBoxCell = CType(wkCell, DataGridViewComboBoxCell)

            If EM_GRID.emCmb_CellType = nIndex Then
                '���ڃZ���^�C�v
                listItemFF.Items.AddRange("TextBox", "ComboBox", "CheckBox", "Button")
            ElseIf EM_GRID.emCmb_Lov_id = nIndex Then
                'lov_id
                Dim drowLov_id() As DataRow = m_Lov_id.Tables(0).Select()                   ' Lov_id
                listItemFF.Items.Add("�ΏۊO")
                'DB��ɓo�^����Ă���lov_id��I�����ڂƂ���
                For ii = 0 To drowLov_id.Length - 1
                    listItemFF.Items.Add(drowLov_id(ii).Item("lov_id").ToString)
                Next ii
            ElseIf EM_GRID.emCmb_KHaiti = nIndex Then
                '���ڔz�u
                listItemFF.Items.AddRange("�㍶", "�㒆", "��E", "����", "����", "���E", "����", "����", "���E")
            ElseIf EM_GRID.emCmb_TableName = nIndex Then
                '�e�[�u����
                ' <20221117> <����> <�ŏI�H�����@�\������1> --->>>
                'listItemFF.Items.AddRange(DB_PARAM_CAMANGLE_SCODE, DB_PARAM_CAMSET, DB_PARAM_CAMSET_ANGLE, DB_PARAM_CAMSET_ANGLE_SCODE, DB_PARAM_CAMSET_SCODE, DB_PARAM_COMMON, DB_PARAM_SCODE)
                listItemFF.Items.AddRange(DB_PARAM_CAMANGLE_SCODE, DB_PARAM_CAMSET, DB_PARAM_CAMSET_ANGLE, DB_PARAM_CAMSET_ANGLE_SCODE, DB_PARAM_CAMSET_SCODE, DB_PARAM_COMMON, DB_PARAM_SCODE, DB_PARAM_FILTERBIN, DB_PARAM_LABELIMAGE, DB_PARAM_SIMPLEBIN)
                ' <20221117> <����> <�ŏI�H�����@�\������1> ---<<<
                'listItemFF.Items.AddRange(DB_PARAM_CAMANGLE_SCODE, DB_PARAM_CAMSET, DB_PARAM_SIDE, DB_PARAM_SIDE_SCODE, DB_PARAM_CAMSET_SCODE, DB_PARAM_COMMON, DB_PARAM_SCODE)
            ElseIf EM_GRID.emCmb_FieldType = nIndex Then
				'�t�B�[���h�^�C�v
				listItemFF.Items.AddRange("Int32", "Float", "����", "bit")
            ElseIf EM_GRID.emCmb_DispDialog = nIndex Then
                '�\���_�C�A���O
                listItemFF.Items.AddRange("�\��", "�\������")
            End If
            '''''''''''''''''''''''''''''''''''''''''''''''
            dgvGrid(nPos, nIndex) = clm
            If EM_GRID.emCmb_FieldType = nIndex Then
                dgvGrid(nPos, nIndex).ReadOnly = True
            End If
        End Using
    End Sub

    ''' <summary>
    ''' �f�[�^�O���b�h�\�����ڕύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGrid_CellValueChanged(ByVal sender As Object, ByVal e As DataGridViewCellEventArgs) Handles dgvGrid.CellValueChanged
        Dim nWkx As Integer = 0
        If 0 > e.ColumnIndex Then Exit Sub
        '���ڃZ���^�C�v��ύX�����ꍇ�Alov_id���擾����
        If EM_GRID.emCmb_CellType = e.RowIndex Then
            Call SetCmbCellTypeChgData(e.ColumnIndex)
        End If
        '�e�[�u������ύX�����ꍇ�A�t�B�[���h������蒼��
        If EM_GRID.emCmb_TableName = e.RowIndex Then
            Call GetCmbFieldName(e.ColumnIndex)
        End If
        '�t�B�[���h����ύX�����ꍇ�A��L�[�A�t�B�[���h�^�C�v�ANULL���͉ۂ��擾����
        If EM_GRID.emCmb_Field = e.RowIndex Then
            Call SetCmbFieldChgData(e.ColumnIndex)
        End If
        '�t�B�[���h�^�C�v��ύX�����ꍇ�A���͉\�����y�я����_�ȉ��������擾����
        If EM_GRID.emCmb_FieldType = e.RowIndex Then
            Call SetCmbFieldTypeChgData(e.ColumnIndex)
        End If
        '�ŏI�s�̃O���b�hID��ǉ������ꍇ�A���̓��̓G���A��ǉ�����
        If EM_GRID.emTxt_KNo = e.RowIndex And (dgvGrid.Columns.Count - 1) = e.ColumnIndex Then
            Call AddDgvRetu(e.ColumnIndex + 1)
        End If
    End Sub
    ''' <summary>
    ''' �f�[�^�O���b�h��ɗ��ǉ�����
    ''' </summary>
    ''' <param name="nIndex">�ǉ��ʒu</param>
    ''' <remarks></remarks>
    Private Sub AddDgvRetu(ByVal nIndex As Integer)
        '�V�K���ǉ�
		dgvGrid.Columns.Add(CStr(nIndex), CStr(nIndex))
		''�����͎�����������
		'dgvGrid.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.DisplayedCellsExceptHeader
		'�Z�������`
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_KNo, 1)            '����No
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Hedder1)           '���ڃw�b�_�[�P
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Hedder2)           '���ڃw�b�_�[�Q
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Bikou)             '���l
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_CellType)         '���ڃZ���^�C�v
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_Lov_id)           'lov_id
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_DispUmu)          '���ڕ\���L��
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_ReadOnly)         '�ǂݎ���p
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_KHaba, 1)          '���ڕ�
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_KHaiti)           '���ڔz�u
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_Format)            '�t�H�[�}�b�g
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_Frozen)           '��Œ�
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_TableName)        'DB�e�[�u����
        'DB�t�B�[���h���͂����ł͒�`���Ȃ�(DB�e�[�u������\����ɒ�`����)
        'CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_Field)            '
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_Key)              'DB��L�[
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_FieldType)        'DB�t�B�[���h�^�C�v
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_InputMin, 1)       '�ŏ��l
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_InputMax, 1)       '�ő�l
        CellSetDataGridViewCheckBox(nIndex, EM_GRID.emChk_NullOK)           'NULL���͉�
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_InputKeta, 1)      '���͉\����
        CellSetDataGridViewTextBox(nIndex, EM_GRID.emTxt_DecKeta, 1)        '�����_�ȉ�����
        CellSetDataGridViewComboBox(nIndex, EM_GRID.emCmb_DispDialog)       '�\���_�C�A���O
        '�����l�Z�b�g
        dgvGrid(nIndex, EM_GRID.emCmb_CellType).Value = "TextBox"
        dgvGrid(nIndex, EM_GRID.emCmb_Lov_id).Value = "�ΏۊO"
        dgvGrid(nIndex, EM_GRID.emChk_DispUmu).Value = False
        dgvGrid(nIndex, EM_GRID.emChk_ReadOnly).Value = False
        dgvGrid(nIndex, EM_GRID.emTxt_KHaba).Value = 0
        dgvGrid(nIndex, EM_GRID.emCmb_KHaiti).Value = "�㍶"
        dgvGrid(nIndex, EM_GRID.emChk_Frozen).Value = False
        dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value = "PARAM_CAMSET"
        dgvGrid(nIndex, EM_GRID.emCmb_Field).Value = "camset"
        dgvGrid(nIndex, EM_GRID.emChk_Key).Value = True
        dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Int32"
        dgvGrid(nIndex, EM_GRID.emChk_NullOK).Value = False
        dgvGrid(nIndex, EM_GRID.emTxt_InputKeta).Value = 0
        dgvGrid(nIndex, EM_GRID.emTxt_DecKeta).Value = 0
        dgvGrid(nIndex, EM_GRID.emCmb_DispDialog).Value = "�\������"
    End Sub

    ''' <summary>
    ''' ���ID�R���{�{�b�N�X��������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetComboBox_GID()

        '���ID�R���{�{�b�N�X�����ް��o�^����
        '�Œ�p�����[�^�ݒ���
        If "" <> EPC_COMMON_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_COMMON_GRID_NAME, EPC_COMMON_GRID_GID, 1))
        End If
        '���W�b�N�p�������ݒ���
        If "" <> EPC_PARA_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_PARA_GRID_NAME, EPC_PARA_GRID_GID, 1))
        End If
        '�J�����ݒ���
        If "" <> EPC_CAMERA_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_CAMERA_GRID_NAME, EPC_CAMERA_GRID_GID, 1))
        End If
		'���F�����ݒ���
		If "" <> EPC_MIXCOLOR_GRID_GID Then
			cmbGID.Items.Add(New cls_CmbItem(EPC_MIXCOLOR_GRID_NAME, EPC_MIXCOLOR_GRID_GID, 1))
		End If
		'�G�b�W���o�ݒ���
        If "" <> EPC_EDGE_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_EDGE_GRID_NAME, EPC_EDGE_GRID_GID, 1))
        End If
        '����ިݸޕ␳�ݒ���
        If "" <> EPC_SHADING_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_SHADING_GRID_NAME, EPC_SHADING_GRID_GID, 1))
        End If
        '臒l�ݒ���
        If "" <> EPC_THRESHOLD_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_THRESHOLD_GRID_NAME, EPC_THRESHOLD_GRID_GID, 1))
        End If
        '�r���o�ݒ���
        If "" <> EPC_DEFECT_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_DEFECT_GRID_NAME, EPC_DEFECT_GRID_GID, 1))
        End If
        '���������ݒ���
        If "" <> EPC_MERGE_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_MERGE_GRID_NAME, EPC_MERGE_GRID_GID, 1))
        End If
        ' <20221117> <����> <�ŏI�H�����@�\������1> --->>>
        '�d�o�b���d����\臒l�ݒ���
        If "" <> EPC_TAJUUTHRESHOLD_GRID_GID Then
            cmbGID.Items.Add(New cls_CmbItem(EPC_TAJUUTHRESHOLD_GRID_NAME, EPC_TAJUUTHRESHOLD_GRID_GID, 1))
        End If
        ' <20221117> <����> <�ŏI�H�����@�\������1> ---<<<

        '�����I����Ԃ��Z�b�g����
        cmbGID.Text = GRID_FIRST_DISP_NAME
        cmbGID.SelectedValue = GRID_FIRST_DISP_GID

        '-------------------------------------
        '��ʕ\���O���b�hID�擾�p�����쐬����
        '-------------------------------------
        ' �Œ�p�����[�^�ݒ���
        SetComboBoxListData(EPC_COMMON_GRID, EPC_COMMON_GRID_ID, EPC_COMMON_GRID_CNT, EPC_COMMON_GRID_GID, EPC_COMMON_GRID_NAME)
        '-------------------------------------
        ' ���W�b�N�p�������ݒ���
        SetComboBoxListData(EPC_PARA_GRID, EPC_PARA_GRID_ID, EPC_PARA_GRID_CNT, EPC_PARA_GRID_GID, EPC_PARA_GRID_NAME)
        '-------------------------------------
        ' �J�����ݒ���
        SetComboBoxListData(EPC_CAMERA_GRID, EPC_CAMERA_GRID_ID, EPC_CAMERA_GRID_CNT, EPC_CAMERA_GRID_GID, EPC_CAMERA_GRID_NAME)
		'-------------------------------------
		' ���F�����ݒ���
		SetComboBoxListData(EPC_MIXCOLOR_GRID, EPC_MIXCOLOR_GRID_ID, EPC_MIXCOLOR_GRID_CNT, EPC_MIXCOLOR_GRID_GID, EPC_MIXCOLOR_GRID_NAME)
		'-------------------------------------
        '�G�b�W���o�ݒ���
        SetComboBoxListData(EPC_EDGE_GRID, EPC_EDGE_GRID_ID, EPC_EDGE_GRID_CNT, EPC_EDGE_GRID_GID, EPC_EDGE_GRID_NAME)
        '-------------------------------------
        '����ިݸޕ␳�ݒ���
        SetComboBoxListData(EPC_SHADING_GRID, EPC_SHADING_GRID_ID, EPC_SHADING_GRID_CNT, EPC_SHADING_GRID_GID, EPC_SHADING_GRID_NAME)
        '-------------------------------------
        '臒l�ݒ���
        SetComboBoxListData(EPC_THRESHOLD_GRID, EPC_THRESHOLD_GRID_ID, EPC_THRESHOLD_GRID_CNT, EPC_THRESHOLD_GRID_GID, EPC_THRESHOLD_GRID_NAME)
        '-------------------------------------
        '�r���o�ݒ���
        SetComboBoxListData(EPC_DEFECT_GRID, EPC_DEFECT_GRID_ID, EPC_DEFECT_GRID_CNT, EPC_DEFECT_GRID_GID, EPC_DEFECT_GRID_NAME)
        '-------------------------------------
        '���������ݒ���
        SetComboBoxListData(EPC_MERGE_GRID, EPC_MERGE_GRID_ID, EPC_MERGE_GRID_CNT, EPC_MERGE_GRID_GID, EPC_MERGE_GRID_NAME)
        '-------------------------------------
        ' <20221117> <����> <�ŏI�H�����@�\������1> --->>>
        '�d�o�b���d����\臒l�ݒ���
        SetComboBoxListData(EPC_TAJUUTHRESHOLD_GRID, EPC_TAJUUTHRESHOLD_GRID_ID, EPC_TAJUUTHRESHOLD_GRID_CNT, EPC_TAJUUTHRESHOLD_GRID_GID, EPC_TAJUUTHRESHOLD_GRID_NAME)
        ' <20221117> <����> <�ŏI�H�����@�\������1> ---<<<

    End Sub

    ''' <summary>
    ''' ��ʕ\���O���b�hID�擾�p�����쐬����
    ''' </summary>
    ''' <param name="strGrid">�O���b�h�h�c��ʕ\������</param>
    ''' <param name="strGridID">�O���b�h�h�c</param>
    ''' <param name="nGridCnt">�O���b�h��</param>
    ''' <param name="strGID">��ʂh�c</param>
    ''' <param name="strGridName">��ʂh�c��ʕ\������</param>
    ''' <remarks></remarks>
    Private Sub SetComboBoxListData(ByVal strGrid As String, ByVal strGridID As String, ByVal nGridCnt As Integer, ByVal strGID As String, ByVal strGridName As String)
        Dim strWk As String
        Dim strWklst As String()                                    '��ʕ\���O���b�hID�z����擾���[�N
        Dim strWklst2 As String()                                   'DB�O���b�hID�z����擾���[�N
        Dim gid_inf As New GID_INFO                                 '���ID�ɕR�t�����i�[�G���A
        Dim listGrid As New Dictionary(Of Integer, String)          '�O���b�hID��KEY�Ƃ�����ʕ\���O���b�hID�擾�p���
        Dim listGridID As New Dictionary(Of String, Integer)        '��ʕ\���O���b�hID��KEY�Ƃ����O���b�hID�擾�p���

        strWk = strGrid
        strWklst = strWk.Split(","c)
        strWk = strGridID
        strWklst2 = strWk.Split(","c)
        For ii As Integer = 0 To nGridCnt - 1
            listGrid.Add(ii + 1, strWklst(ii))
            listGridID.Add(strWklst(ii), CInt(strWklst2(ii)))
        Next ii
        gid_inf.strDBGID = strGID
        gid_inf.nGridCnt = nGridCnt
        gid_inf.listGrid = listGrid
        gid_inf.listGridID = listGridID
        m_DBlst.Add(strGridName, gid_inf)
    End Sub
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    ''' <summary>
    ''' �e�[�u���ʃt�B�[���h���擾����
    ''' </summary>
    ''' <param name="nIndex"></param>
    ''' <remarks></remarks>
    Private Sub GetCmbFieldName(ByVal nIndex As Integer)
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""    '�G���[���b�Z�[�W
        Dim strDBName As String         '�e�[�u����

        '�e�[�u�������擾
        strDBName = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value)
        strSQL = "SELECT name FROM dbo.syscolumns"
        strSQL &= " WHERE ID IN("
        strSQL &= " SELECT ID FROM dbo.sysobjects"
        strSQL &= " WHERE NAME = '" & strDBName & "' )"
        strSQL &= " ORDER BY ID"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_Field, m_AdapterField, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, strDBName & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return
        End If

        Dim drowField() As DataRow = m_Field.Tables(0).Select()          ' �S���R�[�h

        '�擾�����t�B�[���h�����R���{�{�b�N�X�ɓo�^����
        Using clm As New DataGridViewComboBoxCell
            Dim wkCell As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
            wkCell.Style.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            wkCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            Dim listItemFF As DataGridViewComboBoxCell = CType(wkCell, DataGridViewComboBoxCell)
            For jj As Integer = 0 To drowField.Length - 1
                listItemFF.Items.Add(drowField(jj).Item("name").ToString)
            Next jj
            dgvGrid(nIndex, EM_GRID.emCmb_Field).Value = ""
            dgvGrid(nIndex, EM_GRID.emCmb_Field) = clm
        End Using
    End Sub

    ''' <summary>
    ''' �t�B�[���h���ύX���t�B�[���h���ɑΉ���������擾���\������
    ''' </summary>
    ''' <param name="nIndex">��ʒu</param>
    ''' <remarks></remarks>
    Private Sub SetCmbFieldChgData(ByVal nIndex As Integer)
        Dim strSQL As String = ""
        Dim strErrMsg As String = ""    '�G���[���b�Z�[�W
        Dim strDBName As String         '�e�[�u����
        Dim strField As String          '�t�B�[���h��

        If m_bDispFlg Then Exit Sub '�\���������̏ꍇ�͎�t�𖳎�����        

        '�e�[�u�������Ȃ��ꍇ�A�������s��Ȃ�
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value) Then Exit Sub
        '�t�B�[���h�����Ȃ��ꍇ�A�������s��Ȃ�
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value) Then Exit Sub

        '�e�[�u�������擾
        strDBName = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value)
        strSQL = "SELECT "
        strSQL &= " cols.name AS col_name"
        strSQL &= " FROM"
        strSQL &= " sys.tables AS tbls"
        strSQL &= " INNER JOIN sys.key_constraints AS key_const ON"
        strSQL &= " tbls.object_id = key_const.parent_object_id"
        strSQL &= " AND key_const.type = 'PK'"
        strSQL &= " AND tbls.name = '" & strDBName & "'"
        strSQL &= " INNER JOIN sys.index_columns AS idx_cols ON"
        strSQL &= " key_const.parent_object_id = idx_cols.object_id"
        strSQL &= " AND key_const.unique_index_id = idx_cols.index_id"
        strSQL &= " INNER JOIN sys.columns AS cols ON"
        strSQL &= " idx_cols.object_id = cols.object_id"
        strSQL &= " AND idx_cols.column_id = cols.column_id"
        '''''''''''''''''''''''''''''
        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DBKey, m_AdapterDBKey, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, strDBName & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return
        End If

        Dim drowDBKey() As DataRow = m_DBKey.Tables(0).Select()          ' �S���R�[�h
        dgvGrid(nIndex, EM_GRID.emChk_Key).Value = False
        For ii As Integer = 0 To drowDBKey.Length - 1
            '��L�[�o�^����Ă���t�B�[���h���ƈ�v����ꍇ
            If drowDBKey(ii).Item("col_name").ToString = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value) Then
                '��L�[�Ƃ���
                dgvGrid(nIndex, EM_GRID.emChk_Key).Value = True
            End If
        Next ii

        '�t�B�[���h�����擾
        strField = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value)
        'NULL���͉ۋy�уt�B�[���h�^�C�v���擾����
        strSQL = "SELECT is_nullable, data_type FROM information_schema.columns"
        strSQL &= " WHERE table_name = '" & strDBName & "'"
        strSQL &= " AND column_name = '" & strField & "'"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DBAttr, m_AdapterDBAttr, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, strDBName & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return
        End If

        Dim drowDBAttr() As DataRow = m_DBAttr.Tables(0).Select()          ' ���R�[�h

        'NULL���͉ۂ��擾
        If drowDBAttr(0).Item("is_nullable").ToString = "YES" Then
            dgvGrid(nIndex, EM_GRID.emChk_NullOK).Value = True
        Else
            dgvGrid(nIndex, EM_GRID.emChk_NullOK).Value = False
        End If

        '�t�B�[���h�^�C�v���擾
        If drowDBAttr(0).Item("data_type").ToString = "int" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Int32"
        ElseIf drowDBAttr(0).Item("data_type").ToString = "float" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Float"
        ElseIf drowDBAttr(0).Item("data_type").ToString = "varchar" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "����"
        ElseIf drowDBAttr(0).Item("data_type").ToString = "bit" Then
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "bit"
        Else
            dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value = "Int32"
        End If

    End Sub
    ''' <summary>
    ''' �t�B�[���h�^�C�v�ύX���A�Ή���������擾����
    ''' </summary>
    ''' <param name="nIndex">��ʒu</param>
    ''' <remarks></remarks>
    Private Sub SetCmbFieldTypeChgData(ByVal nIndex As Integer)
        If m_bDispFlg Then Exit Sub '�\���������̏ꍇ�͎�t�𖳎�����        
        '�e�[�u�������Ȃ��ꍇ�A�������s��Ȃ�
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_TableName).Value) Then Exit Sub
        '�t�B�[���h�����Ȃ��ꍇ�A�������s��Ȃ�
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_Field).Value) Then Exit Sub
        '�t�B�[���h�^�C�v���Ȃ��ꍇ�A�������s��Ȃ�
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value) Then Exit Sub

        '�t�B�[���h�^�C�v ���� �ȊO�̏ꍇ
        If "����" <> CStr(dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value) Then
            '���͉\������0�Ƃ���
            dgvGrid(nIndex, EM_GRID.emTxt_InputKeta).Value = 0
        End If
        '�t�B�[���h�^�C�v Float �ȊO�̏ꍇ
        If "Float" <> CStr(dgvGrid(nIndex, EM_GRID.emCmb_FieldType).Value) Then
            '�����_�ȉ����͉\������0�Ƃ���
            dgvGrid(nIndex, EM_GRID.emTxt_DecKeta).Value = 0
        End If

    End Sub
    ''' <summary>
    ''' ���ڃZ���^�C�v�ύX���A�Ή���������擾����
    ''' </summary>
    ''' <param name="nIndex">��ʒu</param>
    ''' <remarks></remarks>
    Private Sub SetCmbCellTypeChgData(ByVal nIndex As Integer)
        If m_bDispFlg Then Exit Sub '�\���������̏ꍇ�͎�t�𖳎�����        
        '���ڃZ���^�C�v���Ȃ��ꍇ�A�������s��Ȃ�
        If "" = CStr(dgvGrid(nIndex, EM_GRID.emCmb_CellType).Value) Then Exit Sub

        '�t�B�[���h�^�C�v ���� �ȊO�̏ꍇ
        If "ConboBox" <> CStr(dgvGrid(nIndex, EM_GRID.emCmb_CellType).Value) Then
            'lov_id��ΏۊO�Ƃ���
            dgvGrid(nIndex, EM_GRID.emCmb_Lov_id).Value = "�ΏۊO"
        End If

    End Sub


    ''' <summary>
    ''' �s�w�b�_�[�^�C�g�����Z�b�g����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDgvTitle()
        dgvGrid.Rows(EM_GRID.emTxt_KNo).HeaderCell.Value = "����No"
        dgvGrid.Rows(EM_GRID.emTxt_Hedder1).HeaderCell.Value = "���ڃw�b�_�[�P"
        dgvGrid.Rows(EM_GRID.emTxt_Hedder2).HeaderCell.Value = "���ڃw�b�_�[�Q"
        dgvGrid.Rows(EM_GRID.emTxt_Bikou).HeaderCell.Value = "���l"
        dgvGrid.Rows(EM_GRID.emCmb_CellType).HeaderCell.Value = "���ڃZ���^�C�v"
        dgvGrid.Rows(EM_GRID.emCmb_Lov_id).HeaderCell.Value = "lov_id"
        dgvGrid.Rows(EM_GRID.emChk_DispUmu).HeaderCell.Value = "���ڕ\���L��"
        dgvGrid.Rows(EM_GRID.emChk_ReadOnly).HeaderCell.Value = "�ǂݎ���p"
        dgvGrid.Rows(EM_GRID.emTxt_KHaba).HeaderCell.Value = "���ڕ�"
        dgvGrid.Rows(EM_GRID.emCmb_KHaiti).HeaderCell.Value = "���ڔz�u"
        dgvGrid.Rows(EM_GRID.emTxt_Format).HeaderCell.Value = "�t�H�[�}�b�g"
        dgvGrid.Rows(EM_GRID.emChk_Frozen).HeaderCell.Value = "��Œ�"
        dgvGrid.Rows(EM_GRID.emCmb_TableName).HeaderCell.Value = "�e�[�u����"
        dgvGrid.Rows(EM_GRID.emCmb_Field).HeaderCell.Value = "�t�B�[���h��"
        dgvGrid.Rows(EM_GRID.emChk_Key).HeaderCell.Value = "��L�["
        dgvGrid.Rows(EM_GRID.emCmb_FieldType).HeaderCell.Value = "�t�B�[���h�^�C�v"
        dgvGrid.Rows(EM_GRID.emTxt_InputMin).HeaderCell.Value = "���͉\�ŏ��l"
        dgvGrid.Rows(EM_GRID.emTxt_InputMax).HeaderCell.Value = "���͉\�ő�l"
        dgvGrid.Rows(EM_GRID.emChk_NullOK).HeaderCell.Value = "NULL���͉�"
        dgvGrid.Rows(EM_GRID.emTxt_InputKeta).HeaderCell.Value = "���͉\����"
        dgvGrid.Rows(EM_GRID.emTxt_DecKeta).HeaderCell.Value = "�����_�ȉ�����"
        dgvGrid.Rows(EM_GRID.emCmb_DispDialog).HeaderCell.Value = "�\���_�C�A���O"
    End Sub

    ''' <summary>
    ''' ���ID�R���{�{�b�N�X�؂�ւ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbGID_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbGID.SelectedIndexChanged
        If "" = cmbGID.Text Then Exit Sub
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂ŁA ���ɖ߂����ꍇ�̑΍�)
        If m_KeyGID = cmbGID.Text Then Exit Sub

        If "" = m_KeyGID Then
            '��ʏ����������ł̃C�x���g��t�͕\���������s��Ȃ�
            m_KeyGID = cmbGID.Text
            Exit Sub
        End If

        m_KeyGID = cmbGID.Text
        Call SetComboBox_GridID()
        Data_Disp()

    End Sub

    ''' <summary>
    ''' �O���b�hID�؂�ւ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbGridID_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbGridID.SelectedIndexChanged
        If "" = cmbGridID.Text Then Exit Sub
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂ŁA ���ɖ߂����ꍇ�̑΍�)
        If m_KeyGridID = cmbGridID.Text Then Exit Sub

        If "" = m_KeyGridID Then
            '��ʏ����������ł̃C�x���g��t�͕\���������s��Ȃ�
            m_KeyGridID = cmbGridID.Text
            Exit Sub
        End If

        m_KeyGridID = cmbGridID.Text
        Data_Disp()

    End Sub

    ''' <summary>
    ''' �O���b�hID�o�^
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetComboBox_GridID()
        cmbGridID.Items.Clear()
        '���ID�R���{�{�b�N�X�����ް��o�^����
        '�\�������ID�ɕR�t���O���b�hID�̉�ʕ\�������擾����
        For ii As Integer = 1 To m_DBlst(cmbGID.Text).nGridCnt
            cmbGridID.Items.Add(m_DBlst(cmbGID.Text).listGrid(ii))
        Next ii

        '�����I����Ԃ��Z�b�g����
        cmbGridID.Text = m_DBlst(cmbGID.Text).listGrid(1)
    End Sub

    ''' <summary>
    ''' NULL�A�����́A�X�y�[�X�݂̂����� 
    ''' </summary>
    ''' <param name="targetObj">����Ώ�</param>
    ''' <remarks></remarks>
    Private Function IsNullOrEmptyOrSpace(ByVal targetObj As Object) As Boolean

        ' NULL �����͔���
        If frmMain_AutoParSet.isNullOrEmpty(targetObj) Then
            Return True
        End If

        ' �X�y�[�X�̂ݔ���
        If "" = targetObj.ToString.Trim Then
            Return True
        End If

        Return False
    End Function
End Class

