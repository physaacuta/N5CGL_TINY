'*********************************************************************************
' �����p�^�[�������ݒ��ʃN���X
'	[Ver]
'		Ver.01    2012/06/08  ����
'
'	[����]
'
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmPCodeJudge

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' �f�[�^�O���b�h�r���[�̗�
    Private Enum EM_DATACOLUMN
        emTxt_No = 0                                        ' �e�L�X�g�{�b�N�X�@����No
        emTxt_MeireiNo                                      ' �e�L�X�g�{�b�N�X  ����No
        emTxt_CoilNo                                        ' �e�L�X�g�{�b�N�X  ���YNo
        emTxt_HinSyu                                        ' �e�L�X�g�{�b�N�X  �i��
        emTxt_SyuZai                                        ' �e�L�X�g�{�b�N�X  ��ގ�
        emTxt_Kensa                                         ' �e�L�X�g�{�b�N�X�@�����
        emTxt_Youto                                         ' �e�L�X�g�{�b�N�X�@�p�r
        emTxt_Hyoumen                                       ' �e�L�X�g�{�b�N�X�@�\�ʏ���1����
        emTxt_TyuAtu_l                                      ' �e�L�X�g�{�b�N�X�@������_low
        emTxt_TyuAtu_h                                      ' �e�L�X�g�{�b�N�X�@������_high
        emTxt_Mukesaki                                      ' �e�L�X�g�{�b�N�X�@����
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_Adapter As SqlClient.SqlDataAdapter = Nothing         ' �A�_�v�^�[
    Private m_DS As New DataSet                                     ' ��ڑ��^�e�[�u��
    Private m_KeyCode As String = ""                                ' ��L�[

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
    Private Sub frmPCodeJudge_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
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
                Call HardCopy(IMG_PATH, "�����p�^�[�������ݒ���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
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
    Private Sub frmPCodeJudge_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
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
    ''' <remarks></remarks>
    Private Sub frmPCodeJudge_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                     ' ���j���[���X�g����
        modMain.SetLabelSysName(Me)                                 ' �V�X�e�������x������
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmPCodeJudge            ' �����p�^�[�������ݒ��ʌďo

        '�^�C�g���ݒ�
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        ' �A�b�v�R�[�h�̃R���{�{�b�N�X
        MyBase.SetComboBox_Pcode(cmbUpCodeItem)

        ' �R���{�{�b�N�X��"�f�t�H���g�l(MAX_PCODE)���폜����
        MyBase.Delete_CodeEtc(cmbUpCodeItem, CStr(MAX_PCODE))

        MyBase.SetCmbUpCodeItem(cmbUpCodeItem, dgvUpCodeItem)

        ' �����̐ݒ�
        dgvInitialize(dgvData)

        ' ���̃t�H�[���ŕK�v�ȃf�[�^���擾����
        If Not DB_Select() Then
            ' DB ���s�Ȃ̂ŁA��ʋ����I��
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "�ُ�"
            frmMsg.ChangeMsg("DB�ڑ����s�̈� ��ʂ������I�����܂�", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       ' �I��
        End If

        ' �R���{�{�b�N�X�Ɍ����p�^�[�����Z�b�g
        MyBase.SetComboBox_Pcode(cmbPCode)

        ' �R���{�{�b�N�X�̃f�t�H���g�l(MAX_PCODE)���폜����
        MyBase.Delete_CodeEtc(cmbPCode, CStr(MAX_PCODE))

        If 0 < cmbPCode.Items.Count Then
            ' �O�̉�ʂőI��������Ԃ��Z�b�g
            If cmbPCode.Items.Count - 1 < g_nPaternIndex Then
                ' �Y�����ڂ��Ȃ��ꍇ�́A�擪��\��
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

            modMain.TaskEnd()                       ' �I��
        End If

        ' ��ʂɃp�^�[���ʂ̒l��\��
        m_KeyCode = SelectComboBoxKey(cmbPCode.SelectedIndex, cmbPCode)
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
    Private Sub frmPCodeJudge_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        ' ���݂̕\�����̉�ʂ������̉�ʂȂ�A�v���P�[�V�������I��
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

        ' SQL������
        strSQL = "SELECT pcode"
        strSQL &= ",����No"
        strSQL &= ",�L������"
        strSQL &= ",����No"
        strSQL &= ",���YNo"
        strSQL &= ",����������"
        strSQL &= ",���������"
        strSQL &= ",�����"
        strSQL &= ",����"
        strSQL &= ",�p�r"
        strSQL &= ",��ގ�"
        strSQL &= ",�\�ʏ���1����"
        strSQL &= ",�i��"

        strSQL &= " FROM " & DB_PCODE_JUDGE
        strSQL &= " ORDER BY pcode ASC, ����No ASC"

        ' SQL�����s
        If 0 > MyBase.Get_SelectData(strSQL, m_DS, m_Adapter, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_JUDGE & " �Ǎ��G���[ [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' DB�̏�����ʂɕ\��
    ''' </summary>
    ''' <param name="key">��L�[�̒l(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Disp(ByVal key As String)
        ' �ꊇ�̃e�[�u���Ȃ̂ŁA�����p�^�[�����̒l�̂ݎ��o��
        Dim strWhere As String = String.Format("pcode='{0}'", key)          ' WHERE��ɑ����̏�������
        Dim drow() As DataRow = m_DS.Tables(0).Select(strWhere)

        ' ��ʂɕ\��
        dgvData.RowCount = drow.Length                  ' ��{�I�Ƀ}�X�^�[�ɂ� ����ȃ��R�[�h��o�^�ς݂̂͂��B�ėp�I�ɂ����
        For ii As Integer = 0 To drow.Length - 1
            dgvData(EM_DATACOLUMN.emTxt_No, ii).Value = drow(ii).Item("����No")

            dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value = drow(ii).Item("����No").ToString
            dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value = drow(ii).Item("���YNo").ToString
            dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value = drow(ii).Item("�i��").ToString
            dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value = drow(ii).Item("��ގ�").ToString
            dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value = drow(ii).Item("�����").ToString
            dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value = drow(ii).Item("�p�r").ToString
            dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value = drow(ii).Item("�\�ʏ���1����").ToString
            dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value = drow(ii).Item("����������").ToString
            dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value = drow(ii).Item("���������").ToString
            dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value = drow(ii).Item("����").ToString
        Next ii
    End Sub

    ''' <summary>
    ''' �����p�^�[���ύX��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbPCode_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbPCode.SelectedIndexChanged
        ' ���̑I���Ǝ�L�[�������ꍇ�͖��� (�I�����ē��ُ͈킾�����̂� ���ɖ߂����� �̎��̑΍�)
        If m_KeyCode = cmbPCode.Text Then Exit Sub

        '' ��ʂ̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
        ' ������ʕ\�����ɂ́A�f�[�^�����ׁ̈A��ʂ̃f�[�^����荞�܂Ȃ���
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
    ''' <param name="key">��L�[�̒l(pcode) ���̏��g��Ȃ����ȁ[</param>
    ''' <returns>���A���</returns>
    ''' <remarks></remarks>
    Private Function Data_Check(ByVal key As String) As Boolean
        Dim strWk As String
        Dim strMsg As String

        ' ����No�����[�v
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            ' ����No
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " ����No") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value = strWk

            ' �R�C��No
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " �R�C��No") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value = strWk

            ' �i��
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " �i��") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value = strWk

            ' ��ގ�
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " ��ގ�") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value = strWk

            ' �����
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " �����") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value = strWk

            ' �p�r
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " �p�r") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value = strWk

            ' �\�ʏ���1����
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " �\�ʏ���1����") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value = strWk

            ' ����������
            If System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value Then
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value)
                If "" <> strWk Then
                    If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "����No=" & ii + 1 & " �������l") <> 0 Then Return False
                    dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value = strWk
                End If
            End If

            ' ���������
            If System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value Then
                strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value)
                If "" <> strWk Then
                    If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "����No=" & ii + 1 & " ������l") <> 0 Then Return False
                    dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value = strWk
                End If
            End If

            ' �����������A����`�F�b�N
            If System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value And
                System.DBNull.Value IsNot dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value Then
                If "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value) And
                    "" <> CStr(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value) Then

                    If CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value) >
                     CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value) Then

                        strMsg = "�y ����No=" & ii + 1 & " �������l �z" & vbCrLf
                        strMsg &= "������l�𒴂��Ă��܂��B" & vbCrLf
                        strMsg &= "������l�ȉ��ɐݒ肵�Ă��������B"
                        Call MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                        Return False
                    End If
                End If
            End If

            ' ����
            strWk = CStr(dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value)
            If tClassLibrary.tClass.tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 32, "����No=" & ii + 1 & " ����") <> 0 Then Return False
            dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value = strWk

        Next ii
        Return True
    End Function

    ''' <summary>
    ''' ��ʂɕ\�����̃f�[�^���ڑ��^�e�[�u���ɃZ�b�g
    ''' </summary>
    ''' <param name="key">��L�[�̒l(pcode)</param>
    ''' <remarks></remarks>
    Private Sub Data_Get(ByVal key As String)
        Dim strWhere As String                          ' WHERE��ɑ����̏�������
        Dim drow As DataRow

        ' ����No�����[�v
        For ii As Integer = 0 To dgvData.Rows.Count - 1
            strWhere = String.Format("pcode='{0}' and ����No={1}", key, ii + 1)     ' WHERE��ɑ����̏�������
            drow = m_DS.Tables(0).Select(strWhere)(0)           ' ��L�[���w�肵�Ă̎��o���Ȃ̂ŁA�ꌏ�����Ȃ�

            '''' �����`�F�b�N��Z�b�g
            ' �������S���󔒂Ȃ疳���s�ƔF��
            Dim umu As Integer

            If (dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value.ToString.Trim = "") AndAlso
               (dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value Is System.DBNull.Value Or
                dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value.ToString.Trim = "") Then
                umu = 0                                                 ' ����
            Else
                umu = 1													' �L��
			End If

            ' �Z�b�g
            drow.Item("�L������") = umu
            drow.Item("����No") = dgvData(EM_DATACOLUMN.emTxt_MeireiNo, ii).Value
            drow.Item("���YNo") = dgvData(EM_DATACOLUMN.emTxt_CoilNo, ii).Value
            drow.Item("�i��") = dgvData(EM_DATACOLUMN.emTxt_HinSyu, ii).Value
            drow.Item("��ގ�") = dgvData(EM_DATACOLUMN.emTxt_SyuZai, ii).Value
            drow.Item("�����") = dgvData(EM_DATACOLUMN.emTxt_Kensa, ii).Value
            drow.Item("�p�r") = dgvData(EM_DATACOLUMN.emTxt_Youto, ii).Value
            drow.Item("�\�ʏ���1����") = dgvData(EM_DATACOLUMN.emTxt_Hyoumen, ii).Value

            If Nothing Is dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value OrElse
            "" = dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value.ToString.Trim Then
                drow.Item("����������") = System.DBNull.Value
            Else
                drow.Item("����������") = CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_l, ii).Value)
            End If

            If Nothing Is dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value OrElse
            "" = dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value.ToString.Trim Then
                drow.Item("���������") = System.DBNull.Value
            Else
                drow.Item("���������") = CInt(dgvData(EM_DATACOLUMN.emTxt_TyuAtu_h, ii).Value)
            End If

            drow.Item("����") = dgvData(EM_DATACOLUMN.emTxt_Mukesaki, ii).Value

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

            Using cbWk As New SqlClient.SqlCommandBuilder()

                ' �R�}���h�A�_�v�^���Z�b�g���āA�X�V
                cbWk.DataAdapter = m_Adapter
                m_Adapter.Update(m_DS)

            End Using

            '�R�~�b�g
            transact.Commit()

            ' �����@�\�փp�����[�^�ύX�ʒm
            modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
            modMain.HardCopy(Me)
            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "�ݒ�G���[[" & ex.Message & "] " & Me.Text)

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
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 42             ' �w�b�_�[�̍���

        End With

        '' ���`

        ' ����No
        dgvColSetText(dgv, "����NO", 60, True, True, _
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' ����No
        dgvColSetText(dgv, "����No" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �R�C��No
        dgvColSetText(dgv, "�R�C��No" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �i��
        dgvColSetText(dgv, "�i��" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' ��ގ�
        dgvColSetText(dgv, "��ގ�" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �����
        dgvColSetText(dgv, "�����" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �p�r
        dgvColSetText(dgv, "�p�r" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' �\�ʏ���1����
        dgvColSetText(dgv, "�\�ʏ���1����" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' ����������
        dgvColSetText(dgv, "�������l" & vbNewLine & "[��m]", 120, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' ���������
        dgvColSetText(dgv, "������l" & vbNewLine & "[��m]", 120, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' ����
        dgvColSetText(dgv, "����" & vbNewLine & "[���K�\������]", 140, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

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
        ' �ؑ�
        cmbPCode.SelectedIndex = rec
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

            '''' �R�s�[
            If CopyDB(strFromKey, strToKey) Then
                ' �����@�\�փp�����[�^�ύX�ʒm
                modMain.Send_Main(CH_PARAM_SETTIG_PCODE)
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
    ''' �R�s�[����
    ''' </summary>
    ''' <param name="strFromKey">�R�s�[���L�[ </param>
    ''' <param name="strToKey">�R�s�[��L�[ </param>
    ''' <returns>���A���(True:����,False:�ُ�)</returns>
    ''' <remarks></remarks>
    Private Function CopyDB(ByVal strFromKey As String, ByVal strToKey As String) As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        '''' SQL���̐���
        strSQL = "UPDATE " & DB_PCODE_JUDGE & " SET "
        ' �ϗ�
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�L������", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("����No", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("���YNo", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("����������", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("���������", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�����", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("����", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�p�r", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("��ގ�", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�\�ʏ���1����", "b")
        strSQL &= tClass_SQLServer.sqlColumnUpdate("�i��", "b")

        tClass_SQLServer.sqlColumnTrim(strSQL)      ' ���܂��Ȃ� �K�������Ă�

        ' From��
        strSQL &= "FROM " & DB_PCODE_JUDGE & " a "
        strSQL &= ", (SELECT * FROM " & DB_PCODE_JUDGE
        strSQL &= " WHERE pcode='" & strFromKey & "') b "

        ' WHERE��
        strSQL &= "WHERE a.����No=b.����No AND a.pcode IN(" & strToKey & ")"

        ' SQL�����s
        If 0 > MyBase.UpCode_UpDate(strSQL, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_PCODE_JUDGE & " �X�V�G���[ [" & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

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
