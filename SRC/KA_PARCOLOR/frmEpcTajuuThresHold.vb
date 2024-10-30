'*********************************************************************************
' ���d����\臒l�ݒ��ʃN���X
'	[Ver]
'		Ver.01    2017/01/10  ����
'
'	[����]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmEpcTajuuThresHold

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMN
        emTxt_No = 0                                        ' �e�L�X�g�{�b�N�X�@����No
        emCmb_ReductionId                                   ' �R���{�{�b�N�X �k����ID
        emCmb_FilterId                                      ' �R���{�{�b�N�X �t�B���^ID
        emTxt_UpperThreshold                                ' �e�L�X�g�{�b�N�X�@��臒l
        emTxt_LowerThreshold                                ' �e�L�X�g�{�b�N�X�@��臒l
        emTxt_UpperLimit                                    ' �e�L�X�g�{�b�N�X�@�ナ�~�b�^
        emTxt_LowerLimit                                    ' �e�L�X�g�{�b�N�X�@�����~�b�^
        emTxt_EdgeDeadband                                  ' �e�L�X�g�{�b�N�X�@�s���ѕ�
        emTxt_name                                          ' �e�L�X�g�{�b�N�X�@����
        iInsButton                      ' �}���{�^��
        iDelButton                      ' �폜�{�^��
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u��
    Private m_Adapter_Reduction As SqlClient.SqlDataAdapter = Nothing           ' �A�_�v�^�[ �k�����ݒ�
    Private m_DS_Reduction As New DataSet                                       ' ��ڑ��^�e�[�u�� �k�����ݒ�
    Private m_Adapter_Filter As SqlClient.SqlDataAdapter = Nothing              ' �A�_�v�^�[ �t�B���^�ݒ�
    Private m_DS_Filter As New DataSet                                          ' ��ڑ��^�e�[�u�� �t�B���^�ݒ�
    Private m_KeyCode As String = ""                                ' ��L�[
    Private m_KeyAngle As Integer = 1                               ' �p�x

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
    Private Sub frmEpcTajuuThresHold_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' �t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then Exit Sub

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' ��0�I���W��
            Case 0
                btnF01.Select()                 ' F01�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(False)               ' �O��
            Case 1
                btnF02.Select()                 ' F02�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(True)                ' ����
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "���d����\臒l�ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmEpcTajuuThresHold_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' ���낢��J������
        m_DS.Dispose()
        If Not m_Adapter Is Nothing Then
            m_Adapter.Dispose()
            m_Adapter = Nothing
        End If
        m_DS_Reduction.Dispose()
        If Not m_Adapter_Reduction Is Nothing Then
            m_Adapter_Reduction.Dispose()
            m_Adapter_Reduction = Nothing
        End If
        m_DS_Filter.Dispose()
        If Not m_Adapter_Filter Is Nothing Then
            m_Adapter_Filter.Dispose()
            m_Adapter_Filter = Nothing
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
    Private Sub frmEpcTajuuThresHold_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmEpcTajuuThresHold     ' ���d����\臒l�ݒ� ��ʌďo

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �A�b�v�R�[�h�̃R���{�{�b�N�X
        MyBase.SetComboBox_Scode(cmbUpCodeItem)

        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' �J�������̃Z�b�g
        Call modMain.SetCamAria(camName, 0)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       ' �I��
        End If

        ' DGV������
        dgvInitialize(dgvData)

        ' �R���{�{�b�N�X�Ƀp�^�[�����Z�b�g
        MyBase.SetComboBox_Scode(cmbSCode)

        ' �R���{�{�b�N�X�ɃJ�����p�x��ݒ�
        SetComboBox_Angle(cmbAngle)

        If 0 < cmbSCode.Items.Count Then
            ' �O�̉�ʂőI��������Ԃ��Z�b�g
            If cmbSCode.Items.Count - 1 < g_nScodeIndex Then
                ' �Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
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

            modMain.TaskEnd()                       ' �I��
        End If

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = SelectComboBoxKey(cmbSCode.SelectedIndex, cmbSCode)
        m_KeyAngle = CInt(cmbAngle.Text)
        Data_Disp(m_KeyCode)

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)

        If dgvData.RowCount > 0 Then
            ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
            dgvData(1, 0).Selected = True
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
    Private Sub frmEpcTajuuThresHold_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        ' ���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' DB����Ƃ��Ă����R���{�{�b�N�X�p�̒l��\���p�ɉ��H
    ''' </summary>
    Private Sub Data_Init()
        Dim drowNewReduction As DataRow      ' �k�����ݒ�̐V�K�s
        Dim drowNewFilter As DataRow         ' �t�B���^�ݒ�̐V�K�s

        Try
            drowNewReduction = m_DS_Reduction.Tables(0).NewRow()
            drowNewReduction.Item("id") = 0
            drowNewReduction.Item("Name") = "-"
            ' �e�[�u���̈�ԏ�ɑ}��
            m_DS_Reduction.Tables(0).Rows.InsertAt(drowNewReduction, 0)

            drowNewFilter = m_DS_Filter.Tables(0).NewRow()
            drowNewFilter.Item("id") = 0
            drowNewFilter.Item("Name") = "-"
            ' �e�[�u���̈�ԏ�ɑ}��
            m_DS_Filter.Tables(0).Rows.InsertAt(drowNewFilter, 0)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R���{�{�b�N�X�p�f�[�^ �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

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
        strSQL = "SELECT scode"
        strSQL &= ",camangle"
        strSQL &= ",id"
        strSQL &= ",ReductionId"
        strSQL &= ",FilterId"
        strSQL &= ",UpperThreshold"
        strSQL &= ",LowerThreshold"
        strSQL &= ",UpperLimit"
        strSQL &= ",LowerLimit"
        strSQL &= ",name"
        strSQL &= ",EdgeDeadband"

        strSQL &= " FROM " & DB_PARAM_THRESHOLD
        strSQL &= " ORDER BY id ASC"

        ' SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_THRESHOLD & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL������
        strSQL = "SELECT id"
        strSQL &= ", name"
        strSQL &= " FROM " & DB_PARAM_REDUCTION
        strSQL &= " ORDER BY id ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Reduction, m_Adapter_Reduction, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_REDUCTION & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL������ 
        strSQL = "SELECT id"
        strSQL &= ", name"
        strSQL &= " FROM " & DB_PARAM_FILTER
        strSQL &= " ORDER BY id ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS_Filter, m_Adapter_Filter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_FILTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' �R���{�{�b�N�X�̕\���f�[�^��������
        Data_Init()

        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        '�C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexdgv(dgvData)
        ' �p�^�[�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("scode='{0}' and camangle='{1}'", key, CInt(cmbAngle.Text))          ' WHERE��ɑ����̏�������
        Dim drow() As DataRow = m_DS.Tables(0).Select(strWhere)

        ' ��ʂɕ\��
        dgvData.RowCount = MAX_TAJUUTHRESHOLD
        For ii As Integer = 0 To dgvData.RowCount - 1
            dgvData(EM_DATACOLUMN.emTxt_No, ii).Value = ii + 1
            If ii < drow.Length Then
                dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value = drow(ii).Item("FilterId")
                dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value = drow(ii).Item("ReductionId")
                dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value = drow(ii).Item("UpperLimit")
                dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value = drow(ii).Item("LowerLimit")
                dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value = drow(ii).Item("EdgeDeadband")
                dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value = drow(ii).Item("UpperThreshold")
                dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value = drow(ii).Item("LowerThreshold")
                dgvData(EM_DATACOLUMN.emTxt_name, ii).Value = drow(ii).Item("name")
            Else
                dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value = 0
                dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value = 0
                dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value = ""
                dgvData(EM_DATACOLUMN.emTxt_name, ii).Value = ""
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' �����\�ʏ�ԕύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbSCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbSCode.SelectedIndexChanged
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂� ���ɖ߂����� �̎��̑΍�)
        If m_KeyCode = cmbSCode.Text Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ���
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

        '���݂̏�Ԃ�ۑ�
        g_nScodeIndex = cmbSCode.SelectedIndex

    End Sub

    ''' <summary>
    ''' �p�x�ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbAngle_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbAngle.SelectedIndexChanged
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂� ���ɖ߂����� �̎��̑΍�)
        If m_KeyAngle = CInt(cmbAngle.Text) Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ���
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then
                ' ���ُ͈펞�́A�I���ʒu���߂�
                cmbAngle.Text = CStr(m_KeyAngle)
                Exit Sub
            End If

            ' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
            Data_Get(m_KeyCode)
        End If

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyAngle = CInt(cmbAngle.Text)
        Data_Disp(m_KeyCode)

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
                '�\���X�V
                Data_Disp(m_KeyCode)
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
    ''' <param name="key">��L�[�̒l(scode) ���̏��g��Ȃ����ȁ[</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String
        Dim strMsg As String    '�G���[���b�Z�[�W
        ' ����No�����[�v
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value) And _
               0 = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_name, ii).Value) Then
                '�o�^�Ȃ�
            Else
                '�o�^�L��
                If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value) Then
                    '�k����ID�w�薳��
                    strMsg = "�y ���d����\臒l�ݒ� �z" & vbNewLine
                    strMsg &= "�k����ID��ݒ肵�Ă��������B" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1)
                    MsgBox(strMsg)
                    Return False
                End If
                If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value) Then
                    '�t�B���^ID�w�薳��
                    strMsg = "�y ���d����\臒l�ݒ� �z" & vbNewLine
                    strMsg &= "�t�B���^ID��ݒ肵�Ă��������B" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1)
                    MsgBox(strMsg)
                    Return False
                End If
                ' ��臒l
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " ��臒l") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value = strWk
                ' ��臒l
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " ��臒l") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value = strWk
                ' �ナ�~�b�^
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " �ナ�~�b�^") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value = strWk
                ' �����~�b�^
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value)
                If tClass_ImputCheck.CheckImputMsg_Dbl(strWk, 0, 255, False, 0, "No=" & ii + 1 & " �����~�b�^") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value = strWk
                ' �s���ѕ�
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value)
                If 0 <> tClass_ImputCheck.CheckImputData_Int(strWk, 0, 512, False) Then
                    strMsg = "�y No=" & ii + 1 & " �s���ѕ� �z" & vbNewLine
                    strMsg &= "�K�؂Ȓl�ł͂���܂���B" & vbNewLine
                    strMsg &= "���͔͈́F�u0 �` 512�v"
                    MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                    Return False
                End If
                dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value = strWk
                ' ����
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_name, ii).Value)
                If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 256, "No=" & ii + 1 & " ����") <> 0 Then Return False
                dgvData(EM_DATACOLUMN.emTxt_name, ii).Value = strWk
            End If
        Next ii
        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(scode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String = String.Format("scode='{0}' and camangle='{1}'", key, m_KeyAngle)          ' WHERE��ɑ����̏�������
        Dim drow() As DataRow                                        ' �f�[�^�s
        Dim nRowCount As Integer                                     ' �폜�Ώۂ̃f�[�^��
        Dim nId As Integer = 0
        drow = m_DS.Tables(0).Select(strWhere)

        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DS.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii
        ' ����No�����[�v
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            Dim drowNew As DataRow = m_DS.Tables(0).NewRow()                                      ' �V�K�s

            If 0 = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value) And _
               0 = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value) And _
                "" = CStr(dgvData(EM_DATACOLUMN.emTxt_name, ii).Value) Then
                '�o�^����
            Else
                '�o�^�L��
                'ID���C���N�������g
                nId = nId + 1
                drowNew.Item("scode") = key
                drowNew.Item("camangle") = m_KeyAngle
                drowNew.Item("id") = nId
                drowNew.Item("FilterId") = CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, ii).Value)
                drowNew.Item("ReductionId") = CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, ii).Value)
                drowNew.Item("UpperLimit") = CDbl(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, ii).Value)
                drowNew.Item("LowerLimit") = CDbl(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, ii).Value)
                drowNew.Item("EdgeDeadband") = CInt(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, ii).Value)
                drowNew.Item("UpperThreshold") = CDbl(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, ii).Value)
                drowNew.Item("LowerThreshold") = CDbl(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, ii).Value)
                drowNew.Item("name") = dgvData(EM_DATACOLUMN.emTxt_name, ii).Value
                ' �e�[�u���ɒǉ�
                m_DS.Tables(0).Rows.InsertAt(drowNew, m_DS.Tables(0).Rows.Count)
            End If

        Next ii
    End Sub

    ''' <summary>
    ''' �A�b�v�f�[�g
    ''' </summary>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function DB_UpDate() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB�ڑ�
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSql As String                                'Sql��

        Try
            '''' �f�[�^�x�[�X�I�[�v��
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            ' �f���[�g�C���T�[�g����̂őS���ڂ��폜
            strSql = "DELETE FROM " & DB_PARAM_THRESHOLD
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To m_DS.Tables(0).Rows.Count - 1
                With m_DS.Tables(0).Rows(ii)
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_PARAM_THRESHOLD & " VALUES ('"
                    strSql &= .Item("scode").ToString & "',"            ' scode
                    strSql &= .Item("camangle").ToString & ","          ' �p�x
                    strSql &= .Item("id").ToString & ","                ' ID
                    strSql &= .Item("ReductionId").ToString & ","       ' �k����ID
                    strSql &= .Item("FilterId").ToString & ","          ' �t�B���^ID
                    strSql &= .Item("UpperThreshold").ToString & ","    ' ��臒l
                    strSql &= .Item("LowerThreshold").ToString & ","    ' ��臒l
                    strSql &= .Item("UpperLimit").ToString & ","        ' �ナ�~�b�^
                    strSql &= .Item("LowerLimit").ToString & ","        ' �����~�b�^
                    strSql &= "'" & .Item("name").ToString & "',"       ' ����
                    strSql &= .Item("EdgeDeadband").ToString & " )"     ' �s���ѕ�

                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                    'End If
                End With
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_TAJUUTHRESHOLD)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            ' �G���[�Ȃ烍�[���o�b�N���ďI��
            tcls_DB.TransactRollback()
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return False
        Finally
            '''' ��n��
            tcls_DB.Close()
            tcls_DB.Dispose()
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
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' �I��F����
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .AllowUserToResizeColumns = True                                ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Both                                   ' �X�N���[���o�[�̕\��

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' �I��F���o�b�N�J���[��
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' �I��F����s�����ɐF�ς�

            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' ���`
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42             ' �w�b�_�[�̍���

        End With

        '' ���`

        ' No
        dgvColSetText(dgv, "NO", 60, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �k����ID
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�k����ID"
            clm.Width = 120
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.ReadOnly = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DS_Reduction.Tables(0)            ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X
            clm.ValueMember = "id"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        ' �t�B���^ID
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�t�B���^ID"
            clm.Width = 120
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.ReadOnly = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DS_Filter.Tables(0)            ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X
            clm.ValueMember = "id"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        ' ��臒l
        dgvColSetText(dgv, "��臒l" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' ��臒l
        dgvColSetText(dgv, "��臒l" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' �ナ�~�b�^
        dgvColSetText(dgv, "�ナ�~�b�^" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' �����~�b�^
        dgvColSetText(dgv, "�����~�b�^" & vbNewLine & "( 0 - 255 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' �s���ѕ�
        dgvColSetText(dgv, "�s���ѕ�" & vbNewLine & "( 0 - 512 )", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' ����
        dgvColSetText(dgv, "����", 100, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)


        ' ��֑}��
        dgvColSetButton(dgv, "���" & vbNewLine & "�}��", 50, False, False, _
                        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                        New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�}��")

        ' �폜
        dgvColSetButton(dgv, "�폜", 50, False, False, _
                        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
                        New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�폜")

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
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing
        Dim strSQL As String = ""

        '''' �����`�F�b�N
        ' ��
        Dim strFromKey As String = ""                                    ' �R�s�[���L�[ 
        If cmbUpCodeItem.SelectedIndex < 0 Then Exit Sub '��
        strFromKey = SelectComboBoxKey(cmbUpCodeItem.SelectedIndex, cmbUpCodeItem)
        ' ��
        Dim strToKey As String = MyBase.GetUpCodeItem_To(False)          ' �R�s�[��L�[
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

                ' �����@�\�փp�����[�^�ύX�ʒm
                modMain.Send_Main(CH_PARAM_TAJUUTHRESHOLD)
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
                modMain.TaskEnd()                       ' �I��
            End If

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        '''' ��ʍĕ\��
        Data_Disp(m_KeyCode)

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
    ''' �R���{�{�b�N�X�ɃJ�����p�x��o�^
    ''' </summary>
    ''' <param name="cmb"></param>
    ''' <remarks></remarks>
    Private Sub SetComboBox_Angle(ByVal cmb As ComboBox)
        cmb.Items.Add("1")
        cmb.Items.Add("2")
        cmb.Items.Add("3")
        cmb.Items.Add("4")
        '�����I����R�摜�Ƃ���
        cmb.Text = "1"
    End Sub

    ''' <summary>
    ''' dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_A_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case EM_DATACOLUMN.iInsButton   '��֑}���{�^��
                InsBtnClick_Type(dgv, e.RowIndex)
            Case EM_DATACOLUMN.iDelButton   '�폜�{�^��
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' dgv�̑}���{�^�������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
    ''' <param name="nRowIndex">���R�[�h��}������ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        ' �}���{�^�����g�p�\���`�F�b�N
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' �ŏI�s�͎g�p�s�Ȃ̂Ŕ�����
            Exit Sub
        End If

        ' �ŏI�s�Ƀf�[�^�������Ă���ꍇ�͑}�����s��Ȃ�
        If 0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_ReductionId, dgv.Rows.Count - 1).Value) Or _
           0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_FilterId, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_UpperLimit, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_LowerLimit, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, dgv.Rows.Count - 1).Value) Or _
            "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_name, dgv.Rows.Count - 1).Value) Then
            MsgBox("�ŏI�s�Ƀf�[�^�����݂��܂��B")
            Exit Sub
        End If

        '�}�����邩���b�Z�[�W���o��
        If MsgBox("�}�����܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "�����m�F") = MsgBoxResult.No Then Exit Sub

        ' dgv�Ƀ��R�[�h��}��
        InsertAt_TypeData(nRowIndex)

        ' dgv�̍ŏI�s���폜
        dgv.Rows.RemoveAt(dgv.Rows.Count - 1)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexdgv(dgv)
    End Sub

    ''' <summary>
    ''' dgv�̍폜�{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)
        '�폜���邩���b�Z�[�W���o��
        If MsgBox("�폜���܂����H", MsgBoxStyle.YesNo Or MsgBoxStyle.DefaultButton1, "�����m�F") = MsgBoxResult.No Then Exit Sub

        ' dgv�̍ŏI�s�Ƀ��R�[�h��}��
        InsertAt_TypeData(dgv.Rows.Count)

        ' dgv�̃��R�[�h���폜
        dgv.Rows.RemoveAt(nRowIndex)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexdgv(dgv)
    End Sub

    ''' <summary>
    ''' dgv�Ƀf�[�^�s��}��
    ''' </summary>
    ''' <param name="nRowIndex">�s�̑}���ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgv�Ƀ��R�[�h��}��
        dgvData.Rows.Insert(nRowIndex)

        ' �f�[�^������
        dgvData(EM_DATACOLUMN.emCmb_FilterId, nRowIndex).Value = 0
        dgvData(EM_DATACOLUMN.emCmb_ReductionId, nRowIndex).Value = 0
        dgvData(EM_DATACOLUMN.emTxt_UpperLimit, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_LowerLimit, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_EdgeDeadband, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_UpperThreshold, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_LowerThreshold, nRowIndex).Value = ""
        dgvData(EM_DATACOLUMN.emTxt_name, nRowIndex).Value = ""

    End Sub

    ''' <summary>
    ''' dgv�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexdgv(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMN.emTxt_No, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�

            ' �}���{�^���̎g�p�\��Ԃ�ݒ�    (�{�^�����g�p�s�ɂ͂ł��Ȃ��̂ŁA�F��ύX�j
            If ii = dgv.RowCount - 1 Then
                '�ŏI�s�͑}���{�^�����g�p�s��
                dgv(EM_DATACOLUMN.iInsButton, ii).Style.ForeColor = Color.Gray
            Else
                '����ȊO�͎g�p��
                dgv(EM_DATACOLUMN.iInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next
    End Sub

    ''' <summary>
    ''' �R�s�[����scode����R�s�[���scode�փf�[�^���f���[�g�C���T�[�g����
    ''' </summary>
    ''' <param name="strFromkey">�R�s�[���L�[</param>
    ''' <param name="strToKey">�R�s�[��̃L�[��������������</param>
    ''' <returns>����/�ُ�</returns>
    ''' <remarks></remarks>
    Private Function DB_UPDATE_UpCodeItem(ByVal strFromkey As String, ByVal strToKey As String) As Boolean

        Dim tcls_DB As New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String = ""
        Dim strScode() As String                                ' �R�s�[���scode
        Dim strErrMsg As String = ""

        '�J���}��؂��scode��z��ɕϊ�
        strScode = Split(strToKey, ",")

        '�R�s�[���̃f�[�^���擾
        strSQL = "SELECT scode"
        strSQL &= ",camangle"
        strSQL &= ",id"
        strSQL &= ",ReductionId"
        strSQL &= ",FilterId"
        strSQL &= ",UpperThreshold"
        strSQL &= ",LowerThreshold"
        strSQL &= ",UpperLimit"
        strSQL &= ",LowerLimit"
        strSQL &= ",name"
        strSQL &= ",EdgeDeadband"
        strSQL &= " FROM " & DB_PARAM_THRESHOLD
        strSQL &= " WHERE scode='" & strFromkey & "'"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_THRESHOLD & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Try

            '''' �f���[�g�C���T�[�g
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            ' ���R�[�h�̍폜
            Dim strSqlDel As String     ' ���R�[�h�폜�psql
            strSqlDel = "DELETE FROM " & DB_PARAM_THRESHOLD & " WHERE scode IN(" & strToKey & ")"

            If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False

            ' ���R�[�h�̑}�� �R�s�[��scode�̌������J��Ԃ�
            For ii As Integer = 0 To strScode.Length - 1
                ' �R�s�[��scode�̃��R�[�h�������J��Ԃ��}��
                For kk As Integer = 0 To m_DS.Tables(0).Rows.Count - 1
                    With m_DS.Tables(0).Rows(kk)
                        'DB_PARAM_THRESHOLD�o�^SQL���쐬
                        strSQL = ""
                        strSQL &= "INSERT INTO " & DB_PARAM_THRESHOLD & " VALUES ("
                        strSQL &= strScode(ii) & ","                        ' scode
                        strSQL &= .Item("camangle").ToString & ","          ' �p�x
                        strSQL &= .Item("id").ToString & ","                ' ID
                        strSQL &= .Item("ReductionId").ToString & ","       ' �k����ID
                        strSQL &= .Item("FilterId").ToString & ","          ' �t�B���^ID
                        strSQL &= .Item("UpperThreshold").ToString & ","    ' ��臒l
                        strSQL &= .Item("LowerThreshold").ToString & ","    ' ��臒l
                        strSQL &= .Item("UpperLimit").ToString & ","        ' �ナ�~�b�^
                        strSQL &= .Item("LowerLimit").ToString & ","        ' �����~�b�^
                        strSQL &= "'" & .Item("name").ToString & "',"       ' ����
                        strSQL &= .Item("EdgeDeadband").ToString & " )"     ' �s���ѕ�
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
