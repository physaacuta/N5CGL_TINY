'*********************************************************************************
' �����p�^�[���ʏo�͐ݒ��ʃN���X
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Public Class frmPCodeOutput
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMN
        emTxt_No = 0               ' �e�L�X�g�{�b�N�X  No
        emCmb_Name                 ' �R���{�{�b�N�X    �r��
        emCmb_Tansyo_Top           ' �R���{�{�b�N�X    �T����ʕ\��_�\
        emCmb_Tansyo_Bot           ' �R���{�{�b�N�X�@  �T����ʕ\��_��
        emCmb_DefMap               ' �R���{�{�b�N�X    �r�_�}�b�v��ʕ\��
        emCmb_DB_Ipan_Top          ' �R���{�{�b�N�X    DB�ۑ�_���_�\
        emCmb_DB_Ipan_Bot          ' �R���{�{�b�N�X    DB�ۑ�_���_��
        emCmb_DB_Yusen_Top         ' �R���{�{�b�N�X    DB�ۑ�_�D��_�\
        emCmb_DB_Yusen_Bot         ' �R���{�{�b�N�X    DB�ۑ�_�D��_��
        emCmb_FF                   ' �R���{�{�b�N�X    FF���M�L��
        emCmb_KizuKei              ' �R���{�{�b�N�X    �r�ڋߌy
        emCmb_KizuZyu              ' �R���{�{�b�N�X    �r�ڋߏd
        emCmb_PatLight             ' �R���{�{�b�N�X    �p�g���C�g�o��
        emCmb_VoiceRen             ' �R���{�{�b�N�X    �{�C�X�A���r
        emCmb_VoiceZyu             ' �R���{�{�b�N�X    �{�C�X�d���r
        emCmb_VoiceHege            ' �R���{�{�b�N�X    �{�C�X�w�Q
        emCmb_VoiceSuriba          ' �R���{�{�b�N�X    �{�C�X�X���o
        emCmb_VoiceTopBot          ' �R���{�{�b�N�X    �{�C�X�\�������r
        emCmb_VoiceSyuki           ' �R���{�{�b�N�X    �{�C�X�����r
        emCmb_VoiceEtc             ' �R���{�{�b�N�X    �{�C�X���̑��r
    End Enum

    Private m_AdapterPCode As SqlClient.SqlDataAdapter = Nothing        ' �A�_�v�^�[ �����p�^�[���ʏo�͐ݒ�
    Private m_AdapterType As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[ �r��}�X�^�[
    Private m_AdapterGrade As SqlClient.SqlDataAdapter = Nothing        ' �A�_�v�^�[ �O���[�h�}�X�^�[

    Private m_DsPCode As New DataSet                                    ' ��ڑ��^�e�[�u�� �����p�^�[���ʏo�͐ݒ�f�[�^
    Private m_DsType As New DataSet                                     ' ��ڑ��`�e�[�u�� �r��}�X�^�[ �R���{�{�b�N�X�p
    Private m_DsGrade As New DataSet                                    ' ��ڑ��^�e�[�u�� �O���[�h�}�X�^�[ �R���{�{�b�N�X�p
    Private m_KeyCode As String = ""                                    ' ��L�[

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
    Private Sub frmSetPat_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
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
                Call HardCopy(IMG_PATH, "�����p�^�[���ʏo�͐ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmSetPat_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' �J������
        m_DsPCode.Dispose()
        m_DsType.Dispose()
        m_DsGrade.Dispose()

        If Not m_AdapterPCode Is Nothing Then
            m_AdapterPCode.Dispose()
            m_AdapterPCode = Nothing
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
    ''' <remarks></remarks>
    Private Sub frmPCodeOutput_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                 ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                             ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCodeOutput       ' �����p�^�[���ʏo�͐ݒ��ʌďo

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �A�b�v�R�[�h�̃R���{�{�b�N�X
        MyBase.SetComboBox_Pcode(cmbUpCodeItem)
        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

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

        ' dgv�̐ݒ�
        dgvInitialize(dgvData, True)

        dgvInitialize(dgvEtc, False)

        ' �R���{�{�b�N�X�Ɍ����p�^�[�����Z�b�g
        MyBase.SetComboBox_Pcode(cmbPCode)

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
        Data_Disp(m_KeyCode)

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvData.DataGridViewSelectionRowColorChange(g_ColorSelection)
        dgvEtc.DataGridViewSelectionRowColorChange(g_ColorSelection)

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
    Private Sub frmPCodeOutput_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
    ''' </summary>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""


        strSQL = "SELECT pcode"
        strSQL &= ", �r��EdasysID"
        strSQL &= ", �T����ʕ\��_�\"
        strSQL &= ", �T����ʕ\��_��"
        strSQL &= ", �����P���r�_�}�b�v��ʕ\��"
        strSQL &= ", FF���M�L��"
        strSQL &= ", �r�ڋߌy"
        strSQL &= ", �r�ڋߏd"
        strSQL &= ", �p�g���C�g�o��"
        strSQL &= ", �{�C�X�A���r"
        strSQL &= ", �{�C�X�d���r"
        strSQL &= ", �{�C�X�w�Q"
        strSQL &= ", �{�C�X�X���o"
        strSQL &= ", �{�C�X�\�������r"
        strSQL &= ", �{�C�X�����r"
        strSQL &= ", �{�C�X���̑��r"
        strSQL &= ", DB�ۑ�_���_�\"
        strSQL &= ", DB�ۑ�_���_��"
        strSQL &= ", DB�ۑ�_�D��_�\"
        strSQL &= ", DB�ۑ�_�D��_��"

        strSQL &= " FROM " & DB_PCODE_OUTPUT
        strSQL &= " AS a LEFT OUTER JOIN " & DB_TYPE_NAME
        strSQL &= " AS b ON a.�r��EdasysID = b.EdasysID"
        strSQL &= " ORDER by b.ID ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPCode, m_AdapterPCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_OUTPUT & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

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
            '�r���ڑ��e�[�u���̐ݒ�
            ' �r��̐V�K�s��ݒ� EdasysID�A�r���� 
            drowNewType = m_DsType.Tables(0).NewRow()
            drowNewType.Item("EdasysID") = 0
            drowNewType.Item("Name") = "����"
            ' �e�[�u���̈�ԏ�ɑ}��
            m_DsType.Tables(0).Rows.InsertAt(drowNewType, 0)

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
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <param name="key">��L�[�̒l(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�����p�^�[�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("pcode='{0}' and �r��EdasysID>=0", key)          ' WHERE��ɑ����̏������� 0�`999���r��EdasysID�����o�����͂�
        Dim strWhereEtc As String = String.Format("pcode='{0}' and �r��EdasysID=-1", key)       ' WHERE��ɑ����̏������� -1���r��EdasysID�����o�����͂�
        Dim drow() As DataRow = m_DsPCode.Tables(0).Select(strWhere)                            ' EdasysID��1�ȏ�̃f�[�^��    0�`64�s
        Dim drowEtc() As DataRow = m_DsPCode.Tables(0).Select(strWhereEtc)                      ' EdasysID��-1�̂��̑��f�[�^������Ă���   0�`1�s
        Dim nRowCount As Integer = 0                                                            ' �s���J�E���g�p

        Try
            ' ��ʂɕ\��
            dgvData.RowCount = MAX_TYPE             ' �Œ�64�s

            ' 64�s�� �f�[�^���Z�b�g
            For ii As Integer = 0 To dgvData.RowCount - 1
                dgvData(EM_DATACOLUMN.emTxt_No, ii).Value = ii + 1                              ' No
                If ii < drow.Length Then
                    ' �����p�^�[���ʏo�͐ݒ�e�[�u����pcode�ŏォ�猟������ (select���g���ƃ\�[�g�����̂�)
                    For kk As Integer = nRowCount To m_DsPCode.Tables(0).Rows.Count - 1
                        ' pcode��key�̒l�ŁA�r��EdasysID��-1�ȊO�̍s���擾���A�s���J�E���g���C���N�������g                    
                        If m_DsPCode.Tables(0).Rows(kk).Item("pcode").ToString = key AndAlso _
                            CInt(m_DsPCode.Tables(0).Rows(kk).Item("�r��EdasysID").ToString) >= 0 Then
                            dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�r��EdasysID")
                            dgvData(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�T����ʕ\��_�\")
                            dgvData(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�T����ʕ\��_��")
                            dgvData(EM_DATACOLUMN.emCmb_DefMap, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�����P���r�_�}�b�v��ʕ\��")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB�ۑ�_���_�\")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB�ۑ�_���_��")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB�ۑ�_�D��_�\")
                            dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("DB�ۑ�_�D��_��")
                            dgvData(EM_DATACOLUMN.emCmb_FF, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("FF���M�L��")
                            dgvData(EM_DATACOLUMN.emCmb_KizuKei, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�r�ڋߌy")
                            dgvData(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�r�ڋߏd")
                            dgvData(EM_DATACOLUMN.emCmb_PatLight, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�p�g���C�g�o��")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�{�C�X�A���r")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�{�C�X�d���r")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�{�C�X�w�Q")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�{�C�X�X���o")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�{�C�X�\�������r")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�{�C�X�����r")
                            dgvData(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value = m_DsPCode.Tables(0).Rows(kk).Item("�{�C�X���̑��r")

                            nRowCount = kk + 1
                            Exit For
                        End If
                    Next kk
                Else
                    ' �f�[�^������
                    dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DefMap, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_FF, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_KizuKei, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_PatLight, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value = 0
                    dgvData(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value = 0
                End If
            Next ii

            '���̑�dgv�̐ݒ�
            dgvEtc.RowCount = 1                                                                             ' 1�s�Œ�
            dgvEtc(EM_DATACOLUMN.emTxt_No, 0).Value = ""                                                    ' No ��
            dgvEtc(EM_DATACOLUMN.emCmb_Name, 0).Value = "��L�ȊO"                                          ' �r��

            ' ���̑��s�̕\���f�[�^�����邩
            If drowEtc.Length > 0 Then
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Top, 0).Value = drowEtc(0).Item("�T����ʕ\��_�\")
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Bot, 0).Value = drowEtc(0).Item("�T����ʕ\��_��")
                dgvEtc(EM_DATACOLUMN.emCmb_DefMap, 0).Value = drowEtc(0).Item("�����P���r�_�}�b�v��ʕ\��")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, 0).Value = drowEtc(0).Item("DB�ۑ�_���_�\")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, 0).Value = drowEtc(0).Item("DB�ۑ�_���_��")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, 0).Value = drowEtc(0).Item("DB�ۑ�_�D��_�\")
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, 0).Value = drowEtc(0).Item("DB�ۑ�_�D��_��")
                dgvEtc(EM_DATACOLUMN.emCmb_FF, 0).Value = drowEtc(0).Item("FF���M�L��")
                dgvEtc(EM_DATACOLUMN.emCmb_KizuKei, 0).Value = drowEtc(0).Item("�r�ڋߌy")
                dgvEtc(EM_DATACOLUMN.emCmb_KizuZyu, 0).Value = drowEtc(0).Item("�r�ڋߏd")
                dgvEtc(EM_DATACOLUMN.emCmb_PatLight, 0).Value = drowEtc(0).Item("�p�g���C�g�o��")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceRen, 0).Value = drowEtc(0).Item("�{�C�X�A���r")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceZyu, 0).Value = drowEtc(0).Item("�{�C�X�d���r")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceHege, 0).Value = drowEtc(0).Item("�{�C�X�w�Q")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSuriba, 0).Value = drowEtc(0).Item("�{�C�X�X���o")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceTopBot, 0).Value = drowEtc(0).Item("�{�C�X�\�������r")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSyuki, 0).Value = drowEtc(0).Item("�{�C�X�����r")
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceEtc, 0).Value = drowEtc(0).Item("�{�C�X���̑��r")

            Else
                ' �\���f�[�^�������s�͏���������
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Top, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Bot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DefMap, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_FF, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_KizuKei, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_KizuZyu, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_PatLight, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceRen, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceZyu, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceHege, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSuriba, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceTopBot, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceSyuki, 0).Value = 0
                dgvEtc(EM_DATACOLUMN.emCmb_VoiceEtc, 0).Value = 0
            End If

            ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
            dgvData(1, 0).Selected = True
            dgvEtc(1, 0).Selected = True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
            Return
        End Try
    End Sub

    ''' <summary>
    ''' �����p�^�[���ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbPCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbPCode.SelectedIndexChanged
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂� ���ɖ߂������̑΍�)
        If m_KeyCode = cmbPCode.Text Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ�
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then
                ' ���ُ͈펞�́A�I���ʒu���߂�
                cmbPCode.Text = m_KeyCode
                Exit Sub
            End If

            ' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
            Data_Get(m_KeyCode)
        End If

        ' �I��ύX�ɔ��� ���x���l�ύX
        MyBase.SelectComboBoxItem(cmbPCode.SelectedIndex, cmbPCode, lblPCode)

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
        Data_Disp(m_KeyCode)

        '���݂̏�Ԃ�ۑ�
        g_nPaternIndex = cmbPCode.SelectedIndex

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

            ' �f�[�^�̍Ď擾
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

        ' �\���f�[�^���������A�����s��ǉ�������
        Data_Init()

        ' �ĕ\��
        Data_Disp(m_KeyCode)
    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <param name="key">��L�[�̒l(pcode) ���̏��g��Ȃ����ȁ[</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strMsg As String                    ' �G���[���b�Z�[�W�p
        Dim nCheckGr(0 To 1) As Integer         ' Gr�ݒ�l�`�F�b�N�p

        ' �s�������[�v �R���{�{�b�N�X�����Ȃ��̂ŁA�r��EdasysID�̏d���`�F�b�N�̂ݍs��
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            ' �r�킪�����̍s�͖���
            If CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) = 0 Then Continue For

            ' �r��̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvData.Rows.Count - 1
                ' ��r�悪�����̍s�͖���
                If CInt(dgvData(EM_DATACOLUMN.emCmb_Name, jj).Value) = 0 Then Continue For

                ' �r�킪�����Ȃ烁�b�Z�[�W���o���ďI��
                If CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) = CInt(dgvData(EM_DATACOLUMN.emCmb_Name, jj).Value) Then
                    strMsg = "�y �r�� �z" & vbNewLine
                    strMsg &= "�����r��͓o�^�ł��܂���B" & vbNewLine
                    strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                    MsgBox(strMsg)
                    Return False
                End If
            Next jj

        Next ii

        For ii As Integer = 0 To dgvData.Rows.Count - 1

            ' �f�[�^���Z�b�g
            If 0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) Then               ' �r��

                nCheckGr(0) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value)         ' DB�ۑ�_���_�\
                nCheckGr(1) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value)        ' DB�ۑ�_�D��_�\

                If False = Data_Range_Check(nCheckGr) Then
                    If nCheckGr(0) <= 0 Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                        strMsg &= "[DB�ۑ�_�D��_�\]��ݒ肷��ɂ́A[DB�ۑ�_���_�\]�̐ݒ肪�K�v�ł�"
                        MsgBox(strMsg, , "���̓`�F�b�N")
                        Return False
                    Else
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                        strMsg &= "[DB�ۑ�_���_�\] <= [DB�ۑ�_�D��_�\] �ƂȂ�悤�ɐݒ肵�ĉ������B"
                        MsgBox(strMsg, , "���̓`�F�b�N")
                        Return False
                    End If
                End If
            End If

            ' �f�[�^���Z�b�g
            If 0 <> CInt(dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value) Then               ' �r��

                nCheckGr(0) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value)         ' DB�ۑ�_���_��
                nCheckGr(1) = CInt(dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value)        ' DB�ۑ�_�D��_��

                If False = Data_Range_Check(nCheckGr) Then
                    If nCheckGr(0) <= 0 Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                        strMsg &= "[DB�ۑ�_�D��_��]��ݒ肷��ɂ́A[DB�ۑ�_���_��]�̐ݒ肪�K�v�ł��B"
                        MsgBox(strMsg, , "���̓`�F�b�N")
                        Return False
                    Else
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "No." & CStr(ii + 1) & " �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                        strMsg &= "[DB�ۑ�_���_��] <= [DB�ۑ�_�D��_��] �ƂȂ�悤�ɐݒ肵�ĉ������B"
                        MsgBox(strMsg, , "���̓`�F�b�N")
                        Return False
                    End If
                End If
            End If

        Next ii

        '' ��L�ȊO�̃`�F�b�N
        ' DB�ۑ�_���_�\ <= DB�ۑ�_�D��_�\�̃`�F�b�N
        nCheckGr(0) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, 0).Value)         ' DB�ۑ�_���_�\
        nCheckGr(1) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, 0).Value)        ' DB�ۑ�_�D��_�\

        If False = Data_Range_Check(nCheckGr) Then
            If nCheckGr(0) <= 0 Then
                strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                strMsg &= "��L�ȊO �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                strMsg &= "[DB�ۑ�_�D��_�\]��ݒ肷��ɂ́A[DB�ۑ�_���_�\]�̐ݒ肪�K�v�ł�"
                MsgBox(strMsg, , "���̓`�F�b�N")
                Return False
            Else
                strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                strMsg &= "��L�ȊO �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                strMsg &= "[DB�ۑ�_���_�\] <= [DB�ۑ�_�D��_�\] �ƂȂ�悤�ɐݒ肵�ĉ������B"
                MsgBox(strMsg, , "���̓`�F�b�N")
                Return False
            End If
        End If

        ' DB�ۑ�_���_�� <= DB�ۑ�_�D��_���̃`�F�b�N
        nCheckGr(0) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, 0).Value)         ' DB�ۑ�_���_��
        nCheckGr(1) = CInt(dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, 0).Value)        ' DB�ۑ�_�D��_��

        If False = Data_Range_Check(nCheckGr) Then
            If False = Data_Range_Check(nCheckGr) Then
                If nCheckGr(0) <= 0 Then
                    strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                    strMsg &= "��L�ȊO �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                    strMsg &= "[DB�ۑ�_�D��_��]��ݒ肷��ɂ́A[DB�ۑ�_���_��]�̐ݒ肪�K�v�ł��B"
                    MsgBox(strMsg, , "���̓`�F�b�N")
                    Return False
                Else
                    strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                    strMsg &= "��L�ȊO �̃O���[�h�ݒ肪����������܂���B" & vbNewLine
                    strMsg &= "[DB�ۑ�_���_��] <= [DB�ۑ�_�D��_��] �ƂȂ�悤�ɐݒ肵�ĉ������B"
                    MsgBox(strMsg, , "���̓`�F�b�N")
                    Return False
                End If
            End If
        End If

        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String = String.Format("pcode='{0}'", key)   ' WHERE��ɑ����̏�������
        Dim drow() As DataRow                                        ' �f�[�^�s
        Dim nRowCount As Integer                                     ' �폜�Ώۂ̃f�[�^��
        drow = m_DsPCode.Tables(0).Select(strWhere)                  ' pcode����L�[�̒l�̍s�����o��

        '���݉�ʂɕ\�����Ă���pcode�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DsPCode.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii

        ' ���\�����Ă���f�[�^��S�Ċi�[
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            Dim drowNew As DataRow = m_DsPCode.Tables(0).NewRow()                                      ' �V�K�s
            ' �f�[�^���Z�b�g   
            drowNew.Item("pcode") = key                                                                ' pcode
            drowNew.Item("�r��EdasysID") = dgvData(EM_DATACOLUMN.emCmb_Name, ii).Value                 ' �r��
            drowNew.Item("�T����ʕ\��_�\") = dgvData(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value        ' �T����ʕ\��_�\
            drowNew.Item("�T����ʕ\��_��") = dgvData(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value        ' �T����ʕ\��_��
            drowNew.Item("�����P���r�_�}�b�v��ʕ\��") = dgvData(EM_DATACOLUMN.emCmb_DefMap, ii).Value ' �r�_�}�b�v��ʕ\�� 
            drowNew.Item("DB�ۑ�_���_�\") = dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value        ' DB�ۑ�_���_�\
            drowNew.Item("DB�ۑ�_���_��") = dgvData(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value        ' DB�ۑ�_���_��
            drowNew.Item("DB�ۑ�_�D��_�\") = dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value       ' DB�ۑ�_�D��_�\
            drowNew.Item("DB�ۑ�_�D��_��") = dgvData(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value       ' DB�ۑ�_�D��_��
            drowNew.Item("FF���M�L��") = dgvData(EM_DATACOLUMN.emCmb_FF, ii).Value                     ' FF���M�L��
            drowNew.Item("�r�ڋߌy") = dgvData(EM_DATACOLUMN.emCmb_KizuKei, ii).Value                  ' �r�ڋߌy
            drowNew.Item("�r�ڋߏd") = dgvData(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value                  ' �r�ڋߏd
            drowNew.Item("�p�g���C�g�o��") = dgvData(EM_DATACOLUMN.emCmb_PatLight, ii).Value           ' �p�g���C�g�o��
            drowNew.Item("�{�C�X�A���r") = dgvData(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value             ' �{�C�X�A���r
            drowNew.Item("�{�C�X�d���r") = dgvData(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value             ' �{�C�X�d���r
            drowNew.Item("�{�C�X�w�Q") = dgvData(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value              ' �{�C�X�w�Q
            drowNew.Item("�{�C�X�X���o") = dgvData(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value          ' �{�C�X�X���o
            drowNew.Item("�{�C�X�\�������r") = dgvData(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value      ' �{�C�X�\�������r
            drowNew.Item("�{�C�X�����r") = dgvData(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value           ' �{�C�X�����r
            drowNew.Item("�{�C�X���̑��r") = dgvData(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value           ' �{�C�X���̑��r

            ' �e�[�u���ɒǉ�
            m_DsPCode.Tables(0).Rows.InsertAt(drowNew, m_DsPCode.Tables(0).Rows.Count)
        Next ii

        ' ���̑�dgv�̃f�[�^��S�Ċi�[ 1�s�����Ȃ��͂�
        For ii As Integer = 0 To dgvEtc.Rows.Count - 1
            Dim drowNew As DataRow = m_DsPCode.Tables(0).NewRow()                                      ' �V�K�s
            ' �f�[�^���Z�b�g
            drowNew.Item("pcode") = key                                                                ' pcode
            drowNew.Item("�r��EdasysID") = -1                                                          ' �r��
            drowNew.Item("�T����ʕ\��_�\") = dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Top, ii).Value         ' �T����ʕ\��_�\
            drowNew.Item("�T����ʕ\��_��") = dgvEtc(EM_DATACOLUMN.emCmb_Tansyo_Bot, ii).Value         ' �T����ʕ\��_��
            drowNew.Item("�����P���r�_�}�b�v��ʕ\��") = dgvEtc(EM_DATACOLUMN.emCmb_DefMap, ii).Value  ' �r�_�}�b�v��ʕ\�� 
            drowNew.Item("DB�ۑ�_���_�\") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Top, ii).Value         ' DB�ۑ�_���_�\
            drowNew.Item("DB�ۑ�_���_��") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Ipan_Bot, ii).Value         ' DB�ۑ�_���_��
            drowNew.Item("DB�ۑ�_�D��_�\") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Top, ii).Value        ' DB�ۑ�_�D��_�\
            drowNew.Item("DB�ۑ�_�D��_��") = dgvEtc(EM_DATACOLUMN.emCmb_DB_Yusen_Bot, ii).Value        ' DB�ۑ�_�D��_��
            drowNew.Item("FF���M�L��") = dgvEtc(EM_DATACOLUMN.emCmb_FF, ii).Value                      ' FF���M�L��
            drowNew.Item("�r�ڋߌy") = dgvEtc(EM_DATACOLUMN.emCmb_KizuKei, ii).Value                   ' �r�ڋߌy
            drowNew.Item("�r�ڋߏd") = dgvEtc(EM_DATACOLUMN.emCmb_KizuZyu, ii).Value                   ' �r�ڋߏd
            drowNew.Item("�p�g���C�g�o��") = dgvEtc(EM_DATACOLUMN.emCmb_PatLight, ii).Value            ' �p�g���C�g�o��
            drowNew.Item("�{�C�X�A���r") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceRen, ii).Value              ' �{�C�X�A���r
            drowNew.Item("�{�C�X�d���r") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceZyu, ii).Value              ' �{�C�X�d���r
            drowNew.Item("�{�C�X�w�Q") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceHege, ii).Value               ' �{�C�X�w�Q
            drowNew.Item("�{�C�X�X���o") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceSuriba, ii).Value           ' �{�C�X�X���o
            drowNew.Item("�{�C�X�\�������r") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceTopBot, ii).Value       ' �{�C�X�\�������r
            drowNew.Item("�{�C�X�����r") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceSyuki, ii).Value            ' �{�C�X�����r
            drowNew.Item("�{�C�X���̑��r") = dgvEtc(EM_DATACOLUMN.emCmb_VoiceEtc, ii).Value            ' �{�C�X���̑��r

            ' �e�[�u���ɒǉ�
            m_DsPCode.Tables(0).Rows.InsertAt(drowNew, m_DsPCode.Tables(0).Rows.Count)
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

            ' �f���[�g�C���T�[�g����̂Ō����p�^�[���o�͐ݒ�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_PCODE_OUTPUT
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�����p�^�[���o�͐ݒ�e�[�u���փf�[�^��o�^ 
            '�f�[�^�e�[�u���͕����e�[�u�����Q�Ƃ��Ă���ׁA�A�b�v�f�[�g�ł��Ȃ��̂ŁA�C���T�[�g����
            For ii As Integer = 0 To m_DsPCode.Tables(0).Rows.Count - 1
                With m_DsPCode.Tables(0).Rows(ii)
                    If CInt(.Item("�r��EdasysID").ToString) <> 0 Then
                        'TYPEOUT�o�^SQL���쐬
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_PCODE_OUTPUT & " VALUES ('"
                        strSql &= .Item("pcode").ToString & "',"            ' pcode
                        strSql &= .Item("�r��EdasysID").ToString & ","      ' �r��EdasysID
                        strSql &= .Item("�T����ʕ\��_�\").ToString & ","   ' �T����ʕ\��_�\
                        strSql &= .Item("�T����ʕ\��_��").ToString & ","   ' �T����ʕ\��_��
                        strSql &= .Item("�����P���r�_�}�b�v��ʕ\��").ToString & ","  ' �r�_�}�b�v��ʕ\��
                        strSql &= .Item("FF���M�L��").ToString & ","        ' FF���M�L��
                        strSql &= .Item("�r�ڋߌy").ToString & ","          ' �r�ڋߌy
                        strSql &= .Item("�r�ڋߏd").ToString & ","          ' �r�ڋߏd
                        strSql &= .Item("�p�g���C�g�o��").ToString & ","    ' �p�g���C�g�o��
                        strSql &= .Item("�{�C�X�A���r").ToString & ","      ' �{�C�X�A���r
                        strSql &= .Item("�{�C�X�d���r").ToString & ","      ' �{�C�X�d���r
                        strSql &= .Item("�{�C�X�w�Q").ToString & ","        ' �{�C�X�w�Q
                        strSql &= .Item("�{�C�X�X���o").ToString & ","      ' �{�C�X�X���o
                        strSql &= .Item("�{�C�X�\�������r").ToString & ","  ' �{�C�X�\�������r
                        strSql &= .Item("�{�C�X�����r").ToString & ","      ' �{�C�X�����r
                        strSql &= .Item("�{�C�X���̑��r").ToString & ","    ' �{�C�X���̑��r
                        strSql &= .Item("DB�ۑ�_���_�\").ToString & ","    ' DB�ۑ�_���_�\
                        strSql &= .Item("DB�ۑ�_���_��").ToString & ","    ' DB�ۑ�_���_��
                        strSql &= .Item("DB�ۑ�_�D��_�\").ToString & ","    ' DB�ۑ�_�D��_�\
                        strSql &= .Item("DB�ۑ�_�D��_��").ToString & ")"    ' DB�ۑ�_�D��_��

                        'SQL���s
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' �G���[�Ȃ烍�[���o�b�N���ďI��
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
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
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal bDisptype As Boolean)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                         ' �I��F����
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .ColumnHeadersHeight = 38                                     ' �w�b�_�[�̍���
            .RowTemplate.Height = 19                                      ' ���s����Ă镶���������Ă��܂��̂ŁA�f�t�H���g�̍s������ύX
            .ScrollBars = ScrollBars.Both                                 ' �X�N���[���o�[�̕\��

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '�I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
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
        dgvColSetText(dgv, "NO", 35, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �r��
        If bDisptype = True Then
            ' dgvData���i�v���p�e�B�Z�b�g
            Using clm As New DataGridViewComboBoxColumn
                clm.HeaderText = "�r��"
                clm.Width = 120
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.Frozen = True
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DataSource = m_DsType.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
                clm.ValueMember = "EdasysID"             ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
                clm.DisplayMember = "Name"               ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
                clm.MaxDropDownItems = 10                ' �h���b�v�_�E�����X�g�̕\����
                dgv.Columns.Add(clm)
            End Using
        Else
            ' dgvEtc���i�v���p�e�B�Z�b�g
            dgvColSetText(dgv, "", 120, True, True, _
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        End If

        ' �T����ʕ\��_�\
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�T�����" & vbNewLine & "�\��(�\)"
            clm.Width = 78
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


        ' �T����ʕ\��_��
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�T�����" & vbNewLine & "�\��(��)"
            clm.Width = 78
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

        ' �����P���r�_�}�b�v��ʕ\��
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�r�_ϯ��" & vbNewLine & "��ʕ\��"
            clm.Width = 70
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)            ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        ' DB�ۑ�_���_�\
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�c�a�ۑ�" & vbNewLine & "���(�\)"
            clm.Width = 78
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

        ' DB�ۑ�_���_��
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�c�a�ۑ�" & vbNewLine & "���(��)"
            clm.Width = 78
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

        ' DB�ۑ�_�D��_�\
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�c�a�ۑ�" & vbNewLine & "�D��(�\)"
            clm.Width = 78
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

        ' DB�ۑ�_�D��_��
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�c�a�ۑ�" & vbNewLine & "�D��(��)"
            clm.Width = 78
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

        ' FF���M�L��
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "FF���M" & vbNewLine & "�L��"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        '�r�ڋߌy 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�r�ڋ�" & vbNewLine & "�y"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        '�r�ڋߏd 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�r�ڋ�" & vbNewLine & "�d"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        '�p�g���C�g�o�� 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "���ײ�" & vbNewLine & "�o��"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        '�{�C�X�A���r 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�{�C�X" & vbNewLine & "�A���r"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        '�{�C�X�d���r 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�{�C�X" & vbNewLine & "�d���r"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        ' �{�C�X�w�Q
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�{�C�X" & vbNewLine & "�w�Q"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        '�{�C�X�X���o 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�{�C�X" & vbNewLine & "�X���o"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        ' �{�C�X�\�������r
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�{�C�X" & vbNewLine & "�\���r"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        '�{�C�X�����r 
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�{�C�X" & vbNewLine & "�����r"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

        ' �{�C�X���̑�
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�{�C�X" & vbNewLine & "���̑�"
            clm.Width = 60
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DataSource = m_DsGrade.Tables(0)      ' �R���{�{�b�N�X�ŕ\������f�[�^�\�[�X�i�O���[�h��ڑ��e�[�u���j
            clm.ValueMember = "EdasysID"                    ' �R���{�{�b�N�X�̎��ۂ̒l�i�f�[�^�\�[�X�̗񖼁j
            clm.DisplayMember = "Name"                      ' �R���{�{�b�N�X�ŕ\�������l�i�f�[�^�\�[�X�̗񖼁j
            clm.MaxDropDownItems = 10                       ' �h���b�v�_�E�����X�g�̕\����
            clm.DropDownWidth = 150                         ' ��̕��������̂ŁA�h���b�v�_�E�����X�g�̕����w��
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellEnter, dgvEtc.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        ' ��̃^�C�v���R���{�{�b�N�X���H
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' �h���b�v�_�E���\��
            SendKeys.Send("%{DOWN}")
        End If
    End Sub

    ''' <summary>
    ''' dgvData�̉��X�N���[�����ύX���ꂽ�Ƃ��� ���̑�dgv���X�N���[��������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>���ڂ��������Ƃ��p</remarks>
    Private Sub dgvData_Scroll(ByVal sender As Object, ByVal e As System.Windows.Forms.ScrollEventArgs) Handles dgvData.Scroll
        ' �X�N���[���C�x���g�������X�N���[���Ȃ�
        If e.ScrollOrientation = ScrollOrientation.HorizontalScroll Then
            ' dgvEtc��dgvData�̃X�N���[���ʒu�����킹��
            dgvEtc.HorizontalScrollingOffset = dgvData.HorizontalScrollingOffset
        End If
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
        Dim rec As Integer = CInt(IIf(bln, cmbPCode.SelectedIndex + 1, cmbPCode.SelectedIndex - 1))
        ' �㉺���`�F�b�N
        If rec < 0 OrElse rec >= cmbPCode.Items.Count Then Exit Sub
        '�ؑ�
        cmbPCode.SelectedIndex = rec
    End Sub

    ''' <summary>
    ''' ����Z���̒l���ς������F��ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvData_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvData.CellValueChanged, dgvEtc.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s�ȊO�̓X���[
        If e.RowIndex < 0 Then Exit Sub

        ' GrEdasysID�����Ă����Ȃ�F�ύX
        If e.ColumnIndex = EM_DATACOLUMN.emCmb_Tansyo_Top OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_Tansyo_Bot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DefMap OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Ipan_Top OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Ipan_Bot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Yusen_Top OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_DB_Yusen_Bot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_FF OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_KizuKei OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_KizuZyu OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_PatLight OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceRen OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceZyu OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceHege OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceSuriba OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceTopBot OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceSyuki OrElse
           e.ColumnIndex = EM_DATACOLUMN.emCmb_VoiceEtc Then

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
        Data_Disp(m_KeyCode)
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

        '�J���}��؂��pcode��z��ɕϊ�
        strPcode = Split(strToKey, ",")

        '�R�s�[���̃f�[�^���擾
        strSQL = "SELECT pcode"
        strSQL &= ", �r��EdasysID"
        strSQL &= ", �T����ʕ\��_�\"
        strSQL &= ", �T����ʕ\��_��"
        strSQL &= ", �����P���r�_�}�b�v��ʕ\��"
        strSQL &= ", FF���M�L��"
        strSQL &= ", �r�ڋߌy"
        strSQL &= ", �r�ڋߏd"
        strSQL &= ", �p�g���C�g�o��"
        strSQL &= ", �{�C�X�A���r"
        strSQL &= ", �{�C�X�d���r"
        strSQL &= ", �{�C�X�w�Q"
        strSQL &= ", �{�C�X�X���o"
        strSQL &= ", �{�C�X�\�������r"
        strSQL &= ", �{�C�X�����r"
        strSQL &= ", �{�C�X���̑��r"
        strSQL &= ", DB�ۑ�_���_�\"
        strSQL &= ", DB�ۑ�_���_��"
        strSQL &= ", DB�ۑ�_�D��_�\"
        strSQL &= ", DB�ۑ�_�D��_��"

        strSQL &= " FROM " & DB_PCODE_OUTPUT
        strSQL &= " WHERE pcode='" & strFromkey & "'"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsPCode, m_AdapterPCode, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PARAM_CAMSET & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Try

            '''' �f���[�g�C���T�[�g
            If Not tcls_DB.Open() Then Return False

            '�g�����U�N�V�����J�n
            tcls_DB.TransactStart()

            ' ���R�[�h�̍폜
            Dim strSqlDel As String     ' ���R�[�h�폜�psql
            strSqlDel = "DELETE FROM " & DB_PCODE_OUTPUT & " WHERE pcode IN(" & strToKey & ")"

            If 0 > tcls_DB.DMLExecute(strSqlDel) Then Return False

            ' ���R�[�h�̑}�� �R�s�[��pcode�̌������J��Ԃ�
            For ii As Integer = 0 To strPcode.Length - 1
                ' �R�s�[��pcode�̃��R�[�h�������J��Ԃ��}��
                For kk As Integer = 0 To m_DsPCode.Tables(0).Rows.Count - 1
                    With m_DsPCode.Tables(0).Rows(kk)
                        If CInt(.Item("�r��EdasysID").ToString) <> 0 Then
                            'OUTPUT_PCODE�o�^SQL���쐬
                            strSQL = ""
                            strSQL &= "INSERT INTO " & DB_PCODE_OUTPUT & " VALUES ("
                            strSQL &= strPcode(ii) & ","                        ' pcode
                            strSQL &= .Item("�r��EdasysID").ToString & ","      ' �r��EdasysID
                            strSQL &= .Item("�T����ʕ\��_�\").ToString & ","   ' �T����ʕ\��_�\
                            strSQL &= .Item("�T����ʕ\��_��").ToString & ","   ' �T����ʕ\��_��
                            strSQL &= .Item("�����P���r�_�}�b�v��ʕ\��").ToString & ","  ' �r�_�}�b�v��ʕ\��
                            strSQL &= .Item("FF���M�L��").ToString & ","        ' FF���M�L��
                            strSQL &= .Item("�r�ڋߌy").ToString & ","          ' �r�ڋߌy
                            strSQL &= .Item("�r�ڋߏd").ToString & ","          ' �r�ڋߏd
                            strSQL &= .Item("�p�g���C�g�o��").ToString & ","    ' �p�g���C�g�o��
                            strSQL &= .Item("�{�C�X�A���r").ToString & ","      ' �{�C�X�A���r
                            strSQL &= .Item("�{�C�X�d���r").ToString & ","      ' �{�C�X�d���r
                            strSQL &= .Item("�{�C�X�w�Q").ToString & ","        ' �{�C�X�w�Q
                            strSQL &= .Item("�{�C�X�X���o").ToString & ","      ' �{�C�X�X���o
                            strSQL &= .Item("�{�C�X�\�������r").ToString & ","  ' �{�C�X�\�������r
                            strSQL &= .Item("�{�C�X�����r").ToString & ","      ' �{�C�X�����r
                            strSQL &= .Item("�{�C�X���̑��r").ToString & ","    ' �{�C�X���̑��r
                            strSQL &= .Item("DB�ۑ�_���_�\").ToString & ","    ' DB�ۑ�_���_�\
                            strSQL &= .Item("DB�ۑ�_���_��").ToString & ","    ' DB�ۑ�_���_��
                            strSQL &= .Item("DB�ۑ�_�D��_�\").ToString & ","    ' DB�ۑ�_�D��_�\
                            strSQL &= .Item("DB�ۑ�_�D��_��").ToString & ")"    ' DB�ۑ�_�D��_��

                            'SQL���s
                            If 0 > tcls_DB.DMLExecute(strSQL) Then
                                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                                tcls_DB.TransactRollback()
                                Return False
                            End If
                        End If
                    End With
                Next kk
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            '�����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
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

    ''' <summary>
    ''' ���͒l�͈̔̓`�F�b�N
    ''' </summary>
    ''' <param name="nCheckGr">nCheckGr(0):DB�ۑ�_���_�\,DB�ۑ�_���_���AnCheckGr(1):DB�ۑ�_�D��_�\,DB�ۑ�_�D��_��</param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function Data_Range_Check(ByRef nCheckGr() As Integer) As Boolean

        ' DB�ۑ�_���,�D�� ���ɖ��I����OK
        If 0 >= nCheckGr(0) And 0 >= nCheckGr(1) Then
            Return True
        End If

        ' DB�ۑ�_�D��̂ݑI�������NG
        If 0 >= nCheckGr(0) And 0 < nCheckGr(1) Then
            Return False
        End If

        ' DB�ۑ�_��ʂ̂ݑI�������OK
        If 0 < nCheckGr(0) And 0 >= nCheckGr(1) Then
            Return True
        End If

        Dim nCheckPriority1 As Integer = 0
        Dim nCheckPriority2 As Integer = 0

        nCheckPriority1 = GetCmbPriority(nCheckGr(0))     ' DB�ۑ�_��ʁi�D��x�j
        nCheckPriority2 = GetCmbPriority(nCheckGr(1))     ' DB�ۑ�_�D��i�D��x�j

        ' DB�ۑ�_��ʁ�DB�ۑ�_�D���NG �i�l�̏�������������(�D��x������)�j
        If nCheckPriority1 < nCheckPriority2 Then
            Return False
        End If

        ' DB�ۑ�_��ʁ���DB�ۑ�_�D�� OK
        Return True

    End Function

    ''' <summary>
    ''' GrEdasysID�ɑ΂���D��x�擾
    ''' </summary>
    ''' <param name="key">GrEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetCmbPriority(ByVal key As Integer) As Integer
        ' EdasysID�̒l�����o��
        Dim strWhere As String = String.Format("EdasysID='{0}'", key)           ' WHERE��ɑ����̏�������
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select(strWhere)       ' 1�s����͂�

        Dim retPriority As Integer = 0   ' �߂�l �����l��0

        ' Key��0�Ȃ�0��Ԃ�
        If 0 = key Then Return retPriority

        ' 1�s�����Ȃ��Ǝv������
        For ii As Integer = 0 To drowGrade.Length - 1
            ' 1�s�����Ȃ��Ǝv������
            retPriority = CInt(drowGrade(0).Item("�D��x").ToString)
        Next ii

        Return retPriority
    End Function

    ''' <summary>
    ''' �X�V�{�^���̗L��/����
    ''' </summary>
    ''' <param name="bPassResult">�p�X���[�h���͌��� (True:�L�� False:����)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' �t�@���N�V�����L�[�̐ݒ�{�^��
        pnlItem.Enabled = bPassResult                   ' �R�s�[�p���X�g�̔��f�{�^��

    End Sub

End Class
