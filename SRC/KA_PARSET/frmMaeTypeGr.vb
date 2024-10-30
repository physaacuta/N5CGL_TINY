'*********************************************************************************
' �O�H���}�X�^�[�o�^ �r��E�O���[�h�ݒ��ʃN���X
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmMaeTypeGr
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �O�H���}�X�^�[�f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMNMASTER
        emNo = 0                            ' �e�L�X�g�{�b�N�X No
        emTxt_MaeProcessCode                ' �e�L�X�g�{�b�N�X �O�H���R�[�h
        emChk_Check                         ' �`�F�b�N�{�b�N�X �L������
        emTxt_Name                          ' �e�L�X�g�{�b�N�X ����
        emCmb_DispMode                      ' �R���{�{�b�N�X   �O�H���\�����[�h
        emCmb_ImageKeisiki                  ' �R���{�{�b�N�X   �摜�`��
        emTxt_KizuImageName1                ' �e�L�X�g�{�b�N�X �r�摜����1
        emTxt_KizuImageName2                ' �e�L�X�g�{�b�N�X �r�摜����2
        emTxt_KizuImageName3                ' �e�L�X�g�{�b�N�X �r�摜����3
        emTxt_KizuImageName4                ' �e�L�X�g�{�b�N�X �r�摜����4
        emCmb_TashozaiCheck                 ' �R���{�{�b�N�X   �����ޔ���
    End Enum

    ' �O�H���r��f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMNTYPE
        emNo = 0                            ' �e�L�X�g�{�b�N�X No
        emTxt_FFName                        ' �e�L�X�g�{�b�N�X FFName
        emTxt_ID                            ' �e�L�X�g�{�b�N�X ID
        emTxt_Name                          ' �e�L�X�g�{�b�N�X �r����
        emTxt_Yusen                         ' �e�L�X�g�{�b�N�X �D��x
        emTxt_MapDispName                   ' �e�L�X�g�{�b�N�X �}�b�v�\������
        emCmb_HyoziUmu                      ' �R���{�{�b�N�X �@�\���L��
        emInsButton                         ' �}���{�^��
        emDelButton                         ' �폜�{�^��
    End Enum

    ' �O�H���O���[�h�f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMNGRADE
        emNo = 0                            ' �e�L�X�g�{�b�N�X No
        emTxt_FFName                        ' �e�L�X�g�{�b�N�X FFName
        emTxt_GradeID                       ' �e�L�X�g�{�b�N�X �O���[�hID
        emTxt_Name                          ' �e�L�X�g�{�b�N�X �O���[�h����
        emTxt_Yusen                         ' �e�L�X�g�{�b�N�X �D��x
        emMapButton                         ' �}�b�v�\���F
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterMaeMaster As SqlClient.SqlDataAdapter = Nothing    ' �A�_�v�^�[ MaeMaster(�O�H���}�X�^�[)
    Private m_AdapterMaeType As SqlClient.SqlDataAdapter = Nothing      ' �A�_�v�^�[ MaeType(�O�H���r��)
    Private m_AdapterMaeGrade As SqlClient.SqlDataAdapter = Nothing     ' �A�_�v�^�[ MaeGrade(�O�H���O���[�h)

    Private m_DsMaeMaster As New DataSet                                ' ��ڑ��^�e�[�u��MAE_MASTER
    Private m_DsMaeType As New DataSet                                  ' ��ڑ��^�e�[�u��MAE_TYPE_NAME
    Private m_DsMaeGrade As New DataSet                                 ' ��ڑ��^�e�[�u��MAE_GRADE_NAME

    Private m_KeyCode As String = ""                                    ' ��L�[

    Private m_sMaeGrade() As String                                     '�I��O�H���R�[�h�̑O�H���O���[�h����
    Private m_nMaeGradeID() As Integer                                  '�I��O�H���R�[�h�̑O�H���O���[�hID


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
            Case 0
                btnF01.Select()                 ' F01�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(False)               ' �O��
            Case 1
                btnF02.Select()                 ' F02�R���{�{�b�N�X�ύX���Ƀt�@���N�V�����������ꂽ���̑΍�
                ChangeCode(True)                ' ����
            Case 6                              ' F07 ��ʈ��
                Call PrintOut()
            Case 7                              ' F08 ��ʕۑ�
                Call HardCopy(IMG_PATH, "�O�H���ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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

        m_DsMaeMaster.Dispose()
        If Not m_DsMaeMaster Is Nothing Then
            m_DsMaeMaster.Dispose()
            m_DsMaeMaster = Nothing
        End If

        m_DsMaeType.Dispose()
        If Not m_AdapterMaeType Is Nothing Then
            m_AdapterMaeType.Dispose()
            m_AdapterMaeType = Nothing
        End If

        m_DsMaeGrade.Dispose()
        If Not m_AdapterMaeGrade Is Nothing Then
            m_AdapterMaeGrade.Dispose()
            m_AdapterMaeGrade = Nothing
        End If

        ' ��ʐؑ�
        ShowForm(frmID)
    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmMaeTypeGr             ' �O�H���}�X�^�[�o�^ �r��E�O���[�h�ݒ��ʌďo

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾���� �R���{�{�b�N�X�̒l���Ƃ��Ă���̂ŁAdgv�ݒ����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()   '�I��
        End If

        ' �R���{�{�b�N�X�̕\���f�[�^��������
        Data_Init()

        '�O�H���}�X�^�[dgv������
        dgvInitializeDefMaster(dgvMaeMaster)

        '�O�H���r��dgv������
        dgvInitializeDefType(dgvMaeType)

        '�O�H���O���[�hdgv������
        dgvInitializeDefGrade(dgvMaeGrade)

        ' ��ʂɒl��\��
        Data_Disp(m_KeyCode)

        ' �I���s�̐F�ς� �����\���ŐF�����Ȃ��悤�ɁA�s���ݒ�̌�ɂ��Ɨǂ�
        dgvMaeType.DataGridViewSelectionRowColorChange(g_ColorSelection)        ' �O�H���r��dgv
        dgvMaeGrade.DataGridViewSelectionRowColorChange(g_ColorSelection)       ' �O�H���O���[�hdgv

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
        strSQL = "SELECT �O�H���R�[�h"
        strSQL &= ", �L������"
        strSQL &= ", ����"
        strSQL &= ", �O�H���\�����[�h"
        strSQL &= ", �摜�`��"
        strSQL &= ", �r�摜����1"
        strSQL &= ", �r�摜����2"
        strSQL &= ", �r�摜����3"
        strSQL &= ", �r�摜����4"
        strSQL &= ", �����ޔ���"
        strSQL &= " FROM " & DB_MAE_MASTER

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsMaeMaster, m_AdapterMaeMaster, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_MAE_MASTER & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL������
        strSQL = "SELECT �O�H���R�[�h"
        strSQL &= ", FFName"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", �D��x"
        strSQL &= ", �}�b�v�\������"
        strSQL &= ", �\���L��"
        strSQL &= " FROM " & DB_MAE_TYPE_NAME
        strSQL &= " ORDER BY �O�H���R�[�h ASC"

        'SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DsMaeType, m_AdapterMaeType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_MAE_TYPE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL������
        strSQL = "SELECT �O�H���R�[�h"
        strSQL &= ", FFName"
        strSQL &= ", �O���[�hID"
        strSQL &= ", Name"
        strSQL &= ", �D��x"
        strSQL &= ", �}�b�v�\���F"
        strSQL &= " FROM " & DB_MAE_GRADE_NAME
        strSQL &= " ORDER BY �O�H���R�[�h ASC"

        'SQL�����sm_DsMaeGrade
        If 0 > MyBase.Get_SelectData(strSQL, m_DsMaeGrade, m_AdapterMaeGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_MAE_GRADE_NAME & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' DB����Ƃ��Ă����R���{�{�b�N�X�p�̒l��\���p�ɉ��H
    ''' </summary>
    ''' <remarks>�O���[�h���̂Ɉȏ������A�����̍s��ǉ��A</remarks>
    Private Sub Data_Init()

        Try
            Dim drowMaster() As DataRow = m_DsMaeMaster.Tables(0).Select()           ' �S���R�[�h��\�����邽��WHERE��Ȃ�
            ' �R���{�{�b�N�X�̃f�[�^��������
            cmbMaeProcessCode.Items.Clear()

            If drowMaster.Length = 0 Then Exit Sub

            For ii As Integer = 0 To drowMaster.Length - 1
                cmbMaeProcessCode.Items.Add(drowMaster(ii).Item("�O�H���R�[�h").ToString)
            Next ii
            cmbMaeProcessCode.MaxDropDownItems = drowMaster.Length
            If 0 < cmbMaeProcessCode.Items.Count Then
                ' �O�̉�ʂőI��������Ԃ��Z�b�g
				If cmbMaeProcessCode.Items.Count - 1 < g_nMaeIndex Then
					' �Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
					cmbMaeProcessCode.SelectedIndex = 0
				Else
					cmbMaeProcessCode.SelectedIndex = g_nMaeIndex
				End If
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�R���{�{�b�N�X�p�f�[�^ �ݒ�G���[ [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

    ''' <summary>
    ''' dgType�Ƀf�[�^�s��}��
    ''' </summary>
    ''' <param name="nRowIndex">�s�̑}���ʒu</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvMaeType�Ƀ��R�[�h��}��
        dgvMaeType.Rows.Insert(nRowIndex)

        ' �f�[�^������
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, nRowIndex).Value = ""            ' FFName
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, nRowIndex).Value = ""                ' ID
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, nRowIndex).Value = ""              ' �r����
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, nRowIndex).Value = ""             ' �D��x
        dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, nRowIndex).Value = ""       ' �}�b�v�\������
        ' �\���L��
        Using clm As New DataGridViewComboBoxCell
            Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
            listItem.Items.AddRange(m_sMaeGrade)
            listItem.MaxDropDownItems = listItem.Items.Count
            dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, nRowIndex) = clm
            dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, nRowIndex).Value = ""
        End Using
    End Sub


    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)

        '�O�H���}�X�^�[�̃f�[�^��\��
        Data_DispDgvMaeMaster(dgvMaeMaster)
        '�O�H���}�X�^�[�̃C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexDgvMaeMaster(dgvMaeMaster)

        '�O�H���r��̃f�[�^��\��
        Data_DispDgvMaeType(dgvMaeType, key)
        '�O�H���r��̃C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexDgvMaeType(dgvMaeType)

        '�O�H���O���[�h�̃f�[�^��\��
        Data_DispDgvMaeGrade(dgvMaeGrade, key)
        '�O�H���O���[�h�̃C���f�b�N�X�ƃ{�^���̕\���ݒ�
        InitIndexDgvMaeGrade(dgvMaeGrade)

        ' �s�̐F��ς��� �i�����\���ŐF���ς��΍�j
        dgvMaeType.CurrentRow.DefaultCellStyle.BackColor = Color.Empty
        dgvMaeGrade.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' �O�H���}�X�^�[dgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Data_DispDgvMaeMaster(ByVal dgv As DataGridView) As Boolean
        Dim drowMaster() As DataRow = m_DsMaeMaster.Tables(0).Select()           ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        '�s��������
        dgv.Rows.Clear()

        ' ��ʂɕ\��
        dgv.RowCount = MAX_MAE_LINE                   ' �O�H���}�X�^�[dgv�̃��R�[�h���͌Œ�
        For ii As Integer = 0 To dgv.RowCount - 1
            '�O���[�h��ڑ��e�[�u���̒l������Ԃ͂��������B������Ώ����l
            If ii < drowMaster.Length Then

                ' �O�H���R�[�h��NULL�̎��͋�
                If drowMaster(ii).Item("�O�H���R�[�h") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = drowMaster(ii).Item("�O�H���R�[�h")
                End If

                ' �L�������`�F�b�N�{�b�N�X
                If CInt(drowMaster(ii).Item("�L������")) = 0 Then
                    dgv(EM_DATACOLUMNMASTER.emChk_Check, ii).Value = False                                        ' 0�Ȃ�`�F�b�NOFF
                Else
                    dgv(EM_DATACOLUMNMASTER.emChk_Check, ii).Value = True                                         ' 0�ȊO�̓`�F�b�NON
                End If

                ' �O�H�����̂�NULL�̎��͋�
                If drowMaster(ii).Item("����") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = drowMaster(ii).Item("����")
                End If

                ' �O�H���\�����[�h
                If drowMaster(ii).Item("�O�H���\�����[�h").ToString.Trim = "0" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = "�ΏۊO"
                ElseIf drowMaster(ii).Item("�O�H���\�����[�h").ToString = "1" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = "�Ӄ֏��"
                ElseIf drowMaster(ii).Item("�O�H���\�����[�h").ToString = "2" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = "FF���"
                End If

                ' �摜�`��
                If drowMaster(ii).Item("�摜�`��").ToString.Trim = "0" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value = "JPEG"
                ElseIf drowMaster(ii).Item("�摜�`��").ToString = "1" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value = "BMP"
                End If

                ' �r�摜����1��NULL�̎��͋�
                If drowMaster(ii).Item("�r�摜����1") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = drowMaster(ii).Item("�r�摜����1")
                End If

                ' �r�摜����2��NULL�̎��͋�
                If drowMaster(ii).Item("�r�摜����2") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = drowMaster(ii).Item("�r�摜����2")
                End If

                ' �r�摜����3��NULL�̎��͋�
                If drowMaster(ii).Item("�r�摜����3") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = drowMaster(ii).Item("�r�摜����3")
                End If

                ' �r�摜����4��NULL�̎��͋�
                If drowMaster(ii).Item("�r�摜����4") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = drowMaster(ii).Item("�r�摜����4")
                End If

                ' �����ޔ���
                If drowMaster(ii).Item("�����ޔ���").ToString.Trim = "0" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value = "������"
                ElseIf drowMaster(ii).Item("�����ޔ���").ToString = "1" Then
                    dgv(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value = "������"
                End If

            Else
                dgv(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = ""                    ' �O�H���R�[�h
                dgv(EM_DATACOLUMNMASTER.emChk_Check, ii).Value = False                          ' �L������
                dgv(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = ""                              ' �O�H������
                dgv(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value = ""                          ' �O�H�����[�h
                dgv(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value = ""                      ' �摜�`��
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = ""                    ' �r�摜����1
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = ""                    ' �r�摜����2
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = ""                    ' �r�摜����3
                dgv(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = ""                    ' �r�摜����4
                dgv(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value = ""                     ' �����ޔ���
            End If
        Next ii
    End Function

    ''' <summary>
    ''' �O�H���r��dgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <param name="key">��L�[�̒l(�O�H���R�[�h)</param>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvMaeType(ByVal dgv As DataGridView, ByVal key As String)
        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�O�H�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("�O�H���R�[�h='{0}'", key)          ' WHERE��ɑ����̏�������
        Dim drowType() As DataRow = m_DsMaeType.Tables(0).Select(strWhere)
        Dim strWhereGrade As String
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)

        '�s��������
        dgv.Rows.Clear()

        '�O���[�h�e�[�u���̐ݒ�
        ReDim m_sMaeGrade(drowGrade.Length)
        ReDim m_nMaeGradeID(drowGrade.Length)
        m_sMaeGrade(0) = "-"
        m_nMaeGradeID(0) = 0
        ' �S�s���[�v �O���[�h�̖��̂Ɉȏ������
        For ii As Integer = 0 To drowGrade.Length - 1
            m_sMaeGrade(ii + 1) = drowGrade(ii).Item("Name").ToString & "�ȏ�"
            m_nMaeGradeID(ii + 1) = CInt(drowGrade(ii).Item("�O���[�hID"))
        Next ii

		' ��ʂɕ\��		' 2015/02/13 �O�H���r�킪�P�����\������Ȃ����̑Ή� �Ȃ� MAX_MAE_LINE �Ŋ����Ă���̂��͕s�� �쑺
		'dgv.RowCount = CInt(MAX_MAE_TYPE / MAX_MAE_LINE)	   ' �O�H���r��dgv�̃��R�[�h���͌Œ�
		dgv.RowCount = MAX_MAE_TYPE	   ' �O�H���r��dgv�̃��R�[�h���͌Œ�

        For ii As Integer = 0 To dgv.RowCount - 1

            '�r���ڑ��e�[�u���̒l������Ԃ͂��������B������Ώ����l
            If ii < drowType.Length Then

                ' FFName��NULL�̎��͋�
                If drowType(ii).Item("FFName") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = drowType(ii).Item("FFName")
                End If

                '    ID��NULL�̎��͋�()
                If drowType(ii).Item("ID") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = drowType(ii).Item("ID")
                End If

                ' Name��NULL�̎��͋�
                If drowType(ii).Item("Name") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = drowType(ii).Item("Name")
                End If

                ' �D��x��NULL�̂Ƃ��͋�
                If drowType(ii).Item("�D��x") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = ""
                Else
                    ' ���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = drowType(ii).Item("�D��x")
                End If

                ' �}�b�v�\�����̂�NULL�̂Ƃ��͋�
                If drowType(ii).Item("�}�b�v�\������") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = ""
                Else
                    '���̂܂ܕ\��
                    dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = drowType(ii).Item("�}�b�v�\������")
                End If

                ' �\���L��
                Using clm As New DataGridViewComboBoxCell
                    Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
                    listItem.Items.AddRange(m_sMaeGrade)
                    listItem.MaxDropDownItems = listItem.Items.Count

                    dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii) = clm

                    strWhereGrade = String.Format("�O�H���R�[�h='{0}' and �O���[�hID={1}", key, drowType(ii).Item("�\���L��"))
                    drowGrade = m_DsMaeGrade.Tables(0).Select(strWhereGrade)

                    If drowGrade.Length = 1 Then
                        dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = drowGrade(0).Item("Name").ToString & "�ȏ�"
                    Else
                        dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = "-"
                    End If

                End Using

            Else
                ' �f�[�^�������̂ŏ�����
                dgv(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = ""                  ' ID
                dgv(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = ""              ' FFName
                dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""                ' �r����
                dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = ""               ' �D��x
                dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = ""         ' �}�b�v�\������
                dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = ""            ' �\���L��
                ' �\���L��
                Using clm As New DataGridViewComboBoxCell
                    Dim listItem As DataGridViewComboBoxCell = CType(clm, DataGridViewComboBoxCell)
                    listItem.Items.AddRange(m_sMaeGrade)
                    listItem.MaxDropDownItems = listItem.Items.Count
                    dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii) = clm
                    dgv(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value = ""
                End Using

            End If

        Next ii

        ' �J�[�\���X�^�[�g�ʒu�͓��̓G���A�̐擪
        dgv(1, 0).Selected = True
    End Sub

    ''' <summary>
    ''' �O�H���O���[�hdgv�Ƀf�[�^��\��
    ''' </summary>
    ''' <param name="key">��L�[�̒l(�O�H���R�[�h)</param>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvMaeGrade(ByVal dgv As DataGridView, ByVal key As String)
        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�O�H�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("�O�H���R�[�h='{0}'", key)          ' WHERE��ɑ����̏�������
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)

        '�s��������
        dgv.Rows.Clear()

		' ��ʂɕ\��
		' 2015/02/13 �O�H���O���[�h���P�����\������Ȃ����̑Ή� �Ȃ� MAX_MAE_LINE �Ŋ����Ă���̂��͕s�� �쑺
		'dgv.RowCount = CInt(MAX_MAE_GRADE / MAX_MAE_LINE)				 ' �O�H���O���[�hdgv�̃��R�[�h���͌Œ�
		dgv.RowCount = MAX_MAE_GRADE										' �O�H���O���[�hdgv�̃��R�[�h���͌Œ�
		For ii As Integer = 0 To dgv.RowCount - 1
			'�O���[�h��ڑ��e�[�u���̒l������Ԃ͂��������B������Ώ����l
			If ii < drowGrade.Length Then

				' FFName��NULL�̎��͋�
				If drowGrade(ii).Item("FFName") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = ""
				Else
					'���̂܂ܕ\��
					dgv(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = drowGrade(ii).Item("FFName")
				End If

				' �O���[�hID��NULL�̎��͋�()
				If drowGrade(ii).Item("�O���[�hID") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = ""
				Else
					'���̂܂ܕ\��
					dgv(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = drowGrade(ii).Item("�O���[�hID")
				End If

				' Name��NULL�̎��͋�
				If drowGrade(ii).Item("Name") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""
				Else
					'���̂܂ܕ\��
					dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = drowGrade(ii).Item("Name")
				End If

				' �D��x��NULL�̂Ƃ��͋�
				If drowGrade(ii).Item("�D��x") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = ""
				Else
					'���̂܂ܕ\��
					dgv(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = drowGrade(ii).Item("�D��x")
				End If

				' �}�b�v�\���F�̒l��NULL�̎��͔��F
				If drowGrade(ii).Item("�}�b�v�\���F") Is System.DBNull.Value Then
					dgv(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor = Color.White		' ���F
				Else
					'�ݒ肳��Ă���F��\��
					dgv(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor = _
					  Color.FromArgb(CInt(drowGrade(ii).Item("�}�b�v�\���F")))			   ' �}�b�v�\���F
				End If
			Else
				dgv(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = ""							' �O���[�hID"
				dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""								' �O�H���R�[�h
				dgv(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = ""								' FFName
				dgv(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = ""								' �D��x
				dgv(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor = Color.White			' �}�b�v�\���F�ɔ��F
			End If
		Next ii
    End Sub

    ''' <summary>
    ''' �O�H���R�[�h�ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbMaeProcessCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbMaeProcessCode.SelectedIndexChanged
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂ŁA ���ɖ߂����ꍇ�̑΍�)
        If m_KeyCode = cmbMaeProcessCode.Text Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ�
        If m_KeyCode <> "" Then
            ' ���͒l�`�F�b�N
            If Not Data_Check(m_KeyCode) Then
                ' ���ُ͈펞�́A�I���ʒu���߂�
                cmbMaeProcessCode.Text = m_KeyCode
                Exit Sub
            End If

            ' �ύX�O�̃L�[�ɑ΂� ��ʃf�[�^��荞��
            Data_MaeTypeGrGet(m_KeyCode)
        End If

        ' �I��ύX�ɔ��� ���x���l�ύX
        Dim drowMaster() As DataRow = m_DsMaeMaster.Tables(0).Select()           ' �S���R�[�h��\�����邽��WHERE��Ȃ�

        Dim nIdx As Integer = cmbMaeProcessCode.SelectedIndex

        ' �L���Ȃ�΃f�t�H���g�F�A�����Ȃ�Ζ����F���Z�b�g����B
        If CInt(drowMaster(nIdx).Item("�L������").ToString) = 1 Then
            lblMaeProcessCode.Text = drowMaster(nIdx).Item("����").ToString
            lblMaeProcessCode.BackColor = g_ColorSetting
        Else
            lblMaeProcessCode.Text = drowMaster(nIdx).Item("����").ToString
            lblMaeProcessCode.BackColor = Color.LightGray
        End If

        ' ��ʂɑO�H���R�[�h�ʂ̒l��\��
        If m_KeyCode <> "" Then
            m_KeyCode = cmbMaeProcessCode.Text
            Data_Disp(m_KeyCode)
        Else
            m_KeyCode = cmbMaeProcessCode.Text
        End If

        ' ���݂̏�Ԃ�ۑ�
		g_nMaeIndex = cmbMaeProcessCode.SelectedIndex

    End Sub

    ''' <summary>
    ''' �m�菈��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' ���͒l�`�F�b�N
        If Not Data_Check(m_KeyCode) Then Exit Sub
        ' ���̉�ʕ\�����̒l�Ǝ�荞��()

        Try

            ' �o�^���܂����H
            frmMsg.Text = "�����m�F"
            frmMsg.ChangeMsg("�o�^���܂����H", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            ' ��ʂ̃f�[�^���擾
            Data_MaeMasterGet()
            Data_MaeTypeGrGet(m_KeyCode)

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

        ' �ݒ�̏�����
        Data_Init()

        ' ��ʂɒl��\��
        Data_Disp(m_KeyCode)
    End Sub

    ''' <summary>
    ''' ���͒l�̃`�F�b�N
    ''' </summary>
    ''' <param name="key">��L�[�̒l(�O�H���R�[�h) ���̏����g�p</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String     ' dgv�̃f�[�^�i�[�p
        Dim strMsg As String    ' �d���p�̃G���[���b�Z�[�W

        ' �O�H���}�X�^�[dgv�A���������[�v
        For ii As Integer = 0 To dgvMaeMaster.Rows.Count - 1

            ' �L������擾
            Dim bCheck As Boolean = CBool(dgvMaeMaster(EM_DATACOLUMNMASTER.emChk_Check, ii).Value)

            ' �O�H���R�[�h
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "�O�H���}�X�^�[�ݒ� No=" & ii + 1 & " �O�H���R�[�h") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value = strWk

            ' �O�H���R�[�h���󔒂̍s�͖���
            If Trim(strWk) = "" Then Continue For

            '�O�H���R�[�h�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeMaster.Rows.Count - 1
                ' ��r��̑O�H���R�[�h�������Ă���Δ�r
                If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, jj).Value) <> "" Then
                    ' �O�H���R�[�h�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value).ToUpper = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, jj).Value).ToUpper Then
                        strMsg = "�y �O�H���}�X�^�[ �z" & vbNewLine
                        strMsg &= "�����O�H���R�[�h�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �O�H������
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "�O�H���}�X�^�[�ݒ� No=" & ii + 1 & "�O�H������") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value = strWk
            ' �O�H�����̂��󔒂̍s�͖���
            If Trim(strWk) = "" Then Continue For

            ' �O�H�����̂̏d���`�F�b�N
            If bCheck = True Then
                For jj As Integer = ii + 1 To dgvMaeMaster.Rows.Count - 1
                    If Not CBool(dgvMaeMaster(EM_DATACOLUMNMASTER.emChk_Check, jj).Value) Then
                        Continue For                                                ' �����Ȃ玟��
                    End If
                    ' ��r��̑O�H���R�[�h�������Ă���Δ�r
                    If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, jj).Value) <> "" Then
                        ' �O�H�����̂������Ȃ烁�b�Z�[�W���o���ďI��
                        If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value) = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, jj).Value) Then
                            strMsg = "�y �O�H���}�X�^�[ �z" & vbNewLine
                            strMsg &= "�����O�H�����͓̂o�^�ł��܂���B" & vbNewLine
                            strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                            MsgBox(strMsg)
                            Return False
                        End If
                    End If
                Next jj
            End If

            ' �r�摜����1
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "�O�H���}�X�^�[�ݒ� No=" & ii + 1 & "�r�摜����1") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value = strWk

            ' �r�摜����2
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "�O�H���}�X�^�[�ݒ� No=" & ii + 1 & "�r�摜����2") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value = strWk

            ' �r�摜����3
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "�O�H���}�X�^�[�ݒ� No=" & ii + 1 & "�r�摜����3") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value = strWk

            ' �r�摜����4
            strWk = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "�O�H���}�X�^�[�ݒ� No=" & ii + 1 & "�r�摜����4") <> 0 Then Return False
            dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value = strWk

        Next ii

        ' �O�H���r��dgv�A64�s���[�v
        For ii As Integer = 0 To dgvMaeType.Rows.Count - 1

            ' FFName
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "�r��ݒ� No=" & ii + 1 & "FFName") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value = strWk

            ' FFName�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' ��r���ID�����l�Ȃ��r
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                    ' FFName�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value).ToUpper = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, jj).Value).ToUpper Then
                        strMsg = "�y �r��ݒ� �z" & vbNewLine
                        strMsg &= "����FFName�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' ID
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "�r��ݒ� No=" & ii + 1 & " ID") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value = strWk

            ' ID���󔒂̍s�͖���
            If Trim(strWk) = "" Then Continue For

            ' ID�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' ��r���ID�����l�Ȃ��r
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                    ' ID�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value) = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                        strMsg = "�y �r��ݒ� �z" & vbNewLine
                        strMsg &= "����ID�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �}�b�v�\������
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 4, "�r��ݒ� No=" & ii + 1 & "�}�b�v�\������") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value = strWk

            ' �r����
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 32, "�r��ݒ� No=" & ii + 1 & "�r����") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = strWk

            ' �r���̂̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' ��r���ID�����l�Ȃ��r
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) Then
                    ' �r���̂������Ȃ烁�b�Z�[�W���o���ďI��
                    If CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value) = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, jj).Value) Then
                        strMsg = "�y �r��ݒ� �z" & vbNewLine
                        strMsg &= "�����r���͓̂o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �D��x
            strWk = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, False, "�r��ݒ� No=" & ii + 1 & " �D��x") <> 0 Then Return False
            dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value = strWk

            ' �D��x�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeType.Rows.Count - 1
                ' ��r���ID�����l�ŁA�D��x�����l�Ȃ��r
                If IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value) And _
                    Trim(CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, jj).Value)) <> "" And _
                    IsNumeric(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, jj).Value) Then
                    ' �D��x�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value) = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, jj).Value) Then
                        strMsg = "�y �r��ݒ� �z" & vbNewLine
                        strMsg &= "�����D��x�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

        Next ii

        ' �O�H���O���[�hdgv�A10�s���[�v
        For ii As Integer = 0 To dgvMaeGrade.Rows.Count - 1

            ' FFName
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "�O���[�h�ݒ� No=" & ii + 1 & "FFName") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value = strWk

            ' FFName�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' ��r��̃O���[�hID�����l�Ȃ��r
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                    ' FFName�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value).ToUpper = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, jj).Value).ToUpper Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "����FFName�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �O���[�hID
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "�O���[�h�ݒ� No=" & ii + 1 & " �O���[�hID") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value = strWk

            ' �O���[�hID���󔒂̍s�͖���
            If Trim(strWk) = "" Then Continue For

            ' �O���[�hID�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' ��r��̃O���[�hID�����l�Ȃ��r
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                    ' �O���[�hID�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value) = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "�����O���[�hID�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �O���[�h����
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 32, "�O���[�h�ݒ� No=" & ii + 1 & "�O���[�h����") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = strWk

            ' �O���[�h���̂̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' ��r��̃O���[�hID�����l�Ȃ�O���[�h���̂��r
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) Then
                    ' �O���[�h���̂������Ȃ烁�b�Z�[�W���o���ďI��
                    If CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value) = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, jj).Value) Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "�����O���[�h���͓̂o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' �D��x
            strWk = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, False, "�O���[�h�ݒ� No=" & ii + 1 & " �D��x") <> 0 Then Return False
            dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value = strWk

            ' �D��x�̏d���`�F�b�N
            For jj As Integer = ii + 1 To dgvMaeGrade.Rows.Count - 1
                ' ��r��̃O���[�hID�����l�ŁA�D��x�����l�Ȃ��r
                If IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value) And _
                    Trim(CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, jj).Value)) <> "" And _
                    IsNumeric(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, jj).Value) Then
                    ' �D��x�������Ȃ烁�b�Z�[�W���o���ďI��
                    If CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value) = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, jj).Value) Then
                        strMsg = "�y �O���[�h�ݒ� �z" & vbNewLine
                        strMsg &= "�����D��x�͓o�^�ł��܂���B" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " �� No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_MaeMasterGet()
        Dim nCountNo As Integer = 0  'No�J�E���g�p
        Dim nCheck As Integer
        ' �f���[�g�C���T�[�g����ׁA�������Ă���s���N���A����
        m_DsMaeMaster.Tables(0).Rows.Clear()

        ' �O�H���}�X�^�[dgv�̒l��O�H���}�X�^�[��ڑ��e�[�u���֊i�[
        For ii As Integer = 0 To dgvMaeMaster.RowCount - 1

            If CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value) = "" Then Continue For

            Dim drowMaster As DataRow = m_DsMaeMaster.Tables(0).NewRow()                                            ' �V�K�s

            drowMaster.Item("�O�H���R�[�h") = CStr(dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_MaeProcessCode, ii).Value)  ' �O�H���R�[�h

            ' �L�������`�F�b�N�{�b�N�X�̃`�F�b�N��Ԃɂ��A�L��������Z�b�g
            If CBool(dgvMaeMaster(EM_DATACOLUMNMASTER.emChk_Check, ii).Value) Then
                nCheck = 1                                              ' �L��
            Else
                nCheck = 0                                              ' ����
            End If

            drowMaster.Item("�L������") = nCheck                        ' �L������

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value Is Nothing = True Then            ' �O�H������
                drowMaster.Item("����") = DBNull.Value
            Else
                drowMaster.Item("����") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_Name, ii).Value
            End If

            ' �O�H���\�����[�h�́A�R���{�{�b�N�X�Ȃ̂ŁA�I���������ڂɂ��i�[�l����
            If dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value.ToString.Trim = "�ΏۊO" Then
                drowMaster.Item("�O�H���\�����[�h") = 0
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value.ToString.Trim = "�Ӄ֏��" Then
                drowMaster.Item("�O�H���\�����[�h") = 1
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_DispMode, ii).Value.ToString.Trim = "FF���" Then
                drowMaster.Item("�O�H���\�����[�h") = 2
            Else
                drowMaster.Item("�O�H���\�����[�h") = 0
            End If

            ' �摜�`���́A�R���{�{�b�N�X�Ȃ̂ŁA�I���������ڂɂ��i�[�l����
            If dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value.ToString.Trim = "JPEG" Then
                drowMaster.Item("�摜�`��") = 0
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_ImageKeisiki, ii).Value.ToString.Trim = "BMP" Then
                drowMaster.Item("�摜�`��") = 1
            Else
                drowMaster.Item("�摜�`��") = 0
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value Is Nothing = True Then          ' �r�摜����1
                drowMaster.Item("�r�摜����1") = DBNull.Value
            Else
                drowMaster.Item("�r�摜����1") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName1, ii).Value
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value Is Nothing = True Then          ' �r�摜����2
                drowMaster.Item("�r�摜����2") = DBNull.Value
            Else
                drowMaster.Item("�r�摜����2") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName2, ii).Value
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value Is Nothing = True Then          ' �r�摜����3
                drowMaster.Item("�r�摜����3") = DBNull.Value
            Else
                drowMaster.Item("�r�摜����3") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName3, ii).Value
            End If

            If dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value Is Nothing = True Then          ' �r�摜����4
                drowMaster.Item("�r�摜����4") = DBNull.Value
            Else
                drowMaster.Item("�r�摜����4") = dgvMaeMaster(EM_DATACOLUMNMASTER.emTxt_KizuImageName4, ii).Value
            End If

            ' �����ޔ���́A�R���{�{�b�N�X�Ȃ̂ŁA�I���������ڂɂ��i�[�l����
            If dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value.ToString.Trim = "������" Then
                drowMaster.Item("�����ޔ���") = 0
            ElseIf dgvMaeMaster(EM_DATACOLUMNMASTER.emCmb_TashozaiCheck, ii).Value.ToString.Trim = "������" Then
                drowMaster.Item("�����ޔ���") = 1
            Else
                drowMaster.Item("�����ޔ���") = 0
            End If

            m_DsMaeMaster.Tables(0).Rows.Add(drowMaster)        '�s��ǉ�
            nCountNo += 1                                       'No = No + 1

        Next ii
    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(�O�H���R�[�h)</param>
    ''' <remarks></remarks>
    Private Sub Data_MaeTypeGrGet(ByVal key As String)
        ' �O�H���O���[�h�̃f�[�^���Z�b�g
        Data_MaeGradeGet(key)
        ' �O�H���r��̃f�[�^���Z�b�g
        Data_MaeTypeGet(key)
    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(�O�H���R�[�h)</param>
    ''' <remarks></remarks>
    Private Sub Data_MaeTypeGet(ByVal key As String)
        Dim strWhere As String = String.Format("�O�H���R�[�h='{0}'", key)           ' WHERE��ɑ����̏�������
        Dim drow() As DataRow = m_DsMaeType.Tables(0).Select(strWhere)              ' �O�H���R�[�h����L�[�̒l�̍s�����o��
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)        ' �O�H���R�[�h����L�[�̒l�̍s�����o��
        Dim nRowCount As Integer                                                    ' �폜�Ώۂ̃f�[�^��

        nRowCount = drow.Length
        '���݉�ʂɕ\�����Ă���O�H���R�[�h�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
        For ii As Integer = 1 To nRowCount
            m_DsMaeType.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii

        ' �O�H���r��dgv�̒l��O�H���r���ڑ��e�[�u���֊i�[
        For ii As Integer = 0 To dgvMaeType.Rows.Count - 1
            'ID ���󔒈ȊO�̍s�����擾
            If CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value) = "" Then Continue For

            Dim drowNew As DataRow = m_DsMaeType.Tables(0).NewRow()  ' �V�K�s

            drowNew.Item("�O�H���R�[�h") = key                                                                 ' �O�H���R�[�h
            drowNew.Item("FFName") = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_FFName, ii).Value)                ' FFName
            drowNew.Item("ID") = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_ID, ii).Value)                        ' ID
            drowNew.Item("Name") = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)                    ' �r����
            drowNew.Item("�D��x") = CInt(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_Yusen, ii).Value)                 ' �D��x
            drowNew.Item("�}�b�v�\������") = CStr(dgvMaeType(EM_DATACOLUMNTYPE.emTxt_MapDispName, ii).Value)   ' �}�b�v�\������
            ' �\���L��
            If dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value.ToString = "-" Then
                drowNew.Item("�\���L��") = 0
            Else
                drowNew.Item("�\���L��") = 0
                For jj As Integer = 0 To m_sMaeGrade.Length - 1
                    If dgvMaeType(EM_DATACOLUMNTYPE.emCmb_HyoziUmu, ii).Value.ToString = m_sMaeGrade(jj) Then
                        For ll As Integer = 0 To drowGrade.Length - 1
                            If m_nMaeGradeID(jj) = CInt(drowGrade(ll).Item("�O���[�hID")) Then
                                drowNew.Item("�\���L��") = drowGrade(ll).Item("�O���[�hID")
                                Exit For
                            End If
                        Next ll
                    End If
                Next jj
            End If

            ' �e�[�u���ɒǉ�
            m_DsMaeType.Tables(0).Rows.InsertAt(drowNew, m_DsMaeType.Tables(0).Rows.Count)
        Next ii

    End Sub

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(�O�H���R�[�h)</param>
    ''' <remarks></remarks>
    Private Sub Data_MaeGradeGet(ByVal key As String)
        Dim strWhere As String = String.Format("�O�H���R�[�h='{0}'", key)       ' WHERE��ɑ����̏�������
        Dim drow() As DataRow                                       ' �f�[�^�s
        Dim nRowCount As Integer                                    ' �폜�Ώۂ̃f�[�^��
        drow = m_DsMaeGrade.Tables(0).Select(strWhere)              ' �O�H���R�[�h����L�[�̒l�̍s�����o��

        '���݉�ʂɕ\�����Ă���O�H���R�[�h�̔�ڑ��e�[�u���̃f�[�^���폜(��납��폜)
        nRowCount = drow.Length
        For ii As Integer = 1 To nRowCount
            m_DsMaeGrade.Tables(0).Rows.Remove(drow(nRowCount - ii))
        Next ii

        ' �O���[�hdgv�̒l��O�H���O���[�h��ڑ��e�[�u���֊i�[
        For ii As Integer = 0 To dgvMaeGrade.RowCount - 1

            '�O���[�hID ���󔒈ȊO�̍s�����擾
            If CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value) = "" Then Continue For

            Dim drowNew As DataRow = m_DsMaeGrade.Tables(0).NewRow()  ' �V�K�s

            drowNew.Item("�O�H���R�[�h") = key                                                                  ' �O�H���R�[�h
            drowNew.Item("FFName") = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_FFName, ii).Value)               ' FFName
            drowNew.Item("�O���[�hID") = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_GradeID, ii).Value)          ' �O���[�hID
            drowNew.Item("Name") = CStr(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)                   ' �O���[�h����
            drowNew.Item("�D��x") = CInt(dgvMaeGrade(EM_DATACOLUMNGRADE.emTxt_Yusen, ii).Value)                ' �D��x

            '�}�b�v�\���F �{�^���̔w�i�F��ARGB�`���ɂ��Ċi�[
            drowNew.Item("�}�b�v�\���F") = _
                dgvMaeGrade(EM_DATACOLUMNGRADE.emMapButton, ii).Style.BackColor.ToArgb()

            ' �e�[�u���ɒǉ�
            m_DsMaeGrade.Tables(0).Rows.InsertAt(drowNew, m_DsMaeGrade.Tables(0).Rows.Count)
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

            ' �f���[�g�C���T�[�g����̂őO�H���r��ݒ�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_MAE_MASTER
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�O�H���r��ݒ�e�[�u���փf�[�^��o�^ 
            '�f�[�^�e�[�u���͕����e�[�u�����Q�Ƃ��Ă���ׁA�A�b�v�f�[�g�ł��Ȃ��̂ŁA�C���T�[�g����
            For ii As Integer = 0 To m_DsMaeMaster.Tables(0).Rows.Count - 1
                With m_DsMaeMaster.Tables(0).Rows(ii)

                    If .Item("�O�H���R�[�h").ToString <> "" Then
                        'MAE_MASTER�o�^SQL���쐬()
                        strSql = ""
                        strSql &= "INSERT INTO " & DB_MAE_MASTER & " VALUES ('"
                        strSql &= .Item("�O�H���R�[�h").ToString & "',"         ' �O�H���R�[�h
                        strSql &= .Item("�L������").ToString & ","              ' �L������
                        strSql &= "'" & .Item("����").ToString & "',"           ' ����
                        strSql &= .Item("�O�H���\�����[�h").ToString & ","      ' �O�H���\�����[�h
                        strSql &= .Item("�摜�`��").ToString & ","              ' �摜�`��
                        strSql &= "'" & .Item("�r�摜����1").ToString & "',"    ' �r�摜����1
                        strSql &= "'" & .Item("�r�摜����2").ToString & "',"    ' �r�摜����2
                        strSql &= "'" & .Item("�r�摜����3").ToString & "',"    ' �r�摜����3
                        strSql &= "'" & .Item("�r�摜����4").ToString & "',"    ' �r�摜����4
                        strSql &= "'" & .Item("�����ޔ���").ToString & "')"     ' �����ޔ���
                        'SQL���s
                        If 0 > tcls_DB.DMLExecute(strSql) Then
                            ' �G���[�Ȃ烍�[���o�b�N���ďI��
                            tcls_DB.TransactRollback()
                            Return False
                        End If
                    End If
                End With
            Next ii

            ' �f���[�g�C���T�[�g����̂őO�H���r��ݒ�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_MAE_TYPE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�O�H���r��ݒ�e�[�u���փf�[�^��o�^ 
            '�f�[�^�e�[�u���͕����e�[�u�����Q�Ƃ��Ă���ׁA�A�b�v�f�[�g�ł��Ȃ��̂ŁA�C���T�[�g����
            For ii As Integer = 0 To m_DsMaeType.Tables(0).Rows.Count - 1
                With m_DsMaeType.Tables(0).Rows(ii)
                    'MAE_TYPE�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_MAE_TYPE_NAME & " VALUES ('"
                    strSql &= .Item("�O�H���R�[�h").ToString & "',"             ' �O�H���R�[�h
                    strSql &= "'" & .Item("FFName").ToString & "',"             ' FFName
                    strSql &= .Item("ID").ToString & ","                        ' ID
                    strSql &= "'" & .Item("Name").ToString & "',"               ' �r����
                    strSql &= .Item("�D��x").ToString & ","                    ' �D��x
                    strSql &= "'" & .Item("�}�b�v�\������").ToString & "',"     ' �}�b�v�\������
                    strSql &= .Item("�\���L��").ToString & ")"                  ' �\���L��
                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            ' �f���[�g�C���T�[�g����̂őO�H���O���[�h�ݒ�e�[�u���̑S���ڂ��폜
            strSql = "DELETE FROM " & DB_MAE_GRADE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' �G���[�Ȃ烍�[���o�b�N���ďI��
                tcls_DB.TransactRollback()
                Return False
            End If

            '�O�H���O���[�h�ݒ�e�[�u���փf�[�^��o�^ 
            '�f�[�^�e�[�u���͕����e�[�u�����Q�Ƃ��Ă���ׁA�A�b�v�f�[�g�ł��Ȃ��̂ŁA�C���T�[�g����
            For ii As Integer = 0 To m_DsMaeGrade.Tables(0).Rows.Count - 1
                With m_DsMaeGrade.Tables(0).Rows(ii)
                    'MAE_GRADE�o�^SQL���쐬
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_MAE_GRADE_NAME & " VALUES ('"
                    strSql &= .Item("�O�H���R�[�h").ToString & "',"             ' �O�H���R�[�h
                    strSql &= "'" & .Item("FFName").ToString & "',"             ' FFName
                    strSql &= .Item("�O���[�hID").ToString & ","                ' �O���[�hID
                    strSql &= "'" & .Item("Name").ToString & "',"               ' �O���[�h����
                    strSql &= .Item("�D��x").ToString & ","                    ' �D��x
                    strSql &= .Item("�}�b�v�\���F").ToString & ")"              ' �}�b�v�\���F
                    'SQL���s
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' �G���[�Ȃ烍�[���o�b�N���ďI��
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                End With
            Next ii

            '�R�~�b�g
            tcls_DB.TransactCommint()

            ' �����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_MAE_MAST)
            modMain.HardCopy(Me)
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
    ''' �O�H���}�X�^�[dgv�̗��`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeDefMaster(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' ��{��ݒ�
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                       ' �I��F����
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .ColumnHeadersHeight = 42                                   ' �w�b�_�[�̍���
            .AllowUserToResizeColumns = True                            ' ���[�U�[ ��T�C�Y�ύX
            .ScrollBars = ScrollBars.Both                               ' �X�N���[���o�[�̕\��

            ' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' �I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' ��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With

        '' ���`
        ' No
        dgvColSetText(dgv, "No", 42, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �O�H���R�[�h
        dgvColSetText(dgv, "�O�H��" & vbNewLine & "�R�[�h", 70, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �L������
        dgvColSetCheck(dgv, "�L������" & vbNewLine & "(�����F�L��)", 110, False, False, _
          DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �O�H������
        dgvColSetText(dgv, "�O�H������", 95, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �O�H���\�����[�h
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�O�H��" & vbNewLine & "�\�����[�h"
            clm.Width = 103
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("�ΏۊO", "�Ӄ֏��", "FF���")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' �摜�`��
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�摜�`��"
            clm.Width = 80
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("JPEG", "BMP")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' �r�摜����1
        dgvColSetText(dgv, "�r�摜����1" & vbNewLine & "(���p32����)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �r�摜����2
        dgvColSetText(dgv, "�r�摜����2" & vbNewLine & "(���p32����)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �r�摜����3
        dgvColSetText(dgv, "�r�摜����3" & vbNewLine & "(���p32����)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �r�摜����4
        dgvColSetText(dgv, "�r�摜����4" & vbNewLine & "(���p32����)", 130, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �����ޔ���
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "�����ޔ���"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemTasho As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemTasho.Items.AddRange("������", "������")
            listItemTasho.MaxDropDownItems = listItemTasho.Items.Count
            dgv.Columns.Add(clm)
        End Using
    End Sub

    ''' <summary>
    ''' �O�H���r��dgv�̗��`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeDefType(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' �I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' ��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With

        '' ���`
        ' No
        dgvColSetText(dgv, "No", 42, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' FFName
        dgvColSetText(dgv, "FF" & vbNewLine & "Name", 50, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' ID
        dgvColSetText(dgv, "ID" & vbNewLine & "1�ȏ�", 54, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' Name
        dgvColSetText(dgv, "�r����", 97, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �D��x
        dgvColSetText(dgv, "�D��x" & vbNewLine & "���P", 62, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.TopCenter, DataGridViewContentAlignment.TopCenter, "")

        ' �}�b�v�\������
        dgvColSetText(dgv, "�}�b�v" & vbNewLine & "�\������", 78, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �\���L��
        dgvColSetText(dgv, "�\��" & vbNewLine & "�L��", 85, False, False, _
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

    ''' <summary>
    ''' �O�H���O���[�hdgv�̗��`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitializeDefGrade(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
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

            '' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      ' �I��F
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     ' ��s�����ɑI��F�ύX
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
        End With

        '' ���`
        ' No
        dgvColSetText(dgv, "No", 42, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' FFName
        dgvColSetText(dgv, "FF" & vbNewLine & "Name", 50, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �O���[�hID
        dgvColSetText(dgv, "��ڰ��ID" & vbNewLine & "1�ȏ�", 78, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �O���[�h����
        dgvColSetText(dgv, "��ڰ�ޖ���" & vbNewLine & "(���p32����)", 110, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �D��x
        dgvColSetText(dgv, "�D��x" & vbNewLine & "���P", 62, False, False, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' �}�b�v�\���F
        dgvColSetButton(dgv, "�T�����" & vbNewLine & "�\���F", 86, False, False, _
            DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
            New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "�ύX")

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
        Dim rec As Integer = CInt(IIf(bln, cmbMaeProcessCode.SelectedIndex + 1, cmbMaeProcessCode.SelectedIndex - 1))
        ' �㉺���`�F�b�N
        If rec < 0 OrElse rec >= cmbMaeProcessCode.Items.Count Then Exit Sub
        ' �ؑ�
        cmbMaeProcessCode.SelectedIndex = rec
    End Sub

    ''' <summary>
    ''' �L��������̃`�F�b�N��Ԃ��ς�����Ƃ��ɁA�w�i�F��ύX����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMaeMaster_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeMaster.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        '�w�b�_�͏������Ȃ�
        If e.RowIndex = -1 Then Exit Sub

        ' �L��������
        If e.ColumnIndex = EM_DATACOLUMNMASTER.emChk_Check Then
            If CBool(dgvMaeMaster(e.ColumnIndex, e.RowIndex).Value) Then
                '�`�F�b�N��t������ �w�i�F�ƑI��F���K��F�ɂ���
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle.BackColor = g_ColorSetting
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = g_ColorSetting
            Else
                '�`�F�b�N���O������ �w�i�F�ƑI��F�����f�t�H���g�ɖ߂�
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
                dgvMaeMaster.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
            End If
        End If
    End Sub

    ''' <summary>
    ''' ����Z���̒l���ς������F��ύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMaeType_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeType.CellValueChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s�ȊO�̓X���[
        If e.RowIndex < 0 Then Exit Sub

        ' �\���L���o�͗�Ȃ�F�ύX
        If e.ColumnIndex = EM_DATACOLUMNTYPE.emCmb_HyoziUmu Then

            ' �Z���̒l���擾
            Dim strID As String = dgv(e.ColumnIndex, e.RowIndex).Value.ToString

            ' �Z���̒l�iID�j�ŐF���擾���A�w�i�F��ύX���� 
            dgv(e.ColumnIndex, e.RowIndex).Style.BackColor = GetCmbColor(strID)
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
        If cmbMaeProcessCode.Text = "" Or key = "-" Or key = "" Then Return Color.Empty
        key = key.Remove(key.Length - 2, 2)
        Dim strWhere As String = String.Format("�O�H���R�[�h='{0}' and name='{1}'", cmbMaeProcessCode.Text, key)   ' WHERE��ɑ����̏�������
        Dim drowGrade() As DataRow = m_DsMaeGrade.Tables(0).Select(strWhere)       ' 1�s����͂�

        Dim retColor As Color = Color.Empty   ' �߂�l �����l��NULL�F

        ' 1�s�����Ȃ��Ǝv������
        For ii As Integer = 0 To drowGrade.Length - 1
            ' �J���[���擾
            retColor = Color.FromArgb(CInt(drowGrade(0).Item("�}�b�v�\���F").ToString))
        Next ii

        Return retColor
    End Function

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
    ''' �r��dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvType_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeType.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNTYPE.emInsButton     '��֑}���{�^��
                InsBtnClick_Type(dgv, e.RowIndex)
            Case EM_DATACOLUMNTYPE.emDelButton     '�폜�{�^��
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' �O�H���O���[�hdgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGrade_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeGrade.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' �s���N���b�N����Ă��Ȃ���ΏI��
        If e.RowIndex < 0 Then Exit Sub

        ' ��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNGRADE.emMapButton, EM_DATACOLUMNGRADE.emMapButton  '�}�b�v�\���F
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' �O�H���r��dgv�̑}���{�^�������ꂽ�Ƃ��̏���
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

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            '�ŏI�s�̃e�L�X�g�{�b�N�X�ɉ�������̃f�[�^�������Ă���ꍇ�͑}���s��
            If Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_FFName, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_ID, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Yusen, dgv.Rows.Count - 1).Value)) <> "" Or _
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_MapDispName, dgv.Rows.Count - 1).Value)) <> "" Then
                frmMsg.Text = "�����m�F"
                frmMsg.ChangeMsg("�ŏI�s�Ƀf�[�^�����݂��܂��B", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Exit Sub
            End If

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

        ' �O�H���r��dgv�Ƀ��R�[�h��}��
        InsertAt_TypeData(nRowIndex)

        ' �O�H���r��dgv�̍ŏI�s���폜
        dgv.Rows.RemoveAt(dgvMaeType.Rows.Count - 1)

        'No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexDgvMaeType(dgv)

    End Sub

    ''' <summary>
    ''' �O�H���r��dgv�̍폜�{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
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

        ' �O�H���r��dgv�̍ŏI�s�Ƀ��R�[�h��}��
        InsertAt_TypeData(dgv.Rows.Count)

        ' �O�H���r��dgv�̃��R�[�h���폜
        dgv.Rows.RemoveAt(nRowIndex)

        ' No��U��Ȃ����{�^����\�����Ȃ���
        InitIndexDgvMaeType(dgv)

    End Sub

    ' ��Ɏg�p���邩������Ȃ��̂Ŏc���Ă���
    ''' <summary>
    ''' dgvMaeMaster�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">�O�H���}�X�^�[dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvMaeMaster(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNMASTER.emNo, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�
        Next ii
    End Sub

    ''' <summary>
    ''' dgvMaeType�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">�O�H���r��dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvMaeType(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNTYPE.emNo, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�
            ' �}���{�^���̎g�p�\��Ԃ�ݒ�    (�{�^�����g�p�s�ɂ͂ł��Ȃ��̂ŁA�F��ύX�j
            If ii = dgv.RowCount - 1 Then
                ' �ŏI�s�͑}���{�^�����g�p�s��
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Gray
            Else
                ' ����ȊO�͎g�p��
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' dgvMaeGrade�̃C���f�b�N�X��U��A�{�^���̕\���ݒ������
    ''' </summary>
    ''' <param name="dgv">�O�H���O���[�hdgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvMaeGrade(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNGRADE.emNo, ii).Value = ii + 1   ' ��̃C���f�b�N�X�ԍ�
        Next ii
    End Sub

    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvMaeMaster.SelectionChanged, dgvMaeType.SelectionChanged, dgvMaeGrade.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv
        dgv.CurrentCell.Selected = False
    End Sub


    '�Z�����I�����ꂽ�Ƃ��̏���
    ''' <summary>
    ''' dgv�̃Z�����I�����ꂽ�Ƃ��ɃR���{�{�b�N�X�Ȃ�h���b�v�_�E������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}�ł̃h���b�v�_�E�����삪�A�J��PC���Əo���Ȃ��A�����s��</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvMaeMaster.CellEnter, dgvMaeType.CellEnter, dgvMaeGrade.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' �Ώۂ�dgv

        ' ��̃^�C�v���R���{�{�b�N�X���H
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' �h���b�v�_�E���\��
            SendKeys.Send("%{DOWN}")
        End If
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